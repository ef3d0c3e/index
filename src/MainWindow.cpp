#include "MainWindow.hpp"
#include "Tab.hpp"
#include "DirectoryCache.hpp"

// Widgets
#include "Tabline.hpp"
#include "Statusline.hpp"

// List
#include "UI/List.hpp"

// Prompt
#include "Prompt.hpp"

// Menus
#include "UI/GoMenu.hpp"
#include "UI/SortMenu.hpp"
#include "UI/ShellMenu.hpp"
#include "UI/MarksMenu.hpp"
#include "UI/ShowMenu.hpp"
#include "UI/ChangeMenu.hpp"

// Explorers
#include "UI/MarksExplorer.hpp"
#include "UI/CacheExplorer.hpp"
#include "UI/PositionExplorer.hpp"
#include "UI/JobManager.hpp"

#define LOG(__msg)                                  \
{                                                   \
	static bool first = true;                       \
	std::ofstream logs;                             \
	if (first)                                      \
	{                                               \
		first = false;                              \
		logs.open("/tmp/index_log", std::ios::ate); \
	}                                               \
	else                                            \
		logs.open("/tmp/index_log", std::ios::app); \
	logs << __msg;                                  \
	logs.close();                                   \
}

MainWindow::MainWindow(const std::string& path, std::size_t tabId):
	Window::Window({U"", Settings::Style::default_text_style}),
	m_positionCache(),
	m_currentMode(CurrentMode::NORMAL),
	m_tab(tabId),
	m_sortFn(0),
	// Settings
	m_parentEnabled(true)
{
	SetBorderFlags(0x00);
	SetBackground(Settings::Style::main_window_background, false, false);

	// * General input
	AddKeyboardInput(Settings::Keyboard::quit, [this]
	{
		if (m_prompt->IsActive())
			return;
		//TODO: Prompt
		Termbox::GetTermbox().GetContext().stop = true;
	});

	// * Tabs
	AddKeyboardInput(Settings::Keys::Go::tab_new, [this]
	{
		if (m_prompt->IsActive())
			return;
		Tab tab;
		gTabs.push_back(std::move(tab));
		MainWindow* main = new MainWindow(m_dir->GetDir()->GetPath(), gTabs.size()-1);
		gTabs.back().SetMainWindow(Termbox::GetTermbox().Termbox::AddWidget(main));

		Switch(main);
	});

	AddKeyboardInput(Settings::Keys::Go::tab_next, [this]
	{
		if (m_prompt->IsActive())
			return;
		if (gTabs.size() == 1)
			return;

		MainWindow* win;
		if (Termbox::GetContext().hasRepeat)
			win = gTabs[(m_tab+Termbox::GetContext().repeat) %  gTabs.size()].GetMainWindow();
		else
			win = gTabs[(m_tab+1) % gTabs.size()].GetMainWindow();

		Switch(win);
	});

	AddKeyboardInput(Settings::Keys::Go::tab_prev, [this]
	{
		if (m_prompt->IsActive())
			return;
		//FIXME: g T at tab 0
		if (gTabs.size() == 1)
			return;

		MainWindow* win;
		if (Termbox::GetContext().hasRepeat)
			win = gTabs[(m_tab-Termbox::GetContext().repeat) % gTabs.size()].GetMainWindow();
		else
			win = gTabs[(m_tab-1) % gTabs.size()].GetMainWindow();

		Switch(win);
	});

	AddKeyboardInput(Settings::Keys::Go::tab_close, [this]()
	{
		if (m_prompt->IsActive())
			return;
		if (gTabs.size() == 1)
			return;

		SetVisible(false);
		SetActive(false);
		m_marksExplorer->AddMarks(m_dir->GetDir());

		MainWindow* win = gTabs[(m_tab == gTabs.size()-1) ? m_tab-1 : ((m_tab+1) % gTabs.size())].GetMainWindow();

		Switch(win);
		gTabs[m_tab].Delete();

		gDirectoryCache.DeleteDirectory(m_dir->GetDir());
		gDirectoryCache.DeleteDirectory(m_parent->GetDir());
	});

	// * Tabline & statusline
	m_tabline = new Tabline(this);
	m_tablineId = AddWidget(m_tabline);
	m_statusline = new Statusline(this);
	m_statuslineId = AddWidget(m_statusline);

	// * Lists
	// TODO: Error handling outside the ctor
	m_dir = new List(this, path, true);
	// Update the Statusline & Tabline
	m_dir->OnChangePosition.AddEvent([this]
	{
		SetWidgetExpired(m_tablineId, true);
	}, EventWhen::AFTER);
	m_dir->OnChangePosition.AddEvent([this]
	{
		SetWidgetExpired(m_statuslineId, true);
	}, EventWhen::AFTER);
	m_dir->UpdateFromDir();
	m_dirId = AddWidget(m_dir);

	m_parent = new List(this, path + "/..", false);
	m_parent->SetVisible(ShouldShowParent());
	m_parent->UpdateFromDir();
	m_parentId = AddWidget(m_parent);


	// * Prompt
	m_prompt = new Prompt({U"", Settings::Style::default_text_style}, U"");
	m_prompt->OnStopShowing.AddEvent([this](bool)
	{
		RestoreAllActive(std::move(m_promptStateList));
		Invalidate();
		Termbox::GetContext().noRepeat = false;
	}, EventWhen::AFTER);
	m_promptId = AddWidget(m_prompt);

	// {{{ Menus
	// * Go
	m_goMenu = new GoMenu(this);
	m_goMenuId = AddWidget(m_goMenu);
	// * Sort
	m_sortMenu = new SortMenu(this);
	m_sortMenuId = AddWidget(m_sortMenu);
	// * Shell
	m_shellMenu = new ShellMenu(this);
	m_shellMenuId = AddWidget(m_shellMenu);
	// * Marks
	m_marksMenu = new MarksMenu(this);
	m_marksMenuId = AddWidget(m_marksMenu);
	// * Show
	m_showMenu = new ShowMenu(this);
	m_showMenuId = AddWidget(m_showMenu);
	// * Change
	m_changeMenu = new ChangeMenu(this);
	m_changeMenuId = AddWidget(m_changeMenu);
	// }}}

	// {{{ Explorers
	// * Marks List
	m_marksExplorer = new MarksExplorer(this);
	m_marksExplorer->SetVisible(false);
	m_marksExplorer->SetActive(false);
	m_marksExplorerId = AddWidget(m_marksExplorer);
	AddKeyboardInput(Settings::Keys::Marks::marks, [this]
	{
		if (m_prompt->IsActive())
			return;
		SetMode(m_currentMode == CurrentMode::MARKS ? CurrentMode::NORMAL : CurrentMode::MARKS);
	});

	// * Cache Explorer
	m_cacheExplorer = new CacheExplorer(this);
	m_cacheExplorer->SetVisible(false);
	m_cacheExplorer->SetActive(false);
	m_cacheExplorerId = AddWidget(m_cacheExplorer);
	AddKeyboardInput(Settings::Keys::Cache::cache, [this]
	{
		if (m_prompt->IsActive())
			return;
		SetMode(m_currentMode == CurrentMode::CACHE_EXPLORER ? CurrentMode::NORMAL : CurrentMode::CACHE_EXPLORER);
	});

	// * Position Explorer
	m_positionExplorer = new PositionExplorer(this);
	m_positionExplorer->SetVisible(false);
	m_positionExplorer->SetActive(false);
	m_positionExplorerId = AddWidget(m_positionExplorer);

	AddKeyboardInput(Settings::Keys::Position::position, [this]
	{
		if (m_prompt->IsActive())
			return;
		SetMode(CurrentMode::POSITION_EXPLORER);
	});

	// * Job Manager
	m_jobManager = new JobManager(this);
	m_jobManager->SetVisible(false);
	m_jobManager->SetActive(false);
	m_jobManagerId = AddWidget(m_jobManager);
	AddKeyboardInput(Settings::Keys::JobManager::manager, [this]
	{
		if (m_prompt->IsActive())
			return;
		SetMode(m_currentMode == CurrentMode::JOB_MANAGER ? CurrentMode::NORMAL : CurrentMode::JOB_MANAGER);
	});

	// TODO: Do error handling before the ctor is called
	// if executing index -> exit (like ranger)
	// if creating tab -> cancel tab creation
	try
	{
		ChangeDir(GetCurrentPath());
	}
	catch (IndexError& e)
	{
		Error(e.GetMessage());
	}

	m_marksExplorer->SetMarks(m_dir->GetDir());
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

	// * Menus
	m_goMenu->Resize(Vec2i(w, h));
	m_sortMenu->Resize(Vec2i(w, h));
	m_shellMenu->Resize(Vec2i(w, h));
	m_marksMenu->Resize(Vec2i(w, h));

	m_showMenu->SetPosition(Vec2i(0, h-1-m_showMenu->GetHeight()));
	m_showMenu->SetSize(Vec2i(w, m_showMenu->GetHeight()));

	m_changeMenu->SetPosition(Vec2i(0, h-1-m_changeMenu->GetHeight()));
	m_changeMenu->SetSize(Vec2i(w, m_changeMenu->GetHeight()));

	// * Explorers
	m_marksExplorer->SetPosition(Vec2i(0, 1));
	m_marksExplorer->SetSize(Vec2i(w, h-2));
	m_cacheExplorer->SetPosition(Vec2i(0, 1));
	m_cacheExplorer->SetSize(Vec2i(w, h-2));
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
	m_marksExplorer->SetMarks(m_dir->GetDir());

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
	m_marksExplorer->AddMarks(m_dir->GetDir());
	if (m_dir->GetDir()->GetPath() != "/")
		m_positionCache.AddPosition(m_parent);

	try
	{
		const auto [newDir, shouldUpdate] = gDirectoryCache.GetDirectory(m_dir->GetDir()->GetPath() + ((GetCurrentPath() != "/") ? "/" : "") + Util::StringConvert<char>(folder));
		// Theoretically, changedir should throw first...
		ChangeDir(newDir->GetPath());
		if (shouldUpdate)
		{
			auto [failed, _errno] = newDir->GetFiles();
			if (failed)
			{
				// Message
				switch (_errno)
				{
					case EACCES:
						throw IndexError(U"Error: No permission", IndexError::GENERIC_ERROR);
						break;
					case EFAULT:
						throw IndexError(U"Error: Path points outside your accessible address space", IndexError::GENERIC_ERROR);
						break;
					case EIO:
						throw IndexError(U"Error: An I/O error occurred.", IndexError::GENERIC_ERROR);
						break;
					case ELOOP:
						throw IndexError(U"Error: Too many symbolic links were encountered in resolving path.", IndexError::GENERIC_ERROR);
						break;
					case ENAMETOOLONG:
						throw IndexError(U"Error: Path is too long.", IndexError::GENERIC_ERROR);
						break;
					case ENOENT:
						throw IndexError(U"Error: The directory specified in path does not exist.", IndexError::GENERIC_ERROR);
						break;
					case ENOMEM:
						throw IndexError(U"Error: Insufficient kernel memory was available.", IndexError::GENERIC_ERROR);
						break;
					case ENOTDIR:
						throw IndexError(U"Error: A component of path is not a directory.", IndexError::GENERIC_ERROR);
						break;
					case EMFILE:
						throw IndexError(U"Error: The per-process limit on the number of open file de‐scriptors has been reached.", IndexError::GENERIC_ERROR);
						break;
					case ENFILE:
						throw IndexError(U"Error: The  system-wide  limit  on the total number of open files has been reached.", IndexError::GENERIC_ERROR);
						break;
					default:
						throw IndexError(U"Error: Cannot access directory", IndexError::GENERIC_ERROR);
						break;
				}
			}
		}

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
	m_marksExplorer->AddMarks(m_dir->GetDir());
	m_positionCache.AddPosition(m_dir);

	try
	{
		//TODO: Update
		const auto [newParent, shouldUpdate] = gDirectoryCache.GetDirectory(m_parent->GetDir()->GetPath() + "/..");
		ChangeDir(m_parent->GetDir()->GetPath());
		if (shouldUpdate)
		{
			auto [failed, _errno] = newParent->GetFiles();
			if (failed)
			{
				// Message
				switch (_errno)
				{
					case EACCES:
						throw IndexError(U"Error: No permission", IndexError::GENERIC_ERROR);
						break;
					case EFAULT:
						throw IndexError(U"Error: Path points outside your accessible address space", IndexError::GENERIC_ERROR);
						break;
					case EIO:
						throw IndexError(U"Error: An I/O error occurred.", IndexError::GENERIC_ERROR);
						break;
					case ELOOP:
						throw IndexError(U"Error: Too many symbolic links were encountered in resolving path.", IndexError::GENERIC_ERROR);
						break;
					case ENAMETOOLONG:
						throw IndexError(U"Error: Path is too long.", IndexError::GENERIC_ERROR);
						break;
					case ENOENT:
						throw IndexError(U"Error: The directory specified in path does not exist.", IndexError::GENERIC_ERROR);
						break;
					case ENOMEM:
						throw IndexError(U"Error: Insufficient kernel memory was available.", IndexError::GENERIC_ERROR);
						break;
					case ENOTDIR:
						throw IndexError(U"Error: A component of path is not a directory.", IndexError::GENERIC_ERROR);
						break;
					case EMFILE:
						throw IndexError(U"Error: The per-process limit on the number of open file de‐scriptors has been reached.", IndexError::GENERIC_ERROR);
						break;
					case ENFILE:
						throw IndexError(U"Error: The  system-wide  limit  on the total number of open files has been reached.", IndexError::GENERIC_ERROR);
						break;
					default:
						throw IndexError(U"Error: Cannot access directory", IndexError::GENERIC_ERROR);
						break;
				}
			}
		}

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

List* MainWindow::GetList()
{
	return m_dir;
}

const List* MainWindow::GetParentList() const
{
	return m_parent;
}

List* MainWindow::GetParentList()
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
	m_marksExplorer->AddMarks(m_dir->GetDir());
	m_positionCache.AddPosition(m_parent);
	m_positionCache.AddPosition(m_dir);


	// If CD fails, we stay where we are
	try
	{
		// TODO: Update
		const auto [newDir, updateDir] = gDirectoryCache.GetDirectory(path);
		const auto [newParent, updateParent] = gDirectoryCache.GetDirectory(newDir->GetPath() + "/..");
		ChangeDir(newDir->GetPath());
		if (updateDir)
		{
			auto [failed, _errno] = newDir->GetFiles();
			if (failed)
			{
				// Message
				switch (_errno)
				{
					case EACCES:
						throw IndexError(U"Error: No permission", IndexError::GENERIC_ERROR);
						break;
					case EFAULT:
						throw IndexError(U"Error: Path points outside your accessible address space", IndexError::GENERIC_ERROR);
						break;
					case EIO:
						throw IndexError(U"Error: An I/O error occurred.", IndexError::GENERIC_ERROR);
						break;
					case ELOOP:
						throw IndexError(U"Error: Too many symbolic links were encountered in resolving path.", IndexError::GENERIC_ERROR);
						break;
					case ENAMETOOLONG:
						throw IndexError(U"Error: Path is too long.", IndexError::GENERIC_ERROR);
						break;
					case ENOENT:
						throw IndexError(U"Error: The directory specified in path does not exist.", IndexError::GENERIC_ERROR);
						break;
					case ENOMEM:
						throw IndexError(U"Error: Insufficient kernel memory was available.", IndexError::GENERIC_ERROR);
						break;
					case ENOTDIR:
						throw IndexError(U"Error: A component of path is not a directory.", IndexError::GENERIC_ERROR);
						break;
					case EMFILE:
						throw IndexError(U"Error: The per-process limit on the number of open file de‐scriptors has been reached.", IndexError::GENERIC_ERROR);
						break;
					case ENFILE:
						throw IndexError(U"Error: The  system-wide  limit  on the total number of open files has been reached.", IndexError::GENERIC_ERROR);
						break;
					default:
						throw IndexError(U"Error: Cannot access directory", IndexError::GENERIC_ERROR);
						break;
				}
			}
		}
		if (updateParent)
		{
			auto [failed, _errno] = newParent->GetFiles();
			if (failed)
			{
				// Message
				switch (_errno)
				{
					case EACCES:
						throw IndexError(U"Error: No permission", IndexError::GENERIC_ERROR);
						break;
					case EFAULT:
						throw IndexError(U"Error: Path points outside your accessible address space", IndexError::GENERIC_ERROR);
						break;
					case EIO:
						throw IndexError(U"Error: An I/O error occurred.", IndexError::GENERIC_ERROR);
						break;
					case ELOOP:
						throw IndexError(U"Error: Too many symbolic links were encountered in resolving path.", IndexError::GENERIC_ERROR);
						break;
					case ENAMETOOLONG:
						throw IndexError(U"Error: Path is too long.", IndexError::GENERIC_ERROR);
						break;
					case ENOENT:
						throw IndexError(U"Error: The directory specified in path does not exist.", IndexError::GENERIC_ERROR);
						break;
					case ENOMEM:
						throw IndexError(U"Error: Insufficient kernel memory was available.", IndexError::GENERIC_ERROR);
						break;
					case ENOTDIR:
						throw IndexError(U"Error: A component of path is not a directory.", IndexError::GENERIC_ERROR);
						break;
					case EMFILE:
						throw IndexError(U"Error: The per-process limit on the number of open file de‐scriptors has been reached.", IndexError::GENERIC_ERROR);
						break;
					case ENFILE:
						throw IndexError(U"Error: The  system-wide  limit  on the total number of open files has been reached.", IndexError::GENERIC_ERROR);
						break;
					default:
						throw IndexError(U"Error: Cannot access directory", IndexError::GENERIC_ERROR);
						break;
				}
			}
		}

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

void MainWindow::SetEnabledInternalWidgets(bool v)
{
	// Lists
	m_dir->SetVisible(v);
	m_dir->SetActive(v);
	m_parent->SetVisible(v); // Never active

	// Menus (only need to be disabled)
	m_goMenu->SetActive(v);
	m_sortMenu->SetActive(v);
	m_shellMenu->SetActive(v);
	m_marksMenu->SetActive(v);
	m_showMenu->SetActive(v);
	m_changeMenu->SetActive(v);
}

void MainWindow::SetMode(CurrentMode mode)
{
	// When we are in any mode, we want the basic Keybindings
	// (the ones in MainWindow) to still be active. Because they
	// are the ones for quitting and switching tab, that's why
	// we deactivate every widgets we don't want...
	if (mode == m_currentMode)
		return;

	SetEnabledInternalWidgets(false);
	m_jobManager->SetVisible(false);
	m_jobManager->SetActive(false);
	m_marksExplorer->SetVisible(false);
	m_marksExplorer->SetActive(false);
	m_cacheExplorer->SetVisible(false);
	m_cacheExplorer->SetActive(false);
	m_positionExplorer->SetVisible(false);
	m_positionExplorer->SetActive(false);

	// Normal
	switch (mode)
	{
		case CurrentMode::NORMAL:
			SetEnabledInternalWidgets(true);
			break;
		case CurrentMode::JOB_MANAGER:
			m_jobManager->SetVisible(true);
			m_jobManager->SetActive(true);
		case CurrentMode::MARKS:
			m_marksExplorer->SetVisible(true);
			m_marksExplorer->SetActive(true);
			m_marksExplorer->AddMarks(m_dir->GetDir());
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

void MainWindow::Switch(MainWindow* win)
{
	// Disable current MainWindow
	SetVisible(false);
	SetActive(false);
	m_marksExplorer->AddMarks(m_dir->GetDir());

	win->SetVisible(true);
	win->SetActive(true);
	if (gTabs[win->GetTabID()].ShouldUpdate())
	{
		// We update both, even though in some cases only updating one would be enough
		win->m_dir->UpdateFromDir();
		win->m_parent->UpdateFromDir();
		gTabs[win->GetTabID()].SetShouldUpdate(false);
	}

	win->Invalidate();
	win->m_marksExplorer->SetMarks(win->m_dir->GetDir());
	Termbox::GetContext().clear = true;
}
