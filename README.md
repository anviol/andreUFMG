TP1 Protocolo Olímpico

Aluno: André Vinícius de Oliveira 
Matricula: 2013065935

1) Para compilar o código basta executar o makefile (função: make);
2) Execute o servidor antes do cliente:
	2.1) Para execução do servidor deve ser passado como parâmetro a porta a ser utilizada (Exemplo : ./servidor 51555);	
	2.2) Para execução do cliente dever ser passado como primeiro parâmetro o endereço do servidor e como segundo parâmetro a porta do servidor. (Exemplo: ./cliente localhost 51555).

Obs.: O servidor fica rodando e aceitando vários clientes, um de cada vez, mas quando o cliente enviar "Z" o servidor será finalizado.
