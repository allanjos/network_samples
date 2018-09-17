#include <stdio.h>
#include <pcap.h>
#include <netinet/in.h>
#include <net/ethernet.h>

#define SIZE_ETHERNET 14

void packet_capture_callback(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

int main(int argc, char *argv[])
{
    char *dev = argv[1];
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;
    struct bpf_program fp;		/* The compiled filter expression */
    char filter_exp[] = "port 80";	/* The filter expression */
    bpf_u_int32 mask;		/* The netmask of our sniffing device */
    bpf_u_int32 net;		/* The IP of our sniffing device */
    struct pcap_pkthdr header;	/* The header that pcap gives us */
    const u_char *packet;		/* The actual packet */
    const struct sniff_ethernet *ethernet; /* The ethernet header */
	const struct sniff_ip *ip; /* The IP header */
	const struct sniff_tcp *tcp; /* The TCP header */
	const char *payload; /* Packet payload */

	u_int size_ip;
	u_int size_tcp;

    printf("Device: %s\n", dev);

    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);

    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);

        return 2;
    }

    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return(2);
    }

    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return(2);
	}

    /*
    // Grab a packet
    packet = pcap_next(handle, &header);

    // Print its length
    printf("Jacked a packet with length of [%d]\n", header.len);
    */

    pcap_loop(handle, 1, &packet_capture_callback, NULL);

    //pcap_dumper_t *pd;

    //pcap_dispatch(handle, 0 , packet_capture_callback, (u_char *) pd);

    pcap_close(handle);

    return 0;
}

void packet_capture_callback(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    printf("packet_capture_callback()\n");
    printf("capture length: %d\n", header->caplen);
    printf("packet length: %d\n", header->len);

    struct ether_header *eth_header;

    eth_header = (struct ether_header *) packet;
    
    if (ntohs(eth_header->ether_type) == ETHERTYPE_IP) {
        printf("IP\n");

        printf("Total packet available: %d bytes\n", header->caplen);
        printf("Expected packet size: %d bytes\n", header->len);

        const u_char *ip_header;
        const u_char *tcp_header;
        const u_char *payload;

        int ethernet_header_length = 14; /* Doesn't change */
        int ip_header_length;
        int tcp_header_length;
        int payload_length;

        ip_header = packet + ethernet_header_length;

        /* Find start of IP header */
        ip_header = packet + ethernet_header_length;
        /* The second-half of the first byte in ip_header
        contains the IP header length (IHL). */
        ip_header_length = ((*ip_header) & 0x0F);
        /* The IHL is number of 32-bit segments. Multiply
        by four to get a byte count for pointer arithmetic */
        ip_header_length = ip_header_length * 4;
        printf("IP header length (IHL) in bytes: %d\n", ip_header_length);

        /* Now that we know where the IP header is, we can 
        inspect the IP header for a protocol number to 
        make sure it is TCP before going any further. 
        Protocol is always the 10th byte of the IP header */
        u_char protocol = *(ip_header + 9);
        if (protocol != IPPROTO_TCP) {
            printf("Not a TCP packet. Skipping...\n\n");
            return;
        }

        /* Add the ethernet and ip header length to the start of the packet
        to find the beginning of the TCP header */
        tcp_header = packet + ethernet_header_length + ip_header_length;
        /* TCP header length is stored in the first half 
        of the 12th byte in the TCP header. Because we only want
        the value of the top half of the byte, we have to shift it
        down to the bottom half otherwise it is using the most 
        significant bits instead of the least significant bits */
        tcp_header_length = ((*(tcp_header + 12)) & 0xF0) >> 4;
        /* The TCP header length stored in those 4 bits represents
        how many 32-bit words there are in the header, just like
        the IP header length. We multiply by four again to get a
        byte count. */
        tcp_header_length = tcp_header_length * 4;
        printf("TCP header length in bytes: %d\n", tcp_header_length);

        /* Add up all the header sizes to find the payload offset */
        int total_headers_size = ethernet_header_length+ip_header_length+tcp_header_length;
        printf("Size of all headers combined: %d bytes\n", total_headers_size);
        payload_length = header->caplen - (ethernet_header_length + ip_header_length + tcp_header_length);
        printf("Payload size: %d bytes\n", payload_length);
        payload = packet + total_headers_size;
        printf("Memory address where payload begins: %p\n\n", payload);

        /* Print payload in ASCII */
         if (payload_length > 0) {
            const u_char *temp_pointer = payload;
            int byte_count = 0;

            while (byte_count++ < payload_length) {
                printf("%c", *temp_pointer);
                temp_pointer++;
            }

            printf("\n");
        }
    }
    else  if (ntohs(eth_header->ether_type) == ETHERTYPE_ARP) {
        printf("ARP\n");
    }
    else  if (ntohs(eth_header->ether_type) == ETHERTYPE_REVARP) {
        printf("Reverse ARP\n");
    }
}