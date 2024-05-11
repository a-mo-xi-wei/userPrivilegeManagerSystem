#ifndef SCHECKDELEGATE_H_
#define  SCHECKDELEGATE_H_

#include <QStyledItemDelegate>
#include<set>
class SCheckDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	SCheckDelegate(QObject* parnet = nullptr);
	size_t checkCount()const { return m_checkCount; }
	void setCheckCount(size_t count) { m_checkCount = count; }
protected:
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;
signals:
	void stateChanged(int state, const QModelIndex& index);
private:
	size_t m_checkCount{};		//选中的item的数量
};

#endif //! SCHECKDELEGATE_H_
