/*******************************
udp_client.c: the source file of the client in udp
********************************/

#include "headsock.h"

float str_cli1(FILE *fp, int sockfd, struct sockaddr *addr, int addrlen, int *len);
void tv_sub(struct timeval *out, struct timeval *in); //calcu the time interval between out and in

int main(int argc, char *argv[])
{
	int len, sockfd;
	struct sockaddr_in ser_addr;
	char **pptr;
	float ti, rt;
	struct hostent *sh;
	struct in_addr **addrs;
	FILE *fp;

	if (argc != 2)
	{
		printf("parameters not match.");
		exit(0);
	}

	if ((sh = gethostbyname(argv[1])) == NULL)
	{ //get host's information
		printf("error when gethostbyname");
		exit(0);
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, 0); //create socket
	if (sockfd < 0)
	{
		printf("error in socket");
		exit(1);
	}

	addrs = (struct in_addr **)sh->h_addr_list;
	printf("canonical name: %s\n", sh->h_name);
	for (pptr = sh->h_aliases; *pptr != NULL; pptr++)
		printf("the aliases name is: %s\n", *pptr); //printf socket information
	switch (sh->h_addrtype)
	{
	case AF_INET:
		printf("AF_INET\n");
		break;
	default:
		printf("unknown addrtype\n");
		break;
	}

	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(MYUDP_PORT);
	memcpy(&(ser_addr.sin_addr.s_addr), *addrs, sizeof(struct in_addr));
	bzero(&(ser_addr.sin_zero), 8);

	// Open File
	if ((fp = fopen("myfile.txt", "r+t")) == NULL)
	{
		printf("File doesn't exit\n");
		exit(0);
	}

	ti = str_cli1(fp, sockfd, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr_in), &len); // receive and send
	if (ti != -1)	{
		rt = (len / (float)ti);
		printf("Time(ms) : %.3f, Data sent(byte): %d\nData rate: %f (Kbytes/s)\n", ti, (int)len, rt);
	}
	close(sockfd);
	fclose(fp);

	exit(0);
}

float str_cli1(FILE *fp, int sockfd, struct sockaddr *addr, int addrlen, int *len)
{
	char *buf;
	long lsize;
	char sends[DATALEN];
	struct ack_so ack;
	int n, slen, ci;
	float time_inv = 0.0;
	struct timeval sendt, recvt;
	ci = 0;

	fseek(fp, 0, SEEK_END);
	lsize = *len = ftell(fp);
	rewind(fp);
	printf("The file length is %d bytes\n", (int)lsize);
	printf("the packet length is %d bytes\n", DATALEN);

	// allocate memory to contain the whole file.
	buf = (char *)malloc(lsize);
	if (buf == NULL)
		exit(2);

	// copy the file into the buffer.
	fread(buf, 1, lsize, fp);

	/*** the whole file is loaded in the buffer. ***/
	buf[lsize] = '\0';			//append the end byte
	
	while (ci <= lsize)
	{
		gettimeofday(&sendt, NULL); //get the current time

		if ((lsize + 1 - ci) <= DATALEN)
			slen = lsize + 1 - ci;
		else
			slen = DATALEN;
		memcpy(sends, (buf + ci), slen);
		n = sendto(sockfd, &sends, slen, 0, addr, addrlen); //send the packet to server
		if (n == -1) {
			printf("send error!\n"); //send the data
			exit(1);
		}
		else printf("%d data sent\n", n);
		if ((n = recvfrom(sockfd, &ack, 2, 0, addr, &addrlen)) == -1) //receive the ack
		{
			printf("error when receiving\n");
			exit(1);
		}
		if ((ack.len == 0) && (ack.num == 1)) //if it is ACK
		{
			gettimeofday(&recvt, NULL); //get current time
			tv_sub(&recvt, &sendt);		// get the whole trans time
			time_inv += (recvt.tv_sec) * 1000.0 + (recvt.tv_usec) / 1000.0;
			printf("Time(ms) : %.3f\n", (float)time_inv);
		}
		else
		{
			return (-1);
			printf("Error in transmission\n");
		}

		ci += slen;
	}
	return time_inv;
}

void tv_sub(struct timeval *out, struct timeval *in)
{
	if ((out->tv_usec -= in->tv_usec) < 0)
	{
		--out->tv_sec;
		out->tv_usec += 1000000;
	}
	out->tv_sec -= in->tv_sec;
}
