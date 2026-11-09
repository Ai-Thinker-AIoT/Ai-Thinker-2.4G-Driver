/* ---------------------------------------------

	长缨-I C351编译器 函数库 80351 指令
	C351 库模块  STDLIBI

	作者： 杨为民 	2024-08
	QQ：   86547995

--------------------------------------------- */
#ifndef __C351_STDLIBI_H__
#define __C351_STDLIBI_H__

#define	binary unsigned

// **** 整数函数 ***************
extern int   rand ();
extern void  srand (int v1);

extern char  cabs  (char  v2);
extern int    abs  (int   v2);
extern long  labs  (long  v2);

// **** 字符串转换为数值 ***************
extern long  atol (char *s1);
extern int   atoi (char *s1);

extern long  stol (char *Inps);

// **** 数值转换为字符串 ***************
extern void  ltoh8s  (char *Outs, binary long x32l);
extern void  ltoa  (char *Outs, long x32) ;
extern void  itoa  (char *Outs, int x16) ;
extern void  ctoa  (char *Outs, char x8) ;

#endif
