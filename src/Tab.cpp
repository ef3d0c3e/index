#include "Tab.hpp"

std::deque<Tab> gTabs;

Tab::Tab()
{
}

Tab::~Tab()
{
}

void Tab::SetMainWindow(std::size_t mainId)
{
	m_mainId = mainId;
	m_shouldUpdate = false;
}

MainWindow* Tab::GetMainWindow()
{
	return reinterpret_cast<MainWindow*>(Termbox::GetWidget(m_mainId));
}

std::size_t Tab::GetMainWindowId()
{
	return m_mainId;
}

void Tab::SetShouldUpdate(bool v)
{
	m_shouldUpdate = v;
}

bool Tab::ShouldUpdate() const
{
	return m_shouldUpdate;
}
