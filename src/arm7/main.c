#include <nds.h>

#include <libxm7.h>

#include "../debug.h"

#define FIFO_XM7 (FIFO_USER_07)

void VcountHandler()
{
    inputGetAndSend();
}

void VblankHandler(void)
{
}

void XM7_Value32Handler(u32 command, void* userdata)
{
    debugf("handler invoked\n");

    if (command) {
        debugf("play module\n");
        XM7_PlayModule((XM7_ModuleManager_Type*)(command));
    } else {
        debugf("stop module\n");
        XM7_StopModule();
    }
}

int main()
{
    debugf("powering on\n");

    irqInit();
    fifoInit();

    readUserSettings();

    initClockIRQ();

    SetYtrigger(80);

    installSoundFIFO();

    XM7_Initialize();
    fifoSetValue32Handler(FIFO_XM7, XM7_Value32Handler, 0);
    debugf("XM7 initialized\n");

    installSystemFIFO();

    irqSet(IRQ_VCOUNT, VcountHandler);
    irqSet(IRQ_VBLANK, VblankHandler);

    irqEnable(IRQ_VBLANK | IRQ_VCOUNT | IRQ_NETWORK);

    while (1) {
        swiWaitForVBlank();
    }
}
