#include "SUrlDelegate.h"
#include <QStyleOption>
#include <QCheckBox>
#include <QApplication>
#include <QStyle>
#include <QMouseEvent>
#include <QPainter>
SUrlDelegate::SUrlDelegate(QObject* parent)
	:QStyledItemDelegate(parent)
{

}

void SUrlDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
	painter->save();
	
	//设置文本颜色
	painter->setPen(option.state & QStyle::State_MouseOver ? Qt::red : QColor("#00CCFF"));

	//设置字体
	auto fnt = painter->font();
	fnt.setUnderline(option.state & QStyle::State_MouseOver);
	painter->setFont(fnt);

	//绘制文本
	painter->drawText(option.rect, index.data().toString(),QTextOption(Qt::AlignCenter));

	painter->restore();
}
bool SUrlDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) {
	if (event->type() == QEvent::MouseButtonRelease) {
		emit requestOpenUrl(index);
	}

	return QStyledItemDelegate::editorEvent(event, model, option, index);
}

