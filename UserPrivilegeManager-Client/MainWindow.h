#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

//#include"SNavBar.h"
#include"UserManagerPage.h"
#include "PManagerPage.h"
#include "RoleManagerPage.h"
#include "PersonalPage.h"
#include"SApp.h"
#include "SHttpClient.h"
#include<QWidget>
#include <QStackedWidget>
#include <QTreeWidget>
class MainWindow : public QWidget
{
	Q_OBJECT
public:
	MainWindow(QString user_id ,QString username ,QWidget* parent = nullptr);
	void init();
	void getNewInfor(const QString& user_id,const QString& username);
private:
	//初始化账号，即判断当前账号是否是通过注册登录进来
	void initAccount();
signals:
	void backLogin();
signals:
	void privilegeInit();
private:
	//SNavBar* m_navBar{};
	//树状导航
	QTreeWidget* m_treeNavBar{};
	QTreeWidgetItem* m_item{};//用于后期获取index
	//堆栈窗口
	QStackedWidget* m_stkWidget{};
	//初始化窗口
	void initMainWindow();

	//用户管理界面
	UserManagerPage* m_userManagerPage{};
	void initUserManagerPage();
	
	//角色管理界面
	RoleManagerPage* m_roleManagerPage{};
	void initRoleManagerPage();

	//权限列表界面
	PManagerPage* m_pManagerPage{};
	void initPManagerPage();

	//个人中心界面

	PersonalPage* m_personalPage{};
	void initPersonalPage();

	//当前登录的user_id
	QString m_user_id;
	QString m_username;
	//权限
	int m_privilege_edit;
	int m_privilege_add;
	int m_privilege_delete;
};

#endif // !MAINWINDOW_H_
