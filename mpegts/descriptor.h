#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

#define MPEG_TS_DESCRIPTOR_HEADER_SIZE 2

typedef enum
{
    MPEG_DESCRIPTOR_FORBIDDEN,                  // 0x01
    VIDEO_STREAM_DESCRIPTOR,                    // 0x02
    AUDIO_STREAM_DESCRIPTOR,                    // 0x03
    HIERARCHY_DESCRIPTOR,                       // 0x04
    REGISTRATION_DESCRIPTOR,                    // 0x05
    DATA_STREAM_ALIGNMENT_DESCRIPTOR,           // 0x06
    TARGET_BACKGROUND_GRID_DESCRIPTOR,          // 0x07
    VIDEO_WINDOW_DESCRIPTOR,                    // 0x08
    CA_DESCRIPTOR,                              // 0x09
    ISO_639_LANGUAGE_DESCRIPTOR,                // 0x0A
    SYSTEM_CLOCK_DESCRIPTOR,                    // 0x0B
    MULTIPLEX_BUFFER_UTILIZATION_DESCRIPTOR,    // 0x0C
    COPYRIGHT_DESCRIPTOR,                       // 0x0D
    MAXIMUM_BITRATE_DESCRIPTOR,                 // 0x0E
    PRIVATE_DATA_INDICATOR_DESCRIPTOR,          // 0x0F
    SMOOTHING_BUFFER_DESCRIPTOR,                // 0x10
    STD_DESCRIPTOR,                             // 0x11
    IBP_DESCRIPTOR,                             // 0x12
    ISO_IEC_13818_6,                            // 0x13->0x1A (ISO/IEC 13818-6)
    MPEG_4_VIDEO_DESCRIPTOR,                    // 0x1B
    MPEG_4_AUDIO_DESCRIPTOR,                    // 0x1C
    IOD_DESCRIPTOR,                             // 0x1D
    SL_DESCRIPTOR,                              // 0x1E
    FMC_DESCRIPTOR,                             // 0x1F
    EXTERNAL_ES_ID_DESCRIPTOR,                  // 0x20
    MUXCODE_DESCRIPTOR,                         // 0x21
    FMX_BUFFER_SIZE_DESCRIPTOR,                 // 0x22
    MULTIPLEXBUFFER_DESCRIPTOR,                 // 0x23
    CONTENT_LABELING_DESCRIPTOR,                // 0x24
    METADATA_POINTER_DESCRIPTOR,                // 0x25
    METADATA_DESCRIPTOR,                        // 0x26
    METADATA_STD_DESCRIPTOR,                    // 0x27
    AVC_VIDEO_DESCRIPTOR,                       // 0x28
    IPMP_DESCRIPTOR,                            // 0x29
    AVC_TIMING_AND_HRD_DESCRIPTOR,              // 0x2A
    MPEG_2_AAC_AUDIO_DESCRIPTOR,                // 0x2B
    FLEXMUXTIMING_DESCRIPTOR,                   // 0x2C
    MPEG_4_TEXT_DESCRIPTOR,                     // 0x2D
    MPEG_4_AUDIO_EXTENSION_DESCRIPTOR,          // 0x2E
    AUXILIARY_VIDEO_STREAM_DESCRIPTOR,          // 0x2F
    SVC_EXTENSION_DESCRIPTOR,                   // 0x30
    MVC_EXTENSION_DESCRIPTOR,                   // 0x31
    J2K_VIDEO_DESCRIPTOR,                       // 0x32
    MVC_OPERATION_POINT_DESCRIPTOR,             // 0x33
    MPEG2_STEREOSCOPIC_VIDEO_FORMAT_DESCRIPTOR, // 0x34
    STEREOSCOPIC_PROGRAM_INFO_DESCRIPTOR,       // 0x35
    STEREOSCOPIC_VIDEO_INFO_DESCRIPTOR,         // 0x36
    TRANSPORT_PROFILE_DESCRIPTOR,               // 0x37
    HEVC_VIDEO_DESCRIPTOR,                      // 0x38
                                                // 0x39->0x3E
    EXTENSION_DESCRIPTOR,                       // 0x3F
    MPEG_DESCRIPTOR_USER_PRIVATE,               // 0x40->0xFF
                                                //
    MPEG_DESCRIPTOR_RESERVED                    // 0x00, 0x39->0x3E
} MpegTsDescriptorTag_e;

typedef struct
{
    MpegTsDescriptorTag_e tag;
    uint8_t length;
    const uint8_t *data;

} MpegTsDescriptor_t;

typedef struct
{
    bool has_value;
    MpegTsDescriptor_t value;
} OptionalMpegTsDescriptor_t;

MpegTsDescriptorTag_e mpeg_ts_num_to_descriptor_tag(uint8_t desctiptor_tag_as_int);
const char *mpeg_ts_descriptor_tag_to_string(MpegTsDescriptorTag_e descriptor_tag);

