#include <stdio.h>      
#include <stdlib.h> 
#include <unistd.h>     
#include <string.h>
#include <sys/types.h>  
#include <sys/stat.h>   
#include <fcntl.h>      
#include <termios.h>    
#include <errno.h>     
#include <signal.h>
#include <sys/times.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <syslog.h>

#ifdef DEBUG
    #define MAX_LEVEL LOG_DEBUG
#else
    #define MAX_LEVEL LOG_INFO
#endif

#define log(level, text, args...) {                                     \
    if (level <= MAX_LEVEL) {                                           \
        if (MAX_LEVEL >= LOG_DEBUG) {                                   \
            syslog(level,  text "[" __FILE__ ":%d]", ##args, __LINE__); \
        } else {                                                        \
            syslog(level, text ,##args);                                \
        }                                                               \
    }                                                                   \
}

#define	TRUE 0
#define FALSE -1

int OpenDev(char *Dev)
{
    //int fd = open( Dev, O_RDWR | O_NOCTTY | O_NDELAY	);         //| O_NOCTTY | O_NDELAY	
    int fd = open(Dev, O_RDWR | O_NOCTTY);         //| O_NOCTTY | O_NDELAY	
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
    switch (databits)
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
            options.c_cflag |= (PARODD | PARENB); 
            options.c_iflag |= INPCK;             /* Disnable parity checking */ 
            break;  
        case 'e':  
        case 'E':   
            options.c_cflag |= PARENB;     /* Enable parity */    
            options.c_cflag &= ~PARODD;   
            options.c_iflag |= INPCK;       /* Disnable parity checking */
            break;
        case 'S': 
        case 's':  /*as no parity*/   
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;
            break;  
        default:   
            fprintf(stderr,"Unsupported parity\n");    
            return (FALSE);  
    }  

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
    tcflush(fd,TCIFLUSH);

    options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
    options.c_oflag  &= ~OPOST;   /*Output*/

    options.c_cc[VTIME] = 0; /* how long to block */   
    options.c_cc[VMIN] = 1;  /* at least how many bytes to read */

    /* Update the options and do it NOW */
    if (tcsetattr(fd,TCSANOW,&options) != 0)   
    { 
        printf("SetupSerial failed");   
        return (FALSE);  
    } 
    return (TRUE);  
}

#define PERIOD_STRENGTH 10
#define PERIOD_ALARM    10
#define PERIOD_VPN      2*60

static int quit = 0;
static void quit_signal(int sig) {quit = 1;}
int fd;

static int strength_timer=PERIOD_STRENGTH;
static int vpn_timer=0;
/*tempproally */
#define HOST    "206.191.32.164"
//#define HOST    "192.168.4.1"
#define PORT    59080

int sendinfo(char *buffer)
{
    struct sockaddr_in	sockaddr;
    struct hostent*		host;
    int					outsock;
    FILE*				outfile;
    int					retval;
    fd_set				writefds;
    struct timeval		tvTimeout;

    memset(&sockaddr,0,sizeof(sockaddr));
    sockaddr.sin_family	= AF_INET;
    sockaddr.sin_port	= htons(PORT);

    sockaddr.sin_addr.s_addr = inet_addr(HOST);

    // Open and connect outgoing socket
    outsock = socket(AF_INET,SOCK_DGRAM,0);
    if (outsock < 0) {
        perror("could not create socket");
        return 1;
    }
#if 0
    // Make socket nonblocking
    if (fcntl(outsock, F_SETFL, O_NONBLOCK) < 0) {
        perror("fcntl");
    }

    // Perform a nonblocking connect with a 4-second timeout
    retval = connect(outsock,(struct sockaddr*)&sockaddr,sizeof(sockaddr));
    if (retval < 0 && errno == EINPROGRESS) {
        FD_ZERO(&writefds);
        FD_SET(outsock, &writefds);
        tvTimeout.tv_sec  = 4;
        tvTimeout.tv_usec = 0;
        retval = select(outsock+1, NULL, &writefds, NULL, &tvTimeout);
        if (retval == 0) {
            fprintf(stderr, "connect timed out\n");
            close(outsock);
            return 1;
        } else if (retval < 0) {
            perror("connect");
            fprintf(stderr, "Could not connect to host %s\n", HOST);
            close(outsock);
            return 1;
        }

        // After we have successfully connected, make the socket blocking again,
        // to make the writes more straightforward.
        if (fcntl(outsock, F_SETFL, NULL) < 0) {
            perror("fcntl");
        }
    }
    else if (retval < 0) {
        perror("could not connect to host " HOST);
        close(outsock);
        return 1;
    }

	outfile = fdopen(outsock,"w");
	if (outfile == NULL) {
		perror("could not fdopen output socket");
		close(outsock);
		return 1;
	}

    // Attach a buffered file descriptor to the outgoing socket
    if (outfile == NULL) {
        perror("could not fdopen output socket");
        close(outsock);
        return 1;
    }


    // Transfer data from input file to outgoing socket
    //fwrite(buffer,1,strlen(buffer),outfile);
    send(outsock, buffer, strlen(buffer), 0);
    // Send trailer and close everything down
    fclose(outfile);
    close(outsock);
#endif
    sendto(outsock, buffer, strlen(buffer), 0, (struct sockaddr *)&sockaddr, sizeof(struct sockaddr));
    close(outsock);
    return 0;
}

static void strength_req(int sig)
{
    printf("10sec\n");
    const char *at = "at+csq\r";
    write(fd, at, strlen(at));
}

static void stop_vpn(int sig)
{
    system("/etc/init.d/openvpn stop");
}

static void timer(int sig)
{
    if(strength_timer > 0)
    {
        strength_timer -= PERIOD_ALARM;
        if(strength_timer <= 0)
        {
            //strength_req(sig);
            //start this timer again
            strength_timer = PERIOD_STRENGTH;
        }

    }
    if(vpn_timer > 0)
    {
        vpn_timer -= PERIOD_ALARM;
        if(vpn_timer <= 0)
        {
            stop_vpn(sig);
        }
    }

    alarm(PERIOD_ALARM);
}

static void install_signal_handlers(void)
{
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = quit_signal;
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT , &sa, NULL);
    sigaction(SIGHUP , &sa, NULL);
    signal(SIGALRM, timer);
    signal(SIGUSR2, strength_req);
}
#define BUF_LEN     256

void serialno(char *mac, char *ip)
{
    struct ifreq ifr;
    char achHostname[64];
    int i;

    int netsock = -1;

    /* setup */
    netsock = socket(AF_INET,SOCK_DGRAM,0);

    memset(&ifr,0,sizeof(ifr));
    strcpy(ifr.ifr_name,"br-lan");

    if(ioctl(netsock, SIOCGIFHWADDR, &ifr) == 0)
    {

        sprintf(mac, "%02X%02X%02X%02X%02X%02X", 
                (unsigned)ifr.ifr_hwaddr.sa_data[0]&0x00FF,
                (unsigned)ifr.ifr_hwaddr.sa_data[1]&0x00FF,
                (unsigned)ifr.ifr_hwaddr.sa_data[2]&0x00FF,
                (unsigned)ifr.ifr_hwaddr.sa_data[3]&0x00FF,
                (unsigned)ifr.ifr_hwaddr.sa_data[4]&0x00FF,
                (unsigned)ifr.ifr_hwaddr.sa_data[5]&0x00FF);
    }

    ifr.ifr_addr.sa_family = AF_INET;

    //strcpy(ifr.ifr_name,"3g-wan");

    if (ioctl(netsock, SIOCGIFADDR, &ifr) == 0) {
        sprintf(ip, "%d.%d.%d.%d",
                ((unsigned char*)&((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr.s_addr)[0],
                ((unsigned char*)&((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr.s_addr)[1],
                ((unsigned char*)&((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr.s_addr)[2],
                ((unsigned char*)&((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr.s_addr)[3]
               );
    }
    close(netsock);

    int rx = 0, tx = 0;
    FILE *fp;

    if((fp = fopen("/sys/class/net/3g-wan/statistics/rx_bytes", "rb")) != NULL)
    {    
        fscanf(fp, "%d", &rx);
        fclose(fp);
    }
    if((fp = fopen("/sys/class/net/3g-wan/statistics/tx_bytes", "rb")) != NULL)
    {    
        fscanf(fp, "%d", &tx);
        fclose(fp);
    }
    printf("rx:%d tx:%d\n", rx, tx);
}


int main(int argc, char **argv)
{
    int count = 0;
    int oldcount = -1;
    int nread;
    char buff[BUF_LEN];
    char *dev  = "/dev/ttyUSB3"; 
    char *at;
    int rssi, ber;
    static int flag=0;
    char mac[18];
    char ipaddr[16];
    char linkstat[5];
    char rssi_s[3];
    char ber_s[3];
    char *p;
    int sms_index;

    install_signal_handlers();

    memset(mac, 0, 18);
    memset(ipaddr, 0, 16);

    serialno(mac, ipaddr);


    fd = OpenDev(dev);
    if(fd < 0)
        exit(-1);

    set_speed(fd, 9600);

    if (set_Parity(fd,8,1,'N') == FALSE)  {
        printf("Set Parity Error\n");
        exit (0);
    }

    alarm(PERIOD_ALARM);


    while (1) 
    {   

        memset(buff, 0, BUF_LEN);
        nread = read(fd, buff, BUF_LEN-1);

        tcflush(fd,TCOFLUSH);
        printf("nread=%d ", nread);
        if(nread > 0)
        { 
            //buff[nread+1] = '\0';   
            //printf("\nLen %d\n",nread); 
            printf( "recive=%s\n", buff);
            //if(strncmp(buff, "+CSQ", 4) == 0)
            if((p = strstr(buff, "+CSQ")) != NULL)
            {
                //get the strenth
                sscanf(p, "+CSQ: %d,%d", &rssi, &ber);
#if 1
                FILE *fp = fopen("/tmp/poll_info", "rb");
                //char buf[256];
                if(fp != NULL)
                {
                    memset(buff, 0, BUF_LEN);
                    fread(buff, 1, 256, fp);
                    fclose(fp);
                }
                sprintf(buff+strlen(buff), " %d %d", rssi, ber);
#endif
                //sprintf(buff, "%s %s %s %s %d %d", mac, ipaddr, "POSLynx220", "UXXXXXXX", rssi, ber);
                log(LOG_DEBUG, "send buff=%s\n", buff);
                printf("send buff=%s\n", buff);
                sendinfo(buff);
            }
            //if(strncmp(buff, "+CMTI", 4) == 0)
            else if((p = strstr(buff, "+CMTI:")) != NULL)
            {
                //new SMS mesddsage is comming
                //printf("get the SMS %s\n", buff);
                sscanf(p, "+CMTI: \"SM\", %d", &sms_index);
                memset(buff, 0, BUF_LEN);
                sprintf(buff, "AT+CMGR=%d\r", sms_index);
                at = buff;
                write(fd, at, strlen(at));

                //system("/etc/init.d/openvpn start");
                //start the vpn 10 minutus timer
                //vpn_timer = PERIOD_VPN;
            }
            //get the messages
            else if((p = strstr(buff, "+CMGR:")) != NULL)
            {
                log(LOG_INFO, "Received the SMS message:\n %s", p);

                //delete it to avoid fullfilling the sim card
                //memset(buff, 0, BUF_LEN);
                //sprintf(buff, "AT+CMGD=%d\r", sms_index);
                //sprintf(buff, "AT+CMGD=1, 3", sms_index);
                //at = buff;
                
                //delete all read and sent messages
                at = "AT+CMGD=1, 3\r";
                write(fd, at, strlen(at));

            }
            else
            {
                log(LOG_INFO, "Got the unrecoginized message %s\n", buff);
            }
            //tcflush(fd,TCIOFLUSH);
        }

        if(quit == 1)
            break;
    }

    close(fd);  
    printf("exit!\n");
    exit (0);
}
