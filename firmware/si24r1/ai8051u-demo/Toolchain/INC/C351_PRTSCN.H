/* ---------------------------------------------

	长缨-I C351编译器 函数库 80351 指令
	C351 库模块  PRTSCN

	作者： 杨为民 	2024-08
	QQ：   86547995

--------------------------------------------- */
#ifndef __C351_PRTSCN_H__
#define __C351_PRTSCN_H__

#define	binary unsigned
extern int svtoaf  (char * Outs, char * FMT, binary long X32) ;
extern int sprintf  (char * x,  char * y, ...);
extern int printf  ( char * y, ...);

extern int sscanf (char * x,  char * y, ...);
extern int scanf (char * x, ...);

#endif

