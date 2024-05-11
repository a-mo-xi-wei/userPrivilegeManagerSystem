#ifndef SHEADERVIEW_H_
#define SHEADERVIEW_H_

#include <QHeaderView>

class SHeaderView :public QHeaderView
{
    Q_OBJECT
public:
    SHeaderView(Qt::Orientation ori,QWidget* parent = nullptr);
protected:
	void paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const override;
	void mousePressEvent(QMouseEvent* e) override;
	void mouseReleaseEvent(QMouseEvent* e) override;
	bool event(QEvent* ev)override;
signals:
	void stateChanged(int state);
public:
    void setState(int state);
private:
	/** 鼠标左键是否按下 */
	bool  m_bPressed{ false };
	/** 是否选中了 */
	bool  m_bChecked{ false };
	/** 是不是三态复选框 */
	bool  m_bTristate{ false };
	/** 复选框是否处于第三态 */
	bool  m_bNoChange{ false };
	/** 鼠标是否悬停在头item上 */
	bool  m_bHover{ false };
};
#endif // SHEADERVIEW_H_
