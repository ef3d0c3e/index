#include "Statusline.hpp"
#include "Settings.hpp"
#include "List.hpp"
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

void Statusline::Draw()
{
	if (m_error)
	{
		m_error = false;
		return;
	}

	//* Left side
	
	// Margin
	auto x = GetPosition()[0];
	const auto y = GetPosition()[1];
	const auto w = GetSize()[0]+x;
	const auto bgWidth = wcwidth(m_bg.ch);
	if constexpr (Settings::Layout::Statusline::left_margin != 0)
	{
		Draw::Horizontal(m_bg, Vec2i(x, y), Settings::Layout::Statusline::left_margin*bgWidth);
		x += Settings::Layout::Statusline::left_margin*bgWidth;
	}
	if (x >= w)
		return;

	if (m_main->GetList()->GetPos() < m_main->GetList()->GetEntries()) [[likely]]
	{
		const File& f = m_main->GetList()->Get(m_main->GetList()->GetPos()).first;

		// Filemode
		{
			const int id = [&](){
				auto mode = f.mode;
				int n = 0;
				while (mode)
				{
					mode = mode >> 1;

					++n;
				}

				return n;
			}();
			x += Draw::TextLine(String(Settings::Style::Statusline::filemode[id].first), Settings::Style::Statusline::filemode[id].second, Vec2i(x, y), w-x,
					{Settings::Layout::trailing_character, Settings::Style::Statusline::filemode[id].second}, 0).first;
			if (x >= w)
				return;
		}

		// Permissions
		for (std::size_t i = 0; i < Permission::size-1; ++i)
		{
			int id;
			if (f.perm & (1 << i))
				id = i+1;
			else 
				id = 0;
			x += Draw::TextLine(String(Settings::Style::Statusline::permissions[id].first), Settings::Style::Statusline::permissions[id].second, Vec2i(x, y), w-x,
					{Settings::Layout::trailing_character, Settings::Style::Statusline::permissions[id].second}, 0).first;
			if (x >= w)
				return;
		}

		// Spacing
		{
			Draw::Char(m_bg, Vec2i(x, y));
			x += bgWidth;
			if (x >= w)
				return;
		}

		// Owner
		{
			struct passwd* owner = getpwuid(f.owner);

			if (owner != NULL)
				x += Draw::TextLine(Util::StringConvert<Char>(std::string(owner->pw_name)), Settings::Style::Statusline::owner, Vec2i(x, y), w-x,
						{Settings::Layout::trailing_character, Settings::Style::Statusline::owner}, 0).first;
			else
				x += Draw::TextLine(Settings::Layout::Statusline::unknown_owner, Settings::Style::Statusline::owner, Vec2i(x, y), w-x,
						{Settings::Layout::trailing_character, Settings::Style::Statusline::owner}, 0).first;
			
		}

		// Spacing
		{
			Draw::Char(m_bg, Vec2i(x, y));
			x += bgWidth;
			if (x >= w)
				return;
		}
		
		// Group
		{
			struct group* group = getgrgid(f.group);

			if (group != NULL)
				x += Draw::TextLine(Util::StringConvert<Char>(std::string(group->gr_name)), Settings::Style::Statusline::owner, Vec2i(x, y), w-x,
						{Settings::Layout::trailing_character, Settings::Style::Statusline::owner}, 0).first;
			else
				x += Draw::TextLine(Settings::Layout::Statusline::unknown_group, Settings::Style::Statusline::group, Vec2i(x, y), w-x,
						{Settings::Layout::trailing_character, Settings::Style::Statusline::group}, 0).first;
			if (x >= w)
				return;
		}

		// Spacing
		{
			Draw::Char(m_bg, Vec2i(x, y));
			x += bgWidth;
			if (x >= w)
				return;
		}

		// Date
		{
			char buff[Settings::Layout::Statusline::date_max_size];
			std::size_t ret = std::strftime(buff, Settings::Layout::Statusline::date_max_size-1, Settings::Layout::Statusline::date_format, localtime(&f.lastModification));
			if (ret != 0)
				x += Draw::TextLine(Util::StringConvert<Char>(std::string(buff)), Settings::Style::Statusline::date, Vec2i(x, y), w-x,
						{Settings::Layout::trailing_character, Settings::Style::Statusline::date}, 0).first;
			else
				x += Draw::TextLine(Settings::Layout::Statusline::unknown_date, Settings::Style::Statusline::date, Vec2i(x, y), w-x,
						{Settings::Layout::trailing_character, Settings::Style::Statusline::date}, 0).first;
			if (x >= w)
				return;
		}

		// Spacing
		{
			Draw::Char(m_bg, Vec2i(x, y));
			x += bgWidth;
			if (x >= w)
				return;
		}

		// Link
		if constexpr (Settings::Layout::Statusline::display_link)
		{
			if (f.mode == Mode::LNK)
			{
				// Arrow
				if (f.lnk.link.size() != 0)
					x += Draw::TextLine(Settings::Layout::Statusline::link_arrow, Settings::Style::Statusline::link_arrow,
							Vec2i(x, y), w-x, {Settings::Layout::trailing_character, Settings::Style::Statusline::link_arrow}).first;
				else
					x += Draw::TextLine(Settings::Layout::Statusline::link_invalid_arrow, Settings::Style::Statusline::link_invalid_arrow,
							Vec2i(x, y), w-x, {Settings::Layout::trailing_character, Settings::Style::Statusline::link_arrow}).first;

				// Spacing
				Draw::Char(m_bg, Vec2i(x, y));
				x += bgWidth;

				// Link
				if (f.lnk.link.size() != 0)
					x += Draw::TextLine(Util::StringConvert<Char>(f.lnk.link), Settings::Style::Statusline::link, Vec2i(x, y), w-x,
							{Settings::Layout::trailing_character, Settings::Style::Statusline::link}, 0).first;
				else
					x += Draw::TextLine(Settings::Layout::Statusline::link_invalid_text, Settings::Style::Statusline::link_invalid,
							Vec2i(x, y), w-x, {Settings::Layout::trailing_character, Settings::Style::Statusline::link_invalid}, 0).first;
				if (x >= w)
					return;
			}
		}
	}

	// * Right side
	auto w2 = w;
	
	// Margin
	if constexpr (Settings::Layout::Statusline::right_margin != 0)
	{
		Draw::Horizontal(m_bg, Vec2i(w2-Settings::Layout::Statusline::right_margin*bgWidth, y), Settings::Layout::Statusline::right_margin*bgWidth);
		w2 -= Settings::Layout::Statusline::right_margin*bgWidth;
	}

	// Mode
	if constexpr (Settings::Layout::Statusline::display_mode)
	{
		const static std::array<int, Settings::Style::Statusline::modes.size()> modesLength = [] // Precomputed table
		{
			std::array<int, Settings::Style::Statusline::modes.size()> len;
			[&]<std::size_t... i>(std::index_sequence<i...>)
			{
				((len[i] = Settings::Style::Statusline::modes[i].SizeWide()), ...);
			}(std::make_index_sequence<Settings::Style::Statusline::modes.size()>{});

			return len;
		}();

		w2 -= Draw::TextLine(Settings::Style::Statusline::modes[m_main->GetMode()], Vec2i(w2-modesLength[m_main->GetMode()], y), modesLength[m_main->GetMode()], {Settings::Layout::trailing_character, Settings::Style::Statusline::link_invalid}).first;
	}
	
	// Fill
	{
		Draw::Horizontal(m_bg, Vec2i(x, y), w2-x);
	}
}

void Statusline::DrawError(const String& msg)
{
	m_error = true;

	auto x = GetPosition()[0];
	// Spacing
	{
		Draw::Char({m_bg.ch, Settings::Style::Statusline::error}, Vec2i(x, GetPosition()[1]));
		x += wcwidth(m_bg.ch);
	}

	// Text
	{
		x += Draw::TextLine(msg, Settings::Style::Statusline::error, Vec2i(x, GetPosition()[1]), GetSize()[0],
				{Settings::Layout::trailing_character, Settings::Style::Statusline::error}).first;
		Draw::Horizontal({m_bg.ch, Settings::Style::Statusline::error}, GetPosition()+Vec2i(x, 0), GetSize()[0]-x);
	}
}

Statusline::Statusline(MainWindow* main):
	m_main(main),
	m_bg(Settings::Style::Statusline::background),
	m_error(false)
{
}

Statusline::~Statusline()
{
	
}

void Statusline::SetBackground(const TBChar& bg)
{
	m_bg = bg;
}

const TBChar& Statusline::GetBackground() const
{
	return m_bg;
}

