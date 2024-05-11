#pragma once
#include<QPixmap>

class SStandardPixmap 
{
	SStandardPixmap() {}
public:
	enum StandardPixmap
	{
		SP_TitleBarCloseButton,
		SP_TitleBarMinButton,
		SP_TitleBarMaxButton,
		SP_TitleBarNormalButton,
		SP_TitleBarIconButton,
	};
public:
	/**
	 * @brief 获取标准图片.
	 * @param pix 图片类型枚举
	 * @param theme 图片主题 light 明亮主题 dark黑暗主题 
	 */
	static QPixmap pixmap(SStandardPixmap::StandardPixmap pix,const QString& theme = "light");
};
