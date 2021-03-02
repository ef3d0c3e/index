#include "Menu.hpp"
#include "MainWindow.hpp"
#include "Settings.hpp"

void Menu::Draw()
{
	auto x = GetPosition()[0];
	auto y = GetPosition()[1];
	const auto w = GetSize()[0]+1;
	const auto h = GetSize()[1];
	const int maxh = m_entries.size()/m_cols + 1;
	const TBChar trailing{Settings::trailing_character, m_bg.s};

	int sum = 0;
	int* dims = new int[m_cols];
	// Categories
	for (int i = 0; i < static_cast<int>(m_categories.size()); ++i)
	{
		const auto width = m_categories[i].second * w / 100;
		dims[i] = width;
		sum += width;
		const auto p = Draw::TextLineStyle(m_categories[i].first, TextStyle::Underline, Vec2i(x, y), width, trailing).first;
		TBChar bg = m_bg;
		bg.s.s = static_cast<std::uint32_t>(bg.s.s) | TextStyle::Underline;
		Draw::Horizontal(bg, Vec2i(x+p, y), width - p);
		x += width;
	}
	++y;
	x = GetPosition()[0];
	if (y >= h+GetPosition()[1])
	{
		delete[] dims;
		return;
	}

	for (int i = 0; i < maxh; ++i)
	{
		for (int j = 0; j < static_cast<int>(m_categories.size()); ++j)
		{
			const auto p = Draw::TextLine(m_entries[i*m_cols+j], Vec2i(x, y), dims[j], trailing).first;
			Draw::Horizontal(m_bg, Vec2i(x+p, y), dims[j]-p);
			x += dims[j];
		}

		++y;
		x = GetPosition()[0];
		if (y >= h+GetPosition()[1])
		{
			delete[] dims;
			return;
		}
	}
	
}

void Menu::ActionShow()
{
	if (IsVisible() || m_locked)
	{
		m_locked = false;
		return;
	}
	SetVisible(true);
}

Menu::Menu():
	m_bg(U' ', Settings::Style::Menu::background),
	m_locked(false)
{
	SetVisible(false);

	AddKeyboardInput({U"#ANY", [this](){
		if (!IsVisible())
		{
			m_locked = false;
			return;
		}

		OnStopShowing.Notify<EventWhen::BEFORE>();
		SetVisible(false);
		m_locked = true;
		OnStopShowing.Notify<EventWhen::AFTER>();
	}});
}

Menu::~Menu()
{
	
}

void Menu::SetTable(std::size_t cols, const std::vector<std::pair<TBString, int>>& categories, const std::vector<TBString>& entries)
{
	m_cols = cols;
	m_categories = categories;
	m_entries = entries;
}

void Menu::SetBackground(const TBChar& bg)
{
	m_bg = bg;
}

const TBChar& Menu::GetBackground() const
{
	return m_bg;
}

int Menu::GetHeight() const
{
	return m_entries.size()/m_cols + 1;
}
