module;

export module Debug.Logger;
import std;

/*!
 * @brief A simple log tool.
 * @code
 * Logger::info("info message");
 * Logger::debug("debug message");
 * Logger::warning("warning message");
 * Logger::error("error message");
 * @endcode
 */
export class Logger {
public:
    static auto info(const std::string_view&     str,
                     const std::source_location& location = std::source_location::current()) -> void {
        const std::filesystem::path filePath = location.file_name();
        std::println("{}{} {:<10} ({}:{}) {}", "\033[1;32m", getCurrentTime(), "[info]",
                     filePath.filename().string(), location.line(), str);
    }

    static auto debug(const std::string_view&     str,
                      const std::source_location& location = std::source_location::current()) -> void {
        const std::filesystem::path filePath = location.file_name();
        std::println("{}{} {:<10} ({}:{}) {}", "\033[1;35m", getCurrentTime(), "[debug]",
                     filePath.filename().string(), location.line(), str);
    }

    static auto warning(const std::string_view&     str,
                        const std::source_location& location = std::source_location::current()) -> void {
        const std::filesystem::path filePath = location.file_name();
        std::println("{}{} {:<10} ({}:{}) {}", "\033[1;93m", getCurrentTime(), "[warning]",
                     filePath.filename().string(), location.line(), str);
    }

    static auto error(const std::string_view&     str,
                      const std::source_location& location = std::source_location::current()) -> void {
        const std::filesystem::path filePath = location.file_name();
        std::println("{}{} {:<10} ({}:{}) {}", "\033[1;91m", getCurrentTime(), "[error]",
                     filePath.filename().string(), location.line(), str);
    }

private:
    inline static auto getCurrentTime() -> std::string {
        std::chrono::zoned_time now{
            std::chrono::current_zone(), std::chrono::system_clock::now()
        };
        return std::format("{:%Y-%m-%d %H:%M:%S}", now);;
    }
};
