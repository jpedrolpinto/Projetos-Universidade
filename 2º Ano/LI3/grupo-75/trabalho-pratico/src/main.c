#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "modes/batch.h"

#include "utilities/utilities.h"

int main(int argc, char **argv)
{
    if (argc == 3)
    {
        int ret = batch(argv);

        if (ret != 0)
        {
            printf("%s\n", get_error_as_string(ret));
            return 1;
        }
    }
    else
    {
        printf("Batch mode: %s <dataset-dir> <queries-file>\n", argv[0]);
        return 1;
    }

    return 0;
}