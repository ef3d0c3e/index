#include "MainWindow.hpp"
#include "List.hpp"
#include "Tab.hpp"
#include "Tabline.hpp"
#include "Statusline.hpp"
#include "Menu.hpp"
#include "Marks.hpp"
#include "Prompt.hpp"

MainWindow::MainWindow(const std::string& path, std::size_t tabId):
	Window::Window({U"", Settings::Style::default_text_style}),
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
	dInput->AddKeyboardInput({Settings::Keyboard::quit, [](){ Termbox::GetTermbox().GetContext().stop = true; }});

	// Tabs
	dInput->AddKeyboardInput({Settings::Keys::Go::tab_new, [this]()
	{
		SetVisible(false);
		SetActive(false);
		m_marks->AddMarks(m_dir->GetDir());

		Tab tab;
		Tabs.push_back(std::move(tab));
		MainWindow* main = new MainWindow(m_dir->GetDir()->GetPath(), Tabs.size()-1);
		main->m_marks->SetMarks(main->m_dir->GetDir());
		Tabs[Tabs.size()-1].SetMainWindow(Termbox::GetTermbox().Termbox::AddWidget(main));
	}});

	dInput->AddKeyboardInput({Settings::Keys::Go::tab_next, [&]()
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
	}});

	dInput->AddKeyboardInput({Settings::Keys::Go::tab_prev, [this]()
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
	}});

	dInput->AddKeyboardInput({Settings::Keys::Go::tab_close, [this]()
	{
		if (Tabs.size() == 1)
			return;

		SetVisible(false);
		SetActive(false);
		m_marks->AddMarks(m_dir->GetDir());

		MainWindow* win = Tabs[(m_tab-1) % Tabs.size()].GetMainWindow();

		Termbox::GetTermbox().Termbox::RemoveWidget(Tabs[m_tab].GetMainWindowId());
		delete Tabs[m_tab].GetMainWindow();
		Tabs.erase(Tabs.begin()+m_tab);

		win->SetVisible(true);
		win->SetActive(true);
		win->Invalidate();
		Termbox::GetContext().clear = true;
	}});

	// Directories
	m_dir = new List(this, path, true);
	m_parent = new List(this, path + "/..", false);
	if (m_dir->GetDir()->GetPath() == "/")
		m_parent->SetVisible(false);

	m_parentId = AddWidget(m_parent);
	m_dirId = AddWidget(m_dir);

	m_dir->OnChangePosition.AddEvent([this](){ SetWidgetExpired(m_tablineId, true); }, EventWhen::AFTER);
	m_dir->OnChangePosition.AddEvent([this](){ SetWidgetExpired(m_statuslineId, true); }, EventWhen::AFTER);

	// Tabline & statusline
	m_tabline = new Tabline(this);
	m_tablineId = AddWidget(m_tabline);
	m_statusline = new Statusline(this);
	m_statuslineId = AddWidget(m_statusline);

	//m_tabline->SetLoadingEnabled(true);
	//m_tabline->SetProcessTimed(true);

	// {{{ Menus
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
	m_goMenu->AddKeyboardInput({Settings::Keys::Go::menu, [this](){
			Termbox::GetContext().dontResetRepeat = true;
			m_goMenu->ActionShow();
	}});
	m_goMenu->OnStopShowing.AddEvent([this](){
			Invalidate();
	}, EventWhen::AFTER);
	m_goMenuId = AddWidget(m_goMenu);
	// }}}

	// {{{ Marks
	// Menu
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
	m_marksMenu->AddKeyboardInput({Settings::Keys::Marks::menu, [this]()
	{
		if (m_marksMenu->IsVisible())
			return;

		Termbox::GetContext().dontResetRepeat = true;
		m_marksMenu->ActionShow();
	}});
	m_marksMenu->OnStopShowing.AddEvent([this](){ this->Invalidate(); }, EventWhen::AFTER);
	m_marksMenuId = AddWidget(m_marksMenu);

	// Marks List
	m_marks = new Marks(this);
	m_marks->SetVisible(false);
	m_marks->SetActive(false);
	m_marks->OnChangePosition.AddEvent([this]()
	{
		// Updating the repeat
		SetWidgetExpired(m_tablineId, true);
	});
	m_marksMode = false;
	m_marksId = AddWidget(m_marks);
	m_marks->SetMarks(m_dir->GetDir());

	// Keybindings
	dInput->AddKeyboardInput({Settings::Keys::Marks::marks, [this](){ ToggleMarks(); }});
	m_dir->AddKeyboardInput(KeyComb(Settings::Keys::Marks::select, [this]()
	{
		if (Termbox::GetContext().hasRepeat)
			m_dir->ActionMarkN(Termbox::GetContext().repeat, MarkType::SELECTED);
		else
			m_dir->ActionMarkN(1, MarkType::SELECTED);
	}));
	m_dir->AddKeyboardInput({Settings::Keys::Marks::select_toggle_all, [this]()
	{
		for (std::size_t i = 0; i < m_dir->GetDir()->Size(); ++i)
			m_dir->MarkFn(i, MarkType::SELECTED);
	}});
	m_dir->AddKeyboardInput({Settings::Keys::Marks::unselect_all, [this]()
	{
		for (std::size_t i = 0; i < m_dir->GetDir()->Size(); ++i)
			(*m_dir->GetDir())[i].mark &= ~(MarkType::SELECTED);
	}});
	m_dir->AddKeyboardInput({Settings::Keys::Marks::tag, [this]()
	{
		if (Termbox::GetContext().hasRepeat)
			m_dir->ActionMarkN(Termbox::GetContext().repeat, MarkType::TAGGED);
		else
			m_dir->ActionMarkN(1, MarkType::TAGGED);
	}});
	m_dir->AddKeyboardInput({Settings::Keys::Marks::fav, [this]()
	{
		if (Termbox::GetContext().hasRepeat)
			m_dir->ActionMarkN(Termbox::GetContext().repeat, MarkType::FAV);
		else
			m_dir->ActionMarkN(1, MarkType::FAV);
	}});
	m_marks->AddKeyboardInput({Settings::Keys::Marks::exit, [this]()
	{
		ToggleMarks();
	}});
	// }}}

	// Prompt
	m_prompt = new Prompt({U"Tab name: ", {0xFFFF00, 0x000000, TextStyle::Bold}}, U"test");
	m_promptId = AddWidget(m_prompt);
	m_stateList = SetAllInactive();
	m_prompt->ActionShow();
	Termbox::GetContext().noRepeat = true;
	m_prompt->OnStopShowing.AddEvent([this](bool) { RestoreAllActive(std::move(m_stateList)); Invalidate(); Termbox::GetContext().noRepeat = false; }, EventWhen::AFTER);
	

	UpdateFiles();
	Resize(Termbox::GetDim());
	// Try to find position of directory in the parent List
	const auto pos = m_parent->GetDir()->Find(
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

	// Prompt
	m_prompt->SetPosition(Vec2i(0, h-2));
	m_prompt->SetSize(Vec2i(w, 1));
}

void MainWindow::UpdateFiles()
{
	m_dir->UpdateFiles();
	m_parent->UpdateFiles();
}

void MainWindow::OnChangeDir()
{
	m_marks->SetMarks(m_dir->GetDir());

	if (m_dir->GetDir()->GetPath() == "/")
	{
		m_parent->SetVisible(false);
		Invalidate();
	}
	else
	{
		m_parent->SetVisible(true);
		SetWidgetExpired(m_parentId, true);
	}
}

void MainWindow::Forward(const String& folder)
{
	// Marks
	m_marks->AddMarks(m_dir->GetDir());

	delete m_parent->GetDir();
	m_parent->SetDir(m_dir->GetDir());
	m_parent->ActionSetPosition(m_dir->GetPos());

	m_dir->SetDir(new Directory(m_dir->GetDir()->GetPath() + "/" + Util::StringConvert<char>(folder)));
	m_dir->UpdateFiles();
	m_dir->ActionSetPosition(0);

	OnChangeDir();
}

void MainWindow::Back()
{
	// Marks
	m_marks->AddMarks(m_dir->GetDir());

	delete m_dir->GetDir();
	m_dir->SetDir(m_parent->GetDir());
	m_dir->ActionSetPosition(m_parent->GetPos());

	m_parent->SetDir(new Directory(m_parent->GetDir()->GetPath() + "/.."));
	m_parent->UpdateFiles();
	const String name = Util::StringConvert<Char>(m_dir->GetDir()->GetFolderName());
	
	const auto pos = m_parent->GetDir()->Find(
		Util::StringConvert<Char>(m_dir->GetDir()->GetFolderName()),
		Mode::DIR);
	if (pos != static_cast<std::size_t>(-1))
		m_parent->ActionSetPosition(pos);
	else
		m_parent->ActionSetPosition(0);

	OnChangeDir();
}

const Directory* MainWindow::GetDir() const
{
	return m_dir->GetDir();
}

const List* MainWindow::GetList() const
{
	return m_dir;
}

void MainWindow::Error(const String& msg)
{
	m_statusline->DrawError(msg);
}

void MainWindow::CD(const std::string& path)
{
	// Marks
	m_marks->AddMarks(m_dir->GetDir());

	delete m_dir->GetDir();
	delete m_parent->GetDir();

	m_dir->SetDir(new Directory(path));
	m_dir->UpdateFiles();

	m_parent->SetDir(new Directory(m_dir->GetDir()->GetPath() + "/.."));
	m_parent->UpdateFiles();

	
	m_parent->ActionSetPosition(0);
	m_dir->ActionSetPosition(0);

	OnChangeDir();
}

void MainWindow::ToggleMarks()
{
	m_marks->AddMarks(m_dir->GetDir());

	if (!m_marksMode)
	{
		m_marksMode = true;
		m_dir->SetVisible(false);
		m_parent->SetVisible(false);
		m_dir->SetActive(false);

		m_marks->SetVisible(true);
		m_marks->SetActive(true);
	}
	else
	{
		m_marksMode = false;
		m_dir->SetVisible(true);
		m_parent->SetVisible(m_dir->GetDir()->GetPath() != "/");
		m_dir->SetActive(true);

		m_marks->SetVisible(false);
		m_marks->SetActive(false);
	}

	Termbox::GetContext().stopInput = true;
	Invalidate();
}

std::size_t MainWindow::GetTab() const
{
	return m_tab;
}