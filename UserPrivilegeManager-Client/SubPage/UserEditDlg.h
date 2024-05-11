#ifndef USEREDITDLG_H_
#define USEREDITDLG_H_

#include "SIODialog.h"
#include<QJsonObject>

class QPushButton;
class QLabel;
class QRadioButton;
class QLineEdit;


class UserEditDlg : public SIODialog
{
	Q_OBJECT
public:
	UserEditDlg(QWidget* parent = nullptr);
	void init();
	void setUser(const QJsonObject& user);
private:
	void onOkBtn();
signals:
	void userChanged(const QJsonObject& user);
private:
	QJsonObject m_juser;
	
	QLineEdit* m_user_id_edit{};
	QLineEdit* m_username_edit{};
	QRadioButton* m_gender_man_rbtn{};
	QRadioButton* m_gender_women_rbtn{};
	QLineEdit* m_mobile_edit{};
	QLineEdit* m_email_edit{};
};

#endif // !USERADDDLG_H_
