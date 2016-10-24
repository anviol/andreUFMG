#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include "biblioteca.h"

#define LEN 4096

int main(int argc, char **argv){	
	/*struct sockaddr_in6 serv, client;
    int s;
 	uint16_t Port;
 	int ClientAddrLen = sizeof(client);
 	Port = atoi(argv[1]);
 	s = socket(AF_INET6, SOCK_DGRAM, 0);	
    memset(&serv, 0, sizeof(serv));
    serv.sin6_family = AF_INET6;
    serv.sin6_port   = htons(Port);
    serv.sin6_addr   = in6addr_any;
    bind(s,(struct sockaddr *)&serv,sizeof(serv));
	listen(s, 10);*/






	struct sockaddr_in6 cliente, servidor;
	int tam_cliente, meuSocket, enviei = 0, contMensagens = 0, i = 0;
	// MTU padrão pela IETF
	char buffer[556];

	Mensagem mesagensRecebidas[LEN/4];

	meuSocket = socket(AF_INET6 ,SOCK_DGRAM ,0);
	memset(&servidor, 0, sizeof(servidor));
	servidor.sin6_family = AF_INET6;
	servidor.sin6_port = htons(atoi(argv[1]));
	servidor.sin6_addr = in6addr_any;

	memset(&buffer, 0x0, sizeof buffer);

	bind(meuSocket, (struct sockaddr*)&servidor, sizeof(servidor));


	printf("*** Servidor Ativo***\n");

	while(1){		
		tam_cliente = sizeof(struct sockaddr_in6);
		recvfrom(meuSocket, buffer, 556, MSG_WAITALL, (struct sockaddr*)&cliente, &tam_cliente);
		arrumaMensagem(buffer);
		printf(">Cliente: %s\n", buffer);
		/*printf("Recebi:%s de <endereço:%s> <porta:%d>\n"
			,buffer,inet_ntoa(cliente.sin_addr,ntohs(cliente.sin_port));*/
		enviei = sendto(meuSocket, "ACK!", strlen("ACK!"), 0
			, (struct sockaddr*)&cliente, sizeof(struct sockaddr_in6));
		if (enviei >= 0)
			printf("Envio de ACK!\n");
		else{
			error("sendto");
			printf("Envio de falhou!\n");
		}

		switch (buffer[0]){

					//Se receber 'D' gravando novo tempo
					case 'D':
						strcpy(mesagensRecebidas[contMensagens].texto, buffer + 2);//Copia para lista de mensagens sem o comando
						mesagensRecebidas[contMensagens].pos = contMensagens;//Salva a ordem de envio da mensagem
						mesagensRecebidas[contMensagens].tempo = mensagemTempo(buffer);//Converte o tempo da mensagem para milissegundos e salva
						strcat(mesagensRecebidas[contMensagens].texto, "\n");//Adiciona ao final da mensagem um \n
						contMensagens++;//Conta quantas mensagens foram recebidas
						ordenaMensagens(mesagensRecebidas, contMensagens);//Ordena as mensagens de acordo com o tempo 
					break;

					//Se receber 'C' enviando a posição solicitada
					case 'C':
						i = atoi(&buffer[2]);//Descobre qual posição o cliente quer
						if (i <= contMensagens){//Verifica se a posição existe
							sendto(meuSocket, mesagensRecebidas[i-1].texto, strlen(mesagensRecebidas[i-1].texto), 0
								, (struct sockaddr*)&cliente, sizeof(struct sockaddr_in6));
						}
						else{
							//Envia mensagem de erro, caso posição não exista
							sendto(meuSocket, "Essa posição não foi informada.\n"
								, strlen("Essa posição não foi informada.\n"), 0, (struct sockaddr*)&cliente
								, sizeof(struct sockaddr_in6));
						}
					break;

					//Se receber 'O' envia a lista de todas mensagens recebidas
					case 'O':
						//Envia para o cliente todas as mensagens de acordo com as mensagens recebidas
						for (i = 0; i < contMensagens; ++i){
							sendto(meuSocket, mesagensRecebidas[i].texto, strlen(mesagensRecebidas[i].texto), 0
								, (struct sockaddr*)&cliente, sizeof(struct sockaddr_in6));
							recvfrom(meuSocket, buffer, LEN, MSG_WAITALL, (struct sockaddr*)&cliente, &tam_cliente);
						}
					break;

					//Se receber 'Z' fecha o programa
					case 'Z':
						close (meuSocket);//Fecha a conexão com o cliente
						printf("Servidor finalizado!\n");
						return 0;
					break;

					//Se a mensagem não está seguindo nenhuma dessas letras é que está errada
					default:
						sendto(meuSocket, "Erro na sintax da mensagem! A mensagem foi recusada.\n"
							, strlen("Erro na sintax da mensagem! A mensagem foi recusada.\n"), 0
							, (struct sockaddr*)&cliente, sizeof(struct sockaddr_in6));
						printf(">Servidor: Erro na sintax da mensagem! A mensagem foi recusada.\n");
		}				
		memset(&buffer, 0x0, sizeof buffer);
	}
	close(meuSocket);
}