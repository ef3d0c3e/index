#include "Tabline.hpp"
#include "Settings.hpp"
#include "List.hpp"
#include "Tab.hpp"
#include <unistd.h>

void Tabline::Draw()
{
	char buff[255];
	constexpr std::size_t buf_size = sizeof(buff)/sizeof(buff[0]);
	//* Left side
	
	// Margin
	auto x = GetPosition()[0];
	const auto y = GetPosition()[1];
	const auto w = GetSize()[0]+x;
	if constexpr (Settings::Layout::Tabline::left_margin != 0)
	{
		Draw::Horizontal(m_bg, Vec2i(x, y), Settings::Layout::Tabline::left_margin);
		x += Settings::Layout::Tabline::left_margin;
	}
	if (x >= w)
		return;
	
	// Hostname
	{
		const int err = gethostname(buff, buf_size);
		String hostname;
		if (err == 0)
			x += Draw::TextLine(Util::StringConvert<Char>(std::string(buff)), Settings::Style::Tabline::hostname, Vec2i(x, y), w-x,
				{Settings::Layout::trailing_character, Settings::Style::Tabline::hostname}, 0).first;
		else
			x += Draw::TextLine(Settings::Layout::unknown_username, Settings::Style::Tabline::hostname, Vec2i(x, y), w-x,
				{Settings::Layout::trailing_character, Settings::Style::Tabline::hostname}, 0).first;
		if (x >= w)
			return;
	}

	// Separator
	{
		x += Draw::TextLine(Settings::Layout::Tabline::separator, Settings::Style::Tabline::separator, Vec2i(x, y), w-x,
				{Settings::Layout::trailing_character, Settings::Style::Tabline::separator}, 0).first;
		if (x >= w)
			return;
	}

	// Username
	{
		const int err = getlogin_r(buff, buf_size);
		if (err == 0)
			x += Draw::TextLine(Util::StringConvert<Char>(std::string(buff)), Settings::Style::Tabline::username, Vec2i(x, y), w-x,
				{Settings::Layout::trailing_character, Settings::Style::Tabline::username}, 0).first;
		else
			x += Draw::TextLine(Settings::Layout::unknown_username, Settings::Style::Tabline::username, Vec2i(x, y), w-x,
				{Settings::Layout::trailing_character, Settings::Style::Tabline::username}, 0).first;
		if (x >= w)
			return;
	}

	// Spacing
	const auto cwidth = wcwidth(m_bg.ch);
	{
		Draw::Char(m_bg, Vec2i(x, y));
		x += cwidth;
		if (x >= w)
			return;
	}

	// Directory
	{
		const String path = Util::StringConvert<Char>(m_main->GetDir()->GetPath());
		if (path.size() != 1 || path[0] != U'/')
		{
			x += Draw::TextLine(path, Settings::Style::Tabline::directory, Vec2i(x, y), w-x,
					{Settings::Layout::trailing_character, Settings::Style::Tabline::directory}, 0).first;
			if (x >= w)
				return;
		}
	}

	// Separator
	{
		x += Draw::TextLine(Settings::Layout::Tabline::directory_separator, Settings::Style::Tabline::directory_separator, Vec2i(x, y), w-x,
				{Settings::Layout::trailing_character, Settings::Style::Tabline::directory_separator}, 0).first;
		if (x >= w)
			return;
	}

	// Selected entry
	if (m_main->GetDir()->Size() != 0)
	{
		x += Draw::TextLine((*m_main->GetDir())[m_main->GetList()->GetPos()].name, Settings::Style::Tabline::selected, Vec2i(x, y), w-x,
				{Settings::Layout::trailing_character, Settings::Style::Tabline::selected}, 0).first;
		if (x >= w)
			return;
	}

	// Tab
	int w2 = w;
	{
		const auto fn = [&](std::size_t i)
		{
			// Spacing
			{
				Draw::Char(m_bg, GetPosition()+Vec2i(w2-cwidth, 0));
				w2 -= cwidth;
			}

			//Tab
			{
				const auto s = Util::ToString(i);
				if (m_main->GetTab() == i) [[unlikely]]
					w2 -= Draw::TextLine(s, Settings::Style::Tabline::tab_current, GetPosition() + Vec2i(w2-Util::SizeWide(s), 0),
						w2, {Settings::Layout::trailing_character, Settings::Style::Tabline::tab_current}).first;
				else [[likely]]
					w2 -= Draw::TextLine(s, Settings::Style::Tabline::tab, GetPosition() + Vec2i(w2-Util::SizeWide(s), 0),
						w2, {Settings::Layout::trailing_character, Settings::Style::Tabline::tab}).first;
			}
		};

		for (std::size_t i = 0;
				i < Tabs.size(); ++i)
			fn(i);
	}
	
	// Fill
	{
		Draw::Horizontal(m_bg, Vec2i(x, y), w2-x);
		m_leftSpace = w2-x;
	}
	
}

void Tabline::DrawRepeat()
{
	// Margin
	auto x = GetPosition()[0]+GetSize()[0];
	const auto y = GetPosition()[1];
	if constexpr (Settings::Layout::Tabline::right_margin != 0)
	{
		Draw::Horizontal(m_bg, Vec2i(x-Settings::Layout::Tabline::right_margin, y), Settings::Layout::Tabline::right_margin);
		x -= Settings::Layout::Tabline::right_margin;
	}
	if (x <= 0)
		return;

	// Repeat
	//const static auto repeat_len = Util::GetDigitsNum<10>(std::numeric_limits<decltype(Termbox::GetContext().repeat)>::max());
	if (Termbox::GetContext().hasRepeat)
	{
		const auto len = Util::GetDigitsNum<10>(Termbox::GetContext().repeat);
		x -= len;
		Draw::TextLine(Util::ToString(Termbox::GetContext().repeat), Settings::Style::Tabline::repeat, Vec2i(x, y), len,
				{Settings::Layout::trailing_character, Settings::Style::Tabline::repeat}, 0).first;
	}
	
}

Tabline::Tabline(MainWindow* main):
	m_main(main),
	m_bg(Settings::Style::Tabline::background),
	m_leftSpace(0)
{
	m_repeatId = Termbox::OnRepeatChange.AddEvent(std::bind(&Tabline::DrawRepeat, this), EventWhen::AFTER);
}

Tabline::~Tabline()
{
	Termbox::OnRepeatChange.RemoveEvent(m_repeatId, EventWhen::AFTER);
}

void Tabline::SetLoadingEnabled(bool enabled)
{
	OnSetLoadingEnabled.Notify<EventWhen::BEFORE>(enabled);

	//TODO

	OnSetLoadingEnabled.Notify<EventWhen::AFTER>(enabled);
}

bool Tabline::GetLoadingEnabled() const
{
	return false; //TODO
}


void Tabline::SetBackground(const TBChar& bg)
{
	m_bg = bg;
}

const TBChar& Tabline::GetBackground() const
{
	return m_bg;
}

