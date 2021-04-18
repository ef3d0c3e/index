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

namespace Sort
{
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
			if (std::towlower(l.name[i]) < std::towlower(r.name[i]))
				return true;
			if (std::towlower(l.name[i]) > std::towlower(r.name[i]))
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
// }}}
}

#endif // INDEX_SORT_HPP
