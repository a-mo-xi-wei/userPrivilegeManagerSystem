#ifndef SBUTTONDELEGATE_H_
#define  SBUTTONDELEGATE_H_

#include <QStyledItemDelegate>
class QAbstractButton;
class SButtonDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	SButtonDelegate(QObject* parnet = nullptr);
	QAbstractButton* addButton(QAbstractButton* button);
	QAbstractButton* button(int id);

protected:
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;
signals:
	void buttonClicked(int id, const QModelIndex& index);
private:
	std::vector<std::unique_ptr<QAbstractButton>>m_buttons;
	int m_spacing{ 6 };
	QPoint m_mousePos;
	int buttonTotalWidth(size_t count = -1)const;
};

#endif //! SBUTTONDELEGATE_H_
