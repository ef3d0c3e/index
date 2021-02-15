#ifndef INDEX_TABLINE_HPP
#define INDEX_TABLINE_HPP

#include "MainWindow.hpp"

class Tabline : public Widget
{
	MainWindow* m_main;
	TBChar m_bg;

	std::size_t m_repeatId;

	int m_leftSpace;

	virtual void Draw();
	void DrawRepeat();
public:
	Tabline(MainWindow* main);
	~Tabline();

	EventListener<bool> OnSetLoadingEnabled;
	void SetLoadingEnabled(bool enabled);
	bool GetLoadingEnabled() const;

	void SetBackground(const TBChar& bg);
	const TBChar& GetBackground() const;
};

#endif // INDEX_TABLINE_HPP
