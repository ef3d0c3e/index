#include "Prompt.hpp"
#include "Settings.hpp"

void Prompt::Draw()
{
	const TBChar trailing{Settings::Layout::trailing_character, m_bg.s};

	// Prefix
	int p = 0;
	if (m_prefix.Size())
	{
		p += Draw::TextLine(m_prefix, GetPosition(), GetSize()[0], trailing).first;
		if (GetSize()[0] < p)
			return;
	}

	// Text & cursor
	p += Draw::TextLine(m_text, m_bg.s, GetPosition()+Vec2i(p, 0), GetSize()[0]-p, trailing).first;
	Termbox::SetCursor(GetPosition() + Vec2i(m_prefixWidth+m_cursorPos, 0));
	if (GetSize()[0] < p)
		return;

	// Fill
	Draw::Horizontal(m_bg, GetPosition()+Vec2i(p, 0), GetSize()[0]-p);
}

Prompt::Prompt(const TBString& prefix, const String& text):
	m_prefix(prefix),
	m_text(text),
	m_bg(U' ', Settings::Style::default_text_style),
	m_prefixWidth(prefix.SizeWide()),
	m_position(text.size()),
	m_cursorPos(Util::SizeWide(text))
{
	SetVisible(false);
	SetActive(false);

	AddKeyboardInput({U"#SCHAR", [this](){
		const Char ch = Termbox::GetContext().ev.ch;
		m_text.insert(m_text.begin()+m_position, ch);
		++m_position;
		m_cursorPos += wcwidth(ch);
	}});

	AddKeyboardInput({Settings::Keys::Prompt::submit, [this](){
		OnStopShowing.Notify<EventWhen::BEFORE>(true);

		SetVisible(false);
		SetActive(false);
		Termbox::SetCursor(Vec2i(-1, -1));

		Termbox::GetContext().stopInput = true;
		OnStopShowing.Notify<EventWhen::AFTER>(true);
	}});

	AddKeyboardInput({Settings::Keys::Prompt::cancel, [this](){
		OnStopShowing.Notify<EventWhen::BEFORE>(false);

		SetVisible(false);
		SetActive(false);
		Termbox::SetCursor(Vec2i(-1, -1));

		Termbox::GetContext().stopInput = true;
		OnStopShowing.Notify<EventWhen::AFTER>(false);
	}});

	const auto Left = [this]()
	{
		if (m_position == 0) [[unlikely]]
			return;

		--m_position;
		if (m_position+1 == m_text.size()) [[unlikely]]
			--m_cursorPos;
		else [[likely]]
			m_cursorPos -= wcwidth(m_text[m_position+1]);
	};

	const auto Right = [this]()
	{
		if (m_position >= m_text.size()) [[unlikely]]
			return;

		++m_position;
		if (m_position == m_text.size()) [[unlikely]]
			++m_cursorPos;
		else
			m_cursorPos += wcwidth(m_text[m_position]);
	};

	const auto Remove = [this, Left]()
	{
		if (m_position == 0) [[unlikely]]
			return;

		Left();
		m_text.erase(m_position, 1);
	};

	AddKeyboardInput({Settings::Keys::Prompt::left, Left});
	AddKeyboardInput({Settings::Keys::Prompt::right, Right});
	AddKeyboardInput({Settings::Keys::Prompt::remove, Remove});

	AddKeyboardInput({Settings::Keys::Prompt::word_left, [this, Left]() {
		if (m_position < 2) [[unlikely]]
		{
			m_position = m_cursorPos = 0;
			return;
		}

		const auto pos = m_text.rfind(U' ', m_position-2);
		if (pos == String::npos) [[unlikely]]
		{
			m_position = m_cursorPos = 0;
			return;
		}
		
		for (std::size_t i = m_position; i > pos+1; --i)
			Left();
	}});

	AddKeyboardInput({Settings::Keys::Prompt::word_right, [this, Right]() {
		if (m_position+2 > m_text.size()) [[unlikely]]
		{
			m_position = m_text.size()-1;
			m_cursorPos = Util::SizeWide(m_text)-1;
			return;
		}

		const auto pos = m_text.find(U' ', m_position+2);
		if (pos == String::npos) [[unlikely]]
		{
			m_position = m_text.size()-1;
			m_cursorPos = Util::SizeWide(m_text)-1;
			return;
		}
		
		for (std::size_t i = m_position; i+1 < pos; ++i)
			Right();
	}});

	AddKeyboardInput({Settings::Keys::Prompt::word_kill, [this, Remove]() {
		if (m_position < 2) [[unlikely]]
		{
			m_position = m_cursorPos = 0;
			return;
		}

		auto pos = m_text.rfind(U' ', m_position-2);
		if (pos == String::npos) [[unlikely]]
			pos = 0;
		
		for (std::size_t i = m_position; i > pos; --i)
			Remove();
	}});

	AddKeyboardInput({Settings::Keys::Prompt::begining, [this]() {
		m_position = m_cursorPos = 0;
	}});

	AddKeyboardInput({Settings::Keys::Prompt::end, [this]() {
		m_position = m_text.size();
		m_cursorPos = Util::SizeWide(m_text);
	}});
}

Prompt::~Prompt()
{
}

void Prompt::ActionShow()
{
	SetVisible(true);
	SetActive(true);
}

void Prompt::SetBackground(const TBChar& bg)
{
	m_bg = bg;
}

const TBChar& Prompt::GetBackground() const
{
	return m_bg;
}
