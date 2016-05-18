#include <nds.h>

#include "../debug.h"

int main(void)
{
    debugf("ARM7: Powering on\n");

    while (1) {
        swiWaitForVBlank();
    }

    return 0;
}
