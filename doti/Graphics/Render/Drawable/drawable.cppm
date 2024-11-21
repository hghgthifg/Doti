export module Graphics.Render.Drawable;

export import Graphics.Render.Drawable.Mesh;

import std;

/* A drawable object must have a draw method */
export template<typename T>
concept Drawable = requires(T t)
{
    { t.draw() } -> std::same_as<void>;
};

