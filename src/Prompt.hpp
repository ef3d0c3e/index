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

	std::vector<String> m_completion;

	virtual void Draw();
public:
	Prompt(const TBString& m_prefix, const String& text = U"");
	~Prompt();

	EventListener<bool> OnStopShowing; // true -> submited, false -> cancelled
	void ActionShow();

	void SetPrefix(const TBString& prefix);
	const TBString& GetPrefix() const;

	void SetText(const String& text);
	const String& GetText() const;

	void SetBackground(const TBChar& bg);
	const TBChar& GetBackground() const;

	void SetCompletion(const std::vector<String>& completion);
};

#endif // INDEX_PROMPT_HPP
