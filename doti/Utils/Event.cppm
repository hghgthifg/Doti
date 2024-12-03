export module Utils.Event;

import Debug.Logger;
import std;

// Concept to ensure that the slot function is invocable with the specified arguments and returns void
template<typename Slot, typename... Args>
concept SlotCompatible = std::is_invocable_r_v<void, Slot, Args...>;

// Signal class managing connected slots
template<typename... Args>
class Signal {
public:
    using SlotType = std::function<void(Args...)>;

    /**
     * @brief Connect a new slot to the signal.
     * @param slot The slot function to be connected.
     */
    void connect(SlotType slot) {
        _slots.emplace_back(std::move(slot));
    }

    /**
     * @brief Disconnect all slots from the signal.
     */
    void disconnectAll() {
        _slots.clear();
    }

    /**
     * @brief Emit the signal, invoking all connected slots with the provided arguments.
     * @param args Arguments to pass to the connected slots.
     */
    void emitSignal(Args... args) const {
        for (const auto& slot: _slots) {
            slot(args...);
        }
    }

private:
    std::vector<SlotType> _slots; /*!< Container for connected slots */
};

// EventManager class handling event registration, connection, emission, and disconnection
export class EventManager {
public:
    /**
     * @brief Register a new event with specific argument types.
     * @tparam Args The types of arguments that the event will use.
     * @param name The name of the event to register.
     */
    template<typename... Args>
    static void registerEvent(const std::string& name) {
        if (!_signals.contains(name)) {
            _signals[name] = std::make_shared<Signal<Args...>>();
            Logger::event("Event \"" + name + "\" registered. ");
        } else {
            Logger::warning("Event \"" + name + "\" is already registered. ");
        }
    }

    /**
     * @brief Connect a slot to an event with matching argument types.
     * @tparam Args The types of arguments for the event.
     * @tparam Slot The type of the slot function.
     * @param name The name of the event to connect to.
     * @param slot The slot function to be connected.
     */
    template<typename... Args, typename Slot>
        requires SlotCompatible<Slot, Args...>
    static void connect(const std::string& name, Slot slot) {
        const auto it = _signals.find(name);
        if (it != _signals.end()) {
            auto signal = std::static_pointer_cast<Signal<Args...>>(it->second);
            signal->connect(std::function<void(Args...)>(slot));
            Logger::event("Slot connected to event \"" + name + "\". ");
        } else {
            Logger::error("Event \"" + name + "\" is not registered. ");
        }
    }

    /**
     * @brief Emit the specified event, invoking all connected slots with the provided arguments.
     * @tparam Args The types of arguments to send to the connected slots.
     * @param name The name of the event to emit.
     * @param args The arguments to pass to the connected slots.
     * @code
     * // The event must have been registered before emitting
     * // You can pass any number of arguments, but the types must match the connected slots
     * EventManager::registerEvent<int, std::string>("Event1");
     * EventManager::connect<int, std::string>("Event1", [](int a, const std::string& b) {
     *     // Slot implementation
     * });
     * EventManager::emit("Event1", 1, "Test");
     * @endcode
     */
    template<typename... Args>
    static void emit(const std::string& name, Args... args) {
        const auto it = _signals.find(name);
        if (it != _signals.end()) {
            auto signal = std::static_pointer_cast<Signal<Args...>>(it->second);
            signal->emitSignal(args...);
            // Logger::info("Event '" + name + "' emitted. ");
        } else {
            Logger::warning("Event \"" + name + "\" is not registered. ");
        }
    }

    /**
     * @brief Disconnect all slots from the specified event.
     * @param name The name of the event to disconnect all slots from.
     */
    template<typename... Args>
    static void disconnectAll(const std::string& name) {
        auto it = _signals.find(name);
        if (it != _signals.end()) {
            /* Reset the shared_ptr and erase the event from the map */
            auto signal = std::static_pointer_cast<Signal<Args...>>(it->second);
            signal->disconnectAll();
            // it->second.reset();
            // _signals.erase(it);
            Logger::event("All slots disconnected from event \"" + name + "\". ");
        } else {
            Logger::warning("Event \"" + name + "\" is not registered. ");
        }
    }

private:
    static std::unordered_map<std::string, std::shared_ptr<void>> _signals;
};

/* Initialize the static member */
std::unordered_map<std::string, std::shared_ptr<void>> EventManager::_signals;
