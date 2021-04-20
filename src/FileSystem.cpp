#include "FileSystem.hpp"
#include "Settings.hpp"
#include <dirent.h>
#include <wordexp.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <fmt/format.h>
#include "MainWindow.hpp"

Permission& operator|=(Permission& l, const Permission& r)
{
	l.value |= r.value;
	return l;
}

Directory::Directory(const std::string& path):
	m_path(path)
{
	m_pathResolvedUnscaped = GetUsablePath(path);

	GetFiles();
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
					f.lnk.name = Util::StringConvert<Char>(f.lnk.link);

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
							default:       f.lnk.mode = Mode::UNK; break;
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

	m_oFiles.clear();
	m_oFiles.reserve(Settings::directory_block_size);

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
			m_oFiles.reserve(i+Settings::directory_block_size);

		if (strcmp(pent->d_name, ".") != 0 && strcmp(pent->d_name, "..") != 0)
		{
			const std::string name = m_pathResolvedUnscaped + "/" + pent->d_name;
			File f;
			f.name = Util::StringConvert<Char>(std::string(pent->d_name));
			Populate(f, m_pathResolvedUnscaped + "/" + pent->d_name);

			m_oFiles.push_back(f);
			++i;
		}
	}

	closedir(dir);

	return {false, errno};
}

const File& Directory::operator[](std::size_t i) const
{
	return m_oFiles[i];
}

File& Directory::operator[](std::size_t i)
{
	return m_oFiles[i];
}

const std::size_t Directory::Size() const
{
	return m_oFiles.size();
}

const std::string& Directory::GetPath() const
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

std::size_t Directory::Find(const String& name, Mode mode, std::size_t beg) const
{
	for (std::size_t i = beg; i < m_oFiles.size(); ++i)
	{
		const File& f = m_oFiles[i];

		if (f.mode != mode)
			continue;
		if (f.name != name)
			continue;

		return i;
	}

	return static_cast<std::size_t>(-1);
}

void Directory::Rename(const String& oldName, const String& newName)
{
	if (newName.find('/') != std::string::npos) [[unlikely]]
		throw IndexError(U"Ill-formed name. Name contains a '/', which is not allowed!", IndexError::GENERIC_ERROR);

	// Find position in current representation
	std::size_t pos = static_cast<std::size_t>(-1);
	for (std::size_t i = 0; i < m_oFiles.size(); ++i)
	{
		if (m_oFiles[i].name == oldName)
		{
			pos = i;
			break;
		}
	}

	const int err = rename(Util::StringConvert<char>(oldName).c_str(), Util::StringConvert<char>(newName).c_str());
	if (err == -1)
	{
		// In theory, all error should concern newName only, as the user cannot choose oldName
		const String fromTo = fmt::format(Settings::Layout::error_rename_from_to, oldName, newName);
		switch (errno)
		{
			case EACCES:
			case EPERM:
				throw IndexError(U"Cannot rename " + fromTo + U": permission denied", IndexError::GENERIC_ERROR);
				break;
			case EBUSY:
				throw IndexError(U"Cannot rename " + fromTo + U": path busy", IndexError::GENERIC_ERROR);
				break;
			case EDQUOT:
				throw IndexError(U"Cannot rename " + fromTo + U": user's quota of disk blocks has been exhausted", IndexError::GENERIC_ERROR);
				break;
			case EFAULT:
				throw IndexError(U"Cannot rename " + fromTo + U": path not in addressable space", IndexError::GENERIC_ERROR);
				break;
			case EINVAL:
				throw IndexError(U"Cannot rename " + fromTo + U": you attempted to make a subdirectory from itself", IndexError::GENERIC_ERROR);
				break;
			case EISDIR:
				throw IndexError(U"Cannot rename " + fromTo + U": not a directory", IndexError::GENERIC_ERROR);
				break;
			case ELOOP:
				throw IndexError(U"Cannot rename " + fromTo + U": to many symlink", IndexError::GENERIC_ERROR);
				break;
			case EMLINK:
				throw IndexError(U"Cannot rename " + fromTo + U": the previous path has already too many links", IndexError::GENERIC_ERROR);
				break;
			case ENAMETOOLONG:
				throw IndexError(U"Cannot rename " + fromTo + U": name is too long", IndexError::GENERIC_ERROR);
				break;
			case ENOENT:
				throw IndexError(U"Cannot rename " + fromTo + U": path does not exists", IndexError::GENERIC_ERROR);
				break;
			case ENOMEM:
				throw IndexError(U"Cannot rename " + fromTo + U": not enough kernel memory available", IndexError::GENERIC_ERROR);
				break;
			case ENOSPC:
				throw IndexError(U"Cannot rename " + fromTo + U": the device has no room for a new directory", IndexError::GENERIC_ERROR);
				break;
			case ENOTDIR:
				throw IndexError(U"Cannot rename " + fromTo + U": a component of the new is not a directory", IndexError::GENERIC_ERROR);
				break;
			case ENOTEMPTY:
			case EEXIST:
				throw IndexError(U"Cannot rename " + fromTo + U": the path is a non empty directory", IndexError::GENERIC_ERROR);
				break;
			case EROFS:
				throw IndexError(U"Cannot rename " + fromTo + U": filesystem is read-only", IndexError::GENERIC_ERROR);
				break;
			case EXDEV:
				throw IndexError(U"Cannot rename " + fromTo + U": the new path is not on the same directory as the original file", IndexError::GENERIC_ERROR);
				break;
			default:
				throw IndexError(U"Cannot rename " + fromTo, IndexError::GENERIC_ERROR);
				break;
		}
	}

	if (pos != static_cast<std::size_t>(-1)) [[likely]]
	{
		m_oFiles[pos].name = newName;
		m_oFiles[pos].ftId = FileType::GetFtID(m_oFiles[pos], GetPath());
	}
}

std::string GetWd(const std::string& path)
{
	if (chdir(path.c_str()) == -1)
		throw IndexError(U"chdir() failed", IndexError::GENERIC_ERROR, errno);

	char *dir = get_current_dir_name();
	if (dir == NULL)
		throw IndexError(U"get_current_dir_name() failed", IndexError::GENERIC_ERROR, errno);
	
	const std::string ret = dir;
	free(dir);
	return ret;
}

std::string GetUsablePath(std::string path)
{

	for (std::size_t i = 0; i < path.size(); ++i)
	{
		if (path[i] == ' ')
		{
			path.insert(i, 1, '\\');
			++i;
		}
	}

	wordexp_t p;
	char** w;
	if (wordexp(path.c_str(), &p, 0) != 0)
		throw IndexError(U"wordexp() failed", IndexError::GENERIC_ERROR, errno);
	w = p.we_wordv;
	std::string expanded = "";
	for (std::size_t i = 0; i < p.we_wordc; ++i)
		expanded += w[i];
	wordfree(&p);

	std::string resolved = GetWd(expanded);
	std::remove_if(resolved.begin(), resolved.end(), [](char c){ return c == '\\'; });

	return resolved;
}

void ChangeDir(const std::string& path)
{
	if (chdir(path.c_str()) == -1)
	{
		String reason;
		switch (errno)
		{
			case EACCES:
				reason = U"no permission";
				break;
			case EFAULT:
				reason = U"path is outside accessible address space";
				break;
			case EIO:
				reason = U"an i/o error occured";
				break;
			case ELOOP:
				reason = U"too many symlink while resolving the path";
				break;
			case ENAMETOOLONG:
				reason = U"the path is too long";
				break;
			case ENOENT:
				reason = U"the path does not exists";
				break;
			case ENOTDIR:
				reason = U"the path is not a directory";
				break;
			default:
				reason = U"";
		}
		throw IndexError(U"Could not change workind directory '" + Util::StringConvert<Char>(path) + U"' : " + reason, IndexError::GENERIC_ERROR, errno);
	}
}
