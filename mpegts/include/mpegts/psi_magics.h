#pragma once

#define MPEG_TS_PSI_SECTION_LENGTH_BITS    12
#define MPEG_TS_PSI_PMT_SECTION_MAX_LENGTH 0x3fd // defined in standart

#define MPEG_TS_PSI_PMT_SECTION_LENGTH_OFFSET           3
#define MPEG_TS_PSI_PMT_SECTION_LENGTH_LAST_BYTE_OFFSET MPEG_TS_PSI_PMT_SECTION_LENGTH_OFFSET + 1

#define MPEG_TS_PSI_PMT_VERSION_NUMBER_BITS      5
#define MPEG_TS_PSI_PMT_ELEMENTARY_PID_BITS      13
#define MPEG_TS_PSI_PMT_ES_INFO_LENGTH_BITS      12
#define MPEG_TS_PSI_PMT_PCR_PID_BITS             13
#define MPEG_TS_PSI_PMT_PROGRAM_INFO_LENGTH_BITS 12

#define MPEG_TS_PSI_PMT_SECTION_ID 0x02

#define MPEG_TS_PSI_PMT_SECTION_SYNTAX_INDICATOR_BIT (1 << 7)
#define MPEG_TS_PSI_PMT_SHOULD_BE_ZERO_BIT           (1 << 6)

#define MPEG_TS_PSI_PMT_VERSION_NUMBER_MASK             0x3e // 0b00111110
#define MPEG_TS_PSI_PMT_PCR_PID_MSB_BYTE_MASK           0x1f // 0b00011111
#define MPEG_TS_PSI_PMT_PROGRAM_INFO_LENGTH_BYTE_0_MASK 0x07 // 0b00000111

#define MPEG_TS_PSI_PMT_CURRENT_NEXT_INDICATOR_BIT 0x1

#define MPEG_TS_PSI_PMT_INFO_DESCRIPTORS_OFFSET 12

