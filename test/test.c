#include <gtk/gtk.h>

static void on_entry_activate(GtkWidget *entry, gpointer data) {
	GtkLabel *label = GTK_LABEL(data);
	const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
	gtk_label_set_text(GTK_LABEL(label), text);
}
static void on_app_activate(GApplication *app, gpointer data) {
	GError *res_load_err = NULL;
	GResource *app_res = g_resource_load("test.gresource", &res_load_err);
	g_resources_register(app_res);
	GtkBuilder *app_builder = gtk_builder_new_from_resource(
	                          "/org/samik/test/test.glade");
	GObject *entry = gtk_builder_get_object(app_builder, "entry1");
	GObject *main_window = gtk_builder_get_object(app_builder, "window1");
	GObject *label = gtk_builder_get_object(app_builder, "label1");
	g_signal_connect(GTK_WIDGET(entry), "activate", G_CALLBACK(on_entry_activate), label);
	gtk_window_set_application(GTK_WINDOW(main_window), GTK_APPLICATION(app));
	gtk_widget_show_all(GTK_WIDGET(main_window));
}
int main(int argc, char *argv[]) {
	int status;

	GtkApplication *app = gtk_application_new("org.samik.draw", 
							G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), NULL);
	status = g_application_run(G_APPLICATION(app), argc, argv);

	g_object_unref(app);

	return status;
}
