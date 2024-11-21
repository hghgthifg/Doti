import std;
import Window;
import Math;
import Graphics.Shader;
import Graphics.Render.Drawable.Triangle;
import Debug.Logger;
import Event;

auto main() -> int {
    try {
        const Window window(800, 600, "Hello World");
        Shader       shader("default", "shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
        Triangle     triangle;
        std::vector  points{
            Point3{-0.5f, -0.5f, 0.0f},
            Point3{0.5f, -0.5f, 0.0f},
            Point3{0.0f, 0.5f, 0.0f}
        };
        triangle.set(points);
        while (!window.shouldClose()) {
            window.beginDraw();
            triangle.draw(shader);
            window.endDraw();
        }
    } catch (const std::exception& e) {
        Logger::error(e.what());
        return -1;
    }
    return 0;
}
