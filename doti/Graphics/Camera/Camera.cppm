export module Graphics.Camera;

import std;
import Core.Math;
import Core.Event;
import Core.Logger;

/*!
 * @brief Class representing a camera in the scene with position and orientation.
 */
export class Camera {
public:
    /*!
     * @brief Enumeration for camera movement directions.
     */
    enum class MoveDirection {
        Forward,  /*!< Move the camera forward */
        Backward, /*!< Move the camera backward */
        Left,     /*!< Move the camera to the left */
        Right,    /*!< Move the camera to the right */
    };

    Camera() = default;

    /*!
     * @brief Constructor for the Camera class.
     * @param screen_width Width of the screen.
     * @param screen_height Height of the screen.
     */
    Camera(const float screen_width, const float screen_height) {
        this->reset(screen_width, screen_height);
    }

    /*!
     * @brief Retrieves the view matrix calculated using the camera's position and orientation.
     * @return The view matrix as a Mat4 object.
     */
    auto getViewMatrix() const -> Mat4 {
        const auto view = Math::lookAt(
            _cameraPos,                /*!< Camera position */
            _cameraPos + _cameraFront, /*!< Target position */
            _cameraUp                  /*!< Up vector */
        );
        return view;
    }

    /*!
     * @brief Updates the camera's orientation based on mouse movement.
     * @param deltaX The change in mouse position along the X-axis.
     * @param deltaY The change in mouse position along the Y-axis.
     */
    auto updateOrientation(const float deltaX, const float deltaY) -> void {
        constexpr float sensitivity = 1.1f;                  /*!< Mouse sensitivity factor */
        float           Xoffset     = -deltaX * sensitivity; /*!< Adjusted X offset */
        float           Yoffset     = deltaY * sensitivity;  /*!< Adjusted Y offset */

        _yaw += Xoffset;
        _pitch = Math::clamp(_pitch + Yoffset, -89.0f, 89.0f);

        this->updateCameraVectors();
    }

    /*!
     * @brief Retrieves the camera's field of view (FOV).
     * @return The FOV in degrees.
     */
    auto getFov() const -> float { return _fov; }

    /*!
     * @brief Sets the camera's field of view (FOV).
     * @param fov The desired FOV in degrees.
     */
    auto setFov(const float fov) -> void {
        _fov        = fov;
        _halfHeight = Math::tan(Math::radians(_fov / 2.0f));
        _halfWidth  = _halfHeight * _screenRatio;

        _leftBottomCorner = _cameraFront - _halfWidth * _cameraRight - _halfHeight * _cameraUp;
    }

    /*!
     * @brief Retrieves the camera's position vector.
     * @return The camera position as a Vec3.
     */
    auto getCameraPos() const -> Vec3 { return _cameraPos; }

    /*!
     * @brief Retrieves the camera's front (forward) vector.
     * @return The front vector as a Vec3.
     */
    auto getCameraFront() const -> Vec3 { return _cameraFront; }

    /*!
     * @brief Retrieves the camera's up vector.
     * @return The up vector as a Vec3.
     */
    auto getCameraUp() const -> Vec3 { return _cameraUp; }

    /*!
     * @brief Retrieves the camera's right vector.
     * @return The right vector as a Vec3.
     */
    auto getCameraRight() const -> Vec3 { return _cameraRight; }

    /*!
     * @brief Retrieves half of the viewport width.
     * @return Half of the viewport width as a float.
     */
    auto getHalfWidth() const -> float { return _halfWidth; }

    /*!
     * @brief Retrieves half of the viewport height.
     * @return Half of the viewport height as a float.
     */
    auto getHalfHeight() const -> float { return _halfHeight; }

    /*!
     * @brief Retrieves the left-bottom corner coordinate of the view frustum.
     * @return The left-bottom corner as a Vec3.
     */
    auto getLeftBottomCorner() const -> Vec3 { return _leftBottomCorner; }

    /*!
     * @brief Resets the camera to default parameters with the given screen size.
     * @param screen_width The width of the screen.
     * @param screen_height The height of the screen.
     */
    auto reset(uint32_t screen_width, uint32_t screen_height) -> void {
        _yaw               = -90.0f;                 /*!< Initial yaw angle */
        _pitch             = -50.0f;                 /*!< Initial pitch angle */
        _fov               = 60.0f;                  /*!< Initial field of view */
        _isFirstMouseInput = true;                   /*!< Flag for first mouse input */
        _cameraPos         = Vec3(0.0f, 1.4f, 1.0f); /*!< Initial camera position */
        _worldUp           = Vec3(0.0f, 1.0f, 0.0f); /*!< World up vector */

        _screenRatio = static_cast<float>(screen_width) / static_cast<float>(screen_height);
        _halfHeight  = Math::tan(Math::radians(_fov / 2.0f));
        _halfWidth   = _halfHeight * _screenRatio;
        _cameraSpeed = 1.0f; /*!< Movement speed */
        this->updateCameraVectors();
    }

    /*!
     * @brief Updates the screen aspect ratio and recalculates related parameters.
     * @param screen_width The new screen width.
     * @param screen_height The new screen height.
     */
    auto updateScreenRatio(int screen_width, int screen_height) -> void {
        _screenRatio = static_cast<float>(screen_width) / static_cast<float>(screen_height);
        _halfWidth   = _halfHeight * _screenRatio;

        _leftBottomCorner = _cameraFront - _halfWidth * _cameraRight - _halfHeight * _cameraUp;
    }

    auto moveLeft() -> void {
        this->_cameraPos -= this->_cameraRight * this->_cameraSpeed;
    }

    auto moveRight() -> void {
        this->_cameraPos += this->_cameraRight * this->_cameraSpeed;
    }

    auto moveForward() -> void {
        this->_cameraPos += this->_cameraFront * this->_cameraSpeed;
    }

    auto moveBackward() -> void {
        this->_cameraPos -= this->_cameraFront * this->_cameraSpeed;
    }

private:
    /*!
     * @brief Updates the camera's direction vectors based on the updated Euler angles.
     */
    auto updateCameraVectors() -> void {
        const Vec3 front{
            Math::cos(Math::radians(_yaw)) * Math::cos(Math::radians(_pitch)),
            Math::sin(Math::radians(_pitch)),
            Math::sin(Math::radians(_yaw)) * Math::cos(Math::radians(_pitch))
        };
        _cameraFront = Math::normalize(front);
        _cameraRight = Math::normalize(Math::cross(_cameraFront, _worldUp));
        _cameraUp    = Math::normalize(Math::cross(_cameraRight, _cameraFront));

        _leftBottomCorner = _cameraFront - _halfWidth * _cameraRight - _halfHeight * _cameraUp;
    }

    Vec3  _cameraPos;   /*!< The camera's position in world space */
    Vec3  _cameraFront; /*!< The direction the camera is facing */
    Vec3  _cameraUp;    /*!< The upward direction vector of the camera */
    Vec3  _cameraRight; /*!< The right direction vector of the camera */
    float _cameraSpeed; /*!< The movement speed of the camera */

    Vec3 _worldUp; /*!< The world's upward direction vector */

    float _fov;   /*!< The camera's field of view in degrees */
    float _yaw;   /*!< The yaw angle (rotation around the Y-axis) */
    float _pitch; /*!< The pitch angle (rotation around the X-axis) */

    bool  _isFirstMouseInput; /*!< Flag indicating if it's the first mouse input */
    float _lastX;             /*!< The last recorded X position of the mouse */
    float _lastY;             /*!< The last recorded Y position of the mouse */

    float _screenRatio;      /*!< The aspect ratio of the screen */
    float _halfWidth;        /*!< Half of the viewport's width */
    float _halfHeight;       /*!< Half of the viewport's height */
    Vec3  _leftBottomCorner; /*!< The coordinate of the left-bottom corner of the viewport */
};
