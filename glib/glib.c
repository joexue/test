#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>        

#include <glib.h>

static gboolean gio_2ndin (GIOChannel *gio, GIOCondition condition, gpointer data)
{
	GIOStatus ret;
	GError *err = NULL;
	gchar *msg;
	gsize len;

        int insock = g_io_channel_unix_get_fd(gio);

	printf("condition = %x %x %x %x %x %d\n", condition, G_IO_IN, G_IO_HUP, G_IO_ERR, G_IO_NVAL, insock);

	/* close cocket will raise the G_IO_NVAL signal */
	if (condition & (G_IO_HUP | G_IO_ERR | G_IO_NVAL)) {
		printf("error\n");
		return FALSE;
	}
    	
	printf("get data\n");

	ret = g_io_channel_read_line (gio, &msg, &len, NULL, &err);
	printf("ret = %d\n", ret);
	if (ret == G_IO_STATUS_ERROR)
		g_error ("Error reading: %s\n", err->message);

	/* the peer point closed the socket */
	else if(ret == G_IO_STATUS_EOF) {
		printf("socket is closed\n");
    		printf("=====================\n\n");
		g_source_remove_by_user_data(data);
		g_io_channel_shutdown(gio, TRUE, &err);
		g_io_channel_unref(gio);
		close(insock);
	}
	else 
		printf ("Read %u bytes: %s, userdata=%x\n", len, msg, data);


	g_free (msg);

	return TRUE;
}

static gboolean gio_in (GIOChannel *gio, GIOCondition condition, gpointer data)
{
	GIOStatus ret;
	GError *err = NULL;
	gchar *msg;
	gsize len;

	if (condition & (G_IO_HUP | G_IO_ERR | G_IO_NVAL)) {
		printf("err\n");
		return FALSE;
	}
    	printf("=====================\n");
	printf("get a connection\n");
	struct sockaddr_storage income;

        int insock = g_io_channel_unix_get_fd(gio);
	socklen_t income_len = sizeof(income);
	int newsock = accept(insock, (struct sockaddr*)&income, &income_len );
	if(newsock == -1) {
	        printf("failure on newsock\n");
		return TRUE;
	}

	GIOChannel *new_gio = g_io_channel_unix_new(newsock);
	g_io_channel_set_encoding(new_gio, NULL, NULL);
	g_io_add_watch(new_gio, G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_NVAL, gio_2ndin, (gpointer)new_gio);
	return TRUE;
}



int main(int argc, char *argv[])
{
	GMainLoop *loop;
	GIOChannel *gio;
	gsize len;
	int sockfd;
	struct sockaddr_in sa_in;

	loop = g_main_loop_new(NULL, FALSE);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		printf("Cannot create socket!");
		return 1;
	}

	bzero(&sa_in, sizeof(sa_in));

	sa_in.sin_family = AF_INET;
	sa_in.sin_addr.s_addr = htonl(INADDR_ANY);
		//inet_addr("127.0.0.1");
	sa_in.sin_port = htons(9999);


	// for a client if (connect(sockfd, (struct sockaddr *) &sa_in, sizeof (sa_in)) < 0) {
	if (bind(sockfd, (struct sockaddr *) &sa_in, sizeof (sa_in)) < 0) {
		printf("Cannot connect!");
		return 1;
	}
	/* for a server, if this is client then discard it*/
	if ( listen(sockfd, 1) < 0 ) {
	    printf("ECHOSERV: Error calling listen()\n");
	    return 1;
	}

	gio = g_io_channel_unix_new(sockfd);

	g_io_channel_set_encoding(gio, NULL, NULL);

	g_io_add_watch(gio, G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_NVAL, gio_in, NULL);

	g_main_loop_run(loop);
	return 0;
}
