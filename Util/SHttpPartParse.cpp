#include"SHttpPartParse.h"
#include <QDebug>

SHttpPartParse::SHttpPartParse(const QByteArray& body)
	:m_body(body)
{
}

QByteArray  SHttpPartParse::header(const QByteArray& key)const
{
	return m_headers.value(key);
}

bool SHttpPartParse::parse()
{
	//查找空行
	auto pos = m_body.indexOf("\r\n\r\n");
	//获取头
	auto header = m_body.mid(0, pos + strlen("\r\n\r\n")).trimmed();
	//拆分头
	auto headers = header.split('\n');
	//获取边界
	auto boundary = headers.at(0).trimmed();
	qDebug() << boundary;
	for (int i = 1; i < headers.size(); i++)
	{
		auto pair = headers.at(i).split(':');
		m_headers.insert(pair.first().trimmed(), pair.last().trimmed());
	}
	auto it = m_headers.find("Content-Disposition");
	if (it == m_headers.end())
		return false;

	auto instructs = it.value().split(';');
	instructs.removeFirst();
	for (const auto& instruct : instructs)
	{
		auto tmp = instruct.split('=');
		const auto& key = tmp.first().trimmed();
		const auto& value = tmp.last().removeIf([](const char& ch) { return ch == '\"'; });
		if (key == "name")
			m_name = value;
		else if (key == "filename")
			m_fileName = value;
	}

	auto endPos = m_body.lastIndexOf(boundary);
	m_data = m_body.mid(pos + 4, endPos - 2 - pos - 4);
	return true;
}
