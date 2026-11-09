/* ---------------------------------------------

	长缨-I C351编译器 函数库 80351 指令
	C351 库模块  MATH

	作者： 杨为民 	2024-08
	QQ：   86547995

--------------------------------------------- */
#ifndef _C351_MATH_H__
#define _C351_MATH_H__

// ==== 数学函数 ==========================
extern float sqrt  (float v1);

extern float exp   (float v1);
extern float exp10 (float v1);  		// Only C351

extern float log   (float v1);
extern float log10 (float v1);

#define math_PI 3.1415926535

extern float cos   (float v1);
extern float sin   (float v1);
extern float tan   (float v1);
extern float atan  (float v1);
extern float asin  (float v1);
extern float acos  (float v1);

extern float fint  (float v1);			// Only C351
extern float ceil  (float v1);
extern float floor (float v1);

extern float atan2 (float y, float x);
extern float fmod  (float x, float y);
extern float pow   (float x, float y);

#endif
