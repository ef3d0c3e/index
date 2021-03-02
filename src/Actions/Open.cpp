#include "Open.hpp"
#include <fmt/format.h>
#include "../FileSystem.hpp"

std::string Actions::Opener::GetCommand(const File& f, const std::string& path)
{
	return fmt::format(format, fmt::arg("file", Util::StringConvert<char>(f.name)), fmt::arg("path", path));
}

bool Actions::Openers::isText(const File& f, const std::string& path, const std::string& ext)
{
	if (f.mode != Mode::REG && f.lnk.mode != Mode::REG)
		return false;

	const auto pos = f.name.rfind(U'.');
	if (pos != String::npos)
	{
		const std::string ext = Util::StringConvert<char>(f.name.substr(pos+1));
		if (ext == "out")
			return false;
	}

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
			return {Actions::OpenType::Executable, &Actions::Openers::TextEditor};
	}

	return {Actions::OpenType::CustomOpen, nullptr};
}
