#include "List.hpp"
#include "Actions/Actions.hpp"
#include "DirectoryCache.hpp"
#include <algorithm>

std::pair<TBStyle, TBStyle> List::DrawFn(std::size_t i, Vec2i pos, int w, bool hovered, Char trailing) const
{
	const auto& [f, match] = Get(i);
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
		// We use .size instead of SizeWide, because most icons are not considered wide characters by the standard
		const auto iconWidth = std::max(Settings::Layout::List::icons_min_width, icon.size());
		const int p2 = Draw::TextLine(String(icon), style(c.icon), pos+Vec2i(p, 0), iconWidth, trailChar).first;
		Draw::Horizontal({U' ', style(c.icon)}, pos+Vec2i(p+p2, 0), iconWidth-p2);
		p += iconWidth;

	}

	// Size
	if (m_mainList && f.mode != Mode::DIR && f.lnk.mode != Mode::DIR)
	{
		const TBString size = [&]()
		{
			const auto d = Util::GetDigitsNum<10>(f.sz);
			const std::size_t unit = std::clamp((d-1)/3, 0, static_cast<int>(Settings::Layout::List::units.size())-1);
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
				case FileMatch::SEARCH:
					s = Settings::Style::Search::search_match;
					break;
				default:
					s = style(c.name);
			}
			s.s |= style(c.name).s;
			s.fg = style(c.name).fg;
			if (hovered) [[unlikely]]
			{
				s.fg = s.bg;
				s.bg = style(c.name).bg;
			}

			for (std::size_t i = 0; i < len; ++i)
			{
				tbs[pos+i].s.fg  = s.fg;
				tbs[pos+i].s.bg = s.bg;
				tbs[pos+i].s.s  = s.s;
			}
		}

		p += Draw::TextLine(tbs, pos+Vec2i(p,0), w2-p, {trailing, style(c.name)}).first;
	}
	else
		p += Draw::TextLine(f.name, style(c.name), pos+Vec2i(p,0), w2-p, {trailing, style(c.name)}).first;
	if (ts & TextStyle::Underline)
		ts = static_cast<int>(ts & (~TextStyle::Underline));

	// Fill left empty space
	Draw::Horizontal({GetBackground().ch, style(c.name)}, pos+Vec2i(p, 0), w2-p);

	return {style(c.name), style(c.numbers)};
}

void List::MarkFn(std::size_t i, MarkType mark)
{
	File& f = Get(i).first;

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

	const File& f = Get(GetPos()).first;
	
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
			Actions::Open(f.name, m_dir->GetPath(), *opener);
		else
		{
			m_main->ActionPrompt([&](const String& input)
			{
				try
				{
					Actions::CustomOpen(f.name, m_dir->GetPath(), Util::StringConvert<char>(input));
				}
				catch (IndexError& e)
				{
					m_main->Error(U"Error: " + e.GetMessage());
				}
			}, Settings::Style::List::open_prompt_prefix, Settings::Style::List::open_prompt_background, Settings::Style::List::open_prompt_max_length, U" {}", 0);
		}
	}
}

std::size_t List::SearchNext() const
{
	if (m_files.empty() || m_search == U"") [[unlikely]]
		return std::size_t(-1);

	// Forward
	for (std::size_t forward = GetPos()+1; forward < GetEntries(); ++forward)
	{
		for (const auto& match : m_files[forward].second.matches)
		{
			if (std::get<0>(match) == FileMatch::SEARCH)
				return forward;
		}
	}

	// Backward
	for (std::size_t backward = 0; backward < GetPos(); ++backward)
	{
		for (const auto& match : m_files[backward].second.matches)
		{
			if (std::get<0>(match) == FileMatch::SEARCH)
				return backward;
		}
	}

	return static_cast<std::size_t>(-1);
}

std::size_t List::SearchPrev() const
{
	if (m_files.empty() || m_search == U"") [[unlikely]]
		return std::size_t(-1);

	// Backward
	if (GetPos() != 0) [[likely]]
		for (std::size_t backward = GetPos()-1; backward != std::size_t(-1); --backward)
		{
			for (const auto& match : m_files[backward].second.matches)
			{
				if (std::get<0>(match) == FileMatch::SEARCH)
					return backward;
			}
		}

	// Forward
	for (std::size_t forward = GetEntries()-1; forward > GetPos(); --forward)
	{
		for (const auto& match : m_files[forward].second.matches)
		{
			if (std::get<0>(match) == FileMatch::SEARCH)
				return forward;
		}
	}

	return static_cast<std::size_t>(-1);
}

bool List::ApplySearch()
{
	if (m_files.empty()) [[unlikely]]
		return false;

	// Clear previous search
	for (auto& f : m_files)
	{
		f.second.matches.erase(std::remove_if(f.second.matches.begin(), f.second.matches.end(), []<class T>(const T& tup)
		{
			return std::get<0>(tup) == FileMatch::SEARCH;
		}),  f.second.matches.end());
	}

	if (m_search == U"") // Cancel search
		return true; // All matched technically...

	try
	{
		bool matched = false;
		const std::wregex re(Util::StringConvert<wchar_t>(m_search), Settings::Search::regex_mode);

		for (auto& f : m_files)
		{
			const auto s = Util::StringConvert<wchar_t>(f.first->name);
			if(std::wsmatch m; std::regex_search(s, m, re, Settings::Search::search_mode))
			{
				matched = true;
				FileMatch& match = f.second;
				for (std::size_t i = 0; i < m.size(); ++i)
				{
					match.matches.push_back({FileMatch::SEARCH, m.position(i), m.length(i)});
				}
			}
		}

		return matched;
	}
	catch (std::regex_error& e)
	{
		throw IndexError(U"Error: Invalid regex", IndexError::REGEX_ERROR);
	}

	return false;
}

using namespace std::placeholders;
List::List(MainWindow* main, const std::string& path, bool input):
	ListBase(std::bind(&List::DrawFn, this, _1, _2, _3, _4, _5), std::bind(&List::MarkFn, this, _1, _2)),
	m_main(main),
	m_search(U""),
	m_mainList(input) // If input is true, then this is the main list
{
	SetBackground(Settings::Style::List::background);
	try
	{
		m_dir = gDirectoryCache.GetDirectory(path).first;
	}
	catch (IndexError& e)
	{
		// TODO
	}

	if (!input)
		return;

	// {{{ Navigation
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
		ActionDownN(Settings::Layout::List::page_size);
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
		ActionUpN(Settings::Layout::List::page_size);
	});

	AddMouseInput(Mouse({Vec2i(0, 0), Vec2i(0, 0)}, Mouse::MOUSE_WHEEL_UP,
				[this](const Vec2i&){ ActionUpN(1); }));

	AddKeyboardInput(Settings::Keys::Go::top,  [this]
	{
		ActionSetPosition(0);
	});

	AddKeyboardInput(Settings::Keys::Go::bottom,  [this]
	{
		Termbox& tb = Termbox::GetTermbox();
		if (tb.GetContext().hasRepeat)
			ActionSetPosition(tb.GetContext().repeat);
		else
			ActionSetPosition(GetEntries()-1);
	});

	AddKeyboardInput(Settings::Keys::List::left,  [this]
	{
		ActionLeft();
	});

	AddKeyboardInput(Settings::Keys::List::right, [this]
	{
		ActionRight();
	});

	AddMouseInput({{Vec2i(0, 0), Vec2i(0, 0)}, Mouse::MOUSE_LEFT, [this](const Vec2i& pos){ ActionMouseClick(pos); }});
	// }}}

	// {{{ Filter
	AddKeyboardInput(Settings::Keys::filter, [&]
	{
		m_main->ActionPrompt([&](const String& input)
		{
			const String name = GetCurrentFileName();

			auto filter = GetFilter();
			filter.Match = input;
			SetFilter(filter);
			UpdateFromDir();

			const auto pos = FindByName(name);
			if (pos == static_cast<std::size_t>(-1)) [[unlikely]]
				return;
			ActionSetPosition(pos);
		}, Settings::Style::Filter::filter_prompt_prefix, Settings::Style::Filter::filter_prompt_bg, Settings::Style::Filter::filter_prompt_max_length, m_filter.Match, m_filter.Match.size());
	});
	// }}}

	// {{{ Search
	AddKeyboardInput(Settings::Keys::Search::search, [&]
	{
		m_main->ActionPrompt([&](const String& input)
		{
			m_search = input;
			try
			{
				if (!ApplySearch())
				{
					m_main->Message(Settings::Style::Search::search_not_found, Settings::Style::Search::search_not_found_duration);
					return;
				}
			}
			catch (IndexError& e)
			{
				m_main->Error(e.GetMessage());
				m_search = U"";
				return;
			}

			const auto pos = SearchNext();
			if (pos == static_cast<std::size_t>(-1)) [[unlikely]]
				return;

			ActionSetPosition(pos);

		}, Settings::Style::Search::search_prompt_prefix, Settings::Style::Search::search_prompt_background, Settings::Style::Search::search_prompt_max_length, U"", 0);

	});

	AddKeyboardInput(Settings::Keys::Search::next, [&]
	{
		const auto pos = SearchNext();
		if (pos == static_cast<std::size_t>(-1)) [[unlikely]]
			return;

		ActionSetPosition(pos);
	});

	AddKeyboardInput(Settings::Keys::Search::prev, [&]
	{
		const auto pos = SearchPrev();
		if (pos == static_cast<std::size_t>(-1)) [[unlikely]]
			return;

		ActionSetPosition(pos);
	});
	// }}}

	// Other
	AddKeyboardInput(Settings::Keys::Go::home, [this]
	{
		m_main->CD("~");
	});
	AddKeyboardInput(Settings::Keys::Go::root, [this]
	{
		m_main->CD("/");
	});
}

List::~List()
{
}

void List::MoveFiles(List&& l, bool preservePos)
{
	const auto pos = l.GetPos();

	m_dir = l.m_dir;
	m_files = std::move(l.m_files);

	SetEntries(m_files.size());

	if (preservePos)
		ActionSetPosition(pos);
}

/*
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
*/

void List::SetSettings(const List::DirectorySettings& settings)
{
	m_settings = settings;
}

const List::DirectorySettings& List::GetSettings() const
{
	return m_settings;
}

void List::SetFilter(const List::DirectoryFilter& filter)
{
	m_filter = filter;
}

const List::DirectoryFilter& List::GetFilter() const
{
	return m_filter;
}

std::size_t List::Size() const
{
	return m_files.size();
}

void List::UpdateFromDir(bool preservePos)
{
	File current;
	if (preservePos && !m_files.empty())
		current = *m_files[GetPos()].first; // copy

	[&] // Filter
	{
		m_files.clear();
		m_files.reserve(m_dir->Size());
		if (!m_filter.Match.empty())
		{
			try
			{
				const std::wregex re(Util::StringConvert<wchar_t>(m_filter.Match), Settings::Filter::regex_mode);

				for (std::size_t i = 0; i < m_dir->Size(); ++i)
				{
					File& f = (*m_dir)[i];
					if (!m_filter.HiddenFiles && f.name[0] == U'.')
						continue;

					const auto s = Util::StringConvert<wchar_t>(f.name);
					if(std::wsmatch m; std::regex_search(s, m, re, Settings::Filter::search_mode))
					{
						FileMatch match;
						for (std::size_t j = 0; j < m.size(); ++j)
						{
							match.matches.push_back({FileMatch::FILTER, m.position(j), m.length(j)});
						}

						m_files.push_back({&f, std::move(match)});
					}
				}

				return;
			}
			catch (std::regex_error& e)
			{
				m_main->Error(U"Error: Invalid regex");
			}
		}

		// If regex is invalid, we use the default list
		for (std::size_t i = 0; i < m_dir->Size(); ++i)
		{
			File& f = (*m_dir)[i];
			if (!m_filter.HiddenFiles && f.name[0] == U'.')
				continue;

			m_files.push_back({&f, FileMatch{}});
		}
	}();

	// Search
	ApplySearch();

	// Sort
	Sort(false);

	SetEntries(Size());

	if (preservePos && !m_files.empty())
	{
		const auto pos = Find(current.name, current.mode);
		if (pos == static_cast<std::size_t>(-1))
			ActionSetPosition(0);
		else
			ActionSetPosition(pos);
	}
}

void List::Sort(bool preservePos)
{
	File current;
	if (preservePos && !m_files.empty())
		current = *m_files[GetPos()].first; // copy

	// Sort
	using namespace std::placeholders;
	if (m_settings.SortSettings.Reverse) [[unlikely]]
		std::sort(m_files.begin(), m_files.end(), [this]<class T>(const T& a, const T& b) -> bool
		{
			return !SortFns[m_settings.SortSettings.SortFn](a, b, m_settings.SortSettings);
		});
	else [[likely]]
		std::sort(m_files.begin(), m_files.end(), std::bind(SortFns[m_settings.SortSettings.SortFn], _1, _2, m_settings.SortSettings));

	if (preservePos && !m_files.empty())
	{
		const auto pos = Find(current.name, current.mode);
		if (pos == static_cast<std::size_t>(-1))
			ActionSetPosition(0);
		else
			ActionSetPosition(pos);
	}
}

void List::UpdateFromFilesystem(bool preservePos)
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

	UpdateFromDir(preservePos);
}

Directory* List::GetDir()
{
	return m_dir;
}

const Directory* List::GetDir() const
{
	return m_dir;
}

void List::SetDir(Directory* dir)
{
	m_dir = dir;
}

const std::pair<const File&, const FileMatch&> List::Get(std::size_t i) const
{
	return {*m_files[i].first, m_files[i].second};
}

std::pair<File&, FileMatch&> List::Get(std::size_t i)
{
	return {*m_files[i].first, m_files[i].second};
}

std::size_t List::Find(const String& name, Mode mode, std::size_t beg) const
{
	for (std::size_t i = beg; i < m_files.size(); ++i)
	{
		const File& f = *m_files[i].first;

		if (f.mode != mode)
			continue;
		if (f.name != name)
			continue;

		return i;
	}

	return static_cast<std::size_t>(-1);
}

std::size_t List::FindByName(const String& name) const
{
	for (std::size_t i = 0; i < m_files.size(); ++i)
	{
		const File& f = *m_files[i].first;

		if (f.name != name)
			continue;

		return i;
	}

	return static_cast<std::size_t>(-1);
}

String List::GetCurrentFileName() const
{
	if (Size() == 0)
		return U"";

	return Get(GetPos()).first.name;
}
