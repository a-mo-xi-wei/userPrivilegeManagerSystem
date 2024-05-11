#include "SCheckDelegate.h"
#include <QStyleOption>
#include <QCheckBox>
#include <QApplication>
#include <QStyle>
#include <QMouseEvent>
#include<QPainter>
SCheckDelegate::SCheckDelegate(QObject* parent)
	:QStyledItemDelegate(parent)
{
}

void SCheckDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	//选中的还是不选中
	bool checkState = index.data(Qt::UserRole).toBool();

	QStyleOptionButton checkBoxStyle;
	QCheckBox cbx;
	//根据cbx初始化
	checkBoxStyle.initFrom(&cbx);
	QRect cbxRect = option.rect;
	cbxRect.setSize(QSize(15, 15));
	//移动到矩形框的中间
	cbxRect.moveCenter(option.rect.center());
	//规定复选框的大小范围
	checkBoxStyle.rect = cbxRect;
	//option.rect = QRect(rect.x() + (rect.width() - CHECK_BOX_LENGTH) + 2, rect.y() + (rect.height() - CHECK_BOX_LENGTH) / 2 + 2, CHECK_BOX_LENGTH, CHECK_BOX_LENGTH);
	checkBoxStyle.state = checkState ? QStyle::State_On : QStyle::State_Off;
	checkBoxStyle.state |= QStyle::State_Enabled;
	//QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkBoxStyle, painter);
	painter->setRenderHint(QPainter::Antialiasing);//抗锯齿
	//绘制复选框
	QApplication::style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &checkBoxStyle, painter);
}


bool SCheckDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
	auto ev = dynamic_cast<QMouseEvent*>(event);
	if (event->type() == QEvent::MouseButtonPress &&
		option.rect.contains(ev->position().toPoint()))
	{
		bool state = !index.data(Qt::UserRole).toBool();
		model->setData(index, state, Qt::UserRole);

		if (state) {
			m_checkCount++;
		}
		else {
			m_checkCount--;
		}

		emit stateChanged(state, index);
	}

	return QStyledItemDelegate::editorEvent(event,model,option,index);
}
