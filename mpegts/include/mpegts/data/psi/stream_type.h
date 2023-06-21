#pragma once

#include <inttypes.h>

typedef enum MpegTsStreamType_e
{
    // 0x00 Reserved
    ISO_IEC_11172_2_VIDEO,                                                                    // 0x01
    ISO_IEC_13818_2_VIDEO_OR_ISO_IEC_11172_2_CONSTRAINED_PARAMETER_VIDEO_STREAM,              // 0x02
    ISO_IEC_11172_3_AUDIO,                                                                    // 0x03
    ISO_IEC_13818_3_AUDIO,                                                                    // 0x04
    ISO_IEC_13818_1_PRIVATE_SECTIONS,                                                         // 0x05
    ISO_IEC_13818_1_PES_PACKETS_CONTAINING_PRIVATE_DATA,                                      // 0x06
    ISO_IEC_13522_MHEG,                                                                       // 0x07
    ISO_IEC_13818_1_ANNEX_A_DSM_CC,                                                           // 0x08
    ITU_T_H_222_1,                                                                            // 0x09
    ISO_IEC_13818_6_TYPE_A,                                                                   // 0x0A
    ISO_IEC_13818_6_TYPE_B,                                                                   // 0x0B
    ISO_IEC_13818_6_TYPE_C,                                                                   // 0x0C
    ISO_IEC_13818_6_TYPE_D,                                                                   // 0x0D
    ISO_IEC_13818_1_AUXILIARY,                                                                // 0x0E
    ISO_IEC_13818_7_AUDIO_WITH_ADTS_TRANSPORT_SYNTAX,                                         // 0x0F
    ISO_IEC_14496_2_VISUAL,                                                                   // 0x10
    ISO_IEC_14496_3_AUDIO_WITH_THE_LATM_TRANSPORT_SYNTAX_ISO_IEC_14496_3,                     // 0x11
    ISO_IEC_14496_1_SL_PACKETIZED_STREAM_OR_FLEXMUX_STREAM_CARRIED_IN_PES_PACKETS,            // 0x12
    ISO_IEC_14496_1_SL_PACKETIZED_STREAM_OR_FLEXMUX_STREAM_CARRIED_IN_ISO_IEC_14496_SECTIONS, // 0x13
    ISO_IEC_13818_6_SYNCHRONIZED_DOWNLOAD_PROTOCOL,                                           // 0x14
    METADATA_IN_PES_PACKETS,                                                                  // 0x15
    METADATA_IN_METADATA_SECTIONS,                                                            // 0x16
    METADATA_IN_ISO_IEC_13818_6_DATA_CAROUSEL,                                                // 0x17
    METADATA_IN_ISO_IEC_13818_6_OBJECT_CAROUSEL,                                              // 0x18
    METADATA_IN_ISO_IEC_13818_6_SYNCHRONIZED_DOWNLOAD_PROTOCOL,                               // 0x19
    ISO_IEC_13818_11_MPEG_2_IPMP,                                                             // 0x1A
    _AVC_STREAM_WITH_LONG_DESCRIPTION,                                                        // 0x1B
    ISO_IEC_14496_3_AUDIO_NO_SYNTAX,                                                          // 0x1C
    ISO_IEC_14496_17_TEXT,                                                                    // 0x1D
    ISO_IEC_23002_3_AUXILIARY_VIDEO_STREAM,                                                   // 0x1E
    ISO_IEC_14496_10,                                                                         // 0x1F
    ISO_IEC_MVC_14496_10_SUB_BITSTREAM,                                                       // 0x20
    ISO_IEC_15444_1_VIDEO_STREAM,                                                             // 0x21
    ISO_IEC_13818_2_VIDEO_STREAM,                                                             // 0x22
    ISO_IEC_14496_10_VIDEO_STREAM,                                                            // 0x23
    ISO_IEC_23008_2_VIDEO_STREAM_OR_HEVC_TEMPORAL_SUB_BITSTREAM,                              // 0x24
    ISO_IEC_23008_2_HEVC_TEMPORAL_VIDEO,                                                      // 0x25
    ISO_IEC_14496_10_MVCD_VIDEO_SUB_BITSTREAM,                                                // 0x26
    TIMELINE_AND_EXTERNAL_MEDIA_INFO,                                                         // 0x27
    ISO_IEC_23008_2_ANNEX_G_HEVC_ENHANCEMENT_SUB_PARTITION,                                   // 0x28
    ISO_IEC_23008_2_ANNEX_G_HEVC_TEMPORAL_ENHANCEMENT_SUB_PARTITION,                          // 0x29
    ISO_IEC_23008_2_ANNEX_H_HEVC_ENHANCEMENT_SUB_PARTITION,                                   // 0x2A
    ISO_IEC_23008_2_ANNEX_H_HEVC_TEMPORAL_ENHANCEMENT_SUB_PARTITION,                          // 0x2B
    GREEN_ACCESS_UNITS,                                                                       // 0x2C
    ISO_IEC_23008_3_AUDIO_MAIN,                                                               // 0x2D
    ISO_IEC_23008_3_AUDIO_AUXILIARY,                                                          // 0x2E
    QUALITY_ACCESS_UNITS,                                                                     // 0x2F
                                                                                              // 0x30->0x7E Reserved
    IPMP_STREAM,                                                                              // 0x7F
    STREAM_TYPE_USER_PRIVATE,                                                                             // 0x80->0xFF
    STREAM_TYPE_RESERVED                                                                      // 0x00, 0x30->0x7E
} MpegTsStreamType_e;

MpegTsStreamType_e mpeg_ts_int_to_stream_type(uint8_t type);
const char *mpeg_ts_stream_type_to_string(MpegTsStreamType_e stream_type);

