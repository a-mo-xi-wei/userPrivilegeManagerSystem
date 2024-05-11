/********************************************************************************
** Form generated from reading UI file 'UserManagerPage.ui'
**
** Created by: Qt User Interface Compiler version 6.5.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_USERMANAGERPAGE_H
#define UI_USERMANAGERPAGE_H

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

class Ui_UserManagerPage
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QPushButton *addUserBtn;
    QPushButton *exportBtn;
    QSpacerItem *HSpacer1;
    QLineEdit *searchEdit;
    QPushButton *searchBtn;
    QTableView *tableView;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *batchEnableBtn;
    QPushButton *batchDisableBtn;
    QPushButton *batchDeleteBtn;
    QSpacerItem *HSpacer2;
    QLabel *pageLabel;
    QPushButton *firstPage;
    QPushButton *prePage;
    QPushButton *nextPage;
    QPushButton *lastPage;
    QSpinBox *turnToBox;
    QPushButton *turnTo;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *changeFace;
    QSpacerItem *HSpacer3;
    QPushButton *backLoginBtn;

    void setupUi(QWidget *UserManagerPage)
    {
        if (UserManagerPage->objectName().isEmpty())
            UserManagerPage->setObjectName("UserManagerPage");
        UserManagerPage->resize(736, 540);
        verticalLayout = new QVBoxLayout(UserManagerPage);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, -1, -1, -1);
        widget = new QWidget(UserManagerPage);
        widget->setObjectName("widget");
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(-1, -1, -1, 9);
        addUserBtn = new QPushButton(widget);
        addUserBtn->setObjectName("addUserBtn");
        addUserBtn->setMinimumSize(QSize(105, 0));
        addUserBtn->setMaximumSize(QSize(105, 16777215));

        horizontalLayout->addWidget(addUserBtn);

        exportBtn = new QPushButton(widget);
        exportBtn->setObjectName("exportBtn");

        horizontalLayout->addWidget(exportBtn);

        HSpacer1 = new QSpacerItem(347, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(HSpacer1);

        searchEdit = new QLineEdit(widget);
        searchEdit->setObjectName("searchEdit");
        searchEdit->setMinimumSize(QSize(200, 30));
        searchEdit->setMaximumSize(QSize(200, 30));

        horizontalLayout->addWidget(searchEdit);

        searchBtn = new QPushButton(widget);
        searchBtn->setObjectName("searchBtn");
        searchBtn->setMinimumSize(QSize(0, 25));
        searchBtn->setMaximumSize(QSize(16777215, 25));
        QFont font;
        font.setBold(true);
        font.setItalic(false);
        searchBtn->setFont(font);

        horizontalLayout->addWidget(searchBtn);


        verticalLayout->addWidget(widget);

        tableView = new QTableView(UserManagerPage);
        tableView->setObjectName("tableView");
        tableView->setMinimumSize(QSize(0, 326));
        tableView->setMaximumSize(QSize(16777215, 326));

        verticalLayout->addWidget(tableView);

        widget_2 = new QWidget(UserManagerPage);
        widget_2->setObjectName("widget_2");
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        batchEnableBtn = new QPushButton(widget_2);
        batchEnableBtn->setObjectName("batchEnableBtn");
        batchEnableBtn->setMinimumSize(QSize(0, 30));
        batchEnableBtn->setMaximumSize(QSize(16777215, 30));

        horizontalLayout_2->addWidget(batchEnableBtn);

        batchDisableBtn = new QPushButton(widget_2);
        batchDisableBtn->setObjectName("batchDisableBtn");
        batchDisableBtn->setMinimumSize(QSize(0, 30));
        batchDisableBtn->setMaximumSize(QSize(16777215, 30));

        horizontalLayout_2->addWidget(batchDisableBtn);

        batchDeleteBtn = new QPushButton(widget_2);
        batchDeleteBtn->setObjectName("batchDeleteBtn");
        batchDeleteBtn->setMinimumSize(QSize(0, 30));
        batchDeleteBtn->setMaximumSize(QSize(16777215, 30));

        horizontalLayout_2->addWidget(batchDeleteBtn);

        HSpacer2 = new QSpacerItem(391, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(HSpacer2);

        pageLabel = new QLabel(widget_2);
        pageLabel->setObjectName("pageLabel");
        pageLabel->setMinimumSize(QSize(0, 30));
        pageLabel->setMaximumSize(QSize(16777215, 30));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("\345\215\216\346\226\207\350\241\214\346\245\267")});
        font1.setPointSize(15);
        pageLabel->setFont(font1);

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

        horizontalLayout_2->addWidget(turnTo);


        verticalLayout->addWidget(widget_2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        changeFace = new QPushButton(UserManagerPage);
        changeFace->setObjectName("changeFace");
        changeFace->setMinimumSize(QSize(105, 30));
        changeFace->setMaximumSize(QSize(105, 30));
        changeFace->setCursor(QCursor(Qt::PointingHandCursor));
        changeFace->setAutoDefault(false);
        changeFace->setFlat(false);

        horizontalLayout_3->addWidget(changeFace);

        HSpacer3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(HSpacer3);

        backLoginBtn = new QPushButton(UserManagerPage);
        backLoginBtn->setObjectName("backLoginBtn");
        backLoginBtn->setMinimumSize(QSize(80, 40));
        backLoginBtn->setMaximumSize(QSize(80, 40));
        backLoginBtn->setCursor(QCursor(Qt::PointingHandCursor));

        horizontalLayout_3->addWidget(backLoginBtn);


        verticalLayout->addLayout(horizontalLayout_3);


        retranslateUi(UserManagerPage);

        QMetaObject::connectSlotsByName(UserManagerPage);
    } // setupUi

    void retranslateUi(QWidget *UserManagerPage)
    {
        UserManagerPage->setWindowTitle(QCoreApplication::translate("UserManagerPage", "UserManagerPage", nullptr));
        addUserBtn->setText(QCoreApplication::translate("UserManagerPage", "\346\267\273\345\212\240\347\224\250\346\210\267", nullptr));
        exportBtn->setText(QCoreApplication::translate("UserManagerPage", "\345\257\274\345\207\272\350\241\250\346\240\274", nullptr));
        searchEdit->setPlaceholderText(QCoreApplication::translate("UserManagerPage", "\350\257\267\350\276\223\345\205\245ID/\347\224\250\346\210\267\345\220\215/\351\202\256\347\256\261/\346\211\213\346\234\272", nullptr));
        searchBtn->setText(QCoreApplication::translate("UserManagerPage", "\346\220\234\347\264\242", nullptr));
        batchEnableBtn->setText(QCoreApplication::translate("UserManagerPage", "\346\211\271\351\207\217\345\220\257\347\224\250", nullptr));
        batchDisableBtn->setText(QCoreApplication::translate("UserManagerPage", "\346\211\271\351\207\217\347\246\201\347\224\250", nullptr));
        batchDeleteBtn->setText(QCoreApplication::translate("UserManagerPage", "\346\211\271\351\207\217\345\210\240\351\231\244", nullptr));
        pageLabel->setText(QCoreApplication::translate("UserManagerPage", "pageLabel", nullptr));
        firstPage->setText(QString());
        prePage->setText(QString());
        nextPage->setText(QString());
        lastPage->setText(QString());
        turnTo->setText(QCoreApplication::translate("UserManagerPage", "\350\267\263\350\275\254", nullptr));
        changeFace->setText(QCoreApplication::translate("UserManagerPage", "\344\270\200\351\224\256\346\215\242\350\202\244", nullptr));
        backLoginBtn->setText(QCoreApplication::translate("UserManagerPage", "\350\277\224\345\233\236\347\231\273\345\275\225", nullptr));
    } // retranslateUi

};

namespace Ui {
    class UserManagerPage: public Ui_UserManagerPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USERMANAGERPAGE_H
