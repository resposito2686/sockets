/*
 * FILENAME : server.c
 * DATE     : December 9th, 2021
 *
 * AUTHOR 1 : Robert Esposito
 * AUTHOR 2 : Jaquelyn Fernandez
 * COURSE   : COMPE 560
 *
 * DESCRIPTION :
 *	This program will listen for a connection request from a client.
 * 	It will then accept data from that client and modify it. When
 *	another client connects, the modified data is sent to it.
 *
 * LAST MODIFIED : 12/13/2021
 *
 */

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>	
#include <unistd.h>


 /************************************************************************
 *
 * Struct that stores DATA.
 *
 *************************************************************************/
struct data {
	int32_t i_data;
	float f_data;
	char c_data;
};

int main(int argc, char* argv[])
{
	/***************************************************************************
	 *
	 * Variable declarations.
	 *
	 **************************************************************************/
	int sock, new_sock, sock_size, syn, ack;
	struct sockaddr_in server, client;
	struct data client_data;


	/***************************************************************************
	 *
	 * Variable assignments.
	 *
	 **************************************************************************/
	ack = 1;
	sock_size = sizeof(struct sockaddr_in);


	/***************************************************************************
	 *
	 * A socket named 'sock' is created.
	 * Domain   : AF_INET. Internet Protocol v4.
	 * Type     : SOCK_STREAM. TCP connection.
	 * Protocol : 0. Default protocol for TCP.
	 *
	 **************************************************************************/
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		printf("ERROR: Could not create socket.\n");
		return 1;
	}
	printf("Socket created!\n\n");


	/***************************************************************************
	 *
	 * The parameters of the socket connection are established.
	 * sin_family : Connection domain (IPv4).
	 * sin_addr   : IP Address of the current client.
	 * sin_port   : Port used in connection.
	 *
	 **************************************************************************/
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(42024);

	/***************************************************************************
	 *
	 * Binds the socket so it can accept a client connection.
	 * socket   : sock. The socket used for the connection.
	 * address  : server. Server connection struct. Argument is of type
				  'void*', so server must be typecasted.
	 * addr_len : sizeof(server). The size of server.
	 *
	 **************************************************************************/
	if (bind(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
		printf("ERROR: Bind failed\n");
		return 1;
	}
	printf("Socket bind successful!\n\n");

	/***************************************************************************
	 *
	 * Assigns socket as a passive socket that will listen for connections.
	 * socket  : sock. The binded socket.
	 * backlog : 2. How many clients can be in the connection queue.
	 *
	 **************************************************************************/
	listen(sock, 2);
	printf("Waiting for incoming connections...\n\n");


	/***************************************************************************
	 *
	 * Listens for connection requests. Accepts a connection with a client when
	 * request is received.
	 * socket   : sock. The binded socket.
	 * address  : client. Client connection struct. Argument is of type
				  'void*', so server must be typecasted.
	 * addr_len : sock_size. Size of a generic connection struct.
	 *
	 **************************************************************************/
	while ((new_sock = accept(sock, (struct sockaddr*)&client, (socklen_t*)&sock_size))) {
		printf("Connection accepted\n");
		printf("Client IP Address: %s\n\n", inet_ntoa(client.sin_addr));


		/*************************************************************************
		*
		* Receives a SYN from the client.
		*
		**************************************************************************/
		if (read(new_sock, (int*)&syn, sizeof(syn)) < 0) {
			printf("ERROR: SYN not received.\n");
			return 1;
		}
		printf("SYN received from client: %d\n\n", syn);


		/*************************************************************************
		*
		* Sends an ACK to the client. If the client is the first one to connect,
		* ACK will be 1. Otherwise, ACK will be 0.
		*
		**************************************************************************/
		if (write(new_sock, (int*)&ack, sizeof(ack)) < 0) {
			printf("ERROR: Failed to send ACK.\n");
			return 1;
		}
		printf("ACK sent to client: %d\n\n", ack);


		/*************************************************************************
		*
		* If the client is the first one to connect, receive DATA from that
		* client and modify it according to the project requirements.
		*
		**************************************************************************/
		if (ack) {
			if (read(new_sock, (struct data*)&client_data, sizeof(client_data)) < 0) {
				printf("ERROR: DATA not received.\n");
				return 1;
			}
			printf("DATA received from client\n");
			printf("INT: %d\n", client_data.i_data);
			printf("FLOAT: %f\n", client_data.f_data);
			printf("CHAR: %c\n\n", client_data.c_data);

			client_data.i_data *= 2;
			client_data.f_data += 1;
			client_data.c_data = (char)((int)client_data.c_data + 1);

			printf("DATA modified.\n");
			printf("INT: %d\n", client_data.i_data);
			printf("FLOAT: %f\n", client_data.f_data);
			printf("CHAR: %c\n\n", client_data.c_data);
			ack = 0;
		}


		/*************************************************************************
		*
		* If the client is the second one to connect, send the modified
		* DATA to it.
		*
		**************************************************************************/
		else {
			if (write(new_sock, (struct data*)&client_data, sizeof(client_data)) < 0) {
				printf("DATA failed to send.\n");
			}
			printf("DATA sent to client.\n\n");
		}
	}


	/*************************************************************************
	 *
	 * Print error if client connection failed.
	 *
	 **************************************************************************/
	if (new_sock < 0) {
		printf("ERROR: Failed to connect to client.");
		return 1;
	}

	return 0;
}