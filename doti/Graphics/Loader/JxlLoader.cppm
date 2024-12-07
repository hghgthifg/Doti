export module Graphics.Loader.Image.Jxl;

import libjxl;
import std;
import Debug.Logger;

export class JXLImageLoader {
public:
    /*!
     * @brief Structure to hold image data
     */
    struct ImageData {
        std::vector<uint8_t> pixels{};
        size_t               width{};
        size_t               height{};
        size_t               components{};
        std::string          filename;
        bool                 loaded{false};
    };

    /*!
     * @brief Initialize the parallel runner
     * @return Returns true if initialization is successful, otherwise false
     */
    auto init() -> bool {
        _runner = JxlThreadParallelRunnerMake(nullptr, JxlThreadParallelRunnerDefaultNumWorkerThreads());
        return _runner != nullptr;
    }

    /*!
     * @brief Destructor
     */
    ~JXLImageLoader() {}

    /*!
     * @brief Load multiple images in batch
     * @param filenames A span containing the filenames of images to load
     * @return A vector of indices corresponding to the loaded images
     */
    auto loadImages(std::span<const std::string> filenames) -> std::vector<int32_t> {
        std::vector<int32_t> indices;
        indices.reserve(filenames.size());

        for (const auto& filename: filenames) {
            _images.push_back({.filename = filename});
            indices.push_back(_images.size() - 1);
        }

        // Parallel decoding
        #pragma omp parallel for
        for (int32_t i = 0; i < indices.size(); i++) {
            loadSingleImage(indices[i]);
        }

        Logger::info(std::format("Loaded {} images.", indices.size()));
        return indices;
    }

    /*!
     * @brief Append a single image
     * @param filename The filename of the image to append
     * @return The index of the newly added image
     */
    auto appendImage(const std::string& filename) -> size_t {
        _images.push_back({.filename = filename});
        const size_t index = _images.size() - 1;
        loadSingleImage(index);
        return index;
    }

    /*!
     * @brief Retrieve loaded image data
     * @param index The index of the image to retrieve
     * @return An optional reference to the image data if loaded, otherwise std::nullopt
     */
    auto getImage(const size_t index) const -> std::optional<std::reference_wrapper<const ImageData>> {
        if (index >= _images.size() || !_images[index].loaded) {
            return std::nullopt;
        }
        return std::cref(_images[index]);
    }

private:
    /*!
     * @brief Load a single image by index
     * @param index The index of the image to load
     */
    auto loadSingleImage(const size_t index) -> void {
        auto&                image = _images[index];
        std::vector<uint8_t> file_data;

        try {
            const auto file_size = std::filesystem::file_size(image.filename);
            file_data.resize(file_size);

            std::ifstream file(image.filename, std::ios::binary);
            file.read(reinterpret_cast<char *>(file_data.data()), file_size);
        } catch (...) {
            Logger::error(std::format("Failed to load {}.", image.filename));
            return;
        }

        const auto dec = JxlDecoderMake(nullptr);
        if (!dec) {
            Logger::error(std::format("Failed to create JxlDecoder"));
            return;
        }

        if (JxlDecoderStatus::JXL_DEC_SUCCESS != JxlDecoderSubscribeEvents(dec.get(),
                                                                           JxlDecoderStatus::JXL_DEC_BASIC_INFO |
                                                                           JxlDecoderStatus::JXL_DEC_FULL_IMAGE)) {
            Logger::error(std::format("Failed to subscribe to JxlDecoder"));
            return;
        }

        if (JxlDecoderStatus::JXL_DEC_SUCCESS != JxlDecoderSetParallelRunner(dec.get(),
                                                                             JxlThreadParallelRunner, _runner.get())) {
            Logger::error(std::format("Failed to set parallel runner! "));
            return;
        }

        JxlDecoderSetInput(dec.get(), file_data.data(), file_data.size());

        JxlBasicInfo   info;
        JxlPixelFormat format = {4, JxlDataType::JXL_TYPE_UINT8, JxlEndianness::JXL_NATIVE_ENDIAN, 0};

        for (;;) {
            const JxlDecoderStatus status = JxlDecoderProcessInput(dec.get());

            if (status == JxlDecoderStatus::JXL_DEC_ERROR) {
                Logger::error("JxlDecoder Error: JXL_DEC_ERROR");
                return;
            } else if (status == JxlDecoderStatus::JXL_DEC_NEED_MORE_INPUT) {
                Logger::error("JxlDecoder Error: JXL_DEC_NEED_MORE_INPUT");
                return;
            } else if (status == JxlDecoderStatus::JXL_DEC_BASIC_INFO) {
                if (JxlDecoderStatus::JXL_DEC_SUCCESS != JxlDecoderGetBasicInfo(dec.get(), &info)) {
                    Logger::error("JxlDecoder Error: JXL_DEC_SUCCESS");
                    return;
                }
                image.width      = info.xsize;
                image.height     = info.ysize;
                image.components = 4;
                image.pixels.resize(info.xsize * info.ysize * 4);
            } else if (status == JxlDecoderStatus::JXL_DEC_NEED_IMAGE_OUT_BUFFER) {
                size_t buffer_size;
                if (JxlDecoderStatus::JXL_DEC_SUCCESS !=
                    JxlDecoderImageOutBufferSize(dec.get(), &format, &buffer_size)) {
                    Logger::error("JxlDecoder Error: JXL_DEC_NEED_IMAGE_OUT_BUFFER");
                    return;
                }
                if (buffer_size != image.pixels.size()) {
                    Logger::error("JxlDecoder Error: JXL_DEC_NEED_IMAGE_OUT_BUFFER");
                    return;
                }
                if (JxlDecoderStatus::JXL_DEC_SUCCESS != JxlDecoderSetImageOutBuffer(dec.get(), &format,
                        image.pixels.data(), image.pixels.size())) {
                    Logger::error("JxlDecoder Error: JXL_DEC_NEED_IMAGE_OUT_BUFFER");
                    return;
                }
            } else if (status == JxlDecoderStatus::JXL_DEC_FULL_IMAGE) {
                image.loaded = true;
                break;
            } else if (status == JxlDecoderStatus::JXL_DEC_SUCCESS) {
                break;
            }
        }
    }

    std::vector<ImageData>     _images;
    JxlThreadParallelRunnerPtr _runner{nullptr};
};
