module;

#include <memory>
#include <jxl/cms_interface.h>
#include <jxl/codestream_header.h>
#include <jxl/color_encoding.h>
#include <jxl/compressed_icc.h>
#include <jxl/decode.h>
#include <jxl/encode.h>
#include <jxl/gain_map.h>
#include <jxl/resizable_parallel_runner.h>
#include <jxl/thread_parallel_runner.h>
#include <jxl/types.h>

export module libjxl;

// Decoder API - jxl/decode.h
export using ::JxlDecoder;
export using ::JxlImageOutCallback;
export using ::JxlImageOutInitCallback;
export using ::JxlImageOutRunCallback;
export using ::JxlImageOutDestroyCallback;
export using ::JxlSignature;
export using ::JxlDecoderStatus;
export using ::JxlProgressiveDetail;
export using ::JxlColorProfileTarget;
export using ::JxlDecoderVersion;
export using ::JxlSignatureCheck;
export using ::JxlDecoderCreate;
export using ::JxlDecoderReset;
export using ::JxlDecoderDestroy;
export using ::JxlDecoderRewind;
export using ::JxlDecoderSkipFrames;
export using ::JxlDecoderSkipCurrentFrame;
export using ::JxlDecoderSetParallelRunner;
export using ::JxlDecoderSizeHintBasicInfo;
export using ::JxlDecoderSubscribeEvents;
export using ::JxlDecoderSetKeepOrientation;
export using ::JxlDecoderSetUnpremultiplyAlpha;
export using ::JxlDecoderSetRenderSpotcolors;
export using ::JxlDecoderSetCoalescing;
export using ::JxlDecoderProcessInput;
export using ::JxlDecoderSetInput;
export using ::JxlDecoderReleaseInput;
export using ::JxlDecoderCloseInput;
export using ::JxlDecoderGetBasicInfo;
export using ::JxlDecoderGetExtraChannelInfo;
export using ::JxlDecoderGetExtraChannelName;
export using ::JxlDecoderGetColorAsEncodedProfile;
export using ::JxlDecoderGetICCProfileSize;
export using ::JxlDecoderGetColorAsICCProfile;
export using ::JxlDecoderSetPreferredColorProfile;
export using ::JxlDecoderSetDesiredIntensityTarget;
export using ::JxlDecoderSetOutputColorProfile;
export using ::JxlDecoderSetCms;
export using ::JxlDecoderPreviewOutBufferSize;
export using ::JxlDecoderSetPreviewOutBuffer;
export using ::JxlDecoderGetFrameHeader;
export using ::JxlDecoderGetFrameName;
export using ::JxlDecoderGetExtraChannelBlendInfo;
export using ::JxlDecoderImageOutBufferSize;
export using ::JxlDecoderSetImageOutBuffer;
export using ::JxlDecoderSetImageOutCallback;
export using ::JxlDecoderSetMultithreadedImageOutCallback;
export using ::JxlDecoderExtraChannelBufferSize;
export using ::JxlDecoderSetExtraChannelBuffer;
export using ::JxlDecoderSetJPEGBuffer;
export using ::JxlDecoderReleaseJPEGBuffer;
export using ::JxlDecoderSetBoxBuffer;
export using ::JxlDecoderReleaseBoxBuffer;
export using ::JxlDecoderSetDecompressBoxes;
export using ::JxlDecoderGetBoxType;
export using ::JxlDecoderGetBoxSizeRaw;
export using ::JxlDecoderGetBoxSizeContents;
export using ::JxlDecoderSetProgressiveDetail;
export using ::JxlDecoderGetIntendedDownsamplingRatio;
export using ::JxlDecoderFlushImage;
export using ::JxlDecoderSetImageOutBitDepth;

// Encoder API - jxl/encode.h
export using ::JxlEncoder;
export using ::JxlEncoderFrameSettings;
export using ::JxlDebugImageCallback;
export using ::JxlEncoderStats;
export using ::JxlEncoderStatus;
export using ::JxlEncoderError;
export using ::JxlEncoderFrameSettingId;
export using ::JxlEncoderStatsKey;
export using ::JxlEncoderVersion;
export using ::JxlEncoderCreate;
export using ::JxlEncoderReset;
export using ::JxlEncoderDestroy;
export using ::JxlEncoderSetCms;
export using ::JxlEncoderSetParallelRunner;
export using ::JxlEncoderGetError;
export using ::JxlEncoderProcessOutput;
export using ::JxlEncoderSetFrameHeader;
export using ::JxlEncoderSetExtraChannelBlendInfo;
export using ::JxlEncoderSetFrameName;
export using ::JxlEncoderSetFrameBitDepth;
export using ::JxlEncoderAddJPEGFrame;
export using ::JxlEncoderAddImageFrame;
export using ::JxlEncoderSetOutputProcessor;
export using ::JxlEncoderFlushInput;
export using ::JxlEncoderAddChunkedFrame;
export using ::JxlEncoderSetExtraChannelBuffer;
export using ::JxlEncoderAddBox;
export using ::JxlEncoderUseBoxes;
export using ::JxlEncoderCloseBoxes;
export using ::JxlEncoderCloseFrames;
export using ::JxlEncoderCloseInput;
export using ::JxlEncoderSetColorEncoding;
export using ::JxlEncoderSetICCProfile;
export using ::JxlEncoderInitBasicInfo;
export using ::JxlEncoderInitFrameHeader;
export using ::JxlEncoderInitBlendInfo;
export using ::JxlEncoderSetBasicInfo;
export using ::JxlEncoderSetUpsamplingMode;
export using ::JxlEncoderInitExtraChannelInfo;
export using ::JxlEncoderSetExtraChannelInfo;
export using ::JxlEncoderSetExtraChannelName;
export using ::JxlEncoderFrameSettingsSetOption;
export using ::JxlEncoderFrameSettingsSetFloatOption;
export using ::JxlEncoderUseContainer;
export using ::JxlEncoderStoreJPEGMetadata;
export using ::JxlEncoderSetCodestreamLevel;
export using ::JxlEncoderGetRequiredCodestreamLevel;
export using ::JxlEncoderSetFrameLossless;
export using ::JxlEncoderSetFrameDistance;
export using ::JxlEncoderSetExtraChannelDistance;
export using ::JxlEncoderDistanceFromQuality;
export using ::JxlEncoderFrameSettingsCreate;
export using ::JxlColorEncodingSetToSRGB;
export using ::JxlColorEncodingSetToLinearSRGB;
export using ::JxlEncoderAllowExpertOptions;
export using ::JxlEncoderSetDebugImageCallback;
export using ::JxlEncoderCollectStats;
export using ::JxlEncoderStatsCreate;
export using ::JxlEncoderStatsDestroy;
export using ::JxlEncoderStatsGet;
export using ::JxlEncoderStatsMerge;
export using ::JxlEncoderOutputProcessor;
export using ::JxlChunkedFrameInputSource;

// Common API concepts
export using ::jpegxl_alloc_func;
export using ::jpegxl_free_func;
export using ::JxlMemoryManager;
export using ::JxlBoxType;
export using ::JxlDataType;
export using ::JxlEndianness;
export using ::JxlBitDepthType;
export using ::JxlMemoryManagerStruct;
export using ::JxlPixelFormat;
export using ::JxlBitDepth;

// Image and frame metadata
export using ::JxlOrientation;
export using ::JxlExtraChannelType;
export using ::JxlBlendMode;
export using ::JxlICCProfileEncode;
export using ::JxlICCProfileDecode;
export using ::JxlGainMapGetBundleSize;
export using ::JxlGainMapWriteBundle;
export using ::JxlGainMapReadBundle;
export using ::JxlPreviewHeader;
export using ::JxlAnimationHeader;
export using ::JxlBasicInfo;
export using ::JxlExtraChannelInfo;
export using ::JxlHeaderExtensions;
export using ::JxlBlendInfo;
export using ::JxlLayerInfo;
export using ::JxlFrameHeader;
export using ::JxlGainMapBundle;

// Color encoding and conversion
export using ::jpegxl_cms_set_fields_from_icc_func;
export using ::jpegxl_cms_init_func;
export using ::jpegxl_cms_get_buffer_func;
export using ::jpegxl_cms_run_func;
export using ::jpegxl_cms_destroy_func;
export using ::JxlColorSpace;
export using ::JxlWhitePoint;
export using ::JxlPrimaries;
export using ::JxlTransferFunction;
export using ::JxlRenderingIntent;
export using ::JxlColorProfile;
export using ::JxlCmsInterface;
export using ::JxlColorEncoding;

// Multi-threaded Encoder/Decoder
export using ::JxlParallelRetCode;
export using ::JxlParallelRunInit;
export using ::JxlParallelRunFunction;
export using ::JxlParallelRunner;
export using ::JxlResizableParallelRunner;
export using ::JxlResizableParallelRunnerCreate;
export using ::JxlResizableParallelRunnerSetThreads;
export using ::JxlResizableParallelRunnerSuggestThreads;
export using ::JxlResizableParallelRunnerDestroy;
export using ::JxlThreadParallelRunner;
export using ::JxlThreadParallelRunnerCreate;
export using ::JxlThreadParallelRunnerDestroy;
export using ::JxlThreadParallelRunnerDefaultNumWorkerThreads;

// C++ helpers
export struct JxlDecoderDestroyStruct {
    /// Calls @ref JxlDecoderDestroy() on the passed decoder.
    void operator()(JxlDecoder* decoder) { JxlDecoderDestroy(decoder); }
};

export struct JxlEncoderDestroyStruct {
    /// Calls @ref JxlEncoderDestroy() on the passed encoder.
    void operator()(JxlEncoder* encoder) { JxlEncoderDestroy(encoder); }
};

export using JxlDecoderPtr = std::unique_ptr<JxlDecoder, JxlDecoderDestroyStruct>;
export using JxlEncoderPtr = std::unique_ptr<JxlEncoder, JxlEncoderDestroyStruct>;

export inline JxlDecoderPtr JxlDecoderMake(
    const JxlMemoryManager* memory_manager) {
    return JxlDecoderPtr(JxlDecoderCreate(memory_manager));
}

export inline JxlEncoderPtr JxlEncoderMake(
    const JxlMemoryManager* memory_manager) {
    return JxlEncoderPtr(JxlEncoderCreate(memory_manager));
}

export struct JxlResizableParallelRunnerDestroyStruct {
    /// Calls @ref JxlResizableParallelRunnerDestroy() on the passed runner.
    void operator()(void* runner) { JxlResizableParallelRunnerDestroy(runner); }
};

export using JxlResizableParallelRunnerPtr = std::unique_ptr<void, JxlResizableParallelRunnerDestroyStruct>;

export inline JxlResizableParallelRunnerPtr JxlResizableParallelRunnerMake(
    const JxlMemoryManager* memory_manager) {
    return JxlResizableParallelRunnerPtr(
        JxlResizableParallelRunnerCreate(memory_manager));
}

export struct JxlThreadParallelRunnerDestroyStruct {
    /// Calls @ref JxlThreadParallelRunnerDestroy() on the passed runner.
    void operator()(void* runner) { JxlThreadParallelRunnerDestroy(runner); }
};

export using JxlThreadParallelRunnerPtr = std::unique_ptr<void, JxlThreadParallelRunnerDestroyStruct>;

export inline JxlThreadParallelRunnerPtr JxlThreadParallelRunnerMake(
    const JxlMemoryManager* memory_manager, size_t num_worker_threads) {
    return JxlThreadParallelRunnerPtr(
        JxlThreadParallelRunnerCreate(memory_manager, num_worker_threads));
}

export using ::JxlThreadParallelRunnerPtr;
export using ::JxlResizableParallelRunnerMake;
export using ::JxlThreadParallelRunnerMake;
export using ::JxlDecoderDestroyStruct;
export using ::JxlEncoderDestroyStruct;
export using ::JxlResizableParallelRunnerDestroyStruct;
export using ::JxlThreadParallelRunnerDestroyStruct;
