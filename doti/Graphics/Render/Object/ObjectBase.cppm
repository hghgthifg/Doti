export module Graphics.Render.ObjectBase;

import Graphics.Render.RenderContext;
export import Graphics.Render.Pipeline;

export class ObjectBase {
public:
    virtual ~ObjectBase() = default;

    virtual auto sendDataToPipeline() -> void = 0;
};
