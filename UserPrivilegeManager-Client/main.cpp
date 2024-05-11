#include "SApp.h"
#include "LoginRegisterDlg.h"
#include "MainWindow.h"

int main(int argc, char* argv[])
{
	SApp a(argc, argv);
	QString m_user_id;
	QString m_username;
	LoginRegisterDlg login;
	QObject::connect(&login, &LoginRegisterDlg::getUserId, [&](QString account,QString username)//此处要用 & 捕获，若是用值捕获传递进来的会是const
		{
			m_user_id = account;
			m_username = username;
		});
	if (login.exec() == QDialog::Rejected) { return 0; }

	MainWindow w(m_user_id,m_username);

	QObject::connect(&w, &MainWindow::backLogin, [&]
		{
			LoginRegisterDlg reLogin;
			QObject::connect(&reLogin, &LoginRegisterDlg::getUserId, [&](QString account, QString username)//此处要用 & 捕获，若是用值捕获传递进来的会是const
				{
					m_user_id = account;
					m_username = username;
				});
			if (reLogin.exec() == QDialog::Rejected) { return 0; }
			w.getNewInfor(m_user_id, m_username);
			
		});
	w.show();

	return a.exec();
}
