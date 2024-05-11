
#ifndef EVENTFILTEROBJECT_H
#define EVENTFILTEROBJECT_H

#include <QObject>
#include <QPoint>

class   SEventFilterObject : public QObject
{
    Q_OBJECT
public:
    //防止编译器进行隐式转换,必须显式调用构造函数,这样可以更清晰地表达创建对象的意图,提高代码的可读性
    explicit SEventFilterObject(QObject *parent = nullptr);

    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QPoint m_pressPos;       //左键点击的坐标
    bool m_isMoving{false}; //是否正在移动
};

#endif // EVENTFILTEROBJECT_H
