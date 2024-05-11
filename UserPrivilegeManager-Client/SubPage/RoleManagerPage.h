#ifndef ROLEMANAGERPAGE_H_
#define ROLEMANAGERPAGE_H_

#include<QWidget>
#include<QStandardItemModel>
#include<set>
namespace Ui { class RoleManagerPage; }
class UserDetailsDlg;
class UserAddDlg;
class UserPrivilegeDlg;

class RoleManagerPage : public QWidget
{
	Q_OBJECT
public:
	RoleManagerPage(const QString& user_id, const QString& username,QWidget* parent = nullptr)  ;
	~RoleManagerPage();
	void initPrivilege();
	void init();
	void getNewInfor(const QString& user_id, const QString& username);

	QJsonObject user(int row)const;
public:
	//void getAccount(const QString& user_id, const QString& username);
	void onSearch();
	void setCurrentPage(int page);
	void setPageLabel(int curPage, int totalPage, int total);
private:
	void parseJson(const QJsonObject& obj);
	void drawMessageBox(const QString& text);
public:
	void setSearchLine(const QString& text);
private:
	QString m_styleStr;
	Ui::RoleManagerPage* ui{};

	QStandardItemModel* m_model{};

	std::set<int> m_checkSet;

	QStringList m_fieldName = {"user_id","username","privilege_read","privilege_edit","privilege_add","privilege_delete"};
	
	QMenu* m_faceMenu{};
	void initMenu();
	int column(const QString& field)const;

	QList<QStandardItem*>itemsFromJsonObject(const QJsonObject& jobj);
	void onExport();
	
	void writeCSVFile(const QString& filename);
	void writeXLSXFile(const QString& filename);

	int m_curPage;
	int m_totalPage;
	int m_total;

	//当前账户信息
	QString m_user_id;
	QString m_username;

	//权限
	int m_privilege_edit;
	int m_privilege_add;
	int m_privilege_delete;

};

#endif // !USERMANAGER_H_
