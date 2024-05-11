#ifndef USERRESETPASSWDDLG_H_
#define USERRESETPASSWDDLG_H_

#include "SIODialog.h"
#include<QJsonObject>

class QPushButton;
class QLabel;
class QRadioButton;
class QLineEdit;


class UserResetPasswdDlg : public SIODialog
{
	Q_OBJECT
public:
	UserResetPasswdDlg(QWidget* parent = nullptr);
	void init();
	void getUserPassword(const QString& user_id);
private:
	void onOkBtn();
signals:
	void passwordChanged(const QString& password);
private:
	QString m_user_id;
	QString m_password;
	QLineEdit* m_currentPasswd;
	QLineEdit* m_surePasswd;
	QLineEdit* m_reSurePasswd;
};

#endif // !USERRESETPASSWDDLG_H_
