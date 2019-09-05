#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>  /*Adição*/
#include <stdbool.h>
#define MAX_NAME_SZ 32 //Tamanho maximo de uma string

#ifdef unix		// se o SO for linux/unix
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#else				// se o SO for windows
#include <winsock2.h>
WSADATA wsa_data;
#endif

char mensagemEnvio[2048];	// variável para guardar o que é escrito do socket
bool resp_ok;

comunica_servidor() 
{
	#ifndef unix	// se o SO for windows
	WSAStartup(MAKEWORD(2, 0), &wsa_data);
	#endif
	
	//criar o socket	--------------------------------------------------------
	int sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	
	// Definição e população da estructura sockaddr_in -------------------------
	struct sockaddr_in server;
	memset(&(server.sin_zero),0, sizeof(server.sin_zero));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");	//define o IP
	server.sin_port = htons(2000);						//define a porta [1012-65535]
	
	char mensagemResposta[2048];
	
	int resultado = connect(sock, (struct sockaddr *)&server,sizeof(server));
	
	if (resultado==-1)
		printf("servidor nao econtrado!");
	else {
		send(sock, mensagemEnvio,strlen(mensagemEnvio)+1, 0);
		//printf("1-Mensagem enviada para o Servidor: %s \n", mensagemEnvio);
		
		char resp[1];
		recv(sock, resp,sizeof(resp)+1, 0);			
		recv(sock, mensagemResposta,sizeof(mensagemResposta)+1, 0);
		
		if(atoi(resp)==1)
			resp_ok=true;
		else
			resp_ok=false;
		printf("\n==== Resposta ====\n%s\n========\n",mensagemResposta);
	}

	close(sock);
}

int sub_menu_login()
{
    /* Allocate memory and check if okay. */
    char *username = malloc (MAX_NAME_SZ);
    if (username == NULL) {
        printf ("No memory\n");
        return 1;
    }
    /* Allocate memory and check if okay. */
    char *password = malloc (MAX_NAME_SZ);
    if (password == NULL) {
        printf ("No memory\n");
        return 1;
    }

    /* Ask user for name. */
    printf("Username: ");

    /* Get the name, with size limit. */
	while(getchar()!='\n');
    fgets (username, MAX_NAME_SZ, stdin);
	
    /* Remove trailing newline, if there. */
    if ((strlen(username)>0) && (username[strlen (username) - 1] == '\n'))
        username[strlen (username) - 1] = '\0';
	
	/* Ask user for name. */
    printf("Password: ");

    /* Get the name, with size limit. */
    fgets (password, MAX_NAME_SZ, stdin);
	
    /* Remove trailing newline, if there. */
    if ((strlen(password)>0) && (password[strlen (password) - 1] == '\n'))
        password[strlen (password) - 1] = '\0';

	sprintf(mensagemEnvio, "2;%s;%s",username,password);
	comunica_servidor();
	
	if(resp_ok==false)
		return 0;
	
	//Se a reposta foi ok, verficar se é c - cliente, e - entidade, a - administrador
	switch(username[0])
	{
		case 'c': return 1;
		case 'e': return 2;
		case 'a': return 3;
	}
	
    free (password);
    free (username);
	
	return 0;
}

int escreve_dados_registo(char* dados)
{
	sprintf(mensagemEnvio, "1;%s",dados);
	comunica_servidor();
	return 0;
}

main() {
	menu_principal();
	}


int menu_principal()
{
	int catch_principal;
	bool sair=false;
	
	while(sair==false)
	{
		printf("\n\t=============================================\n");
		printf("\t==Seja Bem Vindo== \n\t= 1 - Registo\t= \n\t= 2 - Login\t= \n\t= 0 - Sair\t= \n");
		printf("\t=============================================\n\n");
		printf("Opção pretendida: ");
		scanf("%d", &catch_principal);
	 
		switch(catch_principal)
		{
		case 0: 
			printf("Obrigado pela visita.\n");
			sair=true;
			break;
		case 1:
				sub_menu_registo(); break;
		case 2: printf("Introduza o seu username e password:\n"); 
			int tipo = sub_menu_login(); 
			switch(tipo)
			{
			case 1:
				lista_opcoes_cliente();
				break;
			case 2:
				lista_opcoes_entidade();
				break;
			case 3:
				lista_opcoes_administrador();
				break;
			}
			break;
		default: 
			printf("Escolha uma opção válida, por favor!\n");  
			break;
		}
	}
}

int sub_menu_registo()
{
	char *username = malloc (MAX_NAME_SZ);
    if (username == NULL) {
        printf ("No memory\n");
        return 1;
    }
    
	char *password = malloc (MAX_NAME_SZ);
    if (password == NULL) {
        printf ("No memory\n");
        return 1;
    }
	
	char *nome = malloc (MAX_NAME_SZ);
    if (nome == NULL) {
        printf ("No memory\n");
        return 1;
    }

	printf("O Username deve conter o prefixo:\n\t'a' para Administrador\n\t'c' para Cliente\n\t'e' para Entidade\n");
    printf("Username: ");
    while(getchar()!='\n');
		fgets (username, MAX_NAME_SZ, stdin);
	
    if ((strlen(username)>0) && (username[strlen (username) - 1] == '\n'))
        username[strlen (username) - 1] = '\0';
		
	printf("Password: ");
	fgets (password, MAX_NAME_SZ, stdin);
	
    if ((strlen(password)>0) && (password[strlen (password) - 1] == '\n'))
        password[strlen (password) - 1] = '\0';
		
	printf("Nome Completo: ");
	fgets (nome, MAX_NAME_SZ, stdin);
	
    if ((strlen(nome)>0) && (nome[strlen (nome) - 1] == '\n'))
        nome[strlen (nome) - 1] = '\0';
 
	char registo[256];
	sprintf(registo,"%s;%s;%s",username,password,nome) ;
 
	switch(registo[0])
		{
		case 'a':
		case 'c':
		case 'e': 
			escreve_dados_registo(registo); break;
		default: printf("Prefixo de utilizador inválido.\n"); break;
		}
	
    free (password);
    free (username);
	free (nome);
}

int criar_conta()
{
	char *ID = malloc (MAX_NAME_SZ);
    if (ID == NULL) {
        printf ("No memory\n");
        return 1;
    }

	printf("Indique o seu ID: ");
	while(getchar()!='\n');
		fgets (ID, MAX_NAME_SZ, stdin);
	
    if ((strlen(ID)>0) && (ID[strlen (ID) - 1] == '\n'))
        ID[strlen (ID) - 1] = '\0';
	
	sprintf(mensagemEnvio, "3;%s",ID);
	comunica_servidor();
	
	free (ID);
}

int listar_contas()
{
	char *ID = malloc (MAX_NAME_SZ);
    if (ID == NULL) {
        printf ("No memory\n");
        return 1;
    }

	printf("Indique o seu ID: ");
	while(getchar()!='\n');
		fgets (ID, MAX_NAME_SZ, stdin);
	
    if ((strlen(ID)>0) && (ID[strlen (ID) - 1] == '\n'))
        ID[strlen (ID) - 1] = '\0';
	
	sprintf(mensagemEnvio, "4;%s",ID);
	comunica_servidor();
	
	free (ID);
}

int consulta_saldo_conta()
{
	char *ID = malloc (MAX_NAME_SZ);
    if (ID == NULL) {
        printf ("No memory\n");
        return 1;
    }

	printf("Indique o seu ID: ");
	while(getchar()!='\n');
		fgets (ID, MAX_NAME_SZ, stdin);
	
    if ((strlen(ID)>0) && (ID[strlen (ID) - 1] == '\n'))
        ID[strlen (ID) - 1] = '\0';
	
	sprintf(mensagemEnvio, "5;%s",ID);
	comunica_servidor();
	
	//Se conseguiu o listar contas
	if(resp_ok==true)
	{
		char *NIB = malloc (MAX_NAME_SZ);
		if (NIB == NULL) {
			printf ("No memory\n");
			return 1;
		}

		printf("Indique o NIB da conta: ");
		fgets (NIB, MAX_NAME_SZ, stdin);

		if ((strlen(NIB)>0) && (NIB[strlen (NIB) - 1] == '\n'))
			NIB[strlen (NIB) - 1] = '\0';
		
		sprintf(mensagemEnvio, "51;%s;%s",ID,NIB);
		comunica_servidor();
		free (NIB);
	}
	free (ID);
}


int pagamento_servico()
{
	char *ID = malloc (MAX_NAME_SZ);
    if (ID == NULL) {
        printf ("No memory\n");
        return 1;
    }

	printf("Indique o seu ID: ");
	while(getchar()!='\n');
		fgets (ID, MAX_NAME_SZ, stdin);
	
    if ((strlen(ID)>0) && (ID[strlen (ID) - 1] == '\n'))
        ID[strlen (ID) - 1] = '\0';
	
	sprintf(mensagemEnvio, "6;%s",ID);
	comunica_servidor();
	
	//Se conseguiu o listar contas
	if(resp_ok==true)
	{
		char *NIB = malloc (MAX_NAME_SZ);
		if (NIB == NULL) {
			printf ("No memory\n");
			return 1;
		}

		//É necessário indicar o NIB de onde o pagamento vai ser efetuado
		printf("Indique o NIB da conta para efetuar o pagamento: ");
		fgets (NIB, MAX_NAME_SZ, stdin);

		if ((strlen(NIB)>0) && (NIB[strlen (NIB) - 1] == '\n'))
			NIB[strlen (NIB) - 1] = '\0';
			
		int nr_servico=0;
		printf("Indique o número do serviço para pagamento: ");
		scanf("%d", &nr_servico);
		
		int montante=0;
		printf("Indique o montante a pagar: ");
		scanf("%d", &montante);
		
		sprintf(mensagemEnvio, "61;%s;%s;%d;%d",ID,NIB,nr_servico,montante);
		comunica_servidor();
		free (NIB);
	}
	free (ID);
}



int saldo_integrado()
{
	char *ID = malloc (MAX_NAME_SZ);
    if (ID == NULL) {
        printf ("No memory\n");
        return 1;
    }

	printf("Indique o seu ID: ");
	while(getchar()!='\n');
		fgets (ID, MAX_NAME_SZ, stdin);
	
    if ((strlen(ID)>0) && (ID[strlen (ID) - 1] == '\n'))
        ID[strlen (ID) - 1] = '\0';
	
	sprintf(mensagemEnvio, "7;%s",ID);
	comunica_servidor();
	
	free (ID);
}


int consulta_movimentos_conta()
{
	char *ID = malloc (MAX_NAME_SZ);
    if (ID == NULL) {
        printf ("No memory\n");
        return 1;
    }

	printf("Indique o seu ID: ");
	while(getchar()!='\n');
		fgets (ID, MAX_NAME_SZ, stdin);
	
    if ((strlen(ID)>0) && (ID[strlen (ID) - 1] == '\n'))
        ID[strlen (ID) - 1] = '\0';
	
	sprintf(mensagemEnvio, "8;%s",ID);
	comunica_servidor();
	
	//Se conseguiu o listar contas
	if(resp_ok==true)
	{
		char *NIB = malloc (MAX_NAME_SZ);
		if (NIB == NULL) {
			printf ("No memory\n");
			return 1;
		}

		printf("Indique o NIB da conta: ");
		fgets (NIB, MAX_NAME_SZ, stdin);

		if ((strlen(NIB)>0) && (NIB[strlen (NIB) - 1] == '\n'))
			NIB[strlen (NIB) - 1] = '\0';
		
		sprintf(mensagemEnvio, "81;%s;%s",ID,NIB);
		comunica_servidor();
		free (NIB);
	}
	free (ID);
}


int movimento_entre_contas()
{
	char *ID = malloc (MAX_NAME_SZ);
    if (ID == NULL) {
        printf ("No memory\n");
        return 1;
    }

	printf("Indique o seu ID: ");
	while(getchar()!='\n');
		fgets (ID, MAX_NAME_SZ, stdin);
	
    if ((strlen(ID)>0) && (ID[strlen (ID) - 1] == '\n'))
        ID[strlen (ID) - 1] = '\0';
	
	sprintf(mensagemEnvio, "9;%s",ID);
	comunica_servidor();
	
	//Se conseguiu o listar contas
	if(resp_ok==true)
	{
		char *NIB1 = malloc (MAX_NAME_SZ);
		if (NIB1 == NULL) {
			printf ("No memory\n");
			return 1;
		}
		
		char *NIB2 = malloc (MAX_NAME_SZ);
		if (NIB2 == NULL) {
			printf ("No memory\n");
			return 1;
		}

		printf("Indique o NIB da conta de origem: ");
		fgets (NIB1, MAX_NAME_SZ, stdin);

		if ((strlen(NIB1)>0) && (NIB1[strlen (NIB1) - 1] == '\n'))
			NIB1[strlen (NIB1) - 1] = '\0';
			
		printf("Indique o NIB da conta de destino: ");
		fgets (NIB2, MAX_NAME_SZ, stdin);

		if ((strlen(NIB2)>0) && (NIB2[strlen (NIB2) - 1] == '\n'))
			NIB2[strlen (NIB2) - 1] = '\0';
			
		if(strcmp(NIB1,NIB2)==0)
		{
			printf("A conta de origem e destino não podem ser a mesma");
			return 0;
		}
		
		int montante=0;
		printf("Indique o montante a movimentar: ");
		scanf("%d", &montante);
		
		sprintf(mensagemEnvio, "91;%s;%s;%s;%d",ID,NIB1,NIB2,montante);
		comunica_servidor();
		free (NIB1);
		free (NIB2);
	}
	free (ID);
}

int lista_servicos()
{
	char *ID = malloc (MAX_NAME_SZ);
    if (ID == NULL) {
        printf ("No memory\n");
        return 1;
    }

	printf("Indique o seu ID: ");
	while(getchar()!='\n');
		fgets (ID, MAX_NAME_SZ, stdin);
	
    if ((strlen(ID)>0) && (ID[strlen (ID) - 1] == '\n'))
        ID[strlen (ID) - 1] = '\0';
	
	sprintf(mensagemEnvio, "10;%s",ID);
	comunica_servidor();
	
	free (ID);
}

int criar_servico()
{
	char *ID = malloc (MAX_NAME_SZ);
    if (ID == NULL) {
        printf ("No memory\n");
        return 1;
    }

	printf("Indique o seu ID: ");
	while(getchar()!='\n');
		fgets (ID, MAX_NAME_SZ, stdin);
	
    if ((strlen(ID)>0) && (ID[strlen (ID) - 1] == '\n'))
        ID[strlen (ID) - 1] = '\0';
	
	sprintf(mensagemEnvio, "11;%s",ID);
	comunica_servidor();
	
	//Se conseguiu listar contas
	if(resp_ok==true)
	{
		char *NIB = malloc (MAX_NAME_SZ);
		if (NIB == NULL) {
			printf ("No memory\n");
			return 1;
		}
		
		char *nome = malloc (MAX_NAME_SZ);
		if (nome == NULL) {
			printf ("No memory\n");
			return 1;
		}

		printf("Indique o NIB da conta que pretende asociar o servico: ");
		fgets (NIB, MAX_NAME_SZ, stdin);

		if ((strlen(NIB)>0) && (NIB[strlen (NIB) - 1] == '\n'))
			NIB[strlen (NIB) - 1] = '\0';
			
		printf("Indique o nome do servico: ");
		fgets (nome, MAX_NAME_SZ, stdin);

		if ((strlen(nome)>0) && (nome[strlen (nome) - 1] == '\n'))
			nome[strlen (nome) - 1] = '\0';
		
		sprintf(mensagemEnvio, "111;%s;%s;%s",ID,NIB,nome);
		comunica_servidor();
		free (NIB);
		free (nome);
	}
	free (ID);
}

lista_clientes_com_saldo()
{
	char *ID = malloc (MAX_NAME_SZ);
    if (ID == NULL) {
        printf ("No memory\n");
        return 1;
    }

	printf("Indique o seu ID: ");
	while(getchar()!='\n');
		fgets (ID, MAX_NAME_SZ, stdin);
	
    if ((strlen(ID)>0) && (ID[strlen (ID) - 1] == '\n'))
        ID[strlen (ID) - 1] = '\0';
	
	sprintf(mensagemEnvio, "12;%s",ID);
	comunica_servidor();
	
	free (ID);
}


int lista_entidades_com_saldo()
{
	char *ID = malloc (MAX_NAME_SZ);
    if (ID == NULL) {
        printf ("No memory\n");
        return 1;
    }

	printf("Indique o seu ID: ");
	while(getchar()!='\n');
		fgets (ID, MAX_NAME_SZ, stdin);
	
    if ((strlen(ID)>0) && (ID[strlen (ID) - 1] == '\n'))
        ID[strlen (ID) - 1] = '\0';
	
	sprintf(mensagemEnvio, "13;%s",ID);
	comunica_servidor();
	
	free (ID);
}

int valor_depositado_banco()
{
	char *ID = malloc (MAX_NAME_SZ);
    if (ID == NULL) {
        printf ("No memory\n");
        return 1;
    }

	printf("Indique o seu ID: ");
	while(getchar()!='\n');
		fgets (ID, MAX_NAME_SZ, stdin);
	
    if ((strlen(ID)>0) && (ID[strlen (ID) - 1] == '\n'))
        ID[strlen (ID) - 1] = '\0';
	
	sprintf(mensagemEnvio, "14;%s",ID);
	comunica_servidor();
	
	free (ID);
}

int congelar_conta()
{
	char *ID = malloc (MAX_NAME_SZ);
    if (ID == NULL) {
        printf ("No memory\n");
        return 1;
    }

	printf("Indique o seu ID: ");
	while(getchar()!='\n');
		fgets (ID, MAX_NAME_SZ, stdin);
	
    if ((strlen(ID)>0) && (ID[strlen (ID) - 1] == '\n'))
        ID[strlen (ID) - 1] = '\0';
	
	sprintf(mensagemEnvio, "15;%s",ID);
	comunica_servidor();
	
	//Se conseguiu o listar contas
	if(resp_ok==true)
	{
		char *NIB = malloc (MAX_NAME_SZ);
		if (NIB == NULL) {
			printf ("No memory\n");
			return 1;
		}

		printf("Indique o NIB da conta a congelar: ");
		fgets (NIB, MAX_NAME_SZ, stdin);

		if ((strlen(NIB)>0) && (NIB[strlen (NIB) - 1] == '\n'))
			NIB[strlen (NIB) - 1] = '\0';
		
		sprintf(mensagemEnvio, "151;%s;%s",ID,NIB);
		comunica_servidor();
		free (NIB);
	}
	free (ID);
}


int descongelar_conta()
{
	char *ID = malloc (MAX_NAME_SZ);
    if (ID == NULL) {
        printf ("No memory\n");
        return 1;
    }

	printf("Indique o seu ID: ");
	while(getchar()!='\n');
		fgets (ID, MAX_NAME_SZ, stdin);
	
    if ((strlen(ID)>0) && (ID[strlen (ID) - 1] == '\n'))
        ID[strlen (ID) - 1] = '\0';
	
	sprintf(mensagemEnvio, "16;%s",ID);
	comunica_servidor();
	
	//Se conseguiu o listar contas
	if(resp_ok==true)
	{
		char *NIB = malloc (MAX_NAME_SZ);
		if (NIB == NULL) {
			printf ("No memory\n");
			return 1;
		}

		printf("Indique o NIB da conta a congelar: ");
		fgets (NIB, MAX_NAME_SZ, stdin);

		if ((strlen(NIB)>0) && (NIB[strlen (NIB) - 1] == '\n'))
			NIB[strlen (NIB) - 1] = '\0';
		
		sprintf(mensagemEnvio, "161;%s;%s",ID,NIB);
		comunica_servidor();
		free (NIB);
	}
	free (ID);
}


int apagar_conta()
{
	char *ID = malloc (MAX_NAME_SZ);
    if (ID == NULL) {
        printf ("No memory\n");
        return 1;
    }

	printf("Indique o seu ID: ");
	while(getchar()!='\n');
		fgets (ID, MAX_NAME_SZ, stdin);
	
    if ((strlen(ID)>0) && (ID[strlen (ID) - 1] == '\n'))
        ID[strlen (ID) - 1] = '\0';
	
	sprintf(mensagemEnvio, "17;%s",ID);
	comunica_servidor();
	
	//Se conseguiu o listar contas
	if(resp_ok==true)
	{
		char *NIB1 = malloc (MAX_NAME_SZ);
		if (NIB1 == NULL) {
			printf ("No memory\n");
			return 1;
		}
		
		char *NIB2 = malloc (MAX_NAME_SZ);
		if (NIB2 == NULL) {
			printf ("No memory\n");
			return 1;
		}

		printf("Indique o NIB da conta que pretende fechar: ");
		fgets (NIB1, MAX_NAME_SZ, stdin);

		if ((strlen(NIB1)>0) && (NIB1[strlen (NIB1) - 1] == '\n'))
			NIB1[strlen (NIB1) - 1] = '\0';
			
		printf("Indique o NIB da conta para transferir o saldo: ");
		fgets (NIB2, MAX_NAME_SZ, stdin);

		if ((strlen(NIB2)>0) && (NIB2[strlen (NIB2) - 1] == '\n'))
			NIB2[strlen (NIB2) - 1] = '\0';
			
		if(strcmp(NIB1,NIB2)==0)
		{
			printf("Os NIB's não podem ser iguais");
			return 0;
		}
		
		//Transfere todo o saldo da conta que vai ser fechada, não é preciso pedir montante
		sprintf(mensagemEnvio, "171;%s;%s;%s",ID,NIB1,NIB2);
		comunica_servidor();
		free (NIB1);
		free (NIB2);
	}
	free (ID);
}


int lista_opcoes_cliente()
{
	int catch_opcoes_cliente;
	bool sair=false;
	
	while(sair==false)
	{
		printf("\t=============================================\n");
		printf("\t= 3 - Criar conta\t= \n\t= 4 - Listar contas\t= \n\t= 5 - Consulta de saldo de uma conta\t= \n\t= 6 - Efectuar pagamento de serviço\t= \n\t= 7 - Consulta de saldo integrado\t= \n\t= 8 - Consulta de movimentos de uma conta\t= \n\t= 9 - Movimento entre contas próprias\t= \n\t= 10 - Lista de serviços de pagamento\t= \n\t= 17 - Apagar conta\t= \n\t= 0 - Sair\t= \n");
		printf("Opção pretendida: ");
		scanf("%d", &catch_opcoes_cliente);

		switch(catch_opcoes_cliente)
		{
		case 0: 
			sair=true;
			printf("obrigado pela visita. \n"); 
			break;
		case 3: criar_conta(); break;
		case 4:	listar_contas(); break;
		case 5: consulta_saldo_conta(); break;
		case 6: pagamento_servico(); break;
		case 7: saldo_integrado(); break;
		case 8: consulta_movimentos_conta(); break;
		case 9: movimento_entre_contas(); break;
		case 10: lista_servicos(); break;
		case 17: apagar_conta(); break;
		default: printf("Escolha uma opção válida, por favor!\n"); break;
		}
	}
}

int lista_opcoes_entidade()
{
	int catch_opcoes_entidade;
	bool sair=false;
	
	while(sair==false)
	{
		printf("\t=============================================\n");
		printf("\t= 3 - Criar conta\t= \n\t= 4 - Listar contas\t= \n\t= 5 - Consulta de saldo de uma conta\t= \n\t= 7 - Consulta de saldo integrado\t= \n\t= 8 - Consulta de movimentos de uma conta\t= \n\t= 9 - Movimento entre contas próprias\t= \n\t= 10 - Lista de serviços de pagamento\t= \n\t= 11 - Criação de novo serviço\t= \n\t= 17 - Apagar conta\t= \n\t= 0 - Sair\t= \n");
		printf("Opção pretendida: ");
		scanf("%d", &catch_opcoes_entidade);

		switch(catch_opcoes_entidade)
		{
		case 0: 
			sair=true;
			printf("obrigado pela visita. \n"); 
			break;
		case 3: criar_conta(); break;
		case 4:	listar_contas(); break;
		case 5: consulta_saldo_conta(); break;
		case 7: saldo_integrado(); break;
		case 8: consulta_movimentos_conta(); break;
		case 9: movimento_entre_contas(); break;
		case 10: lista_servicos(); break;
		case 11: criar_servico(); break;
		case 17: apagar_conta();break;
		default: printf("Escolha uma opção válida, por favor!\n"); break;
		}
	}
}
	
int lista_opcoes_administrador()
{
	int catch_opcoes_admin;
	bool sair=false;
	
	while(sair==false)
	{
		printf("\t=============================================\n");
		printf("\t= 10 - Lista de serviços de pagamento\t= \n\t= 12 - Lista de todos os clientes e respectivos saldos\t= \n\t= 13 - Lista de todas as entidades e respectivos saldos\t= \n\t= 14 - Valor total depositado no banco\t= \n\t= 15 - Congelar conta\t= \n\t= 16 - Descongelar conta\t= \n\t= 0 - Sair\t= \n");
		printf("Opção pretendida: ");
		scanf("%d", &catch_opcoes_admin);

		switch(catch_opcoes_admin)
		{
		case 0: 
			sair=true;
			printf("obrigado pela visita. \n"); 
			break;
		case 10: lista_servicos(); break;
		case 12: lista_clientes_com_saldo();break;
		case 13: lista_entidades_com_saldo();break;
		case 14: valor_depositado_banco();break;
		case 15: congelar_conta();break;
		case 16: descongelar_conta();break;
		default: printf("Escolha uma opção válida, por favor!\n"); break;
		}
	}
}
