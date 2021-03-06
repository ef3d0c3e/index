#ifndef INDEX_DIRECTORYCACHE_HPP
#define INDEX_DIRECTORYCACHE_HPP

#include "FileSystem.hpp"
#include <chrono>

class DirectoryCache
{
public:
	struct Cached
	{
		////////////////////////////////////////////////
		/// \brief Represents a cached directory
		////////////////////////////////////////////////

		Directory* dir = nullptr; ///< The cached directory. If nullptr, then The directory referred by this node is not cached...
		std::chrono::time_point<std::chrono::system_clock> updated; ///< The time this node was last updated
		std::size_t refCount = 0; ///< Number of references to this directory
	};

	friend class CacheExplorer;
private:
	std::map<std::string, Cached> m_cache;

	////////////////////////////////////////////////
	/// \brief Find a directory in the cache by its name
	/// \param path The path
	/// \returns The cached directory or nullptr if not found
	////////////////////////////////////////////////
	Cached* Find(const std::string& path);

	////////////////////////////////////////////////
	/// \brief Tries to reduce the size of the cache
	/// Will only delete directories with a ```refCount``` of 0
	/// Will prefer deleting the least recently updated directories
	////////////////////////////////////////////////
	void Optimize();
public:
	////////////////////////////////////////////////
	/// \brief Default Constructor
	////////////////////////////////////////////////
	DirectoryCache();
	////////////////////////////////////////////////
	/// \brief Desctructor
	////////////////////////////////////////////////
	~DirectoryCache();

	////////////////////////////////////////////////
	/// \brief Get a directory from the cache or allocate it
	/// \param path The path
	/// \returns A directory corresponding to path <The directory, Wether or not the directory should be updated from filesystem>
	/// \note May throw if the directory has to be initialized
	////////////////////////////////////////////////
	std::pair<Directory*, bool> GetDirectory(const std::string& path);

	////////////////////////////////////////////////
	/// \brief Handles a directory
	/// \param path The path
	/// Will decrease the ```refCount``` of this directory and may call ```delete``` on the directory
	////////////////////////////////////////////////
	void DeleteDirectory(Directory* dir);
};

extern DirectoryCache gDirectoryCache;

#endif // INDEX_DIRECTORYCACHE_HPP
