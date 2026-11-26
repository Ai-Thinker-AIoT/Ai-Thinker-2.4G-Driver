/* ---------------------------------------------

	长缨-I C351编译器 函数库 80351 指令
	C351 库模块  STDIO

	作者： 杨为民 	2024-08
	QQ：   86547995

--------------------------------------------- */
#ifndef __C351_STDIO_H__
#define __C351_STDIO_H__

// ==== STDIO 库函数 =============================
extern char qkey (void);
extern char getchar (void);
extern void putchar (char x);
extern void *gets (char * x, int n);
extern void puts (char *x);

#endif

