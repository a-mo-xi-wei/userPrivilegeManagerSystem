#include "PManagerPage.h"
#include "ui_PManagerPage.h"

#include "SApp.h"
#include "SCheckDelegate.h"
#include "SFieldTranslate.h"
#include "SHeaderView.h"
#include "SHttpClient.h"
#include <QFile>
#include <QFileDialog>
#include <set>
#include<QFont>
const int PAGESIZE = 20;
PManagerPage::PManagerPage(QWidget* parent)
	:QWidget(parent)
	,ui(new Ui::PManagerPage)
	,m_model(new QStandardItemModel(this))
{
	ui->setupUi(this);
	init();
}

PManagerPage::~PManagerPage()
{
	delete ui;
}

void PManagerPage::init()
{
	SFieldTranslate::instance()->addTrans("privilege/number", "权限编号");
	SFieldTranslate::instance()->addTrans("privilege/privilege", "权限名称");
	SFieldTranslate::instance()->addTrans("privilege/descr", "权限描述");
	SFieldTranslate::instance()->addTrans("privilege/area", "权限范围");

	//表格设置
	ui->tableView->setModel(m_model);
	//不可选择
	ui->tableView->setSelectionMode(QTableView::NoSelection);
	//不可聚焦
	ui->tableView->setFocusPolicy(Qt::FocusPolicy::NoFocus);
	//不可编辑
	ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
	//文字可换行
	ui->tableView->setWordWrap(true);
	//ui->tableView->setTextElideMode(Qt::ElideNone);
	//最后一列可收缩
	ui->tableView->horizontalHeader()->setStretchLastSection(true);
	
	getPrivilegeList();
}

void PManagerPage::getPrivilegeList()
{
	SHttpClient(URL("/api/privilege/list")).debug(true)
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.success([=](const QByteArray& data)
			{
				QJsonParseError error;
				auto jdom = QJsonDocument::fromJson(data, &error);
				if (error.error != QJsonParseError::NoError) {
					return;
				}
				parseJson(jdom.object());

			}).get();
}

void PManagerPage::setPageLabel(int curPage, int totalPage, int total)
{
	QString labStr = QString("<font color=red>第 %1 页</font> <font color=#FF33FF>每页 %2 条</font> <font color=#33CCFF>共 %3 页</font> <font color=#FF9933>共 %4 条</font>").arg(m_curPage).arg(PAGESIZE).arg(m_totalPage).arg(m_total);
	ui->pageLabel->setText(labStr);
}

void PManagerPage::parseJson(const QJsonObject& obj)
{
	m_model->clear();
	for (size_t i = 0; i < m_fieldName.size(); i++) {
		m_model->setHorizontalHeaderItem(i, new QStandardItem(SFieldTranslate::instance()->trans("privilege", m_fieldName[i])));
	}
	auto jarray =  obj.value("data").toObject().value("lists").toArray();
	
	auto jobj = obj.value("data").toObject();
	auto arr = jobj.value("lists").toArray();
	for (size_t i = 0; i < arr.size(); i++)
	{
		auto juser = arr.at(i).toObject();
		//将每行数据放进去
		m_model->appendRow(itemsFromJsonObject(juser));
	}
	ui->tableView->setColumnWidth(1, 150);
	ui->tableView->setColumnWidth(2, 200);
	//修改 pageLable 内容
	m_curPage = jobj.value("curPage").toInt();
	m_totalPage = jobj.value("totalPage").toInt();
	m_total = jobj.value("total").toInt();
	setPageLabel(m_curPage, m_totalPage, m_total);
}

int PManagerPage::column(const QString& field)
{
	auto pos =  m_fieldName.indexOf(field);
	if (pos == -1) {
		qWarning() << "field " << field << "not exists!";
	}
	return pos;
}

QList<QStandardItem*> PManagerPage::itemsFromJsonObject(const QJsonObject& jobj)
{
	QList<QStandardItem*> items;
	for (const auto& field : m_fieldName)
	{
		auto item = new QStandardItem;
		item->setTextAlignment(Qt::AlignCenter);
		QString str = jobj.value(field).toVariant().toString();
		if (field == "privilege") {
			QFont font = item->font();
			font.setFamily("JetBrains Mono");
			font.setBold(true);
			item->setFont(font);
		}
		item->setText(str);
		items.append(item);
	}
	return items;
}

