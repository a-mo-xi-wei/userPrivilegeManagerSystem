#include<QCoreApplication>
#include"SSqlConnectionPool.h"
#include"Server.h"
/*
class Worker : public QObject
{
public:
	static void worker1() {
		qDebug() << __FUNCTION__;
		QSqlQuery query(SSqlConnectionPool::instance()->openConnection());
		qDebug() << __FUNCTION__;
		if (!query.exec("select * from user")) {
			qWarning() << query.lastError().text();
			return;
		}
		auto count = query.record().count();
		while (query.next()) {
			for (size_t i = 0; i < count; i++)
			{
				std::cout << query.value(i).toString().toStdString();
			}
			std::cout << std::endl;
		}
	}
	static void worker2() {
		qDebug() << __FUNCTION__;
		QSqlQuery query(SSqlConnectionPool::instance()->openConnection());
		qDebug() << __FUNCTION__;
		if (!query.exec("select * from user")) {
			qWarning() << query.lastError().text();
			return;
		}
		auto count = query.record().count();
		while (query.next()) {
			for (size_t i = 0; i < count; i++)
			{
				std::cout << query.value(i).toString().toStdString();
			}
			std::cout << std::endl;
		}
	}
	static void worker3() {
		qDebug() << __FUNCTION__;
		QSqlQuery query(SSqlConnectionPool::instance()->openConnection());
		qDebug() << __FUNCTION__;
		if (!query.exec("select * from user")) {
			qWarning() << query.lastError().text();
			return;
		}
		auto count = query.record().count();
		while (query.next()) {
			for (size_t i = 0; i < count; i++)
			{
				std::cout << query.value(i).toString().toStdString();
			}
			std::cout << std::endl;
		}
	}
};
*/

int main(int argc, char* argv[]) {
	QCoreApplication a(argc, argv);
	SSqlConnectionPool* obj = SSqlConnectionPool::instance();
	obj->setHostName("localhost");
	obj->setPort(3306);
	obj->setDatabaseName("user_privilege_manager_system");
	obj->setUserName("root");
	obj->setPassword("123456789");

	Server server;


	return a.exec();

}
