#include "GameWindow.h"

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Game game;
	if (game.Initialize())
	{
		std::cout << "initialized the game";
	}
	game.Run();
	game.Finalize();

	return 0;
}