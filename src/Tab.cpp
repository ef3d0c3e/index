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
	// TODO: inotify
}

void Tab::Delete()
{
	// Remove widget from Termbox & Update all window's id
	MainWindow* win = reinterpret_cast<MainWindow*>(Termbox::RemoveWidget(m_mainId, true));
	if (win == nullptr) // abort
		throw Util::Exception("Tab::Delete() Tab had invalid window id");
	for (auto it = gTabs.begin()+m_mainId+1; it != gTabs.end(); ++it)
	{
		--it->m_mainId;
		it->GetMainWindow()->SetTabID(it->GetMainWindow()->GetTabID()-1);
	}
	gTabs.erase(gTabs.begin()+m_mainId);
}
