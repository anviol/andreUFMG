#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include "biblioteca.h"

#define LEN 4096

int main(int argc, char **argv){
	//struct addrinfo Hints, *Res=NULL;
    //struct sockaddr_in6 ServerAddr;
    /*memset(&Hints, 0x00, sizeof(Hints));
    Hints.ai_flags = AI_V4MAPPED;
    Hints.ai_family = AF_UNSPEC;
    Hints.ai_socktype = SOCK_DGRAM;
    Rc = inet_pton(AF_INET, argv[1], &ServerAddr);
    if (Rc == 1){ // endereco valido de IPv4?
       Hints.ai_family = AF_INET;
       Hints.ai_flags = AI_NUMERICHOST;
    }
    else{
       Rc = inet_pton(AF_INET6, argv[1], &ServerAddr);
       if (Rc == 1){ // endereco valido de IPv6?
          Hints.ai_family = AF_INET6;
          Hints.ai_flags = AI_NUMERICHOST;
       }
    }
    getaddrinfo(argv[1], argv[2], &Hints, &Res); 
    UdpSocket = socket(Res->ai_family, Res->ai_socktype, Res->ai_protocol);*/

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

    /*if (inet_pton(AF_INET, argv[1], &servidor) == 1){ // endereco valido de IPv4?
       auxiliar.ai_family = AF_INET;
       auxiliar.ai_flags = AI_NUMERICHOST;
    }
    else if (inet_pton(AF_INET6, argv[1], &servidor) == 1){ // endereco valido de IPv6?
          auxiliar.ai_family = AF_INET6;
          auxiliar.ai_flags = AI_NUMERICHOST;
       }
    else {
    	printf("Endereço não localizado!\n");
    }

    getaddrinfo(argv[1], argv[2], &auxiliar, &socketInfo);

	meuSocket = socket(socketInfo->ai_family, socketInfo->ai_socktype, socketInfo->ai_protocol); 
	//servidor.sin_family = AF_INET; // familia de endereços
	//servidor.sin_port = htons(atoi(argv[2])); // porta
	// para usar um ip qualquer use inet_addr("10.10.10.10"); ao invés de htonl(INADDR_ANY)
	//servidor.sin_addr.s_addr=htonl(INADDR_ANY);
	//servidor.sin_addr.s_addr = inet_addr(argv[1]);*/

	memset(&envia, 0x0, sizeof envia);
	memset(&recebe, 0x0, sizeof recebe);

	printf("\n*** Bem vindo ao cliente ***\n");

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
				recvfrom(meuSocket, recebe, 556, MSG_WAITALL, (struct sockaddr*)&servidor, &tamanhoServidor);
				printf("Recebi:%s\n",recebe);
			break;

			//Enviando para o servidor comando para retornar o valor de uma certa posição
			//e recebendo o que se pede ao servidor
			case 'C': case 'c':
				sendto(meuSocket, buffer, strlen(buffer), 0, (struct sockaddr*)&servidor, sizeof(struct sockaddr_in6));//Enviando solicitação para o servidor
				recvfrom(meuSocket, recebe, 556, MSG_WAITALL, (struct sockaddr*)&servidor, &tamanhoServidor);
				printf("Recebi:%s\n",recebe);
				/*sendto(meuSocket, "ACK!", strlen("ACK!"), 0
					, (struct sockaddr*)&servidor, sizeof(struct sockaddr_in6));*/
				slen = recvfrom(meuSocket, buffer, 556, MSG_WAITALL, (struct sockaddr*)&servidor, &tamanhoServidor);//Recebendo do servidor o tempo solicitado
				buffer[slen - 1] = '\0';//Tirando o excesso de \n
				printf(">Servidor: %s\n", buffer);//Mostrando o tempo para o usuário
			break;

			//Enviando para o servidor comando para retornar os valores de todas posições me ordem
			//e recebendo o que se pede ao servidor
			case 'O': case 'o':
				sendto(meuSocket, buffer, strlen(buffer), 0, (struct sockaddr*)&servidor, sizeof(struct sockaddr_in6));//Enviando comando solicitado
				tamanhoServidor = sizeof(struct sockaddr_in6);
				recvfrom(meuSocket, recebe, 556, MSG_WAITALL, (struct sockaddr*)&servidor, &tamanhoServidor);
				printf("Recebi:%s\n",recebe);
				for (i = 0; i < numeroMensagem; ++i){//Recebendo todas as mensagens de acordo com a quantidade enviada
					slen = recvfrom(meuSocket, buffer, 556, MSG_WAITALL, (struct sockaddr*)&servidor, &tamanhoServidor);//Recebendo a posição de acordo com o contador
					buffer[slen - 1] = '\0';//Tirando o excesso de \n
					printf(">Servidor: %s\n", buffer);//Mostrando para o usuário a resposta do servidor
					sendto(meuSocket, "ACK!", strlen("ACK!"), 0
						, (struct sockaddr*)&servidor, sizeof(struct sockaddr_in6));//Enviando para o servidor confirmação de recebimento
				}				
			break;

			//Enviando para o servidor o comando para fechar
			//e fechando a si mesmo
			case 'Z': case 'z':
				sendto(meuSocket, buffer, strlen(buffer), 0, (struct sockaddr*)&servidor, sizeof(struct sockaddr_in6));//Enviando comando solicitado
				tamanhoServidor = sizeof(struct sockaddr_in6);
				recvfrom(meuSocket, recebe, 556, MSG_WAITALL, (struct sockaddr*)&servidor, &tamanhoServidor);//Enviando para o servidor o comando de fechar
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