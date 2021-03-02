#include "Tab.hpp"

std::vector<Tab> Tabs;

Tab::Tab()
{
}

Tab::~Tab()
{
}

void Tab::SetMainWindow(std::size_t mainId)
{
	m_mainId = mainId;
}

MainWindow* Tab::GetMainWindow()
{
	return reinterpret_cast<MainWindow*>(Termbox::GetWidget(m_mainId));
}

std::size_t Tab::GetMainWindowId()
{
	return m_mainId;
}
