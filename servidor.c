/********************************/
/*								*/
/*          servidor.c          */
/*								*/
/*  André Viníicus de Oliveira  */
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
#include <netdb.h>
#include <netinet/in.h>

//Definição do tamanho da mensagem e quantidade de mensagens recebidas
#define LEN 4096

//Inicio do programa principal
int main (int argc, char **argv){

	//Declaração das variaveis utilizadas durante o programa
	int clienteSocket, slen, contMensagens = 0, i = 0, j = 0;
	char buffer[LEN];
	Mensagem mesagensRecebidas[LEN/4];

	//Abrindo a comunicação entre o servidor e o cliente
	clienteSocket = conectaSocketServidor(argv[1]);

	//Iniciando um loop que só será encerrado ao receber 'Z'
	while (1){

		//Abrindo conexão com um novo cliente se nenhum cliente estiver conectado
		// e enviando uma mensagem a esse novo cliente
		// ou apenas enviando mensagem para um cleinte já conectado
		if (clienteSocket == 0){
			printf("Aguardando novo cleinte!\n");
			clienteSocket = conectaSocketServidor(argv[1]);
			strcpy (buffer, "Bem vindo!\n\0");
		}else{
			strcpy (buffer, "Bem vindo!\n\0");
		}
		

		//Se foi possivel enviar mensagem para o cleinte, então tudo certo e começa
		if (send(clienteSocket, buffer, strlen(buffer), 0)){

			printf("Cliente aceito. Recebendo mensagens do cliente...\n");

			memset(buffer, 0x0, LEN); //zerando o verto para receber uma mensagem nova

			//Loop iniciado que só será finalizado so o cliente fechar
			while((slen = recv(clienteSocket, buffer, LEN, 0)) > 0){

				buffer[slen - 1] = '\0';//Retirando o excesso de /n

				arrumaMensagem(buffer);//Ajustando alguns erros de digitação

				//Definindo o que será feito com a mensagem recebida
				switch (buffer[0]){

					//Se receber 'D' gravando novo tempo
					case 'D':
						strcpy(mesagensRecebidas[contMensagens].texto, buffer + 2);//Cobia para lista de mansagens sem o comando
						mesagensRecebidas[contMensagens].pos = contMensagens;//Salva a ordem de envio da mensagem
						mesagensRecebidas[contMensagens].tempo = mensagemTempo(buffer);//Converte o tempo da mensagem para milisegundos e salva 
						strcat(mesagensRecebidas[contMensagens].texto, "\n");//Adiciona ao final da mensagem um \n
						contMensagens++;//Conta quantas mensagens foram recebidas
						ordenaMensagens(mesagensRecebidas, contMensagens);//Ordena as mensagens de acordo com o tempo 
					break;

					//Se receber 'C' enviando a posoção solicitada
					case 'C':
						i = atoi(&buffer[2]);//Descobre qual posição o cliente quer
						if (i <= contMensagens){//Verifica se a posição existe
							send(clienteSocket, mesagensRecebidas[i-1].texto, strlen(mesagensRecebidas[i-1].texto), 0);//Envia a posição solicitada
						}
						else{
							//Envia mensagem se erro, caso posiçaõ não exista
							send(clienteSocket, "Essa posição não foi informada.\n", strlen("Essa posição não foi informada.\n"), 0);
						}
					break;

					//Se receber 'O' envia a lista de todas mensagens recebidas
					case 'O':
						//Enviap ara o ociente todas as mensagens de acordo com as mensagens recebidas
						for (i = 0; i < contMensagens; ++i){
							send(clienteSocket, mesagensRecebidas[i].texto, strlen(mesagensRecebidas[i].texto), 0);
							recv(clienteSocket, buffer, LEN, 0);//Aguarda a confirmação do cliente que recebeu a mensagem anterior
						}
					break;

					//Se receber 'Z' fehca o programa
					case 'Z':
						close (clienteSocket);//Fehca a conexão com o cliente
						printf("Servidor finalizado!\n");
						return 0;
					break;

					//Se a mensagem não está seguindo nenhuma dessas letras é que está errada
					default:
						send(clienteSocket, "Erro na sintax da mensagem! A mensagem foi recusada.\n"
							, strlen("Erro na sintax da mensagem! A mensagem foi recusada.\n"),0);	
						printf("Erro na sintax da mensagem! A mensagem foi recusada.\n");

				}

				//Zera os valores na espera de uma nova mensagemd
				memset(buffer, 0x0, LEN);
			}
		}

		//Zera todos os valores na espera de um novo cliente
		clienteSocket = 0;
		memset(buffer, 0x0, LEN);
		memset(mesagensRecebidas, 0x0, LEN/4);
		contMensagens = 0;
	}
}
