#ifndef INDEX_UI_POSITIONEXPLORER_HPP
#define INDEX_UI_POSITIONEXPLORER_HPP

#include "../TermboxWidgets/Widgets.hpp"
#include "../Settings.hpp"
#include "../PositionCache.hpp"

class MainWindow;
typedef Widgets::ListSelect<Settings::Layout::Position::settings, MarkType> PositionExplorerBase;

class PositionExplorer : public PositionExplorerBase
{
////////////////////////////////////////////////
/// \brief Class that will help display informations about the position cache state
////////////////////////////////////////////////
	MainWindow* m_main;

	std::pair<TBStyle, TBStyle> DrawFn(std::size_t i, Vec2i pos, int w, bool hovered, Char trailing);
	void MarkFn(std::size_t, MarkType);

	// Some optimization for iterating over std::map
	std::size_t m_last_i;
	std::map<std::string, PositionCache::Entry>::iterator m_it;
public:

	////////////////////////////////////////////////
	/// \brief Constructor
	/// \param main The MainWindow tied to tthe widget
	////////////////////////////////////////////////
	PositionExplorer(MainWindow* main);
	////////////////////////////////////////////////
	/// \brief Destructor
	////////////////////////////////////////////////
	~PositionExplorer();
};


#endif // INDEX_UI_POSITIONEXPLORER_HPP
