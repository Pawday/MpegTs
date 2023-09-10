#include "runtime_assert.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void assert(bool value)
{
    if (value) {
        return;
    }
    fprintf(stderr, "Some assertion is not satisfied\n");
    exit(EXIT_FAILURE);
}
