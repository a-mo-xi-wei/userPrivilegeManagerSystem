#include "SNavBar.h"
#include<QButtonGroup>
#include<QPushButton>
#include<QLabel>
#include<QVBoxLayout>
SNavBar::SNavBar(QWidget* parent)
	:QWidget(parent)
{
	init();
}

void SNavBar::addNav(const QString& text, int id)
{
	addNav(QIcon(), text, id);
}

void SNavBar::addNav(const QIcon& icon, const QString& text, int id)
{
	if (id == -1)id = m_lastId++;
	auto btn = new QPushButton(icon,text);
	btn->setFixedHeight(30);
	btn->setCheckable(true);
	m_buttonGroup->addButton(btn, id);
	m_vlayout->insertWidget(m_vlayout->count()-1,btn);
}

void SNavBar::setNavHeader(const QString& text)
{
	m_header->setText(text);
}
void SNavBar::init()
{
	m_buttonGroup = new QButtonGroup(this);
	m_header = new QLabel;
	m_vlayout = new QVBoxLayout(this);
	m_vlayout->setContentsMargins(0, 0, 0, 0);
	m_vlayout->setSpacing(0);
	m_vlayout->addWidget(m_header);
	m_vlayout->addStretch();
	m_buttonGroup->setExclusive(true);//设置互斥
	m_header->setAlignment(Qt::AlignCenter);
	auto fnt = m_header->font();
	fnt.setBold(true);
	fnt.setPointSize(15);
	m_header->setFont(fnt);

	setAttribute(Qt::WA_StyledBackground);
	
	auto style = "QWidget{background-color:rgb(19,99,119)}\
				  QPushButton,QLabel{border:none;color:white}\
				  QPushButton:hover{background-color:rgb(46,172,130);}\
				  QPushButton:checked{background-color:rgb(105,4,64);}";
	setStyleSheet(style);

	connect(m_buttonGroup, &QButtonGroup::idClicked, this, &SNavBar::idClicked);
	connect(m_buttonGroup, &QButtonGroup::buttonClicked, this, &SNavBar::buttonClicked);

}
