#ifndef  PADDEDITDLG_H_
#define  PADDEDITDLG_H_

#include "SIODialog.h"
#include <QJsonObject>

class QPushButton;
class QLabel;
class QRadioButton;
class QLineEdit;
class QTextEdit;

class PAddEditDlg : public SIODialog
{
	Q_OBJECT
public:
	PAddEditDlg(QWidget* parent = nullptr);
	void init();

	void setPrivilege(const QJsonObject& user);
signals:
	void newPrivilege(const QJsonObject& user);
protected:
	void onVisualConfig();	//可视化配置
	void onScriptConfig();	//脚本配置

	void onUpdateName(bool isScript);	//更新权限名
	void reset();		//重置界面
private:
	QJsonObject m_jprivilege;

	QLineEdit* m_name_edit{};					/*!>权限名称*/
	QLineEdit* m_descr_edit{};					/*!>权限描述*/
	QTextEdit* m_script_edit{};					/*!>脚本配置*/
	QLineEdit* m_product_service_edit{};		/*!>产品服务*/
	QLineEdit* m_other_privilege_edit{};		/*!>操作权限（其他权限编辑框）*/
	QLineEdit* m_specifiedRes_edit{};			/*!>资源权限（指定资源编辑）*/

	QRadioButton* m_readonlyPrivilegeRbtn{};
	QRadioButton* m_adminPrivilegeRbtn{};
	QRadioButton* m_otherPrivilegeRbtn{};
	QWidget* CreateScriptConfigWidget();
};

#endif //! USERDETAILSDLG_H_