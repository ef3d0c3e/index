#include "ChangeMenu.hpp"
#include "../List.hpp"
#include "../MainWindow.hpp"
#include "../Settings.hpp"

ChangeMenu::ChangeMenu(MainWindow* main):
	Menu(),
	m_main(main)
{
	// * Menu Entries
	SetTable(2,
	{
		{{U"Key",    Settings::Style::Menu::marks_menu_categories}, 15},
		{{U"Action", Settings::Style::Menu::marks_menu_categories}, 85},
	},
	{
		{Settings::Keys::Change::directory,  Settings::Style::Menu::change_menu},
		{U"Change directory",                Settings::Style::Menu::change_menu},
		{Settings::Keys::Change::name,       Settings::Style::Menu::change_menu},
		{U"Modify name",                     Settings::Style::Menu::change_menu},
		{Settings::Keys::Change::name_empty, Settings::Style::Menu::change_menu},
		{U"Set name",                        Settings::Style::Menu::change_menu},
	});

	// * Keybindings
	// Open menu
	AddKeyboardInput(Settings::Keys::Change::menu, [this]
	{
		if (IsVisible())
			return;

		Termbox::GetContext().dontResetRepeat = true;
		ActionShow();
	});

	AddKeyboardInput(Settings::Keys::Change::directory, [&]
	{
		m_main->ActionPrompt([&](const String& input)
		{
			m_main->CD(Util::StringConvert<char>(input));
		}, Settings::Style::Change::change_dir_prompt_prefix, Settings::Style::Change::change_dir_prompt_bg, Settings::Style::Change::change_dir_prompt_max);
	});

	AddKeyboardInput(Settings::Keys::Change::name, [&]
	{
		// Whenever a file gets renamed, its Directory entry gets changed too.
		// However the its List entry does not, therefore we to need to update it
		// and also send an signal to every tabs that display this file. We don't
		// need to update every single tab every time we rename a file, we can set
		// a boolean in the tab structure to inform the tab to UpdateFromDir()
		// whenever it gets switched as the current tab.
		if (m_main->m_dir->GetEntries() == 0) [[unlikely]]
			return;

		const String cur = m_main->GetCurrentFileName();
		if (cur.empty())  [[unlikely]]
			return;

		m_main->ActionPrompt([&, cur](const String& input)
		{
			if (input.empty()) [[unlikely]]
				return;
			try
			{
				m_main->m_dir->GetDir()->Rename(cur, input);
			}
			catch (IndexError& e)
			{
				m_main->Error(e.GetMessage());
			}
		}, Settings::Style::Change::change_name_prompt_prefix, Settings::Style::Change::change_name_prompt_bg, Settings::Style::Change::change_name_prompt_max, cur, cur.size());
	});

	AddKeyboardInput(Settings::Keys::Change::name_empty, [&]
	{
		if (m_main->m_dir->GetEntries() == 0) [[unlikely]]
			return;

		const String cur = m_main->GetCurrentFileName();
		if (cur.empty())  [[unlikely]]
			return;

		m_main->ActionPrompt([&, cur](const String& input)
		{
			if (input.empty()) [[unlikely]]
				return;
			try
			{
				m_main->m_dir->GetDir()->Rename(cur, input);
			}
			catch (IndexError& e)
			{
				m_main->Error(e.GetMessage());
			}
		}, Settings::Style::Change::change_name_prompt_prefix, Settings::Style::Change::change_name_prompt_bg, Settings::Style::Change::change_name_prompt_max);
	});


	// * Event
	OnStopShowing.AddEvent([this]
	{
		m_main->Invalidate();
	}, EventWhen::AFTER);
}

ChangeMenu::~ChangeMenu()
{

}

void ChangeMenu::Resize(Vec2i dim)
{
	const auto [w, h] = dim;
	SetPosition(Vec2i(0, h-1-GetHeight()));
	SetSize(Vec2i(w, GetHeight()));
}
