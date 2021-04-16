#include "Settings.hpp"
#include "Tab.hpp"

#include "DirectoryCache.hpp"

int main(int argc, char* argv[])
{
	std::setlocale(LC_ALL, Settings::locale);

	try
	{
		std::string path = (".");
		if (argc >= 2)
			path = (argv[argc-1]);

		Termbox tb(Color::COLORS_TRUECOLOR, COLOR_DEFAULT);
		Tab tab;
		Tabs.push_back(std::move(tab));
		MainWindow win(path, Tabs.size()-1);
		Tabs[Tabs.size()-1].SetMainWindow(tb.AddWidget(&win));


		tb.RenderLoop();
	}
	catch (Util::Exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return EXIT_SUCCESS;
}
