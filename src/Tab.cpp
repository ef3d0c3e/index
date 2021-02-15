#include "Tab.hpp"

std::vector<Tab> Tabs;
// Load settings TODO

Tab::Tab():
	m_parentEnabled(true)
{
}

Tab::~Tab()
{
	
}

void Tab::SetMainWindow(MainWindow* main)
{
	m_main = main;
}

void Tab::SetParentEnabled(bool enabled)
{
	OnSetParentEnabled.Notify<EventWhen::BEFORE>(enabled);
	
	m_parentEnabled = enabled;

	m_main->Resize(Termbox::GetDim());

	OnSetParentEnabled.Notify<EventWhen::AFTER>(enabled);
}

bool Tab::GetParentEnabled() const
{
	return m_parentEnabled;
}
