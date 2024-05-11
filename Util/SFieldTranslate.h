#ifndef SFIELDTRANSLATE
#define SFIELDTRANSLATE

#include<QString>
#include<QMap>

class SFieldTranslate
{
	SFieldTranslate() = default;
public:
	static SFieldTranslate* instance();
	void addTrans(const QString& field, const QString& trans);
	QString trans(const QString& field);
	QString trans(const QString& group,const QString& field);

private:
	QMap<QString, QString>m_map;
};


#endif // !SFIELDTRANSLATE
