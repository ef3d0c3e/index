#ifndef INDEX_MAINWINDOW_HPP
#define INDEX_MAINWINDOW_HPP

#include "TermboxWidgets/Window.hpp"
#include "Sort.hpp"
#include "PositionCache.hpp"
class List;
class Tabline;
class Statusline;
class Menu;
class Prompt;

class MarksExplorer;
class CacheExplorer;
class PositionExplorer;
class JobManager;

class GoMenu;
class SortMenu;
class ShellMenu;
class MarksMenu;
class ShowMenu;
class ChangeMenu;

class MainWindow : public Window
{
	friend class ShowMenu;
	friend class ChangeMenu;

	friend class JobManager;
	friend class MarksExplorer;
	friend class CacheExplorer;
	friend class PositionExplorer;

public:
	MAKE_CENUM_Q(CurrentMode, std::uint8_t,
		NORMAL, 0, // Default mode (directory list)
		MARKS, 1, // Marks mode
		JOB_MANAGER, 2, // Manage jobs
		CACHE_EXPLORER, 3, // Cache explorer mode
		POSITION_EXPLORER, 4, // Position cache explorer mode
	);

	struct MainSettings
	{
		bool showParent = true;
	};
private:
	MainSettings m_settings;
	PositionCache m_positionCache;

	// Statusline & Tabline
	Tabline* m_tabline;
	std::size_t m_tablineId;
	Statusline* m_statusline;
	std::size_t m_statuslineId;

	// Lists
	List* m_dir;
	std::size_t m_dirId;
	List* m_parent;
	std::size_t m_parentId;

	Prompt* m_prompt;
	std::size_t m_promptId;

	// Menus
	GoMenu* m_goMenu;
	std::size_t m_goMenuId;
	SortMenu* m_sortMenu;
	std::size_t m_sortMenuId;
	ShellMenu* m_shellMenu;
	std::size_t m_shellMenuId;
	MarksMenu* m_marksMenu;
	std::size_t m_marksMenuId;
	ShowMenu* m_showMenu;
	std::size_t m_showMenuId;
	ChangeMenu* m_changeMenu;
	std::size_t m_changeMenuId;

	// Explorers
	MarksExplorer* m_marksExplorer;
	std::size_t m_marksExplorerId;
	CacheExplorer* m_cacheExplorer;
	std::size_t m_cacheExplorerId;
	PositionExplorer* m_positionExplorer;
	std::size_t m_positionExplorerId;
	JobManager* m_jobManager;
	std::size_t m_jobManagerId;

	CurrentMode m_currentMode;

	std::vector<std::pair<Widget*, bool>> m_promptStateList;

	std::size_t m_tab;
	//TextBox? m_preview;
	//Tabbar m_tab;
	//Statusbar m_status;

	std::size_t m_sortFn;
	Sort::Settings m_sortSettings;

	// Setting:
	bool m_parentEnabled;

	////////////////////////////////////////////////
	/// \brief Returns wether or not to show the parent pane
	/// Will take in account the current directory & the settings
	/// \returns true if the parent pane should be shown, false otherwise
	////////////////////////////////////////////////
	bool ShouldShowParent() const;

	void OnChangeDir();

	////////////////////////////////////////////////
	/// \brief Tries to get a file's position
	/// \param name The name of the file to search for
	/// \returns The position of the file if it has been found, 0 otherwise
	////////////////////////////////////////////////
	std::size_t GetFilePosition(const String& name);
	
	////////////////////////////////////////////////
	/// \brief Enables/Disables all small widgets embedded in the MainWindow (lists and menus mainly)
	/// \param v The value (true for enabled, false for disable)
	////////////////////////////////////////////////
	void SetEnabledInternalWidgets(bool v);
public:
	MainWindow(const std::string& path, std::size_t tabId);
	~MainWindow();

	virtual void Resize(Vec2i dim);

	void UpdateFiles();
	void UpdateFilters();

	// Actions
	void Forward(const String& folder);
	void Back();

	const List* GetList() const;
	List* GetList();
	const List* GetParentList() const;
	List* GetParentList();

	////////////////////////////////////////////////
	/// \brief Draws a message in the Statusline
	/// \param msg The message
	/// \param secs The message's duration in seconds
	////////////////////////////////////////////////
	void Message(const TBString& msg, std::chrono::duration<std::size_t> secs);

	////////////////////////////////////////////////
	/// \brief Draws an error message in the Statusline
	/// \param msg The error message
	/// \param secs The message's duration in seconds
	////////////////////////////////////////////////
	void Error(const String& msg, std::chrono::duration<std::size_t> secs = std::chrono::seconds(3));

	void CD(const std::string& path);

	////////////////////////////////////////////////
	/// \brief Sets the current mode
	/// \param mode The new mode
	////////////////////////////////////////////////
	void SetMode(CurrentMode mode);

	////////////////////////////////////////////////
	/// \brief Gets the current mode
	/// \returns The current mode
	////////////////////////////////////////////////
	CurrentMode GetMode() const
	{
		return m_currentMode;
	}

	////////////////////////////////////////////////
	/// \brief Gets the tabline widget's id
	/// \returns The tabline widget's id
	////////////////////////////////////////////////
	std::size_t GetTablineID() const
	{
		return m_tablineId;
	}

	////////////////////////////////////////////////
	/// \brief Gets the sort settings
	/// \returns The sort settings
	////////////////////////////////////////////////
	const Sort::Settings& GetSortSettings() const;

	////////////////////////////////////////////////
	/// \brief Sets the sort settings
	/// \note Will run ```Directory::Sort``` on the directories to sort
	/// \param settings The sort settings
	////////////////////////////////////////////////
	void SetSortSettings(const Sort::Settings& settings);

	////////////////////////////////////////////////
	/// \brief Gets the current path
	/// \returns The current path
	////////////////////////////////////////////////
	const std::string& GetCurrentPath() const;

	////////////////////////////////////////////////
	/// \brief Displays the prompt
	/// \param callback The callback
	/// \param prefix The prompt's prefix
	/// \param bg The prompt's background
	/// \param input The prompt input
	/// \param position The start position in the input
	/// \returns A pair <true if the user submitted, false if cancelled, the output>
	////////////////////////////////////////////////
	void ActionPrompt(std::function<void(const String&)> callback, const TBString& prefix, const TBChar& bg, std::size_t max, const String& input = U"", std::size_t position = 0);

	////////////////////////////////////////////////
	/// \brief Get the current file's name
	/// \returns The name of the current file (or U"" if none)
	////////////////////////////////////////////////
	String GetCurrentFileName() const;

	////////////////////////////////////////////////
	/// \brief Switches from this to win
	/// \param win The MainWindow to switch to
	////////////////////////////////////////////////
	void Switch(MainWindow* win);

	////////////////////////////////////////////////
	/// \brief Sets the tab's id for this MainWindow
	/// \param id The new tab id
	////////////////////////////////////////////////
	void SetTabID(std::size_t id)
	{
		m_tab = id;
	}

	////////////////////////////////////////////////
	/// \brief Gets the current tab's id
	/// \returns The tab id
	////////////////////////////////////////////////
	std::size_t GetTabID() const
	{
		return m_tab;
	}
};

#endif // INDEX_MAINWINDOW_HPP
