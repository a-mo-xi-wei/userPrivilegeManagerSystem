#ifndef _LOGINREGISTERDLG_H_
#define _LOGINREGISTERDLG_H_

#include<QDialog>
#include<QNetworkAccessManager>

class QLabel;
class QStackedWidget;
class QLineEdit;
class QCheckBox;
class QRadioButton;
class QByteArray;
class QComboBox;

class LoginRegisterDlg : public QDialog
{
	Q_OBJECT
public:
	LoginRegisterDlg(QWidget* parent = nullptr);
	~LoginRegisterDlg();
private:
	void initUi();
	QLineEdit* m_accountEdit{};
	QLineEdit* m_passwordEdit{};
	QCheckBox* m_rememberPwdChx{};
	QCheckBox* m_autoLoginChx{};
	QWidget * CreateLoginWidget();

	void showBox();

	QLineEdit* m_regit_accountEdit{};
	QLineEdit* m_regit_passwordEdit{};
	QLineEdit* m_regit_rePasswordEdit{};
	QLineEdit* m_regit_nicknameEdit{};
	QComboBox* m_regit_postCbx{};

	QWidget* CreateRegisterWidget();

public://slots
	void onLogin();
	void onRegister();
signals:
	void getUserId(QString user_id,QString username);
//signals:
//	void registerSuccess(const QString& user_id,const QString& username);
protected:
	void paintEvent(QPaintEvent* ev)override;
private:
	QString m_username;//注册的时候用得到，主要目的是：每次登陆的时候都忘user_privilege里面 “尝试” 插入本条账号
	//若是插入成功则说明，之前并没有该账号，，是通过注册登录进来，因为无法在注册之就向user_privilege添加数据

	QRect m_rightRect;
	QRect m_leftRect;
	QRect m_middleRect;

	QStackedWidget* m_stkWidget{};
	QWidget* m_loginWidget{};
	QWidget* m_registerWidget{};
	bool m_isLogining{};	//是否正在请求登录

	QLabel* m_tipBox{};
	QTimer* m_timer{};
	void setTip(const QString& msg);

	QNetworkAccessManager* m_manager{};
};

#endif // !_LOGINREGISTERDLG_H_
