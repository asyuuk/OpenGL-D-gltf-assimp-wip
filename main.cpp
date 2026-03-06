#include"Renderer.h"
#include<memory>

int main()
{
	Drawing::Renderer renderer;
	renderer.Run(); // Start the renderer
	renderer.MainLoop();

	return 0;
}