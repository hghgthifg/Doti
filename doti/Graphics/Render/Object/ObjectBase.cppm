export module Graphics.Render.ObjectBase;

import Graphics.Render.RenderContext;
export import Graphics.Render.Object.Types;

export class ObjectBase {
public:
    virtual ~ObjectBase() = default;

    virtual auto load(RenderContext& render_context) -> void = 0;
};
