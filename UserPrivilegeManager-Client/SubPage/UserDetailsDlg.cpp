#include "UserDetailsDlg.h"
#include "SApp.h"
#include "SHttpClient.h"
#include "SMaskWidget.h"
#include <QBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFormLayout>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include<QHttpMultiPart>
#include<QPainter>
#include<QPainterPath>
#include<QPalette>
#include<QStyleOption>
UserDetailsDlg::UserDetailsDlg(QWidget* parent)
	:QWidget(parent)
{
	init();
}

void UserDetailsDlg::init()
{
	//this->setObjectName("detailsDlg");
	//此处样式为按钮设置
	QFile file(":/ResourceClient/style.css");
	QString styleStr;
	if (file.open(QIODevice::ReadOnly))styleStr = file.readAll();
	this->setStyleSheet(styleStr);
	//解决背景颜色无法呈现
	//①使用QPalette来设置背景色
	/*QPalette pal(this->palette());
	pal.setColor(QPalette::Window, QColor("#CCFFFF"));
	this->setAutoFillBackground(true);
	this->setPalette(pal);
	this->show();*/
	//②重写paintEvent函数
	
	//主布局为垂直布局
	auto mlayout = new QVBoxLayout(this);
	
	//返回按钮
	m_backBtn = new QPushButton;
	m_backBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	//m_backBtn->setStyleSheet(styleStr);

	mlayout->addWidget(m_backBtn);
	//填入一段空隙
	mlayout->addSpacerItem(new QSpacerItem(1,30));

	//用户基本信息
	auto baseInfoBtn = new QPushButton("编辑信息");
	baseInfoBtn->setIcon(QIcon(":/ResourceClient/bianji.png"));
	auto resetPasswordBtn = new QPushButton("重置密码");
	resetPasswordBtn->setIcon(QIcon(":/ResourceClient/chongzhi.png"));
	auto delBtn = new QPushButton("删除用户");
	delBtn->setIcon(QIcon(":/ResourceClient/shanchu.png"));
	auto avatarAlterBtn = new QPushButton("修改");
	avatarAlterBtn->setIcon(QIcon(":/ResourceClient/xiugai.png"));
	//设置样式（有点丑陋）
	//baseInfoBtn->setStyleSheet(styleStr);
	//宽度不够，单独设置
	baseInfoBtn->setFixedWidth(90);
	//resetPasswordBtn->setStyleSheet(styleStr);
	//delBtn->setStyleSheet(styleStr);
	//avatarAlterBtn->setStyleSheet(styleStr);

	//水平布局基本信息
	auto hlayout = new QHBoxLayout;
	hlayout->addWidget(new QLabel("用户基本信息"));
	hlayout->addWidget(baseInfoBtn);
	hlayout->addWidget(resetPasswordBtn);
	hlayout->addWidget(delBtn);
	hlayout->addStretch();

	mlayout->addLayout(hlayout);
	
	{
		//相关标签
		m_avatar_lab = new QLabel;
		m_user_id_lab = new QLabel;
		m_username_lab = new QLabel;
		m_gender_lab = new QLabel;
		m_mobile_lab = new QLabel;
		m_email_lab = new QLabel;
		//账号状态控制按钮
		m_isEnable_btn = new SSwitchButton;
		m_isEnable_btn->setFixedWidth(45);
		//默认头像
		// 创建一个QPixmap并绘制图像
		QPixmap pixmap(":/ResourceClient/default_avatar.jpg");
		//绘制头像
		drawEllipsePixmap(pixmap);
		//设置大小
		m_avatar_lab->setFixedSize(40, 40);
		m_avatar_lab->setScaledContents(true);
		//头像和修改按钮水平布局
		auto alayout = new QHBoxLayout;
		alayout->addWidget(m_avatar_lab);
		alayout->addWidget(avatarAlterBtn);

		auto lflayout = new QFormLayout;
		
		lflayout->addRow("头 像 : ",alayout);
		lflayout->addRow("用户ID: ",m_user_id_lab);
		lflayout->addRow("用户名: ",m_username_lab);
		lflayout->addRow("性别: ",m_gender_lab);

		auto rflayout = new QFormLayout;
		auto vlayout = new QVBoxLayout;
		vlayout->addSpacerItem(new QSpacerItem(0, 40));
		rflayout->addRow(vlayout);
		rflayout->addRow("手机号码: ", m_mobile_lab);
		rflayout->addRow("电子邮箱: ", m_email_lab);
		rflayout->addRow("账号状态: ", m_isEnable_btn);
		//将之前的表单布局变成水平布局
		auto hlayout = new QHBoxLayout;
		hlayout->addLayout(lflayout);
		hlayout->addSpacerItem(new QSpacerItem(60,0));
		hlayout->addLayout(rflayout);
		hlayout->addStretch();
		mlayout->addLayout(hlayout);
	}
	//添加弹簧
	mlayout->addStretch();
	//按下返回键
	connect(m_backBtn, &QPushButton::clicked, [this]
		{
			//close();
			deleteLater();
		});
	//删除
	connect(delBtn, &QPushButton::clicked, [=]
		{
			if (m_user_id != m_juser.value("user_id").toString() && !m_privilege_delete) {
				drawMessageBox("无删除权限");
				return;
			}
			QJsonArray jarr;
			auto user_id = m_juser.value("user_id").toString();
			jarr.append(user_id);
			SHttpClient(URL("/api/user/delete")).debug(true)
				.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
				.json({ {"lists",jarr}})
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
							emit(userDeleted(user_id));
							//按下返回键
							m_backBtn->click();
						}
					})
						.post();
		});
	//编辑基本信息按钮
	connect(baseInfoBtn, &QPushButton::clicked, [=]
		{
			if (m_user_id != m_juser.value("user_id").toString() && !m_privilege_edit) {
				drawMessageBox("无编辑权限");
				return;
			}
			m_userEditDlg = new UserEditDlg;
			connect(m_userEditDlg, &UserEditDlg::userChanged, [=](const QJsonObject& user)
				{
					m_juser = user;
					updateUi();
					QString gender = m_juser.value("gender").toString();
					m_juser.insert("gender", gender == "男" ? 1 : 2);
					emit UserDetailsDlg::userChanged(m_juser);
					m_userEditDlg->deleteLater();
				});
			m_userEditDlg->setUser(m_juser);
			//m_userEditDlg->show();
			SMaskWidget::instance()->popup(m_userEditDlg);
		});
	//重置密码按钮
	connect(resetPasswordBtn, &QPushButton::clicked, [=]
		{
			if (m_user_id != m_juser.value("user_id").toString() && !m_privilege_edit) {
				drawMessageBox("无修改权限");
				return;
			}
			m_userResetPasswd = new UserResetPasswdDlg;
			m_userResetPasswd->getUserPassword(m_juser.value("user_id").toString());
			connect(m_userResetPasswd, &UserResetPasswdDlg::passwordChanged, [=]
				{
					m_userResetPasswd->deleteLater();
				});
			SMaskWidget::instance()->popup(m_userResetPasswd, SMaskWidget::PopPosition::MiddlePosition);
		});

	connect(avatarAlterBtn, &QPushButton::clicked, this, &UserDetailsDlg::onAvatarUpload);

	connect(m_isEnable_btn, &SSwitchButton::stateChanged, [=](bool state)
		{
			if (!m_privilege_edit) {//即使是自己，也不能随便改状态，这是管理员的责任
				drawMessageBox("无修改权限");
				updateUi();
				return;
			}
			auto user_id = m_juser.value("user_id").toString();
			if (m_user_id == user_id) {
				drawMessageBox("不可修改自己的账号状态");
				updateUi();
				return;
			}
			//qDebug() << "现在的状态是" <<int(state);
			SHttpClient(URL("/api/user/alter?user_id=" + user_id)).debug(true)
				.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
				.json({ {"isEnable",int(state)} })
				.fail([=](const QString& msg, int code) {

					})
				.success([=](const QByteArray& data)
					{
						QJsonParseError error;
						auto jdoc = QJsonDocument::fromJson(data, &error);
						if (error.error != QJsonParseError::NoError) {

						}
						else {
							m_juser.insert("isEnable", int(state));
							emit UserDetailsDlg::userChanged(m_juser);
						}
					})
				.post();
		});
}

void UserDetailsDlg::setUser(const QJsonObject& user)
{
	m_juser = user;//获取到user信息
	updateUi();//更新界面
	onAvatarDownload();//获取头像
}

void UserDetailsDlg::getAccount(const QString& user_id, const QString& username, int privilege_edit, int privilege_delete)
{
	m_user_id = user_id;
	m_username = username;
	m_privilege_edit = privilege_edit;
	m_privilege_delete = privilege_delete;
}

void UserDetailsDlg::onAvatarDownload()
{
	SHttpClient(URL("/api/user/avatar")).debug(true)
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.param("user_id", m_juser.value("user_id").toString())
		.fail([=](const QString& msg, int code) {
			//qDebug() << "未找头像";
		})
		.success([=](const QByteArray& data)
			{
				//qDebug() << "头像数据下载成功 : "<<data;
				if (data.startsWith('{'))
				{
					QJsonParseError error;
					auto jdoc = QJsonDocument::fromJson(data, &error);
					// 创建一个QPixmap并绘制图像
					QPixmap pixmap(":/ResourceClient/default_avatar.jpg");

					drawEllipsePixmap(pixmap);

					//m_avatar_lab->setPixmap(QPixmap(":/ResourceClient/default_avatar.jpg"));
					//m_avatar_lab->setMask(QRegion(m_avatar_lab->rect(), QRegion::RegionType::Ellipse));
				}
				else
				{
					auto img = QImage::fromData(data);
					// 创建一个QPixmap并绘制图像
					QPixmap pixmap(QPixmap::fromImage(img));

					drawEllipsePixmap(pixmap);
					//m_avatar_lab->setPixmap(QPixmap::fromImage(img));
					//m_avatar_lab->setMask(QRegion(m_avatar_lab->rect(), QRegion::RegionType::Ellipse));
				}
			})
		.get();

}

void UserDetailsDlg::onAvatarUpload()
{
	if (m_user_id != m_juser.value("user_id").toString() && !m_privilege_edit) {
		drawMessageBox("无编辑权限");
		return;
	}

	auto path = sApp->globalConfig()->value("other/select_avatar_path",
		QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).toString();
	auto filename = QFileDialog::getOpenFileName(this, "选择头像", path, "images (*.jpg;*.jpeg;*.png)");
	if (filename.isEmpty())
		return;
	sApp->globalConfig()->setValue("other/select_avatar_path", QFileInfo(filename).canonicalPath());

	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly)) {
		drawMessageBox("头像打开失败");
		return;
	}

	//上传的头像不能超过3M
	if (file.size() > 3 * 1024 * 1034) {
		drawMessageBox("请选择3M以内的图片");
		return;
	}

	auto url = URL("/api/user/avatar?user_id=" + m_juser.value("user_id").toString());

	QNetworkAccessManager* mangaer = new QNetworkAccessManager(this);
	QNetworkRequest request(url);
	request.setRawHeader("Authorization", "Bearer" + sApp->userData("user/token").toString().toUtf8());
	request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/x-www-form-urlencoded");

	QHttpMultiPart* mpart = new QHttpMultiPart(this);
	QHttpPart part;
	part.setBody(file.readAll());
	part.setHeader(QNetworkRequest::ContentTypeHeader, QString("image/%1").arg(QFileInfo(filename).suffix()));
	part.setHeader(QNetworkRequest::ContentDispositionHeader, QString(R"(attachment;name="file";filename="%1")")
		.arg(QFileInfo(filename).fileName()));
	mpart->append(part);

	auto reply = mangaer->post(request, mpart);
	connect(reply, &QNetworkReply::finished, [=]
		{
			if (reply->error() != QNetworkReply::NoError)
			{
				//qDebug() << "replay error" << reply->errorString();
				m_avatar_lab->setPixmap(QPixmap(":/ResourceClient/default_avatar.jpg"));
				m_avatar_lab->setMask(QRegion(m_avatar_lab->rect(),QRegion::Rectangle));
			}
			else
			{
				//qDebug() << QString(reply->readAll());
				m_avatar_lab->setPixmap(QPixmap(filename));
				m_avatar_lab->setMask(QRegion(m_avatar_lab->rect(), QRegion::RegionType::Rectangle));
			}
		});
}

void UserDetailsDlg::paintEvent(QPaintEvent*)
{
	//绘制背景
	QPainter p(this);
	p.setPen(Qt::NoPen);
	p.setBrush(QColor("#CCFFFF"));
	p.drawRect(rect());
}

void UserDetailsDlg::drawEllipsePixmap(QPixmap& pixmap)//未实现成功
{
	QPainter painter(&pixmap);
	painter.setRenderHint(QPainter::Antialiasing, true);

	//// 在QPainter上绘制遮罩形状
	//QPainterPath path;
	//path.addEllipse(pixmap.rect());

	//// 设置合成模式为DestinationIn
	//painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);

	//// 使用透明色填充path区域以外的部分
	//painter.fillPath(path, Qt::white);
	

	// 将QPainter的内容绘制到m_avatar_lab上
	m_avatar_lab->setPixmap(pixmap);
}

void UserDetailsDlg::updateUi()
{
	m_backBtn->setIcon(QIcon(":/ResourceClient/fanhui.png"));
	m_backBtn->setText(m_juser.value("username").toString());
	m_user_id_lab->setText(m_juser.value("user_id").toString());
	m_username_lab->setText(m_juser.value("username").toString());
	m_gender_lab->setText(m_juser.value("gender").toString());
	m_mobile_lab->setText(m_juser.value("mobile").toString());
	m_email_lab->setText(m_juser.value("email").toString());
	m_isEnable_btn->setToggle(m_juser.value("isEnable").toInt());
}

void UserDetailsDlg::drawMessageBox(const QString& text)
{
	QMessageBox box(this);
	box.setWindowTitle("No Privilege");
	box.setText(text);
	box.setIconPixmap(QPixmap(":/ResourceClient/wuquan.png").scaled(70, 70));;
	//box.setStyleSheet(m_styleStr);
	box.exec();
}