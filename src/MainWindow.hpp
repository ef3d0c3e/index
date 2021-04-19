#ifndef INDEX_MAINWINDOW_HPP
#define INDEX_MAINWINDOW_HPP

#include "TermboxWidgets/Window.hpp"
#include "Sort.hpp"
class List;
class Tabline;
class Statusline;
class Menu;
class MarksMenu;
class Prompt;
class CacheExplorer;
class SortMenu;

class DummyInput : public Widget
{
	virtual void Draw() { }
public:
	DummyInput() { }
	~DummyInput() { }
};

class MainWindow : public Window
{
	friend class CacheExplorer;
	friend class MarksMenu;

public:
	MAKE_CENUM_Q(CurrentMode, std::uint8_t,
		NORMAL, 0, // Default mode (directory list)
		MARKS, 1, // Marks mode
		CACHE_EXPLORER, 2, // Cache explorer mode
	);

	struct MainSettings
	{
		bool showParent = true;
	};
private:
	MainSettings m_settings;

	DummyInput* dInput;

	List* m_dir;
	std::size_t m_dirId;
	List* m_parent;
	std::size_t m_parentId;
	Tabline* m_tabline;
	std::size_t m_tablineId;
	Statusline* m_statusline;
	std::size_t m_statuslineId;

	Menu* m_goMenu;
	std::size_t m_goMenuId;
	Menu* m_marksMenu;
	std::size_t m_marksMenuId;
	Menu* m_showMenu;
	std::size_t m_showMenuId;

	Menu* m_changeMenu;
	std::size_t m_changeMenuId;

	SortMenu* m_sortMenu;
	std::size_t m_sortMenuId;

	MarksMenu* m_marks;
	std::size_t m_marksId;

	CacheExplorer* m_cacheExplorer;
	std::size_t m_cacheExplorerId;

	CurrentMode m_currentMode;

	Prompt* m_prompt;
	std::size_t m_promptId;

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
	const List* GetParentList() const;

	////////////////////////////////////////////////
	/// \brief Draws an error message in the status line
	/// \param msg The error message
	/// \param secs The message's duration in seconds
	////////////////////////////////////////////////
	void Error(const String& msg, std::chrono::duration<std::size_t> secs = std::chrono::seconds(5));

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
	CurrentMode GetMode() const;

	std::size_t GetTab() const;

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
};

#endif // INDEX_MAINWINDOW_HPP

