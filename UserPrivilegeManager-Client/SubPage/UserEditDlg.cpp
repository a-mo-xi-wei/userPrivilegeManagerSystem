#include "UserEditDlg.h"
#include "SHttpClient.h"
#include "SApp.h"
#include <QBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFormLayout>
#include <QLineEdit>
#include <QRadioButton>
#include <QFile>
UserEditDlg::UserEditDlg(QWidget* parent)
	:SIODialog(parent)
{
	init();
}

void UserEditDlg::init() {
	this->setStyleSheet("background-color:#FFFFCC");
	this->setAttribute(Qt::WA_StyledBackground);
	this->setWindowTitle("编辑用户");
	//this->setFixedWidth(300);
	m_user_id_edit = new QLineEdit;
	m_username_edit = new QLineEdit;
	m_mobile_edit = new QLineEdit;
	m_email_edit = new QLineEdit;
	m_gender_man_rbtn = new QRadioButton("男");
	m_gender_women_rbtn = new QRadioButton("女");

	m_user_id_edit->setEnabled(false);

	//auto mlayout = new QVBoxLayout(this);
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


	mlayout->addLayout(flayout);
	QFile file(":/ResourceClient/style.css");
	QString styleStr;
	if (file.open(QIODevice::ReadOnly)) styleStr = file.readAll();
	{
		auto okBtn = new QPushButton("确定");
		auto cancelBtn = new QPushButton("取消");
		//设置样式
		okBtn->setObjectName("OK");
		okBtn->setStyleSheet(styleStr);

		cancelBtn->setObjectName("NO");
		cancelBtn->setStyleSheet(styleStr);
		//两个按钮水平布局
		auto bhlayout = new QHBoxLayout;
		bhlayout->addStretch();
		bhlayout->addWidget(okBtn);
		bhlayout->addWidget(cancelBtn);
		mlayout->addStretch();
		mlayout->addLayout(bhlayout);

		connect(okBtn, &QPushButton::clicked,this,&UserEditDlg::onOkBtn);
		//取消就关闭窗口
		connect(cancelBtn, &QPushButton::clicked, [=] {this->close();});
	}

}

void UserEditDlg::setUser(const QJsonObject& user)
{
	m_juser = user;
	//将初始信息填入编辑框
	m_user_id_edit->setText(m_juser.value("user_id").toString());
	m_username_edit->setText(m_juser.value("username").toString());
	m_mobile_edit->setText(m_juser.value("mobile").toString());
	m_email_edit->setText(m_juser.value("email").toString());
	//默认选中
	if (m_juser.value("gender").toString() == "男")m_gender_man_rbtn->setChecked(true);
	else m_gender_women_rbtn->setChecked(true);
}

void UserEditDlg::onOkBtn()
{
	if (m_username_edit->text().isEmpty()) {
		const QString text = "Username cannot be empty !";
		drawMessageBox(text);
		return;
	}
	if (m_mobile_edit->text().size() > 11) {
		const QString text = "The phone number cannot  greater than 11 digits !";
		drawMessageBox(text);
		return;
	}

	m_juser.insert("user_id", m_user_id_edit->text());
	m_juser.insert("username", m_username_edit->text());
	m_juser.insert("gender", m_gender_man_rbtn->isChecked()?1:2);
	m_juser.insert("mobile", m_mobile_edit->text());
	m_juser.insert("email", m_email_edit->text());
	//将相应信息传给服务器
	SHttpClient(URL("/api/user/alter?user_id=" + m_juser.value("user_id").toString()))
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.json(m_juser)
		.success([=](const QByteArray& data)
			{
				QJsonParseError error;
				auto jdoc = QJsonDocument::fromJson(data, &error);
				if (error.error != QJsonParseError::NoError && jdoc["code"].toInt() > 1000) {
					qDebug() << "编辑失败";
					return;
				}
				else {
					//发送信号用户改变
					//qDebug() << "用户编辑成功+++++++++++++++";
					int gender = m_juser.value("gender").toInt();
					m_juser.insert("gender", gender == 1 ? "男" : "女");
					emit userChanged(m_juser);
					this->close();
				}
			})
		.post();


}

