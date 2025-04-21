#include "MyFirstWndGame.h"
#include <iostream>
#include <crtdbg.h>

int main()
{
	// 첫 줄은 꼭 이 함수를 호출하고 시작 합니다.
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//_CrtSetBreakAlloc(161);

	MyFirstWndGame game;
	if (false == game.Initialize())
	{
		std::cout << "Failed to initialize the game. Error: " << GetLastError() << std::endl;
		return -1;
	}

	game.Run();

	game.Finalize();

	return 0;
}