#ifndef USERADDDLG_H_
#define USERADDDLG_H_

#include "SIODialog.h"
#include<QJsonObject>

class QPushButton;
class QLabel;
class QRadioButton;
class QLineEdit;
class QButtonGroup;

class UserAddDlg : public SIODialog
{
	Q_OBJECT
public:
	UserAddDlg(QWidget* parent = nullptr);
	void init();
public:
	void onOkBtn();
signals:
	//void newUser(const QJsonObject& user);
	void newUser();
private:
	QJsonObject m_juser;
	
	QLineEdit* m_user_id_edit{};
	QLineEdit* m_username_edit{};
	QRadioButton* m_gender_man_rbtn{};
	QRadioButton* m_gender_women_rbtn{};
	QLineEdit* m_mobile_edit{};
	QLineEdit* m_email_edit{};
	QRadioButton* m_def_password_rbtn{};
	QRadioButton* m_custom_password_rbtn{};
	QLineEdit* m_password_edit{};

	QButtonGroup* m_gender_group{};
	QButtonGroup* m_password_group{};
};

#endif // !USERADDDLG_H_
