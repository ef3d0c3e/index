#ifndef INDEX_SORT_HPP
#define INDEX_SORT_HPP

#include "FileSystem.hpp"

////////////////////////////////////////////////
/// \brief Represents a match between a file and a filter
/// Currently there are only two possible match type: FILTER and FIND
////////////////////////////////////////////////
struct FileMatch
{
	MAKE_CENUMV_Q(MatchType, std::uint8_t,
		FILTER, 0,
		FIND, 1, // TODO...
	);
	std::vector<std::tuple<MatchType, std::size_t, std::size_t>> matches; // <pos, size>

	FileMatch() {}
};

MAKE_CENUM_Q(SortFnID, std::size_t,
	BASENAME, 0,
	SIZE, 1,
	ACCESS_TIME, 2,
	MDOFICATION_TIME, 3,
	EXTENSION, 4,
);

namespace Sort
{
struct Settings
{
	bool DirFist = true;
	bool CaseSensitive = false;
	bool Reverse = false;
	SortFnID SortFn = 0;
};

// {{{ Functions
static bool Basename(const std::pair<const File*, FileMatch> fl, const std::pair<const File*, FileMatch> fr, const Settings& s)
{
	const File& l = *fl.first;
	const File& r = *fr.first;
	if (s.DirFist)
	{
		if ((l.mode & Mode::DIR || l.lnk.mode & Mode::DIR) && !(r.mode & Mode::DIR || r.lnk.mode & Mode::DIR))
			return true;
		if ((r.mode & Mode::DIR || r.lnk.mode & Mode::DIR) && !(l.mode & Mode::DIR || l.lnk.mode & Mode::DIR))
			return false;
	}

	const std::size_t sizeCmp = std::min(l.name.size(), r.name.size());

	if (s.CaseSensitive)
	{
		for (size_t i = 0; i < sizeCmp; ++i)
		{
			if (l.name[i] < r.name[i])
				return true;
			if (l.name[i] > r.name[i])
				return false;
		}
	}
	else
	{
		for (size_t i = 0; i < sizeCmp; ++i)
		{
			if (std::towlower(l.name[i]) < std::towlower(r.name[i]))
				return true;
			if (std::towlower(l.name[i]) > std::towlower(r.name[i]))
				return false;
		}
	}

	if (l.name.size() < r.name.size())
		return true;
	if (r.name.size() < l.name.size())
		return false;
	return false;
}

static bool Size(const std::pair<const File*, FileMatch> fl, const std::pair<const File*, FileMatch> fr, const Settings& s)
{
	const File& l = *fl.first;
	const File& r = *fr.first;
	if (s.DirFist)
	{
		if ((l.mode & Mode::DIR || l.lnk.mode & Mode::DIR) && !(r.mode & Mode::DIR || r.lnk.mode & Mode::DIR))
			return true;
		if ((r.mode & Mode::DIR || r.lnk.mode & Mode::DIR) && !(l.mode & Mode::DIR || l.lnk.mode & Mode::DIR))
			return false;
	}

	return l.sz > r.sz;
}

static bool AccessTime(const std::pair<const File*, FileMatch> fl, const std::pair<const File*, FileMatch> fr, const Settings& s)
{
	const File& l = *fl.first;
	const File& r = *fr.first;
	if (s.DirFist)
	{
		if ((l.mode & Mode::DIR || l.lnk.mode & Mode::DIR) && !(r.mode & Mode::DIR || r.lnk.mode & Mode::DIR))
			return true;
		if ((r.mode & Mode::DIR || r.lnk.mode & Mode::DIR) && !(l.mode & Mode::DIR || l.lnk.mode & Mode::DIR))
			return false;
	}

	return l.lastAccess > r.lastAccess;
}

static bool ModificationTime(const std::pair<const File*, FileMatch> fl, const std::pair<const File*, FileMatch> fr, const Settings& s)
{
	const File& l = *fl.first;
	const File& r = *fr.first;
	if (s.DirFist)
	{
		if ((l.mode & Mode::DIR || l.lnk.mode & Mode::DIR) && !(r.mode & Mode::DIR || r.lnk.mode & Mode::DIR))
			return true;
		if ((r.mode & Mode::DIR || r.lnk.mode & Mode::DIR) && !(l.mode & Mode::DIR || l.lnk.mode & Mode::DIR))
			return false;
	}

	return l.lastModification > r.lastModification;
}

static bool Extension(const std::pair<const File*, FileMatch> fl, const std::pair<const File*, FileMatch> fr, const Settings& s)
{
	const File& l = *fl.first;
	const File& r = *fr.first;
	if (s.DirFist)
	{
		if ((l.mode & Mode::DIR || l.lnk.mode & Mode::DIR) && !(r.mode & Mode::DIR || r.lnk.mode & Mode::DIR))
			return true;
		if ((r.mode & Mode::DIR || r.lnk.mode & Mode::DIR) && !(l.mode & Mode::DIR || l.lnk.mode & Mode::DIR))
			return false;
	}

	auto GetExt = [](const File& f) -> String
	{
		std::size_t pos = f.name.rfind(U'.');
		if (pos == String::npos)
			return U"";

		return f.name.substr(pos+1);
	};
	const String lext = GetExt(l);
	const String rext = GetExt(r);
	const std::size_t sizeCmp = std::min(lext.size(), rext.size());

	if (s.CaseSensitive)
	{
		for (size_t i = 0; i < sizeCmp; ++i)
		{
			if (lext[i] < rext[i])
				return true;
			if (lext[i] > rext[i])
				return false;
		}
	}
	else
	{
		for (size_t i = 0; i < sizeCmp; ++i)
		{
			if (std::towlower(lext[i]) < std::towlower(rext[i]))
				return true;
			if (std::towlower(lext[i]) > std::towlower(rext[i]))
				return false;
		}
	}

	if (lext.size() < rext.size())
		return true;
	if (rext.size() < lext.size())
		return false;
	return false;
}
// }}}
}

constexpr auto SortFns = Util::make_array
(
	Sort::Basename,
	Sort::Size,
	Sort::AccessTime,
	Sort::ModificationTime,
	Sort::Extension
);

#endif // INDEX_SORT_HPP
