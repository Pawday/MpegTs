#include <stddef.h>
#include <stdint.h>

typedef struct
{
    const uint8_t *data;
    const size_t size;
} IOVec;

IOVec test_data_get_raw_packet(void);
IOVec test_data_get_two_packets(void);
