#ifndef INDEX_ACTIONS_JOBS_HPP
#define INDEX_ACTIONS_JOBS_HPP

#include "../Settings.hpp"
#include <atomic>

////////////////////////////////////////////////
/// \brief Represents a job
////////////////////////////////////////////////
class Job
{
public:
	MAKE_CENUMV_Q(Status, std::uint8_t,
		QUEUED, 0, ///< Has not started yet
		RUNNING, 1, ///< Has started and is in progress
		PAUSED, 2, ///< Task was paused
		FINISHED, 3, ///< Task has finished
		FAILED, 4, ///< Task has failed
	);

	MAKE_CENUMV_Q(JobType, std::uint8_t,
		DELETE, 0,
		COPY, 1,
		MOVE, 2,
		OTHER, 3,
	);

private:
	std::atomic<Status> m_status; ///< Current statuys
	std::atomic<int> m_progress; ///< Completion progress in per 1000
	String m_name; ///< Name of the task
	String m_info; ///< Additional informations, like fail reason, etc...
	std::chrono::time_point<std::chrono::system_clock> m_start, m_end; ///< When has the task started/ended (not always initialized, check using the status)
	std::atomic<bool> m_pause; ///< Set to true to pause the current task
	std::atomic<bool> m_abort; ///< Set to true to abort the current task (this may have some consequences...)

public:
	////////////////////////////////////////////////
	/// \brief Constructor
	////////////////////////////////////////////////
	Job(const String& name);

	////////////////////////////////////////////////
	/// \brief Destructor
	////////////////////////////////////////////////
	~Job();

	////////////////////////////////////////////////
	/// \brief Get the current status
	/// \returns The current status
	////////////////////////////////////////////////
	Status GetStatus() const;

	////////////////////////////////////////////////
	/// \brief Get the current progress
	/// \returns The current progress
	////////////////////////////////////////////////
	int GetProgress() const;

	////////////////////////////////////////////////
	/// \brief Get the time point at which the job started
	/// \returns The time point at which the job started
	////////////////////////////////////////////////
	std::chrono::time_point<std::chrono::system_clock> GetStartTime() const;

	////////////////////////////////////////////////
	/// \brief Get the time point at which the job ended
	/// \returns The time point at which the job ended
	////////////////////////////////////////////////
	std::chrono::time_point<std::chrono::system_clock> GetEndTime() const;

	////////////////////////////////////////////////
	/// \brief Gets the type of job
	/// \returns Thje type of job
	////////////////////////////////////////////////
	virtual JobType GetType() const = 0;

	////////////////////////////////////////////////
	/// \brief Start the job
	////////////////////////////////////////////////
	virtual void Start() = 0;

	////////////////////////////////////////////////
	/// \brief Pause the job
	/// \param v True to pause, false to resume
	////////////////////////////////////////////////
	virtual void Pause(bool v) = 0;

	////////////////////////////////////////////////
	/// \brief Abort the job
	////////////////////////////////////////////////
	virtual void Abort() = 0;
};

static std::deque<std::unique_ptr<Job>> gJobList;

#endif // INDEX_ACTIONS_JOBS_HPP
