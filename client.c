/*
 * FILENAME : client.c
 * DATE     : December 9th, 2021
 *
 * AUTHOR 1 : Robert Esposito
 * AUTHOR 2 : Jaquelyn Fernandez
 * COURSE   : COMPE 560
 *
 * DESCRIPTION :
 *	This program will establish a socket connection with a server.
 *	It will then send or receive data based on the acknowledge code
 * 	it receives back.
 *
 * LAST MODIFIED : 12/13/2021
 *
 */

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

 /***************************************************************************
 *
 * Struct that stores DATA.
 *
 ***************************************************************************/
struct data {
	int32_t i_data;
	float f_data;
	char c_data;
};

int main(int argc, char** argv)
{
	/***************************************************************************
	 *
	 * Variable declarations.
	 *
	 **************************************************************************/
	int sock, syn, ack;
	char* ip;
	char* server_addr;
	struct sockaddr_in server;
	struct data client_data;
	struct hostent* host;
	struct in_addr** addr_list;


	/***************************************************************************
	 *
	 * Variable assignments.
	 *
	 **************************************************************************/
	client_data.i_data = 15;
	client_data.f_data = 3.82758;
	client_data.c_data = 'g';
	syn = 1;
	server_addr = argv[1];


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
	 * Queries the DNS server for the IP address of the server name from
	 * args and stores it into ip.
	 *
	 **************************************************************************/
	if ((host = gethostbyname((const char*)server_addr)) == NULL)
	{
		printf("ERROR: cannot get hostname.\n");
		return 1;
	}
	addr_list = (struct in_addr**)host->h_addr_list;
	ip = inet_ntoa(*addr_list[0]);
	printf("Retrieved IP Address from server address %s: %s\n\n", server_addr, ip);


	/***************************************************************************
	 *
	 * The parameters of the socket connection are established.
	 * sin_addr   : IP Address of server.
	 * sin_family : Connection domain (IPv4).
	 * sin_port   : Port used in connection.
	 *
	 **************************************************************************/
	server.sin_addr.s_addr = inet_addr(ip);
	server.sin_family = AF_INET;
	server.sin_port = htons(42024);

	printf("Attempting connection...\n");
	printf("IP Address: %s\n", inet_ntoa(server.sin_addr));
	printf("Port: %d\n\n", ntohs(server.sin_port));


	/***************************************************************************
	 *
	 * The socket connection is established.
	 * socket   : sock. The socket created above.
	 * address  : server. Argument is of type 'void*', so server must be
			  typecasted.
	 * addr_len : sizeof(server). The size of server.
	 *
	 **************************************************************************/
	if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
		printf("ERROR: Failed to connect to server.\n");
		return 1;
	}
	printf("Connected to server!\n\n");


	/***************************************************************************
	 *
	 * Sends a SYN message to the server.
	 *
	 **************************************************************************/
	if (send(sock, (int*)&syn, sizeof(syn), 0) < 0) {
		printf("ERROR: Failed to send SYN.\n");
		return 1;
	}
	printf("SYN sent to server.\n\n");


	/***************************************************************************
	 *
	 * Receives an ACK from the server.
	 *
	 **************************************************************************/
	if (recv(sock, (int*)&ack, sizeof(ack), 0) < 0) {
		printf("ERROR: ACK not received.\n");
	}
	printf("ACK received. Server replied with code: %d\n\n", ack);


	/***************************************************************************
	 *
	 * If the ACK code is a 1, the current client is the first one to connect
	 * to the server and will send it's DATA.
	 *
	 **************************************************************************/
	if (ack) {
		printf("I am the first client. Sending DATA to the server...\n");
		printf("INT: %d\n", client_data.i_data);
		printf("FLOAT: %f\n", client_data.f_data);
		printf("CHAR: %c\n\n", client_data.c_data);
		if (send(sock, (struct data*)&client_data, sizeof(client_data), 0) < 0) {
			printf("DATA failed to send.\n");
		}
		printf("DATA sent to server.\n\n");
	}


	/***************************************************************************
	 *
	 * If the ACK code is a 0, the current client is the second one to connect
	 * to the server and receive the modified DATA.
	 *
	 **************************************************************************/
	else {
		printf("I am the second client. Receiving DATA from the server...\n\n");
		if (recv(sock, (struct data*)&client_data, sizeof(client_data), 0) < 0) {
			printf("ERROR: DATA not received.");
		}
		printf("DATA received from server.\n");
		printf("INT: %d\n", client_data.i_data);
		printf("FLOAT: %f\n", client_data.f_data);
		printf("CHAR: %c\n\n", client_data.c_data);
	}


	/***************************************************************************
	 *
	 * Closes the socket connection once DATA has been sent or received.
	 *
	 **************************************************************************/
	printf("Closing socket...\n");
	if (shutdown(sock, 2) < 0) {
		printf("ERROR: Could not close socket.\n");
		return 1;
	}
	printf("Socket closed.\n");
	return 0;
}