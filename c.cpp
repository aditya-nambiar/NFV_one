#include "raw_client.h"

int main(){	
	RawClient client;
	Packet pkt;
	RawClient::set_interface("lo");
	client.fill_client_details(4000, "127.0.0.1");
	client.bind_client();
	client.fill_server_details(5000, "127.0.0.1");
	client.connect_with_server(pkt, 1);
	print("Connection successful with server");
	return 0;
}


// #include <iostream>
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>           // close()
// #include <string.h>           // strcpy, memset(), and memcpy()

// #include <netdb.h>            // struct addrinfo
// #include <sys/types.h>        // needed for socket(), uint8_t, uint16_t, uint32_t
// #include <sys/socket.h>       // needed for socket()
// #include <netinet/in.h>       // IPPROTO_RAW, IPPROTO_IP, IPPROTO_UDP, INET_ADDRSTRLEN
// #include <netinet/ip.h>       // struct ip and IP_MAXPACKET (which is 65535)
// #include <netinet/udp.h>      // struct udphdr
// #include <arpa/inet.h>        // inet_pton() and inet_ntop()
// #include <sys/ioctl.h>        // macro ioctl is defined
// #include <bits/ioctls.h>      // defines values for argument "request" of ioctl.
// #include <net/if.h>           // struct ifreq
// #include <signal.h>

// #include <errno.h>            // errno, perror()

// using namespace std;

// // Define some constants.
// #define IP4_HDRLEN 20         // IPv4 header length
// #define UDP_HDRLEN  8         // UDP header length, excludes data

// // Function prototypes
// uint16_t checksum (uint16_t *, int);
// uint16_t udp4_checksum (struct ip, struct udphdr, uint8_t *, int);
// char *allocate_strmem (int);
// uint8_t *allocate_ustrmem (int);
// int *allocate_intmem (int);

// void report_error(int arg){
// 	if(arg<0){
// 		perror("ERROR");
// 		exit(EXIT_FAILURE);
// 	}
// }

// int main ()
// {
// 	int status, datalen, sd, *ip_flags;
// 	const int on = 1;
// 	char *interface, *target, *src_ip, *dst_ip;
// 	struct ip iphdr;
// 	struct udphdr udphdr;
// 	uint8_t *data, *packet;
// 	struct addrinfo hints, *res;
// 	struct sockaddr_in *ipv4, sin;
// 	struct ifreq ifr;
// 	void *tmp;

//   // Allocate memory for various arrays.
// 	data = allocate_ustrmem (IP_MAXPACKET);
// 	packet = allocate_ustrmem (IP_MAXPACKET);
// 	interface = allocate_strmem (40);
// 	target = allocate_strmem (40);
// 	src_ip = allocate_strmem (INET_ADDRSTRLEN);
// 	dst_ip = allocate_strmem (INET_ADDRSTRLEN);
// 	ip_flags = allocate_intmem (4);

//   // Interface to send packet through.
// 	strcpy (interface, "lo");

//   // Submit request for a socket descriptor to look up interface.
// 	if ((sd = socket (AF_INET, SOCK_RAW, IPPROTO_UDP)) < 0) {
// 		perror ("socket() failed to get socket descriptor for using ioctl() ");
// 		exit (EXIT_FAILURE);
// 	}

//   // Use ioctl() to look up interface index which we will use to
//   // bind socket descriptor sd to specified interface with setsockopt() since
//   // none of the other arguments of sendto() specify which interface to use.
// 	memset (&ifr, 0, sizeof (ifr));
// 	strcpy(ifr.ifr_name, interface);
// 	//snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", interface);
// 	if (ioctl (sd, SIOCGIFINDEX, &ifr) < 0) {
// 		perror ("ioctl() failed to find interface ");
// 		return (EXIT_FAILURE);
// 	}
// 	// close (sd);
// 	printf ("Index for interface %s is %i\n", interface, ifr.ifr_ifindex);

//   // Source IPv4 address: you need to fill this out
// 	strcpy (src_ip, "127.0.0.1");

//   // Destination URL or IPv4 address: you need to fill this out
// 	strcpy (dst_ip, "127.0.0.1");

//   // Fill out hints for getaddrinfo().
// 	// memset (&hints, 0, sizeof (struct addrinfo));
// 	// hints.ai_family = AF_INET;
// 	// hints.ai_socktype = SOCK_STREAM;
// 	// hints.ai_flags = hints.ai_flags | AI_CANONNAME;

//   // Resolve target using getaddrinfo().
// 	/*if ((status = getaddrinfo (target, NULL, &hints, &res)) != 0) {
// 		fprintf (stderr, "getaddrinfo() failed: %s\n", gai_strerror (status));
// 		exit (EXIT_FAILURE);
// 	}
// 	ipv4 = (struct sockaddr_in *) res->ai_addr;
// 	tmp = &(ipv4->sin_addr);
// 	if (inet_ntop (AF_INET, tmp, dst_ip, INET_ADDRSTRLEN) == NULL) {
// 		status = errno;
// 		fprintf (stderr, "inet_ntop() failed.\nError message: %s", strerror (status));
// 		exit (EXIT_FAILURE);
// 	}
// 	freeaddrinfo (res);*/

//   // UDP data
//   // IPv4 header
// 	datalen = 4 + UDP_HDRLEN;
// 	char tem[4];
// 	strcpy(tem, "Test");

// 	memcpy (data, &udphdr, UDP_HDRLEN * sizeof (uint8_t));
// 	memcpy (data + 8, tem, 4);


//   // IPv4 header length (4 bits): Number of 32-bit words in header = 5
// 	iphdr.ip_hl = IP4_HDRLEN / sizeof (uint32_t);

//   // Internet Protocol version (4 bits): IPv4
// 	iphdr.ip_v = 4;

//   // Type of service (8 bits)
// 	iphdr.ip_tos = 0;

//   // Total length of datagram (16 bits): IP header + UDP header + datalen
// 	iphdr.ip_len = htons (IP4_HDRLEN + UDP_HDRLEN + datalen);

//   // ID sequence number (16 bits): unused, since single datagram
// 	iphdr.ip_id = htons (0);

//   // Flags, and Fragmentation offset (3, 13 bits): 0 since single datagram

//   // Zero (1 bit)
// 	ip_flags[0] = 0;

//   // Do not fragment flag (1 bit)
// 	ip_flags[1] = 0;

//   // More fragments following flag (1 bit)
// 	ip_flags[2] = 0;

//   // Fragmentation offset (13 bits)
// 	ip_flags[3] = 0;

// 	iphdr.ip_off = htons ((ip_flags[0] << 15)
// 		+ (ip_flags[1] << 14)
// 		+ (ip_flags[2] << 13)
// 		+  ip_flags[3]);

//   // Time-to-Live (8 bits): default to maximum value
// 	iphdr.ip_ttl = 255;

//   // Transport layer protocol (8 bits): 17 for UDP
// 	iphdr.ip_p = IPPROTO_UDP;

//   // Source IPv4 address (32 bits)
// 	if ((status = inet_pton (AF_INET, src_ip, &(iphdr.ip_src))) != 1) {
// 		fprintf (stderr, "inet_pton() failed.\nError message: %s", strerror (status));
// 		exit (EXIT_FAILURE);
// 	}

//   // Destination IPv4 address (32 bits)
// 	if ((status = inet_pton (AF_INET, dst_ip, &(iphdr.ip_dst))) != 1) {
// 		fprintf (stderr, "inet_pton() failed.\nError message: %s", strerror (status));
// 		exit (EXIT_FAILURE);
// 	}

//   // IPv4 header checksum (16 bits): set to 0 when calculating checksum
// 	iphdr.ip_sum = 0;
// 	iphdr.ip_sum = checksum ((uint16_t *) &iphdr, IP4_HDRLEN);

//   // UDP header

//   // Source port number (16 bits): pick a number
// 	udphdr.source = htons (4950);

//   // Destination port number (16 bits): pick a number
// 	udphdr.dest = htons (5000);

//   // Length of UDP datagram (16 bits): UDP header + UDP data
// 	udphdr.len = htons (UDP_HDRLEN + datalen);

//   // UDP checksum (16 bits)
// 	udphdr.check = udp4_checksum (iphdr, udphdr, data, datalen);

//   // Prepare packet.

//   // First part is an IPv4 header.
// 	memcpy (packet, &iphdr, IP4_HDRLEN * sizeof (uint8_t));

//   // Next part of packet is upper layer protocol header.
// 	memcpy ((packet + IP4_HDRLEN), &udphdr, UDP_HDRLEN * sizeof (uint8_t));

// 	// memcpy (packet + IP4_HDRLEN + UDP_HDRLEN, &iphdr, IP4_HDRLEN * sizeof (uint8_t));

//  //  // Next part of packet is upper layer protocol header.
// 	// memcpy ((packet + IP4_HDRLEN + IP4_HDRLEN + UDP_HDRLEN), &udphdr, UDP_HDRLEN * sizeof (uint8_t));


//  //  // Finally, add the UDP data.
// //	memcpy (packet + 2*(IP4_HDRLEN + UDP_HDRLEN), data, datalen * sizeof (uint8_t));
// 	memcpy (packet + IP4_HDRLEN + UDP_HDRLEN, data, datalen * sizeof (uint8_t));
// 	//memcpy (packet + IP4_HDRLEN + UDP_HDRLEN + datalen, &udphdr, UDP_HDRLEN * sizeof (uint8_t));

//   // The kernel is going to prepare layer 2 information (ethernet frame header) for us.
//   // For that, we need to specify a destination for the kernel in order for it
//   // to decide where to send the raw datagram. We fill in a struct in_addr with
//   // the desired destination IP address, and pass this structure to the sendto() function.
// 	memset (&sin, 0, sizeof (struct sockaddr_in));
// 	sin.sin_family = AF_INET;
// 	sin.sin_addr.s_addr = iphdr.ip_dst.s_addr;
// 	sin.sin_port = udphdr.dest;



// 	cout<<ntohs(sin.sin_port)<<endl;
//   // Submit request for a raw socket descriptor.
// 	// if ((sd = socket (AF_INET, SOCK_RAW, IPPROTO_UDP)) < 0) {
// 	// 	perror ("socket() failed ");
// 	// 	exit (EXIT_FAILURE);
// 	// }

//   // Set flag so socket expects us to provide IPv4 header.
// 	if (setsockopt (sd, IPPROTO_IP, IP_HDRINCL, &on, sizeof (on)) < 0) {
// 		perror ("setsockopt() failed to set IP_HDRINCL ");
// 		exit (EXIT_FAILURE);
// 	}

//   // Bind socket to interface index.
// 	if (setsockopt (sd, SOL_SOCKET, SO_BINDTODEVICE, &ifr, sizeof (ifr)) < 0) {
// 		perror ("setsockopt() failed to bind to interface ");
// 		exit (EXIT_FAILURE);
// 	}

// 	socklen_t g_addr_len;
// 	int client_port;
// 	int g_reuse;
// 	const char *client_address;
// 	sockaddr_in client_sock_addr;

// 	g_addr_len = sizeof(sockaddr_in);
// 	client_port = 4950;
// 	client_address = "127.0.0.1";
// 	bzero((char *) &client_sock_addr, sizeof(client_sock_addr));
// 	client_sock_addr.sin_family = AF_INET;  	
// 	client_sock_addr.sin_port = htons(client_port); 
// 	inet_aton(client_address, &client_sock_addr.sin_addr);
// 	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &g_reuse, sizeof(int));
// 	signal(SIGPIPE, SIG_IGN);	  
// 	status = bind(sd, (struct sockaddr*)&client_sock_addr, sizeof(client_sock_addr));
// 	report_error(status);	


// 	// Send packet.
// 	// status = sendto (sd, packet, 2*(IP4_HDRLEN + UDP_HDRLEN) + datalen, 0, (struct sockaddr *) &sin, sizeof(struct sockaddr));
// 	status = sendto(sd, packet, IP4_HDRLEN + 2*UDP_HDRLEN + datalen, 0, (struct sockaddr *) &sin, sizeof(struct sockaddr));
// 	report_error(status);
// 	cout<<"Message sent"<<endl;

//   // Close socket descriptor.
// 	close (sd);

//   // Free allocated memory.
// 	free (data);
// 	free (packet);
// 	free (interface);
// 	free (target);
// 	free (src_ip);
// 	free (dst_ip);
// 	free (ip_flags);

// 	return 0;
// }

// // Computing the internet checksum (RFC 1071).
// // Note that the internet checksum does not preclude collisions.
// uint16_t
// checksum (uint16_t *addr, int len)
// {
// 	int count = len;
// 	register uint32_t sum = 0;
// 	uint16_t answer = 0;

//   // Sum up 2-byte values until none or only one byte left.
// 	while (count > 1) {
// 		sum += *(addr++);
// 		count -= 2;
// 	}

//   // Add left-over byte, if any.
// 	if (count > 0) {
// 		sum += *(uint8_t *) addr;
// 	}

//   // Fold 32-bit sum into 16 bits; we lose information by doing this,
//   // increasing the chances of a collision.
//   // sum = (lower 16 bits) + (upper 16 bits shifted right 16 bits)
// 	while (sum >> 16) {
// 		sum = (sum & 0xffff) + (sum >> 16);
// 	}

//   // Checksum is one's compliment of sum.
// 	answer = ~sum;

// 	return (answer);
// }

// // Build IPv4 UDP pseudo-header and call checksum function.
// uint16_t
// udp4_checksum (struct ip iphdr, struct udphdr udphdr, uint8_t *payload, int payloadlen)
// {
// 	char buf[IP_MAXPACKET];
// 	char *ptr;
// 	int chksumlen = 0;
// 	int i;

//   ptr = &buf[0];  // ptr points to beginning of buffer buf

//   // Copy source IP address into buf (32 bits)
//   memcpy (ptr, &iphdr.ip_src.s_addr, sizeof (iphdr.ip_src.s_addr));
//   ptr += sizeof (iphdr.ip_src.s_addr);
//   chksumlen += sizeof (iphdr.ip_src.s_addr);

//   // Copy destination IP address into buf (32 bits)
//   memcpy (ptr, &iphdr.ip_dst.s_addr, sizeof (iphdr.ip_dst.s_addr));
//   ptr += sizeof (iphdr.ip_dst.s_addr);
//   chksumlen += sizeof (iphdr.ip_dst.s_addr);

//   // Copy zero field to buf (8 bits)
//   *ptr = 0; ptr++;
//   chksumlen += 1;

//   // Copy transport layer protocol to buf (8 bits)
//   memcpy (ptr, &iphdr.ip_p, sizeof (iphdr.ip_p));
//   ptr += sizeof (iphdr.ip_p);
//   chksumlen += sizeof (iphdr.ip_p);

//   // Copy UDP length to buf (16 bits)
//   memcpy (ptr, &udphdr.len, sizeof (udphdr.len));
//   ptr += sizeof (udphdr.len);
//   chksumlen += sizeof (udphdr.len);

//   // Copy UDP source port to buf (16 bits)
//   memcpy (ptr, &udphdr.source, sizeof (udphdr.source));
//   ptr += sizeof (udphdr.source);
//   chksumlen += sizeof (udphdr.source);

//   // Copy UDP destination port to buf (16 bits)
//   memcpy (ptr, &udphdr.dest, sizeof (udphdr.dest));
//   ptr += sizeof (udphdr.dest);
//   chksumlen += sizeof (udphdr.dest);

//   // Copy UDP length again to buf (16 bits)
//   memcpy (ptr, &udphdr.len, sizeof (udphdr.len));
//   ptr += sizeof (udphdr.len);
//   chksumlen += sizeof (udphdr.len);

//   // Copy UDP checksum to buf (16 bits)
//   // Zero, since we don't know it yet
//   *ptr = 0; ptr++;
//   *ptr = 0; ptr++;
//   chksumlen += 2;

//   // Copy payload to buf
//   memcpy (ptr, payload, payloadlen);
//   ptr += payloadlen;
//   chksumlen += payloadlen;

//   // Pad to the next 16-bit boundary
//   for (i=0; i<payloadlen%2; i++, ptr++) {
//   	*ptr = 0;
//   	ptr++;
//   	chksumlen++;
//   }

//   return checksum ((uint16_t *) buf, chksumlen);
// }

// // Allocate memory for an array of chars.
// char *
// allocate_strmem (int len)
// {
// 	char *tmp;
// 	if (len <= 0) {
// 		fprintf (stderr, "ERROR: Cannot allocate memory because len = %i in allocate_strmem().\n", len);
// 		exit (EXIT_FAILURE);
// 	}
// 	tmp = (char *) malloc (len * sizeof (char));
// 	if (tmp != NULL) {
// 		memset (tmp, 0, len * sizeof (char));
// 		return (tmp);
// 	} else {
// 		fprintf (stderr, "ERROR: Cannot allocate memory for array allocate_strmem().\n");
// 		exit (EXIT_FAILURE);
// 	}
// }

// // Allocate memory for an array of unsigned chars.
// uint8_t *
// allocate_ustrmem (int len)
// {
// 	uint8_t *tmp;

// 	if (len <= 0) {
// 		fprintf (stderr, "ERROR: Cannot allocate memory because len = %i in allocate_ustrmem().\n", len);
// 		exit (EXIT_FAILURE);
// 	}

// 	tmp = (uint8_t *) malloc (len * sizeof (uint8_t));
// 	if (tmp != NULL) {
// 		memset (tmp, 0, len * sizeof (uint8_t));
// 		return (tmp);
// 	} else {
// 		fprintf (stderr, "ERROR: Cannot allocate memory for array allocate_ustrmem().\n");
// 		exit (EXIT_FAILURE);
// 	}
// }

// // Allocate memory for an array of ints.
// int *
// allocate_intmem (int len)
// {
// 	int *tmp;

// 	if (len <= 0) {
// 		fprintf (stderr, "ERROR: Cannot allocate memory because len = %i in allocate_intmem().\n", len);
// 		exit (EXIT_FAILURE);
// 	}

// 	tmp = (int *) malloc (len * sizeof (int));
// 	if (tmp != NULL) {
// 		memset (tmp, 0, len * sizeof (int));
// 		return (tmp);
// 	} else {
// 		fprintf (stderr, "ERROR: Cannot allocate memory for array allocate_intmem().\n");
// 		exit (EXIT_FAILURE);
// 	}
// }
