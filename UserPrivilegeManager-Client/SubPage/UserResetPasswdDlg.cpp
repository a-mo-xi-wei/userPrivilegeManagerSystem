#include "UserResetPasswdDlg.h"
#include "UserResetPasswdDlg.h"
#include "SHttpClient.h"
#include "SApp.h"
#include <QBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFormLayout>
#include <QLineEdit>
#include <QRadioButton>
#include <QFile>
UserResetPasswdDlg::UserResetPasswdDlg(QWidget* parent)
	:SIODialog(parent)
{
	init();
}

void UserResetPasswdDlg::init() {
	this->setStyleSheet("background-color:#FFFFCC");
	this->setAttribute(Qt::WA_StyledBackground);
	this->setWindowTitle("重置密码");
	
	m_currentPasswd = new QLineEdit;
	m_surePasswd = new QLineEdit;
	m_reSurePasswd = new QLineEdit;
	
	auto mlayout = new QVBoxLayout(centralWidget());

	auto flayout = new QFormLayout;
	flayout->setRowWrapPolicy(QFormLayout::RowWrapPolicy::WrapAllRows);
	flayout->setSpacing(4);

	flayout->addRow("<font color='red'>*</font>当前密码", m_currentPasswd);

	flayout->addRow("<font color='red'>*</font>密码", m_surePasswd);

	flayout->addRow("<font color='red'>*</font>确认密码", m_reSurePasswd);

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

		connect(okBtn, &QPushButton::clicked, this, &UserResetPasswdDlg::onOkBtn);
		//取消就关闭窗口
		connect(cancelBtn, &QPushButton::clicked, [=] {this->close();});
	}

}

void UserResetPasswdDlg::getUserPassword(const QString& user_id)
{
	m_user_id = user_id;
	//qDebug() << "user_id"<<m_user_id;
	SHttpClient(URL("/api/user/password")).debug(true)
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.param("user_id",m_user_id)
		.fail([=](const QString& msg, int code) {
			qDebug() << "密码获取失败";
			})
		.success([=](const QByteArray& data)
			{
				QJsonParseError error;
				auto jdoc = QJsonDocument::fromJson(data, &error);
				QJsonObject jobj = jdoc["data"].toObject();
				//qDebug() << "password : " << jobj.value("password").toString();
				if (error.error != QJsonParseError::NoError && jdoc["code"].toInt() > 1000) {
					qDebug() << "密码获取失败";
					return;
				}
				else {
					m_password = jobj.value("password").toString();
					//qDebug() << "m_password : " << m_password;
				}
			})
		.get();

}

void UserResetPasswdDlg::onOkBtn()
{
	if (m_currentPasswd->text().isEmpty() || m_surePasswd->text().isEmpty() || m_reSurePasswd->text().isEmpty()) {
		drawMessageBox("Password field cannot be empty !");
		return;
	}
	if (m_currentPasswd->text() != m_password) {
		//qDebug() <<"比较 : "<< m_currentPasswd->text() << " " << m_password;
		drawMessageBox("Current password input error !");
		return;
	}
	if (m_surePasswd->text() != m_reSurePasswd->text()) {
		drawMessageBox("The two password entries are inconsistent !");
		return;
	}
	QJsonObject jobj;
	jobj.insert("password", m_reSurePasswd->text());
	//将相应信息传给服务器
	SHttpClient(URL("/api/user/alter?user_id=" + m_user_id))
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.json(jobj)
		.success([=](const QByteArray& data)
			{
				QJsonParseError error;
				auto jdoc = QJsonDocument::fromJson(data, &error);
				if (error.error != QJsonParseError::NoError && jdoc["code"].toInt() > 1000) {
					qDebug() << "密码修改失败";
					return;
				}
				else {
					//发送信号用户改变
					emit passwordChanged(m_reSurePasswd->text());
					drawMessageBox("恭喜 ！修改成功 ~ QAQ ~", "success");
					this->close();
				}
			})
		.post();
}

