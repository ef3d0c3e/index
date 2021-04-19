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

	////////////////////////////////////////////////
	/// \brief Sets the start position in the prompt
	/// \param pos The position (<= m_text.size())
	////////////////////////////////////////////////
	void SetPos(std::size_t pos);

	void SetBackground(const TBChar& bg);
	const TBChar& GetBackground() const;

	void SetCompletion(const std::vector<String>& completion);

	MAKE_CENUM_Q(TypeT, std::uint8_t,
		DELETE, 0,
		CHARACTER, 1,
	);
	EventListener<TypeT, Char> OnType;
	// TODO...
};

#endif // INDEX_PROMPT_HPP
