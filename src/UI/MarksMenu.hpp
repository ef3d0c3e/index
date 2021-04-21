#ifndef INDEX_UI_MARKSMENU_HPP
#define INDEX_UI_MARKSMENU_HPP

#include "../Menu.hpp"
class MainWindow;

class MarksMenu : public Menu
{
	MainWindow* m_main;

public:
	MarksMenu(MainWindow* main);
	~MarksMenu();

	virtual void Resize(Vec2i dim);
};

#endif // INDEX_UI_MARKSMENU_HPP
