#ifndef SHTTPPARTPARSE_H_
#define  SHTTPPARTPARSE_H_

#include<QMap>

class SHttpPartParse
{
public:
	SHttpPartParse(const QByteArray& body);
	bool parse();
	QByteArray header(const QByteArray& key)const;
	const QByteArray& name()const { return m_name; }
	const QByteArray& fileName()const { return m_fileName; }
	const QByteArray& data()const { return m_data; }
private:
	QByteArray m_body;
	QByteArray m_data;
	QMap<QByteArray, QByteArray> m_headers;
	QByteArray m_name;
	QByteArray m_fileName;
};


#endif //! SHTTPPARTPARSE_H_
