#ifndef INDEX_FILETYPE_HPP
#define INDEX_FILETYPE_HPP

#include "TermboxWidgets/Text.hpp"
#include <map>
struct File;

typedef std::size_t FtID;
namespace FileType
{
constexpr FtID NoID = 0;

static std::map<StringView, FtID> ExtensionMap;
static std::map<StringView, FtID> FullnameDirMap;
static std::map<StringView, FtID> FullnameFilesMap;

const static auto CompiledIcons = []()
{
	constexpr Color default_numbers = 0xFFFFFF;
	constexpr Color default_size = 0xF0D0D0;
	constexpr Color default_sizeUnit = 0xF0D0D0;
	// {{{ Temporary structures
	struct Colors
	{
		Color name;
		Color icon;
		Color size = default_size;
		Color sizeUnit = default_sizeUnit;
		Color numbers = default_numbers;


		constexpr Colors(Color name = Color())
		{
			this->name = icon = name;
		}

		constexpr Colors(Color name, Color icon)
		{
			this->name = name;
			this->icon = icon;
		}

		constexpr Colors(Color name, Color icon, Color size)
		{
			this->name = name;
			this->icon = icon;
			this->size = sizeUnit = size;
		}

		constexpr Colors(Color name, Color icon, Color size, Color numbers)
		{
			this->name = name;
			this->icon = icon;
			this->size = sizeUnit = size;
			this->numbers = numbers;
		}

		constexpr Colors(Color name, Color icon, Color size, Color sizeUnit, Color numbers)
		{
			this->name = name;
			this->icon = icon;
			this->numbers = numbers;
			this->size = size;
			this->sizeUnit = sizeUnit;
		}
	};

	struct Ft2
	{
		StringView icon;
		Colors c;
		TextStyle s;
		std::vector<StringView> exts;
	};

	struct Ft
	{
		StringView icon;
		Colors c;
		TextStyle s;
	
		constexpr Ft& operator=(const Ft2& f)
		{
			icon = f.icon;
			c = f.c;
			s = f.s;
			return *this;
		}
	};
	// }}}

	using namespace std::literals;
	static constexpr auto General = Util::make_array(
		Ft{U"?"sv, {0xFFFFFF}, TextStyle::None}, // unknown
		Ft{U""sv, {0x40BFFF, 0x40BFFF}, TextStyle::Bold}, // directory
		Ft{U""sv, {0xFFFFFF, 0x8F8F8F}, TextStyle::None}  // default file
	);

	//TODO: constexpr c++20 (>gcc11...)
	static const auto FullnameDir = Util::make_array(
		Ft2{U" "sv, {0x40BFFF, 0x40BFFF}, TextStyle::Bold, {U"Documents"sv}},
		Ft2{U" "sv, {0x40BFFF, 0x40BFFF}, TextStyle::Bold, {U"Downloads"sv}},
		Ft2{U" "sv, {0x40BFFF, 0x40BFFF}, TextStyle::Bold, {U"Music"sv}},
		Ft2{U" "sv, {0x40BFFF, 0x40BFFF}, TextStyle::Bold, {U"Pictures"sv}},
		Ft2{U" "sv, {0x40BFFF, 0x40BFFF}, TextStyle::Bold, {U"Videos"sv}}
	);

	static const auto FullnameFiles = Util::make_array(
		Ft2{U""sv, {0xB0A070}, TextStyle::None, {U"LICENSE"sv}},
		Ft2{U""sv, {0xB0A070}, TextStyle::None, {U"Makefile"sv, U"CMakeLists.txt"sv}}
	);

	static const auto Extensions = Util::make_array(
		Ft2{U" "sv, {0xFFFFFF, 0x4040F0}, TextStyle::None, {U"c"sv, U"h"sv}},
		Ft2{U" "sv, {0xFFFFFF, 0x4040F0}, TextStyle::None, {U"cpp"sv, U"hpp"sv, U"cxx"sv, U"tcc"sv, U"cc"sv, U"hh"sv}},
		Ft2{U" "sv, {0xFFFFFF, 0x4040F0}, TextStyle::None, {U"py"sv}},
		Ft2{U"亮"sv, {0xFFFFFF, 0x4040F0}, TextStyle::None, {U"sh"sv, U"zsh"sv, U"bash"sv}},
		Ft2{U" "sv, {0xA0F0B0}, TextStyle::None, {U"out"sv}},
		Ft2{U"遲"sv, {0xD08050}, TextStyle::None, {U"tar"sv, U"xz"sv, U"gz"sv, U"bz2"sv, U"lz"sv, U"lz4"sv, U"lzma"sv, U"lzo"sv, U"rz"sv, U"z"sv, U"Z"sv, U"ar"sv, U"lzx"sv, U"rar"sv, U"zip"sv, U"7z"sv}},
		Ft2{U" "sv, {0xD9F050}, TextStyle::None, {U"json"sv}},

		// Media
		Ft2{U" "sv, {0xFFFFFF, 0x4040F0}, TextStyle::None, {U"png"sv, U"jpg"sv, U"jpeg"sv, U"gif"sv, U"bmp"sv, U"tga"sv, U"ppm"sv, U"pgm"sv, U"pbm"sv, U"pnm"sv, U"ico"sv}},
		Ft2{U" "sv, {0xFFFFFF, 0x4040F0}, TextStyle::None, {U"wav"sv, U"flac"sv, U"tta"sv, U"ape"sv, U"ogg"sv, U"mp3"sv, U"opus"sv, U"ac3"sv, U"aac"sv}},
		Ft2{U"ﳜ "sv, {0xFFFFFF, 0x4040F0}, TextStyle::None, {U"mkv"sv, U"avi"sv, U"mp4"sv, U"h264"sv, U"h265"sv, U"vp8"sv, U"vp9"sv, U"webm"sv}},
		Ft2{U" "sv, {0x2080D0}, TextStyle::None, {U"pdf"sv}}
	);


	// Fullname
	for (std::size_t i = 0; i < FullnameDir.size(); ++i)
	{
		for (const auto& e : FullnameDir[i].exts)
			FileType::FullnameDirMap.insert({e, i+General.size()+Extensions.size()});
	}
	for (std::size_t i = 0; i < FullnameFiles.size(); ++i)
	{
		for (const auto& e : FullnameFiles[i].exts)
			FileType::FullnameFilesMap.insert({e, i+General.size()+Extensions.size()+FullnameDir.size()});
	}
	
	// Extensions
	for (std::size_t i = 0; i < Extensions.size(); ++i)
	{
		for (const auto& e : Extensions[i].exts)
			FileType::ExtensionMap.insert({e, i+General.size()});
	}

	return Util::concat<Ft>(General, Util::concat<Ft>(Extensions, Util::concat<Ft>(FullnameDir, FullnameFiles)));
}();

FtID GetFtID(const File& f, const std::string& path);
}

#endif // INDEX_FILETYPE_HPP
