#include "SMaskWidget.h"
#include<QEvent>
#include<QKeyEvent>
SMaskWidget::SMaskWidget(QWidget* parent)
	:QWidget(parent)
	,m_position(SMaskWidget::PopPosition::TrailingPosition)
{
	this->setStyleSheet("background-color:rgba(194,195,201,180)");
	//指示窗口应该使用样式背景绘制
	this->setAttribute(Qt::WA_StyledBackground);
}

SMaskWidget* SMaskWidget::instance()
{
	static SMaskWidget* ins;
	if (!ins)ins = new SMaskWidget;
	return ins;
}

void SMaskWidget::setMainWidget(QWidget* widget)
{
	if (!widget) {
		qWarning() << "setMainWidget : widget is nullptr!";
		return;
	}
	if(m_mainWidget)qWarning() << "m_mainWidget already exists , will be changed ! ";
	m_mainWidget = widget;
	//将蒙层的父亲设为主窗口
	this->setParent(m_mainWidget);
	//隐藏蒙层
	this->hide();
	//主窗口安装事件过滤器
	m_mainWidget->installEventFilter(this);
}

void SMaskWidget::addDialog(QWidget* dialog)
{
	if (!dialog) {
		qWarning() << "addDialog : dialog is nullptr!";
		return;
	}
	auto it = m_dialogs.find(dialog);
	if (it != m_dialogs.end()) {
		qWarning() << "addDialog : dialog exists!";
		return;
	}
	m_dialogs.insert(dialog);//防止重复包含
	//将对话框（widget）安装事件过滤器
	dialog->installEventFilter(this);
}

void SMaskWidget::popup(QWidget* dialog, PopPosition position)
{
	addDialog(dialog);
	if (dialog && m_dialogs.contains(dialog)) {
		m_position = position;
		//设置当前弹出窗口为 dialog
		m_currentPopup = dialog;
		//将蒙层设置为父亲，即当前窗口出现在蒙层之上
		dialog->setParent(this);
		//展示蒙层
		this->show();
		dialog->setFocus();//获取焦点
		//显示窗口
		dialog->show();
	}
}

bool SMaskWidget::eventFilter(QObject* object, QEvent* ev)
{
	//如果采用事件过滤的是主窗口
	if (object == m_mainWidget) {
		//蒙层的大小随主窗口的大小而变化（相同）
		if (ev->type() == QEvent::Resize) {
			this->setGeometry(m_mainWidget->rect());
		}
	}
	//弹出窗
	else {
		if (ev->type() == QEvent::Hide) {
			this->hide();
		}
		else if (ev->type() == QEvent::Resize) {
			onResize();
		}
		else if (ev->type() == QEvent::KeyPress) {
			auto kev = static_cast<QKeyEvent*>(ev);	
			if (kev->key() == Qt::Key_Escape) {
				//按ESC隐藏
				this->hide();
			}
		}
	}
	return false;
}

void SMaskWidget::resizeEvent(QResizeEvent* ev)
{
	onResize();
}

void SMaskWidget::onResize()
{
	if (m_currentPopup) {
		switch (m_position) {//不同的方向，不同的位置
		case SMaskWidget::PopPosition::LeadingPosition: 
		{
			m_currentPopup->move(0, 0);
			m_currentPopup->setFixedHeight(this->height());
			break;
		}
		case SMaskWidget::PopPosition::MiddlePosition:
		{
			int x = (this->width() - m_currentPopup->width()) / 2;
			int y = (this->height() - m_currentPopup->height()) / 2;
			m_currentPopup->move(x,y);
			break;
		}
		case SMaskWidget::PopPosition::TrailingPosition:
		{
			m_currentPopup->move(this->width() - m_currentPopup->width(), 0);
			m_currentPopup->setFixedHeight(this->height());
			break;
		}
		}
	}
}

