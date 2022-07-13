#include <nds.h>
#include <stdio.h>
#include <sys/stat.h>
#include <limits.h>

#include <string.h>
#include <unistd.h>

int i = 0;

ITCM_CODE bool isTwlSpeed(void) {
	if (REG_SCFG_EXT != 0) {
		return REG_SCFG_CLK & BIT(0);
	}

	int oldIME = enterCriticalSection();

	while (REG_VCOUNT != 191);
	while (REG_VCOUNT == 191);

	while (REG_VCOUNT != 191) {
		i++;
	}

	leaveCriticalSection(oldIME);

	return (i >= 100000 && i < 150000);
}

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------

	defaultExceptionHandler();

	consoleDemoInit();
	iprintf("Now checking CPU Speed...\n");

	bool disableBootNds = false;

	scanKeys();
	if ((keysHeld() & KEY_L) && (REG_SCFG_EXT & BIT(31))) {
		REG_SCFG_EXT &= ~BIT(31); // Lock SCFG
		disableBootNds = true;
	}

	if (disableBootNds) {
		// overwrite reboot stub identifier
		// so tapping power on DSi returns to DSi menu, or pressing START turns off the console
		extern u64 *fake_heap_end;
		*fake_heap_end = 0;
	}

	bool twlSpeed = isTwlSpeed();
	if (i > 0) {
		iprintf("%i", i);
	} else {
		iprintf("SCFG found!");
	}
	iprintf("\n\n");
	iprintf(twlSpeed ? "134MHz (TWL)" : "67MHz (NTR)");
	iprintf(" detected!\n");
	iprintf("\nPress START to exit\n");

	while (!(keysDown() & KEY_START)) {
		scanKeys();
		swiWaitForVBlank();
	}

	return 0;
}
