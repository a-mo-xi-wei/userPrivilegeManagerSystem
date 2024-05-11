#include "SFieldTranslate.h"
#include<QDebug>
SFieldTranslate* SFieldTranslate::instance()
{
	static SFieldTranslate sf;
	return &sf;
}

void SFieldTranslate::addTrans(const QString& field, const QString& trans)
{
	auto it = m_map.find(field);
	if (it != m_map.end())qWarning() << "field " << field << " exists!";
	m_map.insert(field, trans);
}

QString SFieldTranslate::trans(const QString& field)
{
	return trans("", field);
}

QString SFieldTranslate::trans(const QString& group, const QString& field)
{
	QString result;
	if (group.isEmpty()) {
		result = field;
	}
	else {
		result = group + "/" + field;
	}
	auto it = m_map.find(result);
	if (it == m_map.end()) {
		qWarning() << result << " not exists!";
		return field;
	}
	return m_map.value(result);
}
