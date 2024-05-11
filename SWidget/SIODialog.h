#ifndef SIODIALOG_H_
#define SIODIALOG_H_

#include<QWidget>
#include<QPushButton>
#include<QLabel>
//该类用于处理基本的输入输出窗口类，像 编辑用户，添加用户。。。

class SIODialog : public QWidget
{
	Q_OBJECT
public:
	SIODialog(QWidget* parent = nullptr);
	//标题设置
	void setTitle(const QString& title);
	//获取标题
	QString title()const;
	//获取中心窗口
	QWidget* centralWidget()const;

	void drawMessageBox(const QString& str,QString title = "ERROR");
private:
	void init();
	//标题栏
	QLabel* m_titleLab{};
	QPushButton* m_closeBtn{};
	//中心窗口
	QWidget* m_centralWidget{};
};

#endif // !SIODIALOG_H_
