#include "MainWindow.hpp"
#include "List.hpp"
#include "Tab.hpp"
#include "Tabline.hpp"
#include "Statusline.hpp"
#include "Menu.hpp"

MainWindow::MainWindow(const std::string& path):
	Window::Window({U"", Settings::Style::default_text_style}),
	m_tab(0),
	m_sortFn(0)
{
	Tab zero;
	Tabs.push_back(std::move(zero));

	SetSize(Termbox::GetDim()-Vec2i(1, 1));
	SetPosition(Vec2i(0, 0));

	SetBorderFlags(0x00);
	SetBackground(Settings::Style::main_window_background, false, false);

	AddKeyboardInput({Settings::Keyboard::quit, [](){ Termbox::GetTermbox().GetContext().stop = true; }});


	m_dir = new List(this, path, true);
	m_parent = new List(this, path + "/..", false);

	m_parentId = AddWidget(m_parent);
	m_dirId = AddWidget(m_dir);

	m_tabline = new Tabline(this);
	m_tablineId = AddWidget(m_tabline);
	m_statusline = new Statusline(this);
	m_statuslineId = AddWidget(m_statusline);

	//m_tabline->SetLoadingEnabled(true);
	//m_tabline->SetProcessTimed(true);

	m_dir->OnChangePosition.AddEvent([this](){ SetWidgetExpired(m_tablineId, true); }, EventWhen::AFTER);
	m_dir->OnChangePosition.AddEvent([this](){ SetWidgetExpired(m_statuslineId, true); }, EventWhen::AFTER);

	m_goMenu = new Menu(this);
	m_goMenu->SetTable(2,
			{
				{{U"Key", Settings::default_text_style}, 15},
				{{U"Action", Settings::default_text_style}, 85},
			},
			{
				{Settings::Keys::Go::top, Settings::default_text_style},
				{U"Go to the top", Settings::default_text_style},
				{Settings::Keys::Go::bottom, Settings::default_text_style},
				{U"Go to the bottom", Settings::default_text_style},
				{Settings::Keys::Go::home, Settings::default_text_style},
				{U"Go to ~", Settings::default_text_style},
				{Settings::Keys::Go::root, Settings::default_text_style},
				{U"Go to /", Settings::default_text_style},
			});
	m_goMenu->AddKeyboardInput({U"g", [this](){
			if (m_goMenu->IsVisible())
				return;

			m_goMenu->ActionShow();
	}});
	m_goMenu->OnStopShowing.AddEvent([this](){
			this->Invalidate();
	}, EventWhen::AFTER);
	m_goMenuId = AddWidget(m_goMenu);


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

	Tabs[0].SetMainWindow(this);
}

MainWindow::~MainWindow()
{
}

void MainWindow::Resize(Vec2i dim)
{
	SetSize(dim-Vec2i(1, 1));
	const auto [w, h] = GetInteriorSpace(GetPosition(), GetSize()).second;
	Invalidate();

	const Tab& tab = Tabs[m_tab];

	m_tabline->SetPosition(Vec2i(0, 0));
	m_tabline->SetSize(Vec2i(w, 1));

	m_statusline->SetPosition(Vec2i(0, h-1));
	m_statusline->SetSize(Vec2i(w, 1));

	m_goMenu->SetPosition(Vec2i(0, h-1-m_goMenu->GetHeight()));
	m_goMenu->SetSize(Vec2i(w, m_goMenu->GetHeight()));

	// Sum
	/* constexpr int sum_ratio = [&]<std::size_t... i>(std::index_sequence<i...>)
	{ereturn ((Settings::Layout::mainwindow_ratio[i]) + ...); }
	(std::make_index_sequence<sizeof(Settings::Layout::mainwindow_ratio)/sizeof(int)>{});
	constexpr int sum_spacing = [&]<std::size_t... i>(std::index_sequence<i...>)
	{ return ((Settings::Layout::mainwindow_spacing[i]) + ...); }
	(std::make_index_sequence<sizeof(Settings::Layout::mainwindow_spacing)/sizeof(int)>{});
	TODO... */

	int x = 0;
	if (tab.GetParentEnabled())
	{
		// Parent dir
		m_parent->SetSize(Vec2i(Settings::Layout::mainwindow_ratio[0]*w / 100, h-2));
		m_parent->SetPosition(Vec2i(x, 1));
		x += m_parent->GetSize()[0];
	}

	// Current dir
	if (tab.GetParentEnabled())
		m_dir->SetSize(Vec2i(Settings::Layout::mainwindow_ratio[1]*w / 100, h-2));
	else
		m_dir->SetSize(Vec2i((Settings::Layout::mainwindow_ratio[0]+Settings::Layout::mainwindow_ratio[1])*w / 100, h-2));
	m_dir->SetPosition(Vec2i(x + Settings::Layout::mainwindow_spacing[0], 1));
}

void MainWindow::UpdateFiles()
{
	m_dir->UpdateFiles();
	m_parent->UpdateFiles();
}

void MainWindow::OnChangeDir()
{
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
