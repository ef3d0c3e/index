#ifndef INDEX_TAB_HPP
#define INDEX_TAB_HPP

#include "MainWindow.hpp"

class Tab
{
	std::size_t m_mainId;

	bool m_shouldUpdate; ///< Wether UpdateFromDir() should be called whjen we switch to this tab
public:
	Tab();
	~Tab();

	void SetMainWindow(std::size_t mainId);
	MainWindow* GetMainWindow();
	std::size_t GetMainWindowId();

	////////////////////////////////////////////////
	/// \brief Sets whether or not the lists should be updated when this tab is switched as current
	/// \param v The boolean value
	////////////////////////////////////////////////
	void SetShouldUpdate(bool v);

	////////////////////////////////////////////////
	/// \brief Gets wether or not the lists should be updated
	/// \returns A boolean indicating whether or not to update the lists
	////////////////////////////////////////////////
	bool ShouldUpdate() const;
};

extern std::deque<Tab> gTabs;

#endif // INDEX_TAB_HPP
