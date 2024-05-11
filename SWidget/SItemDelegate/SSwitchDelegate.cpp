#include "SSwitchDelegate.h"
#include <QStyleOption>
#include <QCheckBox>
#include <QApplication>
#include <QStyle>
#include <QMouseEvent>
#include <QPainter>
#include <QAbstractItemView>


#define BUTTON_WIDTH  45
#define BUTTON_HEIGHT 20

SSwitchDelegate::SSwitchDelegate(QObject* parent)
	:QStyledItemDelegate(parent)
{
}

void SSwitchDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	painter->save();
	painter->setRenderHint(QPainter::Antialiasing);
	auto state = index.data(Qt::UserRole).toBool();
	//qDebug() << "当前状态 ： "<<state;
	QString text = state ? "正常" : "禁用";
	//文字大小
	int tw = painter->fontMetrics().horizontalAdvance(text);
	int th = painter->fontMetrics().height();

	//文本加按钮整体居中计算
	int hspace = (option.rect.width() - (tw + BUTTON_WIDTH)) / 2;
	int vspace = (option.rect.height() - th) / 2;


	//绘制文本
	painter->drawText(QPoint(option.rect.x() + hspace,option.rect.bottom() - vspace - 2), text);
	//绘制按钮
	QRect btnRect = { option.rect.x() + hspace + tw + 10 ,option.rect.y() + (option.rect.height() - BUTTON_HEIGHT) / 2  ,BUTTON_WIDTH,BUTTON_HEIGHT};
	
	painter->setBrush(state ? QColor(97,162,243) : QColor(136,136,138));
	painter->setPen(Qt::gray);
	//画圆角矩形
	painter->drawRoundedRect(btnRect, btnRect.height() / 2, btnRect.height() / 2);

	
	//小球
	if (option.state & QStyle::State_MouseOver && btnRect.contains(m_mousePos)) {
		painter->setBrush(QColor(232, 232, 232));
	}
	else painter->setBrush(Qt::white);
	painter->setPen(Qt::NoPen);

	if (state) {
		painter->drawEllipse(btnRect.right() - 20, btnRect.y(), 20, 20);
	}
	else {
		painter->drawEllipse(btnRect.left(), btnRect.y(), 20, 20);
	}

	painter->restore();
}

bool SSwitchDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
	if (event->type() == QEvent::MouseButtonRelease) {
		auto state = model->data(index, Qt::UserRole).toInt();
		model->setData(index, !state, Qt::UserRole);
		emit stateChanged(!state, index); 
	}
	else if (event->type() == QEvent::MouseMove) {
		auto ev = dynamic_cast<QMouseEvent*>(event);
		m_mousePos = ev->position().toPoint();
		auto view = dynamic_cast<QAbstractItemView*>(parent());
		if (view)view->viewport()->update(option.rect);
	}
	return false;
}
