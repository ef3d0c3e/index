#ifndef INDEX_ACTIONS_OPEN_HPP
#define INDEX_ACTIONS_OPEN_HPP

#include "../FileType.hpp"

namespace Actions
{
	MAKE_CENUM_Q(OpenType, std::uint8_t,
		Executable, 0, // Will execute the string as a shell command
		CustomOpen, 1, // Will prompt for a program, with the format "/usr/bin/shell -c '{0} {path + filename}'" where 0 is the string entered by the user
	);
	struct Opener
	{
		std::string format;
		MAKE_CENUMV_Q(Flag, std::uint8_t,
			SetSid, 1<<0, // The opener will be started as a child process and setsid() will be called (i.e it will not block the parent process and will become indemendent from the parent process)
			Close, 1<<1, // The opener requires index to close (e.g a terminal program like vi or emacs)
			SupressOutput, 1<<2, // The opener will have its stdout and stderr redirected to /dev/null
		);
		Flag flag;

		Opener(const std::string& _format, Flag _flag):
			format(_format), flag(_flag)
		{ }

		Opener(std::string&& _format, Flag _flag):
			format(std::move(_format)), flag(_flag)
		{ }

		std::string GetCommand(const String& fname, const std::string& path) const;
	};

	using namespace std::literals;
	struct op
	{
		const std::string_view ext;
		const Opener* open;

		constexpr op(std::string_view&& _ext, const Opener* _open):
			ext(std::move(_ext)), open(_open)
		{ }
	};

	namespace Openers
	{
		const static Opener TextEditor("nvim {file}"s, Opener::Close);
		const static Opener ImageViewer("feh {file}"s, Opener::SetSid | Opener::SupressOutput);
		const static Opener AudioPlayer("mpv --no-video {file}"s, Opener::Close);
		const static Opener VideoPlayer("mpv {file}"s, Opener::SetSid | Opener::SupressOutput);
		const static Opener PDFViewer("zathura {file}"s, Opener::SetSid | Opener::SupressOutput);
		const static Opener WebBrowser("x-www-browser {file}"s, Opener::SetSid | Opener::SupressOutput);

		const static Opener Gimp("gimp {file}"s, Opener::SetSid);

		bool isText(const File& f, const std::string& path, const std::string& ext);

		// Text file detection is handled by isText
		const static std::map<std::string_view, const Opener*> List =
		{
			// Images
			{"png"sv,  &ImageViewer},
			{"jpg"sv,  &ImageViewer},
			{"jpeg"sv, &ImageViewer},
			{"gif"sv,  &ImageViewer},
			{"bmp"sv,  &ImageViewer},
			{"tga"sv,  &ImageViewer},
			{"ppm"sv,  &ImageViewer},
			{"pgm"sv,  &ImageViewer},
			{"pbm"sv,  &ImageViewer},
			{"pnm"sv,  &ImageViewer},
			{"ico"sv,  &ImageViewer},

			// Audio
			{"wav"sv,  &AudioPlayer},
			{"flac"sv, &AudioPlayer},
			{"tta"sv,  &AudioPlayer},
			{"ape"sv,  &AudioPlayer},
			{"ogg"sv,  &AudioPlayer},
			{"mp3"sv,  &AudioPlayer},
			{"opus"sv, &AudioPlayer},
			{"ac3"sv,  &AudioPlayer},
			{"aac"sv,  &AudioPlayer},

			// Video
			{"mkv"sv,  &VideoPlayer},
			{"avi"sv,  &VideoPlayer},
			{"mp4"sv,  &VideoPlayer},
			{"h264"sv, &VideoPlayer},
			{"h265"sv, &VideoPlayer},
			{"vp8"sv,  &VideoPlayer},
			{"vp9"sv,  &VideoPlayer},
			{"webm"sv, &VideoPlayer},

			// PDF
			{"pdf"sv, &PDFViewer},

			// Web browser
			{"html"sv, &WebBrowser},
			{"htm"sv,  &WebBrowser},
			{"php"sv,  &WebBrowser},

			// Custom
			{"xcf"sv, &Gimp},
		};
	}

	std::pair<OpenType, const Opener*> GetOpener(const File& f, const std::string& path);

	////////////////////////////////////////////////
	/// \brief Attempts to open a file
	/// \param fname The name of the file to open
	/// \param path The path of the file
	/// \param format The format string for the opener
	/// \note May throw
	////////////////////////////////////////////////
	void Open(const String& fname, const std::string& path, const Opener& opener);

	////////////////////////////////////////////////
	/// \brief Attempts to open a file
	/// \param fname The name of the file to open
	/// \param path The path of the file
	/// \param format The format string for the opener
	/// \note May throw
	////////////////////////////////////////////////
	void CustomOpen(const String& fname, const std::string& path, const std::string& format);

	////////////////////////////////////////////////
	/// \brief Open a shell
	/// \param path The path to open the shell in
	/// \note May throw
	////////////////////////////////////////////////
	void OpenShell(const std::string& path);
}

#endif // INDEX_ACTIONS_OPEN_HPP
