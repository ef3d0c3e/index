#include "MarksExplorer.hpp"
#include "../MainWindow.hpp"

std::map<std::string, MarksExplorer::Marked>* Shared::marked = nullptr;

std::pair<TBStyle, TBStyle> MarksExplorer::DrawFn(std::size_t i, Vec2i pos, int w, bool hovered, Char trailing)
{
	// Some optimization, because the DrawFn are called in order...
	// This makes getting the i-th element O(1) (except for the first)
	if (i != m_last_i+1)
	{
		m_it = Shared::marked->begin();
		for (std::size_t j = 0; j < i; ++j)
			++m_it;
	}
	else
		++m_it;
	m_last_i = i;

	// Style
	TBStyle s = Settings::Style::Marks::path;
	if (hovered)
		s = Settings::Style::Marks::path_hovered;
	const TBChar trailChar{trailing, s};
	const auto cwidth = wcwidth(GetBackground().ch);

	// Marks
	int p(0);
	int w2 = w;
	const auto fn = [&](std::size_t i)
	{
		if (m_it->second.marked[i] != 0)
		{
			// Prefix
			w2 -= Draw::TextLineBackground(Settings::Style::Marks::mark_prefix[i], s.bg,
					pos+Vec2i(w2-Settings::Style::Marks::mark_prefix[i].SizeWide(),0), w2-p, trailChar).first;

			// Spacing
			Draw::Char({GetBackground().ch, s}, pos+Vec2i(w2-cwidth, 0));
			w2 -= cwidth;

			// Number
			const String number = Util::ToString<Settings::Layout::Marks::settings.NumberBase>(m_it->second.marked[i]);
			w2 -= Draw::TextLine(number, {Settings::Style::Marks::mark_numbers[i].fg, s.bg, Settings::Style::Marks::mark_numbers[i].s},
					pos+Vec2i(w2-Util::SizeWide(number),0), w2-p, trailChar).first;

			// Spacing
			Draw::Char({GetBackground().ch, s}, pos+Vec2i(w2-cwidth, 0));
			w2 -= cwidth;
		}
	};
	[&]<std::size_t... i>(std::index_sequence<i...>)
	{
		(( fn(i) ), ...);
	}
	(std::make_index_sequence<MarkType::size-1>{});

	// Path
	p += Draw::TextLine(Util::StringConvert<Char>(m_it->first), s, pos+Vec2i(p,0), w2-p, trailChar).first;

	// Fill
	Draw::Horizontal({GetBackground().ch, s}, pos+Vec2i(p, 0), w2-p);

	return {s, s};
}

void MarksExplorer::MarkFn(std::size_t, MarkType) { }

using namespace std::placeholders;
MarksExplorer::MarksExplorer(MainWindow* main):
	MarksExplorerBase(std::bind(&MarksExplorer::DrawFn, this, _1, _2, _3, _4, _5), std::bind(&MarksExplorer::MarkFn, this, _1, _2)),
	m_main(main),
	m_last_i(0)
{
	// First time
	if (Shared::marked == nullptr)
		Shared::marked = new std::map<std::string, MarksExplorer::Marked>();

	m_it = Shared::marked->begin();

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

	AddKeyboardInput(Settings::Keys::Go::top, [this]
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
		if (Shared::marked->size() == 0)
			return;

		auto it = Shared::marked->begin();
		for (std::size_t i = 0; i < GetPos(); ++i)
			++it;
		m_main->CD(it->first);
		m_main->SetMode(MainWindow::CurrentMode::NORMAL);
	});

	AddKeyboardInput(Settings::Keys::Marks::exit, [this]
	{
		m_main->SetMode(MainWindow::CurrentMode::NORMAL);
	});

	OnChangePosition.AddEvent([this]()
	{
		// Updating the repeat
		m_main->SetWidgetExpired(m_main->GetTablineID(), true);
	});

	SetBackground(Settings::Style::Marks::background);
}

MarksExplorer::~MarksExplorer()
{
	
}

void MarksExplorer::AddMarks(const Directory* dir)
{
	// Path & check
	const auto it = Shared::marked->find(dir->GetPath());
	if (it != Shared::marked->end())
		Shared::marked->erase(it);

	Marked marked;
	[&]<std::size_t... i>(std::index_sequence<i...>)
	{
		(( marked.marked[i] = 0 ), ...);
	}
	(std::make_index_sequence<marked.marked.size()>{});
	for (std::size_t i = 0; i < dir->Size(); ++i)
	{
		const File& f = (*dir)[i];

		if (f.mark != MarkType::NONE)
		{
			marked.elems.push_back({f.name, f.mark});

			[&]<std::size_t... i>(std::index_sequence<i...>)
			{
				(( (MarkType::get<i+1>() & f.mark) ? ++marked.marked[i] : 0 ), ...);
			}
			(std::make_index_sequence<MarkType::size-1>{});
		}
	}
	if (marked.elems.size() != 0)
		Shared::marked->insert({dir->GetPath(), std::move(marked)});

	m_last_i = 0;
	m_it = Shared::marked->begin();
	SetEntries(Shared::marked->size());
}

void MarksExplorer::DelMarks(const std::string& path)
{
	const auto it = Shared::marked->find(path);
	if (it != Shared::marked->end()) [[likely]]
		Shared::marked->erase(it);
	//TODO return...

	m_last_i = 0;
	m_it = Shared::marked->begin();
	SetEntries(Shared::marked->size());
}

void MarksExplorer::SetMarks(Directory* dir)
{
	const auto it = Shared::marked->find(dir->GetPath());
	if (it == Shared::marked->end())
		return;

	for (std::size_t i = 0; i < it->second.elems.size(); ++i)
	{
		bool matched = false;
		for (std::size_t j = 0; j < dir->Size(); ++j)
		{
			if ((*dir)[j].name == it->second.elems[i].first) [[unlikely]]
			{
				(*dir)[j].mark = it->second.elems[i].second;
				matched = true;
				break;
			}
		}
		if (!matched) [[unlikely]] // Because some files might have disappeared
			it->second.elems.erase(it->second.elems.begin()+i);
	}
	if (it->second.elems.size() == 0)
	{
		Shared::marked->erase(it);
		SetEntries(Shared::marked->size());
	}
}
