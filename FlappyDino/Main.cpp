#include "GameWindow.h"

int main()
{
	Game game;
	if (game.Initialize())
	{
		std::cout << "initialized the game";
	}
	game.Run();
	game.Finalize();

	return 0;
}