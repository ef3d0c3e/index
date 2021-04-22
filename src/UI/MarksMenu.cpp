#include "MarksMenu.hpp"
#include "List.hpp"
#include "../MainWindow.hpp"
#include "../Settings.hpp"

MarksMenu::MarksMenu(MainWindow* main):
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
		{Settings::Keys::Marks::marks,             Settings::Style::Menu::marks_menu},
		{U"Show the marks menu",                   Settings::Style::Menu::marks_menu},
		{Settings::Keys::Marks::select,            Settings::Style::Menu::marks_menu},
		{U"Add file to selection",                 Settings::Style::Menu::marks_menu},
		{Settings::Keys::Marks::select_toggle_all, Settings::Style::Menu::marks_menu},
		{U"Toggle all files for selection",        Settings::Style::Menu::marks_menu},
		{Settings::Keys::Marks::unselect_all,      Settings::Style::Menu::marks_menu},
		{U"Unselect all files",                    Settings::Style::Menu::marks_menu},
		{Settings::Keys::Marks::tag,               Settings::Style::Menu::marks_menu},
		{U"Tag current file",                      Settings::Style::Menu::marks_menu},
		{Settings::Keys::Marks::fav,               Settings::Style::Menu::marks_menu},
		{U"Add current file to favorite",          Settings::Style::Menu::marks_menu},
	});

	// * Keybindings
	// Open menu
	AddKeyboardInput(Settings::Keys::Marks::menu, [this]
	{
		if (IsVisible())
			return;

		Termbox::GetContext().dontResetRepeat = true;
		ActionShow();
	});

	// Operations on List
	m_main->GetList()->AddKeyboardInput(Settings::Keys::Marks::select, [this]
	{
		if (Termbox::GetContext().hasRepeat)
			m_main->GetList()->ActionMarkN(Termbox::GetContext().repeat, MarkType::SELECTED);
		else
			m_main->GetList()->ActionMarkN(1, MarkType::SELECTED);
	});

	m_main->GetList()->AddKeyboardInput(Settings::Keys::Marks::select_toggle_all, [this]
	{
		for (std::size_t i = 0; i < m_main->GetList()->Size(); ++i)
			m_main->GetList()->MarkFn(i, MarkType::SELECTED);
	});

	m_main->GetList()->AddKeyboardInput(Settings::Keys::Marks::unselect_all, [this]
	{
		for (std::size_t i = 0; i < m_main->GetList()->Size(); ++i)
			if (m_main->GetList()->Get(i).first.mark & MarkType::SELECTED)
				m_main->GetList()->MarkFn(i, MarkType::SELECTED);
	});

	m_main->GetList()->AddKeyboardInput(Settings::Keys::Marks::tag, [this]
	{
		if (Termbox::GetContext().hasRepeat)
			m_main->GetList()->ActionMarkN(Termbox::GetContext().repeat, MarkType::TAGGED);
		else
			m_main->GetList()->ActionMarkN(1, MarkType::TAGGED);
	});

	m_main->GetList()->AddKeyboardInput(Settings::Keys::Marks::fav, [this]
	{
		if (Termbox::GetContext().hasRepeat)
			m_main->GetList()->ActionMarkN(Termbox::GetContext().repeat, MarkType::FAV);
		else
			m_main->GetList()->ActionMarkN(1, MarkType::FAV);
	});

	// * Event
	OnStopShowing.AddEvent([this]
	{
		m_main->Invalidate();
	}, EventWhen::AFTER);
}

MarksMenu::~MarksMenu()
{

}

void MarksMenu::Resize(Vec2i dim)
{
	const auto [w, h] = dim;
	SetPosition(Vec2i(0, h-1-GetHeight()));
	SetSize(Vec2i(w, GetHeight()));
}
