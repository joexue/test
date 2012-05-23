#include <stdio.h>      
#include <stdlib.h> 
#include <unistd.h>     
#include <sys/types.h>  
#include <sys/stat.h>   
#include <fcntl.h>      
#include <termios.h>    
#include <errno.h>     
#include <signal.h>
#include <sys/times.h>

#define	TRUE 0
#define FALSE -1
static int quit = 0;
static void quit_signal(int sig) {quit = 1;}
static void install_signal_handlers(void)
{
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = quit_signal;
	sa.sa_flags = 0;
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGINT , &sa, NULL);
	sigaction(SIGHUP , &sa, NULL);
}

int OpenDev(char *Dev)
{
	int fd = open( Dev, O_RDWR | O_NOCTTY | O_NDELAY	);         //| O_NOCTTY | O_NDELAY	
	if (-1 == fd)	
	{ 			
		printf("Can't Open Serial Port");
		return -1;		
	}	
	else	
		return fd;
}


void set_speed(int fd, int speed) 
{
	int speed_arr[] = { B38400, B19200, B9600, B4800, B2400, B1200, B300,
		B38400, B19200, B9600, B4800, B2400, B1200, B300, };
	int name_arr[] = {38400,  19200,  9600,  4800,  2400,  1200,  300, 38400,  
		19200,  9600, 4800, 2400, 1200,  300, };
	int   i; 
	int   status; 
	struct termios   Opt;
	tcgetattr(fd, &Opt); 
	for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++) { 
		if  (speed == name_arr[i]) {     
			tcflush(fd, TCIOFLUSH);     
			cfsetispeed(&Opt, speed_arr[i]);  
			cfsetospeed(&Opt, speed_arr[i]);   
			status = tcsetattr(fd, TCSANOW, &Opt);  
			if  (status != 0) {        
				printf("tcsetattr fd1");  
				return;     
			}    
			tcflush(fd,TCIOFLUSH);   
		}  
	}
}

int set_Parity(int fd,int databits,int stopbits,int parity)
{ 
	struct termios options; 
	if  ( tcgetattr( fd,&options)  !=  0) { 
		printf("SetupSerial 1");     
		return(FALSE);  
	}
	options.c_cflag &= ~CSIZE; 
	switch (databits) /*设置数据位数*/
	{   
		case 7:		
			options.c_cflag |= CS7; 
			break;
		case 8:     
			options.c_cflag |= CS8;
			break;   
		default:    
			fprintf(stderr,"Unsupported data size\n"); return (FALSE);  
	}
	switch (parity) 
	{   
		case 'n':
		case 'N':    
			options.c_cflag &= ~PARENB;   /* Clear parity enable */
			options.c_iflag &= ~INPCK;     /* Enable parity checking */ 
			break;  
		case 'o':   
		case 'O':     
			options.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/  
			options.c_iflag |= INPCK;             /* Disnable parity checking */ 
			break;  
		case 'e':  
		case 'E':   
			options.c_cflag |= PARENB;     /* Enable parity */    
			options.c_cflag &= ~PARODD;   /* 转换为偶效验*/     
			options.c_iflag |= INPCK;       /* Disnable parity checking */
			break;
		case 'S': 
		case 's':  /*as no parity*/   
			options.c_cflag &= ~PARENB;
			options.c_cflag &= ~CSTOPB;break;  
		default:   
			fprintf(stderr,"Unsupported parity\n");    
			return (FALSE);  
	}  
	/* 设置停止位*/  
	switch (stopbits)
	{   
		case 1:    
			options.c_cflag &= ~CSTOPB;  
			break;  
		case 2:    
			options.c_cflag |= CSTOPB;  
			break;
		default:    
			fprintf(stderr,"Unsupported stop bits\n");  
			return (FALSE); 
	}
	/* Set input parity option */ 
	if (parity != 'n')   
		options.c_iflag |= INPCK; 
    options.c_lflag &= ~(ICANON | ECHO | ECHOE); 
    options.c_cflag &= ~CRTSCTS;
	tcflush(fd,TCIFLUSH);
	options.c_cc[VTIME] = 0; /* 设置超时15 seconds*/   
	options.c_cc[VMIN] = 0; /* Update the options and do it NOW */
	if (tcsetattr(fd,TCSANOW,&options) != 0)   
	{ 
		printf("SetupSerial 3");   
		return (FALSE);  
	} 
	return (TRUE);  
}
#if 0
int main(int argc, char **argv)
{
	int fd;
	int nread;
	char buff[513];
	char *dev  = "/dev/ttyS1"; 

	install_signal_handlers();

	fd = OpenDev(dev);
	if(fd < 0)
		exit(-1);

	set_speed(fd,9600);

	if (set_Parity(fd,8,1,'N') == FALSE)  {
		printf("Set Parity Error\n");
		exit (0);
	}

	int r;
	while (1) 
	{   
		if((nread = read(fd, buff, 512))>0)
		{
            printf("len=%d\n", nread); 
		    r = write(fd, buff, nread);	
			printf("r=%d\t",r);
		}
		if(quit == 1)
			break;
	}
	close(fd);  
	printf("exit!\n");
	exit (0);
}
#endif

/*

   ECHOSERV.C
   ==========
   (c) Paul Griffiths, 1999
Email: mail@paulgriffiths.net

Simple TCP/IP echo server.

*/


#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */

#include "helper.h"           /*  our own helper functions  */

#include <stdlib.h>
#include <stdio.h>


/*  Global constants  */

#define ECHO_PORT          (23)
#define MAX_LINE           (512)


int main(int argc, char *argv[]) {
    int       list_s;                /*  listening socket          */
    int       conn_s;                /*  connection socket         */
    short int port;                  /*  port number               */
    struct    sockaddr_in servaddr;  /*  socket address structure  */
    char      buffer[MAX_LINE];      /*  character buffer          */
    char     *endptr;                /*  for strtol()              */


	int fd;
	int nread;
	char buff[513];
	char *dev  = "/dev/ttyS1"; 

	install_signal_handlers();

	fd = OpenDev(dev);
	if(fd < 0)
		exit(-1);

	set_speed(fd,9600);

	if (set_Parity(fd,8,1,'N') == FALSE)  {
		printf("Set Parity Error\n");
		exit (0);
	}

    printf("Started...\n");
    /*  Get port number from the command line, and
        set to default port if no arguments were supplied  */

    if ( argc == 2 ) {
        port = strtol(argv[1], &endptr, 0);
        if ( *endptr ) {
            fprintf(stderr, "ECHOSERV: Invalid port number.\n");
            exit(EXIT_FAILURE);
        }
    }
    else if ( argc < 2 ) {
        port = ECHO_PORT;
    }
    else {
        fprintf(stderr, "ECHOSERV: Invalid arguments.\n");
        exit(EXIT_FAILURE);
    }


    /*  Create the listening socket  */

    if ( (list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        fprintf(stderr, "ECHOSERV: Error creating listening socket.\n");
        exit(EXIT_FAILURE);
    }


    /*  Set all bytes in socket address structure to
        zero, and fill in the relevant data members   */

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(port);


    /*  Bind our socket addresss to the 
        listening socket, and call listen()  */

    if ( bind(list_s, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) {
        fprintf(stderr, "ECHOSERV: Error calling bind()\n");
        exit(EXIT_FAILURE);
    }

    if ( listen(list_s, LISTENQ) < 0 ) {
        fprintf(stderr, "ECHOSERV: Error calling listen()\n");
        exit(EXIT_FAILURE);
    }


    /*  Wait for a connection, then accept() it  */
    if ( (conn_s = accept(list_s, NULL, NULL) ) < 0 ) {
        fprintf(stderr, "ECHOSERV: Error calling accept()\n");
        exit(EXIT_FAILURE);
    }

	if (fcntl(conn_s, F_SETFL, O_NONBLOCK) < 0) {
		perror("fcntl");
	}
    /*  Enter an infinite loop to respond
        to client requests and echo input  */
    while ( 1 ) {

        /*  Retrieve an input line from the connected socket
            then simply write it back to the same socket.     */
        memset(buffer, 0, MAX_LINE);
        //nread = Readline(conn_s, buffer, MAX_LINE-1);
        nread = read(conn_s, buffer, MAX_LINE-1);
        //Writeline(conn_s, buffer, strlen(buffer));
        int r;
        if(nread>0)
        {
            //printf("socket_nread=%d\n", nread); 
            r = write(fd, buffer, nread);	
        }

		if((nread = read(fd, buff, 512))>0)
		{
            //printf("len=%d\n", nread); 
            Writeline(conn_s, buff, nread);
		}
		if(quit == 1)
			break;
    }

    /*  Close the connected socket  */
    if ( close(conn_s) < 0 ) {
        fprintf(stderr, "ECHOSERV: Error calling close()\n");
        exit(EXIT_FAILURE);
    }
    if ( close(list_s) < 0 ) {
        fprintf(stderr, "ECHOSERV: Error calling close()\n");
        exit(EXIT_FAILURE);
    }
	close(fd);  
}
