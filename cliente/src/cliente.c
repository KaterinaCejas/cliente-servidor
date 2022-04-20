#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <string.h>
#include <commons/config.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdint.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <netdb.h>
#include <semaphore.h>
#include <pthread.h>


typedef struct {
	int tamanioMensaje;
} t_infoMensaje;

typedef struct {
	t_infoMensaje header;
	void* mensaje;
}t_paquete;


int crearConexion(char* ip, int puerto);
void enviarMensaje(int socketParaComunicarme, void* mensaje, int tamanioMensaje);
void enviarPaquete(int socketParaComunicarme, t_paquete* paquete);

int main(void) {
	char* ip = string_new();
	string_append(&ip, "127.0.0.1");
	int puerto = 6000;
	char* mensaje = string_new();
	string_append(&mensaje, "\n\n/////////////////////////////hola servidor!!! soy el cliente :)\n\n");

	int socketParaComunicarme = crearConexion(ip, puerto);
	printf("\n\nYA CREE LA CONEXION, EL SOCKET TIENE VALOR: %d\n\n", socketParaComunicarme);

	int tamanioMensaje = strlen(mensaje) + 1;  //lenght del mensaje + '/0'
	enviarMensaje(socketParaComunicarme, mensaje, tamanioMensaje);
	printf("\n\nYA ENVIE EL MENSAJE\n\n");
	close(socketParaComunicarme);
}

int crearConexion(char* ip, int puerto){
	struct addrinfo hints;
	struct addrinfo *server_info;
	struct addrinfo *p;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(ip, string_itoa(puerto), &hints, &server_info);
	int socket_cliente = 0;
	for(p=server_info; p!=NULL; p=p->ai_next){
		if((socket_cliente=socket(p->ai_family, p->ai_socktype, p->ai_protocol))==-1){
			continue;
		}
		if(connect(socket_cliente, p->ai_addr, p->ai_addrlen) == -1){
			printf("\n\nno se pudo conectar el cliente al servidor\n\n");
			continue;
		}
		break;
	}
	if(p == NULL){
		printf("\n\nno se pudo conectar p == NULL\n\n");
		freeaddrinfo(server_info);
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(server_info);
	return socket_cliente;
}

void enviarMensaje(int socketParaComunicarme, void* mensaje, int tamanioMensaje){
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->header.tamanioMensaje = tamanioMensaje;
	paquete->mensaje = mensaje;
	enviarPaquete(socketParaComunicarme, paquete);
	free(paquete);
}

void enviarPaquete(int socketParaComunicarme, t_paquete* paquete) {
	int cantAEnviar = sizeof(t_infoMensaje) + paquete->header.tamanioMensaje;
	void* datos = malloc(cantAEnviar);
	memcpy(datos, &(paquete->header), sizeof(t_infoMensaje));

	if (paquete->header.tamanioMensaje > 0){
		memcpy(datos + sizeof(t_infoMensaje), (paquete->mensaje), paquete->header.tamanioMensaje);
	}

	int enviado = 0; //bytes enviados
	int totalEnviado = 0;

	do {
		enviado = send(socketParaComunicarme, datos + totalEnviado, cantAEnviar - totalEnviado, 0);
		totalEnviado += enviado;
		if(enviado==-1){
			break;
		}
	} while (totalEnviado != cantAEnviar);

	free(datos);
}










