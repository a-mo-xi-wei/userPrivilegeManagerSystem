#include "SIODialog.h"
#include "SStandardPixmap.h"
#include<QButtonGroup>

#include<QVBoxLayout>
#include<QApplication>
#include <QMessageBox>
SIODialog::SIODialog(QWidget* parent)
	:QWidget(parent)
	,m_centralWidget(new QWidget)
{
	init();
	//设置默认名称为应用程序名称
	setTitle(QApplication::applicationDisplayName());
}

void SIODialog::setTitle(const QString& title)
{
	//设置标题
	m_titleLab->setText(title);
}

QString SIODialog::title() const
{
	//获取标题
	return m_titleLab->text();
}

QWidget* SIODialog::centralWidget() const
{
	//获取中心窗口
	return m_centralWidget;
}

void SIODialog::drawMessageBox(const QString& str, QString title)
{
	QMessageBox box(this);
	box.setWindowTitle(title);
	box.setText(str);
	box.setIcon(QMessageBox::Warning);
	box.setStyleSheet("QMessageBox {\
			  background-color: #99FFFF;\
			  color: black;\
			  font-family: 'JetBrains Mono';\
			  font-size: 12px;\
			}\
			QPushButton {\
			  background-color: #4CAF50;\
			  color: white;\
			  padding: 8px 16px;\
			  border: none;\
			  border-radius: 4px;\
			}\
			QPushButton:hover {\
			  background-color: #45a049;\
			}");
	box.exec();
}


void SIODialog::init()
{

	m_titleLab = new QLabel;
	m_closeBtn = new QPushButton;
	//标题样式
	m_titleLab->setStyleSheet("font:bold 13px;color:#66CC33");

	QString styleStr = 
		"QPushButton{\
			background-color:#CCFFFF;\
			border:none;\
			padding:5px;\
			border-radius: 10px;\
		}\
		QPushButton:hover{background-color:#99FFFF}";
	m_closeBtn->setStyleSheet(styleStr);
	m_closeBtn->setIcon(SStandardPixmap::pixmap(SStandardPixmap::SP_TitleBarCloseButton));

	connect(m_closeBtn, &QPushButton::clicked, this, [=] {close(); });

	QWidget* titleBar = new QWidget;
	titleBar->setStyleSheet(".QWidget{border-bottom:1px solid rgb(105,4,64)}");
	
	auto titleLayout = new QHBoxLayout(titleBar);
	titleLayout->addWidget(m_titleLab);
	titleLayout->addStretch();
	titleLayout->addWidget(m_closeBtn);
	
	auto vlayout = new QVBoxLayout(this);
	vlayout->setContentsMargins(0, 0, 0, 0);
	vlayout->addWidget(titleBar,0,Qt::AlignTop);
	vlayout->addWidget(centralWidget());
	//vlayout->addStretch();

}
