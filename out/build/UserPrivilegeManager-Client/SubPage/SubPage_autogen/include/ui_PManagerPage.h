/********************************************************************************
** Form generated from reading UI file 'PManagerPage.ui'
**
** Created by: Qt User Interface Compiler version 6.5.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PMANAGERPAGE_H
#define UI_PMANAGERPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PManagerPage
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QLabel *titleLabel;
    QTableView *tableView;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *pageLabel;

    void setupUi(QWidget *PManagerPage)
    {
        if (PManagerPage->objectName().isEmpty())
            PManagerPage->setObjectName("PManagerPage");
        PManagerPage->resize(707, 436);
        verticalLayout = new QVBoxLayout(PManagerPage);
        verticalLayout->setObjectName("verticalLayout");
        widget = new QWidget(PManagerPage);
        widget->setObjectName("widget");
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName("horizontalLayout");
        titleLabel = new QLabel(widget);
        titleLabel->setObjectName("titleLabel");
        titleLabel->setMinimumSize(QSize(0, 50));
        titleLabel->setMaximumSize(QSize(16777215, 50));
        QFont font;
        font.setFamilies({QString::fromUtf8("\344\273\277\345\256\213")});
        font.setPointSize(40);
        font.setBold(false);
        font.setKerning(true);
        font.setStyleStrategy(QFont::PreferAntialias);
        titleLabel->setFont(font);
        titleLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(titleLabel);


        verticalLayout->addWidget(widget);

        tableView = new QTableView(PManagerPage);
        tableView->setObjectName("tableView");
        tableView->setMinimumSize(QSize(0, 300));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("\346\245\267\344\275\223")});
        font1.setPointSize(12);
        tableView->setFont(font1);

        verticalLayout->addWidget(tableView);

        widget_2 = new QWidget(PManagerPage);
        widget_2->setObjectName("widget_2");
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        pageLabel = new QLabel(widget_2);
        pageLabel->setObjectName("pageLabel");
        pageLabel->setBaseSize(QSize(0, 0));
        QFont font2;
        font2.setFamilies({QString::fromUtf8("\345\215\216\346\226\207\350\241\214\346\245\267")});
        font2.setPointSize(13);
        pageLabel->setFont(font2);
        pageLabel->setLayoutDirection(Qt::LeftToRight);
        pageLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        pageLabel->setMargin(4);

        horizontalLayout_2->addWidget(pageLabel);


        verticalLayout->addWidget(widget_2);


        retranslateUi(PManagerPage);

        QMetaObject::connectSlotsByName(PManagerPage);
    } // setupUi

    void retranslateUi(QWidget *PManagerPage)
    {
        PManagerPage->setWindowTitle(QCoreApplication::translate("PManagerPage", "Form", nullptr));
        titleLabel->setText(QCoreApplication::translate("PManagerPage", "\346\235\203\351\231\220\345\210\227\350\241\250", nullptr));
        pageLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class PManagerPage: public Ui_PManagerPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PMANAGERPAGE_H
