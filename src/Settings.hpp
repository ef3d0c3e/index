#ifndef INDEX_SETTINGS_HPP
#define INDEX_SETTINGS_HPP

#include "Sort.hpp"
#include "TermboxWidgets/Widgets.hpp"
#include "Tabline.hpp"

namespace Settings
{
	// Locale used for index
	constexpr char locale[] = "en_US.UTF-8";

	constexpr std::size_t directory_block_size = 1<<9;

	constexpr bool directory_resolve_lnk = true;

	namespace Layout
	{
		constexpr Char trailing_character = U'…';

		// MainWindow, {parent, directory}
		constexpr int mainwindow_ratio[] = {15, 84};
		constexpr int mainwindow_spacing[] = {1, 1};

		// * List
		namespace List
		{
			constexpr bool icons = true;
			constexpr int icons_width = 2;
			constexpr bool display_link = true;
			constexpr Char link_arrow[] = U"->";
			constexpr Char link_invalid_arrow[] = U"~<";

			constexpr Widgets::ListSelectSettings settings{
				.ScrollTriggerUp = 20,
					.ScrollTriggerDown = 20,
					.LeftMargin = 1,
					.NumberSpacing = 1,
					.RightMargin = 1,
					.NumberBase = 10,
					.DrawNumbers = true,
					.RelativeNumbers = true,
					.NumberRightAlign = true,
					.TrailingChar = U'~',
					.Cycling = true,
			};

			constexpr std::array<StringView, 8> units =
			{
				U"B", U"K", U"M", U"G", U"T", U"P", U"E"
			};
		}

		namespace Tabline
		{
			constexpr int left_margin = 1;
			constexpr int right_margin = 1;
			constexpr Char separator[] = U"@";
			constexpr Char directory_separator[] = U"/";
			constexpr auto loading_spinner = Util::make_array(U"⠁", U"⠂", U"⠄", U"⡀", U"⢀", U"⠠", U"⠐", U"⠈");
		}

		namespace Statusline
		{
			constexpr int left_margin = 1;
			constexpr int right_margin = 1;
			constexpr Char unknown_owner[] = U"???";
			constexpr Char unknown_group[] = U"???";
			constexpr std::size_t date_max_size = 32;
			constexpr char date_format[] = "%Y-%m-%d %H:%M:%S";
			constexpr Char unknown_date[] = U"???";
			constexpr bool display_link = true;
			constexpr Char link_arrow[] = U"->";
			constexpr Char link_invalid_arrow[] = U"-<";
			constexpr Char link_invalid_text[] = U"Invalid link";
		}

		// * Posix
		constexpr Char unknown_hostname[] = U"???";
		constexpr Char unknown_username[] = U"???";

	}

	namespace Keyboard
	{
		constexpr Char quit[] = U"q";
	}

	namespace Style
	{
		constexpr TBStyle default_text_style{0xFFFFFF, 0x000000, TextStyle::None};

		constexpr TBChar main_window_background{U' ', 0xFFFFFF, COLOR_DEFAULT, TextStyle::None};

		namespace List
		{
			constexpr TBChar background{U' ', 0xFFFFFF, COLOR_DEFAULT};
			constexpr TBStyle arrow{0x40C080, background.s.bg, TextStyle::Bold};
			constexpr TBStyle arrow_invalid{0xF0A0A0, background.s.bg, TextStyle::Italic};
			constexpr TBStyle link{0x40C080, background.s.bg, TextStyle::Italic};
		}
		
		namespace Tabline
		{
			constexpr TBChar background{U' ', 0xFFFFFF, 0x202030, TextStyle::None};
			constexpr TBStyle hostname{0x8F40F0, background.s.bg, TextStyle::Bold};
			constexpr TBStyle separator{0xFF40F0, background.s.bg, TextStyle::Bold};
			constexpr TBStyle username{0x2F60F0, background.s.bg, TextStyle::Bold};
			constexpr TBStyle directory{0xC0C0C0, background.s.bg, TextStyle::None};
			constexpr TBStyle directory_separator{0xF0A0A0, background.s.bg, TextStyle::Bold};
			constexpr TBStyle selected{0xAFFFFF, background.s.bg, TextStyle::Bold};
			constexpr TBStyle repeat{0xF0C030, 0x201040, TextStyle::Bold};

			constexpr TBStyle loading{0xFF0000, background.s.bg, TextStyle::None};
		}

		namespace Statusline
		{
			constexpr TBChar background{U' ', 0xFFFFFF, 0x303030, TextStyle::None};
			constexpr std::array<std::pair<StringView, TBStyle>, 12> filemode = {
				std::make_pair(U"?", TBStyle{0x90D090, background.s.bg, TextStyle::Bold}), // UNK
				std::make_pair(U"?", TBStyle{0x90D090, background.s.bg, TextStyle::Bold}), // NONE
				std::make_pair(U"d", TBStyle{0x90D090, background.s.bg, TextStyle::Bold}), // DIR
				std::make_pair(U"c", TBStyle{0x90D090, background.s.bg, TextStyle::Bold}), // CHR
				std::make_pair(U"b", TBStyle{0x90D090, background.s.bg, TextStyle::Bold}), // BLK
				std::make_pair(U"-", TBStyle{0x5E5E5E, background.s.bg, TextStyle::Bold}), // REG
				std::make_pair(U"f", TBStyle{0x90D090, background.s.bg, TextStyle::Bold}), // FIFO
				std::make_pair(U"l", TBStyle{0x90D090, background.s.bg, TextStyle::Bold}), // LNK
				std::make_pair(U"s", TBStyle{0x90D090, background.s.bg, TextStyle::Bold}), // SOCK
			};
			constexpr std::array<std::pair<StringView, TBStyle>, Permission::size> permissions = {
				std::make_pair(U"-", TBStyle{0x5E5E5E, background.s.bg, TextStyle::Bold}),
				// User
				std::make_pair(U"r", TBStyle{0x81A85B, background.s.bg, TextStyle::None}),
				std::make_pair(U"w", TBStyle{0xF0A57F, background.s.bg, TextStyle::None}),
				std::make_pair(U"x", TBStyle{0x9C5555, background.s.bg, TextStyle::None}),
				// Group
				std::make_pair(U"r", TBStyle{0x81A85B, background.s.bg, TextStyle::None}),
				std::make_pair(U"w", TBStyle{0xF0A57F, background.s.bg, TextStyle::None}),
				std::make_pair(U"x", TBStyle{0x9C5555, background.s.bg, TextStyle::None}),
				// Other
				std::make_pair(U"r", TBStyle{0x81A85B, background.s.bg, TextStyle::None}),
				std::make_pair(U"w", TBStyle{0xF0A57F, background.s.bg, TextStyle::None}),
				std::make_pair(U"x", TBStyle{0x9C5555, background.s.bg, TextStyle::None}),
			};

			constexpr TBStyle owner{0x9080F0, background.s.bg, TextStyle::Bold};
			constexpr TBStyle group{0x9080F0, background.s.bg, TextStyle::Bold};
			constexpr TBStyle date{0xF08010, background.s.bg, TextStyle::Underline};

			constexpr TBStyle link_arrow{0x90F0F0, background.s.bg, TextStyle::Bold};
			constexpr TBStyle link_invalid_arrow{0xFFA0A0, background.s.bg, TextStyle::Bold};
			constexpr TBStyle link{0x90F0F0, background.s.bg, TextStyle::None};
			constexpr TBStyle link_invalid{0xFFA0A0, background.s.bg, TextStyle::Italic};

			constexpr TBStyle error{0xF05040, background.s.bg, TextStyle::Bold};
		}

		namespace Menu
		{
			constexpr TBStyle background{0xFFFFFF, COLOR_DEFAULT, TextStyle::None};
		}
	}

	namespace Keys
	{
		namespace Go
		{
			const Char top[] = U"g g";
			const Char bottom[] = U"S-G"; // Will also work for <repeat> G
			const Char home[] = U"g h";
			const Char root[] = U"g /";
		}
	}
}


constexpr auto SortFns = Util::make_array
(
	std::make_pair(Sort::Basename, U"basename")
);

#endif // INDEX_SETTINGS_HPP
