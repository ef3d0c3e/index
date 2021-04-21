#ifndef INDEX_UI_GOMENU_HPP
#define INDEX_UI_GOMENU_HPP

#include "../Menu.hpp"
class MainWindow;

class GoMenu : public Menu
{
	MainWindow* m_main;

public:
	GoMenu(MainWindow* main);
	~GoMenu();

	virtual void Resize(Vec2i dim);
};

#endif // INDEX_UI_GOMENU_HPP
