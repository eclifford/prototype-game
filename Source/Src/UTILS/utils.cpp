#include "utils\utils.h"

#include "utils\mmgr.h"

int RandomNum(int min, int max)
{
	return rand() % (max - min + 1) + min;
}

float RandomFloatRange(float min, float max)
{
	float alpha = (rand()%10000) / 10000.f;
	return (1 - alpha) * min + alpha * max;
}