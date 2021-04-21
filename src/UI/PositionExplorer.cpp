#include "PositionExplorer.hpp"
#include "../MainWindow.hpp"
#include <fmt/chrono.h>
using namespace std::placeholders;

std::pair<TBStyle, TBStyle> PositionExplorer::DrawFn(std::size_t i, Vec2i pos, int w, bool hovered, Char trailing)
{
	// <n> <dir>            <filename>              <updated>
	// 1 /home/me/Downloads Archive.tar             (19:46:27)
	// 2 /home/me/Documents doc.tex                 (19:44:31)
	// 3 /home/me a.c                               (19:42:44)
	
	// Some optimization, because the DrawFn are called in order...
	// This makes getting the i-th element O(1) (except for the first)
	if (i != m_last_i+1)
	{
		m_it = m_main->m_positionCache.m_cache.begin();
		for (std::size_t j = 0; j < i; ++j)
			++m_it;
	}
	else
		++m_it;
	m_last_i = i;

	// Style
	TBStyle s = Settings::Style::Position::path;
	if (hovered)
		s = Settings::Style::Position::path_hovered;
	const TBChar trailChar{trailing, s};
	const auto cwidth = wcwidth(GetBackground().ch);

	// Cached
	int p(0);
	int w2 = w;

	// Update
	const String lastAccessed = fmt::format(Settings::Layout::Position::date_format, m_it->second.lastAccessed.time_since_epoch());
	w2 -= Draw::TextLine(lastAccessed, {Settings::Style::Position::last_accessed.fg, s.bg, Settings::Style::Position::last_accessed.s},
			pos+Vec2i(w2-Util::SizeWide(lastAccessed), 0), w2-p, trailChar).first;

	// Path
	p += Draw::TextLine(Util::StringConvert<Char>(m_it->first), s, pos+Vec2i(p,0), w2-p, trailChar).first;

	// Spacing
	Draw::Char({GetBackground().ch, s}, pos+Vec2i(p, 0));
	p += cwidth;

	// Filename
	p += Draw::TextLine(m_it->second.name, {Settings::Style::Position::selected.fg, s.bg, Settings::Style::Position::selected.s}, pos+Vec2i(p,0), w2-p, trailChar).first;

	// Fill
	Draw::Horizontal({GetBackground().ch, s}, pos+Vec2i(p, 0), w2-p);

	return {s, {Settings::Style::Position::numbers.fg, s.bg, Settings::Style::Position::numbers.s}};
}

void PositionExplorer::MarkFn(std::size_t, MarkType) { }

PositionExplorer::PositionExplorer(MainWindow* main):
	PositionExplorerBase(std::bind(&PositionExplorer::DrawFn, this, _1, _2, _3, _4, _5), std::bind(&PositionExplorer::MarkFn, this, _1, _2)),
	m_main(main),
	m_last_i(0)
{
	m_it = m_main->m_positionCache.m_cache.begin();

	// Input
	AddKeyboardInput(Settings::Keys::List::down, [this]
	{
		Termbox& tb = Termbox::GetTermbox();
		if (tb.GetContext().repeat == 0) [[likely]]
			ActionDown();
		else [[unlikely]]
			ActionDownN(tb.GetContext().repeat);
	});

	AddKeyboardInput(Settings::Keys::List::down_page, [this]
	{
		ActionDownN(10);
	});

	AddMouseInput(Mouse({Vec2i(0, 0), Vec2i(0, 0)}, Mouse::MOUSE_WHEEL_DOWN,
				[this](const Vec2i&){ ActionDownN(1); }));

	AddKeyboardInput(Settings::Keys::List::up, [this]
	{
		Termbox& tb = Termbox::GetTermbox();
		if (tb.GetContext().repeat == 0) [[likely]]
			ActionUp();
		else [[unlikely]]
			ActionUpN(tb.GetContext().repeat);
	});

	AddKeyboardInput(Settings::Keys::List::up_page, [this]
	{
		ActionUpN(10);
	});

	AddMouseInput(Mouse({Vec2i(0, 0), Vec2i(0, 0)}, Mouse::MOUSE_WHEEL_UP,
				[this](const Vec2i&){ ActionUpN(1); }));

	AddKeyboardInput(Settings::Keys::Go::top,  [this]
	{
		ActionSetPosition(0);
	});

	AddKeyboardInput(Settings::Keys::Go::bottom, [this]
	{
		Termbox& tb = Termbox::GetTermbox();
		if (tb.GetContext().hasRepeat)
			ActionSetPosition(tb.GetContext().repeat);
		else
			ActionSetPosition(GetEntries()-1);
	});

	AddKeyboardInput(Settings::Keys::List::right, [this]
	{
		auto it = m_main->m_positionCache.m_cache.begin();
		for (std::size_t i = 0; i < GetPos(); ++i)
			++it;
		m_main->CD(it->first);
		m_main->SetMode(MainWindow::CurrentMode::NORMAL);
	});

	AddKeyboardInput(Settings::Keys::Position::exit, [this]
	{
		m_main->SetMode(MainWindow::CurrentMode::NORMAL);
	});

	OnChangePosition.AddEvent([this]()
	{
		// Updating the repeat
		m_main->SetWidgetExpired(m_main->GetTablineID(), true);
	});

	// Update size before redraws
	OnDraw.AddEvent([this]
	{
		SetEntries(m_main->m_positionCache.m_cache.size());
	}, EventWhen::BEFORE);

	SetBackground(Settings::Style::Position::background);
}

PositionExplorer::~PositionExplorer()
{
	
}
