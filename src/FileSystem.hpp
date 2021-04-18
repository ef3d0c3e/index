#ifndef INDEX_FILESYSTEM_HPP
#define INDEX_FILESYSTEM_HPP

#include "FileType.hpp"
class MainWindow;

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

////////////////////////////////////////////////
/// \brief Represents a match between a file and a filter
/// Currently there are only two possible match type: FILTER and FIND
////////////////////////////////////////////////
struct FileMatch
{
	MAKE_CENUMV_Q(MatchType, std::uint8_t,
		FILTER, 0,
		FIND, 1, // TODO...
	);
	std::vector<std::tuple<MatchType, std::size_t, std::size_t>> matches; // <pos, size>

	FileMatch() {}
};

typedef std::function<bool(const File&, const File&, const Sort::Settings&)> SortFn;

////////////////////////////////////////////////
/// \brief Represents a directory
////////////////////////////////////////////////
class Directory
{
	std::string m_path; // Original path
	std::string m_pathResolvedUnscaped; // Resolved

	std::vector <File> m_oFiles; // All the files (unfiltered)
	std::vector<std::pair<File*, FileMatch>> m_files; // The filtered list

public:
	struct DirectorySettings
	{
		Sort::Settings SortSettings{};
	};

	struct DirectoryFilter
	{
		// Do something about passing it to parent/child
		bool HiddenFiles = false; // false -> no hidden files
		String Match = U"";
	};
private:
	DirectorySettings m_settings;
	DirectoryFilter m_filter;
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
	/// \brief Get a file from the directory
	/// \param i The index of the file
	/// \returns The file at index i and its match information
	/// \warn Performs no bound checks
	////////////////////////////////////////////////
	const std::pair<const File&, const FileMatch&> Get(std::size_t i) const;
	std::pair<File&, FileMatch&> Get(std::size_t i);

	////////////////////////////////////////////////
	/// \brief Get the number of files in the directory
	/// \returns The number of files
	////////////////////////////////////////////////
	const std::size_t Size() const;


	////////////////////////////////////////////////
	/// \brief Get the number of files in the directory to be display
	/// \returns The number of files to be displayed
	////////////////////////////////////////////////
	const std::size_t SizeD() const;

	////////////////////////////////////////////////
	/// \brief Filter oFiles and store the passing files in files
	////////////////////////////////////////////////
	void Filter();

	////////////////////////////////////////////////
	/// \brief Sort the files in the directory
	////////////////////////////////////////////////
	void Sort();

	////////////////////////////////////////////////
	/// \brief Change the settings
	/// \param s The new settings
	////////////////////////////////////////////////
	void SetSettings(const DirectorySettings& s);

	////////////////////////////////////////////////
	/// \brief Get the settings
	/// \returns The current settings
	////////////////////////////////////////////////
	const DirectorySettings& GetSettings() const;

	////////////////////////////////////////////////
	/// \brief Change the filter
	/// \param s The new filter
	////////////////////////////////////////////////
	void SetFilter(const DirectoryFilter& f);

	////////////////////////////////////////////////
	/// \brief Get the filter
	/// \returns The current filter
	////////////////////////////////////////////////
	const DirectoryFilter& GetFilter() const;

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
	std::size_t Find(const String& name, Mode mode, std::size_t beg = 0) const;

	////////////////////////////////////////////////
	/// \brief Get the first element matching the query in the display list
	/// \returns The id of the first element that matched (else -1)
	////////////////////////////////////////////////
	std::size_t FindD(const String& name, Mode mode, std::size_t beg = 0) const;

	////////////////////////////////////////////////
	/// \brief Renames a path
	/// \param main The main window
	/// \param oldName The current path
	/// \param newName what to rename it to
	////////////////////////////////////////////////
	void Rename(MainWindow* main, const std::string& oldName, const std::string& newName);
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

#endif // INDEX_FILESYSTEM_HPP
