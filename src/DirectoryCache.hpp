#ifndef INDEX_DIRECTORYCACHE_HPP
#define INDEX_DIRECTORYCACHE_HPP

#include "FileSystem.hpp"

class DirectoryCache
{
public:
	struct Cached
	{
		////////////////////////////////////////////////
		/// \brief Represents a cached directory
		/// A node is a directory, which may have a cache entry
		/// and may contain children. (But must satisfy at least one of the two)
		////////////////////////////////////////////////

		Directory* dir = nullptr; ///< The cached directory. If nullptr, then The directory referred by this node is not cached...
		Time cached; ///< The time this node was added to the cache
		std::size_t refCount = 0; ///< Number of references to this directory
	};

	////////////////////////////////////////////////
	/// \brief Splits the path into directories
	/// \param path The path
	/// \returns A vector containing every directory comprising the path
	/// e.g: ```'/home/me/Downloads' -> {'home', 'me', 'Downloads'}```
	////////////////////////////////////////////////
	static std::vector<std::string> GetDirs(const std::string& path);
private:
	std::map<std::string, Cached> m_cache;
public:
	////////////////////////////////////////////////
	/// \brief Get a directory from the cache or allocate it
	/// \param path The path
	/// \returns A directory corresponding to path
	////////////////////////////////////////////////
	Directory* GetDirectory(const std::string& path);

	////////////////////////////////////////////////
	/// \brief Handles a directory
	/// \param path The path
	/// Will decrease the ```refCount``` of this directory and may call ```delete``` on the directory
	////////////////////////////////////////////////
	void DeleteDirectory(Directory* dir);
};

#endif // INDEX_DIRECTORYCACHE_HPP
