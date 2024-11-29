module;
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

export module assimp;

export using ::aiPostProcessSteps::aiProcess_Triangulate;
export using ::aiPostProcessSteps::aiProcess_GenSmoothNormals;
export using ::aiPostProcessSteps::aiProcess_FlipUVs;
export using ::aiPostProcessSteps::aiProcess_CalcTangentSpace;

export using ::aiTextureType::aiTextureType_AMBIENT;
export using ::aiTextureType::aiTextureType_DIFFUSE;
export using ::aiTextureType::aiTextureType_SPECULAR;
export using ::aiTextureType::aiTextureType_HEIGHT;

constexpr auto _AI_SCENE_FLAGS_INCOMPLETE = AI_SCENE_FLAGS_INCOMPLETE;
#undef AI_SCENE_FLAGS_INCOMPLETE
export constexpr auto AI_SCENE_FLAGS_INCOMPLETE = _AI_SCENE_FLAGS_INCOMPLETE;

export namespace Assimp
{
    using Assimp::Importer;

    using ::aiScene;
    using ::aiNode;
    using ::aiMesh;
    using ::aiFace;
    using ::aiMaterial;
    using ::aiTexture;
    using ::aiTextureType;
    using ::aiString;
}
