#include "Marks.hpp"
#include "MainWindow.hpp"

std::map<std::string, Marks::Marked>* Shared::marked = nullptr;

std::pair<TBStyle, TBStyle> Marks::DrawFn(std::size_t i, Vec2i pos, int w, bool hovered, Char trailing)
{
	// Some optimization, because the DrawFn are called in chronological order...
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

void Marks::MarkFn(std::size_t, MarkType) { }

using namespace std::placeholders;
Marks::Marks(MainWindow* main):
	MarkSelect(std::bind(&Marks::DrawFn, this, _1, _2, _3, _4, _5), std::bind(&Marks::MarkFn, this, _1, _2)),
	m_main(main),
	m_last_i(0)
{
	// First time
	if (Shared::marked == nullptr)
		Shared::marked = new std::map<std::string, Marks::Marked>();

	m_it = Shared::marked->begin();

	// Input
	AddKeyboardInput(KeyComb(U"DOWN", [this]()
	{
		Termbox& tb = Termbox::GetTermbox();
		if (tb.GetContext().repeat == 0) [[likely]]
			ActionDown();
		else [[unlikely]]
			ActionDownN(tb.GetContext().repeat);
	}));
	AddKeyboardInput(KeyComb(U"PGDN", [this](){ ActionDownN(10); }));
	AddMouseInput(Mouse({Vec2i(0, 0), Vec2i(0, 0)}, Mouse::MOUSE_WHEEL_DOWN,
				[this](const Vec2i&){ ActionDownN(1); }));
	AddKeyboardInput(KeyComb(U"UP", [this]()
	{ 
		Termbox& tb = Termbox::GetTermbox();
		if (tb.GetContext().repeat == 0) [[likely]]
			ActionUp();
		else [[unlikely]]
			ActionUpN(tb.GetContext().repeat);
	}));
	AddKeyboardInput(KeyComb(U"PGUP", [this](){ ActionUpN(10); }));
	AddMouseInput(Mouse({Vec2i(0, 0), Vec2i(0, 0)}, Mouse::MOUSE_WHEEL_UP,
				[this](const Vec2i&){ ActionUpN(1); }));

	AddKeyboardInput(KeyComb(Settings::Keys::Go::top,  [this](){ ActionSetPosition(0); }));
	AddKeyboardInput(KeyComb(Settings::Keys::Go::bottom,  [this]()
	{
		Termbox& tb = Termbox::GetTermbox();
		if (tb.GetContext().hasRepeat)
			ActionSetPosition(tb.GetContext().repeat);
		else
			ActionSetPosition(GetEntries()-1);
	}));

	AddKeyboardInput(KeyComb(U"RIGHT", [this]()
	{
		if (Shared::marked->size() == 0)
			return;

		auto it = Shared::marked->begin();
		for (std::size_t i = 0; i < GetPos(); ++i)
			++it;
		m_main->CD(it->first);
		m_main->ToggleMarks();
	}));
	AddKeyboardInput(KeyComb(U"ENTER", [this]()
	{
		if (Shared::marked->size() == 0)
			return;

		auto it = Shared::marked->begin();
		for (std::size_t i = 0; i < GetPos(); ++i)
			++it;
		m_main->CD(it->first);
		m_main->ToggleMarks();
	}));

	SetBackground(Settings::Style::Marks::background);
}

Marks::~Marks()
{
	
}

void Marks::AddMarks(const Directory* dir)
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

void Marks::DelMarks(const std::string& path)
{
	const auto it = Shared::marked->find(path);
	if (it != Shared::marked->end())
		Shared::marked->erase(it);
	//TODO return...

	m_last_i = 0;
	m_it = Shared::marked->begin();
	SetEntries(Shared::marked->size());
}

void Marks::SetMarks(Directory* dir)
{
	const auto it = Shared::marked->find(dir->GetPath());
	if (it == Shared::marked->end())
		return;

	for (std::size_t i = 0; i < dir->Size(); ++i)
	{
		for (const auto& f : it->second.elems)
		{
			if (f.first != (*dir)[i].name)
				continue;

			(*dir)[i].mark = f.second;
		}
	}
}
