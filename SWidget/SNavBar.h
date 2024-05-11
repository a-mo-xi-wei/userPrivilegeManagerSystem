#ifndef SNAVBAR_H_
#define SNAVBAR_H_

#include<QWidget>
class QAbstractButton;
class QLabel;
class QButtonGroup;
class QVBoxLayout;
class SNavBar : public QWidget
{
	Q_OBJECT
public:
	SNavBar(QWidget* parent = nullptr);
	void addNav(const QString& text, int id = -1);
	void addNav(const QIcon& icon, const QString& text, int id = -1);
	void setNavHeader(const QString& text);

signals:
	void idClicked(int id);
signals:
	void buttonClicked(QAbstractButton* button);

private:
	void init();
	QVBoxLayout* m_vlayout{};
	QLabel* m_header{};
	QButtonGroup* m_buttonGroup{};
	int m_lastId{ 0 };
};

#endif // !SNAVBAR_H_
