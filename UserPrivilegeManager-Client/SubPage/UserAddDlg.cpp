#include "UserAddDlg.h"
#include "SHttpClient.h"
#include "SApp.h"
#include <QBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFormLayout>
#include <QLineEdit>
#include <QRadioButton>
#include <QFile>
#include<QMessageBox>
#include<QButtonGroup>
UserAddDlg::UserAddDlg(QWidget* parent)
	:SIODialog(parent)
{
	init();
}

void UserAddDlg::init() {

	this->setStyleSheet("background-color:#FFFFCC");
	this->setAttribute(Qt::WA_StyledBackground);

	this->setWindowTitle("添加用户");
	//this->setFixedWidth(300);
	m_user_id_edit = new QLineEdit;
	m_username_edit = new QLineEdit;

	m_gender_man_rbtn = new QRadioButton("男");
	m_gender_man_rbtn->setChecked(true);//默认选中
	m_gender_women_rbtn = new QRadioButton("女");
	m_gender_group = new QButtonGroup;
	m_gender_group->addButton(m_gender_man_rbtn);
	m_gender_group->addButton(m_gender_women_rbtn);

	m_mobile_edit = new QLineEdit;
	m_email_edit = new QLineEdit;

	m_def_password_rbtn = new QRadioButton("默认密码");
	m_def_password_rbtn->setChecked(true);//默认选中
	m_custom_password_rbtn = new QRadioButton("自定义密码");
	m_password_group = new QButtonGroup;
	m_password_group->addButton(m_def_password_rbtn);
	m_password_group->addButton(m_custom_password_rbtn);

	m_password_edit = new QLineEdit;

	auto mlayout = new QVBoxLayout(centralWidget());
	//mlayout->setSizeConstraint(QLayout::SetFixedSize);


	auto flayout = new QFormLayout;
	flayout->setRowWrapPolicy(QFormLayout::RowWrapPolicy::WrapAllRows);
	flayout->setSpacing(4);
	
	flayout->addRow("<font color='red'>*</font>用户ID", m_user_id_edit);
	flayout->addRow(new QLabel("<font size=2 face='华文行楷' color='gray'>最长64位，允许英文字母、数字</font>"));
	
	flayout->addRow("<font color='red'>*</font>用户名", m_username_edit);
	flayout->addRow(new QLabel("<font size=2 face='华文行楷' color='gray'>最长64位，允许中文、英文字母、数字</font>"));

	auto hlayoutgender = new QHBoxLayout;
	hlayoutgender->addWidget(m_gender_man_rbtn);
	hlayoutgender->addWidget(m_gender_women_rbtn);
	hlayoutgender->addStretch();
	flayout->addRow("<font color='red'>*</font>性别", hlayoutgender);

	flayout->addRow("<font color='gray'>*</font>电话号码", m_mobile_edit);
	flayout->addRow(new QLabel("<font size=2 face='华文行楷' color='gray'>最长11位，允许数字及-符号</font>"));
	
	flayout->addRow("<font color='gray'>*</font>电子邮箱", m_email_edit);
	flayout->addRow(new QLabel("<font size=2 face='华文行楷' color='gray'>最长32位，允许英文字母、数字、特殊字符&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</font>"));

	auto hlayout = new QHBoxLayout;
	hlayout->addWidget(m_def_password_rbtn);
	hlayout->addWidget(m_custom_password_rbtn);
	hlayout->addStretch();

	flayout->addRow("<font color='red'>*</font>初始密码", hlayout);
	flayout->addRow(m_password_edit);

	mlayout->addLayout(flayout);

	QFile file(":/ResourceClient/style.css");
	QString styleStr;
	if (file.open(QIODevice::ReadOnly)) styleStr = file.readAll();
	
	{
		//此处为什么不添加到SIODialog的原因是：在确定、取消按钮和标题栏之间还有一段描述区域
		auto okBtn = new QPushButton("确定");
		okBtn->setObjectName("OK");
		okBtn->setStyleSheet(styleStr);

		auto cancelBtn = new QPushButton("取消");
		cancelBtn->setObjectName("NO");
		cancelBtn->setStyleSheet(styleStr);

		auto bhlayout = new QHBoxLayout;
		bhlayout->addStretch();
		bhlayout->addWidget(okBtn);
		bhlayout->addWidget(cancelBtn);

		mlayout->addStretch();
		mlayout->addLayout(bhlayout);

		connect(okBtn, &QPushButton::clicked,this,&UserAddDlg::onOkBtn);

		connect(cancelBtn, &QPushButton::clicked, [=] {this->close();});
	}

	m_def_password_rbtn->setChecked(true);
	m_password_edit->hide();
	connect(m_custom_password_rbtn, &QRadioButton::toggled, [=](bool checked)
		{
			m_password_edit->setVisible(checked);
		});
	m_user_id_edit->setText(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));//设置默认ID

}

void UserAddDlg::onOkBtn()
{
	if (m_user_id_edit->text().isEmpty() || m_username_edit->text().isEmpty()) {
		const QString text = "User_id and username cannot be empty !";
		drawMessageBox(text);
		return;
	}
	if (m_mobile_edit->text().size() > 11) {
		const QString text = "The phone number cannot  greater than 11 digits !";
		drawMessageBox(text);
		return;
	}
	QString pwd = m_password_edit->text();
	if (m_def_password_rbtn->isChecked())pwd = "123456";//防止先使用自定义密码再使用默认密码
	m_juser.insert("user_id", m_user_id_edit->text());
	m_juser.insert("username", m_username_edit->text());
	m_juser.insert("gender", m_gender_man_rbtn->isChecked()?1:0);
	m_juser.insert("mobile", m_mobile_edit->text());
	m_juser.insert("email", m_email_edit->text());
	m_juser.insert("password", pwd);

	SHttpClient(URL("/api/user/create"))
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.json(m_juser)
		.success([=](const QByteArray& data)
			{
				QJsonParseError error;
				auto jdoc = QJsonDocument::fromJson(data, &error);
				if (error.error != QJsonParseError::NoError && jdoc["code"].toInt() > 1000) {
					qDebug() << "添加用户出错";
					return;
				}
				else {
					//此时数据库已经修改完成
					//m_juser.insert("isEnable", 1);
					//emit newUser(m_juser);
					emit newUser();
					QJsonObject j;
					j.insert("user_id", m_juser.value("user_id"));
					j.insert("username", m_juser.value("username"));
					j.insert("privilege_read", 1);

					SHttpClient(URL("/api/user_privilege/create"))
						.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
						.json(j)
						.success([=](const QByteArray& data)
							{
								QJsonParseError error;
								auto jdoc = QJsonDocument::fromJson(data, &error);
								if (error.error != QJsonParseError::NoError && jdoc["code"].toInt() > 1000) {
									qDebug() << "添加用户权限出错";
									return;
								}
								else {
									//此时数据库已经修改完成
									//m_juser.insert("isEnable", 1);
									//emit newUser(m_juser);
									//qDebug() << "用户权限导入成功";
								}
							})
						.post();
				}
			})
		.post();
}




