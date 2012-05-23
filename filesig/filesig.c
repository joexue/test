#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <sys/inotify.h>

static int quit = 0;
static void quit_signal(int sig) {quit = 1;}

int main(int argc, char *argv[])
{
    int fd;
    signal(SIGINT, quit_signal);
    fd = inotify_init();
    int wd = inotify_add_watch(fd, "/export/home/jxui/temp/liu.c", IN_OPEN);
    //    open("~/temp/liu.c", O_RDONLY);
    //fcntl(fd, F_NOTIFY);
    struct inotify_event  x;
    while(1)
    {
        read(fd, &x, sizeof(x));
        printf("get its\n");
        printf("%s\n", x.name);
        printf("%d\n", x.mask);
        //sleep(1);
        if(quit == 1)
            break;
    }
    //close(fd);
    inotify_rm_watch(fd, wd);
    printf("exit!\n");
}
