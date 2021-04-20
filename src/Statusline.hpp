#ifndef INDEX_STATUSLINE_HPP
#define INDEX_STATUSLINE_HPP

#include "MainWindow.hpp"
#include <chrono>

class Statusline : public Widget
{
	MainWindow* m_main;
	TBChar m_bg;

	virtual void Draw();

	std::chrono::time_point<std::chrono::system_clock> m_messageStop;
	TBString m_message;
public:
	Statusline(MainWindow* main);
	~Statusline();

	void SetBackground(const TBChar& bg);
	const TBChar& GetBackground() const;

	////////////////////////////////////////////////
	/// \brief Display a message in the status line
	/// \param msg The message
	/// \param secs The message's duration in seconds
	////////////////////////////////////////////////
	void SetMessage(const TBString& msg, std::chrono::duration<std::size_t> secs);

	////////////////////////////////////////////////
	/// \brief Display an error in the status line
	/// \param msg The error message
	/// \param secs The message's duration in seconds
	////////////////////////////////////////////////
	void SetError(const String& msg, std::chrono::duration<std::size_t> secs);
};

#endif // INDEX_STATUSLINE_HPP
