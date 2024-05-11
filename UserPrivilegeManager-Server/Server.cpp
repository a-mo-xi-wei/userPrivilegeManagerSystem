#include "Server.h"
#include "SResultCode.h"
#include "SSqlConnectionPool.h"
#include "SHttpPartParse.h"
#include "SJwt.h"
#include "QsLog.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <optional>

QJsonObject RecordToJsonObject(const QSqlRecord& record);

#define CheckJsonParse(request)\
	QJsonParseError error;\
	auto jdoc = QJsonDocument::fromJson(request.body(), &error);\
	if (error.error != QJsonParseError::NoError) {\
		return SResult::failure(SResultCode::ParamJsonInvalid);\
	}

#define CheckQueryExec(query)\
	if (query.lastError().type() != QSqlError::NoError) {\
		return SResult::failure(SResultCode::ServerSqlQueryError);\
	}
const int PAGESIZE = 20;

const char* SECRET = "weisang666";

std::optional<QByteArray>CheckToken(const QHttpServerRequest& request) {
	//验证token
	auto auth = request.value("Authorization");
	//如果没有认证头
	if (auth.isEmpty()) {
		return SResult::failure(SResultCode::UserUnauthorized);
	}
	//必须以Bearer开头
	if (!auth.startsWith("Bearer")) {
		return SResult::failure(SResultCode::UserAuthFormatError);
	}
	//拿到token
	auto token = auth.mid(strlen("Bearer"));
	//验证token 
	auto jwtObject = SJwt::SJwtObject::decode(token, SJwt::SAlgorithm::HS256, SECRET);
	if (jwtObject.status() == SJwt::SJwtObject::Status::Expired) {
		return SResult::failure(SResultCode::UserAuthTokenExpired);
	}
	if (!jwtObject.isValid()) {
		return SResult::failure(SResultCode::UserAuthTokenInvalid);
	}
	return {};
}

Server::Server()
{
	QsLogging::Logger& logger = QsLogging::Logger::instance();
	logger.setLoggingLevel(QsLogging::Level::DebugLevel);
	QDir dir;
	dir.mkpath("logs");
	auto fileDest = QsLogging::DestinationFactory::MakeFileDestination("logs/log.txt", QsLogging::EnableLogRotation, QsLogging::MaxSizeBytes(1024 * 1024), QsLogging::MaxOldLogCount(5));
	//auto consoleDest = QsLogging::DestinationFactory::MakeDebugOutputDestination();
	auto funDest = QsLogging::DestinationFactory::MakeFunctorDestination([](const QString& message, QsLogging::Level level)
		{
			qDebug() << message;
		});
	logger.addDestination(fileDest);
	logger.addDestination(funDest);
	//logger.addDestination(consoleDest);

	//开启服务器
	m_server.listen(QHostAddress::Any, 8888);

	//插入测试数据
	 /* {
		SConnectionWrap wrap;
		auto db = wrap.openConnection();
		db.transaction();
		QSqlQuery query(db);
		query.prepare("insert ignore into user(user_id,username,gender,email,mobile,password) values(?,?,?,?,?,?)");
		for (size_t i = 0; i < 1000; i++) {
			query.bindValue(0, 0+QString::number(99999+i));
			query.bindValue(1,QString("wei_%1").arg(i));
			query.bindValue(2, i%3);
			query.bindValue(3, QString("123456%1@qq.com").arg(i));
			//query.bindValue(4, );
			query.bindValue(5, "123456");
			query.exec();
		}
		db.commit();
		QLOG_DEBUG() << "test data finished";
	}*/

	//添加路径丢失处理
	m_server.setMissingHandler([](const QHttpServerRequest& request,
		QHttpServerResponder&& responder)
		{
			QJsonObject jobj;
			jobj.insert("code", "3000");
			jobj.insert("message", "不支持此操作");
			responder.write(QJsonDocument(jobj));
		});
	//添加路由
	//获取服务器版本
	m_server.route("/api/version", [](const QHttpServerRequest& request)
		{
			QJsonObject jobj;
			jobj.insert("version", "0.0.1");
			jobj.insert("name", "UserPrivilegeManager-Server");
			jobj.insert("datatime", QDateTime::currentDateTime().toString("yyyy-MM-dd"));
			jobj.insert("copyright", "威桑版权所有");
			return QJsonDocument(jobj).toJson(QJsonDocument::Compact);
		});
	//用户登录
	m_server.route("/api/login", [](const QHttpServerRequest& request)
		{
			CheckJsonParse(request);
			SConnectionWrap warp;
			QSqlQuery query(warp.openConnection());
			query.prepare("select * from user where user_id=? and password=? and isDeleted=0 limit 1");
			query.bindValue(0, jdoc["user_id"].toString());
			query.bindValue(1, jdoc["password"].toString());
			if (!query.exec()) {
				return SResult::failure(SResultCode::ServerSqlQueryError);
			}	
			
			if (!query.next()) {	//一定要先query.next 要不然下不去
				return SResult::failure(SResultCode::UserLoginError);
			}
			//qDebug() << "检查是否禁用";

			//检查一下用户是否禁用
			if (!query.value("isEnable").toBool()) {
				return SResult::success(SResultCode::UserAccountDisable);
			}
			//生成token jwt
			QJsonObject payload = {
				{"user_id",jdoc["user_id"]},
				{"iat",QDateTime::currentDateTime().toString(Qt::DateFormat::ISODate)},
				{"exp",QDateTime::currentDateTime().addDays(7).toString(Qt::DateFormat::ISODate)}
			};
			SJwt::SJwtObject jwtObject(SJwt::SAlgorithm::HS256, payload, SECRET);
			
			return SResult::success({ {"token",QString(jwtObject.jwt())} });
		});
	//用户注册	
	m_server.route("/api/register", [](const QHttpServerRequest& request)
		{
			
			CheckJsonParse(request);
			SConnectionWrap warp;
			QSqlQuery query(warp.openConnection());
			query.prepare("insert ignore into user(user_id,username,password) values(?,?,?) ");
			query.bindValue(0, jdoc["user_id"].toString());
			query.bindValue(1, jdoc["username"].toString());
			query.bindValue(2, jdoc["password"].toString());
			query.exec();
			CheckQueryExec(query);
			//用户已经存在
			if (query.numRowsAffected() == 0) {
				return SResult::failure(SResultCode::UserAccountExists);
			}
			return SResult::success();

		});
	
	userRoute();
	privilegeRoute();
	user_privilegeRoute();
}

void Server::userRoute()
{
	//用户列表
	m_server.route("/api/user/list", [](const QHttpServerRequest& request)
		{
			std::optional<QByteArray> ret = CheckToken(request);
			if (ret.has_value()) {
				return ret.value();
			}
			auto uquery = request.query();
			auto user_id = uquery.queryItemValue("user_id");
			auto isEnable = uquery.queryItemValue("isEnable");
			auto curPage = uquery.queryItemValue("curPage").toInt();//当前页数
			auto q = uquery.queryItemValue("query");

			QString filter = "where isDeleted=false ";
			if (!user_id.isEmpty()) {
				filter += QString("and user_id='%1' ").arg(user_id);
			}
			else if (!q.isEmpty()) {
				filter += QString("and user_id like '%%1%' or username like '%%1%' \
					or mobile like '%%1%' or email like '%%1%' ").arg(q);//中间的%1才用于arg替换
			}
			if (!isEnable.isEmpty()) {
				filter += QString("and isEnable=%1 ").arg(isEnable == "true" ? 1 : 0);
			}
			//qDebug() << filter;
			SConnectionWrap warp;
			QSqlQuery query(warp.openConnection());
			//查询总记录条数
			query.exec(QString("select count(*) total from user %1 ").arg(filter));
			CheckQueryExec(query)  ;
			query.next();
			int total = query.value("total").toInt();
			//获取页面总数
			auto totalPage = total / PAGESIZE + (total % PAGESIZE ? 1 : 0);
			if (curPage > totalPage || curPage < 1) {
				curPage = 1;		//初值 
			}	

			QString sql = "select user_id,username,gender,mobile,email,isEnable from user ";
			sql += filter;
			sql += QString("limit %1,%2").arg((curPage - 1) * PAGESIZE).arg(PAGESIZE);
			query.exec(sql);
			CheckQueryExec(query);
			QJsonObject jobj;
			if (query.size() > 0) {
				QJsonArray jarray;
				while (query.next()) {
					jarray.append(RecordToJsonObject(query.record()));
				}
				jobj.insert("lists", jarray);
				jobj.insert("curPage", curPage);
				jobj.insert("totalPage", totalPage);
				jobj.insert("total", total);
			}
			return SResult::success(jobj);
		});

	//用户创建
	m_server.route("/api/user/create", [](const QHttpServerRequest& request)
		{
			std::optional<QByteArray> ret = CheckToken(request);
			if (ret.has_value()) {
				return ret.value();
			}
			CheckJsonParse(request);
			//检查必要的参数是否缺失
			auto rObj = jdoc.object();
			if (!(rObj.contains("user_id") && rObj.contains("username"))) {
				return SResult::failure(SResultCode::ParamLoss, "user_id 或 username 参数丢失");
			}
			auto password = rObj.value("password").toString();
			if (password.isEmpty()) {
				password = "123456";
			}
			SConnectionWrap wrap;
			QSqlQuery query(wrap.openConnection());
			query.prepare("insert ignore into user(user_id,username,gender,email,mobile,password) values(?,?,?,?,?,?)");
			query.bindValue(0, rObj.value("user_id").toString());
			query.bindValue(1, rObj.value("username").toString());
			query.bindValue(2, rObj.value("gender").toInt());
			query.bindValue(3, rObj.value("email").toString());
			query.bindValue(4, rObj.value("mobile").toString());
			query.bindValue(5, password);
			query.exec();
			CheckQueryExec(query);
			//有没有插入成功
			if (query.numRowsAffected() == 0) {
				qDebug() << "插入失败";
				return SResult::failure(SResultCode::UserAccountExists);
			}
			return SResult::success();
		});

	m_server.route("/api/user/batch_create", [](const QHttpServerRequest& request)
		{
			std::optional<QByteArray> ret = CheckToken(request);
			if (ret.has_value()) {
				return ret.value();
			}
			CheckJsonParse(request);

			SConnectionWrap wrap;
			auto db = wrap.openConnection();
			db.transaction();
			QSqlQuery query(db);
			auto rootObj = jdoc.object();
			auto jarr = rootObj.value("lists").toArray();
			query.prepare("insert ignore into user(user_id,username,gender,email,mobile,password) values(?,?,?,?,?,?)");
			for (size_t i = 0; i < jarr.size(); i++)
			{
				//检查必要的参数是否缺失
				auto rObj = jarr.at(i).toObject();
				if (!(rObj.contains("user_id") && rObj.contains("username"))) {
					db.rollback();
					return SResult::failure(SResultCode::ParamLoss, "user_id 或 username 参数丢失");
				}
				query.bindValue(0, rObj.value("user_id").toString());
				query.bindValue(1, rObj.value("username").toString());
				query.bindValue(2, rObj.value("gender").toInt());
				query.bindValue(3, QVariant());//rObj.value("email"));
				query.bindValue(4, rObj.value("mobile").toString());
				//query.bindValue(5, rObj.value("password").toVariant().toString());
				query.bindValue(5,"123456");
				//qDebug() << "密码是 ： " << rObj.value("password").toVariant().toString();
				if (!query.exec()) {
					db.rollback();
					return SResult::failure(SResultCode::ServerSqlQueryError, query.lastError().text());
				}
				CheckQueryExec(query);
			}
			db.commit();

			//有没有插入成功
			if (query.numRowsAffected() == 0) {
				qDebug() << "插入失败";
				return SResult::failure(SResultCode::UserAccountExists);
			}
			return SResult::success();
		});

	//用户删除
	m_server.route("/api/user/delete", [](const QHttpServerRequest& request)
		{
			std::optional<QByteArray> ret = CheckToken(request);
			if (ret.has_value()) {
				return ret.value();
			}
			CheckJsonParse(request);
			auto jarray = jdoc["lists"].toArray();
			SConnectionWrap wrap;
			QSqlQuery query(wrap.openConnection());
			query.prepare("update user set isDeleted=1 where user_id=?");
			for (size_t i = 0; i < jarray.size(); i++)
			{
				query.bindValue(0, jarray.at(i).toString());
				query.exec();
				CheckQueryExec(query);
			}
			return SResult::success();

		});

	//用户修改
	m_server.route("/api/user/alter", [](const QHttpServerRequest& request)
		{
			std::optional<QByteArray> ret = CheckToken(request);
			if (ret.has_value()) {
				return ret.value();
			}
			auto uquery = request.query();
			if (!uquery.hasQueryItem("user_id")) {
				return SResult::failure(SResultCode::ParamLoss);
			}
			CheckJsonParse(request);
			QString update;
			auto rObj = jdoc.object();
			if (rObj.contains("user_id"))
				update += QString("user_id='%1',").arg(rObj.value("user_id").toString());

			if (rObj.contains("username"))
				update += QString("username='%1',").arg(rObj.value("username").toString());

			if (rObj.contains("password"))
				update += QString("password='%1',").arg(rObj.value("password").toString());

			if (rObj.contains("email") && !rObj.value("email").toString().isEmpty())
				update += QString("email='%1',").arg(rObj.value("email").toString());

			if (rObj.contains("mobile")&& !rObj.value("mobile").toString().isEmpty())
				update += QString("mobile='%1',").arg(rObj.value("mobile").toString());

			if (rObj.contains("gender"))
				update += QString("gender=%1,").arg(rObj.value("gender").toInt());

			if (rObj.contains("isEnable"))
				update += QString("isEnable=%1,").arg(rObj.value("isEnable").toInt());
			update.chop(1);	//截断最后一个逗号
			//qDebug() << QString("update user set %1 where user_id='%2'").arg(update).arg(uquery.queryItemValue("user_id"));
			SConnectionWrap wrap;
			QSqlQuery query(wrap.openConnection());
			query.exec(QString("update user set %1 where user_id='%2'").arg(update).arg(uquery.queryItemValue("user_id")));
			CheckQueryExec(query);

			if (query.numRowsAffected() == 0) {
				return SResult::failure(SResultCode::SuccessButNotData);
			}
			return SResult::success();
		});

	//批量修改
	m_server.route("/api/user/batch_alter", [](const QHttpServerRequest& request)
		{
			std::optional<QByteArray> ret = CheckToken(request);
			if (ret.has_value()) {
				return ret.value();
			}
			CheckJsonParse(request);

			auto jarr = jdoc["lists"].toArray();
			if (jarr.isEmpty()) {
				return SResult::failure(SResultCode::SuccessButNotData);
			}
			QString resultSql;
			for (size_t i = 0; i < jarr.size(); i++)
			{
				auto jobj = jarr.at(i).toObject();
				auto filterObj = jobj.value("filter").toObject();
				auto updateObj = jobj.value("update").toObject();
				QString filter;
				for (auto it = filterObj.begin(); it != filterObj.end(); it++) {
					if (it.key() == ("id"))
						filter += QString(" id='%1' and").arg(it.value().toString());
					else if (it.key() == ("user_id"))
						filter += QString(" user_id='%1' and").arg(it.value().toString());
					else if (it.key() == ("isEnable"))
						filter += QString(" isEnable='%1' and").arg(it.value().toString());
					else if (it.key() == ("isDeleted"))
						filter += QString(" isDeleted='%1' and").arg(it.value().toString());
				}
				filter.chop(3);
				QString update;
				for (auto it = updateObj.begin(); it != updateObj.end(); it++) {
					if (it.key() == "username")
						update += QString("username='%1',").arg(it.value().toString());
					else if (it.key() == "password")
						update += QString("password='%1',").arg(it.value().toString());
					else if (it.key() == "email")
						update += QString("email='%1',").arg(it.value().toString());
					else if (it.key() == "mobile")
						update += QString("mobile='%1',").arg(it.value().toString());
					else if (it.key() == "gender")
						update += QString("gender=%1,").arg(it.value().toInt());
					else if (it.key() == "isEnable")
						update += QString("isEnable=%1,").arg(it.value().toInt());
				}
				update.chop(1);	//截断最后一个逗号
				auto sql = QString("update user set %1 where %2;").arg(update).arg(filter);
				resultSql.append(sql);
			}
			QLOG_DEBUG() << resultSql;


			SConnectionWrap wrap;
			QSqlQuery query(wrap.openConnection());
			query.exec(resultSql);
			CheckQueryExec(query);

			if (query.numRowsAffected() == 0) {
				return SResult::failure(SResultCode::SuccessButNotData);
			}
			return SResult::success();
		});

	//用户头像上传（修改）	Post
	m_server.route("/api/user/avatar", QHttpServerRequest::Method::Post, [](const QHttpServerRequest& request)
		{
			std::optional<QByteArray> ret = CheckToken(request);
			if (ret.has_value()) {
				return ret.value();
			}
			auto uquery = request.query();
			if (!uquery.hasQueryItem("user_id")) {
				return SResult::failure(SResultCode::ParamLoss);
			}
			auto user_id = uquery.queryItemValue("user_id");
			auto data = request.body();
			//qDebug() << data;
			SHttpPartParse parse(data);
			if (!parse.parse()) {
				qDebug() << "body parse error";
				return SResult::failure(SResultCode::ParamParseError);
			}
			//qDebug() << parse.name() << " " << parse.fileName();
			QDir dir;
			auto path = "images/avatar/";
			if (!dir.exists(path)) {
				dir.mkpath(path);
			}
			QFile file(path + user_id + "." + QFileInfo(parse.fileName()).suffix());
			if (!file.open(QIODevice::WriteOnly)) {
				return SResult::failure(SResultCode::ServerInnerError, "文件打开失败");
			}
			file.write(parse.data());
			file.close();
			//把路径写入数据库
			SConnectionWrap wrap;
			QSqlQuery query(wrap.openConnection());
			query.prepare("update user set avatar_path=? where user_id=? and isDeleted=false");
			query.bindValue(0, file.fileName());
			query.bindValue(1, user_id);
			query.exec();
			CheckQueryExec(query);
			//if (query.numRowsAffected() == 0) {
			//	//由于文件名没有改变，所以后面就算再次更换图片影响也是0,为避免这种情况，直接注释
			//	return SResult::failure(SResultCode::SuccessButNotData);
			//}

			return SResult::success();

		});
	
	//用户头像获取	Get
	m_server.route("/api/user/avatar", QHttpServerRequest::Method::Get, [](const QHttpServerRequest& request, QHttpServerResponder&& resp)
		{
			std::optional<QByteArray> ret = CheckToken(request);
			if (ret.has_value()) {
				resp.write(ret.value(), "application/json");
				return;
			}
			auto uquery = request.query();
			if (!uquery.hasQueryItem("user_id")) {
				resp.write(SResultCode::ParamLoss.toJson(), "application/json");
				return;
			}
			auto user_id = uquery.queryItemValue("user_id");
			//qDebug() << user_id;
			SConnectionWrap wrap;
			QSqlQuery query(wrap.openConnection());
			query.prepare("select avatar_path from user where user_id=? and isDeleted=false");
			query.bindValue(0, user_id);
			if (!query.exec()) {
				resp.write(SResultCode::ServerSqlQueryError.toJson(), "application/json");
				return;
			}
			//qDebug() << query.value("avatar_path").toString();
			if (!query.next()) {
				resp.write(SResultCode::SuccessButNotData.toJson(), "application/json");
				return;
			}

			QFile file(query.value("avatar_path").toString());
			if (!file.open(QIODevice::ReadOnly)) {
				resp.write(SResult::failure(SResultCode::ServerResourceNotFound, "头像未找到"), "application/json");
				return;
			}
			resp.writeStatusLine();
			resp.writeHeader("Content-Type", QString("image/%1").arg(QFileInfo(file.fileName()).
				suffix()).toUtf8());
			resp.writeHeader("Content-Length", QByteArray::number(file.size()));
			resp.writeHeader("Content-Disposition", ("attachment;filename=" +
				QFileInfo(file.fileName()).fileName()).toUtf8());
			resp.writeBody(file.readAll());

		});
	
	//用户初始密码获取
	m_server.route("/api/user/password", QHttpServerRequest::Method::Get, [](const QHttpServerRequest& request)
		{
			std::optional<QByteArray> ret = CheckToken(request);
			if (ret.has_value()) {
				return ret.value();
			}
			auto uquery = request.query();
			if (!uquery.hasQueryItem("user_id")) {
				return SResult::failure(SResultCode::ParamLoss, "user_id 参数丢失");
			}
			auto user_id = uquery.queryItemValue("user_id");
			//qDebug() <<"user_id = "<< user_id;
			SConnectionWrap wrap;
			QSqlQuery query(wrap.openConnection());
			query.prepare("select * from user where user_id=? and isDeleted=false");
			query.bindValue(0, user_id);
			query.exec();
			CheckQueryExec(query);
			if (!query.next()) {
				return SResult::failure(SResultCode::SuccessButNotData);
			}
			//qDebug() << "当前密码 : "<<query.value("password").toString();
			QJsonObject jobj;
			jobj.insert("password", query.value("password").toString());
			return SResult::success(jobj);
		});
	
}

void Server::privilegeRoute()
{
	//权限列表
	m_server.route("/api/privilege/list", [](const QHttpServerRequest& request)
		{
			std::optional<QByteArray> ret = CheckToken(request);
			if (ret.has_value()) {
				return ret.value();
			}
			auto uquery = request.query();
			auto curPage = uquery.queryItemValue("curPage").toInt();//当前页数

			//qDebug() << filter;
			SConnectionWrap warp;
			QSqlQuery query(warp.openConnection());
			//查询总记录条数
			query.exec(QString("select count(*) total from privilege"));
			CheckQueryExec(query);
			query.next();
			int total = query.value("total").toInt();
			//获取页面总数
			auto totalPage = total / PAGESIZE + (total % PAGESIZE ? 1 : 0);
			if (curPage > totalPage || curPage < 1) {
				curPage = 1;		//初值 
			}

			QString sql = "select number,privilege,descr,area from privilege ";
			sql += QString("limit %1,%2").arg((curPage - 1) * PAGESIZE).arg(PAGESIZE);
			query.exec(sql);
			CheckQueryExec(query);
			QJsonObject jobj;
			if (query.size() > 0) {
				QJsonArray jarray;
				while (query.next()) {
					jarray.append(RecordToJsonObject(query.record()));
				}
				jobj.insert("lists", jarray);
				jobj.insert("curPage", curPage);
				jobj.insert("totalPage", totalPage);
				jobj.insert("total", total);
			}
			return SResult::success(jobj);
		});
	//权限删除
	m_server.route("/api/privilege/delete", [](const QHttpServerRequest& request)
		{
			std::optional<QByteArray> ret = CheckToken(request);
			if (ret.has_value()) {
				return ret.value();
			}
			CheckJsonParse(request);
			auto jarray = jdoc["lists"].toArray();
			SConnectionWrap wrap;
			QSqlQuery query(wrap.openConnection());
			query.prepare("update privilege set isDeleted=true where name=?");
			for (size_t i = 0; i < jarray.size(); i++)
			{
				query.bindValue(0, jarray.at(i).toString());
				query.exec();
				CheckQueryExec(query);
			}
			return SResult::success();

		});
}

void Server::user_privilegeRoute()
{
	//用户列表
	m_server.route("/api/user_privilege/list", [](const QHttpServerRequest& request)
		{
			std::optional<QByteArray> ret = CheckToken(request);
			if (ret.has_value()) {
				return ret.value();
			}
			auto uquery = request.query();
			auto user_id = uquery.queryItemValue("user_id");
			auto curPage = uquery.queryItemValue("curPage").toInt();//当前页数
			auto q = uquery.queryItemValue("query");

			QString filter = "where isDeleted=0 ";
			if (!user_id.isEmpty()) {
				filter += QString("and user_id='%1' ").arg(user_id);
			}
			if (!q.isEmpty()) {
				filter += QString("and user_id like '%%1%' or username like '%%1%' ").arg(q);//中间的%1才用于arg替换
			}
			SConnectionWrap warp;
			QSqlQuery query(warp.openConnection());
			//查询总记录条数
			query.exec(QString("select count(*) total from user_privilege %1 ").arg(filter));
			CheckQueryExec(query);
			query.next();
			int total = query.value("total").toInt();
			//qDebug() << "总功查询到 ： " << total << "条数据";
			//获取页面总数
			auto totalPage = total / PAGESIZE + (total % PAGESIZE ? 1 : 0);
			if (curPage > totalPage || curPage < 1) {
				curPage = 1;		//初值 
			}

			QString sql = "select user_id,username,privilege_read,privilege_edit,privilege_add,privilege_delete from user_privilege ";
			sql += filter;
			sql += QString("limit %1,%2").arg((curPage - 1) * PAGESIZE).arg(PAGESIZE);
			query.exec(sql);
			CheckQueryExec(query);
			QJsonObject jobj;
			if (query.size() > 0) {
				QJsonArray jarray;
				while (query.next()) {
					jarray.append(RecordToJsonObject(query.record()));
				}
				jobj.insert("lists", jarray);
				jobj.insert("curPage", curPage);
				jobj.insert("totalPage", totalPage);
				jobj.insert("total", total);
			}
			return SResult::success(jobj);
		});

	//用户创建
	m_server.route("/api/user_privilege/create", [](const QHttpServerRequest& request)
		{
			std::optional<QByteArray> ret = CheckToken(request);
			if (ret.has_value()) {
				return ret.value();
			}
			CheckJsonParse(request);
			//检查必要的参数是否缺失
			auto rObj = jdoc.object();
			if (!(rObj.contains("user_id") && rObj.contains("username"))) {
				return SResult::failure(SResultCode::ParamLoss, "user_id 或 username 参数丢失");
			}
			SConnectionWrap wrap;
			QSqlQuery query(wrap.openConnection());
			query.prepare("insert ignore into user_privilege(user_id,username,privilege_read) values(?,?,?)");
			query.bindValue(0, rObj.value("user_id").toString());
			query.bindValue(1, rObj.value("username").toString());
			query.bindValue(2, rObj.value("privilege_read").toInt());
			query.exec();
			CheckQueryExec(query);
			//有没有插入成功
			if (query.numRowsAffected() == 0) {
				qDebug() << "插入失败，账号已存在";
				return SResult::failure(SResultCode::UserAccountExists);
			}
			return SResult::success();
		});

	m_server.route("/api/user_privilege/batch_create", [](const QHttpServerRequest& request)
		{
			std::optional<QByteArray> ret = CheckToken(request);
			if (ret.has_value()) {
				return ret.value();
			}
			CheckJsonParse(request);

			SConnectionWrap wrap;
			auto db = wrap.openConnection();
			db.transaction();
			QSqlQuery query(db);
			auto rootObj = jdoc.object();
			auto jarr = rootObj.value("lists").toArray();
			query.prepare("insert ignore into user_privilege(user_id,username,privilege_read) values(?,?,?)");
			for (size_t i = 0; i < jarr.size(); i++)
			{
				//检查必要的参数是否缺失
				auto rObj = jarr.at(i).toObject();
				if (!(rObj.contains("user_id") && rObj.contains("username"))) {
					db.rollback();
					return SResult::failure(SResultCode::ParamLoss, "user_id 或 username 参数丢失");
				}
				query.bindValue(0, rObj.value("user_id").toString());
				query.bindValue(1, rObj.value("username").toString());
				query.bindValue(2, rObj.value("privilege_read").toInt());
				if (!query.exec()) {
					db.rollback();
					return SResult::failure(SResultCode::ServerSqlQueryError, query.lastError().text());
				}
				CheckQueryExec(query);
			}
			//事物提交：将当前事务中所做的所有更改永久性地保存到数据库中。
			// 一旦提交，这些更改对外部世界（其他数据库连接、应用程序等）变得可见。
			db.commit();

			//有没有插入成功
			if (query.numRowsAffected() == 0) {
				qDebug() << "插入失败";
				return SResult::failure(SResultCode::UserAccountExists);
			}
			return SResult::success();
		});

	//用户删除
	m_server.route("/api/user_privilege/delete", [](const QHttpServerRequest& request)
		{
			std::optional<QByteArray> ret = CheckToken(request);
			if (ret.has_value()) {
				return ret.value();
			}
			CheckJsonParse(request);
			auto jarray = jdoc["lists"].toArray();
			SConnectionWrap wrap;
			QSqlQuery query(wrap.openConnection());
			query.prepare("update user_privilege set isDeleted=1 where user_id=?");
			for (size_t i = 0; i < jarray.size(); i++)
			{
				query.bindValue(0, jarray.at(i).toString());
				query.exec();
				CheckQueryExec(query);
			}
			return SResult::success();

		});

	
	//权限修改
	m_server.route("/api/user_privilege/alter", [](const QHttpServerRequest& request)
		{
			std::optional<QByteArray> ret = CheckToken(request);
			if (ret.has_value()) {
				return ret.value();
			}
			auto uquery = request.query();
			if (!uquery.hasQueryItem("user_id")) {
				return SResult::failure(SResultCode::ParamLoss);
			}
			CheckJsonParse(request);
			QString update;
			auto rObj = jdoc.object();
			if (rObj.contains("user_id"))
				update += QString("user_id='%1',").arg(rObj.value("user_id").toString());
			if (rObj.contains("privilege_read"))
				update += QString("privilege_read='%1',").arg(rObj.value("privilege_read").toInt());
			if (rObj.contains("privilege_edit"))
				update += QString("privilege_edit='%1',").arg(rObj.value("privilege_edit").toInt());
			if (rObj.contains("privilege_add"))
				update += QString("privilege_add='%1',").arg(rObj.value("privilege_add").toInt());
			if (rObj.contains("privilege_delete"))
				update += QString("privilege_delete=%1,").arg(rObj.value("privilege_delete").toInt());
			update.chop(1);	//截断最后一个逗号

			SConnectionWrap wrap;
			QSqlQuery query(wrap.openConnection());
			query.exec(QString("update user_privilege set %1 where user_id='%2'").arg(update).arg(uquery.queryItemValue("user_id")));
			CheckQueryExec(query);

			if (query.numRowsAffected() == 0) {
				return SResult::failure(SResultCode::SuccessButNotData);
			}
			return SResult::success();
		});
}

QJsonObject RecordToJsonObject(const QSqlRecord& record)
{
	QJsonObject juser;
	for (size_t i = 0; i < record.count(); i++)
	{
		const auto& k = record.fieldName(i);
		const auto& v = record.value(i);
		switch (v.typeId())
		{
		case QMetaType::Bool:
			juser.insert(k, v.toBool());
			break;
		case QMetaType::Int:
		case QMetaType::UInt:
		case QMetaType::LongLong:
		case QMetaType::ULongLong:
		case QMetaType::Long:
		case QMetaType::Short:
		case QMetaType::ULong:
		case QMetaType::UShort:
			juser.insert(k, v.toLongLong());
			break;
		case QMetaType::Double:
		case QMetaType::Float:
			juser.insert(k, v.toDouble());
			break;
		default:
			juser.insert(k, v.toString());
			break;
		}
	}
	return juser;
}