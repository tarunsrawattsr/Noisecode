#include <stdio.h>
#include <gtk-3.0/gtk/gtk.h>
#include "wav.h"
#include "dct.h"
#include "cipher.h"

char* open_file_dialog() {
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    GtkFileFilter *filter = gtk_file_filter_new();
    gint res;
    char* file = NULL;

    gtk_init(NULL, NULL);

    dialog = gtk_file_chooser_dialog_new("Select Audio File", NULL, action, "_Cancel", GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);

    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
    gtk_file_filter_set_name(filter, "Audio File");
    gtk_file_filter_add_pattern(filter, "*.mp3");
    gtk_file_filter_add_pattern(filter, "*.wav");
    gtk_file_filter_add_pattern(filter, "*.flac");
    gtk_file_chooser_add_filter(chooser, filter);
    gtk_file_chooser_set_filter(chooser, filter);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        file = gtk_file_chooser_get_filename(chooser);
        g_print("Selected file: %s\n", file);
    }
    gtk_widget_destroy(dialog); // This closes the dialog after a response
    while (g_main_context_iteration(NULL, FALSE)); // Process any pending GTK events

    return file;
}

int main(){
    printf("Select Audio file to use : \n");
    char* file = open_file_dialog();

    if (file != NULL && file[0] != '\0'){
        formatChanger(file);
        char output_path[512];
        snprintf(output_path, sizeof(output_path), "%s/output.wav", g_get_current_dir());
        transform(output_path);
        g_free(file);
    } else {
        printf("⚠️ No file selected. Exiting.\n");
    }

    return 0;
}
