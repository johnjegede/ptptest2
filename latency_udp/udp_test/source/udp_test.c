// Copyright (c) 2016, Noa Zilberman
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the project, the copyright holder nor the names of its
//  contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.



#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sched.h>
#include "tsc.h"


#define CPU_MHZ_FILE        "/proc/cpuinfo"
#define CPU_MHZ_PREFIX        "cpu MHz\t\t: "

#define DEFAULT_CPU 2
#define SAMPLES_MAX 100000
#define OUTPUT_FILE "out.txt"
#define DEFAULT_PORT 5555

//read clock frequency from cpuinfo
unsigned int get_mhz(void)
{
    FILE *fp;
    char line[1024];
    unsigned int clock_mhz = 0;
    
    fp = fopen("/proc/cpuinfo", "r");
    if (!fp)
        return 0;

    while (fgets(line, sizeof(line), fp))
    {
        if (strncmp(line, CPU_MHZ_PREFIX, sizeof(CPU_MHZ_PREFIX)-1) == 0)
        {
            clock_mhz = atoi(line+sizeof(CPU_MHZ_PREFIX)-1);
            break;
        }
    }
    fclose(fp);
    return clock_mhz;
}

//pin the task to a specific core
static void pin_cpu(int cpu)
{
        cpu_set_t cpu_set;
        CPU_ZERO(&cpu_set);
        CPU_SET(cpu, &cpu_set);
        if (sched_setaffinity(0, sizeof(cpu_set), &cpu_set) == -1)
                perror("sched_setaffinity");
}

static int compare_ull(const void *pa, const void *pb)
{
        unsigned long long a = *(const unsigned long long *)pa;
        unsigned long long b = *(const unsigned long long *)pb;
        return (a < b) ? -1 : (a > b) ? +1 : 0;
}



//handle the udp message
int udp_msg(struct sockaddr_in udp_hdr, struct sockaddr_in addr, int sock, unsigned char *data )
{
         int rc;
        struct timeval timeout = {2, 0}; //wait max 3 seconds for a reply
        fd_set read_set;
        socklen_t slen;
        struct sockaddr_in rcv_hdr;
       rc = sendto(sock, data, sizeof udp_hdr + 8,
                        0, (struct sockaddr*)&addr, sizeof addr);
        if (rc <= 0) {
            perror("Sendto");
            return -1;
        }
        memset(&read_set, 0, sizeof read_set);
        FD_SET(sock, &read_set);
        
        //wait for a reply with a timeout
        //Yes, busy polling is faster but not representing
        rc = select(sock + 1, &read_set, NULL, NULL, &timeout);
        if (rc == 0) {
            puts("Got no reply\n");
           return -1;
        } else if (rc < 0) {
            perror("Select");
            return -1;
        }

        //we don't care about the sender address in this example..
        slen = 0;
        rc = recvfrom(sock, data, sizeof data, 0, NULL, &slen);
        if (rc <= 0) {
            perror("recvfrom");
            return -1;
        } else if (rc < sizeof rcv_hdr) {
           // printf("Error, got short UDP packet, %d bytes rc, %d bytes rcv__hdr\n", rc,sizeof rcv_hdr);
           // return -1;
        }
        memcpy(&rcv_hdr, data, sizeof rcv_hdr);
    return 0;
}

//Measure round trip time
void measure_rtt(struct in_addr *dst,unsigned int port, unsigned long long *rtt,unsigned int samples_num, unsigned int cpu)
{
    int i;
    struct sockaddr_in udp_hdr;
    struct sockaddr_in addr;
    unsigned long long end, start;
    int sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if (sock < 0) {
        perror("socket");
        return ;
    }

    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr = *dst;
    addr.sin_port = port; 

    memset(&udp_hdr, 0, sizeof udp_hdr);

    for (i=0;i<samples_num;i++) {
        unsigned char data[2048];
        int rc;
        struct timeval timeout = {2, 0}; //wait max 3 seconds for a reply
        fd_set read_set;
        socklen_t slen;
        struct sockaddr_in rcv_hdr;
        char sequence[8];
        
        sprintf(sequence,"%08X",i);
        memcpy(data, &udp_hdr, sizeof udp_hdr);
        memcpy(data + sizeof udp_hdr, sequence, 8); // packet payload
        rdtscpll(start,cpu);
        udp_msg(udp_hdr,addr,sock,data);
        rdtscpll(end,cpu);
        rtt[i]=end-start;
    }
}

//In server mode, start listening 
void start_server (unsigned int port)
{
    
    struct sockaddr_in client;
    struct sockaddr_in server;
    unsigned int serverlen, clientlen, replylen;
    int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    int received=0;
    char buffer[1536];

    if (sock < 0) {
        perror("socket");
        return ;
     }

     memset(&server, 0, sizeof(server));     
     server.sin_family = AF_INET;                 
     server.sin_addr.s_addr = htonl(INADDR_ANY);  
     server.sin_port = port;       /* server port */

          /* Bind the socket */
     serverlen = sizeof(server);
     if (bind(sock, (struct sockaddr *) &server, serverlen) < 0) {
            perror("Failed to bind server socket");
          }
     fprintf(stderr, "Bounded socket, port %d\n",port);
     while (1) {
         /* Receive a message from the client */
          clientlen = sizeof(client);
          if ((received = recvfrom(sock, buffer, 1536, 0, (struct sockaddr *) &client, &clientlen)) < 0) {
                perror("Failed to receive message");
          }
           /* Send the message back to client */
           if (sendto(sock, buffer, received, 0, (struct sockaddr *) &client, sizeof(client)) != received) {
                perror("Mismatch in number of replyed bytes");
              }
            }
          
}

int main(int argc, char **argv)
{
    int flag;
    struct in_addr dst;
    char *fname;
    unsigned int samples_num; 
    unsigned int cpu;
    unsigned long long *rtt;
    unsigned long long start, end;
    unsigned int clock_mhz;
    unsigned int port;
    int server;
    FILE *fp;

     // Setting default values
     cpu= DEFAULT_CPU;
     server = 0;
     port= DEFAULT_PORT;
     samples_num = SAMPLES_MAX;
     fname - OUTPUT_FILE;

  while ((flag = getopt (argc, argv, "si:p:f:n:c:")) != -1)
    switch (flag)
      {
      case 'i':
        if (inet_aton(optarg, &dst) == 0) {
	        perror("inet_aton");
        	printf("%s isn't a valid IP address\n", optarg);
        return 1;
         }
        break;
      case 'f':
        fname = optarg;
        break;
      case 'n':
         samples_num = atoi(optarg);
         break;
      case 'p':
          port = atoi(optarg);
          break;
      case 'c':
         cpu = atoi(optarg);
         break;
      case 's':
	 server = 1;
	 break;
      default:
          printf("\nusage: ./udp_test -i <ip address> -p <port> -f <output file> -n <number of samples> ");
          abort; 
      }
    
    if (server) {
         fprintf(stderr, "Entered server mode\n");
         pin_cpu(cpu);
         start_server(port);   
    }
    else {

    fp = fopen(fname, "w");
    if (!fp)
        return 0;
    
    rtt=malloc(samples_num*sizeof(unsigned long long*));
    if (mlock(rtt,sizeof(rtt)))
           {
            free(rtt);
            return (0);
           }


    clock_mhz = get_mhz();
    if (!clock_mhz)
    {
        fprintf(stderr, "Could not determine clock rate from " CPU_MHZ_FILE "\n");
        return 0;
    }

    pin_cpu(cpu);
    measure_rtt(&dst,port,rtt,samples_num,cpu);
    
    int i;
    for (i=0;i<samples_num;i++){
	fprintf(fp,"%llu\n",rtt[i]/clock_mhz);
    }
    qsort(rtt, samples_num, sizeof(rtt[0]), compare_ull);
        fprintf(fp,"cpu clock frequency\t\t%u\n",clock_mhz);
	fprintf(fp,"number of samples: %d\n",samples_num);
        fprintf(fp,"min\t\t%llu\t\t%llu\n" "1%%\t\t%llu\t\t%llu\n" "5%%\t\t%llu\t\t%llu\n" "10%%\t\t%llu\t\t%llu\n" "25%%\t\t%llu\t\t%llu\n" "median\t\t%llu\t\t%llu\n" "75%%\t\t%llu\t\t%llu\n" "90%%\t\t%llu\t\t%llu\n" "95%%\t\t%llu\t\t%llu\n" "99%%\t\t%llu\t\t%llu\n" "99.9%%\t\t%llu\t\t%llu\n"  "max\t\t%llu\t\t%llu\n",
                rtt[0]/clock_mhz, rtt[0]*1000/clock_mhz, 
                rtt[samples_num*1/100]/clock_mhz, rtt[samples_num*1/100]*1000/clock_mhz,
                rtt[samples_num*5/100]/clock_mhz,rtt[samples_num*5/100]*1000/clock_mhz,
                rtt[samples_num*10/100]/clock_mhz, rtt[samples_num*10/100]*1000/clock_mhz, 
                rtt[samples_num*25/100]/clock_mhz, rtt[samples_num*25/100]*1000/clock_mhz,
                rtt[samples_num*50/100]/clock_mhz, rtt[samples_num*50/100]*1000/clock_mhz,
                rtt[samples_num*75/100]/clock_mhz, rtt[samples_num*75/100]*1000/clock_mhz,
                rtt[samples_num*90/100]/clock_mhz, rtt[samples_num*90/100]*1000/clock_mhz, 
                rtt[samples_num*95/100]/clock_mhz, rtt[samples_num*95/100]*1000/clock_mhz,
                rtt[samples_num*99/100]/clock_mhz, rtt[samples_num*99/100]*1000/clock_mhz,
                rtt[samples_num*999/1000]/clock_mhz,rtt[samples_num*999/1000]*1000/clock_mhz, 
                rtt[samples_num-1]/clock_mhz,  rtt[samples_num-1]*1000/clock_mhz);
                munlock(rtt, sizeof(rtt));
         fclose(fp);
         return 0;
  }
}


