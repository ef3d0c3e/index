#ifndef INDEX_TAB_HPP
#define INDEX_TAB_HPP

#include "MainWindow.hpp"

class Tab
{
	MainWindow* m_main;
	bool m_parentEnabled;
public:
	Tab();
	~Tab();

	void SetMainWindow(MainWindow* main);

	EventListener<bool> OnSetParentEnabled;
	void SetParentEnabled(bool enabled);
	bool GetParentEnabled() const;
};

extern std::vector<Tab> Tabs;

#endif // INDEX_TAB_HPP
