#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include "biblioteca.h"

#define LEN 4096

int main(int argc, char **argv){

	struct sockaddr_in servidor,cliente; 
	int tam_cliente, meuSocket, conectado = 1, numeroMensagem = 0, slen = 0, i = 0;
	char envia[556];
	char recebe[556];
	char buffer[LEN];

	meuSocket = socket(PF_INET, SOCK_DGRAM, 0); 
	servidor.sin_family = AF_INET; // familia de endereços
	servidor.sin_port = htons(atoi(argv[2])); // porta
	// para usar um ip qualquer use inet_addr("10.10.10.10"); ao invés de htonl(INADDR_ANY)
	//servidor.sin_addr.s_addr=htonl(INADDR_ANY);
	servidor.sin_addr.s_addr = inet_addr(argv[1]);

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
				sendto(meuSocket, buffer, strlen(buffer), 0, (struct sockaddr*)&servidor, sizeof(struct sockaddr_in));
				numeroMensagem++;//Contando quantas mensagens foram enviadas
				tam_cliente = sizeof(struct sockaddr_in);
				recvfrom(meuSocket, recebe, 556, MSG_WAITALL, (struct sockaddr*)&cliente, &tam_cliente);
				printf("Recebi:%s\n",recebe);
			break;

			//Enviando para o servidor comando para retornar o valor de uma certa posição
			//e recebendo o que se pede ao servidor
			case 'C': case 'c':
				sendto(meuSocket, buffer, strlen(buffer), 0, (struct sockaddr*)&servidor, sizeof(struct sockaddr_in));//Enviando solicitação para o servidor
				tam_cliente = sizeof(struct sockaddr_in);
				recvfrom(meuSocket, recebe, 556, MSG_WAITALL, (struct sockaddr*)&cliente, &tam_cliente);
				printf("Recebi:%s\n",recebe);
				sendto(meuSocket, "ACK!", strlen("ACK!"), 0
					, (struct sockaddr*)&cliente, sizeof(struct sockaddr_in));
				slen = recvfrom(meuSocket, buffer, 556, MSG_WAITALL, (struct sockaddr*)&cliente, &tam_cliente);//Recebendo do servidor o tempo solicitado
				buffer[slen - 1] = '\0';//Tirando o excesso de \n
				printf(">Servidor: %s\n", buffer);//Mostrando o tempo para o usuário
			break;

			//Enviando para o servidor comando para retornar os valores de todas posições me ordem
			//e recebendo o que se pede ao servidor
			case 'O': case 'o':
				sendto(meuSocket, buffer, strlen(buffer), 0, (struct sockaddr*)&servidor, sizeof(struct sockaddr_in));//Enviando comando solicitado
				tam_cliente = sizeof(struct sockaddr_in);
				recvfrom(meuSocket, recebe, 556, MSG_WAITALL, (struct sockaddr*)&cliente, &tam_cliente);
				printf("Recebi:%s\n",recebe);
				for (i = 0; i < numeroMensagem; ++i){//Recebendo todas as mensagens de acordo com a quantidade enviada
					slen = recvfrom(meuSocket, buffer, 556, MSG_WAITALL, (struct sockaddr*)&cliente, &tam_cliente);//Recebendo a posição de acordo com o contador
					buffer[slen - 1] = '\0';//Tirando o excesso de \n
					printf(">Servidor: %s\n", buffer);//Mostrando para o usuário a resposta do servidor
					sendto(meuSocket, "ACK!", strlen("ACK!"), 0
						, (struct sockaddr*)&cliente, sizeof(struct sockaddr_in));//Enviando para o servidor confirmação de recebimento
				}				
			break;

			//Enviando para o servidor o comando para fechar
			//e fechando a si mesmo
			case 'Z': case 'z':
				sendto(meuSocket, buffer, strlen(buffer), 0, (struct sockaddr*)&servidor, sizeof(struct sockaddr_in));//Enviando comando solicitado
				tam_cliente = sizeof(struct sockaddr_in);
				recvfrom(meuSocket, recebe, 556, MSG_WAITALL, (struct sockaddr*)&cliente, &tam_cliente);//Enviando para o servidor o comando de fechar
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