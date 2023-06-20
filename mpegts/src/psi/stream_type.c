#include <assert.h>

#include <mpegts/data/psi/stream_type.h>

MpegTsStreamType_e mpeg_ts_int_to_stream_type(uint8_t type)
{
    switch (type) {

    case 0x00:
        return STREAM_TYPE_RESERVED;
    case 0x01:
        return ISO_IEC_11172_2_VIDEO;
    case 0x02:
        return ISO_IEC_13818_2_VIDEO_OR_ISO_IEC_11172_2_CONSTRAINED_PARAMETER_VIDEO_STREAM;
    case 0x03:
        return ISO_IEC_11172_3_AUDIO;
    case 0x04:
        return ISO_IEC_13818_3_AUDIO;
    case 0x05:
        return ISO_IEC_13818_1_PRIVATE_SECTIONS;
    case 0x06:
        return ISO_IEC_13818_1_PES_PACKETS_CONTAINING_PRIVATE_DATA;
    case 0x07:
        return ISO_IEC_13522_MHEG;
    case 0x08:
        return ISO_IEC_13818_1_ANNEX_A_DSM_CC;
    case 0x09:
        return ITU_T_H_222_1;
    case 0x0A:
        return ISO_IEC_13818_6_TYPE_A;
    case 0x0B:
        return ISO_IEC_13818_6_TYPE_B;
    case 0x0C:
        return ISO_IEC_13818_6_TYPE_C;
    case 0x0D:
        return ISO_IEC_13818_6_TYPE_D;
    case 0x0E:
        return ISO_IEC_13818_1_AUXILIARY;
    case 0x0F:
        return ISO_IEC_13818_7_AUDIO_WITH_ADTS_TRANSPORT_SYNTAX;
    case 0x10:
        return ISO_IEC_14496_2_VISUAL;
    case 0x11:
        return ISO_IEC_14496_3_AUDIO_WITH_THE_LATM_TRANSPORT_SYNTAX_ISO_IEC_14496_3;
    case 0x12:
        return ISO_IEC_14496_1_SL_PACKETIZED_STREAM_OR_FLEXMUX_STREAM_CARRIED_IN_PES_PACKETS;
    case 0x13:
        return ISO_IEC_14496_1_SL_PACKETIZED_STREAM_OR_FLEXMUX_STREAM_CARRIED_IN_ISO_IEC_14496_SECTIONS;
    case 0x14:
        return ISO_IEC_13818_6_SYNCHRONIZED_DOWNLOAD_PROTOCOL;
    case 0x15:
        return METADATA_IN_PES_PACKETS;
    case 0x16:
        return METADATA_IN_METADATA_SECTIONS;
    case 0x17:
        return METADATA_IN_ISO_IEC_13818_6_DATA_CAROUSEL;
    case 0x18:
        return METADATA_IN_ISO_IEC_13818_6_OBJECT_CAROUSEL;
    case 0x19:
        return METADATA_IN_ISO_IEC_13818_6_SYNCHRONIZED_DOWNLOAD_PROTOCOL;
    case 0x1A:
        return ISO_IEC_13818_11_MPEG_2_IPMP;
    case 0x1B:
        return _AVC_STREAM_WITH_LONG_DESCRIPTION;
    case 0x1C:
        return ISO_IEC_14496_3_AUDIO_NO_SYNTAX;
    case 0x1D:
        return ISO_IEC_14496_17_TEXT;
    case 0x1E:
        return ISO_IEC_23002_3_AUXILIARY_VIDEO_STREAM;
    case 0x1F:
        return ISO_IEC_14496_10;
    case 0x20:
        return ISO_IEC_MVC_14496_10_SUB_BITSTREAM;
    case 0x21:
        return ISO_IEC_15444_1_VIDEO_STREAM;
    case 0x22:
        return ISO_IEC_13818_2_VIDEO_STREAM;
    case 0x23:
        return ISO_IEC_14496_10_VIDEO_STREAM;
    case 0x24:
        return ISO_IEC_23008_2_VIDEO_STREAM_OR_HEVC_TEMPORAL_SUB_BITSTREAM;
    case 0x25:
        return ISO_IEC_23008_2_HEVC_TEMPORAL_VIDEO;
    case 0x26:
        return ISO_IEC_14496_10_MVCD_VIDEO_SUB_BITSTREAM;
    case 0x27:
        return TIMELINE_AND_EXTERNAL_MEDIA_INFO;
    case 0x28:
        return ISO_IEC_23008_2_ANNEX_G_HEVC_ENHANCEMENT_SUB_PARTITION;
    case 0x29:
        return ISO_IEC_23008_2_ANNEX_G_HEVC_TEMPORAL_ENHANCEMENT_SUB_PARTITION;
    case 0x2A:
        return ISO_IEC_23008_2_ANNEX_H_HEVC_ENHANCEMENT_SUB_PARTITION;
    case 0x2B:
        return ISO_IEC_23008_2_ANNEX_H_HEVC_TEMPORAL_ENHANCEMENT_SUB_PARTITION;
    case 0x2C:
        return GREEN_ACCESS_UNITS;
    case 0x2D:
        return ISO_IEC_23008_3_AUDIO_MAIN;
    case 0x2E:
        return ISO_IEC_23008_3_AUDIO_AUXILIARY;
    case 0x2F:
        return QUALITY_ACCESS_UNITS;
    }

    if (type >= 0x30 && type <= 0x7E) {
        return STREAM_TYPE_RESERVED;
    }

    if (type == 0x7F) {
        return IPMP_STREAM;
    }

    assert(type >= 0x80 && type <= 0xFF);

    return STREAM_TYPE_USER_PRIVATE;
}

const char *mpeg_ts_stream_type_to_string(MpegTsStreamType_e stream_type)
{
    switch (stream_type) {

    case ISO_IEC_11172_2_VIDEO:
        return "ISO/IEC 11172-2 Video";
    case ISO_IEC_13818_2_VIDEO_OR_ISO_IEC_11172_2_CONSTRAINED_PARAMETER_VIDEO_STREAM:
        return "ITU-T H.262 | ISO/IEC 13818-2 Video or ISO/IEC 11172-2 constrained parameter video "
               "stream";
    case ISO_IEC_11172_3_AUDIO:
        return "ISO/IEC 11172-3 Audio";
    case ISO_IEC_13818_3_AUDIO:
        return "ISO/IEC 13818-3 Audio";
    case ISO_IEC_13818_1_PRIVATE_SECTIONS:
        return "Rec. ITU-T H.222.0 | ISO/IEC 13818-1 private_sections";
    case ISO_IEC_13818_1_PES_PACKETS_CONTAINING_PRIVATE_DATA:
        return "Rec. ITU-T H.222.0 | ISO/IEC 13818-1 PES packets containing private data";
    case ISO_IEC_13522_MHEG:
        return "ISO/IEC 13522 MHEG";
    case ISO_IEC_13818_1_ANNEX_A_DSM_CC:
        return "Rec. ITU-T H.222.0 | ISO/IEC 13818-1 Annex A DSM-CC";
    case ITU_T_H_222_1:
        return "Rec. ITU-T H.222.1";
    case ISO_IEC_13818_6_TYPE_A:
        return "ISO/IEC 13818-6 type A";
    case ISO_IEC_13818_6_TYPE_B:
        return "ISO/IEC 13818-6 type B";
    case ISO_IEC_13818_6_TYPE_C:
        return "ISO/IEC 13818-6 type C";
    case ISO_IEC_13818_6_TYPE_D:
        return "ISO/IEC 13818-6 type D";
    case ISO_IEC_13818_1_AUXILIARY:
        return "Rec. ITU-T H.222.0 | ISO/IEC 13818-1 auxiliary";
    case ISO_IEC_13818_7_AUDIO_WITH_ADTS_TRANSPORT_SYNTAX:
        return "ISO/IEC 13818-7 Audio with ADTS transport syntax";
    case ISO_IEC_14496_2_VISUAL:
        return "ISO/IEC 14496-2 Visual";
    case ISO_IEC_14496_3_AUDIO_WITH_THE_LATM_TRANSPORT_SYNTAX_ISO_IEC_14496_3:
        return "ISO/IEC 14496-3 Audio with the LATM transport syntax as defined in ISO/IEC 14496-3";
    case ISO_IEC_14496_1_SL_PACKETIZED_STREAM_OR_FLEXMUX_STREAM_CARRIED_IN_PES_PACKETS:
        return "ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in PES packets";
    case ISO_IEC_14496_1_SL_PACKETIZED_STREAM_OR_FLEXMUX_STREAM_CARRIED_IN_ISO_IEC_14496_SECTIONS:
        return "ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in ISO/IEC "
               "14496_sections";
    case ISO_IEC_13818_6_SYNCHRONIZED_DOWNLOAD_PROTOCOL:
        return "ISO/IEC 13818-6 Synchronized Download Protocol";
    case METADATA_IN_PES_PACKETS:
        return "Metadata carried in PES packets";
    case METADATA_IN_METADATA_SECTIONS:
        return "Metadata carried in metadata_sections";
    case METADATA_IN_ISO_IEC_13818_6_DATA_CAROUSEL:
        return "Metadata carried in ISO/IEC 13818-6 Data Carousel";
    case METADATA_IN_ISO_IEC_13818_6_OBJECT_CAROUSEL:
        return "Metadata carried in ISO/IEC 13818-6 Object Carousel";
    case METADATA_IN_ISO_IEC_13818_6_SYNCHRONIZED_DOWNLOAD_PROTOCOL:
        return "Metadata carried in ISO/IEC 13818-6 Synchronized Download Protocol";
    case ISO_IEC_13818_11_MPEG_2_IPMP:
        return "IPMP stream (defined in ISO/IEC 13818-11, MPEG-2 IPMP)";
    case _AVC_STREAM_WITH_LONG_DESCRIPTION:
        return "AVC video stream conforming to one or more profiles defined in Annex A of Rec. "
               "ITU-T H.264 | IS /IEC 14496-10 or AVC video "
               "sub-bitstream of SVC as defined in 2.1.10 or MVC base view sub-bitstream, as "
               "defined in 2.1.83, or AVC video sub-bitstream of MVC, "
               "as defined in 2.1.8 or MVCD base view sub-bitstream, as defined in 2.1.88, or AVC "
               "video sub-bitstream of MVCD, as defined in 2.1.9, "
               "or AVC base layer of an HEVC video stream conforming to one or more profiles "
               "defined in Annex G or Annex H of Rec. ITU-T H.265 | "
               "ISO/IEC 23008-2";
    case ISO_IEC_14496_3_AUDIO_NO_SYNTAX:
        return "ISO/IEC 14496-3 Audio, without using any additional transport syntax, such as DST, "
               "ALS and SLS";
    case ISO_IEC_14496_17_TEXT:
        return "ISO/IEC 14496-17 Text";
    case ISO_IEC_23002_3_AUXILIARY_VIDEO_STREAM:
        return "Auxiliary video stream as defined in ISO/IEC 23002-3";
    case ISO_IEC_14496_10:
        return "SVC video sub-bitstream of an AVC video stream conforming to one or more profiles "
               "defined in Annex G of Rec. ITU-T H.264 | ISO/IEC "
               "14496-10";
    case ISO_IEC_MVC_14496_10_SUB_BITSTREAM:
        return "MVC video sub-bitstream of an AVC video stream conforming to one or more profiles "
               "defined in Annex H of Rec. ITU-T H.264 | ISO/IEC "
               "14496-10";
    case ISO_IEC_15444_1_VIDEO_STREAM:
        return "Video stream conforming to one or more profiles as defined in Rec. ITU-T T.800 | "
               "ISO/IEC 15444-1";
    case ISO_IEC_13818_2_VIDEO_STREAM:
        return "Additional view Rec. ITU-T H.262 | ISO/IEC 13818-2 video stream for "
               "service-compatible stereoscopic 3D services";
    case ISO_IEC_14496_10_VIDEO_STREAM:
        return "Additional view Rec. ITU-T H.264 | ISO/IEC 14496-10 video stream conforming to one "
               "or more profiles defined in Annex A for "
               "service-compatible stereoscopic 3D services";
    case ISO_IEC_23008_2_VIDEO_STREAM_OR_HEVC_TEMPORAL_SUB_BITSTREAM:
        return "Rec. ITU-T H.265 | ISO/IEC 23008-2 video stream or an HEVC temporal video "
               "sub-bitstream";
    case ISO_IEC_23008_2_HEVC_TEMPORAL_VIDEO:
        return "HEVC temporal video subset of an HEVC video stream conforming to one or more "
               "profiles defined in Annex A of Rec. ITU-T H.265 | "
               "ISO/IEC 23008-2";
    case ISO_IEC_14496_10_MVCD_VIDEO_SUB_BITSTREAM:
        return "MVCD video sub-bitstream of an AVC video stream conforming to one or more profiles "
               "defined in Annex I of Rec. ITU-T H.264 | ISO/IEC "
               "14496-10";
    case TIMELINE_AND_EXTERNAL_MEDIA_INFO:
        return "Timeline and External Media Information Stream";
    case ISO_IEC_23008_2_ANNEX_G_HEVC_ENHANCEMENT_SUB_PARTITION:
        return "HEVC enhancement sub-partition which includes TemporalId 0 of an HEVC video stream "
               "where all NALs units contained in the stream "
               "conform to one or more profiles defined in Annex G of Rec. ITU-T H.265 | ISO/IEC "
               "23008-2";
    case ISO_IEC_23008_2_ANNEX_G_HEVC_TEMPORAL_ENHANCEMENT_SUB_PARTITION:
        return "HEVC temporal enhancement sub-partition of an HEVC video stream where all NAL "
               "units contained in the stream conform to one or more "
               "profiles defined in Annex G of Rec. ITU-T H.265 | ISO/IEC 23008-2";
    case ISO_IEC_23008_2_ANNEX_H_HEVC_ENHANCEMENT_SUB_PARTITION:
        return "HEVC enhancement sub-partition which includes TemporalId 0 of an HEVC video stream "
               "where all NAL units contained in the stream "
               "conform to one or more profiles defined in Annex H of Rec. ITU-T H.265 | ISO/IEC "
               "23008-2";
    case ISO_IEC_23008_2_ANNEX_H_HEVC_TEMPORAL_ENHANCEMENT_SUB_PARTITION:
        return "HEVC temporal enhancement sub-partition of an HEVC video stream where all NAL "
               "units contained in the stream conform to one or more "
               "profiles defined in Annex H of Rec. ITU-T H.265 | ISO/IEC 23008-2";
    case GREEN_ACCESS_UNITS:
        return "Green access units carried in MPEG-2 sections";
    case ISO_IEC_23008_3_AUDIO_MAIN:
        return "ISO/IEC 23008-3 Audio with MHAS transport syntax – main stream";
    case ISO_IEC_23008_3_AUDIO_AUXILIARY:
        return "ISO/IEC 23008-3 Audio with MHAS transport syntax – auxiliary stream";
    case QUALITY_ACCESS_UNITS:
        return "Quality access units carried in sections";
    case IPMP_STREAM:
        return "IPMP stream";
    case STREAM_TYPE_USER_PRIVATE:
        return "User Private";
    case STREAM_TYPE_RESERVED:
        return "Reserved";
    default:
        return "UNKNOWN";
    }
}
