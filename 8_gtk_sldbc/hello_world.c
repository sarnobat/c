#include <gtk/gtk.h>
#include <stdio.h>
#include <unistd.h>  // For getpid()

// Function to handle window close event (destroy the window)
static void on_window_close(GtkWidget *widget, gpointer data) {
    gtk_main_quit();  // Exit the GTK main loop
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *label;

    // Initialize GTK
    gtk_init(&argc, &argv);

    // Get the current process ID
    pid_t pid = getpid();
    
    // Convert the PID to a string
    char pid_string[200];
    snprintf(pid_string, sizeof(pid_string), "You can debug ths process with: \nlldb -p %d", pid);

    // Create a new window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "PID Display");
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 100);

    // Set the window's close button to exit the application
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_close), NULL);

    // Create a label with the PID
    label = gtk_label_new(pid_string);
    gtk_container_add(GTK_CONTAINER(window), label);

    // Display the window
    gtk_widget_show_all(window);

    // Run the GTK main event loop (this pauses execution until the window is closed)
    gtk_main();

    // Code will not reach here until the window is closed, thanks to gtk_main()
    return 0;
}
