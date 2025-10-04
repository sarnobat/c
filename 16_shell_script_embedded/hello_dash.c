#include <stdio.h>

/* Generated from myscript.sh by the Makefile: gives
   `unsigned char myscript_sh[]; unsigned int myscript_sh_len;` */
#include "script.inc.h"

/* Call dash's entry point (we'll rename dash's `main` -> `dash_main`) */
extern int dash_main(int, char**);

int main(void) {
    /* Run the embedded script via `-c` (no temp files needed) */
    char *args[] = { "dash", "-c", (char*)myscript_sh, NULL };
    return dash_main(3, args);
}
