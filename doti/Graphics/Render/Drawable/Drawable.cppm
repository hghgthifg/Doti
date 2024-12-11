export module Graphics.Render.Drawable;

export import Graphics.Render.Drawable.Mesh;
// export import Graphics.Render.Drawable.Triangle;
export import Graphics.Render.Drawable.Model;

import std;
import Graphics.Render.RenderContext;

/* A drawable object must have a draw method */
export template<typename T>
concept Drawable = requires(T t, RenderContext& ctx)
{
    { t.draw(ctx) } -> std::same_as<void>;
};
