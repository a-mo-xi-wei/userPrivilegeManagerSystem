#include "PAddEditDlg.h"
#include "SHttpClient.h"
#include "SApp.h"

#include <QBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QRadioButton>
#include <QFormLayout>
#include <QButtonGroup>

PAddEditDlg::PAddEditDlg(QWidget* parent)
	:SIODialog(parent)
{
	init();
}

void PAddEditDlg::init()
{
	this->setStyleSheet("background-color:white");
	this->setAttribute(Qt::WA_StyledBackground);
	setTitle("添加权限");

	m_name_edit = new QLineEdit;
	m_descr_edit = new QLineEdit;
	m_script_edit = new QTextEdit;
	m_product_service_edit = new QLineEdit;
	m_other_privilege_edit = new QLineEdit;
	m_specifiedRes_edit = new QLineEdit;

	//m_name_edit->setPlaceholderText("输入权限名称");
	m_name_edit->setPlaceholderText("服务-资源-权限");
	m_name_edit->setReadOnly(true);
	m_descr_edit->setPlaceholderText("输入权限说明，便于记忆哦！");
	m_other_privilege_edit->setPlaceholderText("输入操作权限");
	m_specifiedRes_edit->setPlaceholderText("输入资源权限");
	m_product_service_edit->setPlaceholderText("选择产品服务");
	m_script_edit->setPlaceholderText("输入JSON脚本");

	auto mlayout = new QVBoxLayout(centralWidget());
	//mlayout->setSizeConstraint(QLayout::SetFixedSize);

	auto flayout = new QFormLayout;
	flayout->setRowWrapPolicy(QFormLayout::RowWrapPolicy::WrapAllRows);
	//flayout->setContentsMargins(0, 0, 0, 0);
	flayout->setSpacing(5);

	flayout->addRow("<font color=red>*</font>权限名称",m_name_edit);
	flayout->addRow(new QLabel("<font size=2 face='宋体' color='gray'>最大长度64位，允许中文、英文字母、数字、特殊字符</font>"));
	flayout->addItem(new QSpacerItem(10,0));

	flayout->addRow("<font color=gray>*</font>权限说明",m_descr_edit);
	flayout->addItem(new QSpacerItem(10,0));

	//配置模式
	auto visualConfigRbtn = new QRadioButton("可视化配置");
	auto scriptConfigRbtn = new QRadioButton("脚本配置");
	auto configGroup = new QButtonGroup(this);
	configGroup->addButton(visualConfigRbtn);
	configGroup->addButton(scriptConfigRbtn);

	auto configLayout = new QHBoxLayout;
	configLayout->addWidget(visualConfigRbtn);
	configLayout->addWidget(scriptConfigRbtn);
	configLayout->addStretch();
	visualConfigRbtn->setChecked(true);
	m_script_edit->hide();

	flayout->addRow("<font color=red>*</font>配置模式", configLayout);
	flayout->addRow(m_script_edit);
	flayout->addItem(new QSpacerItem(10,0));

	auto vcw = CreateScriptConfigWidget();
	flayout->addRow(vcw);
	flayout->addItem(new QSpacerItem(10,0));

	connect(scriptConfigRbtn, &QRadioButton::toggled, [=](bool checked) {
		//不同的配置模式界面显示
		m_script_edit->setHidden(!checked);
		vcw->setHidden(checked);
		//更新权限名称
		onUpdateName(checked);
			});
	//脚本被编辑时
	connect(m_script_edit, &QTextEdit::textChanged, [=]() {
			onUpdateName(true);
		});
	//产品服务编辑时
	connect(m_product_service_edit, &QLineEdit::textChanged, [=]() {
			onUpdateName(false);
		});
	//权限编辑时
	connect(m_other_privilege_edit, &QLineEdit::textChanged, [=]() {
			onUpdateName(false);
		});
	//资源编辑时
	connect(m_specifiedRes_edit, &QLineEdit::textChanged, [=]() {
			onUpdateName(false);
		});
	mlayout->addLayout(flayout);

	{
		auto okBtn = new QPushButton("确定");
		auto cancelBtn = new QPushButton("取消");

		auto bhlayout = new QHBoxLayout;
		bhlayout->addStretch();
		bhlayout->addWidget(okBtn);
		bhlayout->addWidget(cancelBtn);

		mlayout->addStretch();
		mlayout->addLayout(bhlayout);

		connect(okBtn, &QPushButton::clicked, [=]()
			{
				if (visualConfigRbtn->isChecked())
					onVisualConfig();
				else
					onScriptConfig();
			});
		connect(cancelBtn, &QPushButton::clicked, [=]()
			{
				this->close();
			});
	}
	reset();
}

void PAddEditDlg::setPrivilege(const QJsonObject& user)
{
	m_jprivilege = user;
	//m_backBtn->setText(m_jprivilege.value("username").toString());
	//m_user_id_lab->setText(m_jprivilege.value("user_id").toString());
	//m_username_lab->setText(m_jprivilege.value("username").toString());
	////m_isEnable_btn->setText(m_jprivilege.value("user_id").toString());
	//m_isEnable_btn->setToggle(m_jprivilege.value("isEnable").toBool());
	//m_mobile_lab->setText(m_jprivilege.value("mobile").toString());
	//m_email_lab->setText(m_jprivilege.value("email").toString());
}

void PAddEditDlg::onVisualConfig()
{
	auto name = m_name_edit->text();
	auto descr = m_descr_edit->text();
	auto product_service = m_product_service_edit->text();
	//操作权限
	QString opPrivilege;
	if (m_readonlyPrivilegeRbtn->isChecked())
		opPrivilege = "read_access";
	else if (m_adminPrivilegeRbtn->isChecked())
		opPrivilege = "full_access";
	else
		opPrivilege = m_other_privilege_edit->text();

	//资源权限
	QString res = m_specifiedRes_edit->text();
	if (res.isEmpty())
		res = "all";

	QJsonObject jobj;
	jobj.insert("name", QString("%1-%2-%3").arg(product_service, opPrivilege, res));
	jobj.insert("descr", descr);
	//qDebug() << QJsonDocument(jobj).toJson();
}

void PAddEditDlg::onScriptConfig()
{
}

void PAddEditDlg::onUpdateName(bool isScript)
{
	//脚本配置
	if (isScript) {
		auto json = m_script_edit->toPlainText().toUtf8();
		if (json.isEmpty())
			m_name_edit->setPlaceholderText("服务-资源-权限");
		else
		{
			QJsonParseError error;
			auto jdom = QJsonDocument::fromJson(json,&error);
			if (error.error == QJsonParseError::NoError)
			{
				auto jobj = jdom.object().value("data").toObject();
				m_name_edit->setText(QString("%1-%2-%3")
					.arg(jobj.value("service").toString()
						, jobj.value("resource").toString()
						, jobj.value("privilege").toString()));
			}
			else
			{
				m_name_edit->setText("JSON脚本解析失败");
			}
		}
	}
	//可视化配置
	else {
		auto name = m_name_edit->text();
		auto descr = m_descr_edit->text();
		//服务
		auto product_service = m_product_service_edit->text();
		if (product_service.isEmpty())
			product_service = "服务";
		//操作权限
		QString opPrivilege;
		if (m_readonlyPrivilegeRbtn->isChecked())
			opPrivilege = "read_access";
		else if (m_adminPrivilegeRbtn->isChecked())
			opPrivilege = "full_access";
		else
			opPrivilege = m_other_privilege_edit->text();

		//资源权限
		QString res = m_specifiedRes_edit->text();
		if (res.isEmpty())
			res = "all";
		m_name_edit->setText(QString("%1-%2-%3").arg(product_service, opPrivilege, res));
	}

}

void PAddEditDlg::reset()
{
	//初始化默认脚本
	QJsonObject jdata;
	jdata.insert("service", "system");
	jdata.insert("privilege", "readonly");
	jdata.insert("resource", "email");

	QJsonObject jobj;
	jobj.insert("data",jdata);
	jobj.insert("version", "1");
	m_script_edit->setText(QJsonDocument(jobj).toJson());

}

QWidget* PAddEditDlg::CreateScriptConfigWidget()
{
	//脚本配置
	QWidget* vcw = new QWidget;

	auto vcLayout = new QFormLayout(vcw);
	vcLayout->setContentsMargins(0, 0, 0, 0);
	vcLayout->setSpacing(5);
	vcLayout->setRowWrapPolicy(QFormLayout::WrapAllRows);
	vcLayout->addRow("<font color=red>*</font>产品服务", m_product_service_edit);
	vcLayout->addItem(new QSpacerItem(10, 0));

	m_readonlyPrivilegeRbtn = new QRadioButton("只读权限");
	m_adminPrivilegeRbtn = new QRadioButton("管理权限");
	m_otherPrivilegeRbtn = new QRadioButton("其他权限");
	auto privilegeGroup = new QButtonGroup(this);
	privilegeGroup->addButton(m_readonlyPrivilegeRbtn);
	privilegeGroup->addButton(m_adminPrivilegeRbtn);
	privilegeGroup->addButton(m_otherPrivilegeRbtn);

	auto privilegeLayout = new QHBoxLayout;
	privilegeLayout->addWidget(m_readonlyPrivilegeRbtn);
	privilegeLayout->addWidget(m_adminPrivilegeRbtn);
	privilegeLayout->addWidget(m_otherPrivilegeRbtn);
	privilegeLayout->addStretch();
	connect(m_otherPrivilegeRbtn, &QRadioButton::toggled, [=](bool checked) {
		m_other_privilege_edit->setHidden(!checked);
		});
	m_readonlyPrivilegeRbtn->setChecked(true);
	m_other_privilege_edit->hide();

	vcLayout->addRow("<font color=red>*</font>操作权限", privilegeLayout);
	vcLayout->addRow(m_other_privilege_edit);
	vcLayout->addItem(new QSpacerItem(10, 0));

	auto allResRbtn = new QRadioButton("所有资源");
	auto specifiedResRbtn = new QRadioButton("指定资源");
	auto resGroup = new QButtonGroup(this);
	resGroup->addButton(allResRbtn);
	resGroup->addButton(specifiedResRbtn);

	auto resLayout = new QHBoxLayout;
	resLayout->addWidget(allResRbtn);
	resLayout->addWidget(specifiedResRbtn);
	resLayout->addStretch();
	connect(specifiedResRbtn, &QRadioButton::toggled, [=](bool checked) {
		m_specifiedRes_edit->setHidden(!checked);
		//qDebug() << checked;
		});
	allResRbtn->setChecked(true);
	m_specifiedRes_edit->hide();

	vcLayout->addRow("<font color=red>*</font>资源权限", resLayout);
	vcLayout->addRow(m_specifiedRes_edit);

	connect(privilegeGroup, &QButtonGroup::idClicked, [=] {
		onUpdateName(false);
		});
	connect(resGroup, &QButtonGroup::idClicked, [=] {
		onUpdateName(false);
		});
	return vcw;
}
