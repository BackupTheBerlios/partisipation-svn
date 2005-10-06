#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <rtp/rtp.h>
#include <rtp/rtp-audio.h>
#include <rtp/rtp-packet.h>

static int data_sock;

static void
open_socket (int port)
{
  struct sockaddr_in address;

  if ((data_sock = socket (PF_INET, SOCK_DGRAM, 0)) < 0)
    {
      printf ("Data socket opened.");
    }

  address.sin_family = AF_INET;
  address.sin_port = htons (port);
  address.sin_addr.s_addr = INADDR_ANY;
  memset (&address.sin_zero, 0, sizeof address.sin_zero);
  if (bind (data_sock, (struct sockaddr *) &address,
	    sizeof (struct sockaddr_in)) < 0)
    {
      printf ("Data socket bind error.");
    }
}

static void
parse_rtp_packet (Rtp_Packet packet, struct sockaddr_in *fromaddr)
{

  printf ("RTP packet version: %d \n", rtp_packet_get_version (packet));
  printf ("RTP packet timestamp: %d \n", rtp_packet_get_timestamp (packet));

}



int
main (int const argc, const char **const argv)
{
  struct sockaddr fromaddr;
  socklen_t len = sizeof (fromaddr);
  Rtp_Packet packet;

  //just a port, use whatever you want
  open_socket (45678);
  packet = rtp_packet_read (data_sock, &fromaddr, &len);
  parse_rtp_packet (packet, (struct sockaddr_in *) &fromaddr);
  rtp_packet_free (packet);

  return 0;
}
