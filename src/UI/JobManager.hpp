#ifndef INDEX_UI_JOBGMANAGER_HPP
#define INDEX_UI_JOBGMANAGER_HPP

#include "../TermboxWidgets/Widgets.hpp"
#include "../Settings.hpp"
#include "../Actions/Jobs.hpp"

class MainWindow;
typedef Widgets::ListSelect<Settings::Layout::JobManager::settings, MarkType> JobManagerBase;


////////////////////////////////////////////////
/// \brief Class that will list all running/pending/finished jobs
////////////////////////////////////////////////
class JobManager : public JobManagerBase
{
	MainWindow* m_main;

	std::pair<TBStyle, TBStyle> DrawFn(std::size_t i, Vec2i pos, int w, bool hovered, Char trailing);
	void MarkFn(std::size_t, MarkType);
public:
	////////////////////////////////////////////////
	/// \brief Constructor
	/// \param main The MainWindow tied to tthe widget
	////////////////////////////////////////////////
	JobManager(MainWindow* main);
	////////////////////////////////////////////////
	/// \brief Destructor
	////////////////////////////////////////////////
	~JobManager();
};

#endif // INDEX_UI_JOBGMANAGER_HPP
