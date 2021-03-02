#ifndef MARKS_HPP
#define MARKS_HPP

#include "Settings.hpp"
#include "TermboxWidgets/Widgets.hpp"

typedef Widgets::ListSelect<Settings::Layout::Marks::settings, File, MarkType> MarkSelect;
class MainWindow;

class Marks : public MarkSelect
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
	std::map<std::string, Marks::Marked>::iterator m_it;
public:
	Marks(MainWindow* main);
	~Marks();

	void AddMarks(const Directory* dir);
	void DelMarks(const std::string& path);
	
	void SetMarks(Directory* dir);
};

namespace Shared
{
	extern std::map<std::string, Marks::Marked>* marked;
}

#endif // MARKS_HPP
