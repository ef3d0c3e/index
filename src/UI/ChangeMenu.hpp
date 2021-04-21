#ifndef INDEX_UI_CHANGEMENU_HPP
#define INDEX_UI_CHANGEMENU_HPP

#include "../Menu.hpp"
class MainWindow;

class ChangeMenu : public Menu
{
	MainWindow* m_main;

public:
	ChangeMenu(MainWindow* main);
	~ChangeMenu();

	virtual void Resize(Vec2i dim);
};

#endif // INDEX_UI_CHANGEMENU_HPP
