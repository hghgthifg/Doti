export module Scene.SceneBase;

import std;

export class SceneBase {
public:
    virtual ~SceneBase() = default;

    virtual void render() = 0;

    virtual void exit() = 0;
};
