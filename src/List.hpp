#ifndef INDEX_LIST_HPP
#define INDEX_LIST_HPP

#include "Settings.hpp"
#include "MainWindow.hpp"

typedef Widgets::ListSelect<Settings::Layout::List::settings, File, MarkType> ListSelect;

class List : public ListSelect
{
	Directory* m_dir;
	MainWindow* m_main;
	bool m_mainList;

	std::pair<TBStyle, TBStyle> DrawFn(std::size_t i, Vec2i pos, int w, bool hovered, Char trailing) const;

	////////////////////////////////////////////////
	/// \brief Action of leaving the current directory
	/// Will notify ```OnChangePosition```
	////////////////////////////////////////////////
	void ActionLeft();

	////////////////////////////////////////////////
	/// \brief Action of entering in a new directory or opening a file
	/// Will notify ```OnChangePosition``` only if entering a new directory
	////////////////////////////////////////////////
	void ActionRight();
public:
	////////////////////////////////////////////////
	/// \brief Constructor
	/// \param main The main window
	/// \param path The directory's path
	/// \param input Set to true if receives inputs
	/// \note If input is set to true, m_mainList will be set to true (a bit hacky)
	////////////////////////////////////////////////
	List(MainWindow* main, const std::string& path, bool input);
	~List();

	////////////////////////////////////////////////
	/// \brief Re-read from filesystem, will perform filtering
	/// Will call ```UpdateFilter()```
	////////////////////////////////////////////////
	void UpdateFiles();
	////////////////////////////////////////////////
	/// \brief Run the filter, sort and update the number of entries
	////////////////////////////////////////////////
	const std::size_t Size() const;

	////////////////////////////////////////////////
	/// \brief Update the file list to match the current filters
	/// Will also perform sorting
	////////////////////////////////////////////////
	void UpdateFilter();

	////////////////////////////////////////////////
	/// \brief Toggles wether or not to show hidden files
	/// \param v Wether or not to show hidden files
	////////////////////////////////////////////////
	void SetShowHidden(bool v);
	////////////////////////////////////////////////
	/// \brief Get wether or not hidden files are show
	/// \returns true if hidden files are shown, false otherwise
	////////////////////////////////////////////////
	bool GetShowHidden() const;

	////////////////////////////////////////////////
	/// \brief Gets the Directory
	/// \returns The current directory
	/// \see Directory
	////////////////////////////////////////////////
	Directory* GetDir();

	////////////////////////////////////////////////
	/// \brief Sets the Directory
	/// \param dir The new directory
	/// \note If a directory already exists, you will have to manually deallocate it
	/// \see Directory
	////////////////////////////////////////////////
	void SetDir(Directory* dir);

	void MarkFn(std::size_t i, MarkType mark);
};

#endif // INDEX_LIST_HPP
