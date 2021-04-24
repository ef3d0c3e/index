#include "Jobs.hpp"


Job::Job(const String& name):
	m_name(name),
	m_info(),
	m_start(),
	m_end()
{
	m_status.store(Job::QUEUED);
	m_progress.store(0);
	m_pause.store(false);
	m_abort.store(false);
}

Job::~Job()
{

}

Job::Status Job::GetStatus() const
{
	return m_status.load();
}

int Job::GetProgress() const
{
	return m_progress.load();
}

std::chrono::time_point<std::chrono::system_clock> Job::GetStartTime() const
{
	return m_start;
}

std::chrono::time_point<std::chrono::system_clock> Job::GetEndTime() const
{
	return m_end;
}

