#include "DirectoryCache.hpp"

std::vector<std::string> DirectoryCache::GetDirs(const std::string& path)
{
	std::vector<std::string> dirs;
	dirs.reserve(8); // Some very rough estimation

	std::size_t lastpos = 1; // because (path[0] is always '/')
	for (std::size_t i = 1 ; i < path.size(); ++i)
	{
		if (path[i] == '/')
		{
			dirs.push_back( std::move(path.substr(lastpos, i-lastpos)) );

			lastpos = i+1;
		}
	}
	auto last = path.substr(lastpos);
	if (!last.empty())
		dirs.push_back(std::move(last));

	return dirs;
}

Node* DirectoryCache::Find(const std::vector<std::string>& dirs)
{
	Node* node = head;
	for (const auto& dir : dirs)
	{
		const auto it = head->children.find(dir);
		if (it == head->children.end())
			return nullptr;

		node = it->second;
	}

	return node;
}

Directory* DirectoryCache::GetDirectory(const std::string& path)
{
	const auto dirs = GetDirs(dir);
	const Node* d = Find(dirs);
	if (d == nullptr || d->dir == nullptr) // Not cached
	{
		
		auto Directory = new Directory();
	}

	++d->refCount;
	return d->dir;
}

void DirectoryCache::DeleteDirectory(Directory* dir)
{
	const auto it = m_cache.find(dir->GetPath());
	if (it == m_cache.end()) // not cached
	{
		delete dir;
		return;
	}

	if (it->second.refCount == 0)
		throw "Negative refCount";
	--it->second.refCount;
}

