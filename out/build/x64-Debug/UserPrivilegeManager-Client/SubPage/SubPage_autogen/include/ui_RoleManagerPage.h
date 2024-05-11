/********************************************************************************
** Form generated from reading UI file 'RoleManagerPage.ui'
**
** Created by: Qt User Interface Compiler version 6.5.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ROLEMANAGERPAGE_H
#define UI_ROLEMANAGERPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RoleManagerPage
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QPushButton *exportBtn;
    QSpacerItem *horizontalSpacer;
    QLineEdit *searchEdit;
    QPushButton *searchBtn;
    QTableView *tableView;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *changeFace2;
    QSpacerItem *horizontalSpacer_2;
    QLabel *pageLabel;
    QPushButton *firstPage;
    QPushButton *prePage;
    QPushButton *nextPage;
    QPushButton *lastPage;
    QSpinBox *turnToBox;
    QPushButton *turnTo;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *RoleManagerPage)
    {
        if (RoleManagerPage->objectName().isEmpty())
            RoleManagerPage->setObjectName("RoleManagerPage");
        RoleManagerPage->resize(736, 458);
        verticalLayout = new QVBoxLayout(RoleManagerPage);
        verticalLayout->setObjectName("verticalLayout");
        widget = new QWidget(RoleManagerPage);
        widget->setObjectName("widget");
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName("horizontalLayout");
        exportBtn = new QPushButton(widget);
        exportBtn->setObjectName("exportBtn");

        horizontalLayout->addWidget(exportBtn);

        horizontalSpacer = new QSpacerItem(347, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        searchEdit = new QLineEdit(widget);
        searchEdit->setObjectName("searchEdit");
        searchEdit->setMinimumSize(QSize(200, 30));
        searchEdit->setMaximumSize(QSize(200, 30));

        horizontalLayout->addWidget(searchEdit);

        searchBtn = new QPushButton(widget);
        searchBtn->setObjectName("searchBtn");
        searchBtn->setMinimumSize(QSize(0, 25));
        searchBtn->setMaximumSize(QSize(16777215, 25));

        horizontalLayout->addWidget(searchBtn);


        verticalLayout->addWidget(widget);

        tableView = new QTableView(RoleManagerPage);
        tableView->setObjectName("tableView");
        tableView->setMinimumSize(QSize(0, 326));
        tableView->setMaximumSize(QSize(16777215, 326));

        verticalLayout->addWidget(tableView);

        widget_2 = new QWidget(RoleManagerPage);
        widget_2->setObjectName("widget_2");
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        changeFace2 = new QPushButton(widget_2);
        changeFace2->setObjectName("changeFace2");
        changeFace2->setMinimumSize(QSize(105, 30));
        changeFace2->setMaximumSize(QSize(105, 30));
        changeFace2->setCursor(QCursor(Qt::PointingHandCursor));

        horizontalLayout_2->addWidget(changeFace2);

        horizontalSpacer_2 = new QSpacerItem(391, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        pageLabel = new QLabel(widget_2);
        pageLabel->setObjectName("pageLabel");
        pageLabel->setMinimumSize(QSize(0, 30));
        pageLabel->setMaximumSize(QSize(16777215, 30));
        QFont font;
        font.setFamilies({QString::fromUtf8("\345\215\216\346\226\207\350\241\214\346\245\267")});
        font.setPointSize(15);
        pageLabel->setFont(font);

        horizontalLayout_2->addWidget(pageLabel);

        firstPage = new QPushButton(widget_2);
        firstPage->setObjectName("firstPage");
        firstPage->setMinimumSize(QSize(40, 30));
        firstPage->setMaximumSize(QSize(40, 30));

        horizontalLayout_2->addWidget(firstPage);

        prePage = new QPushButton(widget_2);
        prePage->setObjectName("prePage");
        prePage->setMinimumSize(QSize(40, 30));
        prePage->setMaximumSize(QSize(40, 30));

        horizontalLayout_2->addWidget(prePage);

        nextPage = new QPushButton(widget_2);
        nextPage->setObjectName("nextPage");
        nextPage->setMinimumSize(QSize(40, 30));
        nextPage->setMaximumSize(QSize(40, 30));

        horizontalLayout_2->addWidget(nextPage);

        lastPage = new QPushButton(widget_2);
        lastPage->setObjectName("lastPage");
        lastPage->setMinimumSize(QSize(40, 30));
        lastPage->setMaximumSize(QSize(40, 30));

        horizontalLayout_2->addWidget(lastPage);

        turnToBox = new QSpinBox(widget_2);
        turnToBox->setObjectName("turnToBox");
        turnToBox->setMinimumSize(QSize(50, 30));
        turnToBox->setMaximumSize(QSize(50, 30));

        horizontalLayout_2->addWidget(turnToBox);

        turnTo = new QPushButton(widget_2);
        turnTo->setObjectName("turnTo");
        turnTo->setMinimumSize(QSize(60, 30));
        turnTo->setMaximumSize(QSize(60, 30));
        turnTo->setCursor(QCursor(Qt::PointingHandCursor));

        horizontalLayout_2->addWidget(turnTo);


        verticalLayout->addWidget(widget_2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        retranslateUi(RoleManagerPage);

        QMetaObject::connectSlotsByName(RoleManagerPage);
    } // setupUi

    void retranslateUi(QWidget *RoleManagerPage)
    {
        RoleManagerPage->setWindowTitle(QCoreApplication::translate("RoleManagerPage", "RoleManagerPage", nullptr));
        exportBtn->setText(QCoreApplication::translate("RoleManagerPage", "\345\257\274\345\207\272\350\241\250\346\240\274", nullptr));
        searchEdit->setPlaceholderText(QCoreApplication::translate("RoleManagerPage", "\350\257\267\350\276\223\345\205\245ID/\347\224\250\346\210\267\345\220\215/\351\202\256\347\256\261/\346\211\213\346\234\272", nullptr));
        searchBtn->setText(QCoreApplication::translate("RoleManagerPage", "\346\220\234\347\264\242", nullptr));
        changeFace2->setText(QCoreApplication::translate("RoleManagerPage", "\344\270\200\351\224\256\346\215\242\350\202\244", nullptr));
        pageLabel->setText(QCoreApplication::translate("RoleManagerPage", "pageLabel", nullptr));
        firstPage->setText(QString());
        prePage->setText(QString());
        nextPage->setText(QString());
        lastPage->setText(QString());
        turnTo->setText(QCoreApplication::translate("RoleManagerPage", "\350\267\263\350\275\254", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RoleManagerPage: public Ui_RoleManagerPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ROLEMANAGERPAGE_H
