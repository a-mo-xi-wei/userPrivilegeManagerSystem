#include "MainWindow.h"
#include "SMaskWidget.h"
#include <QBoxLayout>
#include <QFile>
#include <QSplitter>

MainWindow::MainWindow(QString user_id,QString username,QWidget* parent)
	:QWidget(parent)
	,m_privilege_edit(0)
	,m_privilege_add(0)
	,m_privilege_delete(0)//默认只有只读权限
{
	m_user_id = user_id;
	m_username = username;
	//qDebug() << "成功获取user_id : " << user_id;
	//设置窗口图标
	this->setWindowIcon(QIcon(":/ResourceClient/windowIcon.ico"));
	init();
	//将this也就是当前窗口设为蒙层的主窗口
	SMaskWidget::instance()->setMainWidget(this);
}

void MainWindow::init()
{
	//初始化账号
	initAccount();
	//设置窗口大小
	resize(1080, 640);
	//初始化主窗口
	initMainWindow();
	//初始化用户管理界面
	initUserManagerPage();
	//初始化角色管理界面
	initRoleManagerPage();
	//初始化权限列表界面
	initPManagerPage();
	//初始化个人中心界面
	initPersonalPage();
}

void MainWindow::getNewInfor(const QString& user_id, const QString& username)
{
	m_user_id = user_id;
	m_username = username;
	//qDebug() << "重新获信息---------------------------------";
	m_userManagerPage->getNewInfor(m_user_id, m_username);
	m_roleManagerPage->getNewInfor(m_user_id, m_username);
	m_personalPage->getNewInfor(m_user_id, m_username);
}

void MainWindow::initAccount()
{
	//qDebug() << "初始化账户";
	if (m_username.isEmpty()) {//没拿到用户名，说明已经存在该用户，不需要注册
		return;
	}
	else {
		QJsonObject j;
		j.insert("user_id", m_user_id);
		j.insert("username", m_username);
		j.insert("privilege_read", 1);
		SHttpClient(URL("/api/user_privilege/create"))
			.header("Authorization", "Bearer" + sApp->userData("user/token").toString())
			.json(j)
			.success([=](const QByteArray& data)
				{
					QJsonParseError error;
					auto jdoc = QJsonDocument::fromJson(data, &error);
					if (error.error != QJsonParseError::NoError && jdoc["code"].toInt() > 1000) {
						//qDebug() << "添加用户权限出错";
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
	
}

void MainWindow::initMainWindow()
{
	//qDebug() << "初始化窗口initMainWindow";
	//m_navBar = new SNavBar;
	m_treeNavBar = new QTreeWidget;
	m_stkWidget = new QStackedWidget;
	enum NavType//导航类型
	{
		NT_UserManager,
		NT_PrivilegeManager,
		NT_RoleManager,
		NT_Personal
	};

	//分离器，使树状窗口和堆栈窗口分离
	QSplitter* sp = new QSplitter(this);
	sp->setContentsMargins(0, 0, 0, 0);
	sp->addWidget(m_treeNavBar);//添加
	sp->addWidget(m_stkWidget);
	//设置最大宽度
	//m_treeNavBar->setMaximumWidth(177);
	m_treeNavBar->setFixedWidth(160);
	//水平布局
	auto hlayout = new QHBoxLayout(this);
	//hlayout->addWidget(m_navBar);
	hlayout->addWidget(sp);//让整个导航栏水平对齐
	hlayout->setSpacing(0);
	/*m_navBar->setMinimumWidth(200);
	m_navBar->setNavHeader("用户权限管理系统");
	m_navBar->addNav(QIcon(":/ResourceClient/user.png"), "用户管理");
	m_navBar->addNav(QIcon(":/ResourceClient/role.png"), "角色管理");
	m_navBar->addNav(QIcon(":/ResourceClient/privilege.png"),"权限管理");

	connect(m_navBar, &SNavBar::idClicked,this, [](int id)
		{
			qDebug() << "id is : " << id;
		});*/
	
	m_treeNavBar->setFocusPolicy(Qt::NoFocus);//取消虚线框
	QTreeWidgetItem* item = nullptr;
	//头
	item = new QTreeWidgetItem({ "用户权限管理" });
	item->setIcon(0, QIcon(":/ResourceClient/privilege_header.png"));
	m_treeNavBar->setHeaderItem(item);
	//item
	item = new QTreeWidgetItem(m_treeNavBar,{ "用户管理" },NavType::NT_UserManager);
	item->setIcon(0, QIcon(":/ResourceClient/user.png"));
	item->setSelected(true);//默认选中

	item = new QTreeWidgetItem(m_treeNavBar, { "角色管理" }, NavType::NT_RoleManager);
	item->setIcon(0, QIcon(":/ResourceClient/role.png"));
	m_item = item;//跳转此处

	item = new QTreeWidgetItem(m_treeNavBar, { "权限列表" }, NavType::NT_PrivilegeManager);
	item->setIcon(0, QIcon(":/ResourceClient/privilege.png"));
	
	item = new QTreeWidgetItem(m_treeNavBar, { "个人中心" }, NavType::NT_Personal);
	item->setIcon(0, QIcon(":/ResourceClient/yonghuzhongxin.png"));

	//处理点击
	connect(m_treeNavBar, &QTreeWidget::itemClicked, [=](QTreeWidgetItem* item, int column)
		{
			//qDebug() << item->type() << " " << column;
			
			switch (item->type())
			{
			case NT_UserManager:
				m_stkWidget->setCurrentWidget(m_userManagerPage);
				m_userManagerPage->onSearch();
				break;
			case NT_PrivilegeManager:
				m_stkWidget->setCurrentWidget(m_pManagerPage);
				break;
			case NT_RoleManager:
				m_stkWidget->setCurrentWidget(m_roleManagerPage);
				m_roleManagerPage->onSearch();
				//每次点击就刷新一下，这么做的原因是，每次在添加用户的时候，无法将添加用户成功的信号发送给角色管理界面，
				//并且这么做准确率更高。。。
				break;
			case NT_Personal:
				m_stkWidget->setCurrentWidget(m_personalPage);
				m_personalPage->getUsername();
				break;
			default:
				break;
			}
		});
	
	QFile file(":/ResourceClient/style.css");
	if (file.open(QIODevice::ReadOnly)) {
		setStyleSheet(file.readAll());
	}
}

void MainWindow::initUserManagerPage()
{
	//qDebug() << "初始化UserManagerPage";
	m_userManagerPage = new UserManagerPage(m_user_id, m_username);
	m_stkWidget->addWidget(m_userManagerPage);

	connect(m_userManagerPage, &UserManagerPage::finduser, [=](const QString& user_id)
		{
			//切换界面
			m_stkWidget->setCurrentWidget(m_roleManagerPage);
			//同时还要重新聚焦
			auto index = m_treeNavBar->indexFromItem(m_item);
			m_treeNavBar->setCurrentIndex(index);//几经波折
			//设置搜索内容
			m_roleManagerPage->setSearchLine(user_id);
			//搜索
			m_roleManagerPage->onSearch();
		});

	connect(m_userManagerPage, &UserManagerPage::backLogin, [this] 
		{
			//qDebug() << "MainWindow 收到信号";
			emit backLogin();
		});
		
}

void MainWindow::initRoleManagerPage()
{
	//qDebug() << "初始化RoleManagerPage";
	m_roleManagerPage = new RoleManagerPage(m_user_id, m_username);
	m_stkWidget->addWidget(m_roleManagerPage);
	//m_roleManagerPage->getAccount(m_user_id, m_username);
	
}

void MainWindow::initPManagerPage()
{
	m_pManagerPage = new PManagerPage;
	m_stkWidget->addWidget(m_pManagerPage);
}

void MainWindow::initPersonalPage()
{
	m_personalPage = new PersonalPage(m_user_id, m_username);
	m_stkWidget->addWidget(m_personalPage);
}
