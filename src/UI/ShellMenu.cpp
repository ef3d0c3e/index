#include "ShellMenu.hpp"
#include "../Actions/Open.hpp"
#include "../MainWindow.hpp"
#include "../Settings.hpp"


ShellMenu::ShellMenu(MainWindow* main):
	Menu(),
	m_main(main)
{
	SetTable(2,
	{
		{{U"Key",    Settings::Style::Menu::shell_menu_categories}, 15},
		{{U"Action", Settings::Style::Menu::shell_menu_categories}, 85},
	},
	{
		{Settings::Keys::Shell::shell_at,   Settings::Style::Menu::shell_menu},
		{U"Open shell",                     Settings::Style::Menu::shell_menu},
		{Settings::Keys::Shell::shell,      Settings::Style::Menu::shell_menu},
		{U"Run shell",                      Settings::Style::Menu::shell_menu},
		{Settings::Keys::Shell::shell_file, Settings::Style::Menu::shell_menu},
		{U"Open shell with filename",       Settings::Style::Menu::shell_menu},
	});

	AddKeyboardInput(Settings::Keys::Shell::menu, [this]
	{
		Termbox::GetContext().dontResetRepeat = true;
		ActionShow();
	});

	AddKeyboardInput(Settings::Keys::Shell::shell_at, [this]
	{
		try
		{
			Actions::OpenShell(m_main->GetCurrentPath());
		}
		catch (IndexError& e)
		{
			m_main->Error(U"Error: " + e.GetMessage());
		}
	});

	AddKeyboardInput(Settings::Keys::Shell::shell, [this]
	{
		m_main->ActionPrompt([&](const String& input)
		{
			try
			{
				Actions::CustomOpen(m_main->GetCurrentFileName(), m_main->GetCurrentPath(), Util::StringConvert<char>(input));
			}
			catch (IndexError& e)
			{
				m_main->Error(U"Error: " + e.GetMessage());
			}
		}, Settings::Style::Shell::shell_prompt_prefix, Settings::Style::Shell::shell_prompt_background, Settings::Style::Shell::shell_prompt_max_length, U"", 0);
	});

	AddKeyboardInput(Settings::Keys::Shell::shell_file, [this]
	{
		m_main->ActionPrompt([&](const String& input)
		{
			try
			{
				Actions::CustomOpen(m_main->GetCurrentFileName(), m_main->GetCurrentPath(), Util::StringConvert<char>(input));
			}
			catch (IndexError& e)
			{
				m_main->Error(U"Error: " + e.GetMessage());
			}
		}, Settings::Style::Shell::shell_prompt_prefix, Settings::Style::Shell::shell_prompt_background, Settings::Style::Shell::shell_prompt_max_length, U" {}", 0);
	});

	OnStopShowing.AddEvent([this]
	{
		m_main->Invalidate();
	}, EventWhen::AFTER);
}

ShellMenu::~ShellMenu()
{

}

void ShellMenu::Resize(Vec2i dim)
{
	const auto [w, h] = dim;
	SetPosition(Vec2i(0, h-1-GetHeight()));
	SetSize(Vec2i(w, GetHeight()));
}
