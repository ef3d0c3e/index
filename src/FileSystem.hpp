#ifndef INDEX_FILESYSTEM_HPP
#define INDEX_FILESYSTEM_HPP

#include "FileType.hpp"
#include "Exceptions.hpp"
class MainWindow;

typedef decltype(errno) Error;

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

////////////////////////////////////////////////
/// \brief Represents a directory
////////////////////////////////////////////////
class Directory
{
	std::string m_path; // Original path
	std::string m_pathResolvedUnscaped; // Resolved

	std::vector <File> m_oFiles; // All the files

public:

	////////////////////////////////////////////////
	/// \brief Constructor, constructs a directory given its path
	/// \param path The directory's path
	/// Performs some basic resolving
	////////////////////////////////////////////////
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
	/// \brief Get the path of the directory
	/// \returns The path
	////////////////////////////////////////////////
	const std::string& GetPath() const;

	////////////////////////////////////////////////
	/// \brief Get the name of the folder
	/// \returns The name
	////////////////////////////////////////////////
	std::string GetFolderName() const;

	////////////////////////////////////////////////
	/// \brief Get the first element matching the query
	/// \returns The id of the first element that matched (else -1)
	////////////////////////////////////////////////
	std::size_t Find(const String& name, Mode mode, std::size_t beg = 0) const;

	////////////////////////////////////////////////
	/// \brief Renames a path
	/// \param oldName The current path
	/// \param newName what to rename it to
	/// \note May throw
	/// This function is for 'high-level' renaming, i.e renaming a file in the current working directory without changing the file's underlying directory
	////////////////////////////////////////////////
	void Rename(const String& oldName, const String& newName);
};

////////////////////////////////////////////////
/// \brief Gets the working directory
/// \param path The directory's path
/// FIXME: This calls chdir(path) and returns get_current_dir_name
/// Doing this on a directory without read permission will crash
////////////////////////////////////////////////
std::string GetWd(const std::string& path);

////////////////////////////////////////////////
/// \brief Gets a usable path from a path
/// \param path The path
/// \returns A readable path
////////////////////////////////////////////////
std::string GetUsablePath(std::string path);

////////////////////////////////////////////////
/// \brief Changes the working directory
/// \param path The path of the new working directory
/// \note May throw
////////////////////////////////////////////////
void ChangeDir(const std::string& path);

#endif // INDEX_FILESYSTEM_HPP
