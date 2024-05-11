#include "UserManagerPage.h"
#include "ui_UserManagerPage.h"
#include "SApp.h"
#include "SHttpClient.h"
#include "SHeaderView.h"
#include "SCheckDelegate.h"
#include "SUrlDelegate.h"
#include "SSwitchDelegate.h"
#include "SFieldTranslate.h"
#include "SSWitchButton.h"
#include "SButtonDelegate.h"
#include "UserDetailsDlg.h"
#include "UserAddDlg.h"
#include "SMaskWidget.h"
#include "xlsxdocument.h"	//文档
#include "xlsxworkbook.h"	//工作簿
#include "xlsxworksheet.h"	//工作表
#include<QFileDialog>
#include<QTextStream>
#include<QBrush>
#include<QMessageBox>
#include<QPixmap>
#include<QPalette>

const int PAGESIZE = 20;//无需修改，最好的状态

UserManagerPage::UserManagerPage(const QString& user_id, const QString& username,QWidget* parent)
	:QWidget(parent) 
	,ui(new Ui::UserManagerPage)
	,m_model(new QStandardItemModel(this))
	,m_curPage(1)
	,m_user_id(user_id)
	,m_username(username)
{
	ui->setupUi(this);
	initPrivilege();
	init();
	//qDebug() << "当前用户权限 ： " << m_privilege_edit << "  " << m_privilege_add << "  " << m_privilege_delete;
}

UserManagerPage::~UserManagerPage()
{
	delete ui;
}

void UserManagerPage::initPrivilege()
{
	qDebug() << "开始获取权限";
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
				qDebug() << "用户ID" << juser.value("user_id").toString();
				m_privilege_edit = juser.value("privilege_edit").toInt();
				m_privilege_add = juser.value("privilege_add").toInt();
				m_privilege_delete = juser.value("privilege_delete").toInt();
				qDebug() << "权限获取成功： " << m_privilege_edit << " " << m_privilege_add << " " << m_privilege_delete;
			})
		.get();
}

void UserManagerPage::getNewInfor(const QString& user_id, const QString& username)
{
	m_user_id = user_id;
	m_username = username;
	initPrivilege();
}

void UserManagerPage::init()
{
	QFile file(":/ResourceClient/style.css");
	
	if (file.open(QIODevice::ReadOnly)) {
		m_styleStr = file.readAll();
	}
	setStyleSheet(m_styleStr);
	
	//翻译映射
	SFieldTranslate::instance()->addTrans("user/user_id", "用户ID");
	SFieldTranslate::instance()->addTrans("user/username", "用户名");
	SFieldTranslate::instance()->addTrans("user/gender", "性别");
	SFieldTranslate::instance()->addTrans("user/mobile", "电话");
	SFieldTranslate::instance()->addTrans("user/email", "邮箱");
	SFieldTranslate::instance()->addTrans("user/isEnable", "账号状态");
	initMenu();
	//表格设置
	ui->tableView->setModel(m_model);
	ui->tableView->setSelectionMode(QTableView::NoSelection);
	ui->tableView->setFocusPolicy(Qt::FocusPolicy::NoFocus);
	ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
	ui->tableView->setMouseTracking(true);

	ui->firstPage->setIcon(QIcon(":/ResourceClient/double-left.png"));
	ui->prePage->setIcon(QIcon(":/ResourceClient/pre.png"));
	ui->nextPage->setIcon(QIcon(":/ResourceClient/next.png"));
	ui->lastPage->setIcon(QIcon(":/ResourceClient/double-right.png"));

	ui->turnToBox->setMinimum(1);

	ui->addUserBtn->setIcon(QIcon(":/ResourceClient/tianjiayonghu.png"));
	ui->searchBtn->setIcon(QIcon(":/ResourceClient/sousuo.png"));
	ui->exportBtn->setIcon(QIcon(":/ResourceClient/daochu.png"));
	ui->batchEnableBtn->setIcon(QIcon(":/ResourceClient/piliangqiyong.png"));
	ui->batchDisableBtn->setIcon(QIcon(":/ResourceClient/piliangjinyong.png"));
	ui->batchDeleteBtn->setIcon(QIcon(":/ResourceClient/piliangshanchu.png"));
	ui->turnTo->setIcon(QIcon(":/ResourceClient/tiaozhuan.png"));
	ui->changeFace->setIcon(QIcon(":/ResourceClient/changeFace.png"));
	ui->backLoginBtn->setIcon(QIcon(":/ResourceClient/backLogin.png"));

	//导出按钮（CSV/XLSX）
	connect(ui->exportBtn, &QPushButton::clicked, this, &UserManagerPage::onExport);
	//搜索
	//按下回车
	connect(ui->searchEdit, &QLineEdit::returnPressed, this, &UserManagerPage::onSearch);
	//connect(ui->searchEdit, &QLineEdit::textChanged, [=](const QString& text)
	//	{
	//		if (text.isEmpty()) {
	//			onSearch();
	//		}
	//	});
	connect(ui->searchBtn, &QPushButton::clicked, this, &UserManagerPage::onSearch);
	//界面创建的时候，查询一下
	onSearch();
	//如果只有一条数据，那么他就是管理员，拥有所有权限
	if (m_total == 1) {
		m_privilege_edit = 1;
		m_privilege_add = 1;
		m_privilege_delete = 1;
	}

	//连接批量操作按钮信号
	connect(ui->batchEnableBtn, &QPushButton::clicked, this, &UserManagerPage::onBatchEnable);
	connect(ui->batchDisableBtn, &QPushButton::clicked, this, &UserManagerPage::onBatchDisable);
	connect(ui->batchDeleteBtn, &QPushButton::clicked, this, &UserManagerPage::onBatchDelete);
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
	auto hHeaderView = new SHeaderView(Qt::Horizontal);
	ui->tableView->setHorizontalHeader(hHeaderView);
	//将得到的头的最后一列适当拉伸，以适应表格宽度
	ui->tableView->horizontalHeader()->setStretchLastSection(true);
		
	//复选框联动(舍弃，使用代理)
	connect(m_model, &QStandardItemModel::itemChanged, [=](QStandardItem* item)
		{
			return;
			auto index = item->index().row();
			qDebug() << "changed" << item->checkState() << " inedx : " << index;
			if (item->checkState() == Qt::Unchecked) {
				m_checkSet.erase(index);
			}
			else {
				m_checkSet.insert(index);
			}
			int size = m_checkSet.size();
			if (size > 0 &&size < m_model->rowCount())
				hHeaderView->setState(Qt::PartiallyChecked);
			else if(size == 0)
				hHeaderView->setState(Qt::Unchecked);
			else 
				hHeaderView->setState(Qt::Checked);
		});

	//------------------------------------------------------item 代理
	//SCkeckDelegate
	//将ui->tableView设置为代理的父对象
	auto checkDelegate = new SCheckDelegate(ui->tableView);
	//给第0列设置代理为 checkDelegate
	ui->tableView->setItemDelegateForColumn(0, checkDelegate);
	connect(hHeaderView, &SHeaderView::stateChanged, [=](int state)
		{
			if (state) {
				for (size_t i = 0; i < m_model->rowCount(); i++)
				{
					auto item = m_model->item(i, 0);
					//item->setCheckState(Qt::CheckState(state));
					item->setData(state, Qt::UserRole);
					m_checkSet.insert(i);
				}
				checkDelegate->setCheckCount(m_model->rowCount());
			}
			else
			{
				for (size_t i = 0; i < m_model->rowCount(); i++)
				{
					auto item = m_model->item(i, 0);
					//item->setCheckState(Qt::CheckState(state));
					item->setData(state, Qt::UserRole);
					m_checkSet.erase(i);
				}
				checkDelegate->setCheckCount(0);
			}
		});
	connect(checkDelegate, &SCheckDelegate::stateChanged, [=](int state, const QModelIndex& index)
		{
			if (state)m_checkSet.insert(index.row());
			else m_checkSet.erase(index.row());

			size_t cnt = checkDelegate->checkCount();
			if (cnt > 0 && cnt < m_model->rowCount())
				hHeaderView->setState(Qt::PartiallyChecked);
			else if (cnt == 0)
				hHeaderView->setState(Qt::Unchecked);
			else
				hHeaderView->setState(Qt::Checked);
			 
		});

	//SUrlDelegate
	auto urlDelegate = new SUrlDelegate(ui->tableView);
	ui->tableView->setItemDelegateForColumn(column("user_id"), urlDelegate);
	connect(urlDelegate, &SUrlDelegate::requestOpenUrl, [=](const QModelIndex& index)
		{
			//qDebug() << "现在点击进入第" << index.row() << "行";
			//if(!m_detailsDlg)
			m_detailsDlg = new UserDetailsDlg(this);
			connect(m_detailsDlg, &UserDetailsDlg::userChanged, [=](const QJsonObject& user)
				{
					qDebug() << "m_detailsDlg修改，第 " << index.row() << " 行发生改变";
					m_model->item(index.row(), column("user_id"))->setText(user.value("user_id").toString());
					m_model->item(index.row(), column("username"))->setText(user.value("username").toString());
					m_model->item(index.row(), column("gender"))->setText(user.value("gender").toInt() == 1 ? "男" : user.value("gender").toInt() == 2 ? "女" : "未知");
					m_model->item(index.row(), column("isEnable"))->setData(user.value("isEnable").toInt(), Qt::UserRole);
					m_model->item(index.row(), column("mobile"))->setText(user.value("mobile").toString());
					m_model->item(index.row(), column("email"))->setText(user.value("email").toString());
				});
			connect(m_detailsDlg, &UserDetailsDlg::userDeleted, [=](const QString& user_id)
				{
					auto items = m_model->findItems(user_id, Qt::MatchFlag::MatchFixedString, column("user_id"));
					if (!items.isEmpty()) {
						auto row = items.first()->row();
						m_model->removeRow(row);
					}
				});
			
			m_detailsDlg->getAccount(m_user_id,m_username,m_privilege_edit,m_privilege_delete);
			m_detailsDlg->setUser(user(index.row()));
			m_detailsDlg->resize(this->size());
			m_detailsDlg->show();

			qDebug() << index;
		});

	//SSwitchDelegate
	auto switchDelegate = new SSwitchDelegate(ui->tableView);
	ui->tableView->setItemDelegateForColumn(column("isEnable"), switchDelegate);
	connect(switchDelegate, &SSwitchDelegate::stateChanged, [=](bool state, const QModelIndex& index)
		{ 
			//m_checkSet.insert(index.row());
			//if (state) {
			//	qDebug() << "m_checkSet插入一条数据";
			//	onBatchEnable();
			//}
			//else {
			//	qDebug() << "m_checkSet删除一条数据";
			//	onBatchDisable();
			//};
			//有编辑权限才能够修改用户状态
			if (!m_privilege_edit) {
				drawMessageBox("无修改权限");
				//难点 ： 如何让SwitchButton的状态不再改变
				onSearch();//侥幸过关
				return;
			}
			auto user_id = m_model->item(index.row(), column("user_id"))->text();
			auto isEnable = m_model->data(index, Qt::UserRole).toInt();
			if (m_user_id == user_id) {
				drawMessageBox("不可修改自己的账号状态");
				onSearch();
				return;
			}
			SHttpClient(URL("/api/user/alter?user_id=" + user_id)).debug(true)
				.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
				.json({{ "isEnable",isEnable }})
				.fail([](const QString& msg, int code)
					{
						qDebug() << msg << code;
					})
				.success([=](const QByteArray& data)
					{
						QJsonParseError error;
						auto jdoc = QJsonDocument::fromJson(data, &error);
						if (jdoc["code"].toInt() < 1000) {
							qDebug() << "修改成功";
						}
					}).post();
		});
	
	//SButtonDelegate
	auto buttonDelegate = new SButtonDelegate(ui->tableView);
	ui->tableView->setItemDelegateForColumn(column("操作"), buttonDelegate);
	auto privilegeBtn = buttonDelegate->addButton(new QPushButton("权限设置"));
	auto delBtn = buttonDelegate->addButton(new QPushButton("删除"));
	QString styleStrBtn = R"(QPushButton{border-radius:3px;background-color:rgb(97,162,243);}
	QPushButton:hover{background-color:red;})";
	privilegeBtn->setStyleSheet(styleStrBtn);
	privilegeBtn->setFixedSize(55, 20);
	delBtn->setStyleSheet(styleStrBtn);
	//moreBtn->setStyleSheet(styleStr);
	delBtn->setFixedSize(45, 20);
	//moreBtn->setFixedSize(60, 20);

	connect(buttonDelegate, &SButtonDelegate::buttonClicked, [=](int id, const QModelIndex& index)
		{
			qDebug() << id;
			auto user_id = m_model->item(index.row(), column("user_id"))->text();
			if (id == 0) {
				//一点按下极限设置就跳转到角色管理页面，并在搜索栏中键入user_id，并按下搜索(重难点)
				//思路：向 userManagerPAge 和 RoleMAnagerPage 的共同父类MainWindow发送信号，
				//父类接收到信号后再对信号进行转发。。。（解决)
				qDebug() << "向 MainWindow 发送信号";
				emit finduser(user_id);
			}
			//删除用户
			else if (id == 1) {
				if (m_user_id == user_id) {
					drawMessageBox("不可删除自己");
					return;
				}
				if (!m_privilege_delete) {
					drawMessageBox("无删除权限");
					return;
				}
				
				QJsonArray jarr;
				QJsonArray j;
				jarr.append(user_id);
				j.append(user_id);
				SHttpClient(URL("/api/user/delete")).debug(true)
					.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
					.json({ {"lists",jarr} })
					.fail([](const QString& msg, int code)
						{
							qDebug() << msg << code;
						})
					.success([=](const QByteArray& data)
						{
							QJsonParseError error;
							auto jdoc = QJsonDocument::fromJson(data, &error);
							if (jdoc["code"].toInt() < 1000) {
								//删除成功
								m_model->removeRow(index.row());
								m_total -= 1;
								if (m_total % PAGESIZE == 0 && m_totalPage != 1)m_curPage -= 1;
								onSearch();
								SHttpClient(URL("/api/user_privilege/delete")).debug(true)
									.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
									.json({ {"lists",j} })
									.fail([](const QString& msg, int code)
										{
											qDebug() << msg << code;
										})
									.success([=](const QByteArray& data)
										{
											qDebug() << "用户权限删除成功";
										})
											.post();
							}
							else {
								//删除失败

							}
						})
							.post();
			}
		});

	connect(ui->backLoginBtn, &QPushButton::clicked, [=]
		{
			qDebug() << "返回登录";
			auto config = sApp->globalConfig();
			config->setValue("user/autoLogin", false);
			emit backLogin();
		});
}

QJsonObject UserManagerPage::user(int row) const
{	//获取第row行的 user 信息
	QJsonObject juser;
	juser.insert("user_id", m_model->item( row, column("user_id"))->text());
	juser.insert("username", m_model->item( row, column("username"))->text());
	juser.insert("gender", m_model->item( row, column("gender"))->text());
	juser.insert("isEnable", m_model->item( row, column("isEnable"))->data(Qt::UserRole).toInt());
	juser.insert("mobile", m_model->item( row, column("mobile"))->text());
	juser.insert("email", m_model->item( row, column("email"))->text());
	return juser;
}

void UserManagerPage::resizeEvent(QResizeEvent* ev)
{
	//重写resizEvent函数，使细节窗口匹配userManagerPage窗口大小
	if (m_detailsDlg)m_detailsDlg->resize(this->size());
}

void UserManagerPage::onSearch()
{
	QVariantMap params;//参数
	params.insert("idDeleted", 0);
	auto filter = ui->searchEdit->text();
	if (!filter.isEmpty()) {//即搜索框否有字，有的话就作为过滤器
		params.insert("query", filter);
	}
	params.insert("curPage", m_curPage);//传递当前页
	SHttpClient(URL("/api/user/list"))
		.header("Authorization","Bearer"+ sApp->userData("user/token").toString())
		.params(params)
		.success([=](const QByteArray& data)
			{
				QJsonParseError error;
				auto jdoc = QJsonDocument::fromJson(data, &error);
				if (error.error != QJsonParseError::NoError) {
					return;//有错误就退出
				}
				//解析服务器传递来的信息
				parseJson(jdoc.object());
			})
		.get();
	
}

void UserManagerPage::setBatchEnable(int op)
{
	QJsonObject jobj;
	QJsonArray jarr;
	QString str = (op == 1 ? "正常" : "禁用");//描述当前状态
	//判断有哪些行是被选中的
	/*for (size_t i = 0; i < m_model->rowCount(); i++)//效率过于低下
	{
		auto check = m_model->item(i);//默认列为0
		auto user_id = m_model->item(i, 1)->text();
		qDebug() << " user_id : " << user_id;
		if (!user_id.isEmpty() && check->checkState() == Qt::CheckState::Checked) {
			qDebug() << " m_model->item(i, 6)->text() : " << m_model->item(i, 6)->text();
			if (m_model->item(i, 6)->text() != str) {
				QJsonObject recvObj;
				QJsonObject filterObj{ {"user_id",user_id} };
				QJsonObject updateObj{ {"isEnable",op} };
				recvObj.insert("filter", filterObj);
				recvObj.insert("update", updateObj);
				jarr.append(recvObj);
			}
		}
	}*/
	//优化
	qDebug() << "遍历m_checkSet，其大小为 ： "<<m_checkSet.size();
	for (auto& val : m_checkSet) {
		if (m_model->item(val, 6)->text() != str) {
			QJsonObject recvObj;
			QJsonObject filterObj{ {"user_id",m_model->item(val, 1)->text()} };
			QJsonObject updateObj{ {"isEnable",op} };
			recvObj.insert("filter", filterObj);
			recvObj.insert("update", updateObj);
			jarr.append(recvObj);
		}
	}
	jobj.insert("lists", jarr);
	SHttpClient(URL("/api/user/batch_alter")).debug(true)
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.json(jobj)
		.fail([](const QString& msg, int code)
			{
				qDebug() << "批量修改失败";
			})
		.success([=](const QByteArray& data)
			{
				onSearch();

			}).post();
		
}

void UserManagerPage::onBatchEnable()
{
	if (!m_privilege_delete) {
		drawMessageBox("无权启用");
		return;
	}
	setBatchEnable(1);
}

void UserManagerPage::onBatchDisable()
{
	if (!m_privilege_delete) {
		drawMessageBox("无权禁用");
		return;
	}
	setBatchEnable(0);
}

void UserManagerPage::onBatchDelete()
{
	if (!m_privilege_delete) {
		drawMessageBox("无权删除");
		return;
	}
	QJsonObject jobj;
	QJsonArray jarr;
	QJsonObject j;
	QJsonArray jj;
	
	//判断有哪些行是被选中的
	/*for (size_t i = 0; i < m_model->rowCount(); i++)
	{
		auto item = m_model->item(i,1);//默认列为0
		//qDebug() << " item->text() : " << item->text();
		if (item && item->checkState() == Qt::CheckState::Checked) {
			jarr.append(item->text());
		}
	}*/
	//优化
	int s = m_checkSet.size();
	for (auto& val : m_checkSet) {
		auto item = m_model->item(val, column("user_id"));
		jarr.append(item->text());
		jj.append(item->text());
	}
	jobj.insert("lists", jarr);
	j.insert("lists", jj);
	SHttpClient(URL("/api/user/delete")).debug(true)
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.json(jobj)
		.fail([](const QString& msg, int code)
			{
			})
		.success([=](const QByteArray& data)
			{
				m_total -= s;
				if (m_total % PAGESIZE == 0 && m_curPage != 1)m_curPage -= 1;
				onSearch();
				SHttpClient(URL("/api/user_privilege/delete")).debug(true)
					.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
					.json(j)
					.fail([](const QString& msg, int code)
						{
						})
					.success([=](const QByteArray& data)
						{
							qDebug() << "用户权限批量删除成功";

						}).post();
			}).post();
}

void UserManagerPage::setCurrentPage(int page)//获取第page页
{
	ui->turnToBox->setValue(m_curPage);
	setPageLabel(m_curPage, m_totalPage, m_total);
	onSearch();
}

void UserManagerPage::setPageLabel(int curPage, int totalPage, int total)
{
	QString labStr = QString("<font color=red>第 %1 页</font> <font color=#FF33FF>每页 %2 条</font> <font color=#33CCFF>共 %3 页</font> <font color=#FF9933>共 %4 条</font>").arg(m_curPage).arg(PAGESIZE).arg(m_totalPage).arg(m_total);
	ui->pageLabel->setText(labStr);
}

void UserManagerPage::parseJson(const QJsonObject& obj)
{
	//先清空再写入
	m_model->clear();
	for (size_t i = 0; i < m_fieldName.size(); i++)
	{
		//写表头
		auto item = new QStandardItem(SFieldTranslate::instance()->trans("user", m_fieldName[i]));
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
	ui->tableView->setColumnWidth(column("user_id"), 130);
	ui->tableView->setColumnWidth(column("gender"), 60);
	ui->tableView->setColumnWidth(column("email"), 170);
	ui->tableView->setColumnWidth(column("操作"), 130);
	//修改 pageLable 内容
	m_curPage = jobj.value("curPage").toInt();
	m_totalPage = jobj.value("totalPage").toInt();
	m_total = jobj.value("total").toInt();
	setPageLabel(m_curPage, m_totalPage, m_total);
	ui->turnToBox->setValue(m_curPage);//此处不能调用setCurpage否则会造成循环
	//qDebug() << "数据解析成功";
	dynamic_cast<SHeaderView*>(ui->tableView->horizontalHeader())->setState(Qt::Unchecked);
	m_checkSet.clear();
}

void UserManagerPage::drawMessageBox(const QString& text)
{
	QMessageBox box(this);
	box.setWindowTitle("No Privilege");
	box.setText(text);
	if (text == "导出成功")
		box.setIconPixmap(QPixmap(":/ResourceClient/exportSuccess.png").scaled(70, 70));
	else box.setIconPixmap(QPixmap(":/ResourceClient/wuquan.png").scaled(70, 70));;
	//box.setStyleSheet(m_styleStr);
	box.exec();
}

int UserManagerPage::column(const QString& field)const
{
	auto pos = m_fieldName.indexOf(field);
	if (pos == -1) {
		qWarning() << "field " << field << "not exists";
	}
	return pos;
}

//根据拿到的一行数据进行解析插入
QList<QStandardItem*> UserManagerPage::itemsFromJsonObject(const QJsonObject& jobj)
{
	QList<QStandardItem*>items;
	auto gender = jobj.value("gender").toInt();
	auto isEnable = jobj.value("isEnable").toInt();//此处转Bool会出错
	//qDebug() << "数据中的isEnable为 ： " << jobj.value("isEnable");
	for (const auto& field : m_fieldName) {
		auto item = new QStandardItem;
		item->setTextAlignment(Qt::AlignCenter);
		//gender 和 isEnable 特判
		if (field == "gender")item->setText(gender ==1  ? "男" : gender ==2 ?"女" : "未知");
		else if (field == "isEnable") { 
			item->setText(isEnable ? "启用" : "禁用");
			item->setData(isEnable, Qt::UserRole);
		}
		else item->setText(jobj.value(field).toVariant().toString());
		items.append(item);
	}
	return items;
}

void UserManagerPage::initMenu()
{
	m_importMenu = new QMenu(this);
	ui->addUserBtn->setMenu(m_importMenu);
	m_importMenu->addAction(QIcon(":/ResourceClient/dantiaodaoru.png"), "单条导入", this, &UserManagerPage::singleImport);
	m_importMenu->addAction(QIcon(":/ResourceClient/piliangdaoru.png"),"批量导入", this, &UserManagerPage::batchImport);
	
	m_faceMenu = new QMenu(this);
	ui->changeFace->setMenu(m_faceMenu);
	
	m_faceMenu->addAction(QIcon(":/ResourceClient/pink.png"), "粉色背景", this, [=]
		{
			QFile file(":/ResourceClient/pink.css");
			if (file.open(QIODevice::ReadOnly)) {
				m_styleStr = file.readAll();
			}
			setStyleSheet(m_styleStr);
			QPalette pal(this->palette());
			pal.setColor(QPalette::Window, QColor(231,166,190));
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

void UserManagerPage::singleImport()
{
	if (!m_privilege_add) {
		drawMessageBox("无权添加用户");
		return;
	}
	qDebug() << "添加用户";
	m_userAddDlg = new UserAddDlg;
	//SMaskWidget::instance()->addDialog(m_userAddDlg);//转到popup调用
	
	connect(m_userAddDlg, &UserAddDlg::newUser, [=]//(const QJsonObject& user)
		{
			qDebug() << "导入一条数据";
			m_userAddDlg->close();
			m_userAddDlg->deleteLater();
			onSearch();

		});
	//m_userAddDlg->show();
	//弹出添加用户界面
	SMaskWidget::instance()->popup(m_userAddDlg);
}

void UserManagerPage::batchImport()
{
	if (!m_privilege_add) {
		drawMessageBox("无权添加用户");
		return;
	}
	//getOpenFileName可以弹出一个文件选择框
	auto filename = QFileDialog::getOpenFileName(this, "选择用户表格", "./", "xlsx (*.xlsx;*.csv);;all (*.*)");
	if (filename.isEmpty()) {
		return;
	}
	qDebug() << filename;
	
	//判断是什么类型的文件 csv \ xlsx
	auto  extraName = QFileInfo(filename).suffix();//后缀
	if (extraName.compare("csv", Qt::CaseInsensitive) == 0) {//比较判断后缀类型
		readCSVFile(filename);
	}
	else if (extraName.compare("xlsx", Qt::CaseInsensitive) == 0) {
		readXLSXFile(filename);
	}
}
 
void UserManagerPage::onExport()
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

void UserManagerPage::readCSVFile(const QString& filename)
{
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly)) {
		qWarning() << filename << " open failed : " << file.errorString();
		return;
	}
	
	QTextStream stream(&file);
	auto guessBom = file.read(3);
	if (guessBom.startsWith("\xEF\xBB\xBF"))
		stream.setEncoding(QStringConverter::Utf8);
	else 
		stream.setEncoding(QStringConverter::System);

	auto header = stream.readLine().split(',');
	//qDebug() << header;
	QJsonArray jarr;
	QJsonArray j;
	while (!stream.atEnd()) {
		auto lineData = stream.readLine().split(',');
		//qDebug() << lineData;
		if (lineData.size() != 4)
		{
			qWarning() << "表头字段数量不正确,导入失败";
			break;
		}
		auto gender = lineData.at(2) == "女" ? 0 : lineData.at(2) == "男" ? 1 : 2;
		QJsonObject jobj;
		jobj.insert("user_id", lineData.at(0));
		jobj.insert("username", lineData.at(1));
		jobj.insert("gender", gender);
		jobj.insert("mobile", lineData.at(3));
		jobj.insert("password", "123456");
		jarr.append(jobj);

		QJsonObject jj;
		jj.insert("user_id", lineData.at(0));
		jj.insert("username", lineData.at(1));
		jj.insert("privilege_read", 1);
		j.append(jj);
	}
	SHttpClient(URL("/api/user/batch_create"))
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.json({ {"lists",jarr}})
		.success([=](const QByteArray& data)
			{
				QJsonParseError error;
				auto jdoc = QJsonDocument::fromJson(data, &error);
				if (error.error != QJsonParseError::NoError && jdoc["code"].toInt() > 1000) {
					qDebug() << "添加用户出错";
					return;
				}
				else {
					onSearch();
					SHttpClient(URL("/api/user_privilege/batch_create"))
						.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
						.json({ {"lists",j} })
						.success([=](const QByteArray& data)
							{
								QJsonParseError error;
								auto jdoc = QJsonDocument::fromJson(data, &error);
								if (error.error != QJsonParseError::NoError && jdoc["code"].toInt() > 1000) {
									qDebug() << "添加用户权限出错";
									return;
								}
								else {
									qDebug() << "权限批量导入成功";
								}
							})
						.post();
				}
			})
		.post();
}

void UserManagerPage::readXLSXFile(const QString& filename)
{
	using namespace QXlsx;
	Document doc(filename);
	if (!doc.load()) {
		qWarning() <<filename<< "xlsx load failed";
		return;
	}
	//获取工作簿
	//Workbook* book = doc.workbook();
	//qDebug() << "sheetCount : " << book->sheetCount();
	//qDebug() << "sheetNames" << doc.sheetNames();
	//获取工作表
	//book->sheet(0);
	auto sheet = doc.currentWorksheet();
	//获取单元格子的范围
	CellRange range = sheet->dimension();
	//qDebug() << range.rowCount()<<range.columnCount();
	QJsonArray jarr;
	QJsonArray j;

	for (size_t i = 2; i <= range.rowCount(); i++)//去掉表头
	{
		QJsonObject jobj;
		QJsonObject jj;
		for (size_t j = 1; j <= range.columnCount(); j++)
		{
			Cell* cell = sheet->cellAt(i, j);
			if (cell) {
				//qDebug() << cell->value().toString();
				auto v = cell->value().toString();
				if (j == 1)jobj.insert("user_id", v), jj.insert("user_id", v);
				else if(j == 2)jobj.insert("username", v), jj.insert("username", v);
				else if(j == 3)jobj.insert("gender", v == "女" ? 0 :v == "男" ? 1 : 2);
				else if(j == 4)jobj.insert("mobile", v);
				jobj.insert("password", "123456");
				jj.insert("privilege_read", 1);
			}
		}
		j.append(jj);
		jarr.append(jobj);
	}

	SHttpClient(URL("/api/user/batch_create"))
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.json({ {"lists",jarr} })
		.success([=](const QByteArray& data)
			{
				QJsonParseError error;
				auto jdoc = QJsonDocument::fromJson(data, &error);
				if (error.error != QJsonParseError::NoError && jdoc["code"].toInt() > 1000) {
					qDebug() << "添加用户出错";
					return;
				}
				else {
					onSearch();
					SHttpClient(URL("/api/user_privilege/batch_create"))
						.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
						.json({ {"lists",j} })
						.success([=](const QByteArray& data)
							{
								QJsonParseError error;
								auto jdoc = QJsonDocument::fromJson(data, &error);
								if (error.error != QJsonParseError::NoError && jdoc["code"].toInt() > 1000) {
									qDebug() << "添加用户权限出错";
									return;
								}
								else {
									qDebug() << "批量添加用户权限成功";
								}
							})
						.post();
				}
			})
		.post();
}

void UserManagerPage::writeCSVFile(const QString& filename)
{
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly)) {
		qWarning() << filename << " open failed : " << file.errorString();
		return;
	}
	QTextStream stream(&file);
	//写表头
	for (int i = 1; i < m_model->columnCount() - 1; i++)
	{
		stream<<m_model->horizontalHeaderItem(i)->text();
		if (i < m_model->columnCount() - 2)stream << ",";
	}
	stream << "\n";

	//写数据	
	for (int i = 0; i < m_model->rowCount() ; i++)
	{
		for (int j = 1; j < m_model->columnCount() - 1; j++)
		{
			auto item = m_model->item(i, j);
			if (item)stream << item->text();
			if (j < m_model->columnCount() - 2)stream << ",";
		}
		stream << "\n";
	}
	drawMessageBox("导出成功");
}

void UserManagerPage::writeXLSXFile(const QString& filename)
{
	using namespace QXlsx;
	Document doc(filename);

	auto sheet = doc.currentWorksheet();
	//写表头
	for (size_t i = 1; i < m_model->columnCount() - 1; i++)
	{
		sheet->write(1, i, m_model->horizontalHeaderItem(i)->text());
	}
	//写数据
	for (size_t i = 0; i < m_model->rowCount(); i++)
	{
		for (size_t j = 1; j < m_model->columnCount() - 1; j++)
		{
			auto item = m_model->item(i, j);
			if (item) {
				sheet->write(i+2, j, item->text());
			}
		}
	}
	//保存文档
	doc.save();
	drawMessageBox("导出成功");
}
