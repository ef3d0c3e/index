#ifndef INDEX_SETTINGS_HPP
#define INDEX_SETTINGS_HPP

#include "Sort.hpp"
#include "TermboxWidgets/Widgets.hpp"
#include <fstream>
#include <regex>
#include <fmt/format.h>

namespace Settings
{
	// Locale used for index
	constexpr char locale[] = "en_US.UTF-8";

	constexpr std::size_t directory_block_size = 1<<9;

	constexpr bool directory_resolve_lnk = true;


	// Shell variable that gets incremented for each nested OpenShell
	// Leave empty to disable this feature
	constexpr char shell_variable[] = "RANGER_LEVEL"; 
	

	namespace Layout
	{
		constexpr Char trailing_character = U'…';

		// MainWindow, {parent, directory}
		constexpr auto mainwindow_ratio = Util::make_array(15, 85);
		constexpr auto mainwindow_spacing = Util::make_array(1, 1);

		namespace List
		{
			constexpr bool icons = true;
			constexpr std::size_t icons_min_width = 2;
			constexpr bool display_link = true;
			constexpr Char link_arrow[] = U"➜ "; // They are not treated as wide character by wcwidth
			constexpr Char link_invalid_arrow[] = U"➜ ";
			constexpr int page_size = 10;

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
			constexpr std::string_view date_format = "{:%Y-%m-%d %H:%M:%S}";
			constexpr bool display_link = true;
			constexpr Char link_arrow[] = U"➜ ";
			constexpr Char link_invalid_arrow[] = U"➜ ";
			constexpr Char link_invalid_text[] = U"Could not resolve link";
			constexpr bool display_mode = true; ///< Displays the current mode in the status line
			constexpr bool display_clipboard_mode = true; ///< Displays the current clipboard mode & number in the status line
		}

		namespace Marks
		{
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
		}

		namespace Cache
		{
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

			constexpr StringView date_format = U"({:%H:%M:%S})";
			constexpr Char ref_count_format[] = U"[{0}]";
		}

		namespace Position
		{
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

			constexpr StringView date_format = U"({:%H:%M:%S})";
		}

		namespace Clipboard
		{
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
		}

		namespace JobManager
		{
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

			constexpr std::size_t max_finished = 1<<8; ///< Maximum number of finished jobs kept in the list
		}

		// * Posix
		constexpr Char unknown_hostname[] = U"???";
		constexpr Char unknown_username[] = U"???";

		// * Error
		constexpr StringView error_rename_from_to(U"'{}' -> '{}'");

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

			static const std::array<TBString, MarkType::size-1> mark_prefix = Util::make_array // Background will be ignored
			(
				TBString{U"[s]", {0x707070, main_window_background.s.bg, TextStyle::Bold}}, // SELECTED
				TBString{U"*", {0xC04040, main_window_background.s.bg, TextStyle::Bold}},   // TAGGED
				TBString{U"*", {0xF0D040, main_window_background.s.bg, TextStyle::Bold}},   // FAV

				TBString{U"☡ ", {0xF080F0, main_window_background.s.bg, TextStyle::Bold}},  // CLIP_CUT
				TBString{U"☡ ", {0x40F0B0, main_window_background.s.bg, TextStyle::Bold}}   // CLIP_YANK
			);

			// Open prompt
			const static TBString open_prompt_prefix{U"open: ", {0x00A0F0, main_window_background.s.bg, TextStyle::Bold}};
			constexpr TBChar open_prompt_background{U' ', {0xFFFFFF, main_window_background.s.bg, TextStyle::None}};
			constexpr std::size_t open_prompt_max_length = 1<<10;
		}
		
		namespace Tabline
		{
			constexpr TBChar background{U' ', 0xFFFFFF, COLOR_DEFAULT, TextStyle::None};
			constexpr TBStyle hostname{0x61A171, background.s.bg, TextStyle::Bold};
			constexpr TBStyle separator{0x61A171, background.s.bg, TextStyle::None};
			constexpr TBStyle username{0x61A171, background.s.bg, TextStyle::Bold};
			constexpr TBStyle directory{0x6BA4A4, background.s.bg, TextStyle::None};
			constexpr TBStyle directory_separator{0x6BA4A4, background.s.bg, TextStyle::Bold};
			constexpr TBStyle selected{0xFFFFFF, background.s.bg, TextStyle::Bold};
			constexpr TBStyle repeat{0xF0C030, 0x201040, TextStyle::Bold};
			constexpr TBStyle tab{0xAFAFAF, background.s.bg, TextStyle::Bold};
			constexpr TBStyle tab_current{0xFF8F9F, background.s.bg, TextStyle::Bold | TextStyle::Underline};

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

			constexpr TBStyle owner{0x9080F0, background.s.bg, TextStyle::None};
			const static TBString owner_group_sep{U":", {0x9080F0, background.s.bg, TextStyle::None}};
			constexpr TBStyle group{0x9080F0, background.s.bg, TextStyle::None};
			constexpr TBStyle date{0xF05040, background.s.bg, TextStyle::None};

			constexpr TBStyle link_arrow{0x90F0F0, background.s.bg, TextStyle::Bold};
			constexpr TBStyle link_invalid_arrow{0xFFA0A0, background.s.bg, TextStyle::Bold};
			constexpr TBStyle link{0x90F0F0, background.s.bg, TextStyle::None};
			constexpr TBStyle link_invalid{0xFFA0A0, background.s.bg, TextStyle::Italic};

			constexpr TBStyle error{0xF05040, background.s.bg, TextStyle::Bold};

			const static std::array<TBString, 6> modes =
			{
				// Regular modes
				TBString{U" LIST ",      {background.s.bg, 0xF07040, TextStyle::Bold}},
				TBString{U" MARKS ",     {background.s.bg, 0xF07040, TextStyle::Bold}},
				TBString{U" CLIPBOARD ", {background.s.bg, 0xF07040, TextStyle::Bold}},
				TBString{U" JOBS ",      {background.s.bg, 0xF07040, TextStyle::Bold}},
				// Debug modes
				TBString{U" CACHE ",     {background.s.bg, 0xD080D0, TextStyle::Bold}},
				TBString{U" POSITIONS ", {background.s.bg, 0xD080D0, TextStyle::Bold}},
			};

			constexpr TBStyle clipboard_number{0xCFFFFF, background.s.bg, TextStyle::None}; ///< Number of cut/yanked in current folder
			const static std::array<TBString, 2> clipboard_modes =
			{
				TBString{U"cut",  {0xCFFFFF, background.s.bg, TextStyle::None}},
				TBString{U"yank", {0xCFFFFF, background.s.bg, TextStyle::None}},
			};
		}

		namespace Filter
		{
			const TBString filter_prompt_prefix(U"filter: ", {0xF0A000, main_window_background.s.bg, TextStyle::Bold});
			constexpr TBChar filter_prompt_bg(U' ', 0xFFFFFF, main_window_background.s.bg, TextStyle::None);
			constexpr std::size_t filter_prompt_max_length = 1<<10;

			constexpr TBStyle filter_match{0x000000, 0xFA4A20, TextStyle::Underline}; // FG is ignored
		}

		namespace Search
		{
			const static TBString search_prompt_prefix{U"search: ", {0x00F0A0, main_window_background.s.bg, TextStyle::Bold}};
			constexpr TBChar search_prompt_background{U' ', {0xFFFFFF, main_window_background.s.bg, TextStyle::None}};
			constexpr std::size_t search_prompt_max_length = 1<<10;

			const static TBString search_not_found = {U"Nothing found", {0x00F0A0, 0, TextStyle::Bold}}; ///< Message in case nothing is found (background will be ignored)
			const static std::chrono::duration<std::size_t> search_not_found_duration = std::chrono::seconds(2);

			constexpr TBStyle search_match{0x000000, 0xFA4AF0, TextStyle::Underline}; // FG is ignored
		}

		namespace Menu
		{
			constexpr TBStyle background{0xFFFFFF, 0x202020, TextStyle::None};

			// Go
			constexpr TBStyle go_menu_categories{0xFFFFFF, 0x202020, TextStyle::Bold};
			constexpr TBStyle go_menu{0xCFCFCF, COLOR_DEFAULT, TextStyle::None};

			// Marks
			constexpr TBStyle marks_menu_categories{0xFFFFFF, 0x202020, TextStyle::Bold};
			constexpr TBStyle marks_menu{0xCFCFCF, COLOR_DEFAULT, TextStyle::None};

			// Cut
			constexpr TBStyle cut_menu_categories{0xFFFFFF, 0x202020, TextStyle::Bold};
			constexpr TBStyle cut_menu{0xCFCFCF, COLOR_DEFAULT, TextStyle::None};

			// Yank
			constexpr TBStyle yank_menu_categories{0xFFFFFF, 0x202020, TextStyle::Bold};
			constexpr TBStyle yank_menu{0xCFCFCF, COLOR_DEFAULT, TextStyle::None};

			// Show
			constexpr TBStyle show_menu_categories{0xFFFFFF, 0x202020, TextStyle::Bold};
			constexpr TBStyle show_menu{0xCFCFCF, COLOR_DEFAULT, TextStyle::None};
			const TBString show_menu_none{U"", {0xCFCFCF, COLOR_DEFAULT, TextStyle::None}};
			const TBString show_menu_true{U"true", {0x5777FF, COLOR_DEFAULT, TextStyle::Bold}};
			const TBString show_menu_false{U"false", {0xFF5777, COLOR_DEFAULT, TextStyle::Bold}};

			// Change
			constexpr TBStyle change_menu_categories{0xFFFFFF, 0x202020, TextStyle::Bold};
			constexpr TBStyle change_menu{0xCFCFCF, COLOR_DEFAULT, TextStyle::None};

			// Sort
			constexpr TBStyle sort_menu_categories{0xFFFFFF, 0x202020, TextStyle::Bold};
			constexpr TBStyle sort_menu{0xCFCFCF, COLOR_DEFAULT, TextStyle::None};
			const TBString sort_menu_none{U"", {0xCFCFCF, COLOR_DEFAULT, TextStyle::None}};
			const TBString sort_menu_true{U"true", {0x5777FF, COLOR_DEFAULT, TextStyle::Bold}};
			const TBString sort_menu_false{U"false", {0xFF5777, COLOR_DEFAULT, TextStyle::Bold}};
			const TBString sort_menu_selected{U"X", {0x40F090, COLOR_DEFAULT, TextStyle::Bold}};

			// Shell
			constexpr TBStyle shell_menu_categories{0xFFFFFF, 0x202020, TextStyle::Bold};
			constexpr TBStyle shell_menu{0xCFCFCF, COLOR_DEFAULT, TextStyle::None};
		}

		namespace Marks
		{
			constexpr TBChar background{U' ', 0xFFFFFF, COLOR_DEFAULT};
			constexpr TBStyle path{0x40D0F0, background.s.bg, TextStyle::Bold};
			constexpr TBStyle path_hovered{background.s.bg, 0x40D0F0, TextStyle::Bold};

			static const std::array<TBString, MarkType::size-1> mark_prefix = Util::make_array // Background will be ignored
			(
				TBString{U"s", {0x707070, main_window_background.s.bg, TextStyle::Bold}}, // SELECTED
				TBString{U"t", {0xC04040, main_window_background.s.bg, TextStyle::Bold}}, // TAGGED
				TBString{U"f", {0xF0D040, main_window_background.s.bg, TextStyle::Bold}}, // FAV

				TBString{U"c", {0xF080F0, main_window_background.s.bg, TextStyle::Bold}}, // CLIP_CUT
				TBString{U"y", {0x40F0B0, main_window_background.s.bg, TextStyle::Bold}}  // CLIP_YANK
			);

			static const std::array<TBStyle, MarkType::size-1> mark_numbers = Util::make_array // Background will be ignored
			(
				TBStyle{0x707070, main_window_background.s.bg, TextStyle::Bold}, // SELECTED
				TBStyle{0xC04040, main_window_background.s.bg, TextStyle::Bold}, // TAGGED
				TBStyle{0xF0D040, main_window_background.s.bg, TextStyle::Bold}, // FAV

				TBStyle{0xF080F0, main_window_background.s.bg, TextStyle::Bold},  // CLIP_CUT
				TBStyle{0x40B0F0, main_window_background.s.bg, TextStyle::Bold}   // CLIP_YANK
			);
		}

		namespace Clipboard
		{
			constexpr TBChar background{U' ', 0xFFFFFF, COLOR_DEFAULT};
			constexpr TBStyle path{0x40D090, background.s.bg, TextStyle::Bold};
			constexpr TBStyle path_hovered{background.s.bg, 0x40D090, TextStyle::Bold};

			constexpr TBStyle numbers{0xF0C000, background.s.bg, TextStyle::None}; // Background will be ignored
		}

		namespace Change
		{
			const TBString change_dir_prompt_prefix(U"cd: ", {0xF0A000, main_window_background.s.bg, TextStyle::Bold});
			constexpr TBChar change_dir_prompt_bg(U' ', 0xFFFFFF, main_window_background.s.bg, TextStyle::None);
			constexpr std::size_t change_dir_prompt_max = 1<<10;

			const TBString change_name_prompt_prefix(U"rename: ", {0xF0A000, main_window_background.s.bg, TextStyle::Bold});
			constexpr TBChar change_name_prompt_bg(U' ', 0xFFFFFF, main_window_background.s.bg, TextStyle::None);
			constexpr std::size_t change_name_prompt_max = 1<<8;
		}

		namespace Shell
		{
			const static TBString shell_prompt_prefix{U"shell: ", {0x00A0F0, main_window_background.s.bg, TextStyle::Bold}};
			constexpr TBChar shell_prompt_background{U' ', {0xFFFFFF, main_window_background.s.bg, TextStyle::None}};
			constexpr std::size_t shell_prompt_max_length = 1<<10;
		}

		namespace Cache
		{
			constexpr TBChar background{U' ', 0xFFFFFF, COLOR_DEFAULT};
			constexpr TBStyle path{0x40D0F0, background.s.bg, TextStyle::Bold};
			constexpr TBStyle path_hovered{background.s.bg, 0x40D0F0, TextStyle::Bold};

			constexpr TBStyle numbers{0xF0C000, background.s.bg, TextStyle::None}; // Background will be ignored
			constexpr TBStyle ref_count{0x904040, background.s.bg, TextStyle::Bold}; // Background will be ignored
			constexpr TBStyle update{0x404090, background.s.bg, TextStyle::Bold}; // Background will be ignored
		}

		namespace Position
		{
			constexpr TBChar background{U' ', 0xFFFFFF, COLOR_DEFAULT};
			constexpr TBStyle path{0x40D0F0, background.s.bg, TextStyle::Bold};
			constexpr TBStyle path_hovered{background.s.bg, 0x40D0F0, TextStyle::Bold};

			constexpr TBStyle numbers{0xF0C000, background.s.bg, TextStyle::None}; // Background will be ignored
			constexpr TBStyle selected{0xF04090, background.s.bg, TextStyle::Bold}; // Background will be ignored
			constexpr TBStyle last_accessed{0x404090, background.s.bg, TextStyle::Bold}; // Background will be ignored
		}

		namespace JobManager
		{
			constexpr TBChar background{U' ', 0xFFFFFF, COLOR_DEFAULT};
			constexpr TBStyle job{0x40D0F0, background.s.bg, TextStyle::Bold};
			constexpr TBStyle job_hovered{background.s.bg, 0x40D0F0, TextStyle::Bold};
			const static std::array<TBString, 5> status = Util::make_array(
				TBString{U"Queued",   {0x0CF0F0, background.s.bg, TextStyle::Italic}},
				TBString{U"Running",  {0x0CF0F0, background.s.bg, TextStyle::Italic}},
				TBString{U"Paused",   {0x0CF0F0, background.s.bg, TextStyle::Italic}},
				TBString{U"Finished", {0x0CF0F0, background.s.bg, TextStyle::Italic}},
				TBString{U"Failed",   {0x0CF0F0, background.s.bg, TextStyle::Italic}}
			);

			constexpr TBStyle numbers{0xF0C000, background.s.bg, TextStyle::None}; // Background will be ignored
			constexpr TBStyle info{0xF04090, background.s.bg, TextStyle::Bold}; // Background will be ignored
			constexpr TBStyle started{0x404090, background.s.bg, TextStyle::Bold}; // Background will be ignored
			constexpr TBStyle ended{0x404090, background.s.bg, TextStyle::Bold}; // Background will be ignored
			constexpr TBStyle progress{0x40F090, background.s.bg, TextStyle::Bold}; // Background will be ignored
		}
	}

	namespace Keys
	{
		namespace List
		{
			constexpr auto up = Util::make_array(U"UP", U"k");
			constexpr Char up_page[] = U"PGUP";
			constexpr auto down = Util::make_array(U"DOWN", U"j");
			constexpr Char down_page[] = U"PGDN";
			constexpr auto left = Util::make_array(U"LEFT", U"BACKSPACE", U"h");
			constexpr auto right = Util::make_array(U"RIGHT", U"ENTER", U"l");
		}

		namespace Prompt
		{
			constexpr Char cancel[] = U"ESC";
			constexpr Char submit[] = U"ENTER";
			constexpr Char remove[] = U"BACKSPACE";
			constexpr Char word_kill[] = U"C-w";
			constexpr Char word_complete[] = U"TAB";

			constexpr Char left[] = U"LEFT";
			constexpr Char word_left[] = U"C-LEFT";
			constexpr Char right[] = U"RIGHT";
			constexpr Char word_right[] = U"C-RIGHT";
			constexpr Char begining[] = U"C-a";
			constexpr Char end[] = U"C-e";
		}

		namespace Cache
		{
			constexpr Char cache[] = U"S-D c";
			constexpr Char exit[] = U"ESC";
		}

		namespace Position
		{
			constexpr Char position[] = U"S-D p";
			constexpr Char exit[] = U"ESC";
		}

		namespace Go
		{
			constexpr Char menu[] = U"g";
			constexpr Char top[] = U"g g";
			constexpr Char bottom[] = U"S-G"; // Will also work for <repeat> S-G
			constexpr Char home[] = U"g h";
			constexpr Char root[] = U"g /";

			constexpr Char tab_next[] = U"g t";
			constexpr Char tab_prev[] = U"g S-t";
			constexpr Char tab_new[] = U"g n";
			constexpr Char tab_close[] = U"g c";
		}

		namespace Sort
		{
			constexpr Char menu[]           = U"o";
			constexpr Char sort_basename[]  = U"o b";
			constexpr Char sort_size[]      = U"o s";
			constexpr Char sort_atime[]     = U"o a";
			constexpr Char sort_mtime[]     = U"o m";
			constexpr Char sort_ext[]       = U"o e";
			constexpr Char reverse[]        = U"o r";
			constexpr Char dir_first[]      = U"o d";
			constexpr Char case_sensitive[] = U"o i";
		}

		namespace Shell
		{
			constexpr Char menu[]       = U"s"; ///< Shell menu
			constexpr Char shell_at[]   = U"S-S"; ///< Open shell at
			constexpr Char shell[]      = U"!"; ///< Run shell
			constexpr Char shell_file[] = U"@"; ///< Run shell but adds a file component
		}

		namespace Marks
		{
			constexpr Char menu[] = U"m";
			constexpr Char exit[] = U"ESC";
			constexpr Char marks[] = U"m m";
			constexpr Char select[] = U"SPC";
			constexpr Char select_toggle_all[] = U"v";
			constexpr Char unselect_all[] = U"m v";
			constexpr Char tag[] = U"t";
			constexpr Char fav[] = U"m f";

			// Note: CUT and YANK bindings are in Settings::Keys::Cut and Settings::Keys::Yank
		}

		namespace Cut
		{
			constexpr Char menu[] = U"d";
			// Note: Calling any of the cut or yank keybinding will
			// result in changing the clipboard'mode to respectivly
			// cut or yank mode. (And thus reset the clipboard)
			constexpr Char cut_set[] = U"d d"; ///< Set the clipboard to the list of currently selected files (in current folder)
			constexpr Char cut_add[] = U"d a"; ///< Add selected files (in current folder) to the clipboard
			constexpr Char cut_remove[] = U"d r"; ///< Add selected files (in current folder) from the clipboard
			constexpr Char cut_toggle[] = U"d t"; ///< Toggle selected files (in current folder) in the clipboard
		}

		namespace Yank
		{
			constexpr Char menu[] = U"y";
			// Note: Calling any of the cut or yank keybinding will
			// result in changing the clipboard'mode to respectivly
			// cut or yank mode. (And thus reset the clipboard)
			constexpr Char yank_set[] = U"y y"; ///< Set the clipboard to the list of currently selected files (in current folder)
			constexpr Char yank_add[] = U"y a"; ///< Add selected files (in current folder) to the clipboard
			constexpr Char yank_remove[] = U"y r"; ///< Add selected files (in current folder) from the clipboard
			constexpr Char yank_toggle[] = U"y t"; ///< Toggle selected files (in current folder) in the clipboard
		}

		namespace Clipboard
		{
			constexpr Char list[] = U"j c";
			constexpr Char exit[] = U"ESC";
		}

		namespace JobManager
		{
			constexpr Char manager[] = U"j m";
			constexpr Char exit[] = U"ESC";
		}

		namespace Show
		{
			constexpr Char menu[] = U"z";
			constexpr Char hidden[] = U"z h";
			constexpr Char parent[] = U"z p";
		}

		namespace Change
		{
			constexpr Char menu[] = U"c";
			constexpr Char directory[] = U"c d";
			constexpr Char name[] = U"c w";
			constexpr Char name_empty[] = U"c S-W";
		}

		namespace Search
		{
			constexpr Char search[] = U"/";
			constexpr Char next[] = U"n";
			constexpr Char prev[] = U"S-N";
		}

		constexpr Char filter[] = U"f";
	}

	namespace Filter
	{
		constexpr auto regex_mode = std::regex_constants::ECMAScript | std::regex_constants::icase;
		constexpr auto search_mode = std::regex_constants::match_any;
	}

	namespace Search
	{
		constexpr auto regex_mode = std::regex_constants::ECMAScript | std::regex_constants::icase;
		constexpr auto search_mode = std::regex_constants::match_any;
	}

	namespace Cache
	{
		constexpr std::size_t cache_num = 1<<8; ///< Maximum number of (unused) cached directories at once
		constexpr std::chrono::duration<std::size_t> update_age(std::chrono::seconds(300));  ///< Automatically update from filesystem if older than (in seconds)
	}

	namespace Position
	{
		constexpr std::size_t cache_num = 1<<8; ///< Maximum number of cached positions (per Tab)
	}
}

#endif // INDEX_SETTINGS_HPP
