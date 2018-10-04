/* FileName:myping.cpp
 * Author:Hover
 * E-Mail:hover@hust.edu.cn
 * GitHub:HoverWings
 * Description:the Hover's impletation of ping 
 * Attention: you may need sudo to run this code
 */
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <vector>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <setjmp.h>
#include <errno.h>

using namespace std;

#define MAX_WAIT_TIME 5
#define PACKET_SIZE 4096              
           
int max_no_packets=5;
int interval=1;
char sendpacket[PACKET_SIZE];    
char recvpacket[PACKET_SIZE];    

pid_t   pid;
int sockfd;
int datalen = 56;    
int nsend = 0;       
int nreceived = 0;   

struct sockaddr_in    dest_addr;        
struct sockaddr_in    from_addr;        
struct timeval    tvrecv;
vector<double> rtt_vec;
void statistics(int signo);
unsigned short cal_chksum(unsigned short *addr,int len);
int pack(int pack_no);
void send_packet(void);
void recv_packet(void);
int unpack(char *buf,int len);
void timediff(struct timeval *out,struct timeval *in);


bool opt_t = false; // set ttl
bool opt_i = false; // interval

void Stop(int signo)
{
    statistics(signo);
    _exit(0);
}


int main(int argc,char *argv[])
{
    signal(SIGINT, Stop); //set exit function
    char opt;
    int option_index = 0;
    static struct option long_options[] =
    {
        {"help",    no_argument, NULL, 'h'}
    };
    char str[256];
    strcpy(str,argv[1]);
    while ((opt = getopt_long(argc, argv, "t:i:h", long_options, &option_index)) != -1)
    {
        //printf("%c",opt);
        //cout<<argv[optind - 1];
        switch (opt)
        {
            case 't':
                max_no_packets=atoi(argv[optind - 1]);
                opt_t = true;
                break;
            case 'i':
                interval==atoi(argv[optind - 1]);
                opt_i = true;
                break;
            case 'h':
                opt_i = true;
                break;
        }
    }
    struct hostent  *host;        //host entry
    struct protoent *protocol;
    unsigned long    inaddr = 0l;
    int    size = 50*1024;        //50k

    if(argc < 2)
    {
        printf("use : %s hostname/IP address \n", argv[0]);
        exit(1);
    }
    if((protocol = getprotobyname("icmp")) == NULL)
    {
        perror("getprotobyname");
        exit(1);
    }
    // setuid(getpid());
    // need root to create socket
    if((sockfd = socket(AF_INET, SOCK_RAW, protocol->p_proto)) < 0){
            perror("socket error");
            exit(1);
    }

    setuid(getuid());    // recycle root privilage

    // case: broadcast address then there will be a lot of reply 
    // so the buf need enough size
    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size) );
    bzero(&dest_addr, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;

    // domain or address judge
    printf("%s",str);
    if((inaddr=inet_addr(str)) == INADDR_NONE)
    {
        if((host = gethostbyname(str)) == NULL)
        {
            perror("gethostbyname error");
            exit(1);
        }
        memcpy((char*)&dest_addr.sin_addr, host->h_addr, host->h_length);
    }
    else 
    {
        memcpy((char*)&dest_addr.sin_addr, (char*)&inaddr, sizeof(inaddr));
    }
    pid = getpid();        
    printf("PING %s(%s): %d bytes data in ICMP packets.\n",argv[1], inet_ntoa(dest_addr.sin_addr), datalen);

    send_packet();     
    statistics(SIGALRM);    
    return 0;
}


void statistics(int signo)
{
    printf("\n--------------------PING statistics-------------------\n");
    printf("%d packets transmitted, %d received , %%%d lost\n",nsend, nreceived, (nsend-nreceived)/nsend*100);
    printf("rtt min/avg/max/mdev = ");
    
    sort(rtt_vec.begin(), rtt_vec.end());
    double min=rtt_vec.front();
    double max=rtt_vec[rtt_vec.size()-1];
    double total;
    for(vector<double>::iterator iter=rtt_vec.begin();iter!=rtt_vec.end();iter++)
    {
        //  cout << (*iter) << endl;
        total+=*iter;
    }
    double avg=total/nsend;
    double mdev=max-min;
    cout<<fixed<<setprecision(3) <<min<<"/"<<avg<<"/"<<max<<"/"<<mdev<<"ms"<<endl;

    close(sockfd);
    exit(1);
}
/*
I:  addr: check data buffer
    check data len(byte)
*/
unsigned short cal_chksum(unsigned short *addr,int len)
{
    int sum=0;
    int nleft = len;
    unsigned short *w = addr;
    unsigned short answer = 0;
    while(nleft > 1)
    {
            sum += *w++;
            nleft -= 2;
    }
    //if the ICMP head len is odd, then the final data is high bit and add it 
    if(nleft == 1)
    {
            *(unsigned char *)(&answer) = *(unsigned char *)w;
            sum += answer;    /* 这里将 answer 转换成 int 整数 */
    }
    sum = (sum >> 16) + (sum & 0xffff);     // add high bit and low bit
    sum += (sum >> 16);                     // add overflow
    answer = ~sum;                          // 16 bit checksum

    return answer;
}

/*
I:  icmp struct
    sequence
O:  packed icmp
*/
int pack(int pack_no)
{
    int packsize;
    struct icmp    *icmp;
    struct timeval *tval;

    icmp = (struct icmp*)sendpacket;
    icmp->icmp_type = ICMP_ECHO;    // type of service
    icmp->icmp_code = 0;            
    icmp->icmp_cksum = 0;           
    icmp->icmp_seq = pack_no;       
    icmp->icmp_id = pid;            
    packsize = 8 + datalen;         //64=8(head)+56

    tval = (struct timeval *)icmp->icmp_data;    /* 获得icmp结构中最后的数据部分的指针 */
    gettimeofday(tval, NULL); /* 将发送的时间填入icmp结构中最后的数据部分 */

    icmp->icmp_cksum = cal_chksum((unsigned short *)icmp, packsize);/*填充发送方的校验和*/

    return packsize;
}

void send_packet()
{
    int packetsize;
    while(nsend < max_no_packets)
    {   
        nsend++;
        packetsize = pack(nsend); // set ICMP message head
        if(sendto(sockfd, sendpacket, packetsize, 0,(struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
        {
            perror("sendto error");
            continue;
        }
        recv_packet();
        sleep((uint)interval); 
    }
}

void recv_packet()
{
    int n;
    extern int errno;
    signal(SIGALRM,statistics);
    int from_len = sizeof(from_addr);        

    while(nreceived < nsend)
    {
        alarm(MAX_WAIT_TIME);
        if((n = recvfrom(sockfd, recvpacket, sizeof(recvpacket), 0,(struct sockaddr *)&from_addr, (socklen_t *)&from_len)) < 0)
        {
                if(errno == EINTR)
                {
                    continue;
                }
                perror("recvfrom error");
                continue;
        }
        gettimeofday(&tvrecv, NULL); // get receive time
        if(unpack(recvpacket, n) == -1)
            continue;
        nreceived++;
    }
}

/*
I:buf  IP buf
  len  IP message len
  addr ICMP dest address
O:error code
*/ 
int unpack(char *buf, int len)
{
        int iphdrlen;
        struct ip *ip;
        struct icmp *icmp;
        struct timeval *tvsend;
        double rtt;

        ip = (struct ip *)buf;
        iphdrlen = ip->ip_hl << 2;               //ip head len

        icmp = (struct icmp *)(buf + iphdrlen);  // seek to IP message
        len -= iphdrlen;                        
        if(len < 8)                              // less than ICMP head len
        {                           
                printf("ICMP packets\'s length is less than 8\n");
                return -1;
        }
        // check ICMP reply 
        if((icmp->icmp_type == ICMP_ECHOREPLY) && (icmp->icmp_id == pid))
        {
                tvsend = (struct timeval *)icmp->icmp_data;
                timediff(&tvrecv, tvsend);   
                rtt = tvrecv.tv_sec * 1000 + tvrecv.tv_usec / 1000;
                rtt_vec.push_back(rtt);
                printf("%d byte from %s: icmp_seq=%u ttl=%d time=%.3f ms\n",
                        len,                                // total message len
                        inet_ntoa(from_addr.sin_addr),    
                        icmp->icmp_seq,                     
                        ip->ip_ttl,                   
                        rtt);                               //ms rtt
                return 0;
        }
        else
            return -1;
}


/*
I:begin_time
  endtime
O:ms diff
*/
void timediff(struct timeval *recv, struct timeval *send)
{
    if((recv->tv_usec -= send->tv_usec) < 0)
    {
            --recv->tv_sec;
            recv->tv_usec += 1000000;
    }
    recv->tv_sec -= send->tv_sec;
}