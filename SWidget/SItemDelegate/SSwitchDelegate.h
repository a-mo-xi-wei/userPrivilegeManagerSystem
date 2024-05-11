#ifndef SSWITCHDELEGATE_H_
#define  SSWITCHDELEGATE_H_

#include <QStyledItemDelegate>

class SSwitchDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	SSwitchDelegate(QObject* parnet = nullptr);
protected:
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;

signals:
	void stateChanged(bool state, const QModelIndex& index);

private:
	QPoint m_mousePos;
};

#endif //! SSWITCHDELEGATE_H_
