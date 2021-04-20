#ifndef INDEX_EXCEPTIONS_HPP
#define INDEX_EXCEPTIONS_HPP

#include "TermboxWidgets/Text.hpp"

class IndexError
{
public:
	MAKE_CENUMV_Q(Type, std::uint8_t,
		GENERIC_ERROR, 0,
		FORK_FAILED, 1,
		FS_CANT_ACCESS, 2,
		FORMAT_ERROR, 3,
	);

private:
	typedef std::remove_reference<decltype(errno)>::type ErrorType;
	String m_message;
	Type m_exceptionType;
	ErrorType m_errno;

public:
	IndexError(const String& message, Type type, ErrorType error = 0) throw():
		m_message(message), m_exceptionType(type), m_errno(error) {}
	~IndexError() throw() {}

	String GetMessage() const throw()
	{
		return m_message;
	}

	Type GetType() const throw()
	{
		return m_exceptionType;
	}

	ErrorType GetErrno() const throw()
	{
		return m_errno;
	}
};

#endif // INDEX_EXCEPTIONS_HPP
