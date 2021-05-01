#ifndef INDEX_UI_CLIPBOARDEXPLORER_HPP
#define INDEX_UI_CLIPBOARDEXPLORER_HPP

#include "../TermboxWidgets/Widgets.hpp"
#include "../Settings.hpp"
#include "../Actions/Jobs.hpp"

class MainWindow;
typedef Widgets::ListSelect<Settings::Layout::Clipboard::settings, MarkType> ClipboardExplorerBase;


////////////////////////////////////////////////
/// \brief Class that will list all running/pending/finished jobs
////////////////////////////////////////////////
class ClipboardExplorer : public ClipboardExplorerBase
{
	MainWindow* m_main;

	std::pair<TBStyle, TBStyle> DrawFn(std::size_t i, Vec2i pos, int w, bool hovered, Char trailing);
	void MarkFn(std::size_t, MarkType);
public:
	////////////////////////////////////////////////
	/// \brief Constructor
	/// \param main The MainWindow tied to the widget
	////////////////////////////////////////////////
	ClipboardExplorer(MainWindow* main);
	////////////////////////////////////////////////
	/// \brief Destructor
	////////////////////////////////////////////////
	~ClipboardExplorer();
};

#endif // INDEX_UI_CLIPBOARDEXPLORER_HPP
