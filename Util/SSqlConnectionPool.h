#ifndef SSQLCONNECTIONPOOL_H_
#define  SSQLCONNECTIONPOOL_H_

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QQueue>

#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>

class SSqlConnectionPool
{
	SSqlConnectionPool();
public:
	static SSqlConnectionPool* instance();
	~SSqlConnectionPool();

	QSqlDatabase openConnection();
	void closeConnection(QSqlDatabase db);

	void setHostName(const QString& host) { m_hostName = host; }
	void setPort(quint16 port) { m_port =port; }
	void setDatabaseName(const QString& databaseName) { m_databaseName= databaseName; }
	void setUserName(const QString& userName) { m_userName = userName; }
	void setPassword(const QString& password) { m_password = password; }
	void setDatabaseType(const QString& type) { m_databaseType = type; }
	void setMaxConnectionCount(qint32 max) { m_maxConneciontCount = max; }

	int maxConnectionCount()const { return m_maxConneciontCount; }
	int useConnectionCount()const { return m_useConnections.count(); }
private:
	QSqlDatabase createConnection(const QString& con_name);
	void releaseConnection(QSqlDatabase db);

	quint16 m_port;
	QString m_hostName;
	QString m_databaseName;
	QString m_userName;
	QString m_password;
	QString m_databaseType;					//连接驱动类型 QMYSQL QSQLITE ...


	QMutex m_mutex;						//互斥量
	QWaitCondition m_waitCond;				//条件
	quint32 m_waitInterval;					//等待间隔 毫秒
	quint32 m_waitTime;						//等待总时间:
	qint32 m_maxConneciontCount;			//最大连接数
	QQueue<QString> m_useConnections;		//已使用连接名队列
	QQueue<QString> m_unUseConnections;		//未使用连接名队列

	friend class SConnectionWrap;
};

class SConnectionWrap
{
public:
	 SConnectionWrap();
	~SConnectionWrap();
	QSqlDatabase openConnection();
private:
	QSqlDatabase m_db;
};
#endif // !SSQLCONNECTIONPOOL_H_
