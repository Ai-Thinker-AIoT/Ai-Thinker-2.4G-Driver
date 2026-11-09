/* ---------------------------------------------

	长缨-I C351编译器 函数库 80351 指令
	C351 库模块  STRING

	作者： 杨为民 	2024-08
	QQ：   86547995

--------------------------------------------- */
#ifndef __C351_STRING_H__
#define __C351_STRING_H__

extern int strlen (char *STRSRC ) ;

// ---- *STRDST 只能是 XDATA 存储类型 ----------------
extern char strcpy (char *STRDST, char *STRSRC ) ;
extern char strcat (char *STRDST, char *STRSRC ) ;
extern char strcmp (char *STRDST, char *STRSRC ) ;

#endif
