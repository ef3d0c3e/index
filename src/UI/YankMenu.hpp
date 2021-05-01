#ifndef INDEX_UI_YANKMENU_HPP
#define INDEX_UI_YANKMENU_HPP

#include "../Menu.hpp"
class MainWindow;

class YankMenu : public Menu
{
	MainWindow* m_main;

public:
	YankMenu(MainWindow* main);
	~YankMenu();

	virtual void Resize(Vec2i dim);
};

#endif // INDEX_UI_YANKMENU_HPP
