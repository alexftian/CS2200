#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "rtp.h"

/* GIVEN Function:
 * Handles creating the client's socket and determining the correct
 * information to communicate to the remote server
 */
CONN_INFO* setup_socket(char* ip, char* port){
	struct addrinfo *connections, *conn = NULL;
	struct addrinfo info;
	memset(&info, 0, sizeof(struct addrinfo));
	int sock = 0;

	info.ai_family = AF_INET;
	info.ai_socktype = SOCK_DGRAM;
	info.ai_protocol = IPPROTO_UDP;
	getaddrinfo(ip, port, &info, &connections);

	/*for loop to determine corr addr info*/
	for(conn = connections; conn != NULL; conn = conn->ai_next){
		sock = socket(conn->ai_family, conn->ai_socktype, conn->ai_protocol);
		if(sock <0){
			if(DEBUG)
				perror("Failed to create socket\n");
			continue;
		}
		if(DEBUG)
			printf("Created a socket to use.\n");
		break;
	}
	if(conn == NULL){
		perror("Failed to find and bind a socket\n");
		return NULL;
	}
	CONN_INFO* conn_info = malloc(sizeof(CONN_INFO));
	conn_info->socket = sock;
	conn_info->remote_addr = conn->ai_addr;
	conn_info->addrlen = conn->ai_addrlen;
	return conn_info;
}

void shutdown_socket(CONN_INFO *connection){
	if(connection)
		close(connection->socket);
}

/* 
 * ===========================================================================
 *
 *			STUDENT CODE STARTS HERE. PLEASE COMPLETE ALL FIXMES
 *
 * ===========================================================================
 */

char* append(char* prev, char* adding, int prev_len, int post_len) {
	char *result = malloc((prev_len + post_len) * sizeof(char));
	int i = 0;
	for (i = 0; i < prev_len; i++) {
		result[i] = prev[i];
	}
	for (i = 0; i < post_len; i++) {
		result[i + prev_len] = adding[i];
	}
	
	return result;
}	

/*
 *  Returns a number computed based on the data in the buffer.
 */
static int checksum(char *buffer, int length){


	int i = 0;
	int sum = 0;
	
	for(i = 0; i < length; i++) {
		sum += buffer[i];
	}
	return sum;

	 /*
	 *  The goal is to return a number that is determined by the contents
	 *  of the buffer passed in as a parameter.  There a multitude of ways
	 *  to implement this function.  For simplicity, simply sum the ascii
	 *  values of all the characters in the buffer, and return the total.
	 */ 
}

/*
 *  Converts the given buffer into an array of PACKETs and returns
 *  the array.  The value of (*count) should be updated so that it 
 *  contains the length of the array created.
 */
static PACKET* packetize(char *buffer, int length, int *count){

	int size = 0;
	int i = 0;
	int pos = 0;
	if(length % MAX_PAYLOAD_LENGTH) {
		size = (length / MAX_PAYLOAD_LENGTH) + 1;
	} else {
		size = length / MAX_PAYLOAD_LENGTH; 
	}
	*count = size;

	PACKET *packets = malloc(size * sizeof(struct _PACKET));	
	PACKET *current_pack;

	for(i = 0; i < length; i++) {
		pos = (i % MAX_PAYLOAD_LENGTH);
		current_pack = (packets + (i / MAX_PAYLOAD_LENGTH));
		current_pack->payload[pos] = buffer[i];
		if(i == (length -1)){
			current_pack->type = LAST_DATA;
			current_pack->payload_length = pos+1;
			current_pack->checksum = checksum(current_pack->payload, current_pack->payload_length);
		} else { // compare here again
			current_pack->type = DATA;
			current_pack->payload_length = MAX_PAYLOAD_LENGTH;
			current_pack->checksum = checksum(current_pack->payload, current_pack->payload_length);
		}
	}
	return packets;

	/*  ----    ----
	 *  The goal is to turn the buffer into an array of packets.
	 *  You should allocate the space for an array of packets and
	 *  return a pointer to the first element in that array.  Each 
	 *  packet's type should be set to DATA except the last, as it 
	 *  should be LAST_DATA type. The integer pointed to by 'count' 
	 *  should be updated to indicate the number of packets in the 
	 *  array.
	 */ 
}

/*
 * Send a message via RTP using the connection information
 * given on UDP socket functions sendto() and recvfrom()
 */
int rtp_send_message(CONN_INFO *connection, MESSAGE*msg){
	int count = 0;
	int i = 0;
	PACKET *pack_type = malloc(sizeof(PACKET));
	PACKET *packets = packetize(msg->buffer, msg->length, &count);
	for(i = 0; i < count; i++) {
		if(i == (count -1)) {

			do {
				sendto(connection->socket,
					packets + i,
					sizeof(packets[count-1]), 0,
					connection->remote_addr,
					connection->addrlen);
				do {
					recvfrom(connection->socket, pack_type,
						24, 0, connection->remote_addr,
						&(connection->addrlen));

				} while(pack_type->type != ACK && pack_type->type != NACK);

			} while(pack_type->type != ACK);
		} else {

			
			sendto(connection->socket,
				packets + i,
				sizeof(packets[i]), 0,
				connection->remote_addr,
				connection->addrlen);

				do {
					recvfrom(connection->socket, pack_type,
						24, 0, connection->remote_addr,
						&(connection->addrlen));

				} while(pack_type->type != ACK && pack_type->type != NACK);
			if(pack_type->type == NACK) {
				i--;
			}
		}
	}
free(pack_type);
free(packets);
	return 0;
	
	/* ----  ----
	 * The goal of this function is to turn the message buffer
	 * into packets and then, using stop-n-wait RTP protocol,
	 * send the packets and re-send if the response is a NACK.
	 * If the response is an ACK, then you may send the next one
	 */
}

/*
 * Receive a message via RTP using the connection information
 * given on UDP socket functions sendto() and recvfrom()
 */
MESSAGE* rtp_receive_message(CONN_INFO *connection){


	int *resp = malloc(4);
	char *result = malloc(1);
	int size = 0;
	int sum_recv = 0;
	int end = 0;
	MESSAGE *mess = malloc(sizeof(MESSAGE));
	PACKET *packet = malloc(sizeof(PACKET));

	do {

recvfrom(connection->socket, packet, sizeof(PACKET), 0,
			NULL, NULL);

		sum_recv = checksum(packet->payload, packet->payload_length);
		if(packet->type == LAST_DATA) { end = 1;}

		if(sum_recv != packet->checksum) {

			*resp = NACK;
			packet->type = NACK;
			sendto(connection->socket, packet,
				24, 0, connection->remote_addr,
				connection->addrlen);
		} else {

			*resp = ACK;
			packet->type = ACK;
			sendto(connection->socket, packet,
				24, 0, connection->remote_addr,
				connection->addrlen);

			result = append(result, packet->payload,
				size, packet->payload_length);

				size = size + packet->payload_length;	
		}


	}while(*resp != ACK || !end);

	mess->length = packet->payload_length;
	mess->buffer = result;
free(packet);
	return mess;	
	
	/* ----  ----
	 * The goal of this function is to handle 
	 * receiving a message from the remote server using
	 * recvfrom and the connection info given. You must 
	 * dynamically resize a buffer as you receive a packet
	 * and only add it to the message if the data is considered
	 * valid. The function should return the full message, so it
	 * must continue receiving packets and sending response 
	 * ACK/NACK packets until a LAST_DATA packet is successfully 
	 * received.
	 */
}



