#ifndef SMASKWIDGET_H_
#define SMASKWIDGET_H_

#include<QWidget>

class SMaskWidget : public QWidget
{
	Q_OBJECT
public:
	enum PopPosition {		//弹出窗口的位置
		LeadingPosition,	//左边
		MiddlePosition,		//中间
		TrailingPosition	//右边
	};
public:

	SMaskWidget(QWidget* parent = nullptr);

	static SMaskWidget* instance();//蒙层单例
	
	void setMainWidget(QWidget* widget);	//设置主窗口

	void addDialog(QWidget* dialog);		//添加窗口

	void popup(QWidget* dialog,PopPosition position = PopPosition::TrailingPosition);//弹出窗口
protected:
	bool eventFilter(QObject* object, QEvent* ev)override;	//重写事件过滤

	void resizeEvent(QResizeEvent* ev)override;				//重写大小改变

	void onResize();
private:
	QWidget* m_mainWidget{};

	QWidgetSet m_dialogs;

	QWidget* m_currentPopup{};	//当前正弹出的窗口

	PopPosition m_position;
};

#endif // !SMASKWIDGET_H_
