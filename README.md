# userManagerPrivilegeSystem
基于Qt 的用户权限管理系统,项目成果可参考演示文件。
主要技术栈： Qt Widget + Dialog + HTTP + +QXlsx + QsLog + Model-View Framework + MySql + UiTools module+ Custom Control + Json Analysis + QSS + QThread

### 项目介绍

本项目是一款仿Linux系统权限处理的管理项目，基于Qt6，采用C/S架构、mvc架构，前后端分离，模块化管理，使用模型视图框架、代理。登录模块采用JWT技术,后端模块使用QHttpServer，使用Json传输、解析数据，采用数据库连接池实现MySql连接复用和灵活配置，自定义登录、注册控件和事件处理，个性化界面定制。

### 项目难点

客户端界面设计、事件响应、及相关逻辑处理，JWT的token请求验证问题，自定义属性设置问题，用户和权限键值对绑定问题，QSS美化问题，网络模块的数据包设计、传输问题，数据解析问题，数据库实时数据显示问题。

### 项目成果

客户端界面美化，相关数据能够使用第三方文件QXlsx方便xlsx文件格式的导出和导入，QsLog方便打印调试信息和记录日志。跨平台性能良好，支持多客户端连接，且保证了并发安全。已使用windeployqt成功部署项目(代码行数1W+）。
图示如下：
![输入图片说明](%E6%BC%94%E7%A4%BA%E6%96%87%E4%BB%B6/%E5%9B%BE%E7%89%87%E6%BC%94%E7%A4%BA/1.png)
![输入图片说明](%E6%BC%94%E7%A4%BA%E6%96%87%E4%BB%B6/%E5%9B%BE%E7%89%87%E6%BC%94%E7%A4%BA/2.png)
![输入图片说明](%E6%BC%94%E7%A4%BA%E6%96%87%E4%BB%B6/%E5%9B%BE%E7%89%87%E6%BC%94%E7%A4%BA/3.png)
![输入图片说明](%E6%BC%94%E7%A4%BA%E6%96%87%E4%BB%B6/%E5%9B%BE%E7%89%87%E6%BC%94%E7%A4%BA/4.png)
![输入图片说明](%E6%BC%94%E7%A4%BA%E6%96%87%E4%BB%B6/%E5%9B%BE%E7%89%87%E6%BC%94%E7%A4%BA/5.png)
![输入图片说明](%E6%BC%94%E7%A4%BA%E6%96%87%E4%BB%B6/%E5%9B%BE%E7%89%87%E6%BC%94%E7%A4%BA/6.png)
![输入图片说明](%E6%BC%94%E7%A4%BA%E6%96%87%E4%BB%B6/%E5%9B%BE%E7%89%87%E6%BC%94%E7%A4%BA/7.png)
![输入图片说明](%E6%BC%94%E7%A4%BA%E6%96%87%E4%BB%B6/%E5%9B%BE%E7%89%87%E6%BC%94%E7%A4%BA/8.png)
![输入图片说明](%E6%BC%94%E7%A4%BA%E6%96%87%E4%BB%B6/%E5%9B%BE%E7%89%87%E6%BC%94%E7%A4%BA/10.png)
![输入图片说明](%E6%BC%94%E7%A4%BA%E6%96%87%E4%BB%B6/%E5%9B%BE%E7%89%87%E6%BC%94%E7%A4%BA/11.png)
![输入图片说明](%E6%BC%94%E7%A4%BA%E6%96%87%E4%BB%B6/%E5%9B%BE%E7%89%87%E6%BC%94%E7%A4%BA/12.png)
![输入图片说明](%E6%BC%94%E7%A4%BA%E6%96%87%E4%BB%B6/%E5%9B%BE%E7%89%87%E6%BC%94%E7%A4%BA/13.png)
![输入图片说明](%E6%BC%94%E7%A4%BA%E6%96%87%E4%BB%B6/%E5%9B%BE%E7%89%87%E6%BC%94%E7%A4%BA/14.png)
![输入图片说明](%E6%BC%94%E7%A4%BA%E6%96%87%E4%BB%B6/%E5%9B%BE%E7%89%87%E6%BC%94%E7%A4%BA/15.png)
![输入图片说明](%E6%BC%94%E7%A4%BA%E6%96%87%E4%BB%B6/%E5%9B%BE%E7%89%87%E6%BC%94%E7%A4%BA/16.png)
![输入图片说明](%E6%BC%94%E7%A4%BA%E6%96%87%E4%BB%B6/%E5%9B%BE%E7%89%87%E6%BC%94%E7%A4%BA/17.png)
![输入图片说明](%E6%BC%94%E7%A4%BA%E6%96%87%E4%BB%B6/%E5%9B%BE%E7%89%87%E6%BC%94%E7%A4%BA/18.png)
![输入图片说明](%E6%BC%94%E7%A4%BA%E6%96%87%E4%BB%B6/%E5%9B%BE%E7%89%87%E6%BC%94%E7%A4%BA/19.png)
![输入图片说明](%E6%BC%94%E7%A4%BA%E6%96%87%E4%BB%B6/%E5%9B%BE%E7%89%87%E6%BC%94%E7%A4%BA/20.png)
![输入图片说明](%E6%BC%94%E7%A4%BA%E6%96%87%E4%BB%B6/%E5%9B%BE%E7%89%87%E6%BC%94%E7%A4%BA/21.png)
![输入图片说明](%E6%BC%94%E7%A4%BA%E6%96%87%E4%BB%B6/%E5%9B%BE%E7%89%87%E6%BC%94%E7%A4%BA/22.png)
![输入图片说明](%E6%BC%94%E7%A4%BA%E6%96%87%E4%BB%B6/%E5%9B%BE%E7%89%87%E6%BC%94%E7%A4%BA/23.png)
![输入图片说明](%E6%BC%94%E7%A4%BA%E6%96%87%E4%BB%B6/%E5%9B%BE%E7%89%87%E6%BC%94%E7%A4%BA/24.png)
