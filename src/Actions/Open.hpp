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
		std::string_view format;
		MAKE_CENUMV_Q(Flag, std::uint8_t,
			SetSid, 1<<0, // The opener will be started as a child process and setsid() will be called (i.e it will not block the parent process and will become indemendent from the parent process)
			Close, 1<<1, // The opener requires index to close (e.g a terminal program like vi or emacs)
			SupressOutput, 1<<2, // The opener will have its stdout and stderr redirected to /dev/null
		);
		Flag flag;

		constexpr Opener(std::string_view&& _format, Flag _flag):
			format(std::move(_format)), flag(_flag)
		{ }

		std::string GetCommand(const File& f, const std::string& path) const;
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
		constexpr static Opener TextEditor("nvim {file}"sv, Opener::Close);
		constexpr static Opener ImageViewer("feh {file}"sv, Opener::SetSid);
		constexpr static Opener AudioPlayer("mpv --no-video {file}"sv, Opener::Close);
		constexpr static Opener VideoPlayer("mpv {file}"sv, Opener::SetSid);
		constexpr static Opener PDFViewer("zathura {file}"sv, Opener::SetSid);
		constexpr static Opener WebBrowser("x-www-browser {file}"sv, Opener::SetSid);

		constexpr static Opener Gimp("gimp {file}"sv, Opener::SetSid);

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

	void Open(const File& f, const std::string& path, const Opener* opener);
}

#endif // INDEX_ACTIONS_OPEN_HPP
