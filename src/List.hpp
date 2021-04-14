#ifndef INDEX_LIST_HPP
#define INDEX_LIST_HPP

#include "FileSystem.hpp"
#include "TermboxWidgets/Widgets.hpp"
#include "Settings.hpp"
#include "MainWindow.hpp"

typedef Widgets::ListSelect<Settings::Layout::List::settings, File, MarkType> ListSelect;

class List : public ListSelect
{
	Directory* m_dir;
	MainWindow* m_main;
	bool m_mainList;

	std::pair<TBStyle, TBStyle> DrawFn(std::size_t i, Vec2i pos, int w, bool hovered, Char trailing) const;

	void ActionLeft();
	void ActionRight();
public:
	List(MainWindow* main, const std::string& path, bool input);
	~List();

	////////////////////////////////////////////////
	/// \brief Re-read from filesystem, will perform filtering
	////////////////////////////////////////////////
	void UpdateFiles();
	////////////////////////////////////////////////
	/// \brief Run the filter, sort and update the number of entries
	////////////////////////////////////////////////
	const std::size_t Size() const;
	void UpdateFilter();

	void SetShowHidden(bool v);
	bool GetShowHidden() const;

	Directory* GetDir();
	void SetDir(Directory* dir);
	void MarkFn(std::size_t i, MarkType mark);
};

#endif // INDEX_LIST_HPP
