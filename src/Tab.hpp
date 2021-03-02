#ifndef INDEX_TAB_HPP
#define INDEX_TAB_HPP

#include "MainWindow.hpp"

class Tab
{
	std::size_t m_mainId;
public:
	Tab();
	~Tab();

	void SetMainWindow(std::size_t mainId);
	MainWindow* GetMainWindow();
	std::size_t GetMainWindowId();
};

extern std::vector<Tab> Tabs;

#endif // INDEX_TAB_HPP
