#include "YankMenu.hpp"
#include "List.hpp"
#include "../Actions/Jobs.hpp"
#include "../MainWindow.hpp"
#include "../Settings.hpp"

YankMenu::YankMenu(MainWindow* main):
	Menu(),
	m_main(main)
{
	// * Menu Entries
	SetTable(2,
	{
		{{U"Key",    Settings::Style::Menu::yank_menu_categories}, 15},
		{{U"Action", Settings::Style::Menu::yank_menu_categories}, 85},
	},
	{
		{Settings::Keys::Yank::yank_set,                     Settings::Style::Menu::yank_menu},
		{U"Set the clipboard to the list of selected files", Settings::Style::Menu::yank_menu},
		{Settings::Keys::Yank::yank_add,                     Settings::Style::Menu::yank_menu},
		{U"Add selected files to the clipboard",             Settings::Style::Menu::yank_menu},
		{Settings::Keys::Yank::yank_remove,                  Settings::Style::Menu::yank_menu},
		{U"Remove selected files from the clipboard",        Settings::Style::Menu::yank_menu},
		{Settings::Keys::Yank::yank_toggle,                  Settings::Style::Menu::yank_menu},
		{U"Toggle selected files in the clipboard",          Settings::Style::Menu::yank_menu},
	});

	// * Keybindings
	// Open menu
	AddKeyboardInput(Settings::Keys::Yank::menu, [this]
	{
		if (IsVisible())
			return;

		Termbox::GetContext().dontResetRepeat = true;
		ActionShow();
	});

	// Operations on List
	AddKeyboardInput(Settings::Keys::Yank::yank_set, [this]
	{
		std::size_t selected = m_main->GetList()->GetSelected();

		gClipboard.clear(); // always clear
		m_main->GetList()->Apply([](std::size_t, File* f, FileMatch) // Clear already selected files
		{
			f->mark &= ~(MarkType::CLIP_CUT | MarkType::CLIP_YANK);
		});
		gClipboardMode = ClipboardMode::YANK;

		if (selected != 0)
		{
			m_main->GetList()->Apply([](std::size_t, File* f, FileMatch)
			{
				if (f->mark & MarkType::SELECTED)
					f->mark |= MarkType::CLIP_YANK;
			});
		}
		else
		{
			if (Termbox::GetContext().hasRepeat)
				m_main->GetList()->ActionMarkN(Termbox::GetContext().repeat, MarkType::CLIP_YANK, Widgets::MarkFnAction::SET);
			else
				m_main->GetList()->ActionMarkN(1, MarkType::CLIP_YANK, Widgets::MarkFnAction::SET);
		}
		m_main->GetList()->UpdateMarkings();
	});

	AddKeyboardInput(Settings::Keys::Yank::yank_add, [this]
	{
		std::size_t selected = m_main->GetList()->GetSelected();

		if (gClipboardMode == ClipboardMode::CUT)
		{
			gClipboard.clear();
			m_main->GetList()->Apply([](std::size_t, File* f, FileMatch)
			{
				f->mark &= ~MarkType::CLIP_CUT;
			});
		}
		gClipboardMode = ClipboardMode::YANK;

		if (selected != 0)
		{
			m_main->GetList()->Apply([](std::size_t, File* f, FileMatch)
			{
				if (f->mark & MarkType::SELECTED)
					f->mark |= MarkType::CLIP_YANK;
			});
		}
		else
		{
			if (Termbox::GetContext().hasRepeat)
				m_main->GetList()->ActionMarkN(Termbox::GetContext().repeat, MarkType::CLIP_YANK, Widgets::MarkFnAction::SET);
			else
				m_main->GetList()->ActionMarkN(1, MarkType::CLIP_YANK, Widgets::MarkFnAction::SET);
		}
		m_main->GetList()->UpdateMarkings();
	});

	AddKeyboardInput(Settings::Keys::Yank::yank_remove, [this]
	{
		if (gClipboardMode == ClipboardMode::CUT) // abort
			return;
		std::size_t selected = m_main->GetList()->GetSelected();

		if (selected != 0)
		{
			m_main->GetList()->Apply([](std::size_t, File* f, FileMatch)
			{
				if (!(f->mark & MarkType::SELECTED))
					return;
				f->mark &= ~(MarkType::CLIP_YANK);
			});
		}
		else
		{
			if (Termbox::GetContext().hasRepeat)
				m_main->GetList()->ActionMarkN(Termbox::GetContext().repeat, MarkType::CLIP_YANK, Widgets::MarkFnAction::REMOVE);
			else
				m_main->GetList()->ActionMarkN(1, MarkType::CLIP_YANK, Widgets::MarkFnAction::REMOVE);
		}
		m_main->GetList()->UpdateMarkings();
	});

	AddKeyboardInput(Settings::Keys::Yank::yank_toggle, [this]
	{
		std::size_t selected = m_main->GetList()->GetSelected();
		if (gClipboardMode == ClipboardMode::CUT) // clear
		{
			gClipboard.clear();
			m_main->GetList()->Apply([](std::size_t, File* f, FileMatch)
			{
				f->mark &= ~MarkType::CLIP_CUT;
			});
		}
		gClipboardMode = ClipboardMode::YANK;

		if (selected != 0)
		{
			m_main->GetList()->Apply([](std::size_t, File* f, FileMatch)
			{
				if (!(f->mark & MarkType::SELECTED))
					return;

				if (f->mark & MarkType::CLIP_YANK)
					f->mark &= ~(MarkType::CLIP_YANK);
				else
					f->mark |= MarkType::CLIP_YANK;
			});
		}
		else
		{
			if (Termbox::GetContext().hasRepeat)
				m_main->GetList()->ActionMarkN(Termbox::GetContext().repeat, MarkType::CLIP_YANK, Widgets::MarkFnAction::TOGGLE);
			else
				m_main->GetList()->ActionMarkN(1, MarkType::CLIP_YANK, Widgets::MarkFnAction::TOGGLE);
		}
		m_main->GetList()->UpdateMarkings();
	});

	// * Event
	OnStopShowing.AddEvent([this]
	{
		m_main->Invalidate();
	}, EventWhen::AFTER);
}

YankMenu::~YankMenu()
{

}

void YankMenu::Resize(Vec2i dim)
{
	const auto [w, h] = dim;
	SetPosition(Vec2i(0, h-1-GetHeight()));
	SetSize(Vec2i(w, GetHeight()));
}
