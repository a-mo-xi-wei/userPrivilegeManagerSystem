#include "SSqlConnectionPool.h"
#include <QThread>

SSqlConnectionPool::SSqlConnectionPool()
	:m_port(3306)
	,m_databaseType("QMYSQL")
	,m_maxConneciontCount(10)
	,m_waitInterval(100)
	,m_waitTime(1000)
{

}

SSqlConnectionPool* SSqlConnectionPool::instance()
{
	static SSqlConnectionPool* ins = nullptr;
	if (!ins) {
		ins = new SSqlConnectionPool;
	}
	return ins;
}

SSqlConnectionPool::~SSqlConnectionPool()
{
	QMutexLocker locker(&m_mutex);

	for (const auto& con_name : m_useConnections) {
		QSqlDatabase::removeDatabase(con_name);
	}

	for (const auto& con_name : m_unUseConnections) {
		QSqlDatabase::removeDatabase(con_name);
	}
}

QSqlDatabase SSqlConnectionPool::openConnection()
{	
	QMutexLocker<QMutex> locker(&m_mutex);

	//当前总连接数
	int count = m_useConnections.count() + m_unUseConnections.count();

	//如果所有连接都在使用中count == m_maxConneciontCount;并且没有未使用的连接
	//如果没有未使用的连接；并且总连接数等于最大连接数；并且没有到达等待时间；则等待其他线程关闭连接
	for (size_t i = 0;m_unUseConnections.isEmpty() && count == m_maxConneciontCount  && i < m_waitTime; i += m_waitInterval)
	{
		//等待其他的连接被关闭
		m_waitCond.wait(&m_mutex, m_waitInterval);
		//重新计算一下总连接数
		count = m_useConnections.count() + m_unUseConnections.count();
	}

	QString con_name;
	//判断是否需要创建新的连接
	if (m_unUseConnections.count() > 0)
		con_name = m_unUseConnections.dequeue();
	else if (count < m_maxConneciontCount)
		con_name = QString("conneciont-%1").arg(count +1);	//把当前总连接数+1作为连接名
	else {
		qWarning() << "all connection used!";
		return QSqlDatabase();
	}

	auto db = createConnection(con_name);
	if (db.isOpen())
		m_useConnections.enqueue(con_name);

	return db;
}

void SSqlConnectionPool::closeConnection(QSqlDatabase db)
{
	QMutexLocker locker(&m_mutex);
	auto con_name = db.connectionName();
	if (m_useConnections.contains(con_name))
	{
		m_useConnections.removeOne(con_name);
		m_unUseConnections.enqueue(con_name);
		m_waitCond.wakeOne();
	}
	//qDebug() << "total connections:" << m_useConnections.count() + m_unUseConnections.count();
	//qDebug() << "use connections:" << m_useConnections.count();
	//qDebug() << "unUse connections:" << m_unUseConnections.count();
}

QSqlDatabase SSqlConnectionPool::createConnection(const QString& con_name)
{
	if (QSqlDatabase::contains(con_name)) {
		//qWarning() << con_name << " connection already exists!";
		auto db = QSqlDatabase::database(con_name);
		if (db.isOpen())
			return db;
	}

	QSqlDatabase db = QSqlDatabase::addDatabase(m_databaseType, con_name);
	db.setHostName(m_hostName);
	db.setPort(m_port);
	db.setDatabaseName(m_databaseName);
	db.setUserName(m_userName);
	db.setPassword(m_password);

	if (!db.open()) {
		qWarning() << "createConnection error:" << db.lastError().text();
		return QSqlDatabase();
	}
	return  db;
}

void SSqlConnectionPool::releaseConnection(QSqlDatabase db)
{
	auto con_name = db.connectionName();
	QSqlDatabase::removeDatabase(con_name);
}

SConnectionWrap::SConnectionWrap()
{
}

SConnectionWrap::~SConnectionWrap()
{
	auto pool =  SSqlConnectionPool::instance();
	pool->closeConnection(m_db);

/*	printf(R"(
---max connections:%d
---total connections:%d
---use connections:%d
---unUse connections:%d)",
	pool->maxConnectionCount(),
	pool->m_useConnections.count() + pool->m_unUseConnections.count(),
	pool->useConnectionCount(),
	pool->m_unUseConnections.count());
	*/
}

QSqlDatabase SConnectionWrap::openConnection()
{
	m_db = SSqlConnectionPool::instance()->openConnection();
	return m_db;
}



