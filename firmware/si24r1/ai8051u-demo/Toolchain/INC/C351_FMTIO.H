/* ---------------------------------------------

	长缨-I C351编译器 函数库 80351 指令
	C351 库模块  FMTIO

	作者： 杨为民 	2024-08
	QQ：   86547995

--------------------------------------------- */

#ifndef __C351_FMTIO_H__
#define __C351_FMTIO_H__

extern char  ltos(char *Out, long x32u, char Width, char FMT);
extern char  itos(char *Out, long x32u, char Width, char FMT);
extern char  ctos(char *Out, long x32u, char Width, char FMT);

extern void  ptos  (char *Outs, long *PM32);
extern char  ftos  (char *Outs, float x32u, char Width, char Dec, char FMT);

#endif

