#include "MainWindow.hpp"
#include "List.hpp"
#include "Tab.hpp"
#include "Tabline.hpp"
#include "Statusline.hpp"
#include "Menu.hpp"
#include "UI/MarksMenu.hpp"
#include "Prompt.hpp"
#include "DirectoryCache.hpp"
#include "UI/CacheExplorer.hpp"
#include "UI/PositionExplorer.hpp"
#include "UI/SortMenu.hpp"
#include "UI/ShellMenu.hpp"

MainWindow::MainWindow(const std::string& path, std::size_t tabId):
	Window::Window({U"", Settings::Style::default_text_style}),
	m_positionCache(),
	m_currentMode(CurrentMode::NORMAL),
	m_tab(tabId),
	m_sortFn(0),
	// Settings
	m_parentEnabled(true)
{
	SetSize(Termbox::GetDim()-Vec2i(1, 1));
	SetPosition(Vec2i(0, 0));

	SetBorderFlags(0x00);
	SetBackground(Settings::Style::main_window_background, false, false);

	dInput = new DummyInput();
	AddWidget(dInput);

	// General input
	dInput->AddKeyboardInput(Settings::Keyboard::quit, [](){ Termbox::GetTermbox().GetContext().stop = true; });

	// {{{ Tab
	dInput->AddKeyboardInput(Settings::Keys::Go::tab_new, [this]()
	{
		SetVisible(false);
		SetActive(false);
		m_marks->AddMarks(m_dir->GetDir());

		Tab tab;
		Tabs.push_back(std::move(tab));
		MainWindow* main = new MainWindow(m_dir->GetDir()->GetPath(), Tabs.size()-1);
		Tabs[Tabs.size()-1].SetMainWindow(Termbox::GetTermbox().Termbox::AddWidget(main));
	});

	dInput->AddKeyboardInput(Settings::Keys::Go::tab_next, [&]()
	{
		if (Tabs.size() == 1)
			return;

		SetVisible(false);
		SetActive(false);
		m_marks->AddMarks(m_dir->GetDir());

		MainWindow* win;
		if (Termbox::GetContext().hasRepeat)
			win = Tabs[(m_tab+Termbox::GetContext().repeat) % Tabs.size()].GetMainWindow();
		else
			win = Tabs[(m_tab+1) % Tabs.size()].GetMainWindow();

		win->SetVisible(true);
		win->SetActive(true);
		win->Invalidate();
		win->m_marks->SetMarks(win->m_dir->GetDir());
		Termbox::GetContext().clear = true;
	});

	dInput->AddKeyboardInput(Settings::Keys::Go::tab_prev, [this]()
	{
		if (Tabs.size() == 1)
			return;

		SetVisible(false);
		SetActive(false);
		m_marks->AddMarks(m_dir->GetDir());

		MainWindow* win;
		if (Termbox::GetContext().hasRepeat)
			win = Tabs[(m_tab-Termbox::GetContext().repeat) % Tabs.size()].GetMainWindow();
		else
			win = Tabs[(m_tab-1) % Tabs.size()].GetMainWindow();

		win->SetVisible(true);
		win->SetActive(true);
		win->Invalidate();
		win->m_marks->SetMarks(win->m_dir->GetDir());
		Termbox::GetContext().clear = true;
	});

	dInput->AddKeyboardInput(Settings::Keys::Go::tab_close, [this]()
	{
		if (Tabs.size() == 1)
			return;

		SetVisible(false);
		SetActive(false);
		m_marks->AddMarks(m_dir->GetDir());

		MainWindow* win = Tabs[(m_tab-1) % Tabs.size()].GetMainWindow();

		gDirectoryCache.DeleteDirectory(m_dir->GetDir());
		gDirectoryCache.DeleteDirectory(m_parent->GetDir());

		Termbox::GetTermbox().Termbox::RemoveWidget(Tabs[m_tab].GetMainWindowId());
		delete Tabs[m_tab].GetMainWindow();
		Tabs.erase(Tabs.begin()+m_tab);

		win->SetVisible(true);
		win->SetActive(true);
		win->Invalidate();
		Termbox::GetContext().clear = true;
	});
	// }}}

	// * Directories
	m_dir = new List(this, path, true);
	m_parent = new List(this, path + "/..", false);
	m_parent->SetVisible(ShouldShowParent());

	m_parentId = AddWidget(m_parent);
	m_dirId = AddWidget(m_dir);

	m_dir->OnChangePosition.AddEvent([this]{ SetWidgetExpired(m_tablineId, true); }, EventWhen::AFTER);
	m_dir->OnChangePosition.AddEvent([this]{ SetWidgetExpired(m_statuslineId, true); }, EventWhen::AFTER);

	// * Tabline & statusline
	m_tabline = new Tabline(this);
	m_tablineId = AddWidget(m_tabline);
	m_statusline = new Statusline(this);
	m_statuslineId = AddWidget(m_statusline);

	// * Prompt
	m_prompt = new Prompt({U"", Settings::Style::default_text_style}, U"");
	m_promptId = AddWidget(m_prompt);
	m_prompt->OnStopShowing.AddEvent([this](bool) {
		RestoreAllActive(std::move(m_promptStateList));
		Invalidate();
		Termbox::GetContext().noRepeat = false;
	}, EventWhen::AFTER);

	// {{{ Go
	// * Menu
	m_goMenu = new Menu();
	m_goMenu->SetTable(2,
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
	m_goMenu->AddKeyboardInput(Settings::Keys::Go::menu, [this](){
			Termbox::GetContext().dontResetRepeat = true;
			m_goMenu->ActionShow();
	});
	m_goMenu->OnStopShowing.AddEvent([this](){
			Invalidate();
	}, EventWhen::AFTER);
	m_goMenuId = AddWidget(m_goMenu);
	// }}}

	// {{{ Marks
	// * Menu
	m_marksMenu = new Menu();
	m_marksMenu->SetTable(2,
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
	m_marksMenu->AddKeyboardInput(Settings::Keys::Marks::menu, [this]()
	{
		if (m_marksMenu->IsVisible())
			return;

		Termbox::GetContext().dontResetRepeat = true;
		m_marksMenu->ActionShow();
	});
	m_marksMenu->OnStopShowing.AddEvent([this](){ this->Invalidate(); }, EventWhen::AFTER);
	m_marksMenuId = AddWidget(m_marksMenu);

	// * Marks List
	m_marks = new MarksMenu(this);
	m_marks->SetVisible(false);
	m_marks->SetActive(false);
	m_marksId = AddWidget(m_marks);

	// * Keybindings
	dInput->AddKeyboardInput(Settings::Keys::Marks::marks, [this](){ SetMode(m_currentMode == CurrentMode::MARKS ? CurrentMode::NORMAL : CurrentMode::MARKS); });
	m_dir->AddKeyboardInput(Settings::Keys::Marks::select, [this]()
	{
		if (Termbox::GetContext().hasRepeat)
			m_dir->ActionMarkN(Termbox::GetContext().repeat, MarkType::SELECTED);
		else
			m_dir->ActionMarkN(1, MarkType::SELECTED);
	});
	m_dir->AddKeyboardInput(Settings::Keys::Marks::select_toggle_all, [this]()
	{
		for (std::size_t i = 0; i < m_dir->Size(); ++i)
			m_dir->MarkFn(i, MarkType::SELECTED);
	});
	m_dir->AddKeyboardInput(Settings::Keys::Marks::unselect_all, [this]()
	{
		for (std::size_t i = 0; i < m_dir->Size(); ++i)
			if (m_dir->Get(i).first.mark & MarkType::SELECTED)
				m_dir->MarkFn(i, MarkType::SELECTED);
	});
	m_dir->AddKeyboardInput(Settings::Keys::Marks::tag, [this]()
	{
		if (Termbox::GetContext().hasRepeat)
			m_dir->ActionMarkN(Termbox::GetContext().repeat, MarkType::TAGGED);
		else
			m_dir->ActionMarkN(1, MarkType::TAGGED);
	});
	m_dir->AddKeyboardInput(Settings::Keys::Marks::fav, [this]()
	{
		if (Termbox::GetContext().hasRepeat)
			m_dir->ActionMarkN(Termbox::GetContext().repeat, MarkType::FAV);
		else
			m_dir->ActionMarkN(1, MarkType::FAV);
	});
	// }}}

	// {{{ Show
	// * Menu
	m_showMenu = new Menu();
	m_showMenu->SetTable(3,
		{
			{{U"Key",    Settings::Style::Menu::show_menu_categories}, 15},
			{{U"Status",    Settings::Style::Menu::show_menu_categories}, 15},
			{{U"Action", Settings::Style::Menu::show_menu_categories}, 70},
		},
		{
			{Settings::Keys::Show::hidden, Settings::Style::Menu::show_menu},
			Settings::Style::Menu::show_menu_false,
			{U"Toggle hidden files", Settings::Style::Menu::show_menu},
			{Settings::Keys::Show::parent, Settings::Style::Menu::show_menu},
			Settings::Style::Menu::show_menu_true,
			{U"Toggle parent pane", Settings::Style::Menu::show_menu},
		});
	m_showMenu->AddKeyboardInput(Settings::Keys::Show::menu, [this]
	{
			// Update some of the values

			Termbox::GetContext().dontResetRepeat = true;
			m_showMenu->ActionShow();
	});
	m_showMenu->OnStopShowing.AddEvent([this]
	{
			Invalidate();
	}, EventWhen::AFTER);
	m_showMenuId = AddWidget(m_showMenu);
	// * Keybindings
	m_dir->AddKeyboardInput(Settings::Keys::Show::hidden, [this]
	{
		//TODO: Store the marks?
		const String filename = m_dir->Get(m_dir->GetPos()).first.name;
		auto dirFilter = m_dir->GetFilter();
		const bool v = !dirFilter.HiddenFiles;

		// Set the new filter
		dirFilter.HiddenFiles = v;
		m_dir->SetFilter(dirFilter);
		auto parentFilter = m_dir->GetFilter();
		parentFilter.HiddenFiles = v;
		m_parent->SetFilter(parentFilter);

		// Update the list
		m_dir->UpdateFromDir();
		m_parent->UpdateFromDir();

		// Reload the marks
		m_marks->SetMarks(m_dir->GetDir());

		// ttry to get the position
		m_dir->ActionSetPosition(GetFilePosition(filename));

		// Update
		m_showMenu->GetEntries()[1] = (v)
		? Settings::Style::Menu::show_menu_true : Settings::Style::Menu::show_menu_false;
	});

	m_dir->AddKeyboardInput(Settings::Keys::Show::parent, [this]
	{
		m_settings.showParent = !m_settings.showParent;
		m_parent->SetVisible(ShouldShowParent());
		Invalidate();

		// Update
		m_showMenu->GetEntries()[4] = (m_parent->IsVisible())
		? Settings::Style::Menu::show_menu_true : Settings::Style::Menu::show_menu_false;
	});
	// }}}

	// {{{ Change
	// * Menu
	m_changeMenu = new Menu();
	m_changeMenu->SetTable(2,
	{
		{{U"Key",    Settings::Style::Menu::change_menu_categories}, 15},
		{{U"Action", Settings::Style::Menu::change_menu_categories}, 85},
	},
	{
			{Settings::Keys::Change::directory,  Settings::Style::Menu::change_menu},
			{U"Change directory",                Settings::Style::Menu::change_menu},
			{Settings::Keys::Change::name,       Settings::Style::Menu::change_menu},
			{U"Modify name",                     Settings::Style::Menu::change_menu},
			{Settings::Keys::Change::name_empty, Settings::Style::Menu::change_menu},
			{U"Set name",                        Settings::Style::Menu::change_menu},
	});
	m_changeMenu->AddKeyboardInput(Settings::Keys::Change::menu, [this]()
	{
		if (m_changeMenu->IsVisible())
			return;

		Termbox::GetContext().dontResetRepeat = true;
		m_changeMenu->ActionShow();
	});
	m_changeMenu->OnStopShowing.AddEvent([this](){ this->Invalidate(); }, EventWhen::AFTER);
	m_changeMenuId = AddWidget(m_changeMenu);
	// * Keybindings
	AddKeyboardInput(Settings::Keys::Change::directory, [&]
	{
		ActionPrompt([&](const String& input)
		{
			CD(Util::StringConvert<char>(input));
		}, Settings::Style::Change::change_dir_prompt_prefix, Settings::Style::Change::change_dir_prompt_bg, Settings::Style::Change::change_dir_prompt_max);
	});

	AddKeyboardInput(Settings::Keys::Change::name, [&]
	{
		if (m_dir->GetEntries() == 0) [[unlikely]]
			return;

		const String cur = GetCurrentFileName();
		if (cur.empty())  [[unlikely]]
			return;

		ActionPrompt([&, cur](const String& input)
		{
			if (input.empty()) [[unlikely]]
				return;
			try
			{
				m_dir->GetDir()->Rename(cur, input);
			}
			catch (IndexError& e)
			{
				Error(e.GetMessage());
			}
		}, Settings::Style::Change::change_name_prompt_prefix, Settings::Style::Change::change_name_prompt_bg, Settings::Style::Change::change_name_prompt_max, cur, cur.size());
	});

	AddKeyboardInput(Settings::Keys::Change::name_empty, [&]
	{
		if (m_dir->GetEntries() == 0) [[unlikely]]
			return;

		const String cur = GetCurrentFileName();
		if (cur.empty())  [[unlikely]]
			return;

		ActionPrompt([&, cur](const String& input)
		{
			if (input.empty()) [[unlikely]]
				return;
			try
			{
				m_dir->GetDir()->Rename(cur, input);
			}
			catch (IndexError& e)
			{
				Error(e.GetMessage());
			}
		}, Settings::Style::Change::change_name_prompt_prefix, Settings::Style::Change::change_name_prompt_bg, Settings::Style::Change::change_name_prompt_max);
	});
	// }}}

	m_sortMenu = new SortMenu(this);
	m_sortMenuId = AddWidget(m_sortMenu);

	m_shellMenu = new ShellMenu(this);
	m_shellMenuId = AddWidget(m_shellMenu);

	// {{{ Cache
	// * CacheExplorer
	m_cacheExplorer = new CacheExplorer(this);
	m_cacheExplorer->SetVisible(false);
	m_cacheExplorer->SetActive(false);
	m_cacheExplorerId = AddWidget(m_cacheExplorer);

	dInput->AddKeyboardInput(Settings::Keys::Cache::cache, [this](){ SetMode(CurrentMode::CACHE_EXPLORER); });
	m_cacheExplorer->AddKeyboardInput(Settings::Keys::Cache::cache, [this](){ SetMode(CurrentMode::NORMAL); });
	// }}}

	// {{{ Positions
	// * PositionsExplorer
	m_positionExplorer = new PositionExplorer(this);
	m_positionExplorer->SetVisible(false);
	m_positionExplorer->SetActive(false);
	m_positionExplorerId = AddWidget(m_positionExplorer);

	dInput->AddKeyboardInput(Settings::Keys::Position::position, [this](){ SetMode(CurrentMode::POSITION_EXPLORER); });
	m_positionExplorer->AddKeyboardInput(Settings::Keys::Position::position, [this](){ SetMode(CurrentMode::NORMAL); });
	// }}}

	m_dir->UpdateFromDir();
	m_parent->UpdateFromDir();
	try
	{
		ChangeDir(GetCurrentPath());
	}
	catch (IndexError& e)
	{
		Error(e.GetMessage());
	}

	m_marks->SetMarks(m_dir->GetDir());
	Resize(Termbox::GetDim());
	// Try to find position of directory in the parent List
	const auto pos = m_parent->Find(
		Util::StringConvert<Char>(m_dir->GetDir()->GetFolderName()),
		Mode::DIR);
	if (pos != static_cast<std::size_t>(-1))
		m_parent->ActionSetPosition(pos);
	else
		m_parent->ActionSetPosition(0);
}

MainWindow::~MainWindow()
{
}

void MainWindow::Resize(Vec2i dim)
{
	SetSize(dim-Vec2i(1, 1));
	const auto [w, h] = GetInteriorSpace(GetPosition(), GetSize()).second;
	Invalidate();

	m_tabline->SetPosition(Vec2i(0, 0));
	m_tabline->SetSize(Vec2i(w, 1));

	m_statusline->SetPosition(Vec2i(0, h-1));
	m_statusline->SetSize(Vec2i(w, 1));

	m_goMenu->SetPosition(Vec2i(0, h-1-m_goMenu->GetHeight()));
	m_goMenu->SetSize(Vec2i(w, m_goMenu->GetHeight()));

	m_showMenu->SetPosition(Vec2i(0, h-1-m_showMenu->GetHeight()));
	m_showMenu->SetSize(Vec2i(w, m_showMenu->GetHeight()));

	m_changeMenu->SetPosition(Vec2i(0, h-1-m_changeMenu->GetHeight()));
	m_changeMenu->SetSize(Vec2i(w, m_changeMenu->GetHeight()));

	m_sortMenu->Resize(Vec2i(w, h));
	m_shellMenu->Resize(Vec2i(w, h));

	// MainWindow layout
	const int available_width = w-[&]<std::size_t... i>(std::index_sequence<i...>)
	{
		return ((Settings::Layout::mainwindow_spacing[i])+ ...);
	}
	(std::make_index_sequence<Settings::Layout::mainwindow_spacing.size()>{});

	int x = 0;
	if (m_parentEnabled)
	{
		// Parent dir
		m_parent->SetSize(Vec2i(Settings::Layout::mainwindow_ratio[0]*available_width / 100, h-2));
		m_parent->SetPosition(Vec2i(x, 1));
		x += m_parent->GetSize()[0];
	}

	// Current dir
	if (m_parentEnabled)
		m_dir->SetSize(Vec2i(Settings::Layout::mainwindow_ratio[1]*available_width / 100, h-2));
	else
		m_dir->SetSize(Vec2i((Settings::Layout::mainwindow_ratio[0]+Settings::Layout::mainwindow_ratio[1])*available_width / 100, h-2));
	m_dir->SetPosition(Vec2i(x + Settings::Layout::mainwindow_spacing[0], 1));

	// Marks
	m_marksMenu->SetPosition(Vec2i(0, h-1-m_marksMenu->GetHeight()));
	m_marksMenu->SetSize(Vec2i(w, m_marksMenu->GetHeight()));

	m_marks->SetPosition(Vec2i(0, 1));
	m_marks->SetSize(Vec2i(w, h-2));

	// CacheExplorer
	m_cacheExplorer->SetPosition(Vec2i(0, 1));
	m_cacheExplorer->SetSize(Vec2i(w, h-2));

	// PositionExplorer
	m_positionExplorer->SetPosition(Vec2i(0, 1));
	m_positionExplorer->SetSize(Vec2i(w, h-2));

	// Prompt
	m_prompt->SetPosition(Vec2i(0, h-2));
	m_prompt->SetSize(Vec2i(w, 1));
}

std::size_t MainWindow::GetFilePosition(const String& name)
{
	const auto pos = m_dir->FindByName(name);
	if (pos == static_cast<std::size_t>(-1))
		return 0;

	return pos;
}

bool MainWindow::ShouldShowParent() const
{
	return m_settings.showParent && m_dir->GetDir()->GetPath() != "/";
}

void MainWindow::OnChangeDir()
{
	// Reload the marks
	m_marks->SetMarks(m_dir->GetDir());

	// If we're in '/', do not display the parent list
	if (ShouldShowParent())
	{
		m_parent->SetVisible(true);
		SetWidgetExpired(m_parentId, true);
	}
	else
	{
		m_parent->SetVisible(false);
		Invalidate(); // We ha	ve some empty space that needs to be erased...
	}
}

void MainWindow::Forward(const String& folder)
{
	// Marks & Position
	m_marks->AddMarks(m_dir->GetDir());
	if (m_dir->GetDir()->GetPath() != "/")
		m_positionCache.AddPosition(m_parent);

	try
	{
		//TODO: Update
		const auto newDir = gDirectoryCache.GetDirectory(m_dir->GetDir()->GetPath() + "/" + Util::StringConvert<char>(folder)).first;
		ChangeDir(newDir->GetPath());

		// Parent := Dir
		gDirectoryCache.DeleteDirectory(m_parent->GetDir());
		m_parent->MoveFiles(std::move(*m_dir), true);
		m_parent->UpdateFromDir(); // We want to re-filter it

		// Dir := ...
		m_dir->SetDir(newDir);
		m_dir->UpdateFromDir();
		m_dir->ActionSetPosition(m_positionCache.GetPosition(m_dir));
	}
	catch (IndexError& e)
	{
		Error(e.GetMessage());
		return;
	}

	OnChangeDir();
}

void MainWindow::Back()
{
	// Marks
	m_marks->AddMarks(m_dir->GetDir());
	m_positionCache.AddPosition(m_dir);

	try
	{
		//TODO: update
		const auto newParent = gDirectoryCache.GetDirectory(m_parent->GetDir()->GetPath() + "/..").first;
		ChangeDir(m_parent->GetDir()->GetPath());

		// Dir := Parent
		gDirectoryCache.DeleteDirectory(m_dir->GetDir());
		m_dir->MoveFiles(std::move(*m_parent), true);
		m_dir->UpdateFromDir(); // We want to re-filter it

		// Parent := ...
		m_parent->SetDir(newParent);
		m_parent->UpdateFromDir();
	}
	catch (IndexError& e)
	{
		Error(e.GetMessage());
		return;
	}

	// Find the previous dir
	const auto pos = m_parent->Find(
		Util::StringConvert<Char>(m_dir->GetDir()->GetFolderName()),
		Mode::DIR);
	if (pos != static_cast<std::size_t>(-1))
		m_parent->ActionSetPosition(pos);
	else
		m_parent->ActionSetPosition(0);

	OnChangeDir();
}

const List* MainWindow::GetList() const
{
	return m_dir;
}

const List* MainWindow::GetParentList() const
{
	return m_parent;
}

void MainWindow::Message(const TBString& msg, std::chrono::duration<std::size_t> secs)
{
	m_statusline->SetMessage(msg, secs);
}

void MainWindow::Error(const String& msg, std::chrono::duration<std::size_t> secs)
{
	m_statusline->SetError(msg, secs);
}

void MainWindow::CD(const std::string& path)
{
	// Marks
	m_marks->AddMarks(m_dir->GetDir());
	m_positionCache.AddPosition(m_parent);
	m_positionCache.AddPosition(m_dir);


	// If CD fails, we stay where we are
	try
	{
		// TODO: Update
		const auto newDir = gDirectoryCache.GetDirectory(path).first;
		const auto newParent = gDirectoryCache.GetDirectory(newDir->GetPath() + "/..").first;
		ChangeDir(newDir->GetPath());

		gDirectoryCache.DeleteDirectory(m_dir->GetDir());
		gDirectoryCache.DeleteDirectory(m_parent->GetDir());

		m_dir->SetDir(newDir);
		m_parent->SetDir(newParent);
	}
	catch (IndexError& e)
	{
		Error(e.GetMessage());
		return;
	}

	m_dir->UpdateFromDir(false);
	m_parent->UpdateFromDir(false);

	m_dir->ActionSetPosition(m_positionCache.GetPosition(m_dir));

	// Find the previous dir
	const auto pos = m_parent->Find(
		Util::StringConvert<Char>(m_dir->GetDir()->GetFolderName()),
		Mode::DIR);
	if (pos != static_cast<std::size_t>(-1))
		m_parent->ActionSetPosition(pos);
	else
		m_parent->ActionSetPosition(0);

	//TODO: position cache
	
	OnChangeDir();
}

void MainWindow::SetMode(CurrentMode mode)
{
	if (mode == m_currentMode)
		return;

	m_dir->SetVisible(false);
	m_dir->SetActive(false);
	m_parent->SetActive(false);
	m_marks->SetVisible(false);
	m_marks->SetActive(false);
	m_cacheExplorer->SetVisible(false);
	m_cacheExplorer->SetActive(false);
	m_positionExplorer->SetVisible(false);
	m_positionExplorer->SetActive(false);

	// Normal
	switch (mode)
	{
		case CurrentMode::NORMAL:
			m_dir->SetVisible(true);
			m_dir->SetActive(true);
			m_parent->SetVisible(ShouldShowParent());
			break;
		case CurrentMode::MARKS:
			m_marks->SetVisible(true);
			m_marks->SetActive(true);
			m_marks->AddMarks(m_dir->GetDir());
			break;
		case CurrentMode::CACHE_EXPLORER:
			m_cacheExplorer->SetVisible(true);
			m_cacheExplorer->SetActive(true);
			break;
		case CurrentMode::POSITION_EXPLORER:
			m_positionExplorer->SetVisible(true);
			m_positionExplorer->SetActive(true);
			break;
	}
	m_currentMode = mode;

	// Do not process current input any further
	Termbox::GetContext().stopInput = true;
	Invalidate();
}

MainWindow::CurrentMode MainWindow::GetMode() const
{
	return m_currentMode;
}

std::size_t MainWindow::GetTab() const
{
	return m_tab;
}

const Sort::Settings& MainWindow::GetSortSettings() const
{
	return m_dir->GetSettings().SortSettings;
}

void MainWindow::SetSortSettings(const Sort::Settings& settings)
{
	auto dirSettings = m_dir->GetSettings();
	auto parentSettings = m_parent->GetSettings();

	dirSettings.SortSettings = settings;
	parentSettings.SortSettings = settings;

	m_dir->SetSettings(dirSettings);
	m_parent->SetSettings(parentSettings);

	m_dir->Sort(true);
	m_parent->Sort(true);
}


const std::string& MainWindow::GetCurrentPath() const
{
	return m_dir->GetDir()->GetPath();
}


void MainWindow::ActionPrompt(std::function<void(const String&)> callback, const TBString& prefix, const TBChar& bg, std::size_t max, const String& input, std::size_t position)
{
	m_promptStateList = SetAllInactive(); // Stores all the widgets after disabling them
										  // The prompt will automatically restore them after exiting
	m_prompt->SetPrefix(prefix);
	m_prompt->SetBackground(bg);
	m_prompt->SetText(input);
	m_prompt->SetPos(position);
	m_prompt->SetMax(max);
	m_prompt->ActionShow();
	Termbox::GetContext().noRepeat = true;
	Termbox::GetContext().stopInput = true;

	m_prompt->OnStopShowing.AddEvent([this, callback](bool v){
		if (!v)
			return;
		callback(m_prompt->GetText());
	}, EventWhen::AFTER_ONCE);
}

String MainWindow::GetCurrentFileName() const
{
	return m_dir->GetCurrentFileName();
}
