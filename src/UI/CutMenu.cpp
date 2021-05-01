#include "CutMenu.hpp"
#include "List.hpp"
#include "../Actions/Jobs.hpp"
#include "../MainWindow.hpp"
#include "../Settings.hpp"

CutMenu::CutMenu(MainWindow* main):
	Menu(),
	m_main(main)
{
	// * Menu Entries
	SetTable(2,
	{
		{{U"Key",    Settings::Style::Menu::cut_menu_categories}, 15},
		{{U"Action", Settings::Style::Menu::cut_menu_categories}, 85},
	},
	{
		{Settings::Keys::Cut::cut_set,                       Settings::Style::Menu::cut_menu},
		{U"Set the clipboard to the list of selected files", Settings::Style::Menu::cut_menu},
		{Settings::Keys::Cut::cut_add,                       Settings::Style::Menu::cut_menu},
		{U"Add selected files to the clipboard",             Settings::Style::Menu::cut_menu},
		{Settings::Keys::Cut::cut_remove,                    Settings::Style::Menu::cut_menu},
		{U"Remove selected files from the clipboard",        Settings::Style::Menu::cut_menu},
		{Settings::Keys::Cut::cut_toggle,                    Settings::Style::Menu::cut_menu},
		{U"Toggle selected files in the clipboard",          Settings::Style::Menu::cut_menu},
	});

	// * Keybindings
	// Open menu
	AddKeyboardInput(Settings::Keys::Cut::menu, [this]
	{
		if (IsVisible())
			return;

		Termbox::GetContext().dontResetRepeat = true;
		ActionShow();
	});

	// Operations on List
	AddKeyboardInput(Settings::Keys::Cut::cut_set, [this]
	{
		std::size_t selected = m_main->GetList()->GetSelected();

		gClipboard.clear(); // always clear
		m_main->GetList()->Apply([](std::size_t, File* f, FileMatch) // Clear already selected files
		{
			f->mark &= ~(MarkType::CLIP_CUT | MarkType::CLIP_YANK);
		});
		gClipboardMode = ClipboardMode::CUT;

		if (selected != 0)
		{
			m_main->GetList()->Apply([](std::size_t, File* f, FileMatch)
			{
				if (f->mark & MarkType::SELECTED)
					f->mark |= MarkType::CLIP_CUT;
			});
		}
		else
		{
			if (Termbox::GetContext().hasRepeat)
				m_main->GetList()->ActionMarkN(Termbox::GetContext().repeat, MarkType::CLIP_CUT, Widgets::MarkFnAction::SET);
			else
				m_main->GetList()->ActionMarkN(1, MarkType::CLIP_CUT, Widgets::MarkFnAction::SET);
		}
		m_main->GetList()->UpdateMarkings();
	});

	AddKeyboardInput(Settings::Keys::Cut::cut_add, [this]
	{
		std::size_t selected = m_main->GetList()->GetSelected();

		if (gClipboardMode == ClipboardMode::YANK)
		{
			gClipboard.clear();
			m_main->GetList()->Apply([](std::size_t, File* f, FileMatch)
			{
				f->mark &= ~MarkType::CLIP_YANK;
			});
		}
		gClipboardMode = ClipboardMode::CUT;

		if (selected != 0)
		{
			m_main->GetList()->Apply([](std::size_t, File* f, FileMatch)
			{
				if (f->mark & MarkType::SELECTED)
					f->mark |= MarkType::CLIP_CUT;
			});
		}
		else
		{
			if (Termbox::GetContext().hasRepeat)
				m_main->GetList()->ActionMarkN(Termbox::GetContext().repeat, MarkType::CLIP_CUT, Widgets::MarkFnAction::SET);
			else
				m_main->GetList()->ActionMarkN(1, MarkType::CLIP_CUT, Widgets::MarkFnAction::SET);
		}
		m_main->GetList()->UpdateMarkings();
	});

	AddKeyboardInput(Settings::Keys::Cut::cut_remove, [this]
	{
		if (gClipboardMode == ClipboardMode::YANK) // abort
			return;
		std::size_t selected = m_main->GetList()->GetSelected();

		if (selected != 0)
		{
			m_main->GetList()->Apply([](std::size_t, File* f, FileMatch)
			{
				if (!(f->mark & MarkType::SELECTED))
					return;
				f->mark &= ~(MarkType::CLIP_CUT);
			});
		}
		else
		{
			if (Termbox::GetContext().hasRepeat)
				m_main->GetList()->ActionMarkN(Termbox::GetContext().repeat, MarkType::CLIP_CUT, Widgets::MarkFnAction::REMOVE);
			else
				m_main->GetList()->ActionMarkN(1, MarkType::CLIP_CUT, Widgets::MarkFnAction::REMOVE);
		}
		m_main->GetList()->UpdateMarkings();
	});

	AddKeyboardInput(Settings::Keys::Cut::cut_toggle, [this]
	{
		std::size_t selected = m_main->GetList()->GetSelected();
		if (gClipboardMode == ClipboardMode::YANK) // clear
		{
			gClipboard.clear();
			m_main->GetList()->Apply([](std::size_t, File* f, FileMatch)
			{
				f->mark &= ~MarkType::CLIP_YANK;
			});
		}
		gClipboardMode = ClipboardMode::CUT;

		if (selected != 0)
		{
			m_main->GetList()->Apply([](std::size_t, File* f, FileMatch)
			{
				if (!(f->mark & MarkType::SELECTED))
					return;

				if (f->mark & MarkType::CLIP_CUT)
					f->mark &= ~(MarkType::CLIP_CUT);
				else
					f->mark |= MarkType::CLIP_CUT;
			});
		}
		else
		{
			if (Termbox::GetContext().hasRepeat)
				m_main->GetList()->ActionMarkN(Termbox::GetContext().repeat, MarkType::CLIP_CUT, Widgets::MarkFnAction::TOGGLE);
			else
				m_main->GetList()->ActionMarkN(1, MarkType::CLIP_CUT, Widgets::MarkFnAction::TOGGLE);
		}
		m_main->GetList()->UpdateMarkings();
	});

	// * Event
	OnStopShowing.AddEvent([this]
	{
		m_main->Invalidate();
	}, EventWhen::AFTER);
}

CutMenu::~CutMenu()
{

}

void CutMenu::Resize(Vec2i dim)
{
	const auto [w, h] = dim;
	SetPosition(Vec2i(0, h-1-GetHeight()));
	SetSize(Vec2i(w, GetHeight()));
}
