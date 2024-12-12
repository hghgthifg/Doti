export module Window.UI.Components.Console;

import std;
import ImGui;
import Debug.Logger;
import Window.UI.ComponentBase;

struct LogEntry {
    std::string time;
    std::string level;
    std::string source;
    std::string message;

    bool operator<(const LogEntry& rhs) const {
        return time < rhs.time;
    }
};

export class Console : public UIComponent {
public:
    Console(const std::string& name) : UIComponent(name) {}

    auto setLogBuffer(std::stringstream& ss) -> void {
        _logBuffer = &ss;
    }

    void beginRender() override {
        updateLogs();
        ImGui::Begin(getName().c_str(), &_visible);
    }

    void renderContent() override {
        const bool isWindowFocused = ImGui::IsWindowFocused(ImGuiFlags::ImGuiFocusedFlags::RootAndChildWindows);

        ImGuiFlags::ImGuiWindowFlags childFlags = isWindowFocused
                                                      ? ImGuiFlags::ImGuiWindowFlags::None
                                                      : ImGuiFlags::ImGuiWindowFlags::NoScrollWithMouse;

        ImGui::BeginTabBar("Logs");

        if (ImGui::BeginTabItem("all")) {
            ImGui::BeginChild("ScrollingRegionAll", ImVec2(0, -1), false, childFlags);
            for (const auto& log: _allLogs) {
                renderLogEntry(log);
            }
            ImGui::EndChild();
            ImGui::EndTabItem();
        }

        for (const auto& [level, logs]: _catagorizedLogs) {
            if (ImGui::BeginTabItem(level.c_str())) {
                ImGui::BeginChild(("ScrollingRegion" + level).c_str(), ImVec2(0, -1), false, childFlags);
                for (const auto& log: logs) {
                    renderLogEntry(log);
                }
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
        }

        ImGui::EndTabBar();
    }

    void endRender() override {
        ImGui::End();
    }

private:
    auto updateLogs() -> void {
        std::string tmp_str = _logBuffer->str();
        if (!tmp_str.empty()) {
            parseLogs(tmp_str);
            _logBuffer->str("");
            _logBuffer->clear();
        }
    }

    auto parseLogs(const std::string& logs) -> void {
        // format: time [level] (source) message
        std::regex log_pattern(
            R"(^(?:\x1B\[\d+(?:;\d+)*m)?(.*?) \[(\w+)\]\s+\(([^)]+)\) (.*))"
        );
        std::stringstream ss(logs);
        std::string       line;

        while (std::getline(ss, line)) {
            std::smatch matches;
            if (std::regex_search(line, matches, log_pattern)) {
                LogEntry entry{
                    matches[1].str(),
                    matches[2].str(),
                    matches[3].str(),
                    matches[4].str(),
                };
                _catagorizedLogs[entry.level].emplace_back(entry);
                _allLogs.emplace_back(entry);
            }
        }
    }

    void renderLogEntry(const LogEntry& log) {
        const std::string& output = std::format("{} [{}] ({}) {}", log.time, log.level, log.source, log.message);

        static const std::unordered_map<std::string, ImVec4> colors = {
            {"info", ImVec4(0.0f, 1.0f, 0.0f, 1.0f)},
            {"debug", ImVec4(1.0f, 0.0f, 1.0f, 1.0f)},
            {"warning", ImVec4(1.0f, 1.0f, 0.0f, 1.0f)},
            {"error", ImVec4(1.0f, 0.0f, 0.0f, 1.0f)},
            {"event", ImVec4(0.0f, 1.0f, 1.0f, 1.0f)}
        };

        const auto it = colors.find(log.level);
        ImGui::PushStyleColor(ImGuiStyles::ImGuiCol::ImGuiCol_Text,
                              it != colors.end() ? it->second : ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
        ImGui::TextUnformatted(output.data());
        ImGui::PopStyleColor();
    }

    std::stringstream*                           _logBuffer;
    std::map<std::string, std::vector<LogEntry>> _catagorizedLogs;
    std::vector<LogEntry>                        _allLogs;
};
