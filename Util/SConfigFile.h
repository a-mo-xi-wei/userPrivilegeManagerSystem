#ifndef SCONFIGFILE_H_
#define  SCONFIGFILE_H_

#include <QSettings>
#include <QAnyStringView>

class SConfigFile
{
public:
	SConfigFile(const QString& filename);
	bool isOpen()const { return m_settings.status() == QSettings::NoError; }

	void setValue(QAnyStringView key, const QVariant& value);
	QVariant value(QAnyStringView key)const;
	QVariant value(QAnyStringView key,const QVariant& def_value)const;
	void removeValue(QAnyStringView key);
private:
	QSettings m_settings;
};

#endif // !SCONFIGFILE_H_
