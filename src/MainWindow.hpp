#ifndef INDEX_MAINWINDOW_HPP
#define INDEX_MAINWINDOW_HPP

#include "TermboxWidgets/Window.hpp"
#include "Sort.hpp"
class List;
class Tabline;
class Statusline;
class Menu;

class MainWindow : public Window
{
	List* m_dir;
	std::size_t m_dirId;
	List* m_parent;
	std::size_t m_parentId;
	Tabline* m_tabline;
	std::size_t m_tablineId;
	Statusline* m_statusline;
	std::size_t m_statuslineId;
	Menu* m_goMenu;
	std::size_t m_goMenuId;

	std::size_t m_tab;
	//TextBox? m_preview;
	//Tabbar m_tab;
	//Statusbar m_status;

	std::size_t m_sortFn;
	Sort::Settings m_sortSettings;

	void OnChangeDir();
public:
	MainWindow(const std::string& path);
	~MainWindow();

	virtual void Resize(Vec2i dim);

	void UpdateFiles();

	// Actions
	void Forward(const String& folder);
	void Back();

	const Directory* GetDir() const;
	const List* GetList() const;

	void Error(const String& msg);
	void CD(const std::string& path);
};

#endif // INDEX_MAINWINDOW_HPP

