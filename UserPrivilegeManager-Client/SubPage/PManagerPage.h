#ifndef  PMANAGERPAGE_H_
#define  PMANAGERPAGE_H_

#include <QWidget>
#include <QStandardItemModel>
#include <QMenu>

namespace Ui { class PManagerPage; }
class PAddEditDlg;

class PManagerPage : public QWidget
{
	Q_OBJECT
public:
	PManagerPage(QWidget* parent = nullptr);
	~PManagerPage();
	void init();
	void getPrivilegeList();
	void setPageLabel(int curPage, int totalPage, int total);
private:
	void parseJson(const QJsonObject& obj);
private:
	Ui::PManagerPage* ui{};
	QStandardItemModel* m_model{};
	QStringList m_fieldName = { "number","privilege","descr","area"};
	int column(const QString& field);
	QList<QStandardItem*> itemsFromJsonObject(const QJsonObject& jobj);
	int m_curPage;
	int m_totalPage;
	int m_total;
};

#endif //!  USERMANAGERPAGE_H_