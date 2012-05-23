#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>        
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <glib.h>

static gboolean gio_in (GIOChannel *gio, GIOCondition condition, gpointer data)
{
	GIOStatus ret;
	GError *err = NULL;
	gchar *msg;
	gsize len;

	printf("condition = %x %x %x %x %x \n", condition, G_IO_IN, G_IO_HUP, G_IO_ERR, G_IO_NVAL);

	if (condition & (G_IO_HUP | G_IO_ERR | G_IO_NVAL)) {
		return FALSE;
	}
    	
	ret = g_io_channel_read_line (gio, &msg, &len, NULL, &err);
	if (ret == G_IO_STATUS_ERROR)
		g_error ("Error reading: %s\n", err->message);

	printf ("Read %u bytes: %s\n", len, msg);

	g_free (msg);

	return TRUE;
}




int main(int argc, char *argv[])
{
	GMainLoop *loop;
	GIOChannel *gio;
	gsize len;
	int fd;

	loop = g_main_loop_new(NULL, FALSE);

	fd = open("./test", O_RDWR | O_NONBLOCK, 0);
	if (fd < 0) {
		printf("Cannot create socket!");
		return 1;
	}

	gio = g_io_channel_unix_new(fd);

	g_io_channel_set_encoding(gio, NULL, NULL);

	g_io_add_watch(gio, G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_NVAL, gio_in, NULL);

	g_main_loop_run(loop);
	return 0;
}
