#ifndef INDEX_PROMPT_HPP
#define INDEX_PROMPT_HPP

#include "TermboxWidgets/Widgets.hpp"

class Prompt : public Widget
{
	TBString m_prefix;
	String m_text;
	TBChar m_bg;

	int m_prefixWidth;
	std::size_t m_position;
	int m_cursorPos;

	virtual void Draw();
public:
	Prompt(const TBString& m_prefix, const String& text = U"");
	~Prompt();

	EventListener<bool> OnStopShowing; // true -> validated, false -> cancelled
	void ActionShow();

	void SetBackground(const TBChar& bg);
	const TBChar& GetBackground() const;
};

#endif // INDEX_PROMPT_HPP
