#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <linux/types.h>
#include <linux/netfilter.h>		/* for NF_ACCEPT */
#include <errno.h>
#include <libnetfilter_queue/libnetfilter_queue.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <string.h>

static int status = 0;

struct ip *iphdr;
struct tcphdr *tcp_hdr;

void dump(unsigned char* buf, int size) {
	int i;
	for (i = 0; i < size; i++) {
		if (i % 16 == 0)
			printf("\n");
		printf("%02x ", buf[i]);
	}
}

/* returns packet id */
static u_int32_t print_pkt (struct nfq_data *tb)
{
	int id = 0;
	struct nfqnl_msg_packet_hdr *ph;
	struct nfqnl_msg_packet_hw *hwph;
	struct ethernet_hdr *eth_hdr;
	u_int32_t mark,ifi;
	int ret;
	unsigned char *data;
	u_char *httphdr;

	// printf delete

	ph = nfq_get_msg_packet_hdr(tb);
	if (ph) {
		id = ntohl(ph->packet_id);
	}

	hwph = nfq_get_packet_hw(tb);

	mark = nfq_get_nfmark(tb);

	ifi = nfq_get_indev(tb);

	ifi = nfq_get_outdev(tb);

	ifi = nfq_get_physindev(tb);

	ifi = nfq_get_physoutdev(tb);

	ret = nfq_get_payload(tb, &data);


	if (ret >= 0)
		printf("payload_len=%d\n ", ret);
		iphdr = (struct ip *) data;
		if(iphdr->ip_p == IPPROTO_TCP){
			printf("ip header len : %d\n", iphdr->ip_hl);
			tcp_hdr = (struct tcphdr *) (data + sizeof(ip));
			printf("sport : %d\n", ntohs(tcp_hdr->th_sport));
			printf("dport : %d\n", ntohs(tcp_hdr->th_dport));

			// http port check routine
			if(ntohs(tcp_hdr->th_dport) == 80){
				httphdr = data + sizeof(ip) + sizeof(tcphdr);
				printf("%s\n",httphdr);
			}
		}

	fputc('\n', stdout);

	return id;
}


static int cb(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg,
	      struct nfq_data *nfa, void *data)
{
	u_int32_t id = print_pkt(nfa);
	printf("entering callback\n");
	if(status == 0){
		return nfq_set_verdict(qh, id, NF_ACCEPT, 0, NULL); /** ACCEPT, DROP **/
	}else if(status == 1){
		return nfq_set_verdict(qh, id, NF_ACCEPT, 0, NULL); /** ACCEPT, DROP **/
	}
}

int main(int argc, char **argv)
{

	if(argc < 2){
		printf("usage : sudo %s <host-domain>\n",argv[0]);
		return 0;
	}

	struct nfq_handle *h;
	struct nfq_q_handle *qh;
	struct nfnl_handle *nh;
	struct ethernet_hdr *eth_hdr;
	int fd;
	int rv;
	char buf[4096] __attribute__ ((aligned));

	printf("opening library handle\n");
	h = nfq_open();
	if (!h) {
		fprintf(stderr, "error during nfq_open()\n");
		exit(1);
	}

	printf("unbinding existing nf_queue handler for AF_INET (if any)\n");
	if (nfq_unbind_pf(h, AF_INET) < 0) {
		fprintf(stderr, "error during nfq_unbind_pf()\n");
		exit(1);
	}

	printf("binding nfnetlink_queue as nf_queue handler for AF_INET\n");
	if (nfq_bind_pf(h, AF_INET) < 0) {
		fprintf(stderr, "error during nfq_bind_pf()\n");
		exit(1);
	}

	printf("binding this socket to queue '0'\n");
	qh = nfq_create_queue(h,  0, &cb, NULL); // packet -> callback
	if (!qh) {
		fprintf(stderr, "error during nfq_create_queue()\n");
		exit(1);
	}

	printf("setting copy_packet mode\n");
	if (nfq_set_mode(qh, NFQNL_COPY_PACKET, 0xffff) < 0) {
		fprintf(stderr, "can't set packet_copy mode\n");
		exit(1);
	}

	fd = nfq_fd(h);

	for (;;) {
		if ((rv = recv(fd, buf, sizeof(buf), 0)) >= 0) {
			printf("pkt received\n");
			nfq_handle_packet(h, buf, rv);
			continue;
		}
		/* if your application is too slow to digest the packets that
		 * are sent from kernel-space, the socket buffer that we use
		 * to enqueue packets may fill up returning ENOBUFS. Depending
		 * on your application, this error may be ignored. nfq_nlmsg_verdict_putPlease, see
		 * the doxygen documentation of this library on how to improve
		 * this situation.
		 */
		if (rv < 0 && errno == ENOBUFS) {
			printf("losing packets!\n");
			continue;
		}
		perror("recv failed");
		break;
	}

	printf("unbinding from queue 0\n");
	nfq_destroy_queue(qh);

#ifdef INSANE
	/* normally, applications SHOULD NOT issue this command, since
	 * it detaches other programs/sockets from AF_INET, too ! */
	printf("unbinding from AF_INET\n");
	nfq_unbind_pf(h, AF_INET);
#endif

	printf("closing library handle\n");
	nfq_close(h);

	exit(0);
}
