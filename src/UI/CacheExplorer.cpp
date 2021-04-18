#include "CacheExplorer.hpp"
#include "../MainWindow.hpp"
using namespace std::placeholders;

std::pair<TBStyle, TBStyle> CacheExplorer::DrawFn(std::size_t i, Vec2i pos, int w, bool hovered, Char trailing)
{
	// <n> <dir>                          <updated> <refcount>
	// 1 /home/me/Downloads                     (19:46:27) [3]
	// 2 /home/me/Documents                     (19:44:31) [1]
	// 3 /home/me                               (19:42:44) [0]
	
	// Some optimization, because the DrawFn are called in order...
	// This makes getting the i-th element O(1) (except for the first)
	if (i != m_last_i+1)
	{
		m_it = gDirectoryCache.m_cache.begin();
		for (std::size_t j = 0; j < i; ++j)
			++m_it;
	}
	else
		++m_it;
	m_last_i = i;

	// Style
	TBStyle s = Settings::Style::Cache::path;
	if (hovered)
		s = Settings::Style::Cache::path_hovered;
	const TBChar trailChar{trailing, s};
	const auto cwidth = wcwidth(GetBackground().ch);

	// Cached
	int p(0);
	int w2 = w;

	// RefCount
	const TBString refCount = {fmt::format(Settings::Layout::Cache::ref_count_format, m_it->second.refCount), Settings::Style::Cache::ref_count};
	w2 -= Draw::TextLineBackground(refCount, s.bg,
			pos+Vec2i(w2-refCount.SizeWide(), 0), w2-p, trailChar).first;

	// Spacing
	Draw::Char({GetBackground().ch, s}, pos+Vec2i(w2-cwidth, 0));
	w2 -= cwidth;

	// Update
	TBString updated;
	char buff[Settings::Layout::Cache::date_max_size];
	std::size_t ret = std::strftime(buff, Settings::Layout::Cache::date_max_size-1, Settings::Layout::Cache::date_format, localtime(&m_it->second.updated));
	if (ret != 0)
		updated = TBString(Util::StringConvert<Char>(std::string(buff)), Settings::Style::Cache::update);
	else
		updated = TBString(Settings::Layout::Cache::unknown_date, Settings::Style::Cache::update);
	w2 -= Draw::TextLineBackground(updated, s.bg,
			pos+Vec2i(w2-updated.SizeWide(), 0), w2-p, trailChar).first;

	// Path
	p += Draw::TextLine(Util::StringConvert<Char>(m_it->first), s, pos+Vec2i(p,0), w2-p, trailChar).first;

	// Fill
	Draw::Horizontal({GetBackground().ch, s}, pos+Vec2i(p, 0), w2-p);

	return {s, s};
}

void CacheExplorer::MarkFn(std::size_t, MarkType) { }

CacheExplorer::CacheExplorer(MainWindow* main):
	CacheExplorerBase(std::bind(&CacheExplorer::DrawFn, this, _1, _2, _3, _4, _5), std::bind(&CacheExplorer::MarkFn, this, _1, _2)),
	m_main(main),
	m_last_i(0)
{
	m_it = gDirectoryCache.m_cache.begin();

	// Input
	AddKeyboardInput(Settings::Keys::List::down, [this]
	{
		Termbox& tb = Termbox::GetTermbox();
		if (tb.GetContext().repeat == 0) [[likely]]
			ActionDown();
		else [[unlikely]]
			ActionDownN(tb.GetContext().repeat);
	});

	AddKeyboardInput(Settings::Keys::List::down_page, [this]{ ActionDownN(10); });

	AddMouseInput(Mouse({Vec2i(0, 0), Vec2i(0, 0)}, Mouse::MOUSE_WHEEL_DOWN,
				[this](const Vec2i&){ ActionDownN(1); }));

	AddKeyboardInput(Settings::Keys::List::down, [this]
	{
		Termbox& tb = Termbox::GetTermbox();
		if (tb.GetContext().repeat == 0) [[likely]]
			ActionUp();
		else [[unlikely]]
			ActionUpN(tb.GetContext().repeat);
	});

	AddKeyboardInput(Settings::Keys::List::up_page, [this]{ ActionUpN(10); });

	AddMouseInput(Mouse({Vec2i(0, 0), Vec2i(0, 0)}, Mouse::MOUSE_WHEEL_UP,
				[this](const Vec2i&){ ActionUpN(1); }));

	AddKeyboardInput(Settings::Keys::Go::top,  [this]{ ActionSetPosition(0); });

	AddKeyboardInput(Settings::Keys::Go::bottom,  [this]
	{
		Termbox& tb = Termbox::GetTermbox();
		if (tb.GetContext().hasRepeat)
			ActionSetPosition(tb.GetContext().repeat);
		else
			ActionSetPosition(GetEntries()-1);
	});

	AddKeyboardInput(Settings::Keys::List::right, [this]
	{
		auto it = gDirectoryCache.m_cache.begin();
		for (std::size_t i = 0; i < GetPos(); ++i)
			++it;
		m_main->CD(it->first);
		m_main->SetMode(MainWindow::CurrentMode::NORMAL);
	});

	// Update size before redraws
	OnDraw.AddEvent([this]
	{
		SetEntries(gDirectoryCache.m_cache.size());
	}, EventWhen::BEFORE);

	SetBackground(Settings::Style::Cache::background);
}

CacheExplorer::~CacheExplorer()
{
	
}
