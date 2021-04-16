#include "List.hpp"
#include "Actions/Actions.hpp"
#include <algorithm>

std::pair<TBStyle, TBStyle> List::DrawFn(std::size_t i, Vec2i pos, int w, bool hovered, Char trailing) const
{
	const auto& [f, match] = m_dir->Get(i);
	auto [icon, c, ts] = FileType::CompiledIcons[f.ftId];
	const TBChar trailChar{trailing, {c.name, Settings::Style::List::background.s.bg, ts}};

	const auto style = [&](Color color) -> TBStyle
	{
		if (hovered) [[unlikely]]
			return {Settings::Style::List::background.s.bg, c.name, static_cast<std::uint32_t>(ts) | TextStyle::Bold};
		else [[likely]]
			return {color, Settings::Style::List::background.s.bg, ts};
	};

	const auto style2 = [&](TBStyle s) -> TBStyle
	{
		if (hovered) [[unlikely]]
			return {Settings::Style::List::background.s.bg, c.name, static_cast<std::uint32_t>(s.s) | TextStyle::Bold};
		else [[likely]]
			return {s.fg, Settings::Style::List::background.s.bg, s.s};
	};

	// Mark
	int p(0);
	int w2 = w;
	if (f.mark != MarkType::NONE) [[unlikely]]
	{
		const auto fn = [&](MarkType m, std::size_t i)
		{
			if (f.mark & m)
				p += Draw::TextLineBackground(Settings::Style::List::mark_prefix[i], style(c.name).bg, pos+Vec2i(p,0), w2-p, trailChar).first;

		};
		[&]<std::size_t... i>(std::index_sequence<i...>)
		{
			(( fn(MarkType::get<i+1>(), i) ), ...);
		}
		(std::make_index_sequence<MarkType::size-1>{});
	}

	// Icons
	if constexpr (Settings::Layout::List::icons)
	{
		const int p2 = Draw::TextLine(String(icon), style(c.icon), pos+Vec2i(p, 0), Settings::Layout::List::icons_width, trailChar).first;
		Draw::Horizontal({U' ', style(c.icon)}, pos+Vec2i(p+p2, 0), Settings::Layout::List::icons_width-p2);
		p += Settings::Layout::List::icons_width;

	}

	// Size
	if (m_mainList && f.mode != Mode::DIR && f.lnk.mode != Mode::DIR)
	{
		const TBString size = [&]()
		{
			const auto d = Util::GetDigitsNum<10>(f.sz);
			const std::size_t unit = std::clamp((d-1)/3, 0, static_cast<int>(Settings::Layout::List::units.size()));
			const std::size_t n1 = f.sz / std::pow(10, unit*3);
			const std::size_t n2 = (f.sz - n1*std::pow(10, unit*3)) / std::pow(10, unit*3-3+Util::GetDigitsNum<10>(n1));

			if (n2 == 0)
				return TBString{Util::ToString(n1) + U" ", style(c.size)} +
				TBString{String(Settings::Layout::List::units[unit]), style(c.sizeUnit)};
			else
				return TBString{Util::ToString(n1) + U"." + Util::ToString(n2) + U" ", style(c.size)} +
					TBString{String(Settings::Layout::List::units[unit]), style(c.sizeUnit)};
		}();
		w2 -= Draw::TextLine(size, pos+Vec2i((int)(w-size.SizeWide()), 0), w, trailChar).first;
	}

	// Link
	if constexpr (Settings::Layout::List::display_link)
	{
		//TODO
		if (f.mode == Mode::LNK) [[unlikely]]
		{
			// Spacing
			const auto cwidth = wcwidth(GetBackground().ch);
			if (w2 != w) // If filesize has been drawn
			{
				Draw::Char({GetBackground().ch, style2(GetBackground().s)}, pos+Vec2i(w2-cwidth, 0));
				w2 -= cwidth;
			}

			// Arrow
			if (f.lnk.link.size() != 0) [[likely]]
				w2 -= Draw::TextLine(Settings::Layout::List::link_arrow, style2(Settings::Style::List::arrow),
						pos+Vec2i(w2-Util::SizeWide(Settings::Layout::List::link_arrow), 0), w2, trailChar).first;
			else [[unlikely]]
				w2 -= Draw::TextLine(Settings::Layout::List::link_invalid_arrow, style2(Settings::Style::List::arrow_invalid),
						pos+Vec2i(w2-Util::SizeWide(Settings::Layout::List::link_invalid_arrow), 0), w2, trailChar).first;

			// Spacing
			Draw::Char({GetBackground().ch, style2(GetBackground().s)}, pos+Vec2i(w2-cwidth, 0));
			w2 -= cwidth;
		}
	}

	// File
	if (!match.matches.empty())
	{
		TBString tbs(f.name, style(c.name));
		for (const auto& m : match.matches)
		{
			const auto& [t, pos, len] = m;
			TBStyle s;
			switch (t)
			{
				case FileMatch::FILTER:
					s = Settings::Style::Filter::filter_match;
					break;
				default:
					s = style(c.name);
			}

			for (std::size_t i = 0; i < len; ++i)
			{
				tbs[pos+i].s.bg = s.bg;
				tbs[pos+i].s.s  = s.s;
			}
		}

		p += Draw::TextLine(tbs, pos+Vec2i(p,0), w2-p, trailChar).first;
	}
	else
		p += Draw::TextLine(f.name, style(c.name), pos+Vec2i(p,0), w2-p, trailChar).first;
	if (ts & TextStyle::Underline)
		ts = static_cast<int>(ts & (~TextStyle::Underline));

	// Fill left empty space
	Draw::Horizontal({GetBackground().ch, style(c.name)}, pos+Vec2i(p, 0), w2-p);

	return {style(c.name), style(c.numbers)};
}

void List::MarkFn(std::size_t i, MarkType mark)
{
	File& f = (*m_dir)[i];

	if (f.mark & mark)
		f.mark &= ~(mark);
	else
		f.mark |= mark;
}

void List::ActionLeft()
{
	OnChangePosition.Notify<EventWhen::BEFORE>();
	if (m_dir->GetPath() == "/")
		return;

	m_main->Back();
	OnChangePosition.Notify<EventWhen::AFTER>();
}

void List::ActionRight()
{
	if (GetEntries() == 0)
		return;

	const File& f = m_dir->Get(GetPos()).first;
	
	if (f.mode == Mode::DIR || f.lnk.mode == Mode::DIR)
	{
		OnChangePosition.Notify<EventWhen::BEFORE>();
		m_main->Forward(f.name);
		OnChangePosition.Notify<EventWhen::AFTER>();
	}
	else
	{
		const auto [openType, opener] = Actions::GetOpener(f, m_dir->GetPath());
		if (openType == Actions::OpenType::Executable)
			Actions::Open(f, m_dir->GetPath(), opener);
	}
}


using namespace std::placeholders;
List::List(MainWindow* main, const std::string& path, bool input):
	ListSelect(std::bind(&List::DrawFn, this, _1, _2, _3, _4, _5), std::bind(&List::MarkFn, this, _1, _2)),
	m_main(main),
	m_mainList(input) // If input is true, then this is the main list
{
	SetBackground(Settings::Style::List::background);
	m_dir = new Directory(path);

	if (!input)
		return;

	// Input
	AddKeyboardInput(Settings::Keys::List::down, [this]()
	{
		Termbox& tb = Termbox::GetTermbox();
		if (tb.GetContext().repeat == 0) [[likely]]
			ActionDown();
		else [[unlikely]]
			ActionDownN(tb.GetContext().repeat);
	});
	AddKeyboardInput(Settings::Keys::List::down_page, [this](){ ActionDownN(Settings::Layout::List::page_size); });
	AddMouseInput(Mouse({Vec2i(0, 0), Vec2i(0, 0)}, Mouse::MOUSE_WHEEL_DOWN,
				[this](const Vec2i&){ ActionDownN(1); }));
	AddKeyboardInput(Settings::Keys::List::up, [this]()
	{
		Termbox& tb = Termbox::GetTermbox();
		if (tb.GetContext().repeat == 0) [[likely]]
			ActionUp();
		else [[unlikely]]
			ActionUpN(tb.GetContext().repeat);
	});
	AddKeyboardInput(Settings::Keys::List::up_page, [this](){ ActionUpN(Settings::Layout::List::page_size); });
	AddMouseInput(Mouse({Vec2i(0, 0), Vec2i(0, 0)}, Mouse::MOUSE_WHEEL_UP,
				[this](const Vec2i&){ ActionUpN(1); }));

	AddKeyboardInput(Settings::Keys::Go::top,  [this](){ ActionSetPosition(0); });
	AddKeyboardInput(Settings::Keys::Go::bottom,  [this]()
	{
		Termbox& tb = Termbox::GetTermbox();
		if (tb.GetContext().hasRepeat)
			ActionSetPosition(tb.GetContext().repeat);
		else
			ActionSetPosition(GetEntries()-1);
	});

	AddKeyboardInput(Settings::Keys::List::left,  [this]() { ActionLeft(); });
	AddKeyboardInput(Settings::Keys::List::right, [this]() { ActionRight(); });

	AddMouseInput({{Vec2i(0, 0), Vec2i(0, 0)}, Mouse::MOUSE_LEFT, [this](const Vec2i& pos){ ActionMouseClick(pos); }});

	// Other
	AddKeyboardInput(Settings::Keys::Go::home, [this]() { m_main->CD("~"); });
	AddKeyboardInput(Settings::Keys::Go::root, [this]() { m_main->CD("/"); });
}

List::~List()
{
	delete m_dir;
}

void List::UpdateFiles()
{
	auto [error, flag] = m_dir->GetFiles();
	if (error)
	{
		switch (flag)
		{
			case EACCES:
				m_main->Error(U"Error: No permission");
				break;
			case EFAULT:
				m_main->Error(U"Error: Path points outside your accessible address space");
				break;
			case EIO:
				m_main->Error(U"Error: An I/O error occurred.");
				break;
			case ELOOP:
				m_main->Error(U"Error: Too many symbolic links were encountered in resolving path.");
				break;
			case ENAMETOOLONG:
				m_main->Error(U"Error: Path is too long.");
				break;
			case ENOENT:
				m_main->Error(U"Error: The directory specified in path does not exist.");
				break;
			case ENOMEM:
				m_main->Error(U"Error: Insufficient kernel memory was available.");
				break;
			case ENOTDIR:
				m_main->Error(U"Error: A component of path is not a directory.");
				break;
			case EMFILE:
				m_main->Error(U"Error: The per-process limit on the number of open file de‐ scriptors has been reached.");
				break;
			case ENFILE:
				m_main->Error(U"Error: The  system-wide  limit  on the total number of open files has been reached.");
				break;
			default:
				m_main->Error(U"Error: Cannot access directory");
				break;
		}
	}

	UpdateFilter();
}

void List::UpdateFilter()
{
	m_dir->Filter();
	m_dir->Sort();
	SetEntries(m_dir->SizeD());
	ActionSetPosition(0);
}

void List::SetShowHidden(bool v)
{
	auto f = m_dir->GetFilter();
	f.HiddenFiles = v;
	m_dir->SetFilter(std::move(f));
}

bool List::GetShowHidden() const
{
	return m_dir->GetFilter().HiddenFiles;
}

Directory* List::GetDir()
{
	return m_dir;
}

void List::SetDir(Directory* dir)
{
	m_dir = dir;
	SetEntries(m_dir->SizeD());
}
