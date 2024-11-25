export module Event;

import Debug.Logger;
import std;

/*!
 * @brief The event manager class which provides event registration, connection and emitting functionality. Here is an example:
 * @code
 * EventManager::registerEvent("Foo");
 * EventManager::connect("Foo", []() {
 *    Logger::info("Foo event emitted.");
 * });
 * EventManager::emit("Foo");
 * @endcode
 */
export class EventManager {
public:
    /*!
     * @brief Register the signal with the given name.
     * @param name The name of the signal to be registered.
     */
    static auto registerEvent(const std::string& name) -> void {
        _signals[name] = std::vector<std::function<void()>>();
    }

    /*!
     * @brief Connect the signal with the slot.
     * @tparam Slot The function type of the slot.
     * @param name The name of the signal.
     * @param slot The function to handle the signal.
     */
    template<typename Slot>
    static auto connect(const std::string& name, Slot&& slot) -> void {
        if (const auto signal = _signals.find(name); signal == _signals.end()) {
            Logger::error("Signal " + name + " is not registered.");
        } else {
            signal->second.push_back(std::forward<Slot>(slot));
        }
    }

    /**
     * @brief If the signal is registered, emit the signal with the given arguments.
     * @tparam Args The types of specified arguments sent to the connected slots.
     * @param name The name of the signal.
     * @param args Specified arguments sent to the connected slots.
     * @code
     * // The signal must have been registered before emitting
     * // You can pass any number of arguments, but the types must match the connected slots
     * EventManager::registerEvent("Event1", "1");
     * EventManager::registerEvent("Event2", 1, 2, 3);
     * @endcode
     */
    template<typename... Args>
    static auto emit(const std::string& name, Args... args) -> void {
        if (const auto signal = _signals.find(name); signal == _signals.end()) {
            Logger::error("Signal " + name + " is not registered.");
        } else {
            for (const auto& slot: signal->second) {
                slot(args...);
            }
        }
    }

private:
    static std::map<std::string, std::vector<std::function<void()>>> _signals;
};

std::map<std::string, std::vector<std::function<void()>>> EventManager::_signals;
