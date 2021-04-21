#include "GoMenu.hpp"
#include "../MainWindow.hpp"
#include "../Settings.hpp"

GoMenu::GoMenu(MainWindow* main):
	Menu(),
	m_main(main)
{
	SetTable(2,
	{
		{{U"Key",    Settings::Style::Menu::go_menu_categories}, 15},
		{{U"Action", Settings::Style::Menu::go_menu_categories}, 85},
	},
	{
		{Settings::Keys::Go::top,       Settings::Style::Menu::go_menu},
		{U"Go to the top",              Settings::Style::Menu::go_menu},
		{Settings::Keys::Go::bottom,    Settings::Style::Menu::go_menu},
		{U"Go to the bottom",           Settings::Style::Menu::go_menu},
		{Settings::Keys::Go::home,      Settings::Style::Menu::go_menu},
		{U"Go to ~",                    Settings::Style::Menu::go_menu},
		{Settings::Keys::Go::root,      Settings::Style::Menu::go_menu},
		{U"Go to /",                    Settings::Style::Menu::go_menu},
		{Settings::Keys::Go::tab_next,  Settings::Style::Menu::go_menu},
		{U"Next tab",                   Settings::Style::Menu::go_menu},
		{Settings::Keys::Go::tab_prev,  Settings::Style::Menu::go_menu},
		{U"Previous tab",               Settings::Style::Menu::go_menu},
		{Settings::Keys::Go::tab_new,   Settings::Style::Menu::go_menu},
		{U"New tab",                    Settings::Style::Menu::go_menu},
		{Settings::Keys::Go::tab_close, Settings::Style::Menu::go_menu},
		{U"Close tab",                  Settings::Style::Menu::go_menu},
	});

	AddKeyboardInput(Settings::Keys::Go::menu, [this]
	{
		Termbox::GetContext().dontResetRepeat = true;
		ActionShow();
	});

	OnStopShowing.AddEvent([this]
	{
		m_main->Invalidate();
	}, EventWhen::AFTER);
}

GoMenu::~GoMenu()
{ }

void GoMenu::Resize(Vec2i dim)
{
	const auto [w, h] = dim;
	SetPosition(Vec2i(0, h-1-GetHeight()));
	SetSize(Vec2i(w, GetHeight()));
}
