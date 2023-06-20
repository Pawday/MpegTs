#include <assert.h>

#include <mpegts/data/psi/descriptor.h>

MpegTsDescriptorTag_e mpeg_ts_num_to_descriptor_tag(uint8_t desctiptor_tag_as_int)
{
    switch (desctiptor_tag_as_int) {
    case 0x00:
        return MPEG_DESCRIPTOR_RESERVED;
    case 0x01:
        return MPEG_DESCRIPTOR_FORBIDDEN;
    case 0x02:
        return VIDEO_STREAM_DESCRIPTOR;
    case 0x03:
        return AUDIO_STREAM_DESCRIPTOR;
    case 0x04:
        return HIERARCHY_DESCRIPTOR;
    case 0x05:
        return REGISTRATION_DESCRIPTOR;
    case 0x06:
        return DATA_STREAM_ALIGNMENT_DESCRIPTOR;
    case 0x07:
        return TARGET_BACKGROUND_GRID_DESCRIPTOR;
    case 0x08:
        return VIDEO_WINDOW_DESCRIPTOR;
    case 0x09:
        return CA_DESCRIPTOR;
    case 0x0A:
        return ISO_639_LANGUAGE_DESCRIPTOR;
    case 0x0B:
        return SYSTEM_CLOCK_DESCRIPTOR;
    case 0x0C:
        return MULTIPLEX_BUFFER_UTILIZATION_DESCRIPTOR;
    case 0x0D:
        return COPYRIGHT_DESCRIPTOR;
    case 0x0E:
        return MAXIMUM_BITRATE_DESCRIPTOR;
    case 0x0F:
        return PRIVATE_DATA_INDICATOR_DESCRIPTOR;
    case 0x10:
        return SMOOTHING_BUFFER_DESCRIPTOR;
    case 0x11:
        return STD_DESCRIPTOR;
    case 0x12:
        return IBP_DESCRIPTOR;
    }

    if (desctiptor_tag_as_int >= 0x13 && desctiptor_tag_as_int <= 0x1A) {
        return ISO_IEC_13818_6;
    }

    switch (desctiptor_tag_as_int) {
    case 0x1B:
        return MPEG_4_VIDEO_DESCRIPTOR;
    case 0x1C:
        return MPEG_4_AUDIO_DESCRIPTOR;
    case 0x1D:
        return IOD_DESCRIPTOR;
    case 0x1E:
        return SL_DESCRIPTOR;
    case 0x1F:
        return FMC_DESCRIPTOR;
    case 0x20:
        return EXTERNAL_ES_ID_DESCRIPTOR;
    case 0x21:
        return MUXCODE_DESCRIPTOR;
    case 0x22:
        return FMX_BUFFER_SIZE_DESCRIPTOR;
    case 0x23:
        return MULTIPLEXBUFFER_DESCRIPTOR;
    case 0x24:
        return CONTENT_LABELING_DESCRIPTOR;
    case 0x25:
        return METADATA_POINTER_DESCRIPTOR;
    case 0x26:
        return METADATA_DESCRIPTOR;
    case 0x27:
        return METADATA_STD_DESCRIPTOR;
    case 0x28:
        return AVC_VIDEO_DESCRIPTOR;
    case 0x29:
        return IPMP_DESCRIPTOR;
    case 0x2A:
        return AVC_TIMING_AND_HRD_DESCRIPTOR;
    case 0x2B:
        return MPEG_2_AAC_AUDIO_DESCRIPTOR;
    case 0x2C:
        return FLEXMUXTIMING_DESCRIPTOR;
    case 0x2D:
        return MPEG_4_TEXT_DESCRIPTOR;
    case 0x2E:
        return MPEG_4_AUDIO_EXTENSION_DESCRIPTOR;
    case 0x2F:
        return AUXILIARY_VIDEO_STREAM_DESCRIPTOR;
    case 0x30:
        return SVC_EXTENSION_DESCRIPTOR;
    case 0x31:
        return MVC_EXTENSION_DESCRIPTOR;
    case 0x32:
        return J2K_VIDEO_DESCRIPTOR;
    case 0x33:
        return MVC_OPERATION_POINT_DESCRIPTOR;
    case 0x34:
        return MPEG2_STEREOSCOPIC_VIDEO_FORMAT_DESCRIPTOR;
    case 0x35:
        return STEREOSCOPIC_PROGRAM_INFO_DESCRIPTOR;
    case 0x36:
        return STEREOSCOPIC_VIDEO_INFO_DESCRIPTOR;
    case 0x37:
        return TRANSPORT_PROFILE_DESCRIPTOR;
    case 0x38:
        return HEVC_VIDEO_DESCRIPTOR;
    }

    if (desctiptor_tag_as_int >= 0x39 && desctiptor_tag_as_int <= 0x3E) {
        return MPEG_DESCRIPTOR_RESERVED;
    }

    if (desctiptor_tag_as_int == 0x3F) {
        return EXTENSION_DESCRIPTOR;
    }

    assert(desctiptor_tag_as_int >= 0x40 && desctiptor_tag_as_int <= 0xFF);

    return MPEG_DESCRIPTOR_USER_PRIVATE;
}

const char *mpeg_ts_descriptor_tag_to_string(MpegTsDescriptorTag_e descriptor_tag)
{
    switch (descriptor_tag) {

    case MPEG_DESCRIPTOR_FORBIDDEN:
        return "FORBIDDEN";
    case VIDEO_STREAM_DESCRIPTOR:
        return "VIDEO_STREAM_DESCRIPTOR";
    case AUDIO_STREAM_DESCRIPTOR:
        return "AUDIO_STREAM_DESCRIPTOR";
    case HIERARCHY_DESCRIPTOR:
        return "HIERARCHY_DESCRIPTOR";
    case REGISTRATION_DESCRIPTOR:
        return "REGISTRATION_DESCRIPTOR";
    case DATA_STREAM_ALIGNMENT_DESCRIPTOR:
        return "DATA_STREAM_ALIGNMENT_DESCRIPTOR";
    case TARGET_BACKGROUND_GRID_DESCRIPTOR:
        return "TARGET_BACKGROUND_GRID_DESCRIPTOR";
    case VIDEO_WINDOW_DESCRIPTOR:
        return "VIDEO_WINDOW_DESCRIPTOR";
    case CA_DESCRIPTOR:
        return "CA_DESCRIPTOR";
    case ISO_639_LANGUAGE_DESCRIPTOR:
        return "ISO_639_LANGUAGE_DESCRIPTOR";
    case SYSTEM_CLOCK_DESCRIPTOR:
        return "SYSTEM_CLOCK_DESCRIPTOR";
    case MULTIPLEX_BUFFER_UTILIZATION_DESCRIPTOR:
        return "MULTIPLEX_BUFFER_UTILIZATION_DESCRIPTOR";
    case COPYRIGHT_DESCRIPTOR:
        return "COPYRIGHT_DESCRIPTOR";
    case MAXIMUM_BITRATE_DESCRIPTOR:
        return "MAXIMUM_BITRATE_DESCRIPTOR";
    case PRIVATE_DATA_INDICATOR_DESCRIPTOR:
        return "PRIVATE_DATA_INDICATOR_DESCRIPTOR";
    case SMOOTHING_BUFFER_DESCRIPTOR:
        return "SMOOTHING_BUFFER_DESCRIPTOR";
    case STD_DESCRIPTOR:
        return "STD_DESCRIPTOR";
    case IBP_DESCRIPTOR:
        return "IBP_DESCRIPTOR";
    case ISO_IEC_13818_6:
        return "ISO_IEC_13818_6";
    case MPEG_4_VIDEO_DESCRIPTOR:
        return "MPEG_4_VIDEO_DESCRIPTOR";
    case MPEG_4_AUDIO_DESCRIPTOR:
        return "MPEG_4_AUDIO_DESCRIPTOR";
    case IOD_DESCRIPTOR:
        return "IOD_DESCRIPTOR";
    case SL_DESCRIPTOR:
        return "SL_DESCRIPTOR";
    case FMC_DESCRIPTOR:
        return "FMC_DESCRIPTOR";
    case EXTERNAL_ES_ID_DESCRIPTOR:
        return "EXTERNAL_ES_ID_DESCRIPTOR";
    case MUXCODE_DESCRIPTOR:
        return "MUXCODE_DESCRIPTOR";
    case FMX_BUFFER_SIZE_DESCRIPTOR:
        return "FMX_BUFFER_SIZE_DESCRIPTOR";
    case MULTIPLEXBUFFER_DESCRIPTOR:
        return "MULTIPLEXBUFFER_DESCRIPTOR";
    case CONTENT_LABELING_DESCRIPTOR:
        return "CONTENT_LABELING_DESCRIPTOR";
    case METADATA_POINTER_DESCRIPTOR:
        return "METADATA_POINTER_DESCRIPTOR";
    case METADATA_DESCRIPTOR:
        return "METADATA_DESCRIPTOR";
    case METADATA_STD_DESCRIPTOR:
        return "METADATA_STD_DESCRIPTOR";
    case AVC_VIDEO_DESCRIPTOR:
        return "AVC_VIDEO_DESCRIPTOR";
    case IPMP_DESCRIPTOR:
        return "IPMP_DESCRIPTOR";
    case AVC_TIMING_AND_HRD_DESCRIPTOR:
        return "AVC_TIMING_AND_HRD_DESCRIPTOR";
    case MPEG_2_AAC_AUDIO_DESCRIPTOR:
        return "MPEG_2_AAC_AUDIO_DESCRIPTOR";
    case FLEXMUXTIMING_DESCRIPTOR:
        return "FLEXMUXTIMING_DESCRIPTOR";
    case MPEG_4_TEXT_DESCRIPTOR:
        return "MPEG_4_TEXT_DESCRIPTOR";
    case MPEG_4_AUDIO_EXTENSION_DESCRIPTOR:
        return "MPEG_4_AUDIO_EXTENSION_DESCRIPTOR";
    case AUXILIARY_VIDEO_STREAM_DESCRIPTOR:
        return "AUXILIARY_VIDEO_STREAM_DESCRIPTOR";
    case SVC_EXTENSION_DESCRIPTOR:
        return "SVC_EXTENSION_DESCRIPTOR";
    case MVC_EXTENSION_DESCRIPTOR:
        return "MVC_EXTENSION_DESCRIPTOR";
    case J2K_VIDEO_DESCRIPTOR:
        return "J2K_VIDEO_DESCRIPTOR";
    case MVC_OPERATION_POINT_DESCRIPTOR:
        return "MVC_OPERATION_POINT_DESCRIPTOR";
    case MPEG2_STEREOSCOPIC_VIDEO_FORMAT_DESCRIPTOR:
        return "MPEG2_STEREOSCOPIC_VIDEO_FORMAT_DESCRIPTOR";
    case STEREOSCOPIC_PROGRAM_INFO_DESCRIPTOR:
        return "STEREOSCOPIC_PROGRAM_INFO_DESCRIPTOR";
    case STEREOSCOPIC_VIDEO_INFO_DESCRIPTOR:
        return "STEREOSCOPIC_VIDEO_INFO_DESCRIPTOR";
    case TRANSPORT_PROFILE_DESCRIPTOR:
        return "TRANSPORT_PROFILE_DESCRIPTOR";
    case HEVC_VIDEO_DESCRIPTOR:
        return "HEVC_VIDEO_DESCRIPTOR";
    case EXTENSION_DESCRIPTOR:
        return "EXTENSION_DESCRIPTOR";
    case MPEG_DESCRIPTOR_USER_PRIVATE:
        return "USER_PRIVATE_DESCRIPTOR";
    case MPEG_DESCRIPTOR_RESERVED:
        return "RESERVED_DESCRIPTOR";
    }

    return "UNKNOWN_DESCRIPTOR";
}

