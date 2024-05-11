#include "RoleManagerPage.h"
#include "ui_RoleManagerPage.h"
#include "SApp.h"
#include "SHttpClient.h"
#include "SSwitchDelegate.h"
#include "SFieldTranslate.h"
#include "SSWitchButton.h"
#include "SButtonDelegate.h"
#include "xlsxdocument.h"	//文档
#include "xlsxworkbook.h"	//工作簿
#include "xlsxworksheet.h"	//工作表
#include<QFileDialog>
#include<QTextStream>
#include<QBrush>
#include<QHeaderView>
#include<QMessageBox>
#include<QMenu>
const int PAGESIZE = 20;//无需修改，最好的状态
RoleManagerPage::RoleManagerPage(const QString& user_id, const QString& username,QWidget* parent)
	:m_user_id(user_id)
	,m_username(username)
	,QWidget(parent) 
	,ui(new Ui::RoleManagerPage)
	,m_model(new QStandardItemModel(this))
	
	,m_curPage(1)
{
	ui->setupUi(this);
	initPrivilege();
	init();
}

RoleManagerPage::~RoleManagerPage()
{
	delete ui;
}

void RoleManagerPage::initPrivilege()
{
	//qDebug() << "开始获取权限";
	QVariantMap params;
	//params.insert("idDeleted", 0);//能够登录成功就说明当前用户并没有被删除
	params.insert("query", m_user_id);
	SHttpClient(URL("/api/user_privilege/list"))//异步，导致权限无法得以初始化成功
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.params(params)
		.success([=](const QByteArray& data)
			{
				QJsonParseError error;
				auto jdoc = QJsonDocument::fromJson(data, &error);
				if (error.error != QJsonParseError::NoError) {
					return;
				}
				auto jobj = jdoc.object().value("data").toObject();
				auto arr = jobj.value("lists").toArray();
				auto juser = arr.at(0).toObject();
				m_privilege_edit = juser.value("privilege_edit").toInt();
				m_privilege_add = juser.value("privilege_add").toInt();
				m_privilege_delete = juser.value("privilege_delete").toInt();
				//qDebug() << "权限获取成功： " << m_privilege_edit << " " << m_privilege_add << " " << m_privilege_delete;
			})
		.get();
}

void RoleManagerPage::init()
{
	QFile file(":/ResourceClient/style.css");
	if (file.open(QIODevice::ReadOnly)) {
		m_styleStr = file.readAll();
	}
	setStyleSheet(m_styleStr);
	
	//翻译映射
	SFieldTranslate::instance()->addTrans("user_privilege/user_id", "用户ID");
	SFieldTranslate::instance()->addTrans("user_privilege/username", "用户名");
	SFieldTranslate::instance()->addTrans("user_privilege/privilege_read", "读取权限");
	SFieldTranslate::instance()->addTrans("user_privilege/privilege_edit", "编辑权限");
	SFieldTranslate::instance()->addTrans("user_privilege/privilege_add", "添加权限");
	SFieldTranslate::instance()->addTrans("user_privilege/privilege_delete", "删除权限");
	initMenu();
	//表格设置
	ui->tableView->setModel(m_model);
	//ui->tableView->setSelectionBehavior(QTableView::SelectRows);
	ui->tableView->setSelectionMode(QTableView::NoSelection);
	ui->tableView->setFocusPolicy(Qt::FocusPolicy::NoFocus);
	ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
	ui->tableView->setMouseTracking(true);
	

	ui->firstPage->setIcon(QIcon(":/ResourceClient/double-left.png"));
	ui->prePage->setIcon(QIcon(":/ResourceClient/pre.png"));
	ui->nextPage->setIcon(QIcon(":/ResourceClient/next.png"));
	ui->lastPage->setIcon(QIcon(":/ResourceClient/double-right.png"));
	ui->changeFace2->setIcon(QIcon(":/ResourceClient/changeFace.png"));
	ui->turnToBox->setMinimum(1);

	ui->searchBtn->setIcon(QIcon(":/ResourceClient/sousuo.png"));
	ui->exportBtn->setIcon(QIcon(":/ResourceClient/daochu.png"));
	ui->turnTo->setIcon(QIcon(":/ResourceClient/tiaozhuan.png"));

	//导出按钮（CSV/XLSX）
	connect(ui->exportBtn, &QPushButton::clicked, this, &RoleManagerPage::onExport);
	//搜索
	//按下回车
	connect(ui->searchEdit, &QLineEdit::returnPressed, this, &RoleManagerPage::onSearch);

	connect(ui->searchBtn, &QPushButton::clicked, this, &RoleManagerPage::onSearch);
	//界面创建的时候，查询一下
	onSearch();
	//如果只有一条数据，那么他就是管理员，拥有所有权限
	if (m_total == 1) {
		m_privilege_edit = 1;
		m_privilege_add = 1;
		m_privilege_delete = 1;
	}
	//连接页面跳转
	connect(ui->firstPage, &QPushButton::clicked, this, [this] {setCurrentPage(m_curPage = 1); });
	connect(ui->prePage, &QPushButton::clicked, this, [this] {setCurrentPage(m_curPage = m_curPage > 1 ? m_curPage - 1 : m_curPage); });
	connect(ui->nextPage, &QPushButton::clicked, this, [this] {setCurrentPage(m_curPage = m_curPage < m_totalPage ? m_curPage + 1 : m_curPage); });
	connect(ui->lastPage, &QPushButton::clicked, this, [=] {setCurrentPage(m_curPage = m_totalPage); });
	connect(ui->turnTo, &QPushButton::clicked, this, [=] {setCurrentPage(m_curPage = ui->turnToBox->text().toInt()); });

	connect(ui->turnToBox, &QSpinBox::valueChanged, [=](int value)
		{
			//qDebug() << "值发生变化 , value = " << value << "m_totalPage = "<<m_totalPage;
			if (value > m_totalPage) {
				ui->turnToBox->setValue(m_totalPage);
			}
		});

	//设置表格视图得到头视图
	auto hHeaderView = new QHeaderView(Qt::Horizontal);
	ui->tableView->setHorizontalHeader(hHeaderView);
	ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	//------------------------------------------------------item 代理
	//SSwitchDelegate
	auto edit_SwitchDelegate = new SSwitchDelegate(ui->tableView);
	ui->tableView->setItemDelegateForColumn(column("privilege_edit"), edit_SwitchDelegate);
	connect(edit_SwitchDelegate, &SSwitchDelegate::stateChanged, [=](bool state, const QModelIndex& index)
		{
			auto user_id = m_model->item(index.row(), column("user_id"))->text();
			auto st = m_model->data(index, Qt::UserRole).toInt();
			//qDebug() << "当前权限为 ： " << m_privilege_edit << "  " << m_privilege_add << "  " << m_privilege_delete;
			if (m_user_id == user_id) {
				drawMessageBox("不可修改自己的权限");
				onSearch();
				return;
			}
			//qDebug() << "当前状态 : " << state;
			//只有当前用户拥有所有权限时才可修改他人权限（root）
			if (!m_privilege_edit || !m_privilege_add || !m_privilege_delete) {
				drawMessageBox("无修改权限");
				//难点 ： 如何让SwitchButton的状态不再改变
				onSearch();//取巧
				return;
			}
			
			SHttpClient(URL("/api/user_privilege/alter?user_id=" + user_id)).debug(true)
				.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
				.json({{ "privilege_edit",st}})
				.fail([](const QString& msg, int code)
					{
						//qDebug() << msg << code;
					})
				.success([=](const QByteArray& data)
					{
						QJsonParseError error;
						auto jdoc = QJsonDocument::fromJson(data, &error);
						if (jdoc["code"].toInt() < 1000) {
							//qDebug() << "修改成功";
						}
					}).post();
		});
	
	auto add_SwitchDelegate = new SSwitchDelegate(ui->tableView);
	ui->tableView->setItemDelegateForColumn(column("privilege_add"), add_SwitchDelegate);
	connect(add_SwitchDelegate, &SSwitchDelegate::stateChanged, [=](bool state, const QModelIndex& index)
		{
			auto user_id = m_model->item(index.row(), column("user_id"))->text();
			auto st = m_model->data(index, Qt::UserRole).toInt();
			//qDebug() << "当前权限为 ： " << m_privilege_edit << "  " << m_privilege_add << "  " << m_privilege_delete;
			if (m_user_id == user_id) {
				drawMessageBox("不可修改自己的权限");
				onSearch();
				return;
			}
			if (!m_privilege_edit || !m_privilege_add || !m_privilege_delete) {
				drawMessageBox("无修改权限");
				//难点 ： 如何让SwitchButton的状态不再改变
				onSearch();//侥幸过关
				return;
			}
			
			SHttpClient(URL("/api/user_privilege/alter?user_id=" + user_id)).debug(true)
				.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
				.json({ { "privilege_add",st} })
				.fail([](const QString& msg, int code)
					{
						//qDebug() << msg << code;
					})
				.success([=](const QByteArray& data)
					{
						QJsonParseError error;
						auto jdoc = QJsonDocument::fromJson(data, &error);
						if (jdoc["code"].toInt() < 1000) {
							//qDebug() << "修改成功";
						}
					}).post();
		});
	
	auto delete_SwitchDelegate = new SSwitchDelegate(ui->tableView);
	ui->tableView->setItemDelegateForColumn(column("privilege_delete"), delete_SwitchDelegate);
	connect(delete_SwitchDelegate, &SSwitchDelegate::stateChanged, [=](bool state, const QModelIndex& index)
		{
			auto user_id = m_model->item(index.row(), column("user_id"))->text();
			auto st = m_model->data(index, Qt::UserRole).toInt();
			//qDebug() << "当前权限为 ： " << m_privilege_edit << "  " << m_privilege_add << "  " << m_privilege_delete;
			if (m_user_id == user_id) {
				drawMessageBox("不可修改自己的权限");
				onSearch();
				return;
			}
			if (!m_privilege_edit || !m_privilege_add || !m_privilege_delete) {
				drawMessageBox("无修改权限");
				//难点 ： 如何让SwitchButton的状态不再改变
				onSearch();//侥幸过关
				return;
			}
			
			SHttpClient(URL("/api/user_privilege/alter?user_id=" + user_id)).debug(true)
				.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
				.json({ { "privilege_delete",st} })
				.fail([](const QString& msg, int code)
					{
						//qDebug() << msg << code;
					})
				.success([=](const QByteArray& data)
					{
						QJsonParseError error;
						auto jdoc = QJsonDocument::fromJson(data, &error);
						if (jdoc["code"].toInt() < 1000) {
							//qDebug() << "修改成功";
						}
					}).post();
		});
	
}

void RoleManagerPage::getNewInfor(const QString& user_id, const QString& username)
{
	m_user_id = user_id;
	m_username = username;
	//qDebug()<<"重新获取权限 ： "<< m_privilege_edit << "  " << m_privilege_add << "  " << m_privilege_delete;
	initPrivilege();
}

QJsonObject RoleManagerPage::user(int row) const
{	
	QJsonObject juser;
	juser.insert("user_id", m_model->item( row, column("user_id"))->text());
	juser.insert("username", m_model->item( row, column("username"))->text());
	juser.insert("privilege_read", m_model->item( row, column("privilege_read"))->text());
	juser.insert("privilege_edit", m_model->item(row, column("privilege_edit"))->text());
	juser.insert("privilege_add", m_model->item( row, column("privilege_add"))->text());
	juser.insert("privilege_delete", m_model->item( row, column("privilege_delete"))->text());
	return juser;
}

//void RoleManagerPage::getAccount(const QString& user_id, const QString& username)
//{
//	m_user_id = user_id;
//	m_username = username;
//}

void RoleManagerPage::onSearch()
{
	QVariantMap params;
	params.insert("idDeleted", 0);
	auto filter = ui->searchEdit->text();
	if (!filter.isEmpty()) {
		params.insert("query", filter);
	}
	params.insert("curPage", m_curPage);
	SHttpClient(URL("/api/user_privilege/list"))
		.header("Authorization","Bearer"+ sApp->userData("user/token").toString())
		.params(params)
		.success([=](const QByteArray& data)
			{
				QJsonParseError error;
				auto jdoc = QJsonDocument::fromJson(data, &error);
				if (error.error != QJsonParseError::NoError) {
					return;
				}
				parseJson(jdoc.object());
			})
		.get();
	
}

void RoleManagerPage::setCurrentPage(int page)//获取第page页
{
	ui->turnToBox->setValue(m_curPage);
	setPageLabel(m_curPage, m_totalPage, m_total);
	onSearch();
}

void RoleManagerPage::setPageLabel(int curPage, int totalPage, int total)
{
	QString labStr = QString("<font color=red>第 %1 页</font> <font color=#FF33FF>每页 %2 条</font> <font color=#33CCFF>共 %3 页</font> <font color=#FF9933>共 %4 条</font>").arg(m_curPage).arg(PAGESIZE).arg(m_totalPage).arg(m_total);
	ui->pageLabel->setText(labStr);
}

void RoleManagerPage::parseJson(const QJsonObject& obj)
{
	//先清空再写入
	m_model->clear();
	for (size_t i = 0; i < m_fieldName.size(); i++)
	{
		//写表头
		auto item = new QStandardItem(SFieldTranslate::instance()->trans("user_privilege", m_fieldName[i]));
		m_model->setHorizontalHeaderItem(i, item);
	}
	auto jobj = obj.value("data").toObject();
	auto arr = jobj.value("lists").toArray();
	for (size_t i = 0; i < arr.size(); i++)
	{
		auto juser = arr.at(i).toObject();
		//将每行数据放进去
		m_model->appendRow(itemsFromJsonObject(juser));
	}
	ui->tableView->setColumnWidth(0, 20);
	
	//修改 pageLable 内容
	m_curPage = jobj.value("curPage").toInt();
	m_totalPage = jobj.value("totalPage").toInt();
	m_total = jobj.value("total").toInt();
	setPageLabel(m_curPage, m_totalPage, m_total);
	ui->turnToBox->setValue(m_curPage);//此处不能调用setCurpage否则会造成循环
	//qDebug() << "数据解析成功";
	m_checkSet.clear();
}

void RoleManagerPage::drawMessageBox(const QString& text)
{
	QMessageBox box(this);
	box.setWindowTitle("No Privilege");
	box.setText(text);
	if(text=="导出成功")
		box.setIconPixmap(QPixmap(":/ResourceClient/exportSuccess.png").scaled(70, 70));
	else box.setIconPixmap(QPixmap(":/ResourceClient/wuquan.png").scaled(70, 70));
	//box.setStyleSheet(m_styleStr);
	box.exec();
}

void RoleManagerPage::setSearchLine(const QString& text)
{
	ui->searchEdit->setText(text);
}

void RoleManagerPage::initMenu()
{

	m_faceMenu = new QMenu(this);
	ui->changeFace2->setMenu(m_faceMenu);

	m_faceMenu->addAction(QIcon(":/ResourceClient/pink.png"), "粉色背景", this, [=]
		{
			QFile file(":/ResourceClient/pink.css");
			if (file.open(QIODevice::ReadOnly)) {
				m_styleStr = file.readAll();
			}
			setStyleSheet(m_styleStr);
			QPalette pal(this->palette());
			pal.setColor(QPalette::Window, QColor(231, 166, 190));
			this->setAutoFillBackground(true);
			this->setPalette(pal);
			this->show();
		});
	m_faceMenu->addAction(QIcon(":/ResourceClient/black.png"), "黑色背景", this, [=]
		{
			QFile file(":/ResourceClient/black.css");
			if (file.open(QIODevice::ReadOnly)) {
				m_styleStr = file.readAll();
			}
			setStyleSheet(m_styleStr);
			QPalette pal(this->palette());
			pal.setColor(QPalette::Window, QColor(39, 42, 48));
			this->setAutoFillBackground(true);
			this->setPalette(pal);
			this->show();
		});
	m_faceMenu->addAction(QIcon(":/ResourceClient/white.png"), "白色背景", this, [=]
		{
			QFile file(":/ResourceClient/white.css");
			if (file.open(QIODevice::ReadOnly)) {
				m_styleStr = file.readAll();
			}
			setStyleSheet(m_styleStr);
			QPalette pal(this->palette());
			pal.setColor(QPalette::Window, Qt::white);
			this->setAutoFillBackground(true);
			this->setPalette(pal);
			this->show();
		});
	m_faceMenu->addAction(QIcon(":/ResourceClient/backFace.png"), "还原背景", this, [=]
		{
			QFile file(":/ResourceClient/style.css");
			if (file.open(QIODevice::ReadOnly)) {
				m_styleStr = file.readAll();
			}
			//qDebug() << "样式表如下 ： " << m_styleStr;
			setStyleSheet(m_styleStr);
		});

}

int RoleManagerPage::column(const QString& field)const
{
	auto pos = m_fieldName.indexOf(field);
	if (pos == -1) {
		qWarning() << "field " << field << "not exists";
	}
	return pos;
}

//根据拿到的一行数据进行解析插入
QList<QStandardItem*> RoleManagerPage::itemsFromJsonObject(const QJsonObject& jobj)
{
	QList<QStandardItem*>items;
	auto privilege_read = jobj.value("privilege_read").toInt();
	auto privilege_edit = jobj.value("privilege_edit").toInt();
	auto privilege_add = jobj.value("privilege_add").toInt();
	auto privilege_delete = jobj.value("privilege_delete").toInt();
	//qDebug() << "读权限 : " << privilege_read << " 编辑权限 : " << privilege_edit << " 添加用户权限 ： " << privilege_add << " 删除用户权限 : " << privilege_delete;
	for (const auto& field : m_fieldName) {
		auto item = new QStandardItem;
		item->setTextAlignment(Qt::AlignCenter);

		if (field == "user_id" || field == "username") {
			item->setText(jobj.value(field).toVariant().toString());
		}
		else {
			if(field == "privilege_read")item->setText("启用");
			else if (field == "privilege_edit") {
				item->setText(privilege_edit ? "启用" : "禁用");
				item->setData(privilege_edit, Qt::UserRole);
			}
			else if (field == "privilege_add") {
				item->setText(privilege_add ? "启用" : "禁用");
				item->setData(privilege_add, Qt::UserRole);
			}
			else if (field == "privilege_delete") {
				item->setText(privilege_delete ? "启用" : "禁用");
				item->setData(privilege_delete, Qt::UserRole);
			}
		}
		items.append(item);
	}
	return items;
}

void RoleManagerPage::onExport()
{
	auto filename = QFileDialog::getSaveFileName(this, "选择目录", "./undefined.csv", "csv (*.csv);;xlsx (*.xlsx);;all (*.*)");
	if (filename.isEmpty()) {
		return;
	}
	auto extra = QFileInfo(filename).suffix();
	if (extra.compare("csv", Qt::CaseInsensitive) == 0) {
		writeCSVFile(filename);
	}
	else if (extra.compare("xlsx", Qt::CaseInsensitive) == 0) {
		writeXLSXFile(filename);
	}
}

void RoleManagerPage::writeCSVFile(const QString& filename)
{
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly)) {
		qWarning() << filename << " open failed : " << file.errorString();
		return;
	}
	QTextStream stream(&file);
	//写表头
	for (int i = 0; i < m_model->columnCount(); i++)
	{
		stream<<m_model->horizontalHeaderItem(i)->text();
		if (i < m_model->columnCount() - 1)stream << ",";
	}
	stream << "\n";

	//写数据	
	for (int i = 0; i < m_model->rowCount() ; i++)
	{
		for (int j = 0; j < m_model->columnCount() ; j++)
		{
			auto item = m_model->item(i, j);
			if (item)stream << item->text();
			if (j < m_model->columnCount() - 1)stream << ",";
		}
		stream << "\n";
	}
	drawMessageBox("导出成功");
}

void RoleManagerPage::writeXLSXFile(const QString& filename)
{
	using namespace QXlsx;
	Document doc(filename);

	auto sheet = doc.currentWorksheet();
	//只能从第一列开始导（缺陷）
	//写表头
	for (size_t i = 1; i < m_model->columnCount(); i++)
	{
		sheet->write(1, i, m_model->horizontalHeaderItem(i)->text());
	}
	//写数据
	for (size_t i = 1; i < m_model->rowCount(); i++)
	{
		for (size_t j = 1; j < m_model->columnCount(); j++)
		{
			auto item = m_model->item(i, j);
			if (item) {
				sheet->write(i+1, j, item->text());
			}
		}
	}
	//保存文档
	doc.save();
	drawMessageBox("导出成功");
}
