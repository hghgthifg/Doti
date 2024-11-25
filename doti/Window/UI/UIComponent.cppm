/*!
* @brief UI Component Base Module declaration
 */
export module Window.UI.ComponentBase;

import std;
/*!
 * @brief Component type concept constraint
 * @tparam T Type that must satisfy component interface
 * @requires T must have render() method returning void
 * @requires T must have getName() method convertible to string_view
 */
export template<typename T>
concept ComponentType = requires(T t)
{
    { t.render() } -> std::same_as<void>;
    { t.getName() } -> std::convertible_to<std::string_view>;
};

/*!
 * @brief Base UI component class providing fundamental UI functionality and hierarchy management
 * @details Inherits from enable_shared_from_this to support shared pointer management
 */
export class UIComponent : public std::enable_shared_from_this<UIComponent> {
public:
    /*!
     * @brief Constructs a UI component
     * @param name Name of the component
     */
    explicit UIComponent(std::string name) : _name(std::move(name)) {}

    /*!
     * @brief Virtual destructor for proper cleanup of derived classes
     * @details If the UIComponent or any derived class manages resources such as memory, file handles,
     * or network connections, it is crucial to override this destructor to release those resources.
     */
    virtual ~UIComponent() = default;

    /*!
     * @brief Renders the component
     * @details Skips rendering if the component is not visible
     */
    virtual auto render() -> void {
        if (!_visible) return;
        beginRender();
        renderContent();
        for (const auto& child: _children) { child->render(); }
        endRender();
    }

    /*!
     * @brief Creates a child component
     * @tparam T Child component type that must satisfy ComponentType concept
     * @tparam Args Parameter pack for constructor arguments
     * @param args Arguments forwarded to child component constructor
     * @return Shared pointer to the newly created child component
     */
    template<ComponentType T, typename... Args>
    auto createChild(Args&&... args) -> std::shared_ptr<T> {
        auto child = std::make_shared<T>(std::forward<Args>(args)...);
        addChild(child);
        return child;
    }

    /*!
     * @brief Adds a child component to this component
     * @param child Child component to add
     */
    auto addChild(std::shared_ptr<UIComponent> child) -> void {
        child->_parent = shared_from_this();
        _children.push_back(std::move(child));
    }

    /*!
     * @brief Removes a child component from this component
     * @param child Child component to remove
     */
    auto removeChild(const std::shared_ptr<UIComponent>& child) -> void {
        _children.erase(std::ranges::remove(_children, child).begin(), _children.end());
    }

    /*!
     * @brief Gets the component's name
     * @return Const reference to the component's name
     */
    auto getName() const -> const std::string& { return _name; }

    /*!
     * @brief Sets the visibility state of the component
     * @param visible Visibility flag
     */
    auto setVisible(bool visible) { _visible = visible; }

    /*!
     * @brief Gets the parent component
     * @return Shared pointer to the parent component
     */
    auto isVisible() const { return _visible; }

    /*!
     * @brief Gets the parent component
     * @return Shared pointer to the parent component
     */
    auto getParent() const { return _parent.lock(); }

    /*!
     * @brief Gets all child components
     * @return Const reference to the collection of child components
     */
    const auto& getChildren() const { return _children; }

    /*!
     * @brief Finds a component of specific type by name
     * @tparam T Type of component to find
     * @param name Name of the component to find
     * @return Shared pointer to the found component, nullptr if not found
     */
    template<ComponentType T>
    auto findComponent(std::string_view name) {
        if (_name == name) { return std::dynamic_pointer_cast<T>(shared_from_this()); }
        for (auto& child: _children) {
            if (auto found = child->findComponent<T>(name)) { return found; }
        }
        return nullptr;
    }

protected:
    /*!
     * @brief Called before rendering begins
     * @details It usually contains @code ImGui::Begin(); @endcode
     */
    virtual void beginRender() {}

    /*!
     * @brief Renders the component's content. It depends on the component's implementation.
     */
    virtual void renderContent() {}

    /*!
     * @brief Called after rendering is complete
     * @details It usually contains @code ImGui::Begin(); @endcode.
     */
    virtual void endRender() {}

private:
    std::string                               _name;          /*!< Component name */
    std::vector<std::shared_ptr<UIComponent>> _children;      /*!< Collection of child components */
    std::weak_ptr<UIComponent>                _parent;        /*!< Weak reference to parent component */
    bool                                      _visible{true}; /*!< Visibility flag */
};
