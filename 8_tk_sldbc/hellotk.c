#include <tk.h>

int main(int argc, char *argv[]) {
    // Initialize the Tcl/Tk library
    Tcl_Interp *interp;
    Tk_Window mainWindow;
    interp = Tcl_CreateInterp();
    mainWindow = Tk_CreateMainWindow(interp, NULL, "Hello World", NULL);

    // Create the label with the text "Hello World"
    Tk_CreateLabel(mainWindow, NULL, NULL, "Hello World");

    // Enter the Tk event loop
    Tk_MainLoop();
    
    return 0;
}
