#ifndef USERMANAGER_H_
#define USERMANAGER_H_

#include<QWidget>
#include<QStandardItemModel>
#include<set>
#include<QMenu>
namespace Ui { class UserManagerPage; }
class UserDetailsDlg;
class UserAddDlg;
class UserPrivilegeDlg;

class UserManagerPage : public QWidget
{
	Q_OBJECT
public:
	UserManagerPage(const QString& user_id,const QString& username,QWidget* parent = nullptr);
	~UserManagerPage();
	void init();
	void initPrivilege();
	void getNewInfor(const QString& user_id, const QString& username);

	QJsonObject user(int row)const;
protected:
	void resizeEvent(QResizeEvent* ev)override;

public:
	void onSearch();
	void setBatchEnable(int op);
	void onBatchEnable();
	void onBatchDisable();
	void onBatchDelete();
	void setCurrentPage(int page);
	void setPageLabel(int curPage, int totalPage, int total);
	
private:
	void parseJson(const QJsonObject& obj);
	void drawMessageBox(const QString& text);
signals:
	void finduser(const QString& user_id);
	void backLogin();
private:
	QString m_styleStr;
	Ui::UserManagerPage* ui{};

	QStandardItemModel* m_model{};
	std::set<int> m_checkSet;

	QStringList m_fieldName = { "","user_id","username","gender","mobile","email","isEnable","操作" };
	int column(const QString& field)const;
	QList<QStandardItem*>itemsFromJsonObject(const QJsonObject& jobj);

	UserDetailsDlg* m_detailsDlg{};
	UserAddDlg* m_userAddDlg{};
	UserPrivilegeDlg* m_userPDlg{};

	QMenu* m_importMenu{};
	void initMenu();
	void singleImport();
	void batchImport();
	void onExport();

	void readCSVFile(const QString& filename);
	void readXLSXFile(const QString& filename);
	
	void writeCSVFile(const QString& filename);
	void writeXLSXFile(const QString& filename);

	int m_curPage;
	int m_totalPage;
	int m_total;
	
	QMenu* m_faceMenu{};

	//当前账户信息
	QString m_user_id;
	QString m_username;

	//权限	//通过MainWindow传递进来
	int m_privilege_edit;
	int m_privilege_add;
	int m_privilege_delete;

	
};

#endif // !USERMANAGER_H_
