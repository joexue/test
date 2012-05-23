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

int main(int argc, char **argv)
{
	int count = 0;
	int oldcount = -1;
	int fd;
	int nread;
	char buff[513];
	char buff1[513];
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

	while (1) 
	{   
		while((nread = read(fd, buff, 512))>0)
		{ 
			int r;
			//printf("\nLen %d\n",nread); 
			//buff[nread+1] = '\0';   
			//printf( "\n%s", buff); 
			printf("AA\t",r);
			count += nread;
			memcpy(buff1, buff, nread);
			int s1 = times((struct tms *)NULL);
		        r = write(fd, buff1, nread);	
			int s2 = times((struct tms *)NULL);
			printf("r=%d c=%c\t",r, buff[0]);
			printf("used ticks = %d\t", s2-s1);
		}
		if(count != oldcount)
		{
			printf("len=%d\n", count);
			oldcount = count;
		}
		if(quit == 1)
			break;
	}
	close(fd);  
	printf("exit!\n");
	exit (0);
}
