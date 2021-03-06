#ifndef INDEX_UI_CACHEEXPLORER_HPP
#define INDEX_UI_CACHEEXPLORER_HPP

#include "../TermboxWidgets/Widgets.hpp"
#include "../Settings.hpp"
#include "../DirectoryCache.hpp"

class MainWindow;
typedef Widgets::ListSelect<Settings::Layout::Cache::settings, MarkType> CacheExplorerBase;

////////////////////////////////////////////////
/// \brief Class that will help display informations about the cache state
////////////////////////////////////////////////
class CacheExplorer : public CacheExplorerBase
{
	MainWindow* m_main;

	std::pair<TBStyle, TBStyle> DrawFn(std::size_t i, Vec2i pos, int w, bool hovered, Char trailing);
	void MarkFn(std::size_t, MarkType);

	// Some optimization for iterating over std::map
	std::size_t m_last_i;
	std::map<std::string, DirectoryCache::Cached>::iterator m_it;
public:
	////////////////////////////////////////////////
	/// \brief Constructor
	/// \param main The MainWindow tied to tthe widget
	////////////////////////////////////////////////
	CacheExplorer(MainWindow* main);
	////////////////////////////////////////////////
	/// \brief Destructor
	////////////////////////////////////////////////
	~CacheExplorer();
};

#endif // INDEX_UI_CACHEEXPLORER_HPP
