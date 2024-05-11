#ifndef PERSONALPAGE_H_
#define PERSONALPAGE_H_

#include<QWidget>
#include<QJsonObject>
#include"SSwitchButton.h"
#include "UserEditDlg.h"
#include "UserResetPasswdDlg.h"
class QPushButton;
class QLabel;
class PersonalPage : public QWidget
{
	Q_OBJECT
public:
	PersonalPage(const QString& user_id, const QString& username, QWidget* parent = nullptr);
	void init();
	void getUsername();
	void initPrivilege();
	void setUser(const QJsonObject& user);
	void getNewInfor(const QString& user_id, const QString& username);
signals:
	void userChanged(const QJsonObject& user);
signals:
	void userDeleted(const QString& user_id);
private://slots
	void onAvatarDownload();
	void onAvatarUpload();
protected:
	void paintEvent(QPaintEvent*)override;
private:
	void drawEllipsePixmap(QPixmap& pixmap);
	void drawMessageBox(const QString& text);
private:
	QJsonObject m_juser;
	QLabel* m_prologue;
	QLabel* m_avatar_lab{};
	QLabel* m_user_id_lab{};
	QLabel* m_username_lab{};
	QLabel* m_gender_lab{};
	QLabel* m_mobile_lab{};
	QLabel* m_email_lab{};
	QLabel* m_privilege_edit_lab{};
	QLabel* m_privilege_add_lab{};
	QLabel* m_privilege_delete_lab{};
	void updateUi();
	//编辑窗口
	UserEditDlg* m_userEditDlg{};
	//重置密码窗口
	UserResetPasswdDlg* m_userResetPasswd{};

	QString m_user_id;
	QString m_username;

	int m_privilege_edit;
	int m_privilege_add;
	int m_privilege_delete;
};

#endif // !PERSONALPAGE_H_
