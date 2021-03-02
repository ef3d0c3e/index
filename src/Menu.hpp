#ifndef INDEX_MENU_HPP
#define INDEX_MENU_HPP

#include "TermboxWidgets/Widgets.hpp"
class MainWindow;

class Menu : public Widget
{
	TBChar m_bg;

	std::size_t m_cols;
	std::vector<std::pair<TBString, int>> m_categories;
	std::vector<TBString> m_entries;

	bool m_locked;

	virtual void Draw();

public:
	EventListener<> OnStopShowing;
	void ActionShow();

	Menu();
	~Menu();

	void SetTable(std::size_t cols, const std::vector<std::pair<TBString, int>>& categories, const std::vector<TBString>& entries);

	void SetBackground(const TBChar& bg);
	const TBChar& GetBackground() const;

	int GetHeight() const;
};

#endif // INDEX_MENU_HPP
