export module Graphics.ShaderCollection;

import Shaders.geometry_pass_vert;
import Shaders.geometry_pass_frag;
import Graphics.Shader;

export namespace Shaders
{
    auto GetRasterizationGeometryPassShader() {
        return Shader::loadFromBin("Raster_Geopass",
                                   geometry_pass_vert_spv,
                                   geometry_pass_vert_spv_size,
                                   geometry_pass_frag_spv,
                                   geometry_pass_frag_spv_size);
    }
};
