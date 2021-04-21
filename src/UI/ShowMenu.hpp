#ifndef INDEX_UI_SHOWMENU_HPP
#define INDEX_UI_SHOWMENU_HPP

#include "../Menu.hpp"
class MainWindow;

class ShowMenu : public Menu
{
	MainWindow* m_main;

public:
	ShowMenu(MainWindow* main);
	~ShowMenu();

	virtual void Resize(Vec2i dim);
};

#endif // INDEX_UI_SHOWMENU_HPP
