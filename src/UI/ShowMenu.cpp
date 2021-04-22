#include "ShowMenu.hpp"
#include "List.hpp"
#include "../MainWindow.hpp"
#include "../Settings.hpp"
#include "MarksExplorer.hpp"

ShowMenu::ShowMenu(MainWindow* main):
	Menu(),
	m_main(main)
{
	// * Menu Entries
	SetTable(3,
	{
		{{U"Key",    Settings::Style::Menu::show_menu_categories}, 15},
		{{U"Status", Settings::Style::Menu::show_menu_categories}, 15},
		{{U"Action", Settings::Style::Menu::show_menu_categories}, 70},
	},
	{
		{Settings::Keys::Show::hidden, Settings::Style::Menu::show_menu},
		Settings::Style::Menu::show_menu_false,
		{U"Toggle hidden files",       Settings::Style::Menu::show_menu},
		{Settings::Keys::Show::parent, Settings::Style::Menu::show_menu},
		Settings::Style::Menu::show_menu_true,
		{U"Toggle parent pane",        Settings::Style::Menu::show_menu},
	});

	// * Keybindings
	// Open menu
	AddKeyboardInput(Settings::Keys::Show::menu, [this]
	{
		if (IsVisible())
			return;

		Termbox::GetContext().dontResetRepeat = true;
		ActionShow();
	});

	AddKeyboardInput(Settings::Keys::Show::hidden, [this]
	{
		//TODO: Store the marks?
		const String filename = m_main->GetCurrentFileName();
		auto dirFilter = m_main->m_dir->GetFilter();
		auto parentFilter = m_main->m_parent->GetFilter();
		const bool v = !dirFilter.HiddenFiles;

		// Set the new filter
		dirFilter.HiddenFiles = v;
		parentFilter.HiddenFiles = v;
		m_main->m_dir->SetFilter(dirFilter);
		m_main->m_parent->SetFilter(parentFilter);

		// Update the list
		m_main->m_dir->UpdateFromDir();
		m_main->m_parent->UpdateFromDir();

		// Reload the marks
		m_main->m_marksExplorer->SetMarks(m_main->m_dir->GetDir());

		// try to get the position
		m_main->m_dir->ActionSetPosition(m_main->GetFilePosition(filename));

		// Update
		GetEntries()[3*0+1] = v ? Settings::Style::Menu::show_menu_true : Settings::Style::Menu::show_menu_false;
	});

	AddKeyboardInput(Settings::Keys::Show::parent, [this]
	{
		m_main->m_settings.showParent = !m_main->m_settings.showParent;
		m_main->m_parent->SetVisible(m_main->ShouldShowParent());
		m_main->Invalidate();

		// Update
		GetEntries()[3*1+1] = (m_main->m_parent->IsVisible())
		? Settings::Style::Menu::show_menu_true : Settings::Style::Menu::show_menu_false;
	});

	// * Event
	OnStopShowing.AddEvent([this]
	{
		m_main->Invalidate();
	}, EventWhen::AFTER);
}

ShowMenu::~ShowMenu()
{

}

void ShowMenu::Resize(Vec2i dim)
{
	const auto [w, h] = dim;
	SetPosition(Vec2i(0, h-1-GetHeight()));
	SetSize(Vec2i(w, GetHeight()));
}
