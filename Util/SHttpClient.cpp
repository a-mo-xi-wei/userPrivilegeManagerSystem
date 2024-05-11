#include "SHttpClient.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QUrlQuery>
#include <QHttpPart>
#include <QHttpMultiPart>



//[如何设计API接口，实现统一格式返回？]
/*-----------------------------------------------------------------------------|
|                              SHttpClientPrivate                              |
|----------------------------------------------------------------------------*/
/**
* @brief 请求的类型
*
* 注: UPLOAD 不是 HTTP Method，只是为了上传时对请求进行特殊处理而定义的
*/
enum class SHttpClientRequestMethod {
    GET, POST, PUT, DELETE, UPLOAD
};

/**
* @brief 缓存 SHttpClientPrivate 的数据成员，方便在异步 lambda 中使用 = 以值的方式访问。
*/
class SHttpClientPrivateCache {
public:
    std::function<void(const QByteArray&)>      successHandler{};
    std::function<void(const QString&, int)>    failHandler{};
    std::function<void(bool)>                   completeHandler{};
    std::function<void(qint64, qint64)>         progressHandler{};
    std::function<void(const QString&)>         pathingHandler{};     
    bool debug = false;
    bool internal = false;
    QNetworkAccessManager* manager = nullptr;
};

/**
* @brief SHttpClient 的辅助类，封装不希望暴露给客户端的数据和方法，使得 SHttpClient 只暴露必要的 API 给客户端。
*/
class SHttpClientPrivate {
    friend class SHttpClient;
    using ptr = std::shared_ptr<SHttpClientPrivate>;
    
    SHttpClientPrivate(const QUrl &url);
public:
    ~SHttpClientPrivate();
private:
    void stop1();

    /**
    * @brief 缓存 SHttpClientPrivate 的数据成员
    *
    * @return 返回 SHttpClientPrivateCache 缓存对象
    */
    SHttpClientPrivateCache cache();

    /**
    * @brief 获取 Manager，如果传入了 manager 则返回此 manager，否则新创建一个 manager，默认会自动创建一个 manager，
    *        使用传入的 manager 则 interval 被设置为 false，自动创建的 manager 则设置 interval 为 true
    *
    * @return 返回 QNetworkAccessManager 对象
    */
    QNetworkAccessManager* getManager();

    /**
    * @brief 使用用户设定的 URL、请求头、参数等创建 Request
    *
    * @param d      SHttpClientPrivate 的对象
    * @param method 请求的类型
    * @return 返回可用于执行请求的 QNetworkRequest
    */
    static QNetworkRequest createRequest(const SHttpClientPrivate::ptr& d, SHttpClientRequestMethod method);

    /**
    * @brief 执行请求的辅助函数
    *
    * @param d      SHttpClientPrivate 的对象
    * @param method 请求的类型
    */
    static void executeQuery(const SHttpClientPrivate::ptr& d, SHttpClientRequestMethod method);

    /**
    * @brief 上传文件或者数据
    *
    * @param d     SHttpClientPrivate 的对象
    * @param paths 要上传的文件的路径(path 和 data 不能同时使用)
    * @param data  要上传的文件的数据
    */
    static void upload(const SHttpClientPrivate::ptr& d, const QStringList &paths, const QByteArray &data);

	/**
     * @brief 使用GET进行下载.
     *
	 * @param d        SHttpClientPrivate 的对象
     * @param path 文件保存路径；若为空则保存到当前程序路径
     * @param filename 不带后缀名(拓展名)的文件名；若为空则使用服务器返回的Content-Disposition头指定的文件名
     */
	static void download(const SHttpClientPrivate::ptr& d, const QString& path, const QString& filename);

    /**
    * @brief 读取服务器响应的数据
    *
    * @param reply   请求的 QNetworkReply 对象
    * @param encoding 请求响应的字符集，默认使用 UTF-8
    * @return 返回服务器端响应的字符串
    */
    static  QByteArray readReply(QNetworkReply *reply);

    /**
    * @brief 请求结束的处理函数
    *
    * @param cache          SHttpClientPrivateCache 缓存对象
    * @param reply          QNetworkReply 对象，不能为 NULL
    * @param successMessage 请求成功的消息
    * @param failMessage    请求失败的消息
    */
    static void handleFinish(SHttpClientPrivateCache cache, QNetworkReply *reply, const QByteArray &successMessage, const QString &failMessage);

    /////////////////////////////////////////////////// 成员变量 //////////////////////////////////////////////
    QUrl   url;                            // 请求的 URL
    QString   json;                           // 请求的参数使用 Json 格式
    QUrlQuery params;                         // 请求的参数使用 Form 格式
    QHash<QString, QString> headers;          // 请求头
    QNetworkAccessManager *manager = nullptr; // 执行 HTTP 请求的 QNetworkAccessManager 对象
    bool useJson = false;                    // 为 true 时请求使用 Json 格式传递参数，否则使用 Form 格式传递参数
    bool debug = false;                    // 为 true 时输出请求的 URL 和参数
    bool internal = true;                     // 是否使用自动创建的 manager

    std::function<void(const QByteArray&)>   successHandler = nullptr; // 成功的回调函数，参数为响应的字符串
    std::function<void(const QString&, int)> failHandler = nullptr; // 失败的回调函数，参数为失败原因和 HTTP status code
	std::function<void(bool)>                completeHandler = nullptr; // 结束的回调函数，无参数
    std::function<void(qint64, qint64)>      progressHandler{};         // 上传或下载进度回调
    std::function<void(const QString&)>      pathingHandler{};      // 文件下载时，下载的路径准备完毕回调，可以通过此回调拿到下载的文件的路径
};

SHttpClientPrivate::SHttpClientPrivate(const QUrl &url) 
    : url(url) { }

SHttpClientPrivate::~SHttpClientPrivate() {
    manager = nullptr;
    successHandler = nullptr;
    failHandler = nullptr;
    completeHandler = nullptr;
}

void SHttpClientPrivate::stop1()
{
    manager->deleteLater();
}

// 缓存 SHttpClientPrivate 的数据成员
SHttpClientPrivateCache SHttpClientPrivate::cache() {
    SHttpClientPrivateCache cache;

    cache.successHandler = successHandler;
    cache.failHandler = failHandler;
    cache.completeHandler = completeHandler;
    cache.progressHandler = progressHandler;
    cache.pathingHandler = pathingHandler;
    cache.debug = debug;
    cache.internal = internal;
    cache.manager = getManager();

    return cache;
}

// 执行请求的辅助函数
void SHttpClientPrivate::executeQuery(const SHttpClientPrivate::ptr& d, SHttpClientRequestMethod method) {
    // 1. 缓存需要的变量，在 lambda 中使用 = 捕获进行值传递 (不能使用引用 &，因为 d 已经被析构)
    // 2. 创建请求需要的变量
    // 3. 根据 method 执行不同的请求
    // 4. 请求结束时获取响应数据，在 handleFinish 中执行回调函数

    // [1] 缓存需要的变量，在 lambda 中使用 = 捕获进行值传递 (不能使用引用 &，因为 d 已经被析构)
    SHttpClientPrivateCache cache = d->cache();

    // [2] 创建请求需要的变量
    QNetworkRequest request = SHttpClientPrivate::createRequest(d, method);
    QNetworkReply    *reply = nullptr;

    // [3] 根据 method 执行不同的请求
    switch (method) {
    case SHttpClientRequestMethod::GET:
        reply = cache.manager->get(request);
        break;
    case SHttpClientRequestMethod::POST:
        reply = cache.manager->post(request, d->useJson ? d->json.toUtf8() : d->params.toString(QUrl::FullyEncoded).toUtf8());
        break;
    case SHttpClientRequestMethod::PUT:
        reply = cache.manager->put(request, d->useJson ? d->json.toUtf8() : d->params.toString(QUrl::FullyEncoded).toUtf8());
        break;
    case SHttpClientRequestMethod::DELETE:
        reply = cache.manager->deleteResource(request);
        break;
    default:
        break;
    }

    // [4] 请求结束时获取响应数据，在 handleFinish 中执行回调函数
    // 请求结束时一次性读取所有响应数据
    QObject::connect(reply, &QNetworkReply::finished, [=] {
        auto successData = SHttpClientPrivate::readReply(reply);
        QString failMessage = reply->errorString();
        SHttpClientPrivate::handleFinish(cache, reply, successData, failMessage);
    });
}

// 下载文件或者数据的实现
void SHttpClientPrivate::download(const SHttpClientPrivate::ptr& d, const QString& path, const QString& filename)
{
	// 1. 缓存需要的变量，在 lambda 中使用 = 捕获进行值传递 (不能使用引用 &，因为 d 已经被析构)
	// 2. 创建请求需要的变量，执行请求
	// 3. 有数据可读取时，保存到文件
	// 4. 注册请求结束时，关闭文件的回调
	// 5. 下载进度回调
	// 6. 请求结束时获取响应数据，在 handleFinish 中执行回调函数

	// [1] 缓存需要的变量，在 lambda 中使用 = 捕捉使用 (不能使用引用 &，因为 d 已经被析构)
	SHttpClientPrivateCache cache = d->cache();

	// [2] 创建请求需要的变量，执行请求
	QNetworkRequest request = SHttpClientPrivate::createRequest(d, SHttpClientRequestMethod::GET);
	QNetworkReply* reply = cache.manager->get(request);

	// [3] 有数据可读取时,获取服务器Content-Disposition头中传递的文件名，打开并写入本地
	QFile* file = new QFile;  
	QObject::connect(reply, &QNetworkReply::readyRead, [=] {
		if (!file->isOpen()) {
			QString savePath;
            if (filename.isEmpty()) {
				auto instruct_list = reply->header(QNetworkRequest::ContentDispositionHeader).toString().split('=');
				if (instruct_list.size() < 2) {
					qDebug().noquote() << QString("[警告] :Content-Disposition Header Not Found! %1")
						.arg("已自动切换保存文件名");

					if (cache.failHandler)
						cache.failHandler("Content-Disposition Header Not Found!", -3);
					savePath = path + QString("/download_%1.data").arg(reply->header(QNetworkRequest::ContentLengthHeader).toLongLong());

				}
				else {
					savePath = path + "/" + QFileInfo(instruct_list.at(1)).fileName();
				}
            }
            else {
                savePath = path + "/" + filename;
            }
            file->setFileName(savePath);
			// [1] 打开下载文件，如果打开文件出错，不进行下载
			if (!file->open(QIODevice::WriteOnly | QIODevice::Truncate)) {
				file->deleteLater();

				if (d->debug) {
					qDebug().noquote() << QString("[错误] 打开文件出错: %1").arg(savePath);
				}

				if (nullptr != d->failHandler) {
					d->failHandler(QString("[错误] 打开文件出错: %1").arg(savePath), -1);
				}
				return;
			}
            //文件打开成功才调用
            //if (cache.pathingHandler)
            //    cache.pathingHandler(savePath);

		}
		file->write(reply->readAll());
		});

	// [4] 给请求结束的回调函数注入关闭释放文件的行为
	std::function<void(bool)> userCompleteHandler = cache.completeHandler;
	std::function<void(bool)> injectedCompleteHandler = [=](bool hasError) {

		// 请求结束后释放文件对象
		file->flush();
		file->close();

        //在文件保存之后进行回调
		if (!hasError && cache.pathingHandler)
		    cache.pathingHandler(file->fileName());

		file->deleteLater();
		// 执行用户指定的结束回调函数
		if (nullptr != userCompleteHandler) {
			userCompleteHandler(hasError);
		}
		};
	cache.completeHandler = injectedCompleteHandler;

	// [5] 下载进度
	QObject::connect(reply, &QNetworkReply::downloadProgress, [=](qint64 bytesCurrent, qint64 bytesTotal) {
		if (cache.progressHandler)
			cache.progressHandler(bytesCurrent, bytesTotal);
		});

	// [6] 请求结束时获取响应数据，在 handleFinish 中执行回调函数
	QObject::connect(reply, &QNetworkReply::finished, [=] {
		auto  successMessage = "下载完成"; // 请求结束时一次性读取所有响应数据
		QString failMessage = reply->errorString();
		SHttpClientPrivate::handleFinish(cache, reply, successMessage, failMessage);
		});
}

// 上传文件或者数据的实现
void SHttpClientPrivate::upload(const SHttpClientPrivate::ptr& d, const QStringList& paths, const QByteArray& data) {
    // 1. 缓存需要的变量，在 lambda 中使用 = 捕获进行值传递 (不能使用引用 &，因为 d 已经被析构)
    // 2. 创建 Form 表单的参数 Text Part
    // 3. 创建上传的 File Part
    //    3.1 使用文件创建 File Part
    //    3.2 使用数据创建 File Part
    // 4. 创建请求需要的变量，执行请求
    // 5. 请求结束时释放 multiPart 和打开的文件，获取响应数据，在 handleFinish 中执行回调函数

    // [1] 缓存需要的变量，在 lambda 中使用 = 捕捉使用 (不能使用引用 &，因为 d 已经被析构)
    SHttpClientPrivateCache cache = d->cache();

    // [2] 创建 Form 表单的参数 Text Part
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QList<QPair<QString, QString> > paramItems = d->params.queryItems();
    for (int i = 0; i < paramItems.size(); ++i) {
        QString name = paramItems.at(i).first;
        QString value = paramItems.at(i).second;

        QHttpPart textPart;
        textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QString("form-data; name=\"%1\"").arg(name));
        textPart.setBody(value.toUtf8());
        multiPart->append(textPart);
    }

    if (paths.size() > 0) {
        // [3.1] 使用文件创建 File Part
        QString inputName = paths.size() == 1 ? "file" : "files"; // 一个文件时为 file，多个文件时为 files

        for (const QString &path : paths) {
            // path 为空时，不上传文件
            if (path.isEmpty()) {
                continue;
            }

            // We cannot delete the file now, so delete it with the multiPart
            QFile *file = new QFile(path, multiPart);

            // 如果文件打开失败，则释放资源返回，终止上传
            if (!file->open(QIODevice::ReadOnly)) {
                QString failMessage = QString("打开文件失败[%2]: %1").arg(path).arg(file->errorString());

                if (cache.debug) {
                    qDebug().noquote() << failMessage;
                }

                if (nullptr != cache.failHandler) {
                    cache.failHandler(failMessage, -1);
                }

                multiPart->deleteLater();
                return;
            }

            // 单个文件时，name 为服务器端获取文件的参数名，为 file
            // 多个文件时，name 为服务器端获取文件的参数名，为 files
            // 注意: 服务器是 Java 的则用 form-data
            // 注意: 服务器是 PHP  的则用 multipart/form-data
            QString disposition = QString("form-data; name=\"%1\"; filename=\"%2\"").arg(inputName).arg(file->fileName());
            QHttpPart filePart;
            filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(disposition));
            filePart.setBodyDevice(file);
            multiPart->append(filePart);
        }
    }
    else {
        // [3.2] 使用数据创建 File Part
        QString   disposition = QString("form-data; name=\"file\"; filename=\"no-name\"");
        QHttpPart dataPart;
        dataPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(disposition));
        dataPart.setBody(data);
        multiPart->append(dataPart);
    }

    // [4] 创建请求需要的变量，执行请求
    QNetworkRequest request = SHttpClientPrivate::createRequest(d, SHttpClientRequestMethod::UPLOAD);
    QNetworkReply    *reply = cache.manager->post(request, multiPart);

    // [4] 上传进度 
    QObject::connect(reply, &QNetworkReply::uploadProgress, [=](qint64 bytesCurrent,qint64 bytesTotal) {
        if (cache.progressHandler)
            cache.progressHandler(bytesCurrent, bytesTotal);
    });

    // [5] 请求结束时释放 multiPart 和文件，获取响应数据，在 handleFinish 中执行回调函数
    QObject::connect(reply, &QNetworkReply::finished, [=] {
        multiPart->deleteLater(); // 释放资源: multiPart + file

        QByteArray successData = SHttpClientPrivate::readReply(reply); // 请求结束时一次性读取所有响应数据
        QString failMessage = reply->errorString();
        SHttpClientPrivate::handleFinish(cache, reply, successData, failMessage);
    });
}

// 获取 Manager，如果传入了 manager 则返回此 manager，否则新创建一个 manager，默认会自动创建一个 manager
QNetworkAccessManager* SHttpClientPrivate::getManager() {
    return internal ? new QNetworkAccessManager() : manager;
}

// 使用用户设定的 URL、请求头、参数等创建 Request
QNetworkRequest SHttpClientPrivate::createRequest(const SHttpClientPrivate::ptr& d, SHttpClientRequestMethod method) {
    // 1. 如果是 GET 请求，并且参数不为空，则编码请求的参数，放到 URL 后面
    // 2. 调试时输出网址和参数
    // 3. 设置 Content-Type
    // 4. 添加请求头到 request 中

    bool get = method == SHttpClientRequestMethod::GET;
    bool upload = method == SHttpClientRequestMethod::UPLOAD;
    bool withForm = !get && !upload && !d->useJson; // PUT、POST 或者 DELETE 请求，且 useJson 为 false
    bool withJson = !get && !upload &&  d->useJson; // PUT、POST 或者 DELETE 请求，且 useJson 为 true

                                                    // [1] 如果是 GET 请求，并且参数不为空，则编码请求的参数，放到 URL 后面
    if (get && !d->params.isEmpty()) {
        d->url.setQuery(d->params);
        //d->url += "?" + d->params.toString(QUrl::FullyEncoded);
        qDebug() << d->url << "   " << d->params.toString();

    }

    // [2] 调试时输出网址和参数
    if (d->debug) {
        qDebug().noquote() << "[网址]" << d->url;

        if (withJson) {
            qDebug().noquote() << "[参数]" << d->json;
        }
        else if (withForm || upload) {
            QList<QPair<QString, QString> > paramItems = d->params.queryItems();
            QString buffer; // 避免多次调用 qDebug() 输入调试信息，每次 qDebug() 都有可能输出行号等

                            // 按键值对的方式输出参数
            for (int i = 0; i < paramItems.size(); ++i) {
                QString name = paramItems.at(i).first;
                QString value = paramItems.at(i).second;

                if (0 == i) {
                    buffer += QString("[参数] %1=%2\n").arg(name).arg(value);
                }
                else {
                    buffer += QString("       %1=%2\n").arg(name).arg(value);
                }
            }

            if (!buffer.isEmpty()) {
                qDebug().noquote() << buffer;
            }
        }
    }

    // [3] 设置 Content-Type
    // 如果是 POST 请求，useJson 为 true 时添加 Json 的请求头，useJson 为 false 时添加 Form 的请求头
    if (withForm) {
        d->headers["Content-Type"] = "application/x-www-form-urlencoded";
    }
    else if (withJson) {
        d->headers["Content-Type"] = "application/json; charset=utf-8";
    }

    // [4] 添加请求头到 request 中
    QNetworkRequest request(QUrl(d->url));
    for (auto i = d->headers.cbegin(); i != d->headers.cend(); ++i) {
        request.setRawHeader(i.key().toUtf8(), i.value().toUtf8());
    }

    return request;
}

// 读取服务器响应的数据
QByteArray SHttpClientPrivate::readReply(QNetworkReply *reply) {
    QByteArray result;

    while (!reply->atEnd()) {
        result += reply->readAll();
    }

    return result;
}

// 请求结束的处理函数
void SHttpClientPrivate::handleFinish(SHttpClientPrivateCache cache, QNetworkReply *reply, const QByteArray &successMessage, const QString &failMessage) {
    // 1. 执行请求成功的回调函数
    // 2. 执行请求失败的回调函数
    // 3. 执行请求结束的回调函数
    // 4. 释放 reply 和 manager 对象
    bool hasError = false;
    if (reply->error() == QNetworkReply::NoError) {
        if (cache.debug) {
            qDebug().noquote() << QString("[结束] 成功: %1").arg(successMessage);
        }

        // [1] 执行请求成功的回调函数
        if (nullptr != cache.successHandler) {
            cache.successHandler(successMessage);
        }
    }
    else {
        if (cache.debug) {
            qDebug().noquote() << QString("[结束] 失败: %1").arg(failMessage);
        }

        // [2] 执行请求失败的回调函数
        if (nullptr != cache.failHandler) {
            cache.failHandler(failMessage, reply->error());
        }
        hasError = true;
    }

    // [3] 执行请求结束的回调函数
    if (nullptr != cache.completeHandler) {
        cache.completeHandler(hasError);
    }

    // [4] 释放 reply 和 manager 对象
    if (nullptr != reply) {
        reply->deleteLater();
    }

    if (cache.internal && nullptr != cache.manager) {
        cache.manager->deleteLater();
    }
}

/*-----------------------------------------------------------------------------|
|                                 SHttpClient                                  |
|----------------------------------------------------------------------------*/

// 注意: 在异步请求中 SHttpClient 的 SHttpClientPrivate 成员变量 d 已经被析构，所以需要先缓存相关变量为栈对象，使用 = 以值的方式访问

SHttpClient::SHttpClient(const QUrl &url) : d(new SHttpClientPrivate(url)) { }

SHttpClient::SHttpClient(const SHttpClient& httpc)
    :d(httpc.d)
{

}

SHttpClient::~SHttpClient() {
}

void SHttpClient::stop2()
{
    d->stop1();
}


void SHttpClient::stop()
{
    
}

// 传入 QNetworkAccessManager 给多个请求共享
SHttpClient& SHttpClient::manager(QNetworkAccessManager *manager) {
    d->manager = manager;
    d->internal = (nullptr == manager);

    return *this;
}

// 传入 debug 为 true 则使用 debug 模式，请求执行时输出请求的 URL 和参数等
SHttpClient& SHttpClient::debug(bool debug) {
    d->debug = debug;

    return *this;
}

// 添加一个请求的参数，可以多次调用添加多个参数
SHttpClient& SHttpClient::param(const QString &name, const QVariant &value) {
    d->params.addQueryItem(name, value.toString());

    return *this;
}

// 添加多个请求的参数
SHttpClient& SHttpClient::params(const QMap<QString, QVariant> &ps) {
    for (auto iter = ps.cbegin(); iter != ps.cend(); ++iter) {
        d->params.addQueryItem(iter.key(), iter.value().toString());
    }

    return *this;
}

// 添加请求的参数 (请求体)，使用 Json 格式，例如 "{\"name\": \"Alice\"}"
SHttpClient& SHttpClient::json(const QByteArray&json) {
    d->json = json;
    d->useJson = true;

    return *this;
}

SHttpClient& SHttpClient::json(const QJsonObject& json)
{
    d->json = QJsonDocument(json).toJson(QJsonDocument::Compact);
    d->useJson = true;
    return *this;
}

// 添加请求头
SHttpClient& SHttpClient::header(const QString &name, const QString &value) {
    d->headers[name] = value;

    return *this;
}

// 添加多个请求头
SHttpClient& SHttpClient::headers(const QMap<QString, QString> nameValues) {
    for (auto i = nameValues.cbegin(); i != nameValues.cend(); ++i) {
        d->headers[i.key()] = i.value();
    }

    return *this;
}

// 注册请求成功的回调函数
SHttpClient& SHttpClient::success(const std::function<void(const QByteArray&)>& successHandler) {
    d->successHandler = successHandler;

    return *this;
}

// 注册请求失败的回调函数
SHttpClient& SHttpClient::fail(const std::function<void(const QString&, int)>& failHandler) {
    d->failHandler = failHandler;

    return *this;
}

// 注册请求结束的回调函数，不管成功还是失败都会执行
SHttpClient& SHttpClient::complete(const std::function<void(bool)>& completeHandler) {
    d->completeHandler = completeHandler;

    return *this;
}

SHttpClient& SHttpClient::progress(const std::function<void(qint64, qint64)>& progressHandler)
{
    d->progressHandler = progressHandler;
    return *this;
}

// 执行 GET 请求
void SHttpClient::get() {
    SHttpClientPrivate::executeQuery(d, SHttpClientRequestMethod::GET);
}

// 执行 POST 请求
void SHttpClient::post() {
    SHttpClientPrivate::executeQuery(d, SHttpClientRequestMethod::POST);
}

// 执行 PUT 请求
void SHttpClient::put() {
    SHttpClientPrivate::executeQuery(d, SHttpClientRequestMethod::PUT);
}

// 执行 DELETE 请求
void SHttpClient::remove() {
    SHttpClientPrivate::executeQuery(d, SHttpClientRequestMethod::DELETE);
}

// 使用 GET 进行下载，下载的文件保存到 path
void SHttpClient::download(const QString& path, const QString& filename) {
    SHttpClientPrivate::download(d,path,filename);
}

SHttpClient& SHttpClient::pathing(const std::function<void(QString)>& pathingHandler) {
    d->pathingHandler = pathingHandler;
    return *this;
}

// 上传文件
void SHttpClient::upload(const QString &path) {
    QStringList paths = { path };
    SHttpClientPrivate::upload(d, paths, QByteArray());
}

// 上传文件，文件的内容以及读取到 data 中
void SHttpClient::upload(const QByteArray &data) {
    SHttpClientPrivate::upload(d, QStringList(), data);
}

// 上传多个文件
void SHttpClient::upload(const QStringList &paths) {
    SHttpClientPrivate::upload(d, paths, QByteArray());
}

