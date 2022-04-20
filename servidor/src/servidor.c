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


int iniciarServidor(void);
void recibirMensaje(int socketParaRecibir, t_paquete* paquete);
int recibirDatos(void* paquete, int socketParaRecibir, int cantARecibir);
int puerto = 6000;

int main(void) {
	int socketConexion = iniciarServidor();
	int socketParaRecibir;

	struct sockaddr_in dir_cliente;
	socklen_t tam_direccion = sizeof(struct sockaddr_in);

	socketParaRecibir = accept(socketConexion, (void*)&dir_cliente, &tam_direccion);

	t_paquete paquete;
	recibirMensaje(socketParaRecibir, &paquete);

	printf("\n\nRECIBI EL MENSAJE: %s\n\n", (char*)paquete.mensaje);
}

int iniciarServidor(void){
	int socketServidor = 0;
	int valor = 0;

	struct addrinfo hints;
	struct addrinfo *servinfo;
	struct addrinfo *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, string_itoa(puerto), &hints, &servinfo);

	p=servinfo;
	while(p != NULL){
		socketServidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if(socketServidor == -1){
			p = p->ai_next;
			continue;
		}
		valor = bind(socketServidor, p->ai_addr, p->ai_addrlen);
		if(valor == -1){
			close(socketServidor);
			p = p->ai_next;
			continue;
		}
		if(valor != -1){
			break;
		}
	}

	valor = listen(socketServidor, SOMAXCONN);
	if(valor < 0){
		printf("\n\nLISTEN DEVOLVIO UN VALOR NEGATIVO\n\n");
		perror("error con listen");
		return EXIT_FAILURE;
	}

	freeaddrinfo(servinfo);
	printf("\n\nEL SERVIDOR PASO BIEN EL LISTEN");
	return socketServidor;
}

void recibirMensaje(int socketParaRecibir, t_paquete* paquete){
	paquete->mensaje = NULL;
	int resul = recibirDatos(&(paquete->header), socketParaRecibir, sizeof(t_infoMensaje));

	if (resul > 0 && paquete->header.tamanioMensaje > 0) {
		paquete->mensaje = malloc(paquete->header.tamanioMensaje);
		resul = recibirDatos(paquete->mensaje, socketParaRecibir, paquete->header.tamanioMensaje);
	}
}

int recibirDatos(void* paquete, int socketParaRecibir, int cantARecibir) {
	void* datos = malloc(cantARecibir);
	int recibido = 0;
	int totalRecibido = 0;

	do {
		recibido = recv(socketParaRecibir, datos + totalRecibido, cantARecibir - totalRecibido, 0);
		totalRecibido += recibido;
	} while (totalRecibido != cantARecibir && recibido > 0);

	memcpy(paquete, datos, cantARecibir);
	free(datos);
	return recibido;
}













