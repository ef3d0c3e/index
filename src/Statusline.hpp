#ifndef INDEX_STATUSLINE_HPP
#define INDEX_STATUSLINE_HPP

#include "MainWindow.hpp"

class Statusline : public Widget
{
	MainWindow* m_main;
	TBChar m_bg;

	virtual void Draw();
	bool m_error;
public:
	Statusline(MainWindow* main);
	~Statusline();

	void SetBackground(const TBChar& bg);
	const TBChar& GetBackground() const;

	void DrawError(const String& msg);
};

#endif // INDEX_STATUSLINE_HPP
