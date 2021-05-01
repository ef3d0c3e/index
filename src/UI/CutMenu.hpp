#ifndef INDEX_UI_CUTMENU_HPP
#define INDEX_UI_CUTMENU_HPP

#include "../Menu.hpp"
class MainWindow;

class CutMenu : public Menu
{
	MainWindow* m_main;

public:
	CutMenu(MainWindow* main);
	~CutMenu();

	virtual void Resize(Vec2i dim);
};

#endif // INDEX_UI_CUTMENU_HPP
