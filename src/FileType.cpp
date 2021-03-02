#include "FileType.hpp"
#include "FileSystem.hpp"

MAKE_CENUM_Q(GeneralID, FtID,
		UNKNOWN, 0,
		DIRECTORY, 1,
		FILE, 2,
);

FtID FileType::GetFtID(const File& f, const std::string& path)
{
	//TODO: Do something with links...

	if (f.mode == Mode::DIR || f.lnk.mode == Mode::DIR)
	{
		// Fullname
		auto it = FileType::FullnameDirMap.find(f.name);
		if (it != FileType::FullnameDirMap.end())
			return it->second;

		return GeneralID::DIRECTORY;
	}
	else if (f.mode == Mode::REG || f.lnk.mode == Mode::REG)
	{
		// Fullname
		auto it = FileType::FullnameFilesMap.find(f.name);
		if (it != FileType::FullnameFilesMap.end())
			return it->second;
		
		// Extension
		const auto pos = f.name.rfind(U'.');
		if (pos != String::npos)
		{
			const String ext = f.name.substr(pos+1); // Will return npos if no match
			auto it = FileType::ExtensionMap.find(ext);
			if (it != FileType::ExtensionMap.end())
				return it->second;
		}

		// Default
		return GeneralID::FILE;
	}

	return GeneralID::UNKNOWN;
}
