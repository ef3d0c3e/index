#include "SortMenu.hpp"
#include "../MainWindow.hpp"
#include "../Settings.hpp"

SortMenu::SortMenu(MainWindow* main):
	Menu(),
	m_main(main)
{
	SetTable(3,
	{
		{{U"Key",     Settings::Style::Menu::sort_menu_categories}, 15},
		{{U"Status",  Settings::Style::Menu::sort_menu_categories}, 15},
		{{U"Action",  Settings::Style::Menu::sort_menu_categories}, 70},
	},
	{
		{Settings::Keys::Sort::sort_basename,  Settings::Style::Menu::sort_menu},
		Settings::Style::Menu::sort_menu_selected,
		{U"Basename",                          Settings::Style::Menu::sort_menu},
		{Settings::Keys::Sort::sort_size,      Settings::Style::Menu::sort_menu},
		Settings::Style::Menu::sort_menu_none,
		{U"Size",                              Settings::Style::Menu::sort_menu},
		{Settings::Keys::Sort::sort_atime,     Settings::Style::Menu::sort_menu},
		Settings::Style::Menu::sort_menu_none,
		{U"Access Time",                       Settings::Style::Menu::sort_menu},
		{Settings::Keys::Sort::sort_mtime,     Settings::Style::Menu::sort_menu},
		Settings::Style::Menu::sort_menu_none,
		{U"Modification Time",                 Settings::Style::Menu::sort_menu},
		{Settings::Keys::Sort::sort_ext,       Settings::Style::Menu::sort_menu},
		Settings::Style::Menu::sort_menu_none,
		{U"Extension",                         Settings::Style::Menu::sort_menu},

		{Settings::Keys::Sort::reverse,        Settings::Style::Menu::sort_menu},
		Settings::Style::Menu::sort_menu_false,
		{U"Reverse",                           Settings::Style::Menu::sort_menu},
		{Settings::Keys::Sort::dir_first,      Settings::Style::Menu::sort_menu},
		Settings::Style::Menu::sort_menu_true,
		{U"Directories first",                 Settings::Style::Menu::sort_menu},
		{Settings::Keys::Sort::case_sensitive, Settings::Style::Menu::sort_menu},
		Settings::Style::Menu::sort_menu_false,
		{U"Case sensitive",                    Settings::Style::Menu::sort_menu},
	});

	AddKeyboardInput(Settings::Keys::Sort::menu, [this]
	{
		Termbox::GetContext().dontResetRepeat = true;
		ActionShow();
	});

	AddKeyboardInput(Settings::Keys::Sort::sort_basename, [this]
	{
		auto settings = m_main->GetSortSettings();
		const auto oldId = settings.SortFn;
		settings.SortFn = SortFnID::BASENAME;
		m_main->SetSortSettings(settings);

		// Update
		GetEntries()[3*oldId+1] = Settings::Style::Menu::sort_menu_none;
		GetEntries()[3*settings.SortFn+1] = Settings::Style::Menu::sort_menu_selected;
	});

	AddKeyboardInput(Settings::Keys::Sort::sort_size, [this]
	{
		auto settings = m_main->GetSortSettings();
		const auto oldId = settings.SortFn;
		settings.SortFn = SortFnID::SIZE;
		m_main->SetSortSettings(settings);

		// Update
		GetEntries()[3*oldId+1] = Settings::Style::Menu::sort_menu_none;
		GetEntries()[3*settings.SortFn+1] = Settings::Style::Menu::sort_menu_selected;
	});

	AddKeyboardInput(Settings::Keys::Sort::sort_atime, [this]
	{
		auto settings = m_main->GetSortSettings();
		const auto oldId = settings.SortFn;
		settings.SortFn = SortFnID::ACCESS_TIME;
		m_main->SetSortSettings(settings);

		// Update
		GetEntries()[3*oldId+1] = Settings::Style::Menu::sort_menu_none;
		GetEntries()[3*settings.SortFn+1] = Settings::Style::Menu::sort_menu_selected;
	});

	AddKeyboardInput(Settings::Keys::Sort::sort_mtime, [this]
	{
		auto settings = m_main->GetSortSettings();
		const auto oldId = settings.SortFn;
		settings.SortFn = SortFnID::MDOFICATION_TIME;
		m_main->SetSortSettings(settings);

		// Update
		GetEntries()[3*oldId+1] = Settings::Style::Menu::sort_menu_none;
		GetEntries()[3*settings.SortFn+1] = Settings::Style::Menu::sort_menu_selected;
	});

	AddKeyboardInput(Settings::Keys::Sort::sort_ext, [this]
	{
		auto settings = m_main->GetSortSettings();
		const auto oldId = settings.SortFn;
		settings.SortFn = SortFnID::EXTENSION;
		m_main->SetSortSettings(settings);

		// Update
		GetEntries()[3*oldId+1] = Settings::Style::Menu::sort_menu_none;
		GetEntries()[3*settings.SortFn+1] = Settings::Style::Menu::sort_menu_selected;
	});

	AddKeyboardInput(Settings::Keys::Sort::reverse, [this]
	{
		auto settings = m_main->GetSortSettings();
		settings.Reverse = !settings.Reverse;
		m_main->SetSortSettings(settings);

		// Update
		GetEntries()[3*5+1] = (settings.Reverse)
		? Settings::Style::Menu::show_menu_true : Settings::Style::Menu::show_menu_false;
	});

	AddKeyboardInput(Settings::Keys::Sort::dir_first, [this]
	{
		auto settings = m_main->GetSortSettings();
		settings.DirFist = !settings.DirFist;
		m_main->SetSortSettings(settings);

		// Update
		GetEntries()[3*6+1] = (settings.DirFist)
		? Settings::Style::Menu::show_menu_true : Settings::Style::Menu::show_menu_false;
	});

	AddKeyboardInput(Settings::Keys::Sort::case_sensitive, [this]
	{
		auto settings = m_main->GetSortSettings();
		settings.CaseSensitive = !settings.CaseSensitive;
		m_main->SetSortSettings(settings);

		// Update
		GetEntries()[3*7+1] = (settings.CaseSensitive)
		? Settings::Style::Menu::show_menu_true : Settings::Style::Menu::show_menu_false;
	});

	OnStopShowing.AddEvent([this]
	{
		m_main->Invalidate();
	}, EventWhen::AFTER);
}

SortMenu::~SortMenu()
{

}

void SortMenu::Resize(Vec2i dim)
{
	const auto [w, h] = dim;
	SetPosition(Vec2i(0, h-1-GetHeight()));
	SetSize(Vec2i(w, GetHeight()));
}
