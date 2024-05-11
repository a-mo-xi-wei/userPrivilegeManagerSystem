#include "PersonalPage.h"
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
PersonalPage::PersonalPage(const QString& user_id, const QString& username, QWidget* parent)
	:m_user_id(user_id)
	,m_username(username)
	,QWidget(parent)
{
	init();
}

void PersonalPage::init()
{
	//this->setObjectName("detailsDlg");
	//此处样式为按钮设置
	QFile file(":/ResourceClient/style.css");
	QString styleStr;
	if (file.open(QIODevice::ReadOnly))styleStr = file.readAll();
	getUsername();
	initPrivilege();
	//主布局为垂直布局
	auto mlayout = new QVBoxLayout(this);

	m_prologue = new QLabel;
	//qDebug() << "开始绘制用户名 ： " << m_username;
	m_username = m_juser.value("username").toString();
	//qDebug() << "当前用户名 ： " << m_username;
	QString text = QString("<font size=20 face='微软雅黑' color=#66FF66>Hi,%1!</font>").arg(m_username);
	m_prologue->setText(text);
	mlayout->addWidget(m_prologue);

	//填入一段空隙
	mlayout->addSpacerItem(new QSpacerItem(1,30));

	//用户基本信息
	auto baseInfoBtn = new QPushButton("编辑信息");
	baseInfoBtn->setIcon(QIcon(":/ResourceClient/bianji.png"));
	auto resetPasswordBtn = new QPushButton("重置密码");
	resetPasswordBtn->setIcon(QIcon(":/ResourceClient/chongzhi.png"));
	auto avatarAlterBtn = new QPushButton("修改");
	avatarAlterBtn->setIcon(QIcon(":/ResourceClient/xiugai.png"));

	baseInfoBtn->setFixedWidth(90);
	
	//水平布局基本信息
	auto hlayout = new QHBoxLayout;
	hlayout->addWidget(new QLabel("<font size=6 face='华文行楷'>用户基本信息&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</font>"));

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
		m_privilege_edit_lab = new QLabel;
		m_privilege_add_lab = new QLabel;
		m_privilege_delete_lab = new QLabel;
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
		auto hlayout = new QHBoxLayout;
		auto vlayout = new QVBoxLayout;
		vlayout->addSpacerItem(new QSpacerItem(0, 1));
		rflayout->addRow(vlayout);
		hlayout->addWidget(baseInfoBtn);
		hlayout->addWidget(resetPasswordBtn);
		rflayout->addRow(hlayout);
		vlayout = new QVBoxLayout;
		vlayout->addSpacerItem(new QSpacerItem(0, 2));
		rflayout->addRow(vlayout);
		rflayout->addRow("手机号码: ", m_mobile_lab);
		rflayout->addRow("电子邮箱: ", m_email_lab);
		//将之前的表单布局变成水平布局
		hlayout = new QHBoxLayout;
		hlayout->addLayout(lflayout);
		hlayout->addSpacerItem(new QSpacerItem(60,0));
		hlayout->addLayout(rflayout);
		hlayout->addStretch();
		mlayout->addLayout(hlayout);

		auto v_privilege_layout = new QVBoxLayout;
		v_privilege_layout->addSpacerItem(new QSpacerItem(0, 40));
		v_privilege_layout->addWidget(new QLabel("<font size=6 face='华文行楷'>个人权限列表</font>"));
		v_privilege_layout->addWidget(new QLabel("<font size=5 color=#FFFF66>read_access&nbsp;:&nbsp;&nbsp;&nbsp;&nbsp;<b>true</b></font>"));
		v_privilege_layout->addWidget(m_privilege_edit_lab);
		v_privilege_layout->addWidget(m_privilege_add_lab);
		v_privilege_layout->addWidget(m_privilege_delete_lab);
		mlayout->addLayout(v_privilege_layout);
	}
	//添加弹簧
	mlayout->addStretch();
	//编辑基本信息按钮
	connect(baseInfoBtn, &QPushButton::clicked, [=]
		{
			m_userEditDlg = new UserEditDlg;
			connect(m_userEditDlg, &UserEditDlg::userChanged, [=](const QJsonObject& user)
				{
					setUser(user);
					m_userEditDlg->deleteLater();
				});
			m_userEditDlg->setUser(m_juser);
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

	connect(avatarAlterBtn, &QPushButton::clicked, this, &PersonalPage::onAvatarUpload);

}

void PersonalPage::getUsername()
{
	QVariantMap params;//参数
	params.insert("idDeleted", 0);
	params.insert("user_id", m_user_id);
	SHttpClient(URL("/api/user/list"))
		.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
		.params(params)
		.success([=](const QByteArray& data)
			{
				QJsonParseError error;
				auto jdoc = QJsonDocument::fromJson(data, &error);
				if (error.error != QJsonParseError::NoError) {
					return;//有错误就退出
				}
				//解析服务器传递来的信息
				auto jobj = jdoc.object().value("data").toObject().value("lists").toArray().at(0).toObject();
				jobj.insert("gender", jobj.value("gender") == 1 ? "男" : "女");
				setUser(jobj);
			})
		.get();
}

void PersonalPage::initPrivilege()
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
				//qDebug() << "用户ID" << juser.value("user_id").toString();
				m_privilege_edit = juser.value("privilege_edit").toInt();
				m_privilege_add = juser.value("privilege_add").toInt();
				m_privilege_delete = juser.value("privilege_delete").toInt();
				//qDebug() << "权限获取成功： " << m_privilege_edit << " " << m_privilege_add << " " << m_privilege_delete;
			})
		.get();
}

void PersonalPage::setUser(const QJsonObject& user)
{
	m_juser = user;//获取到user信息
	updateUi();//更新界面
	onAvatarDownload();//获取头像
}

void PersonalPage::getNewInfor(const QString& user_id, const QString& username)
{
	m_user_id = user_id;
	m_username = username;
	initPrivilege();
}

void PersonalPage::onAvatarDownload()
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

void PersonalPage::onAvatarUpload()
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
		QMessageBox::warning(this, "失败", "头像打开失败");
		return;
	}

	//上传的头像不能超过3M
	if (file.size() > 3 * 1024 * 1034) {
		QMessageBox::warning(this, "失败", "请选择3M以内的图片");
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
				m_avatar_lab->setMask(QRegion(m_avatar_lab->rect(), QRegion::RegionType::Ellipse));
			}
			else
			{
				//qDebug() << QString(reply->readAll());
				m_avatar_lab->setPixmap(QPixmap(filename));
				m_avatar_lab->setMask(QRegion(m_avatar_lab->rect(), QRegion::RegionType::Ellipse));
			}
		});
}

void PersonalPage::paintEvent(QPaintEvent*)
{
	//绘制背景
	QPainter p(this);
	p.setPen(Qt::NoPen);
	p.setBrush(QColor(39, 150, 156));
	p.drawRect(rect());
}

void PersonalPage::drawEllipsePixmap(QPixmap& pixmap)//未实现成功
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

void PersonalPage::updateUi()
{
	QString text = QString("<font size=20 face='微软雅黑' color=#66FF66>Hi,%1!</font>").arg(m_juser.value("username").toString());
	m_prologue->setText(text);
	m_user_id_lab->setText(m_juser.value("user_id").toString());
	m_username_lab->setText(m_juser.value("username").toString());
	m_gender_lab->setText(m_juser.value("gender").toString());
	m_mobile_lab->setText(m_juser.value("mobile").toString());
	m_email_lab->setText(m_juser.value("email").toString());
	m_privilege_edit_lab->setText("<font size=5 color=#FFFF66>edit_access&nbsp;:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + QString(m_privilege_edit ? "<b>true</b>" : "<b>false</b></font>"));
	m_privilege_add_lab->setText("<font size=5 color=#FFFF66>add_access&nbsp;:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + QString(m_privilege_add ? "<b>true</b>" : "<b>false</b></font>"));
	m_privilege_delete_lab->setText("<font size=5 color=#FFFF66>delete_access&nbsp;:&nbsp;" + QString(m_privilege_delete ? "<b>true</b>" : "<b>false</b></font>"));
}

void PersonalPage::drawMessageBox(const QString& text)
{
	QMessageBox box(this);
	box.setWindowTitle("No Privilege");
	box.setText(text);
	box.setIconPixmap(QPixmap(":/ResourceClient/wuquan.png").scaled(70, 70));;
	//box.setStyleSheet(m_styleStr);
	box.exec();
}