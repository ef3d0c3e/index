#include "Open.hpp"
#include <fmt/format.h>
#include "../TermboxWidgets/Termbox.hpp"
#include "../FileSystem.hpp"
#include <aio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>

#include <fstream>
#define LOG(__msg)                                  \
{                                                   \
	static bool first = true;                       \
	std::ofstream logs;                             \
	if (first)                                      \
	{                                               \
		first = false;                              \
		logs.open("/tmp/index_log", std::ios::ate); \
	}                                               \
	else                                            \
		logs.open("/tmp/index_log", std::ios::app); \
	logs << __msg;                                  \
	logs.close();                                   \
}

std::string Actions::Opener::GetCommand(const String& fname, const std::string& path) const
{
	std::string name = Util::StringConvert<char>(fname);
	for (std::size_t i = 0; i < name.size(); ++i)
	{
		if (name[i] == ' ')
		{
			name.insert(i, 1, '\\');
			++i;
		}
	}

	return fmt::format(format, fmt::arg("file", name), fmt::arg("path", path));
}

bool Actions::Openers::isText(const File& f, const std::string& path, const std::string& ext)
{
	if (f.mode != Mode::REG && f.lnk.mode != Mode::REG)
		return false;

	// Executables
	if ((ext == "" && (f.perm & Permission::UX))
		|| ext == "out")
		return false;

	return true;
}

std::pair<Actions::OpenType, const Actions::Opener*> Actions::GetOpener(const File& f, const std::string& path)
{
	const auto pos = f.name.rfind(U'.');
	if (pos != String::npos)
	{
		// Extension is in the list
		const std::string ext = Util::StringConvert<char>(f.name.substr(pos+1));
		auto it = Actions::Openers::List.find(ext);
		if (it != Actions::Openers::List.end())
			return {Actions::OpenType::Executable, it->second};

		// Text
		if (Actions::Openers::isText(f, path, ext))
		{
			return {Actions::OpenType::Executable, &Actions::Openers::TextEditor};
		}
	}
	// Text
	if (Actions::Openers::isText(f, path, ""))
	{
		return {Actions::OpenType::Executable, &Actions::Openers::TextEditor};
	}

	return {Actions::OpenType::CustomOpen, nullptr};
}

void Actions::Open(const String& fname, const std::string& path, const Opener& opener)
{
	const std::string command = opener.GetCommand(fname, path);

	int status = 0;
	pid_t pid = fork();
	if (pid == -1)
		throw IndexError(U"Actions::Open() fork() failed", IndexError::FORK_FAILED, errno);
	
	if (opener.flag & Opener::Close)
		Termbox::Close();
	if (pid == 0) // child
	{
		Termbox::GetContext().stop = true;
		if (chdir(path.c_str()) == -1)
			throw IndexError(U"Actions::Open() chdir() failed", IndexError::FS_CANT_ACCESS, errno);

		if (opener.flag & Opener::SupressOutput)
		{
			int fd = open("/dev/null", O_WRONLY, 0200);

			dup2(fd, 1); // stdout
			dup2(fd, 2); // stderr
			close(fd);
		}
		if (opener.flag & Opener::SetSid)
			setsid();

		execlp("/bin/sh", "sh", "-c", command.c_str(), (char*)0);
		exit(1);
	}
	else // parent
	{
		if (opener.flag & Opener::Close)
		{
			waitpid(pid, &status, 0);
			Termbox::ReOpen();
		}
	}
}

void Actions::CustomOpen(const String& fname, const std::string& path, const std::string& format)
{
	try
	{
		Open(fname, path, Opener(format, Opener::Close));
	}
	catch (fmt::format_error& e)
	{
		throw IndexError(U"Format error '" + Util::StringConvert<Char>(std::string(e.what())) + U"'", IndexError::FORMAT_ERROR);
	}
}

void Actions::OpenShell(const std::string& path)
{
	const char* shell = getenv("SHELL");
	if (shell == NULL)
		shell = "/usr/bin/sh";

	int status = 0;
	pid_t pid = fork();
	if (pid == -1)
		throw IndexError(U"Actions::OpenShell() fork() failed", IndexError::FORK_FAILED, errno);
	
	Termbox::Close();
	if (pid == 0) // child
	{
		Termbox::GetContext().stop = true;
		if (chdir(path.c_str()) == -1)
			throw IndexError(U"Actions::OpenShell() chdir() failed", IndexError::FS_CANT_ACCESS, errno);

		execlp("/bin/sh", "sh", "-c", shell, (char*)0);
		exit(1);
	}
	else // parent
	{
		waitpid(pid, &status, 0);
		Termbox::ReOpen();
	}
}
