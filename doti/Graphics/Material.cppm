export module Graphics.Material;

import std;
import Math;
import OpenGL;

/*!
* @brief Enum class defining different material property types
*/
export enum class MaterialPropertyType {
    Ambient,   // Ka
    Diffuse,   // Kd
    Specular,  // Ks
    Emission,  // Ke
    Normal,    // Normal map
    Roughness, // Roughness/Glossiness
    Metallic,  // Metallic/Specular workflow
    AO,        // Ambient Occlusion
    Opacity    // Transparency
};
