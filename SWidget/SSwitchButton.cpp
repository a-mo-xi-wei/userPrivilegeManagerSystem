#include "SSwitchButton.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>

SSwitchButton::SSwitchButton(QWidget* parent)
	:QWidget(parent),
	m_nHeight(16),
	m_bState(false),
	m_radius(10.0),
	m_nMargin(3),
	m_checkedColor(0, 150, 136),
	m_thumbColor(Qt::white),
	m_disabledColor(190, 190, 190),
	m_background(Qt::black)
{
	// 鼠标滑过光标形状 - 手型
    setCursor(Qt::PointingHandCursor);

    // 连接信号槽
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

// 绘制开关
void SSwitchButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    QColor background;
    QColor thumbColor;
    qreal dOpacity;
    if (isEnabled()) { // 可用状态
        if (m_bState) { // 打开状态
            background = m_checkedColor;
            thumbColor = m_checkedColor;
            dOpacity = 0.600;
        } else { //关闭状态
            background = m_background;
            thumbColor = m_thumbColor;
            dOpacity = 0.800;
        }
    } else {  // 不可用状态
        background = m_background;
        dOpacity = 0.260;
        thumbColor = m_disabledColor;
    }
    // 绘制大椭圆
    painter.setBrush(background);
    painter.setOpacity(dOpacity);
    path.addRoundedRect(QRectF(m_nMargin, m_nMargin, width() - 2 * m_nMargin, height() - 2 * m_nMargin), m_radius, m_radius);
    painter.drawPath(path.simplified());

    // 绘制小椭圆
    painter.setBrush(thumbColor);
    painter.setOpacity(1.0);
    painter.drawEllipse(QRectF(m_nX - (m_nHeight / 2), m_nY - (m_nHeight / 2), height(), height()));
}

// 鼠标按下事件
void SSwitchButton::mousePressEvent(QMouseEvent *event)
{
    if (isEnabled()) {
        if (event->buttons() & Qt::LeftButton) {
            event->accept();
        } else {
            event->ignore();
        }
    }
}

// 鼠标释放事件 - 切换开关状态、发射stateChanged()信号
void SSwitchButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (isEnabled()) {
        if ((event->type() == QMouseEvent::MouseButtonRelease) && (event->button() == Qt::LeftButton)) {
            event->accept();
            m_bState = !m_bState;
            emit stateChanged(m_bState);
            m_timer.start(10);
        } else {
            event->ignore();
        }
    }
}

// 大小改变事件
void SSwitchButton::resizeEvent(QResizeEvent *event)
{
    m_nX = m_nHeight / 2;
    m_nY = m_nHeight / 2;
    QWidget::resizeEvent(event);
}

// 默认大小
QSize SSwitchButton::sizeHint() const
{
    return minimumSizeHint();
}

// 最小大小
QSize SSwitchButton::minimumSizeHint() const
{
    return QSize(2 * (m_nHeight + m_nMargin)+20, m_nHeight + 2 * m_nMargin);
}

// 切换状态 - 滑动
void SSwitchButton::onTimeout()
{
    if (m_bState) {
        m_nX += 1;
        if (m_nX >= width() - m_nHeight)
            m_timer.stop();
    } else {
        m_nX -= 1;
        if (m_nX <= m_nHeight / 2)
            m_timer.stop();
    }
    update();
}

// 返回开关状态 - 打开：true 关闭：false
bool SSwitchButton::isToggled() const
{
    return m_bState;
}

// 设置开关状态
void SSwitchButton::setToggle(bool checked)
{
    m_bState = checked;
    m_timer.start(10);
}

// 设置背景颜色
void SSwitchButton::setBackgroundColor(QColor color)
{
    m_background = color;
}

// 设置选中颜色
void SSwitchButton::setCheckedColor(QColor color)
{
    m_checkedColor = color;
}

// 设置不可用颜色
void SSwitchButton::setDisbaledColor(QColor color)
{
    m_disabledColor = color;
}

