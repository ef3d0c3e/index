#ifndef INDEX_LIST_HPP
#define INDEX_LIST_HPP

#include "FileSystem.hpp"
#include "TermboxWidgets/Widgets.hpp"
#include "Settings.hpp"
#include "MainWindow.hpp"

typedef Widgets::ListSelect<Settings::Layout::List::settings, File> ListSelect;

class List : public ListSelect
{
	Directory* m_dir;
	MainWindow* m_main;
	bool m_mainList;

	std::pair<TBStyle, TBStyle> DrawFn(std::size_t i, Vec2i pos, int w, bool hovered, ListSelect::MarkType mark, Char trailing) const;
	void ActionLeft();
	void ActionRight();
public:
	List(MainWindow* main, const std::string& path, bool input);
	~List();

	void SetSettings(const Directory::DirectorySettings& settings);
	void UpdateFiles();

	Directory* GetDir();
	void SetDir(Directory* dir);
};

#endif // INDEX_LIST_HPP
