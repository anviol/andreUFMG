/********************************/
/*								*/
/*         biblioteca.c         */
/*								*/
/*   André Vinícus de Oliveira  */
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
#include <netdb.h>
#include <arpa/inet.h>

//Ordenação das mensagens de acordo com o tempo que cada uma representa
void ordenaMensagens (Mensagem *mensagensDesordenadas, int quantidadeMensagens){
    int troca = 1, i = 0, j = 0;
    
    Mensagem memoria;
	
	for(j = quantidadeMensagens - 1; (j >= 1) && (troca == 1); j--) {
		troca = 0;		
		for(i = 0; i < j; i++){
			if(mensagensDesordenadas[i].tempo > mensagensDesordenadas[i + 1].tempo){
				memoria = mensagensDesordenadas[i];
                mensagensDesordenadas[i] = mensagensDesordenadas[i + 1];
                mensagensDesordenadas[i + 1] = memoria;
                troca = 1;
            }
        }
    }
}

//Fazendo ajuste de possíveis erros do usuário
void arrumaMensagem (char buffer[]){
	
	int achou = 1, i = 0, j = 0;

	while (achou == 1){
		achou = 0;
		//Tirando espaços no lugar errado
		for (i = 0; i < strlen(buffer); ++i){
			if (buffer[i] == ' ' && 
			((buffer[i-1] == '0')
			|| (buffer[i-1] == '1')
			|| (buffer[i-1] == '2')
			|| (buffer[i-1] == '3')
			|| (buffer[i-1] == '4')
			|| (buffer[i-1] == '5')
			|| (buffer[i-1] == '6')
			|| (buffer[i-1] == '7')
			|| (buffer[i-1] == '8')
			|| (buffer[i-1] == '9')
			|| (buffer[i-1] == ' ')
			|| (buffer[i+1] == 'D')
			|| (buffer[i+1] == 'C')
			|| (buffer[i+1] == 'O')
			|| (buffer[i+1] == 'Z')
			|| (buffer[i+1] == 'd')
			|| (buffer[i+1] == 'c')
			|| (buffer[i+1] == 'o')
			|| (buffer[i+1] == 'z'))){
				for (j = i; j < strlen(buffer); ++j)
				{
					buffer[j] = buffer[j+1];
					{
					if (j == (strlen(buffer) - 1))
					
						buffer[i] = '\0';
					}
				}
				achou = 1;
			}
		}
	}

	//Trocando minúsculo por maiúsculo
	if (buffer[0] == 'd'){
		buffer[0] = 'D';
	}
	if (buffer[0] == 'c'){
		buffer[0] = 'C';
	}
	if (buffer[0] == 'o'){
		buffer[0] = 'O';
	}
	if (buffer[0] == 'z'){
		buffer[0] = 'Z';
	}
}

//Convertendo a mensagem em milissegundos
int mensagemTempo (char buffer[]){
	int i = 0, hora = 0, minuto = 0, segundo = 0, milisegundo = 0;

	while (buffer[i] != '\0'){
		if (buffer[i] == 'h'){
			hora = atoi(&buffer[i-2]);
		}
		if (buffer[i] == 'm' && buffer[i+1] != 's'){
			minuto = atoi(&buffer[i-2]);
		}
		if (buffer[i] == 's' && buffer[i-1] != 'm'){
			segundo = atoi(&buffer[i-2]);
		}
		if (buffer[i] == 'm' && buffer[i+1] == 's' && buffer[i-2] != ' ' && buffer[i-3] != ' '){
			milisegundo = atoi(&buffer[i-3]);
		}
		if (buffer[i] == 'm' && buffer[i+1] == 's' && (buffer[i-2] == ' ' || buffer[i-3] == ' ')){
			milisegundo = atoi(&buffer[i-2]);
		}		
		i++;
	}

	i = hora*3600000 + minuto*60000 + segundo*1000 + milisegundo;
	
	return i;
}

//Criando o socket do cliente
int conectaSocketCliente(char *endereco, char *porta){

	int meuSocket;
    struct addrinfo auxiliar, *socketInfo;

    memset(&auxiliar, 0x0, sizeof auxiliar);
    auxiliar.ai_family = AF_UNSPEC;
    auxiliar.ai_socktype = SOCK_STREAM;

	getaddrinfo(endereco, porta, &auxiliar, &socketInfo);

    meuSocket = socket(socketInfo->ai_family, socketInfo->ai_socktype, socketInfo->ai_protocol);

    freeaddrinfo(socketInfo);

    if(connect(meuSocket, socketInfo->ai_addr, socketInfo->ai_addrlen) < 0)
    {
    	close(meuSocket);
    	return -1;
    }

    return meuSocket;
}

//Criando o socket do servidor, conectando ao cliente e retornando a conexão com o cliente
int conectaSocketServidor(char *porta){

	struct sockaddr_in6 local6;
	memset(&local6, 0, sizeof(struct sockaddr_in6));

	int meuSocket = 0, socketCliente = 0, confirmacao = 1, len4 = sizeof(local6);

	local6.sin6_family = AF_INET6;
	local6.sin6_port = htons(atoi(porta));
	local6.sin6_addr = in6addr_any;

	meuSocket = socket(AF_INET6, SOCK_STREAM, 0);
	setsockopt(meuSocket, SOL_SOCKET, SO_REUSEADDR, &confirmacao, sizeof(int)); //anula as mensagens de erro de porta sendo usada
	bind(meuSocket, (struct sockaddr *) &local6, sizeof(local6));
	listen(meuSocket, 1);

	socketCliente = accept(meuSocket, (struct sockaddr *) &local6, &len4);

	close(meuSocket);
	return socketCliente;
}