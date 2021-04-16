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
		Ft2{U""sv, {0x40BFFF, 0x40BFFF}, TextStyle::Bold, {U"Documents"sv}},
		Ft2{U""sv, {0x40BFFF, 0x40BFFF}, TextStyle::Bold, {U"Downloads"sv}},
		Ft2{U""sv, {0x40BFFF, 0x40BFFF}, TextStyle::Bold, {U"Music"sv}},
		Ft2{U""sv, {0x40BFFF, 0x40BFFF}, TextStyle::Bold, {U"Pictures"sv}},
		Ft2{U""sv, {0x40BFFF, 0x40BFFF}, TextStyle::Bold, {U"Videos"sv}}
	);

	static const auto FullnameFiles = Util::make_array(
		Ft2{U"ex"sv, {0xA0F0B0}, TextStyle::None, {U"a.out"sv}}
	);

	static const auto Extensions = Util::make_array(
		Ft2{U""sv, {0xFFFFFF, 0x4040F0}, TextStyle::None, {U"c"sv, U"h"sv}},
		Ft2{U""sv, {0xFFFFFF, 0x4040F0}, TextStyle::None, {U"cpp"sv, U"hpp"sv, U"cxx"sv, U"tcc"sv, U"cc"sv, U"hh"sv}}
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
