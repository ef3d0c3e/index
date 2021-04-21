#ifndef INDEX_UI_MARKSEXPLORER_HPP
#define INDEX_UI_MARKSEXPLORER_HPP

#include "../Settings.hpp"
#include "../TermboxWidgets/Widgets.hpp"

typedef Widgets::ListSelect<Settings::Layout::Marks::settings, MarkType> MarksExplorerBase;
class MainWindow;

class MarksExplorer : public MarksExplorerBase
{
////////////////////////////////////////////////
/// \brief Class that will hold informations about marks
////////////////////////////////////////////////
public:
	struct Marked
	{
		std::array<std::size_t, MarkType::size-1> marked;
		std::vector<std::pair<String, MarkType>> elems;
	};
private:
	MainWindow* m_main;

	std::pair<TBStyle, TBStyle> DrawFn(std::size_t i, Vec2i pos, int w, bool hovered, Char trailing);
	void MarkFn(std::size_t, MarkType);

	std::size_t m_last_i;
	std::map<std::string, Marked>::iterator m_it;
public:
	MarksExplorer(MainWindow* main);
	~MarksExplorer();

	void AddMarks(const Directory* dir);
	void DelMarks(const std::string& path);
	
	void SetMarks(Directory* dir);
};

namespace Shared
{
	extern std::map<std::string, MarksExplorer::Marked>* marked;
}

#endif // INDEX_UI_MARKSEXPLORER_HPP
