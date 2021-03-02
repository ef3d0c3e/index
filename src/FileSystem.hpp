#ifndef INDEX_FILESYSTEM_HPP
#define INDEX_FILESYSTEM_HPP

#include "TermboxWidgets/Util.hpp"
#include "FileType.hpp"

typedef decltype(errno) Error;

namespace Sort
{
struct Settings
{
	bool DirFist = true;
	bool CaseSensitive = false;
	std::size_t SortFn = 0;
};
}

MAKE_CENUM_Q(Mode, std::uint8_t,
	UNK,  0,
	NONE, 1<<0,
	DIR,  1<<1,
	CHR,  1<<2,
	BLK,  1<<3,
	REG,  1<<4,
	FIFO, 1<<5,
	LNK,  1<<6,
	SOCK, 1<<7,
);

MAKE_CENUM_Q(Permission, std::uint16_t,
	NONE, 0,
	UR,   1<<0,
	UW,   1<<1,
	UX,   1<<2,
	GR,   1<<3,
	GW,   1<<4,
	GX,   1<<5,
	OR,   1<<6,
	OW,   1<<7,
	OX,   1<<8,
);

MAKE_CENUM_Q(MarkType, std::uint8_t,
	NONE,     0,
	SELECTED, 1 << 0,
	TAGGED,   1 << 1,
	FAV,      1 << 2,
);

typedef uid_t UserID;
typedef gid_t GroupID;
typedef off64_t Size;
typedef time_t Time;

////////////////////////////////////////////////
/// \brief Represents a single file for drawing
////////////////////////////////////////////////
struct File
{
	String name;

	Mode mode;
	Size sz;
	
	Permission perm;
	UserID owner;
	GroupID group;

	Time lastAccess;
	Time lastModification;

	struct
	{
		std::string link;
		String name;
		Mode mode;
	} lnk;

	FtID ftId;

	MarkType mark;
};

typedef std::function<bool(const File&, const File&, const Sort::Settings&)> SortFn;

////////////////////////////////////////////////
/// \brief Represents a directory
////////////////////////////////////////////////
class Directory
{
	std::string m_path; // Original path
	std::string m_pathResolvedUnscaped; // Resolved

	std::vector<File> m_files;

public:
	struct DirectorySettings
	{
		bool HiddenFiles = false;

		Sort::Settings SortSettings{};
	};
private:
	DirectorySettings m_settings;
public:

	Directory(const std::string& path);
	~Directory();

	////////////////////////////////////////////////
	/// \brief Get the files from the folder
	////////////////////////////////////////////////
	std::pair<bool, Error> GetFiles();

	////////////////////////////////////////////////
	/// \brief Get a file from the directory
	/// \param i The index of the file
	/// \returns The file at index i
	/// \warn Performs no bound checks
	////////////////////////////////////////////////
	const File& operator[](std::size_t i) const;
	File& operator[](std::size_t i);

	////////////////////////////////////////////////
	/// \brief Get the number of files in the directory
	/// \returns The number of files
	////////////////////////////////////////////////
	const std::size_t Size() const;

	////////////////////////////////////////////////
	/// \brief Sort the files in the directory
	/// \param fn The sort function
	/// \param settings The sort settings
	////////////////////////////////////////////////
	void Sort();

	////////////////////////////////////////////////
	/// \brief Change the settings
	/// \param s The new settings
	////////////////////////////////////////////////
	void SetSettings(const DirectorySettings& s);

	////////////////////////////////////////////////
	/// \brief Get the path of the directory
	/// \returns The path
	////////////////////////////////////////////////
	std::string GetPath() const;

	////////////////////////////////////////////////
	/// \brief Get the name of the folder
	/// \returns The name
	////////////////////////////////////////////////
	std::string GetFolderName() const;

	////////////////////////////////////////////////
	/// \brief Get the first element matching the query
	/// \returns The id of the first element that matched (else -1)
	////////////////////////////////////////////////
	std::size_t Find(const String& name, Mode mode) const;
};

std::string GetWd(const std::string& path);

#endif // INDEX_FILESYSTEM_HPP
