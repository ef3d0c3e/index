#ifndef INDEX_UI_SORTMENU_HPP
#define INDEX_UI_SORTMENU_HPP

#include "../Menu.hpp"
class MainWindow;

class SortMenu : public Menu
{
	MainWindow* m_main;

public:
	SortMenu(MainWindow* main);
	~SortMenu();

	virtual void Resize(Vec2i dim);
};

#endif // INDEX_UI_SORTMENU_HPP
