#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include "biblioteca.h"

#define LEN 4096

int main(int argc, char **argv){
	struct addrinfo auxiliar, *socketInfo;
	struct sockaddr_in6 servidor; 
	int tamanhoServidor, meuSocket, conectado = 1, numeroMensagem = 0, slen = 0, i = 0;
	char envia[556];
	char recebe[556];
	char buffer[LEN];

	tamanhoServidor = sizeof(struct sockaddr_in6);

	meuSocket = socket(PF_INET6, SOCK_DGRAM,0);

	memset(&servidor, 0x0, sizeof servidor);

	servidor.sin6_port = htons(atoi(argv[2]));
	servidor.sin6_family = AF_INET6;
	servidor.sin6_addr = in6addr_any;

	bind(meuSocket, (struct sockaddr *)&servidor, tamanhoServidor);

	memset(&auxiliar, 0x0, sizeof auxiliar);
	memset(&socketInfo, 0x0, sizeof socketInfo);

	auxiliar.ai_flags = 0;
	auxiliar.ai_family = PF_INET6;
	auxiliar.ai_socktype = SOCK_DGRAM;
	auxiliar.ai_protocol = IPPROTO_UDP;
	getaddrinfo(argv[1], argv[2], &auxiliar, &socketInfo);

	memset(&envia, 0x0, sizeof envia);
	memset(&recebe, 0x0, sizeof recebe);

	struct timeval tempoEspera;
    tempoEspera.tv_sec = 0;
    tempoEspera.tv_usec = 500000;

	printf("\n** Bem vindo ao cliente ***\n\n");

	while(conectado){
		//Recebendo a mensagem do usuário que será enviada
		printf(">Cliente: ");
		fgets(buffer, LEN, stdin);

		arrumaMensagem(buffer); //Ajustando de possíveis erros cometidos pelo usuário

		memset(&envia, 0x0, sizeof envia);
		memset(&recebe, 0x0, sizeof recebe);

		//Detectando qual o comando que foi dado pelo usuário
		switch (buffer[0]){
			//Enviando para o servidor comando de gravação de tempo com o tempo a ser gravado
			case 'D': case 'd':
				sendto(meuSocket, buffer, strlen(buffer), 0, (struct sockaddr*)&servidor, sizeof(struct sockaddr_in6));
				numeroMensagem++;//Contando quantas mensagens foram enviadas
				setsockopt(meuSocket, SOL_SOCKET, SO_RCVTIMEO,&tempoEspera,sizeof(tempoEspera));
				recvfrom(meuSocket, recebe, 556, MSG_WAITALL, (struct sockaddr*)&servidor, &tamanhoServidor);
				if (recebe[0] == '0'){
					sendto(meuSocket, buffer, strlen(buffer), 0, (struct sockaddr*)&servidor, sizeof(struct sockaddr_in6));
					recvfrom(meuSocket, recebe, 556, MSG_WAITALL, (struct sockaddr*)&servidor, &tamanhoServidor);
				}else{
					printf("Recebi:%s\n\n",recebe);	
				}
				if (recebe[0] == '0')
				{
					printf(">Servidor: Mensagem não enviada, envie novamente.\n");
				}
			break;

			//Enviando para o servidor comando para retornar o valor de uma certa posição
			//e recebendo o que se pede ao servidor
			case 'C': case 'c':
				sendto(meuSocket, buffer, strlen(buffer), 0, (struct sockaddr*)&servidor, sizeof(struct sockaddr_in6));//Enviando solicitação para o servidor
				setsockopt(meuSocket, SOL_SOCKET, SO_RCVTIMEO,&tempoEspera,sizeof(tempoEspera));
				recvfrom(meuSocket, recebe, 556, MSG_WAITALL, (struct sockaddr*)&servidor, &tamanhoServidor);
				if (recebe[0] == '0'){
					sendto(meuSocket, buffer, strlen(buffer), 0, (struct sockaddr*)&servidor, sizeof(struct sockaddr_in6));//Enviando solicitação para o servidor
					recvfrom(meuSocket, recebe, 556, MSG_WAITALL, (struct sockaddr*)&servidor, &tamanhoServidor);
				}else{
					printf("Recebi:%s\n",recebe);	
				}		
				if (recebe[0] == '0')
				{
					printf(">Servidor: Mensagem não enviada, envie novamente.\n");
				}		
				slen = recvfrom(meuSocket, buffer, 556, MSG_WAITALL, (struct sockaddr*)&servidor, &tamanhoServidor);//Recebendo do servidor o tempo solicitado
				buffer[slen - 1] = '\0';//Tirando o excesso de \n
				printf(">Servidor: %s\n", buffer);//Mostrando o tempo para o usuário
			break;

			//Enviando para o servidor comando para retornar os valores de todas posições me ordem
			//e recebendo o que se pede ao servidor
			case 'O': case 'o':
				sendto(meuSocket, buffer, strlen(buffer), 0, (struct sockaddr*)&servidor, sizeof(struct sockaddr_in6));//Enviando comando solicitado
				tamanhoServidor = sizeof(struct sockaddr_in6);
				setsockopt(meuSocket, SOL_SOCKET, SO_RCVTIMEO,&tempoEspera,sizeof(tempoEspera));
				recvfrom(meuSocket, recebe, 556, MSG_WAITALL, (struct sockaddr*)&servidor, &tamanhoServidor);
				if (recebe[0] == '0'){
					sendto(meuSocket, buffer, strlen(buffer), 0, (struct sockaddr*)&servidor, sizeof(struct sockaddr_in6));//Enviando comando solicitado
					recvfrom(meuSocket, recebe, 556, MSG_WAITALL, (struct sockaddr*)&servidor, &tamanhoServidor);

				}else{
					printf("Recebi:%s\n",recebe);
				}
				if (recebe[0] == '0'){
					printf(">Servidor: Mensagem não enviada, envie novamente.\n");
				}				
				for (i = 0; i < numeroMensagem; ++i){//Recebendo todas as mensagens de acordo com a quantidade enviada
					slen = recvfrom(meuSocket, buffer, 556, MSG_WAITALL, (struct sockaddr*)&servidor, &tamanhoServidor);//Recebendo a posição de acordo com o contador
					buffer[slen - 1] = '\0';//Tirando o excesso de \n
					printf(">Servidor: %s", buffer);//Mostrando para o usuário a resposta do servidor
					sendto(meuSocket, "ACK!", strlen("ACK!"), 0
						, (struct sockaddr*)&servidor, sizeof(struct sockaddr_in6));//Enviando para o servidor confirmação de recebimento
				}	
				printf("\n");			
			break;

			//Enviando para o servidor o comando para fechar
			//e fechando a si mesmo
			case 'Z': case 'z':
				sendto(meuSocket, buffer, strlen(buffer), 0, (struct sockaddr*)&servidor, sizeof(struct sockaddr_in6));//Enviando comando solicitado
				tamanhoServidor = sizeof(struct sockaddr_in6);
				setsockopt(meuSocket, SOL_SOCKET, SO_RCVTIMEO,&tempoEspera,sizeof(tempoEspera));
				recvfrom(meuSocket, recebe, 556, MSG_WAITALL, (struct sockaddr*)&servidor, &tamanhoServidor);//Enviando para o servidor o comando de fechar
				if (recebe[0] == '0')
				{
					sendto(meuSocket, buffer, strlen(buffer), 0, (struct sockaddr*)&servidor, sizeof(struct sockaddr_in6));//Enviando comando solicitado
					recvfrom(meuSocket, recebe, 556, MSG_WAITALL, (struct sockaddr*)&servidor, &tamanhoServidor);//Enviando para o servidor o comando de fechar
				}else{
					printf("Recebi:%s\n",recebe);
				}
				if (recebe[0] == '0')
				{
					printf(">Servidor: Mensagem não enviada, envie novamente.\n");
				}
				conectado = 0;//Saindo do loop
			break;

			//Se a mensagem não enquadra em nenhum dos critérios acima
			// é que a mensagem está errada
			default:
				printf(">Servidor: Comando desconhecido!\n");
		}
	}

	close(meuSocket);
}