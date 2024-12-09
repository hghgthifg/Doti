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
export using ::aiTextureType::aiTextureType_NORMALS;
export using ::aiTextureType::aiTextureType_SHININESS;
export using ::aiTextureType::aiTextureType_OPACITY;
export using ::aiTextureType::aiTextureType_METALNESS;
export using ::aiTextureType::aiTextureType_DIFFUSE_ROUGHNESS;
export using ::aiTextureType::aiTextureType_AMBIENT_OCCLUSION;
export using ::aiTextureType::aiTextureType_EMISSIVE;

constexpr auto _AI_SCENE_FLAGS_INCOMPLETE = AI_SCENE_FLAGS_INCOMPLETE;
#undef AI_SCENE_FLAGS_INCOMPLETE
export constexpr auto AI_SCENE_FLAGS_INCOMPLETE = _AI_SCENE_FLAGS_INCOMPLETE;

constexpr auto _AI_SUCCESS = AI_SUCCESS;
#undef AI_SUCCESS
export constexpr auto AI_SUCCESS = _AI_SUCCESS;

constexpr auto _AI_FAILURE = AI_FAILURE;
#undef AI_FAILURE
export constexpr auto AI_FAILURE = _AI_FAILURE;

export namespace Assimp
{
    using Assimp::Importer;

    using ::aiColor3D;
    using ::aiColor4D;
    using ::aiScene;
    using ::aiNode;
    using ::aiMesh;
    using ::aiFace;
    using ::aiMaterial;
    using ::aiTexture;
    using ::aiTextureType;
    using ::aiString;
}
