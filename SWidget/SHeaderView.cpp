#include "SHeaderView.h"
#include <QPainter>
#include <QMouseEvent>
#include <QCheckBox>
#define CHECK_BOX_COLUMN 0
#define CHECK_BOX_LENGTH 15

SHeaderView::SHeaderView(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation, parent),
      m_bPressed(false),
      m_bChecked(false),
      m_bTristate(false),
      m_bNoChange(false),
	  m_bHover(false)
{
    // setStretchLastSection(true);
    //setStyleSheet("background-color:#C0C0C0");
    setHighlightSections(false);
    setMouseTracking(true);
    // 响应鼠标
    setSectionsClickable(true);
}

// 槽函数，用于更新复选框状态
void SHeaderView::setState(int state)
{
    if (state == Qt::PartiallyChecked) {
        m_bTristate = true;
        m_bNoChange = true;
    } else {
        m_bNoChange = false;
    }

    m_bChecked = (state != Qt::Unchecked);
	viewport()->update();
}

//绘制复选框
void SHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();
    painter->setRenderHint(QPainter::Antialiasing);
    //painter->fillRect(rect,QColor("#C0C0C0"));
    if (logicalIndex == CHECK_BOX_COLUMN)
    {
        QStyleOptionButton option;
        
        //QCheckBox checkBox;
        option.initFrom(this);
        QRect checkBoxRect = rect; 
        checkBoxRect.setSize(QSize(CHECK_BOX_LENGTH, CHECK_BOX_LENGTH));
        checkBoxRect.moveCenter(rect.center());
        option.rect = checkBoxRect;
        if (m_bChecked)
            option.state |= QStyle::State_Sunken;
         
        if (m_bTristate && m_bNoChange)
            option.state |= QStyle::State_NoChange;
        else
            option.state |= m_bChecked ? QStyle::State_On : QStyle::State_Off;
        if (testAttribute(Qt::WA_Hover) && underMouse()) {
            if (m_bHover)
                option.state |= QStyle::State_MouseOver;
            else
                option.state &= ~QStyle::State_MouseOver;
        }

        //绘制复选框
        //style()->drawControl(QStyle::CE_CheckBox, &option, painter, this);
        style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &option, painter, this);
    }
}


// 鼠标按下表头
void SHeaderView::mousePressEvent(QMouseEvent *event)
{
    int nColumn = QHeaderView::logicalIndexAt(event->pos());
    if ((event->buttons() & Qt::LeftButton) && (nColumn == CHECK_BOX_COLUMN))
    {
        m_bPressed = true;
    }
    else
    {
        QHeaderView::mousePressEvent(event);
    }
}

// 鼠标从表头释放，发送信号，更新model数据
void SHeaderView::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_bPressed)
    {
        if (m_bTristate && m_bNoChange)
        {
            m_bChecked = true;
            m_bNoChange = false;
        }
        else
        {
            m_bChecked = !m_bChecked;
        }

        viewport()->update();

        Qt::CheckState state = m_bChecked ? Qt::Checked : Qt::Unchecked;

        emit stateChanged(state);
    }
    else
    {
        QHeaderView::mouseReleaseEvent(event);
    }

    m_bPressed = false;
}

// 鼠标滑过、离开，更新复选框状态
bool SHeaderView::event(QEvent *event)
{
    updateSection(0);
    if (event->type() == QEvent::Enter)
    {
		//QMouseEvent* pEvent = dynamic_cast<QMouseEvent*>(event);
        //此处一定不能转成QMouseEvent，原因在于它们与原始事件类型的关系以及在特定事件场景下的适用性。
        //QEvent::Enter 本身并不是 QMouseEvent 的实例。它是一个单独的事件类型，由 QEnterEvent 类来表示。
        QEnterEvent* pEvent = dynamic_cast<QEnterEvent*>(event);
        int nColumn = logicalIndexAt(pEvent->position().toPoint());
        if (nColumn == CHECK_BOX_COLUMN)
        {
            m_bHover = true;

            return true;
        }
    }
    else if (event->type() == QEvent::Leave)
    {
        m_bHover = false;
        return true;
    }

    return QHeaderView::event(event);
}


