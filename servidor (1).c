#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

#ifdef unix			// se o SO for linux/unix
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#else				// se o SO for windows
#include <winsock2.h>
WSADATA wsa_data;
#endif

typedef struct pessoa
{
	char username[32];
	char password[32];
	char nome[64];
} pessoa;

typedef struct conta
{
	char username[32];   //identifica a quem pertence a conta
	char NIB[21];
	int saldo;
	bool congelada;
} conta;

typedef struct servico
{
	int num_servico;
	char NIB[21]; //identifica a conta da entidade
	char nome[32];
} servico;

typedef struct login //Associa um username a um ID
{
	char ID[16];
	char username[32];
} login;

typedef struct movimento //Associa um username a um ID
{
	char NIB[21];
	char descricao[256];
} movimento;

pessoa lista_clientes[100];
pessoa lista_administradores[100];
pessoa lista_entidades[100];
conta  lista_contas[100];
servico lista_servicos[100];
login lista_logins[100];
movimento lista_movimentos[100];

char resp_ok[1];        //Variável para indicar se a resposta ao pedido é ok ou não
char resposta[4096];	// variável para responder ao pedido
int num_servicos=0;
int num_clientes=0;
int num_administradores=0;
int num_entidades=0;
int num_contas=0;
int num_logins=0;
int num_movimentos=0;
int id_login=1;
int id_servico=1;
int nib=1;

int ler_dados()
{
   FILE* ficheiro;
   char* linha = NULL;
   size_t len = 0;
   ssize_t read;

   //Clientes
   ficheiro = fopen("clientes.txt", "r");
   if (ficheiro != NULL) 
   {
      while (getline(&linha, &len, ficheiro)!= -1) 
      {
		 if ((strlen(linha)>0) && (linha[strlen (linha) - 1] == '\n'))
            linha[strlen (linha) - 1] = '\0';
         strcpy(lista_clientes[num_clientes].username,strtok(linha, ";"));
         strcpy(lista_clientes[num_clientes].password,strtok(NULL, ";"));
		 strcpy(lista_clientes[num_clientes].nome,strtok(NULL, ";"));
         num_clientes++;
      }
   }
   fclose(ficheiro); 
   
   //Entidades
   ficheiro = fopen("entidades.txt", "r");
   if (ficheiro != NULL) 
   {
      while ((read = getline(&linha, &len, ficheiro)) != -1) 
      {
		if ((strlen(linha)>0) && (linha[strlen (linha) - 1] == '\n'))
            linha[strlen (linha) - 1] = '\0';
         strcpy(lista_entidades[num_entidades].username,strtok(linha, ";"));
         strcpy(lista_entidades[num_entidades].password,strtok(NULL, ";"));
		 strcpy(lista_entidades[num_entidades].nome,strtok(NULL, ";"));
         num_entidades++;
      }
   }
   fclose(ficheiro); 
   
   //Administradores
   ficheiro = fopen("administradores.txt", "r");
   if (ficheiro != NULL) 
   {
      while ((read = getline(&linha, &len, ficheiro)) != -1) 
      {
		if ((strlen(linha)>0) && (linha[strlen (linha) - 1] == '\n'))
            linha[strlen (linha) - 1] = '\0';
         strcpy(lista_administradores[num_administradores].username,strtok(linha, ";"));
         strcpy(lista_administradores[num_administradores].password,strtok(NULL, ";"));
		 strcpy(lista_administradores[num_administradores].nome,strtok(NULL, ";"));
         num_administradores++;
      }
   }
   fclose(ficheiro);
   
   //Contas
   ficheiro = fopen("contas.txt", "r");
   if (ficheiro != NULL) 
   {
      while ((read = getline(&linha, &len, ficheiro)) != -1) 
      {
		if ((strlen(linha)>0) && (linha[strlen (linha) - 1] == '\n'))
            linha[strlen (linha) - 1] = '\0';
         strcpy(lista_contas[num_contas].username,strtok(linha, ";"));
         strcpy(lista_contas[num_contas].NIB,strtok(NULL, ";"));
		 lista_contas[num_contas].saldo = atoi(strtok(NULL, ";"));
		 int congel = atoi(strtok(NULL, ";"));
		 if(congel==0)
			lista_contas[num_contas].congelada=false;
		 else 
			lista_contas[num_contas].congelada=true; 
         num_contas++;
      }
   }
   fclose(ficheiro);
   
   //Serviços
   ficheiro = fopen("servicos.txt", "r");
   if (ficheiro != NULL) 
   {
      while ((read = getline(&linha, &len, ficheiro)) != -1) 
      {
		if ((strlen(linha)>0) && (linha[strlen (linha) - 1] == '\n'))
            linha[strlen (linha) - 1] = '\0';
         lista_servicos[num_servicos].num_servico = atoi(strtok(linha, ";"));
         strcpy(lista_servicos[num_servicos].NIB,strtok(NULL, ";"));
		 strcpy(lista_servicos[num_servicos].nome,strtok(NULL, ";"));
         num_servicos++;
      }
   }
   fclose(ficheiro);
   
   //Movimentos
   ficheiro = fopen("movimentos.txt", "r");
   if (ficheiro != NULL) 
   {
      while ((read = getline(&linha, &len, ficheiro)) != -1) 
      {
		if ((strlen(linha)>0) && (linha[strlen (linha) - 1] == '\n'))
            linha[strlen (linha) - 1] = '\0';
         strcpy(lista_movimentos[num_movimentos].NIB,strtok(linha, ";"));
		 strcpy(lista_movimentos[num_movimentos].descricao,strtok(NULL, ";"));
         num_movimentos++;
      }
   }
   fclose(ficheiro);
}

int gravar_dados()
{
   FILE* ficheiro;
   int i;

   //Clientes
   ficheiro = fopen("clientes.txt", "w");
   if (ficheiro != NULL) 
   {
      for(i=0; i < num_clientes; i++)
      {
         fprintf(ficheiro,"%s;%s;%s\n", lista_clientes[i].username,lista_clientes[i].password,lista_clientes[i].nome);
      }
   }
   fclose(ficheiro);
   
   //Entidades
   ficheiro = fopen("entidades.txt", "w");
   if (ficheiro != NULL) 
   {
      for(i=0; i < num_entidades; i++)
      {
         fprintf(ficheiro,"%s;%s;%s\n", lista_entidades[i].username,lista_entidades[i].password,lista_entidades[i].nome);
      }
   }
   fclose(ficheiro);
   
   //Administradores
   ficheiro = fopen("administradores.txt", "w");
   if (ficheiro != NULL) 
   {
      for(i=0; i < num_administradores; i++)
      {
         fprintf(ficheiro,"%s;%s;%s\n", lista_administradores[i].username,lista_administradores[i].password,lista_administradores[i].nome);
      }
   }
   fclose(ficheiro);
   
   //Contas
   ficheiro = fopen("contas.txt", "w");
   if (ficheiro != NULL) 
   {
      for(i=0; i < num_contas; i++)
      {
		 if( lista_contas[i].congelada == true)
			fprintf(ficheiro,"%s;%s;%d;1\n", lista_contas[i].username,lista_contas[i].NIB,lista_contas[i].saldo);
		 else
			fprintf(ficheiro,"%s;%s;%d;0\n", lista_contas[i].username,lista_contas[i].NIB,lista_contas[i].saldo);
      }
   }
   fclose(ficheiro);
   
   //Serviços
   ficheiro = fopen("servicos.txt", "w");
   if (ficheiro != NULL) 
   {
      for(i=0; i < num_servicos; i++)
      {
		 fprintf(ficheiro,"%d;%s;%s\n",lista_servicos[i].num_servico,lista_servicos[i].NIB,lista_servicos[i].nome);
      }
   }
   fclose(ficheiro);
   
   //Movimentos
   ficheiro = fopen("movimentos.txt", "w");
   if (ficheiro != NULL) 
   {
      for(i=0; i < num_movimentos; i++)
      {
		 fprintf(ficheiro,"%s;%s\n",lista_movimentos[i].NIB,lista_movimentos[i].descricao);
      }
   }
   fclose(ficheiro);
}


bool novo_registo(char* dados)
{
	char* oper     = strtok(dados, ";");
	char* username = strtok(NULL, ";");
	char* password = strtok(NULL, ";");
	char* nome = strtok(NULL, ";");
	
	if(oper==NULL || username==NULL || password==NULL || nome==NULL)
	{
		resp_ok[0] = '0';
		strcpy(resposta, "Parâmetros inválidos");
		return false;
	}

	switch(username[0])
	{
	case 'c':
		if(num_clientes >= 100)
		{
			resp_ok[0] = '0';
			strcpy(resposta, "Número máximo de clientes atingido");
			return false;
		}
		strcpy(lista_clientes[num_clientes].username, username);
		strcpy(lista_clientes[num_clientes].password, password);
		strcpy(lista_clientes[num_clientes].nome, nome);
		num_clientes=num_clientes+1;
		resp_ok[0] = '1';
		strcpy(resposta, "Registo efetuado com sucesso");
		break;
	case 'a':
		if(num_administradores >= 100)
		{
			resp_ok[0] = '0';
			strcpy(resposta, "Número máximo de administradores atingido");
			return false;
		}
		strcpy(lista_administradores[num_administradores].username, username);
		strcpy(lista_administradores[num_administradores].password, password);
		strcpy(lista_administradores[num_administradores].nome, nome);
		num_administradores=num_administradores+1;
		resp_ok[0] = '1';
		strcpy(resposta, "Registo efetuado com sucesso");
		break;
	case 'e':
		if(num_entidades >= 100)
		{
			resp_ok[0] = '0';
			strcpy(resposta, "Número máximo de entidades atingido");
			return false;
		}
		strcpy(lista_entidades[num_entidades].username, username);
		strcpy(lista_entidades[num_entidades].password, password);
		strcpy(lista_entidades[num_entidades].nome, nome);
		num_entidades=num_entidades+1;
		resp_ok[0] = '1';
		strcpy(resposta, "Registo efetuado com sucesso");
		break;
	default: 
		resp_ok[0] = '0';
		strcpy(resposta, "Registo não efetuado, o username não tem um dos prefixos disponíveis");
		return false; //Não tem nenhum dos prefixos possíveis
	}
	return true;
}

bool novo_login(char* dados)
{
	char* oper     = strtok(dados, ";");
	char* username = strtok(NULL, ";");
	char* password = strtok(NULL, ";");
	
	if(oper==NULL || username==NULL || password==NULL)
	{
		resp_ok[0] = '0';
		strcpy(resposta, "Parâmetros inválidos");
		return false;
	}
	
	if(num_logins >= 100)
	{
		resp_ok[0] = '0';
		strcpy(resposta, "Número máximo de logins atingido");
		return false;
	}
	
	int i;
	bool encontrou=false;
	char nome[32];
	
	
	switch(username[0])
	{
	case 'c':
		for(i=0; i < num_clientes; i++)
		{
			if(strcmp(username,lista_clientes[i].username) == 0 && strcmp(password,lista_clientes[i].password) == 0)
			{
				encontrou = true;
				strcpy(nome,lista_clientes[i].nome);
			}
		}
		break;
	case 'a':
		for(i=0; i < num_administradores; i++)
		{
			if(strcmp(username,lista_administradores[i].username) == 0 && strcmp(password,lista_administradores[i].password) == 0)
			{
				encontrou = true;
				strcpy(nome,lista_administradores[i].nome);
			}
		}
		break;
	case 'e':
		for(i=0; i < num_entidades; i++)
		{
			if(strcmp(username,lista_entidades[i].username) == 0 && strcmp(password,lista_entidades[i].password) == 0)
			{
				encontrou = true;
				strcpy(nome,lista_entidades[i].nome);
			}
		}
		break;
	default: 
		strcpy(resposta, "O username introduzido não é válido");
		resp_ok[0] = '0';
		encontrou = false; //Não tem nenhum dos prefixos possíveis
		break;
	}
	
	//se encontrou faz login
	if(encontrou==true)
	{
		//Verificar se o utilizador já fez login nesta sessão para lhe dar o mesmo ID
		char ID[16];
		bool existe_login=false;
		for(i=0; i < num_logins; i++)
		{
			if(strcmp(username,lista_logins[i].username) == 0)
			{
				existe_login = true;
				strcpy(ID,lista_logins[i].ID);
			}
		}
		//Se não encontrou faz login
		if(existe_login==false)
		{
			sprintf(ID,"id%d",id_login);
			strcpy(lista_logins[num_logins].ID,ID);
			strcpy(lista_logins[num_logins].username, username);
			num_logins=num_logins+1;
			id_login=id_login+1;
		}
		resp_ok[0] = '1';
		sprintf(resposta, "Seja bem-vindo %s o seu ID para efetuar operações é: %s",nome,ID);
	}
	else
	{
		strcpy(resposta, "O username/password inválidos");
		resp_ok[0] = '0';
	}
		
	return encontrou;
}


bool criar_conta(char* dados)
{
	char* oper     = strtok(dados, ";");
	char* ID       = strtok(NULL, ";");
	
	if(oper==NULL || ID==NULL)
	{
		resp_ok[0] = '0';
		strcpy(resposta, "Parâmetros inválidos");
		return false;
	}
	
	if(num_contas >= 100)
	{
		resp_ok[0] = '0';
		strcpy(resposta, "Número máximo de contas atingido");
		return false;
	}
	
	//Verificar login
	int i;
	char username[32];
	bool encontrou=false;
	for(i=0; i < num_logins; i++)
	{
		if(strcmp(ID,lista_logins[i].ID) == 0)
		{
			strcpy(username,lista_logins[i].username);
			encontrou = true;
		}
	}
	
	if(encontrou == false)
	{
		strcpy(resposta,"ID inválido");
		resp_ok[0] = '0';
		return false;
	}
	
	sprintf(lista_contas[num_contas].NIB, "000%d",nib);
	strcpy(lista_contas[num_contas].username, username);
	lista_contas[num_contas].saldo = 1000;
	lista_contas[num_contas].congelada = false;
	sprintf(resposta, "Criada nova conta. NIB: %s Saldo %d",lista_contas[num_contas].NIB,lista_contas[num_contas].saldo);
	num_contas=num_contas+1;
	nib=nib+1;
	resp_ok[0] = '1';
	return true;
}

bool listar_contas(char* dados)
{
	char* oper     = strtok(dados, ";");
	char* ID       = strtok(NULL, ";");
	
	if(oper==NULL || ID==NULL)
	{
		resp_ok[0] = '0';
		strcpy(resposta, "Parâmetros inválidos");
		return false;
	}
	
	//Verificar login
	int i;
	char username[32];
	bool encontrou=false;
	for(i=0; i < num_logins; i++)
	{
		if(strcmp(ID,lista_logins[i].ID) == 0)
		{
			strcpy(username,lista_logins[i].username);
			encontrou = true;
		}
	}
	
	if(encontrou == false)
	{
		strcpy(resposta,"ID inválido");
		resp_ok[0] = '0';
		return false;
	}
	
	strcpy(resposta,"As suas contas:\n");
	for(i=0; i < num_contas; i++)
	{
		if(strcmp(username,lista_contas[i].username) == 0)
		{
			char conta[32];
			sprintf(conta, "NIB: %s\n",lista_contas[i].NIB);
			strcat(resposta,conta);
		}
	}
	resp_ok[0] = '1';
	
	return true;
}

bool consulta_saldo(char* dados)
{
	char* oper = strtok(dados, ";");
	char* ID = strtok(NULL, ";");
	char* NIB = strtok(NULL, ";");
	
	if(oper==NULL || ID==NULL || NIB==NULL)
	{
		resp_ok[0] = '0';
		strcpy(resposta, "Parâmetros inválidos");
		return false;
	}
	
	//Verificar login
	int i;
	char username[32];
	bool encontrou=false;
	for(i=0; i < num_logins; i++)
	{
		if(strcmp(ID,lista_logins[i].ID) == 0)
		{
			strcpy(username,lista_logins[i].username);
			encontrou = true;
		}
	}
	
	if(encontrou == false)
	{
		strcpy(resposta,"ID inválido");
		resp_ok[0] = '0';
		return false;
	}
	
	encontrou=false;
	for(i=0; i < num_contas; i++)
	{
		if(strcmp(username,lista_contas[i].username) == 0 && strcmp(NIB,lista_contas[i].NIB) == 0)
		{
			sprintf(resposta, "O saldo da sua conta é: %d\n",lista_contas[i].saldo);
			encontrou=true;
			resp_ok[0] = '1';
		}
	}
	
	if(encontrou==false)
	{
		strcpy(resposta,"O NIB não existe ou não pertencem ao utilizador");
		resp_ok[0] = '0';
		return false;
	}
	
	return true;
}

bool saldo_integrado(char* dados)
{
	char* oper     = strtok(dados, ";");
	char* ID       = strtok(NULL, ";");
	
	if(oper==NULL || ID==NULL)
	{
		resp_ok[0] = '0';
		strcpy(resposta, "Parâmetros inválidos");
		return false;
	}
	
	//Verificar login
	int i;
	char username[32];
	bool encontrou=false;
	for(i=0; i < num_logins; i++)
	{
		if(strcmp(ID,lista_logins[i].ID) == 0)
		{
			strcpy(username,lista_logins[i].username);
			encontrou = true;
		}
	}
	
	if(encontrou == false)
	{
		strcpy(resposta,"ID inválido");
		resp_ok[0] = '0';
		return false;
	}
	
	int saldo=0;
	for(i=0; i < num_contas; i++)
	{
		if(strcmp(username,lista_contas[i].username) == 0)
		{
			saldo = saldo + lista_contas[i].saldo;
		}
	}
	
	sprintf(resposta, "O saldo integrado das suas contas é: %d\n",saldo);
	resp_ok[0] = '1';
	
	return true;
}


bool listar_movimentos_conta(char* dados)
{
	char* oper = strtok(dados, ";");
	char* ID = strtok(NULL, ";");
	char* NIB = strtok(NULL, ";");
	
	if(oper==NULL || ID==NULL || NIB==NULL)
	{
		resp_ok[0] = '0';
		strcpy(resposta, "Parâmetros inválidos");
		return false;
	}
	
	//Verificar login
	int i;
	char username[32];
	bool encontrou=false;
	for(i=0; i < num_logins; i++)
	{
		if(strcmp(ID,lista_logins[i].ID) == 0)
		{
			strcpy(username,lista_logins[i].username);
			encontrou = true;
		}
	}
	
	if(encontrou == false)
	{
		strcpy(resposta,"ID inválido");
		resp_ok[0] = '0';
		return false;
	}
	
	//Verificar se a conta pertence ao utilizador
	encontrou=false;
	for(i=0; i < num_contas; i++)
	{
		if(strcmp(username,lista_contas[i].username) == 0 && strcmp(NIB,lista_contas[i].NIB) == 0)
		{
			sprintf(resposta, "O saldo da sua conta é: %d\n",lista_contas[i].saldo);
			encontrou=true;
			resp_ok[0] = '1';
		}
	}
	
	if(encontrou==false)
	{
		strcpy(resposta,"O NIB não existe ou não pertence ao utilizador");
		resp_ok[0] = '0';
		return false;
	}
	
	//Listar Movimentos
	sprintf(resposta, "Lista de movimentos associados ao NIB:\n",NIB);
	encontrou=false;
	for(i=0; i < num_movimentos; i++)
	{
		if(strcmp(NIB,lista_movimentos[i].NIB) == 0)
		{
			strcat(resposta,lista_movimentos[i].descricao);
			strcat(resposta,"\n");
			encontrou=true;
			resp_ok[0] = '1';
		}
	}
	
	if(encontrou==false)
	{
		sprintf(resposta, "Não existem movimentos associados ao NIB:\n",NIB);
	}
	
	return true;
}

bool pagamento_servico(char* dados)
{
	char* oper = strtok(dados, ";");
	char* ID = strtok(NULL, ";");
	char* NIB = strtok(NULL, ";");
	char* serv = strtok(NULL, ";");
	char* valor = strtok(NULL, ";");
	
	if(oper==NULL || ID==NULL || NIB==NULL || serv==NULL || valor==NULL)
	{
		resp_ok[0] = '0';
		strcpy(resposta, "Parâmetros inválidos");
		return false;
	}
	
	int nr_servico = atoi(serv);
	int montante = atoi(valor);
	
	//Verificar login
	int i;
	char username[32];
	bool encontrou=false;
	for(i=0; i < num_logins; i++)
	{
		if(strcmp(ID,lista_logins[i].ID) == 0)
		{
			strcpy(username,lista_logins[i].username);
			encontrou = true;
		}
	}
	
	if(encontrou == false)
	{
		strcpy(resposta,"ID inválido");
		resp_ok[0] = '0';
		return false;
	}
	
	bool encontrou_nib=false;
	int num_conta_cliente=0;
	for(i=0; i < num_contas; i++)
	{
		//procurar a conta cliente
		if(strcmp(username,lista_contas[i].username) == 0 && strcmp(NIB,lista_contas[i].NIB) == 0)
		{
			encontrou_nib=true;
			num_conta_cliente = i; //Guardar a posição da conta
		}
	}
	
	if(encontrou_nib==false)
	{
		strcpy(resposta,"A conta para pagamento não pertence ao utilizador");
		resp_ok[0] = '0';
		return false;
	}
	
	if(lista_contas[num_conta_cliente].congelada==true)
	{
		strcpy(resposta,"A conta indicada encontra-se congelada");
		resp_ok[0] = '0';
		return false;
	}
	
	//Verificar se a conta do cliente tem saldo suficiente
	if(montante > lista_contas[num_conta_cliente].saldo)
	{
		strcpy(resposta,"A conta para pagamento não tem saldo suficiente para o montante indicado");
		resp_ok[0] = '0';
		return false;
	}
	
	//Procurar o serviço
	bool encontrou_serv=false;
	int num_servico=0;
	for(i=0; i < num_servicos; i++)
	{
		if(lista_servicos[i].num_servico == nr_servico)
		{
			num_servico = i;
			encontrou_serv = true;
		}
	}
	
	if(encontrou_serv==false)
	{
		strcpy(resposta,"O número de serviço indicado nao existe");
		resp_ok[0] = '0';
		return false;
	}
	
	char NIB_Servico[21];
	strcpy(NIB_Servico, lista_servicos[num_servico].NIB);
	
	//Procurar a conta da entidade para creditar o pagamento do serviço
	encontrou_nib=false;
	for(i=0; i < num_contas; i++)
	{
		if(strcmp(NIB_Servico,lista_contas[i].NIB) == 0)
		{
			//Verificar se a conta da entidade se encontra congelada
			if(lista_contas[i].congelada==true)
			{
				strcpy(resposta,"A conta associada ao serviço encontra-se congelada");
				resp_ok[0] = '0';
				return false;
			}
			
			encontrou_nib=true;
			lista_contas[i].saldo = lista_contas[i].saldo + montante; //por o saldo na conta da entidade
			lista_contas[num_conta_cliente].saldo = lista_contas[num_conta_cliente].saldo - montante; //retirar o saldo da conta do cliente
			resp_ok[0] = '1';
			sprintf(resposta,"Pagamento efetuado com sucesso.\n Serviço: %d - %s Montante: %d\n",nr_servico,lista_servicos[num_servico].nome,montante);
			
			//Registar o movimento da conta do cliente
			strcpy(lista_movimentos[num_movimentos].NIB,lista_contas[num_conta_cliente].NIB);
			sprintf(lista_movimentos[num_movimentos].descricao,"Débito: Pagamento de Serviços - Serviço: %d - %s Montante: %d",nr_servico,lista_servicos[num_servico].nome,montante);
			num_movimentos=num_movimentos+1;
			//Registar o movimento da conta da entidade
			strcpy(lista_movimentos[num_movimentos].NIB,lista_contas[i].NIB);
			sprintf(lista_movimentos[num_movimentos].descricao,"Crédito: Pagamento de Serviços - Serviço: %d - %s Montante: %d",nr_servico,lista_servicos[num_servico].nome,montante);
			num_movimentos=num_movimentos+1;
		}
	}
	
	if(encontrou_nib==false)
	{
		strcpy(resposta,"A conta para pagamento não pertence ao utilizador");
		resp_ok[0] = '0';
		return false;
	}
	return true;
}

bool movimento_entre_contas(char* dados)
{
	char* oper = strtok(dados, ";");
	char* ID = strtok(NULL, ";");
	char* NIB1 = strtok(NULL, ";");
	char* NIB2 = strtok(NULL, ";");
	char* valor = strtok(NULL, ";");
	
	if(oper==NULL || ID==NULL || NIB1==NULL || NIB2==NULL || valor==NULL)
	{
		resp_ok[0] = '0';
		strcpy(resposta, "Parâmetros inválidos");
		return false;
	}
	
	int montante = atoi(valor);
	
	//Verificar login
	int i;
	char username[32];
	bool encontrou=false;
	for(i=0; i < num_logins; i++)
	{
		if(strcmp(ID,lista_logins[i].ID) == 0)
		{
			strcpy(username,lista_logins[i].username);
			encontrou = true;
		}
	}
	
	if(encontrou == false)
	{
		strcpy(resposta,"ID inválido");
		resp_ok[0] = '0';
		return false;
	}
	
	//Verificar que a conta de origem e destino pertencem ao utilizador
	bool encontrou_nib1=false;
	bool encontrou_nib2=false;
	int num_conta_origem=0;
	int num_conta_destino=0;
	for(i=0; i < num_contas; i++)
	{
		//conta origem
		if(strcmp(username,lista_contas[i].username) == 0 && strcmp(NIB1,lista_contas[i].NIB) == 0)
		{
			encontrou_nib1=true;
			num_conta_origem = i; //Guardar a posição da conta
		}
		//conta destino
		if(strcmp(username,lista_contas[i].username) == 0 && strcmp(NIB2,lista_contas[i].NIB) == 0)
		{
			encontrou_nib2=true;
			num_conta_destino = i; //Guardar a posição da conta
		}
	}
	
	if(encontrou_nib1==false || encontrou_nib2==false)
	{
		strcpy(resposta,"A conta de origem/destino não pertencem ao utilizador");
		resp_ok[0] = '0';
		return false;
	}
	
	//Verificar se alguma das contas se encontra congelada
	if(lista_contas[num_conta_origem].congelada==true || lista_contas[num_conta_destino].congelada==true)
	{
		strcpy(resposta,"A conta de origem/destino encontra-se congelada");
		resp_ok[0] = '0';
		return false;
	}
	
	//Verificar se a conta de destino tem saldo suficiente
	if(montante > lista_contas[num_conta_origem].saldo)
	{
		strcpy(resposta,"A conta de origem não tem saldo suficiente para o montante indicado");
		resp_ok[0] = '0';
		return false;
	}
	
	//Fazer o movimento
	lista_contas[num_conta_origem].saldo = lista_contas[num_conta_origem].saldo - montante;
	lista_contas[num_conta_destino].saldo = lista_contas[num_conta_destino].saldo + montante; 
	
	//Registar o movimento da conta de origem
	strcpy(lista_movimentos[num_movimentos].NIB,lista_contas[num_conta_origem].NIB);
	sprintf(lista_movimentos[num_movimentos].descricao,"Débito: Movimento entre contas - Conta Destino: %s Montante: %d",lista_contas[num_conta_destino].NIB,montante);
	num_movimentos=num_movimentos+1;
	//Registar o movimento da conta de destino
	strcpy(lista_movimentos[num_movimentos].NIB,lista_contas[num_conta_destino].NIB);
	sprintf(lista_movimentos[num_movimentos].descricao,"Crédito: Movimento entre contas - Conta Origem: %s Montante: %d",lista_contas[num_conta_origem].NIB,montante);
	num_movimentos=num_movimentos+1;
	
	sprintf(resposta,"Movimento efetuado com sucesso.\n Origem NIB: %s - Saldo:%d\n Destino NIB: %s - Saldo:%d\n",lista_contas[num_conta_origem].NIB,lista_contas[num_conta_origem].saldo,lista_contas[num_conta_destino].NIB,lista_contas[num_conta_destino].saldo);
	resp_ok[0] = '1';
	return true;
}

bool listar_servicos(char* dados)
{
	//Os serviços são comuns a todos por isso não precisa validar o ID
	if(num_servicos==0)
	{
		strcpy(resposta,"Não existem serviços de pagamento\n");
	}
	else
	{
		strcpy(resposta,"Os serviços de pagamento existentes são:\n");
		int i;
		for(i=0; i < num_servicos; i++)
		{
			char servico[32];
			sprintf(servico, "%d - %s\n",lista_servicos[i].num_servico,lista_servicos[i].nome);
			strcat(resposta,servico);
		}
	}
	resp_ok[0] = '1';
	
	return true;
}

bool criar_servico(char* dados)
{
	char* oper  = strtok(dados, ";");
	char* ID = strtok(NULL, ";");
	char* NIB = strtok(NULL, ";");
	char* nome = strtok(NULL, ";");
	
	if(oper==NULL || ID==NULL || NIB==NULL || nome==NULL)
	{
		resp_ok[0] = '0';
		strcpy(resposta, "Parâmetros inválidos");
		return false;
	}
	
	if(num_servicos >= 100)
	{
		resp_ok[0] = '0';
		strcpy(resposta, "Número máximo de serviços atingido");
		return false;
	}
	
	//Verificar login
	int i;
	char username[32];
	bool encontrou=false;
	for(i=0; i <= num_logins; i++)
	{
		if(strcmp(ID,lista_logins[i].ID) == 0)
		{
			strcpy(username,lista_logins[i].username);
			encontrou = true;
		}
	}
	
	if(encontrou == false)
	{
		strcpy(resposta,"O ID é inválido");
		resp_ok[0] = '0';
		return false;
	}
	
	//Verificar se o NIB da conta está associado ao username
	encontrou=false;
	for(i=0; i <= num_contas; i++)
	{
		if(strcmp(username,lista_contas[i].username) == 0 && strcmp(NIB,lista_contas[i].NIB) == 0)
		{
			encontrou = true;
			strcpy(NIB,lista_contas[i].NIB);
		}
	}
		
	if(encontrou == false)
	{
		strcpy(resposta,"Não foi possível encontrar o NIB associado ao ID");
		resp_ok[0] = '0';
		return false;
	}
	
	lista_servicos[num_servicos].num_servico = id_servico;
	sprintf(lista_servicos[num_servicos].nome, nome);
	strcpy(lista_servicos[num_servicos].NIB, NIB);
	sprintf(resposta, "Criado novo serviço com o número %d",id_servico);
	resp_ok[0] = '1';
	id_servico = id_servico+1;
	num_servicos=num_servicos+1;
}


bool lista_clientes_saldo(char* dados)
{
	char* oper     = strtok(dados, ";");
	char* ID       = strtok(NULL, ";");
	
	if(oper==NULL || ID==NULL)
	{
		resp_ok[0] = '0';
		strcpy(resposta, "Parâmetros inválidos");
		return false;
	}
	
	//Verificar login
	int i;
	char username[32];
	bool encontrou=false;
	for(i=0; i < num_logins; i++)
	{
		if(strcmp(ID,lista_logins[i].ID) == 0)
		{
			strcpy(username,lista_logins[i].username);
			encontrou = true;
		}
	}
	
	if(encontrou == false)
	{
		strcpy(resposta,"ID inválido");
		resp_ok[0] = '0';
		return false;
	}
	
	//Para cada cliente somar o saldo das suas contas
	sprintf(resposta, "Lista de clientes e respetivos saldos:\n");
	for(i=0; i < num_clientes; i++)
	{
		int j;
		int saldo=0;
		for(j=0; j < num_contas; j++)
		{
			if(strcmp(lista_clientes[i].username,lista_contas[j].username) == 0)
			{
				saldo = saldo + lista_contas[j].saldo;
			}
		}
		char cliente[32];
		sprintf(cliente, "Username: %s Nome: %s Saldo:%d\n",lista_clientes[i].username,lista_clientes[i].nome,saldo);
		strcat(resposta,cliente);
	}
	resp_ok[0] = '1';
	
	return true;
}


bool lista_entidades_saldo(char* dados)
{
	char* oper     = strtok(dados, ";");
	char* ID       = strtok(NULL, ";");
	
	if(oper==NULL || ID==NULL)
	{
		resp_ok[0] = '0';
		strcpy(resposta, "Parâmetros inválidos");
		return false;
	}
	
	//Verificar login
	int i;
	char username[32];
	bool encontrou=false;
	for(i=0; i < num_logins; i++)
	{
		if(strcmp(ID,lista_logins[i].ID) == 0)
		{
			strcpy(username,lista_logins[i].username);
			encontrou = true;
		}
	}
	
	if(encontrou == false)
	{
		strcpy(resposta,"ID inválido");
		resp_ok[0] = '0';
		return false;
	}
	
	//Para cada cliente somar o saldo das suas contas
	sprintf(resposta, "Lista de clientes e respetivos saldos:\n");
	for(i=0; i < num_entidades; i++)
	{
		int j;
		int saldo=0;
		for(j=0; j < num_contas; j++)
		{
			if(strcmp(lista_entidades[i].username,lista_contas[j].username) == 0)
			{
				saldo = saldo + lista_contas[j].saldo;
			}
		}
		char cliente[32];
		sprintf(cliente, "Username: %s Nome: %s Saldo:%d\n",lista_entidades[i].username,lista_entidades[i].nome,saldo);
		strcat(resposta,cliente);
	}
	resp_ok[0] = '1';
	
	return true;
}

bool valor_depositado_banco(char* dados)
{
	char* oper     = strtok(dados, ";");
	char* ID       = strtok(NULL, ";");
	
	if(oper==NULL || ID==NULL)
	{
		resp_ok[0] = '0';
		strcpy(resposta, "Parâmetros inválidos");
		return false;
	}
	
	//Verificar login
	int i;
	char username[32];
	bool encontrou=false;
	for(i=0; i < num_logins; i++)
	{
		if(strcmp(ID,lista_logins[i].ID) == 0)
		{
			strcpy(username,lista_logins[i].username);
			encontrou = true;
		}
	}
	
	if(encontrou == false)
	{
		strcpy(resposta,"ID inválido");
		resp_ok[0] = '0';
		return false;
	}
	
	//Para cada cliente somar o saldo das suas contas
	

	int saldo=0;
	for(i=0; i < num_contas; i++)
	{
		saldo = saldo + lista_contas[i].saldo;
	}
	sprintf(resposta, "Valor total depositado no banco: %d\n",saldo);
	resp_ok[0] = '1';
	
	return true;
}


bool listar_todas_contas(char* dados)
{
	char* oper     = strtok(dados, ";");
	char* ID       = strtok(NULL, ";");
	
	if(oper==NULL || ID==NULL)
	{
		resp_ok[0] = '0';
		strcpy(resposta, "Parâmetros inválidos");
		return false;
	}
	
	//Verificar login
	int i;
	char username[32];
	bool encontrou=false;
	for(i=0; i < num_logins; i++)
	{
		if(strcmp(ID,lista_logins[i].ID) == 0)
		{
			strcpy(username,lista_logins[i].username);
			encontrou = true;
		}
	}
	
	if(encontrou == false)
	{
		strcpy(resposta,"ID inválido");
		resp_ok[0] = '0';
		return false;
	}
	
	strcpy(resposta,"As contas existentes são:\n");
	for(i=0; i < num_contas; i++)
	{
		char conta[32];
		sprintf(conta, "Username: %s NIB: %s\n",lista_contas[i].username,lista_contas[i].NIB);
		strcat(resposta,conta);
	}
	resp_ok[0] = '1';
	
	return true;
}


bool congelar_conta(char* dados)
{
	char* oper = strtok(dados, ";");
	char* ID = strtok(NULL, ";");
	char* NIB = strtok(NULL, ";");
	
	if(oper==NULL || ID==NULL || NIB==NULL)
	{
		resp_ok[0] = '0';
		strcpy(resposta, "Parâmetros inválidos");
		return false;
	}
	
	//Verificar login
	int i;
	char username[32];
	bool encontrou=false;
	for(i=0; i < num_logins; i++)
	{
		if(strcmp(ID,lista_logins[i].ID) == 0)
		{
			strcpy(username,lista_logins[i].username);
			encontrou = true;
		}
	}
	
	if(encontrou == false)
	{
		strcpy(resposta,"ID inválido");
		resp_ok[0] = '0';
		return false;
	}
	
	encontrou=false;
	for(i=0; i <= num_contas; i++)
	{
		if(strcmp(NIB,lista_contas[i].NIB) == 0)
		{
			encontrou = true;
			lista_contas[i].congelada=true;
		}
	}
	
	if(encontrou == false)
	{
		strcpy(resposta,"NIB de conta não encontrado");
		resp_ok[0] = '0';
		return false;
	}
	
	strcpy(resposta,"Conta congelada");
	resp_ok[0] = '1';
	return true;
}


bool descongelar_conta(char* dados)
{
	char* oper = strtok(dados, ";");
	char* ID = strtok(NULL, ";");
	char* NIB = strtok(NULL, ";");
	
	if(oper==NULL || ID==NULL || NIB==NULL)
	{
		resp_ok[0] = '0';
		strcpy(resposta, "Parâmetros inválidos");
		return false;
	}
	
	//Verificar login
	int i;
	char username[32];
	bool encontrou=false;
	for(i=0; i < num_logins; i++)
	{
		if(strcmp(ID,lista_logins[i].ID) == 0)
		{
			strcpy(username,lista_logins[i].username);
			encontrou = true;
		}
	}
	
	if(encontrou == false)
	{
		strcpy(resposta,"ID inválido");
		resp_ok[0] = '0';
		return false;
	}
	
	encontrou=false;
	for(i=0; i <= num_contas; i++)
	{
		if(strcmp(NIB,lista_contas[i].NIB) == 0)
		{
			encontrou = true;
			lista_contas[i].congelada=false;
		}
	}
	
	if(encontrou == false)
	{
		strcpy(resposta,"NIB de conta não encontrado");
		resp_ok[0] = '0';
		return false;
	}
	
	strcpy(resposta,"Conta descongelada");
	resp_ok[0] = '1';
	return true;
}


bool apagar_conta(char* dados)
{
	char* oper = strtok(dados, ";");
	char* ID = strtok(NULL, ";");
	char* NIB1 = strtok(NULL, ";");
	char* NIB2 = strtok(NULL, ";");
	
	if(oper==NULL || ID==NULL || NIB1==NULL || NIB2==NULL)
	{
		resp_ok[0] = '0';
		strcpy(resposta, "Parâmetros inválidos");
		return false;
	}
	
	//Verificar login
	int i;
	char username[32];
	bool encontrou=false;
	for(i=0; i < num_logins; i++)
	{
		if(strcmp(ID,lista_logins[i].ID) == 0)
		{
			strcpy(username,lista_logins[i].username);
			encontrou = true;
		}
	}
	
	if(encontrou == false)
	{
		strcpy(resposta,"ID inválido");
		resp_ok[0] = '0';
		return false;
	}
	
	//Verificar que a conta de origem e destino pertencem ao utilizador
	bool encontrou_nib1=false;
	bool encontrou_nib2=false;
	int num_conta_origem=0;
	int num_conta_destino=0;
	for(i=0; i < num_contas; i++)
	{
		//conta origem
		if(strcmp(username,lista_contas[i].username) == 0 && strcmp(NIB1,lista_contas[i].NIB) == 0)
		{
			encontrou_nib1=true;
			num_conta_origem = i; //Guardar a posição da conta
		}
		//conta destino
		if(strcmp(username,lista_contas[i].username) == 0 && strcmp(NIB2,lista_contas[i].NIB) == 0)
		{
			encontrou_nib2=true;
			num_conta_destino = i; //Guardar a posição da conta
		}
	}
	
	if(encontrou_nib1==false || encontrou_nib2==false)
	{
		strcpy(resposta,"A conta de origem/destino não pertencem ao utilizador");
		resp_ok[0] = '0';
		return false;
	}
	
	//Verificar se alguma das contas se encontra congelada
	if(lista_contas[num_conta_origem].congelada==true || lista_contas[num_conta_destino].congelada==true)
	{
		strcpy(resposta,"A conta de origem/destino encontra-se congelada");
		resp_ok[0] = '0';
		return false;
	}
	
	
	//Fazer o movimento
	lista_contas[num_conta_destino].saldo = lista_contas[num_conta_destino].saldo + lista_contas[num_conta_origem].saldo;
	//Registar o movimento da conta de destino
	strcpy(lista_movimentos[num_movimentos].NIB,lista_contas[num_conta_destino].NIB);
	sprintf(lista_movimentos[num_movimentos].descricao,"Crédito: Fecho de Conta - Conta Origem: %s Montante: %d",NIB2,lista_contas[num_conta_origem].saldo);
	num_movimentos=num_movimentos+1;
	
	//Apagar o NIB e o saldo
	lista_contas[num_conta_origem].saldo = 0;
	strcpy(lista_contas[num_conta_origem].NIB,"");
	strcpy(lista_contas[num_conta_origem].username,"");
	
	sprintf(resposta,"Conta fechada com sucesso.\n Conta Destino NIB: %s - Saldo:%d\n",lista_contas[num_conta_destino].NIB,lista_contas[num_conta_destino].saldo);
	resp_ok[0] = '1';
	return true;
}


main() {
	#ifndef unix 	// se o SO for windows
	WSAStartup(MAKEWORD(2, 0), &wsa_data);
	#endif
	
	ler_dados();
	
	struct sockaddr_in cliente; // preciso de outro para o cliente
		
	// Definição e população da estructura sockaddr_in -------------------------
	struct sockaddr_in servidor;	
	memset(&(servidor.sin_zero),0, sizeof(servidor.sin_zero));
	servidor.sin_family = AF_INET;
	servidor.sin_addr.s_addr = htonl(INADDR_ANY);
	servidor.sin_port = htons(2000); //servidor na porta 2000 
	
	//criar o socket	--------------------------------------------------------
	int sock = socket(AF_INET,SOCK_STREAM,0);
	
	//fazer o bind	------------------------------------------------------------
	int bindResult = bind(sock, (struct sockaddr *) &servidor, sizeof(servidor));
	if (bindResult==-1) // verifica se houve erros no bind
		printf("Bind: Falhou!\n");
	else
		printf("Bind: ok!\n"); 

	// fazer o listen com uma fila de 1 cliente no máximo ----------------------
	int tamanho_fila = 1;
	if (listen(sock,tamanho_fila) == -1)  // verifica se o listen falhou
		printf("Listen: Falhou!\n");
	else 
		printf("Listen: ok!\n");
	
	char mensagemRecebida[2048];	// variável para guardar o que é lido do socket
	int nbytes;				// guarda o número de bytes da mensagem lida
	int tamanho_cliente;	// guarda o tamanho da estructura do cliente
	int sock_aceite;		// guarda socket aceite
	
	tamanho_cliente = sizeof(struct sockaddr_in); //por imposição do accept temos que fazer isto

	while(1) {				// o servidor vai estar sempre a correr
		
		// fica à espera que um cliente se ligue
		sock_aceite = accept(sock, (struct sockaddr *)&cliente, &tamanho_cliente);
		
		
		if (sock_aceite>-1) 
		{	//se recebeu um cliente
			//mostra informações do cliente
			printf("Recebida uma ligacao do cliente IP: %s\n",inet_ntoa(cliente.sin_addr));
			
			// lê uma mensagem do socket
			nbytes = recv(sock_aceite, mensagemRecebida,sizeof(mensagemRecebida)+1, 0);
			if (nbytes>0) 
			{	
				char dados[2048];
				strcpy(dados,mensagemRecebida);
				// processar o código da operação
				char* codOperacao = strtok(dados, ";");
				
				switch(atoi(codOperacao))
				{
				case 1: //registo
					novo_registo(mensagemRecebida);
					break;
				case 2: //login
					novo_login(mensagemRecebida);
					break;
				case 3: //criar conta
					criar_conta(mensagemRecebida);
					break;
				case 4: //listar contas
					listar_contas(mensagemRecebida);
					break;
				case 5: //Consulta de Saldo - 1º listar contas
					listar_contas(mensagemRecebida);
					break;
				case 51: //Consulta de Saldo - 2º saldo da conta
					consulta_saldo(mensagemRecebida);
					break;
				case 6: //Pagamento Serviço - 1º listar contas
					listar_contas(mensagemRecebida);
					break;
				case 61:
					pagamento_servico(mensagemRecebida);
					break;
				case 7: //Saldo integrado
					saldo_integrado(mensagemRecebida);
					break;
				case 8: //Consulta Movimentos de conta - 1º listar contas
					listar_contas(mensagemRecebida);
					break;
				case 81: //Consulta Movimentos de conta - 2º lista dos movimentos
					listar_movimentos_conta(mensagemRecebida);
					break;
				case 9: //Movimento entre contas - 1º listar contas
					listar_contas(mensagemRecebida);
					break;
				case 91: //Movimento entre contas - 2º fazer o movimento
					movimento_entre_contas(mensagemRecebida);
					break;
				case 10: //Listar serviços
					listar_servicos(mensagemRecebida);
					break;
				case 11: //Criar serviço - 1º listar as contas
					listar_contas(mensagemRecebida);
					break;
				case 111: //Criar serviço - 2º criar o serviço
					criar_servico(mensagemRecebida);
					break;
				case 12: //Lista saldos clientes
					lista_clientes_saldo(mensagemRecebida);
					break;
				case 13: //lista saldos entidades
					lista_entidades_saldo(mensagemRecebida);
					break;
				case 14: //valor depositado no banco
					valor_depositado_banco(mensagemRecebida);
					break;
				case 15: //Congelar conta - 1º listar todas as contas
					listar_todas_contas(mensagemRecebida);
					break;
				case 151: //Congelar conta - 2º Congelar a conta
					congelar_conta(mensagemRecebida);
					break;
				case 16: //Desongelar conta - 1º listar todas as contas
					listar_todas_contas(mensagemRecebida);
					break;
				case 161: //Desongelar conta - 2º Congelar a conta
					descongelar_conta(mensagemRecebida);
					break;
				case 17: //Apagar conta - 1º listar todas as contas
					listar_contas(mensagemRecebida);
					break;
				case 171: //Apagar conta - 2º Apagar a conta
					apagar_conta(mensagemRecebida);
					break;
				default:
					strcpy(resposta, "Código de Operação Inválido");
					break;
				}
				send(sock_aceite, resp_ok,strlen(resp_ok)+1, 0);
				send(sock_aceite, resposta,strlen(resposta)+1, 0);
				gravar_dados();
			}
		}
	}
}
