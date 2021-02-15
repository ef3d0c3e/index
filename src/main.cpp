#include "Settings.hpp"
#include "Tab.hpp"

int main(int argc, char* argv[])
{
	std::setlocale(LC_ALL, Settings::locale);

	std::string path = GetWd(".");
	if (argc >= 2)
		path = GetWd(argv[argc-1]);

	try
	{
		Termbox tb(Color::COLORS_TRUECOLOR, COLOR_DEFAULT);
		MainWindow win(path);
		tb.AddWidget(&win);

		tb.RenderLoop();
	}
	catch (Util::Exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return EXIT_SUCCESS;
}
