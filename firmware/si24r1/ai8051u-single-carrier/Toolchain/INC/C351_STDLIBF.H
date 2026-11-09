/* ---------------------------------------------

	长缨-I C351编译器 函数库 80351 指令
	C351 库模块  STDLIBF

	作者： 杨为民 	2024-08
	QQ：   86547995

--------------------------------------------- */
#ifndef __C351_STDLIBF_H__
#define __C351_STDLIBF_H__

extern float fabs  (float v1);

// **** 字符串转换为数值 ***************
extern float atof (char *s1);
extern char  ftoa  (char *Outs, float X);

#endif
