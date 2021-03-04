#include "FileSystem.hpp"
#include "Sort.hpp"
#include "Settings.hpp"
#include <dirent.h>
#include <wordexp.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>

Permission& operator|=(Permission& l, const Permission& r)
{
	l.value |= r.value;
	return l;
}

Directory::Directory(const std::string& path):
	m_path(path)
{
	for (std::size_t i = 0; i < m_path.size(); ++i)
	{
		if (m_path[i] == ' ')
		{
			m_path.insert(i, 1, '\\');
			++i;
		}
	}

	wordexp_t p;
	char** w;
	if (wordexp(m_path.c_str(), &p, 0) != 0)
		throw Util::Exception("wordexp() failed");
	w = p.we_wordv;
	std::string expanded = "";
	for (std::size_t i = 0; i < p.we_wordc; ++i)
		expanded += w[i];
	wordfree(&p);

	/*
	char* resolved = realpath(expanded.c_str(), NULL);
	if (resolved == NULL)
		throw Util::Exception("realpath() failed");
		*/

	m_pathResolvedUnscaped = GetWd(expanded);
	std::remove_if(m_pathResolvedUnscaped.begin(), m_pathResolvedUnscaped.end(), [](char c){ return c == '\\'; });
	
	//free(resolved);
}

Directory::~Directory()
{
	
}

std::pair<bool, Error> Directory::GetFiles()
{
	auto Populate = [&](File& f, const std::string& name)
	{
		struct stat64 sb;
		if (lstat64(name.c_str(), &sb) == -1)
			throw Util::Exception("lstat64() failed");

		// Mode
		switch (sb.st_mode & S_IFMT)
		{
			case S_IFDIR:  f.mode = Mode::DIR; break;
			case S_IFCHR:  f.mode = Mode::CHR; break;
			case S_IFBLK:  f.mode = Mode::BLK; break;
			case S_IFREG:  f.mode = Mode::REG; break;
			case S_IFIFO:  f.mode = Mode::FIFO; break;
			case S_IFLNK:  f.mode = Mode::LNK; break;
			case S_IFSOCK: f.mode = Mode::SOCK; break;
			default:       f.mode = Mode::UNK; break;
		}

		// Resolving
		if constexpr (Settings::directory_resolve_lnk)
		{
			if (f.mode == Mode::LNK) [[unlikely]]
			{
				const std::size_t bufsiz = sb.st_size+1;
				char* buf = new char[bufsiz];
				if (readlink(name.c_str(), buf, bufsiz-1) == -1)
				{
					f.lnk.link = "";
					f.lnk.name = U"";
				}
				else
				{
					buf[bufsiz-1] = 0;

					f.lnk.link = buf;
					f.lnk.name = Util::StringConvert<Char>(std::string(buf));

					struct stat64 sb2;
					if (stat64(buf, &sb2) != -1)
					{
						switch (sb2.st_mode & S_IFMT)
						{
							case S_IFDIR:  f.lnk.mode = Mode::DIR; break;
							case S_IFCHR:  f.lnk.mode = Mode::CHR; break;
							case S_IFBLK:  f.lnk.mode = Mode::BLK; break;
							case S_IFREG:  f.lnk.mode = Mode::REG; break;
							case S_IFIFO:  f.lnk.mode = Mode::FIFO; break;
							case S_IFLNK:  f.lnk.mode = Mode::LNK; break;
							case S_IFSOCK: f.lnk.mode = Mode::SOCK; break;
							default:	   f.lnk.mode = Mode::UNK; break;
						}
					}
					else
						f.lnk.mode = Mode::UNK;
				}
				delete[] buf;
			}
		}

		// Size
		f.sz = sb.st_size;

		// Permissions
		f.perm = Permission::NONE;
		if (sb.st_mode & S_IREAD)
			f.perm |= Permission::UR;
		if (sb.st_mode & S_IWRITE)
			f.perm |= Permission::UW;
		if (sb.st_mode & S_IEXEC)
			f.perm |= Permission::UX;
		if (sb.st_mode & S_IRGRP)
			f.perm |= Permission::GR;
		if (sb.st_mode & S_IWGRP)
			f.perm |= Permission::GW;
		if (sb.st_mode & S_IXGRP)
			f.perm |= Permission::GX;
		if (sb.st_mode & S_IROTH)
			f.perm |= Permission::OR;
		if (sb.st_mode & S_IWOTH)
			f.perm |= Permission::OW;
		if (sb.st_mode & S_IXOTH)
			f.perm |= Permission::OX;

		// Owner/Group
		f.owner = sb.st_uid;
		f.group = sb.st_gid;

		// Time
		f.lastAccess = sb.st_atim.tv_sec;
		f.lastModification = sb.st_mtim.tv_sec;

		// Filetype
		f.ftId = FileType::GetFtID(f, name);
		
		// Mark
		f.mark = MarkType::NONE;
	};

	// Chdir
	if (chdir(m_pathResolvedUnscaped.c_str()) == -1)
		return {true, errno};

	m_files.clear();
	m_files.reserve(Settings::directory_block_size);

	DIR* dir = opendir(m_pathResolvedUnscaped.c_str());
	if (dir == NULL)
		return {true, errno};

	dirent64* pent = nullptr;
	std::size_t i = 0;

	while ((pent = readdir64(dir)))
	{
		if (pent == NULL)
			throw Util::Exception("readdir64() returned NULL");
		if (i % Settings::directory_block_size && i != 0)
			m_files.reserve(i+Settings::directory_block_size);

		if (strcmp(pent->d_name, ".") == 0 || strcmp(pent->d_name, "..") == 0) { }
		else if (pent->d_name[0] == '.' && !m_settings.HiddenFiles) {}
		else
		{
			const std::string name = m_pathResolvedUnscaped + "/" + pent->d_name;
			File f;
			f.name = Util::StringConvert<Char>(std::string(pent->d_name));
			Populate(f, m_pathResolvedUnscaped + "/" + pent->d_name);

			m_files.push_back(f);
			++i;
		}
	}

	closedir(dir);

	return {false, errno};
}

const File& Directory::operator[](std::size_t i) const
{
	return m_files[i];
}

File& Directory::operator[](std::size_t i)
{
	return m_files[i];
}

const std::size_t Directory::Size() const
{
	return m_files.size();
}

void Directory::SetSettings(const Directory::DirectorySettings& settings)
{
	m_settings = settings;
}

const Directory::DirectorySettings& Directory::GetSettings() const
{
	return m_settings;
}

void Directory::Sort()
{
	using namespace std::placeholders;
	std::sort(m_files.begin(), m_files.end(), std::bind(SortFns[m_settings.SortSettings.SortFn].first, _1, _2, m_settings.SortSettings));
}

std::string Directory::GetPath() const
{
	return m_pathResolvedUnscaped;
}

std::string Directory::GetFolderName() const
{
	const auto pos = m_pathResolvedUnscaped.rfind('/');
	if (pos == std::string::npos)
		return m_pathResolvedUnscaped.substr(1);
	
	return m_pathResolvedUnscaped.substr(pos+1);
}

std::size_t Directory::Find(const String& name, Mode mode) const
{
	for (std::size_t i = 0; i < m_files.size(); ++i)
	{
		const File& f = m_files[i];

		if (f.mode != mode)
			continue;
		if (f.name != name)
			continue;

		return i;
	}

	return static_cast<std::size_t>(-1);
}

std::string GetWd(const std::string& path)
{
	if (chdir(path.c_str()) == -1)
		throw Util::Exception("chdir() failed");

	char *dir = get_current_dir_name();
	if (dir == NULL)
		throw Util::Exception("get_current_dir_name() failed");
	
	const std::string ret = dir;
	free(dir);
	return ret;
}
