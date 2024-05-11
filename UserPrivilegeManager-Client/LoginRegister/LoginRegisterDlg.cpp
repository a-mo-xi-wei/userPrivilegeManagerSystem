#include "LoginRegisterDlg.h"
#include "SEventFilterObject.h"
#include "SApp.h"
#include "SHttpClient.h"
#include <QPainter>
#include <QStaticText>
#include <QPushButton>
#include <QFile>
#include <QStackedWidget>
#include <QBoxLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QRadioButton>
#include <QLabel>
#include <QSettings>
#include <QJsonDocument> 
#include <QJsonObject>
#include <QJsonParseError>
#include <QApplication>
#include <QTimer>
#include <QMessageBox>
#include <QNetworkReply>

LoginRegisterDlg::LoginRegisterDlg(QWidget* parent)
	:QDialog(parent)
	,m_manager(new QNetworkAccessManager(this))
	,m_username("")
{
	//在库中的资源，必须手动初始化
    Q_INIT_RESOURCE(LoginRegister_Res);
	initUi();
	
}

LoginRegisterDlg::~LoginRegisterDlg()
{
	Q_CLEANUP_RESOURCE(LoginRegister_Res);
}

void LoginRegisterDlg::initUi()
{
	//去掉标题栏
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	//让无边框窗口能够被拖动
	installEventFilter(new SEventFilterObject(this));
	//让窗口背景透明
	setAttribute(Qt::WA_TranslucentBackground);

	//调整窗口大小
	resize(740, 470);
	//右边白色矩形
	m_rightRect = { 5,40,width() - 5 * 2,height() - 40 * 2 };
	//左边的矩形
	m_leftRect = {5,5,185,440};
	//中间弦的矩形
	m_middleRect = { 5,5,370,440 };

	//提示框
	m_tipBox = new QLabel(this);
	m_tipBox->setGeometry(434, 390,200,30);
	m_tipBox->setAlignment(Qt::AlignCenter);
	m_timer = new QTimer(this);
	m_timer->setInterval(1000);
	m_timer->callOnTimeout([=] { m_tipBox->clear(); m_timer->stop(); });

	//setTip("请登录QAQ!");

	//关闭按钮
	auto closeBtn = new QPushButton(this);
	closeBtn->setGeometry(width() - 45, 45, 32, 32);
	closeBtn->setFocusPolicy(Qt::FocusPolicy::NoFocus);
	closeBtn->setObjectName("closeBtn");
	connect(closeBtn, &QPushButton::clicked, this, &QDialog::close);

	//堆栈窗口
	m_stkWidget = new QStackedWidget(this);
	m_stkWidget ->setGeometry(434, 150, 200, 240);
	m_stkWidget ->setContentsMargins(0, 0, 0, 0);

	m_loginWidget =  CreateLoginWidget();
	m_registerWidget =  CreateRegisterWidget();
	m_stkWidget->addWidget(m_loginWidget);
	m_stkWidget->addWidget(m_registerWidget);


	//加载样式表
	QFile file(":/LoginRegister_Res/style.css");
	if (file.open(QIODevice::ReadOnly))
	{
		setStyleSheet(file.readAll());
	}
}

QWidget* LoginRegisterDlg::CreateLoginWidget()
{
	QWidget* w = new QWidget;

	auto config = sApp->globalConfig();

	m_accountEdit = new  QLineEdit;
	m_passwordEdit = new  QLineEdit;
	m_autoLoginChx = new QCheckBox("自动登陆");
	m_rememberPwdChx = new QCheckBox("记住密码");
	auto goRegisterBtn = new QPushButton("注册账号");
	auto loginBtn = new QPushButton("登录");

	m_accountEdit->setPlaceholderText("输入用户ID/手机/邮箱");

	m_passwordEdit->setPlaceholderText("输入密码");
	m_passwordEdit->setEchoMode(QLineEdit::EchoMode::Password);
	
	
	m_autoLoginChx->setChecked(config->value("user/autoLogin").toBool());
	connect(m_autoLoginChx, &QCheckBox::stateChanged, [=](int checked) 
		{
			//如果选择了自动登陆，那么自动勾选上记住密码(不记住密码，怎么自动登陆)
			if (checked)
				m_rememberPwdChx->setChecked(true);
			config->setValue("user/autoLogin", m_autoLoginChx->isChecked());
		});

	m_rememberPwdChx->setChecked(config->value("user/rememberPassword").toBool());
	m_rememberPwdChx->setObjectName("rememberPwdChx");
	connect(m_rememberPwdChx, &QCheckBox::stateChanged, [=](int checked)
		{
			//如果取消记住密码，则取消自动登陆(没记住密码，哪来的自动登陆)
			if (!checked)
				m_autoLoginChx->setChecked(false);
			config->setValue("user/rememberPassword", m_rememberPwdChx->isChecked());

		});

	if (m_rememberPwdChx->isChecked()) {
		m_accountEdit->setText(config->value("user/user_id").toString());
		m_passwordEdit->setText(config->value("user/password").toString());
	}

	goRegisterBtn->setObjectName("goRegisterBtn");
	goRegisterBtn->setFocusPolicy(Qt::FocusPolicy::NoFocus);

	loginBtn->setObjectName("loginBtn");
	//设置快捷键为Enter，打开登录界面时，可以直接按回车登录
	loginBtn->setShortcut(Qt::Key::Key_Return);
	loginBtn->setDefault(true);

	//输入完成，按回车直接登录
	connect(m_accountEdit, &QLineEdit::returnPressed, this, &LoginRegisterDlg::onLogin);
	connect(m_passwordEdit, &QLineEdit::returnPressed, this, &LoginRegisterDlg::onLogin);
	connect(loginBtn, &QPushButton::clicked, this, &LoginRegisterDlg::onLogin);
	connect(goRegisterBtn, &QPushButton::clicked, [=]
		{
			qDebug() << "goRegisterBtn";
			m_stkWidget->setCurrentWidget(m_registerWidget);
		});

	//如果上次退出之前勾选了自动登陆，则自动点击登陆按钮
	if (config->value("user/autoLogin").toBool())
		loginBtn->click();

	auto hlayout = new QHBoxLayout;
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->addWidget(m_autoLoginChx);
	hlayout->addWidget(m_rememberPwdChx);
	hlayout->addWidget(goRegisterBtn);
	
	auto vlayout = new QVBoxLayout;
	vlayout->setSpacing(6);
	vlayout->setContentsMargins(0, 0, 0, 0);
	vlayout->addWidget(m_accountEdit);
	vlayout->addWidget(m_passwordEdit);
	vlayout->addLayout(hlayout);
	vlayout->addWidget(loginBtn);

	w->setLayout(vlayout);

	return w;
}

void LoginRegisterDlg::showBox()
{
	const QString text = "服务器连接失败";
	QMessageBox box(this);
	box.setWindowTitle("Error!");
	box.setText(text);
	box.setIconPixmap(QPixmap(":/ResourceClient/404.png").scaled(70, 70));
	box.setStyleSheet("\
						QMessageBox{\
							background-color:#FFFF99;\
							min-width: 150px;\
							min-height: 60px;\
						}\
						QLabel{\
							font-family: '华文行楷';\
							font-size: 18px;\
							color: red;\
							padding-left:0px;\
							margin-right:10px;\
							qproperty-alignment: AlignCenter;\
						}\
						QPushButton{\
							background-color: #99FF33;\
							color: black;\
							width:40px;\
							height:10px;\
							margin-right:10px;\
							border: none;\
							border-radius: 4px;\
						}\
						QPushButton:hover {\
							background-color: #CCFF33;\
						}"\
	);
	box.exec();
}

QWidget* LoginRegisterDlg::CreateRegisterWidget()
{
	QWidget* w = new QWidget;

	m_regit_accountEdit = new QLineEdit;
	m_regit_passwordEdit = new QLineEdit;
	m_regit_rePasswordEdit= new QLineEdit;
	m_regit_nicknameEdit= new QLineEdit;
	m_regit_postCbx = new QComboBox;

	auto registerBtn = new QPushButton("注册");
	auto backLoginBtn = new QPushButton("返回登录");

	m_regit_accountEdit->setPlaceholderText("请输入用户ID/手机号/邮箱");
	m_regit_passwordEdit->setPlaceholderText("请输入密码");
	m_regit_passwordEdit->setEchoMode(QLineEdit::Password);
	m_regit_rePasswordEdit->setPlaceholderText("请再次输入密码");
	m_regit_rePasswordEdit->setEchoMode(QLineEdit::Password);
	m_regit_nicknameEdit->setPlaceholderText("请输入用户名");

	registerBtn->setObjectName("registerBtn");
	backLoginBtn->setObjectName("backLoginBtn");

	connect(backLoginBtn, &QPushButton::clicked, [=]
		{
			m_stkWidget->setCurrentWidget(m_loginWidget);
		});
	connect(registerBtn, &QPushButton::clicked, this, &LoginRegisterDlg::onRegister);

	
	auto hlayout2 = new QHBoxLayout;
	hlayout2->setContentsMargins(0, 0, 0, 0);
	hlayout2->setSpacing(0);
	hlayout2->addWidget(registerBtn);
	hlayout2->addWidget(backLoginBtn);

	auto vlayout = new QVBoxLayout(w);
	vlayout->setContentsMargins(0, 0, 0, 0);
	vlayout->setSpacing(6);
	vlayout->addWidget(m_regit_accountEdit);
	vlayout->addWidget(m_regit_nicknameEdit);
	vlayout->addWidget(m_regit_passwordEdit);
	vlayout->addWidget(m_regit_rePasswordEdit);
	//vlayout->addWidget(m_regit_postCbx);
	vlayout->addLayout(hlayout2);

	return w;
}

void LoginRegisterDlg::onLogin()
{
	if (m_isLogining) {
		setTip("正在登录中，不要着急...");
		return;
	}
	m_isLogining = true;
	auto account = m_accountEdit->text();
	auto password = m_passwordEdit->text();
	auto isRememberPwd = m_rememberPwdChx->isChecked();

	if (account.isEmpty())
	{
		setTip("请输入账号~");
		return;
	}
	else if (password.isEmpty())
	{
		setTip("请输入密码~");
		return;
	}

	setTip("登录中,请稍后...");

	SHttpClient(URL("/api/login"))
		.debug(true)
		.json({ {"user_id",account},{"password",password} })
		.fail([=](const QString& msg, int code)
			{
				setTip(msg);
				if (code == QNetworkReply::ConnectionRefusedError) {
					showBox();
				}
			})
		.success([=](const QByteArray& data)
			{
				QJsonParseError error;
				auto jdoc = QJsonDocument::fromJson(data, &error);
				if (error.error != QJsonParseError::NoError) {
					qWarning() << "服务器返回的数据错误，需要json数据";
					return;
				}
				//qDebug() << QString(data);
				if (jdoc["code"].toInteger() < 1000)
				{
					auto token = jdoc.object().value("data").toObject().value("token").toString();
					sApp->setUserData("user/token", token);
					auto config = sApp->globalConfig();
					config->setValue("user/user_id", account);
					config->setValue("user/password", password);
					config->setValue("user/rememberPassword", isRememberPwd);
					config->setValue("user/autoLogin", m_autoLoginChx->isChecked());
					emit getUserId(account,m_username);
					accept();
				}
				else {
					setTip(jdoc["message"].toString());
				}
			})
		.complete([=](bool hasError)//完成
			{
				m_isLogining = false;
			})
		.post();


	/*
	QJsonObject jobj{ {"user_id",account},{"password",password}};

	//发送登录请求	http://127.0.0.1:8888/api/login
	QNetworkRequest request(QUrl("http://" + sApp->globalConfig()->value("host/host").toString() + "/api/login"));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	auto reply = m_manager->post(request, QJsonDocument(jobj).toJson(QJsonDocument::Compact));
	connect(reply, &QNetworkReply::errorOccurred, [=]
		{
			qWarning() << "reply error:" << reply->errorString();
		});
	connect(reply, &QNetworkReply::finished, [=]
		{
			//判断有没有错误
			if (reply->error() == QNetworkReply::NoError)
			{
				//判断是不是json数据
				auto data = reply->readAll();
				QJsonParseError error;
				auto jdoc = QJsonDocument::fromJson(data, &error);
				if (error.error != QJsonParseError::NoError) {
					qWarning() << "服务器返回的数据错误，需要json数据";
				}
				else
				{
					qDebug() << QString(data);
					if (jdoc["code"].toInteger() < 1000)
					{
						auto token = jdoc.object().value("data").toObject().value("token").toString();
						sApp->setUserData("user/token", token);
					}
				}
			}
			qDebug() << sApp->userData("user/token").toString();
			
			reply->deleteLater();
		});
	*/

}

void LoginRegisterDlg::onRegister()
{
	auto account = m_regit_accountEdit->text();
	auto password = m_regit_passwordEdit->text();
	auto rePassword = m_regit_rePasswordEdit->text();
	auto post = m_regit_postCbx->currentText();
	auto username = m_regit_nicknameEdit->text();
	if (account.isEmpty())
	{
		setTip("账号不能为空~");
		return;
	}
	else if (password.isEmpty())
	{
		setTip("密码不能为空");
		return;
	}
	else if (rePassword.isEmpty())
	{
		setTip("请输入确认密码");
		return;
	}
	else if (password != rePassword)
	{
		setTip("两次输入的密码不一致");
		return;
	}
	else if (password.size() < 6)
	{
		setTip("你的密码过于简单，请重设(最少6位)");
		return;
	}
	setTip("注册中...");

	//发送注册请求
	SHttpClient(URL("/api/register"))
		.debug(true)
		.json({
			{"user_id",account},
			{"password",password},
			{"username",username}
			})
		.fail([=](const QString& msg, int code)
			{
				setTip(msg);
			})
		.success([=](const QByteArray& data)
			{
				QJsonParseError error;
				auto jdoc = QJsonDocument::fromJson(data, &error);
				if (error.error != QJsonParseError::NoError) {
					setTip( "服务器返回的数据错误，需要json数据");
					return;
				}
				if (jdoc["code"].toInteger() < 1000) {
					setTip("注册完成，请返回登录！");
					//emit registerSuccess(account,username);//尝试失败，在登录之前获取token是一个错误行为
					//以下为替代行为
					m_username = username;
				}
				else
					setTip(jdoc["message"].toString());
			}).post();
}

void LoginRegisterDlg::paintEvent(QPaintEvent* ev)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	//绘制右边白色矩形
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::white);
	painter.drawRoundedRect(m_rightRect, 5, 5);

	//绘制左边的矩形qRgb(27, 45, 67)
	painter.setBrush(QColor(27, 45, 67));
	painter.drawRoundedRect(m_leftRect, 5, 5);

	//绘制中间的弦
	painter.setBrush(QColor(27, 45, 67));
	//painter.drawChord(m_middleRect, -90 *  16, 180 *  16);
	painter.drawChord(m_middleRect, -120 * 16, 240 * 16);

	//绘制welcome
	painter.setPen(Qt::white);
	painter.setFont(QFont("JetBrains Mono", 30));
	painter.drawStaticText(48, 60, QStaticText("Welcome!"));

	painter.setFont(QFont("JetBrains Mono", 10, QFont::Bold));
	painter.drawStaticText(48, 120, QStaticText("用户权限管理系统——客户端 v2.1.0"));

	//梦想
	painter.drawPixmap(QRect(40, 200, 250, 250), QPixmap(":/LoginRegister_Res/cai.png"));
	painter.drawPixmap(QRect(40, 200, 250, 250), QPixmap(":/LoginRegister_Res/dream.png"));

	//logo
	painter.drawPixmap(QPoint(423, 70), QPixmap(":/LoginRegister_Res/jxauedu.png"));
}


void  LoginRegisterDlg::setTip(const QString& msg)
{
	m_timer->stop();
	m_tipBox->setText("<font color=red>" + msg + "</font>");
	m_timer->start();
}

 