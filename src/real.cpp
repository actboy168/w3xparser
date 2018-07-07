#include "real.h"

/*
Jass中实数类型转换。
为了防止编译器的优化，必须把编译选项中的“浮点模型”改为“严格”。
*/
float from_real(real_t val)
{
	return *(float*)&val;
}

real_t to_real(float val)
{
	return *(real_t*)&val;
}
