#ifndef INDEX_UI_SHELLMENU_HPP
#define INDEX_UI_SHELLMENU_HPP

#include "../Menu.hpp"
class MainWindow;

class ShellMenu : public Menu
{
	MainWindow* m_main;

public:
	ShellMenu(MainWindow* main);
	~ShellMenu();

	virtual void Resize(Vec2i dim);
};

#endif // INDEX_UI_SHELLMENU_HPP
