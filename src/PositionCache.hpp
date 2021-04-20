#ifndef INDEX_POSITIONCACHE_HPP
#define INDEX_POSITIONCACHE_HPP

#include "FileSystem.hpp"
#include <chrono>
class List;

class PositionCache
{
public:
	struct Entry
	{
		String name;
		std::chrono::time_point<std::chrono::system_clock> lastAccessed;
	};
private:
	std::map<std::string, Entry> m_cache; // path, file

	friend class PositionExplorer;

	////////////////////////////////////////////////
	/// \brief Tries to reduce the size of the cache
	/// Will delete the oldest entries until their number is below Settings::Position::cache_num;
	////////////////////////////////////////////////
	void Optimize();
public:
	////////////////////////////////////////////////
	/// \brief Default Constructor
	////////////////////////////////////////////////
	PositionCache();
	////////////////////////////////////////////////
	/// \brief Destructor
	////////////////////////////////////////////////
	~PositionCache();

	////////////////////////////////////////////////
	/// \brief Gets the position in the directory
	/// \param list The list
	/// \returns The position if found, 0 otherwise
	/// \note If found, will upedate the entry's last accessed time
	////////////////////////////////////////////////
	std::size_t GetPosition(const List* list);

	////////////////////////////////////////////////
	/// \brief Adds the list's current item to the cache
	/// \param list The list
	/// \note Will simply update the entry if it already exists
	/// \note Will run optimize if a file was added
	////////////////////////////////////////////////
	void AddPosition(const List* list);
};

#endif // INDEX_POSITIONCACHE_HPP
