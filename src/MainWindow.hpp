#ifndef INDEX_MAINWINDOW_HPP
#define INDEX_MAINWINDOW_HPP

#include "TermboxWidgets/Window.hpp"
#include "Sort.hpp"
class List;
class Tabline;
class Statusline;
class Menu;
class Marks;
class Prompt;

class DummyInput : public Widget
{
	virtual void Draw() { }
public:
	DummyInput() { }
	~DummyInput() { }
};

class MainWindow : public Window
{
	DummyInput* dInput;

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
	Menu* m_marksMenu;
	std::size_t m_marksMenuId;
	Menu* m_showMenu;
	std::size_t m_showMenuId;

	Menu* m_changeMenu;
	std::size_t m_changeMenuId;

	Marks* m_marks;
	std::size_t m_marksId;
	bool m_marksMode;

	Prompt* m_prompt;
	std::size_t m_promptId;

	std::vector<std::pair<Widget*, bool>> m_promptStateList;

	std::size_t m_tab;
	//TextBox? m_preview;
	//Tabbar m_tab;
	//Statusbar m_status;

	std::size_t m_sortFn;
	Sort::Settings m_sortSettings;

	// Setting:
	bool m_parentEnabled;

	void OnChangeDir();
public:
	MainWindow(const std::string& path, std::size_t tabId);
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
	void ToggleMarks();

	std::size_t GetTab() const;
};

#endif // INDEX_MAINWINDOW_HPP

