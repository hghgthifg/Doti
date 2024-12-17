export module Graphics.ShaderCollection;

import Shaders.raster_vert;
import Graphics.Shader;

export namespace Shaders
{
    auto GetRasterizationShader() {
        return Shader::loadFromBin("Rasterizalization",
                                   raster_vert_spv,
                                   raster_vert_spv_size,
                                   raster_frag_spv,
                                   raster_frag_spv_size);
    }
};
