#include "PositionCache.hpp"
#include "Settings.hpp"
#include "UI/List.hpp"

PositionCache::PositionCache()
{ }

PositionCache::~PositionCache()
{ }

std::size_t PositionCache::GetPosition(const List* list)
{
	auto it = m_cache.find(list->GetDir()->GetPath());
	if (it == m_cache.end())
		return 0;

	it->second.lastAccessed = std::chrono::system_clock::now();

	// Find its position
	const auto pos = list->FindByName(it->second.name);
	if (pos == static_cast<std::size_t>(-1)) [[unlikely]]
		return 0;

	return  pos;
}

void PositionCache::AddPosition(const List* list)
{
	const String name = list->GetCurrentFileName();
	if (name == U"") [[unlikely]]
		return;

	m_cache[list->GetDir()->GetPath()] = Entry{
		.name = name,
		.lastAccessed = std::chrono::system_clock::now(),
	};

	Optimize();
}

void PositionCache::Optimize()
{
	if (m_cache.size() < Settings::Position::cache_num)
		return;

	using P = std::pair<const std::string*, Entry*>;
	std::vector<P> sorted;
	sorted.reserve(m_cache.size());


	for (auto& cached : m_cache)
		sorted.push_back(std::make_pair(&cached.first, &cached.second));

	std::sort(sorted.begin(), sorted.end(), [](const P& l, const P& r) { return l.second->lastAccessed < r.second->lastAccessed; });

	for (std::size_t i = 0; i < sorted.size() - Settings::Cache::cache_num; ++i)
		m_cache.erase(*sorted[i].first);
}
