#include "DirectoryCache.hpp"
#include "Settings.hpp"

DirectoryCache::Cached* DirectoryCache::Find(const std::string& dir)
{
	auto it = m_cache.find(dir);
	if (it == m_cache.end())
		return nullptr;

	return &it->second;
}

void DirectoryCache::Optimize()
{
	using P = std::pair<const std::string*, Cached*>;
	std::vector<P> sorted;
	sorted.reserve(m_cache.size());

	for (auto& cached : m_cache)
	{
		if (cached.second.refCount == 0)
			sorted.push_back(std::make_pair(&cached.first, &cached.second));
	}

	if (sorted.size() < Settings::Cache::cache_num)
		return;

	std::sort(sorted.begin(), sorted.end(), [](const P& l, const P& r) { return l.second->updated < r.second->updated; });

	for (std::size_t i = 0; i < sorted.size() - Settings::Cache::cache_num; ++i)
	{
		delete sorted[i].second->dir;
		m_cache.erase(*sorted[i].first);
	}
}

DirectoryCache::DirectoryCache()
{
	
}

DirectoryCache::~DirectoryCache()
{
	for (auto& cached : m_cache)
		delete cached.second.dir;
}


std::pair<Directory*, bool> DirectoryCache::GetDirectory(const std::string& path)
{
	const std::string resolved = GetUsablePath(path);

	Cached* cached = Find(resolved);
	if (cached == nullptr) // Not cached
	{
		auto dir = new Directory(resolved);
		m_cache.insert(std::make_pair(dir->GetPath(), DirectoryCache::Cached{
			.dir = dir,
			.updated = std::chrono::system_clock::now(),
			.refCount = 1,
		}));

		Optimize();
		return std::make_pair(dir, false);
	}

	bool shouldUpdate = std::chrono::system_clock::now()-cached->updated > Settings::Cache::update_age;

	++cached->refCount;
	return std::make_pair(cached->dir, shouldUpdate);
}

void DirectoryCache::DeleteDirectory(Directory* dir)
{
	Cached* cached = Find(dir->GetPath());
	if (cached == nullptr) // not cached (should not happen...)
	{
		delete dir;
		return;
	}

	if (cached->refCount == 0)
	{
		std::cout << "Dir = " << dir->GetPath() << "|\n";
		throw "Negative refCount";
	}
	--cached->refCount;
}
