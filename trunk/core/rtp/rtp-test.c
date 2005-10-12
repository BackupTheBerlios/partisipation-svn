#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <time.h>

#include <rtp/rtp.h>
#include <rtp/rtp-audio.h>
#include <rtp/rtp-packet.h>

static int data_sock;
static struct sockaddr_in address;

/**
 * This function displays error codes.
 */
void handle_error(int e) {
	switch (e) {
		case EACCES:

			printf("EACCES\n");

			break;

		case EADDRINUSE:

			printf("EADDRINUSE\n");

			break;

		case EADDRNOTAVAIL:

			printf("EADDRNOTAVAIL\n");

			break;

		case EAGAIN:

			printf("EAGAIN\n");

			break;

		case EBADF:

			printf("EBADF\n");

			break;

		case EINVAL:

			printf("EINVAL\n");

			break;

		case ENOTSOCK:

			printf("ENOTSOCK\n");

			break;

		case ECONNABORTED:

			printf("ECONNABORTED\n");

			break;

		case EINTR:

			printf("EINTR\n");

			break;

		case EMFILE:

			printf("EMFILE\n");

			break;

		case ENFILE:

			printf("ENFILE\n");

			break;

		case EOPNOTSUPP:

			printf("EOPNOTSUPP\n");

			break;

		case EFAULT:

			printf("EFAULT\n");

			break;

		case ENOBUFS:

			printf("ENOBUFS\n");

			break;

		case EPROTO:

			printf("EPROTO\n");

			break;

		case EPERM:

			printf("EPERM\n");

			break;

		case EISCONN:

			printf("EISCON\n");

			break;

		case ECONNREFUSED:

			printf("ECONNREFUSED\n");

			break;

		case ETIMEDOUT:

			printf("ETIMEDOUT\n");

			break;

		case ENETUNREACH:

			printf("ENETUNREACH\n");

			break;

		case EINPROGRESS:

			printf("EINPROGRESS\n");

			break;

		case EALREADY:

			printf("EALREADY\n");

			break;

		case EAFNOSUPPORT:

			printf("EAFNOSUPPORT\n");

			break;

		default:

			printf("An unknown error occured.\n");

	}
}

/**
 * This function opens a UDP socket for a given host and port without bindig it. Set host to 
 * NULL to use INADDR_ANY as host.
 * 
 * @param char* host 
 * @param int port 
 */
static void open_socket(char *host, int port) {

	if ((data_sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("Could not open UDP socket. Error code:");
		handle_error(errno);
	}

	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	if (host) {
		address.sin_addr.s_addr = inet_addr(host);
	} else {
		address.sin_addr.s_addr = INADDR_ANY;
	}
	memset(&address.sin_zero, 0, sizeof address.sin_zero);
}

static void parse_rtp_packet(Rtp_Packet packet) {

	printf("-------------------------------------------------\n");
	printf(">> Sequence Number: %d \n", rtp_packet_get_seq(packet));
	printf(">> Timestamp: %d \n", rtp_packet_get_timestamp(packet));
	printf(">> Version: %d \n", rtp_packet_get_version(packet));
	printf(">> Padding: %d \n", rtp_packet_get_padding(packet));
	printf(">> Extension: %d \n", rtp_packet_get_extension(packet));
	printf(">> CSRC (Contributing Source): %d \n",
		   rtp_packet_get_csrc_count(packet));
	printf(">> SSRC (Synchronization Source): %d \n",
		   rtp_packet_get_ssrc(packet));
	printf(">> Marker: %d \n", rtp_packet_get_marker(packet));
	printf(">> Payload Type: %d \n", rtp_packet_get_payload_type(packet));
	printf(">> Payload Length: %d \n", rtp_packet_get_payload_len(packet));
	printf(">> Packet Length: %d \n", rtp_packet_get_packet_len(packet));
	printf(">> Payload: %s \n", (char *) rtp_packet_get_payload(packet));
	printf("-------------------------------------------------\n");
}

int main(int argc, char **argv) {
	if (argc < 5) {
		printf
			("Please enter the mode (send or recv), number of packets to send, ");
		printf("a host address, and a user port (1024 - 65535). \n");
	} else {
		char *mode = (char *) malloc(strlen(argv[1]));
		strcpy(mode, argv[1]);
		int num = atoi(argv[2]);
		char *host = argv[3];
		int port = atoi(argv[4]);
		if (!(port > 1023 && port < 65536)) {
			// invalid port number
			printf("Illegal port: %d.\n", port);
			return 0;
		}
		if (strcmp(mode, "send") == 0) {
			// work as sender       
			Rtp_Packet packet;
			open_socket(host, port);

			guint32 pkt_seq = random32(1);
			guint32 pkt_ssrc = 0;
			guint32 pkt_timestamp;
			time_t moment;

			int i;

			for (i = 0; i < num; i++) {

				char *data = "HELLO. THIS IS THE PAYLOAD.";

				// initial sequence number should be random due to RFC 3550 
				int pkt_size = strlen(data);
				pkt_timestamp = (long) time(&moment);

				packet = rtp_packet_new_allocate(pkt_size, 0, 0);

				rtp_packet_set_csrc_count(packet, 0);
				rtp_packet_set_extension(packet, 0);
				rtp_packet_set_padding(packet, 0);
				rtp_packet_set_version(packet, RTP_VERSION);
				rtp_packet_set_payload_type(packet, PAYLOAD_GSM);
				rtp_packet_set_marker(packet, 0);
				rtp_packet_set_ssrc(packet, pkt_ssrc);
				rtp_packet_set_seq(packet, pkt_seq);
				rtp_packet_set_timestamp(packet, pkt_timestamp);
				g_memmove(rtp_packet_get_payload(packet), data, pkt_size);

				// check packet
				printf("SENDING NEXT PACKET ...\n");
				parse_rtp_packet(packet);

				socklen_t len = sizeof(address);
				rtp_packet_send(packet, data_sock,
								(struct sockaddr *) &address, len);

				rtp_packet_free(packet);

				pkt_seq++;

				printf("\n");
			}

			close(data_sock);

		} else if (strcmp(mode, "recv") == 0) {
			// work as receiver
			Rtp_Packet packet;
			open_socket(NULL, port);

			if (bind(data_sock, (struct sockaddr *) &address,
					 sizeof(struct sockaddr_in)) < 0) {
				printf("Data socket bind error. Error code: ");
				handle_error(errno);
			}

			socklen_t len = sizeof(address);

			for (;;) {
				printf("WAITING FOR NEXT PACKET ...\n");
				packet =
					rtp_packet_read(data_sock,
									(struct sockaddr *) &address, &len);
				parse_rtp_packet(packet);
				rtp_packet_free(packet);
			}

		} else {
			// invalid mode
			printf("Unknown mode: %s. \n", mode);
		}
		free(mode);

	}
	return 0;
}
