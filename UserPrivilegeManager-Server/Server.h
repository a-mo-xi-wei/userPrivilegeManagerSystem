#ifndef SERVER_H_
#define SERVER_H_

#include<QHttpServer>
class Server
{
public:
	Server();
	void userRoute();
	void privilegeRoute();
	void user_privilegeRoute();
private:
	QHttpServer m_server;
};

#endif // !SERVER_H_
