#ifndef INDEX_UI_CACHEEXPLORER_HPP
#define INDEX_UI_CACHEEXPLORER_HPP

#include "../TermboxWidgets/Widgets.hpp"
#include "../Settings.hpp"
#include "../DirectoryCache.hpp"

class MainWindow;
typedef Widgets::ListSelect<Settings::Layout::Cache::settings, File, MarkType> CacheExplorerBase;

class CacheExplorer : public CacheExplorerBase
{
////////////////////////////////////////////////
/// \brief Class that will help display informations about the cache state
////////////////////////////////////////////////
	MainWindow* m_main;

	std::pair<TBStyle, TBStyle> DrawFn(std::size_t i, Vec2i pos, int w, bool hovered, Char trailing);
	void MarkFn(std::size_t, MarkType);

	// Some optimization for iterating in std::map
	std::size_t m_last_i;
	std::map<std::string, DirectoryCache::Cached>::iterator m_it;
public:

	CacheExplorer(MainWindow* main);
	~CacheExplorer();
};

#endif // INDEX_UI_CACHEEXPLORER_HPP
