/********************************/
/*								*/
/*          servidor.c          */
/*								*/
/*  André Viníicus de Oliveira  *
/*          2013065935			*/
/*								*/
/********************************/

//Incluindo bibliotecas para implementação das funções comuns em C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

//Incluindo minha biblioteca criada para esse programa
#include "biblioteca.h"

//Incluindo bibliotecas para conexão entre o servidor e o cliente
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

//Definição do tamanho das mensagens enviadas
#define LEN 4096

//Início do programa principal
int main (int argc, char **argv){

	//Declaração das variáveis utilizadas durante o programa
	char buffer[LEN];
	int meuSocket = 0, conectado = 1, slen = 0, numeroMensagem = 0, i = 0;

	//Abrindo o socket do cliente
	meuSocket = conectaSocketCliente(argv[1], argv[2]);
	
	//Verificando se o socket foi criado com sucesso
	if (meuSocket == -1){
		perror("socket ");
		exit(1);
	}else
		printf("Socket criado com sucesso!\n");

	slen = recv(meuSocket, buffer, LEN, 0);//Recebendo mensagem do servidor
	buffer[slen - 1] = '\0'; //Retirando o excesso de \n
	printf(">Servidor: %s\n", buffer);//Escrevendo na tela a mensagem recebida do servidor

	//Iniciando o loop até que o comando 'Z' seja enviado
	while (conectado){

		//Recebendo a mensagem do usuário que será enviada
		printf(">Cliente: ");
		fgets(buffer, LEN, stdin);

		arrumaMensagem(buffer); //Ajustando de possíveis erros cometidos pelo usuário

		//Detectando qual o comando que foi dado pelo usuário
		switch (buffer[0]){
			//Enviando para o servidor comando de gravação de tempo com o tempo a ser gravado
			case 'D': case 'd':
				send(meuSocket, buffer, strlen(buffer), 0);//Enviando a mensagem para o servidor
				numeroMensagem++;//Contando quantas mensagens foram enviadas
			break;

			//Enviando para o servidor comando para retornar o valor de uma certa posição
			//e recebendo o que se pede ao servidor
			case 'C': case 'c':
				send(meuSocket, buffer, strlen(buffer), 0);//Enviando solicitação para o servidor
				slen = recv(meuSocket, buffer, LEN, 0);//Recebendo do servidor o tempo solicitado
				buffer[slen - 1] = '\0';//Tirando o excesso de \n
				printf(">Servidor: %s\n", buffer);//Mostrando o tempo para o usuário
			break;

			//Enviando para o servidor comando para retornar os valores de todas posições me ordem
			//e recebendo o que se pede ao servidor
			case 'O': case 'o':
				send(meuSocket, buffer, strlen(buffer), 0);//Enviando comando solicitado
				for (i = 0; i < numeroMensagem; ++i){//Recebendo todas as mensagens de acordo com a quantidade enviada
					slen = recv(meuSocket, buffer, LEN, 0);//Recebendo a posição de acordo com o contador
					buffer[slen - 1] = '\0';//Tirando o excesso de \n
					printf(">Servidor: %s\n", buffer);//Mostrando para o usuário a resposta do servidor
					send(meuSocket, "ack", strlen("ack"), 0);//Enviando para o servidor confirmação de recebimento
				}				
			break;

			//Enviando para o servidor o comando para fechar
			//e fechando a si mesmo
			case 'Z': case 'z':
				send(meuSocket, buffer, strlen(buffer), 0);//Enviando para o servidor o comando de fechar
				conectado = 0;//Saindo do loop
			break;

			//Se a mensagem não enquadra em nenhum dos critérios acima
			// é que a mensagem está errada
			default:
				printf(">Servidor: Comando desconhecido!\n");
		}
	}

	//Deslocando o socket
	close(meuSocket);	

	return EXIT_SUCCESS;
}