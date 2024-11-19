/*!
 * @brief A simple log tool.
 */

module;

export module Debug.Logger;
import std;

export class Logger {
public:
    static auto info(const std::string_view&     str,
                     const std::source_location& location = std::source_location::current()) -> void {
        const std::string_view file_name = location.file_name();
        std::println("{}{} {:<10} ({}:{}) {}", "\033[1;32m", getCurrentTime(), "[info]",
                     file_name.substr(file_name.find_last_of('/') + 1), location.line(), str);
    }

    static auto debug(const std::string_view&     str,
                      const std::source_location& location = std::source_location::current()) -> void {
        const std::string_view file_name = location.file_name();
        std::println("{}{} {:<10} ({}:{}) {}", "\033[1;35m", getCurrentTime(), "[debug]",
                     file_name.substr(file_name.find_last_of('/') + 1), location.line(), str);
    }

    static auto warning(const std::string_view&     str,
                        const std::source_location& location = std::source_location::current()) -> void {
        const std::string_view file_name = location.file_name();
        std::println("{}{} {:<10} ({}:{}) {}", "\033[1;93m", getCurrentTime(), "[warning]",
                     file_name.substr(file_name.find_last_of('/') + 1), location.line(), str);
    }

    static auto error(const std::string_view&     str,
                      const std::source_location& location = std::source_location::current()) -> void {
        const std::string_view file_name = location.file_name();
        std::println("{}{} {:<10} ({}:{}) {}", "\033[1;91m", getCurrentTime(), "[error]",
                     file_name.substr(file_name.find_last_of('/') + 1), location.line(), str);
    }

private:
    inline static auto getCurrentTime() -> std::string {
        std::chrono::zoned_time now{
            std::chrono::current_zone(), std::chrono::system_clock::now()
        };
        return std::format("{:%Y-%m-%d %H:%M:%S}", now);;
    }
};
