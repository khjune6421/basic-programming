#include "INC_Windows.h"
#include "DrawRandom.h"

int main()
{
	DrawRandom drawRandomInstance;
	drawRandomInstance.GameInit();
	drawRandomInstance.GameLoop();
	return 0;
}