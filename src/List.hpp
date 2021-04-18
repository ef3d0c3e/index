#ifndef INDEX_LIST_HPP
#define INDEX_LIST_HPP

#include "MainWindow.hpp"
#include "Settings.hpp"

//typedef std::function<bool(const File&, const File&, const Sort::Settings&)> SortFn;

typedef Widgets::ListSelect<Settings::Layout::List::settings, MarkType> ListBase;
class List : public ListBase
{
	Directory* m_dir;
	MainWindow* m_main;
	bool m_mainList;

	std::pair<TBStyle, TBStyle> DrawFn(std::size_t i, Vec2i pos, int w, bool hovered, Char trailing) const;
public:
	struct DirectorySettings
	{
		Sort::Settings SortSettings{};
	};

	struct DirectoryFilter
	{
		bool HiddenFiles = false; // false -> no hidden files
		String Match = U"";
	};
private:
	DirectorySettings m_settings;
	DirectoryFilter m_filter;

	std::vector<std::pair<File*, FileMatch>> m_files; // Files

	////////////////////////////////////////////////
	/// \brief Action of leaving the current directory
	/// Will notify ```OnChangePosition```
	////////////////////////////////////////////////
	void ActionLeft();

	////////////////////////////////////////////////
	/// \brief Action of entering in a new directory or opening a file
	/// Will notify ```OnChangePosition``` only if entering a new directory
	////////////////////////////////////////////////
	void ActionRight();
public:
	////////////////////////////////////////////////
	/// \brief Constructor
	/// \param main The main window
	/// \param path The directory's path
	/// \param input Set to true if receives inputs
	/// \note If input is set to true, m_mainList will be set to true (a bit hacky)
	////////////////////////////////////////////////
	List(MainWindow* main, const std::string& path, bool input);
	~List();

	////////////////////////////////////////////////
	/// \brief Moves the files of l
	/// \param preservePos Attempts to set the position so that it corresponds to the same file
	////////////////////////////////////////////////
	void MoveFiles(List&& l, bool preservePos);

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
	/// \brief Gets the number of files to be displayed
	////////////////////////////////////////////////
	std::size_t Size() const;

	////////////////////////////////////////////////
	/// \brief Run filtering, sort and update the number of entries
	/// \param preservePos Attempts to set the position so that it corresponds to the same file
	////////////////////////////////////////////////
	void UpdateFromDir(bool preservePos = false);

	////////////////////////////////////////////////
	/// \brief Will get the informations from the filesystem
	/// Will call ```UpdateFromDir```
	/// \param preservePos Attempts to set the position so that it corresponds to the same file
	/// \see UpdateFromDir
	////////////////////////////////////////////////
	void UpdateFromFilesystem(bool preservePos = false);

	////////////////////////////////////////////////
	/// \brief Gets the Directory
	/// \returns The current directory
	/// \see Directory
	////////////////////////////////////////////////
	Directory* GetDir();

	////////////////////////////////////////////////
	/// \brief Sets the Directory
	/// \param dir The new directory
	/// \note If a directory already exists, you will have to manually deallocate it
	/// \note Does not perform any extra action, you will have to run the ```UpdateFromDir```
	/// \see Directory
	/// \see UpdateFromDir
	////////////////////////////////////////////////
	void SetDir(Directory* dir);

	////////////////////////////////////////////////
	/// \brief Marks/Unmarks a file
	/// \param i The index of the file
	/// \param mark The type of mark
	////////////////////////////////////////////////
	void MarkFn(std::size_t i, MarkType mark);

	////////////////////////////////////////////////
	/// \brief Get a file from the listing
	/// \param i The index of the file
	/// \returns The file at index i and its match information
	/// \warn Performs no bound checks
	////////////////////////////////////////////////
	const std::pair<const File&, const FileMatch&> Get(std::size_t i) const;
	std::pair<File&, FileMatch&> Get(std::size_t i);

	////////////////////////////////////////////////
	/// \brief Get the first element matching the query in the display list
	/// \param name The name of the file
	/// \param mode The file's mode
	/// \param beg Where the start the search from
	/// \returns The index of the file if found, std::size_t(-1) otherwise
	////////////////////////////////////////////////
	std::size_t Find(const String& name, Mode mode, std::size_t beg = 0) const;

	////////////////////////////////////////////////
	/// \brief Find a file's index
	/// \param name The name of the file to search for
	/// \returns The index of the file if found, std::size_t(-1) otherwise
	////////////////////////////////////////////////
	std::size_t FindByName(const String& name) const;
};

#endif // INDEX_LIST_HPP
