#include <iostream>  // cout
#include <sstream>   // cout
#include <stdlib.h>  // exit
#include <string.h>  // bzero
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <pthread.h>
#include <ctype.h>
#include <postgresql/libpq-fe.h>
#include <set>
#include <map> // BIBLIOTECA QUE PERMITE DETERMINAR ESTRUTURAS MAP PARA DETERMINAR OS SOCKETS E PROCURAR USUARIOS NELAS CRIADOS
#include <algorithm>
#include <cstring>
#include <string>

using namespace std;
#define SOMA 0
#define II 0
#define ADMIN "admin"

time_t seconds = time (NULL);
//A função localtime retorna uma estrutura com os dados da data:

struct tm * timeinfo = localtime (&seconds);


string intToString(int i) {
        ostringstream oss;
        oss << i;
        return oss.str();
}

//----------------------------------------------------------------------------------------------------------//
//					  	   DEFINIÇOES DE CONEXÃO/DESCONEXÃO COM A BASE DE DADOS
//----------------------------------------------------------------------------------------------------------//

PGconn* conn = NULL; //INICIA ESTADO DA CONEXÃO (DESCONECTADO)

void initDB();  				   //INICIA CONEXÃO
PGresult *executeSQL(string sql);  //RECEBE EXECUTA COMANDOS SQL
void closeDB();                    //FECHA SESSÃO COM A BD


//----------------------------------------------------------------------------------------------------------//
//					  	   				 DEFINIÇOES MAP - SOCKETS
//----------------------------------------------------------------------------------------------------------//

//OS MAPAS SÃO RECIPIENTES ASSOCIATIVOS QUE ARMAZENAM  ELEMENTOS FORMADOS POR UMA COMBINAÇÃO DE UM VARIAVEL CHAVE COM UMA VARIAVEL A SER MAPEADA.
//CRIA ESTRURA MAP PARA OS SOCKETS, USERNAMES, CLIENTES, LIGAÇOES
map<string, int> sockets;   //MAP COM VARIAVEL CHAVE string E VARIAVEL ATRIBUIDA int
map<int, string> usernames; //MAP COM VARIAVEL CHAVE int E VARIAVEL ATRIBUIDA string
map<string,int>  clientes;  //MAP COM VARIAVEL CHAVE string E VARIAVEL ATRIBUIDA int
map<int, string> ligacoes; 	//MAP COM VARIAVEL CHAVE int E VARIAVEL ATRIBUIDA string
set<int> clients;     	 	//DETERMINA OS int's (socket_id) QUE SERAM ATRIBUIDO A CHAVES DA MAP usernames EX: usernames[socket_id] = username;




void writeline(int socket_id, string line);   	// PREENCHE CHAVE TIPO string DA MAP sockets
bool readline(int socket_id, string &line);  	// LÊ LINHA
void broadcast (int origin, string text);    	// FAZ COMUNICAÇÃO COM A BD
void* cliente(void* args) ;                     // PERMITE USAR COMANDOS SQL
void ClearScreen();
void mpp_jogar(int socket_id, string line);

//----------------------------------------------------------------------------------------------------------//
//					  			     INICIALIZAÇÃO DAS FUNÇOES DO JOGO
//----------------------------------------------------------------------------------------------------------//

//FUNÇOES DE INICIALIZAÇÃO DO JOGO

void mpp_start(int socket_id,int soma, int ii);       //FUNÇÃO QUE DA INICIO AO JOGO***
void mpp_register(int socket_id, string line);   //FUNÇÃO DE REGISTO
void mpp_login(int socket_id, string line);  //FUNÇÃO LOGIN
void mpp_setadmin(string line, int socket_id);  //FUNÇÃO DETERMINA ADMINISTRADOR

//FUNÇOES SUPORTE AO USUARIO
void mpp_logout(string line, int socket_id);   	  //FUNÇÃO LOGOUT
void mpp_help(int socket_id);       	 		  //LISTA AS FUNÇOES DISPONIVEIS PARA OS USUARIOS
void mpp_shutdown (int socket_id, string line) ;                             //DESLIGA DO SERVIDOR E SAI DO JOGO

//FUNÇOES DE INTERAÇÃO COM O JOGO E MULTI - JOGADO
void mpp_challenge(int socket_id, string line);        //FUNÇÃO DE DESAFIO DO JOGO
void mpp_friend_list(string user, int socket_id, string line);      //FUNÇÃO QUE LISTA O NUMERO DE AMIGOS
void mpp_top10();  		  // FUNÇÃO TOP10
void mpp_fifty_fifty(string pergunta);   // FUNÇÃO 50-50
int mpp_change(int socket_id, int num, int ii,int ponto);        // FUNÇÃO PULAR QUESTÃO


//FUNÇOES INTERNAS DO JOGO - CHECK FUNTIONS
bool mpp_readCommand(int socket_id, string line);     //FUNÇÃO QUE LE O COMANDO
bool mpp_is_logged(int socket_id );     //FUNÇÃO QUE VERIFICA SE UTILIZADR ESTA LOGADO
int  mpp_identify(int socket_id);       //FUNÇÃO IDENTIFICA ADMINISTRADOR
void mpp_kick (int socket_id, string line);       //FUNÇÃO EXPULSA JOGADOR
void mpp_create(int socket_id);    //FUNÇÃO ADD PERGUNSTAS AO JOGO

//----------------------------------------------------------------------------------------------------------//
//                                  FUNÇÕES PARA MANIPULAÇÃO DE VORES
//----------------------------------------------------------------------------------------------------------//
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

//----------------------------------------------------------------------------------------------------------//
//					  					 	 			MAIN
//----------------------------------------------------------------------------------------------------------//

int main(int argc, char *argv[])
{
    srand(time(NULL));

    initDB();


    /* Estruturas de dados */
  int socket_id, newsocket_id, port = rand() % 100 + 3500;
  socklen_t client_addr_length;
  struct sockaddr_in serv_addr, cli_addr;

  cout << "Connecting to port: " << port << endl;

  /* Inicializar o socket
     AF_INET - para indicar que queremos usar IP
     SOCK_STREAM - para indicar que queremos usar TCP
     socket_id - id do socket principal do servidor
     Se retornar < 0 ocorreu um erro */int soma=0;
  socket_id = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_id < 0) {
    cout << "Error creating socket" << endl;
     exit(-1);
  }

  /* Criar a estrutura que guarda o endereço do servidor
     bzero - apaga todos os dados da estrutura (coloca a 0's)
     AF_INET - endereço IP
     INADDR_ANY - aceitar pedidos para qualquer IP */
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port);

  /* Fazer bind do socket. Apenas nesta altura é que o socket fica ativo
     mas ainda não estamos a tentar receber ligações.
     Se retornar < 0 ocorreu um erro */
  int res = bind(socket_id, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
  if (res < 0) {
    cout << "Error binding to socket" << endl;
    exit(-1);
  }

  /* Indicar que queremos escutar no socket com um backlog de 5 (podem
     ficar até 5 ligações pendentes antes de fazermos accept */
  listen(socket_id, 5);

  while(true) {
    /* Aceitar uma nova ligação. O endereço do cliente fica guardado em
       cli_addr - endereço do cliente
       newsocket_id - id do socket que comunica com este cliente */
    client_addr_length = sizeof(cli_addr);
    newsocket_id = accept(socket_id, (struct sockaddr *) &cli_addr, &client_addr_length);

    /* Criar uma thread para tratar dos pedidos do novo cliente */
    pthread_t thread;
    pthread_create(&thread, NULL, cliente, &newsocket_id);
  }

  close(socket_id);
  closeDB();
  return 0;

}



//----------------------------------------------------------------------------------------------------------//
//					  				  	FUNÇOES DO JOGO
//----------------------------------------------------------------------------------------------------------//

//FUNÇOES DE INICIALIZAÇÃO DO JOGO

void mpp_start(int socket_id,int soma,int ii)
{
    if(soma!=0 && ii!=0)
    {
        writeline(socket_id,"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

        std::stringstream j;
        soma = soma -100;
        string num_pontos = intToString(soma);
        j << "Resposta errada! voce perdeu :[" << " seu score foi: (€):"<< num_pontos;
        string s = j.str();
        writeline(socket_id,s);

        PGresult* res = executeSQL("SELECT score FROM utilizador WHERE utilizador.username = '" + usernames[socket_id] + "'");
        int score = atoi(PQgetvalue(res, 0, 0));

        if(score<soma)
        {
        std::stringstream ss2; ss2 << "UPDATE utilizador SET score = '" << soma << "' WHERE utilizador.username = '" << usernames[socket_id] << "'";
        executeSQL(ss2.str());
        writeline(socket_id, "PARABENS!! TEM UM NOVO RECORD");
        }

        writeline(socket_id,"\n");
        writeline(socket_id,"\t\t________________________________________");
        writeline(socket_id,"\t\t     BEM VINDO AO * RESPONDA SE PUDER *");
        writeline(socket_id,"\t\t________________________________________");
        writeline(socket_id,"\t\t________________________________________");
        writeline(socket_id,"\t\t >|mpp_help      -PARA LISTAR COMANDOS");
        writeline(socket_id,"\t\t >|mpp_register  -PARA REGISTRAR-SE");
        writeline(socket_id,"\t\t >|mpp_login     -PARA LOGIN");
        writeline(socket_id,"\t\t >|mpp_jogar     -PARA JOGAR");
        writeline(socket_id,"\t\t >|mpp_challenge -PARA DESAFIAR");
        writeline(socket_id,"\t\t >|mpp_shutdown  -PARA SAIR");
        writeline(socket_id,"\t\t >|mpp_create    -ADD PERGUNTA A BD");
        writeline(socket_id,"\t\t >|mpp_identify  -VERIFICA SE USUARIO X ESTA NA DB");
        writeline(socket_id,"\t\t________________________________________\n\n");

        soma=0;

     }else{
      writeline(socket_id,"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n ");
      writeline(socket_id,"\t\t\t\tSINF 2015 ");
      writeline(socket_id,"\t\t\t  Matheus Macedo Lopes - 201108096");
          writeline(socket_id,"\t\t________________________________________");
      writeline(socket_id,"\t\t________________________________________");
      writeline(socket_id,"\t\t     BEM VINDO AO * RESPONDA SE PUDER *");
      writeline(socket_id,"\t\t________________________________________");
      writeline(socket_id,"\t\t________________________________________");
      writeline(socket_id,"\t\t >|mpp_help      -PARA LISTAR COMANDOS");
      writeline(socket_id,"\t\t >|mpp_register  -PARA REGISTRAR-SE");
      writeline(socket_id,"\t\t >|mpp_login     -PARA LOGIN");
      writeline(socket_id,"\t\t >|mpp_jogar     -PARA JOGAR");
      writeline(socket_id,"\t\t >|mpp_challenge -PARA DESAFIAR");
      writeline(socket_id,"\t\t >|mpp_shutdown  -PARA SAIR");
      writeline(socket_id,"\t\t >|mpp_create    -ADD PERGUNTA A BD");
      writeline(socket_id,"\t\t >|mpp_identify  -VERIFICA SE USUARIO X ESTA NA DB");
      writeline(socket_id,"\t\t________________________________________\n\n");

}
}

//-----------------------------------------------------------------------------------------------------------//
//					          Funções
//-----------------------------------------------------------------------------------------------------------//
int mpp_change(int socket_id, int num, int ii,int ponto){
    int  soma=0;
    string escolha;
    int jogada =0;
    string decide;
    int aux;
    string resposta1;
    string resposta2;
    string resposta3;
    string resposta4;
    string alinea1;
    string alinea2;
    string alinea3;
    string alinea4;
    vector<string> items2;
   string perg;
   ponto=ponto-100;
    stringstream t;
       t << "SELECT pergunta FROM change WHERE change.num ='" << num << "'";
       PGresult* res = executeSQL(t.str());

if (PQntuples(res) == 1)
{
   writeline(socket_id,"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
   if(ii==2)writeline(socket_id, "\tRESPOSTA CORRETA!!!- PODE PROSSEGUIR RUMO AO PREMIO MAXIMO");
   if(ii==3)writeline(socket_id, "\tRESPOSTA CORRETA!!!- ATENÇÃO A DIFICULDADE VAI AUMENTAR!");
   if(ii==4)writeline(socket_id, "\tRESPOSTA CORRETA!!!- TEMOS UM POTENCIAL VENCEDOR");
   if(ii==5)writeline(socket_id, "\tRESPOSTA CORRETA!!!- AMAZING!");
   if(ii==6)writeline(socket_id, "\tRESPOSTA CORRETA!!!- KEEEP GOING!");
   if(ii==7)writeline(socket_id, "\tRESPOSTA CORRETA!!!- ON FIREEE!");
   if(ii==8)writeline(socket_id, "\tRESPOSTA CORRETA!!!- COME ONN!");
   if(ii==9)writeline(socket_id, "\tRESPOSTA CORRETA!!!- zZzZzZZZ");
   if(ii==10)writeline(socket_id,"\tRESPOSTA CORRETA!!!- É AGORA OU NUNCA");
   writeline(socket_id,"____________________________________________________________________");
   std::stringstream ss;
   string num_questao = intToString(num);
   soma=soma+ponto;
   ponto=ponto+100;
   ss << "\t\tQuestão numero:" << num_questao  << " VALENDO (€):"<< ponto;
   string s = ss.str();
   writeline(socket_id,s);
   writeline(socket_id,"____________________________________________________________________");
   string pergunta(PQgetvalue(res, 0, 0));
   //cout << PQgetvalue(res, 0, 0) << endl;
   vector<string> items = split(pergunta, '|');

    for (aux=0 ; aux<5 ; aux++) {
        vector<string> items2 = split(items[aux], '_');
        if (aux ==0) perg = items2[1];
        if (aux ==1){ alinea1 = items2[1]; resposta1 = items2[0]; }
        if (aux ==2){ alinea2 = items2[1]; resposta2 = items2[0]; }
        if (aux ==3){ alinea3 = items2[1]; resposta3 = items2[0]; }
        if (aux ==4){ alinea4 = items2[1]; resposta4 = items2[0]; }
     }
   writeline(socket_id," " + perg + " ");
   writeline(socket_id,"____________________________________________________________________");
   writeline(socket_id,"\t\t\t\t A.\n");
   writeline(socket_id, " " + alinea1  + " ");
   writeline(socket_id,"____________________________________________________________________");
   writeline(socket_id,"\t\t\t\t B.\n");
   writeline(socket_id, " " + alinea2 + " ");
   writeline(socket_id,"____________________________________________________________________");
   writeline(socket_id,"\t\t\t\t C.\n");
   writeline(socket_id, " " + alinea3 + " ");
   writeline(socket_id,"____________________________________________________________________");
   writeline(socket_id,"\t\t\t\t D.\n");
   writeline(socket_id, " " + alinea4 + " ");
   writeline(socket_id,"____________________________________________________________________");
   writeline(socket_id,"Oque pretende fazer- (r)esponder -(p)ular questão -(f)ifty-fifty");

   int cont=2;
   while(cont!=1)
   {
           readline(socket_id,decide);
           if(decide=="r" || decide=="p" || decide=="f") cont=1;
           else writeline(socket_id,"COMANDO ERRADO TENTE NOVAMENTE -r -p ou -f ");
   }
            if(decide=="r")
            {
            writeline(socket_id,"QUAL É SUA RESPOSTA: ");
              readline(socket_id,escolha);
                if(escolha=="a" && resposta1=="c"){ writeline(socket_id, "Resposta correta!");  }
                   else if(escolha=="b" && resposta2=="c"){ writeline(socket_id, "Resposta certa");  }
                       else if(escolha=="c" && resposta3=="c"){ writeline(socket_id, "Resposta certa"); }
                           else if(escolha=="d" && resposta4=="c"){ writeline(socket_id, "Resposta certa");
                    return 0;
                }
               else
                   {

                     return 1;
                   }
            }
      }
}

void mpp_jogar(int socket_id, string line){
int ii;
int num=1,rand_num;
int ponto=0, soma=0;
string escolha;
string decide;
int aux;
    string resposta1;
     string resposta2;
      string resposta3;
       string resposta4;

string alinea1;
    string alinea2;
        string alinea3;
            string alinea4;

    vector<string> items2;
    string perg;


cout<<rand;

    if (mpp_is_logged(socket_id) == 0)
    {
        writeline(socket_id, "Para JOGAR tem que estar logado");




        return ;
    }

    for (ii=1 ; ii<10 ; ii++)
    {
        rand_num = rand() % 10;
         stringstream t;
            t << "SELECT perg FROM pergunta WHERE pergunta.numero ='" << rand_num << "'";
            PGresult* res = executeSQL(t.str());

     if (PQntuples(res) == 1)
    {
        writeline(socket_id,"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
        if(ii==2)writeline(socket_id, "\tRESPOSTA CORRETA!!!- PODE PORCEGUIR RUMO AO PREMIO MAXIMO");
        if(ii==3)writeline(socket_id, "\tRESPOSTA CORRETA!!!- ATENÇÃO A DIFICULDADE VAI AUMENTAR!");
        if(ii==4)writeline(socket_id, "\tRESPOSTA CORRETA!!!- TEMOS UM POTENCIAL VENCEDOR");
        if(ii==5)writeline(socket_id, "\tRESPOSTA CORRETA!!!- AMAZING!");
        if(ii==6)writeline(socket_id, "\tRESPOSTA CORRETA!!!- KEEEP GOING!");
        if(ii==7)writeline(socket_id, "\tRESPOSTA CORRETA!!!- ON FIREEE!");
        if(ii==8)writeline(socket_id, "\tRESPOSTA CORRETA!!!- COME ONN!");
        if(ii==9)writeline(socket_id, "\tRESPOSTA CORRETA!!!- zZzZzZZZ");
        if(ii==10)writeline(socket_id,"\tRESPOSTA CORRETA!!!- É AGORA OU NUNCA");

        writeline(socket_id,"____________________________________________________________________");
        std::stringstream ss;
        string num_questao = intToString(num);
        soma=soma+ponto;
        ponto=ponto+100;
        ss << "\t\tQuestão numero:" << num_questao  << " VALENDO (€):"<< ponto;
        string s = ss.str();
        writeline(socket_id,s);
        writeline(socket_id,"____________________________________________________________________");
        string pergunta(PQgetvalue(res, 0, 0));
        //cout << PQgetvalue(res, 0, 0) << endl;
        vector<string> items = split(pergunta, '|');

         for (aux=0 ; aux<5 ; aux++) {
             vector<string> items2 = split(items[aux], '_');
             if (aux ==0) perg = items2[1];
             if (aux ==1){ alinea1 = items2[1]; resposta1 = items2[0]; }
             if (aux ==2){ alinea2 = items2[1]; resposta2 = items2[0]; }
             if (aux ==3){ alinea3 = items2[1]; resposta3 = items2[0]; }
             if (aux ==4){ alinea4 = items2[1]; resposta4 = items2[0]; }


          }
      //   writeline(socket_id," " + resposta1 + " ");
      //   writeline(socket_id," " + resposta2 + " ");
      //   writeline(socket_id," " + resposta3 + " ");
      //   writeline(socket_id," " + resposta4 + " ");

        writeline(socket_id," " + perg + " ");
        writeline(socket_id,"____________________________________________________________________");
        writeline(socket_id,"\t\t\t\t A.\n");
        writeline(socket_id, " " + alinea1  + " ");
        writeline(socket_id,"____________________________________________________________________");
        writeline(socket_id,"\t\t\t\t B.\n");
        writeline(socket_id, " " + alinea2 + " ");
        writeline(socket_id,"____________________________________________________________________");
        writeline(socket_id,"\t\t\t\t C.\n");
        writeline(socket_id, " " + alinea3 + " ");
        writeline(socket_id,"____________________________________________________________________");
        writeline(socket_id,"\t\t\t\t D.\n");
        writeline(socket_id, " " + alinea4 + " ");
        writeline(socket_id,"____________________________________________________________________");
        writeline(socket_id,"Oque pretende fazer- (r)esponder -(p)ular questão -(f)ifty-fifty");


        int cont=2;
        while(cont!=1)
        {
                readline(socket_id,decide);
                if(decide=="r" || decide=="p" || decide=="f") cont=1;
                else writeline(socket_id,"COMANDO ERRADO TENTE NOVAMENTE -r -p ou -f ");

        }

     if(decide=="r")
    {
     writeline(socket_id,"QUAL É SUA RESPOSTA: ");
       readline(socket_id,escolha);
         if(escolha=="a" && resposta1=="c"){ writeline(socket_id, "Resposta correta!");  }
            else if(escolha=="b" && resposta2=="c"){ writeline(socket_id, "Resposta certa");  }
                else if(escolha=="c" && resposta3=="c"){ writeline(socket_id, "Resposta certa"); }
                    else if(escolha=="d" && resposta4=="c"){ writeline(socket_id, "Resposta certa");

         } else { return mpp_start(socket_id,soma,ii);    }

     }

    }

        if(decide=="f")
        {
            int cont_a =0,cont_b =0,cont_c =0,cont_d =0, aux2=0;
            while(aux2<2){
            if(resposta1=="e" && cont_a!=1){alinea1=" ";  cont_a=1; }
               else if(resposta2=="e" && cont_b!=1){ alinea2=" ";  cont_b=1; }
                   else if(resposta3=="e" && cont_c!=1 ){ alinea3=" ";  cont_c=1; }
                       else if(resposta4=="e" && cont_d!=1){ alinea4=" ";  cont_d=1; }
            aux2++;
            }

            writeline(socket_id,"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
            writeline(socket_id,"foi penalizado em 100€ ");
            writeline(socket_id," " + perg + " ");
            writeline(socket_id,"____________________________________________________________________");
            writeline(socket_id,"\t\t\t\t A.\n");
            writeline(socket_id, " " + alinea1  + " ");
            writeline(socket_id,"____________________________________________________________________");
            writeline(socket_id,"\t\t\t\t B.\n");
            writeline(socket_id, " " + alinea2 + " ");
            writeline(socket_id,"____________________________________________________________________");
            writeline(socket_id,"\t\t\t\t C.\n");
            writeline(socket_id, " " + alinea3 + " ");
            writeline(socket_id,"____________________________________________________________________");
            writeline(socket_id,"\t\t\t\t D.\n");
            writeline(socket_id, " " + alinea4 + " ");
            writeline(socket_id,"____________________________________________________________________");
            writeline(socket_id,"Oque pretende fazer- (r)esponder -(p)ular questão -(f)ifty-fifty");

            int cont=2;
            while(cont!=1)
            {
                    readline(socket_id,decide);
                    if(decide=="r" || decide=="p" || decide=="f") cont=1;
                    else writeline(socket_id,"COMANDO ERRADO TENTE NOVAMENTE -r -p ou -f ");

            }

            writeline(socket_id,"QUAL É SUA RESPOSTA: ");
              readline(socket_id,escolha);
                if(escolha=="a" && resposta1=="c"){ writeline(socket_id, "Resposta correta!");  }
                   else if(escolha=="b" && resposta2=="c"){ writeline(socket_id, "Resposta certa");  }
                       else if(escolha=="c" && resposta3=="c"){ writeline(socket_id, "Resposta certa"); }
                           else if(escolha=="d" && resposta4=="c"){ writeline(socket_id, "Resposta certa");}


                    else
                        {
                        return mpp_start(socket_id,soma,ii);
                        }

        }
        if(decide=="p")
        {
            writeline(socket_id, "sera penalizado em 100€");
            int k=mpp_change(socket_id, num,ii,ponto);
            if(k==1)  return mpp_start(socket_id,soma,ii);

         }
        num++;
      }
   }


//-----------------------------------------------------------------------------------------------------------//
//					          Funções REGISTER
//-----------------------------------------------------------------------------------------------------------//
void mpp_register(int socket_id, string line)
{
    vector<string> parts = split(line, ' ');

    if (parts.size() != 3)
    {
        writeline(socket_id, "Parametros errados para poder registar.");
        return;
    }

    string comando = parts[0];
    string username = parts[1];
    string password = parts[2];

    if (mpp_is_logged(socket_id) == 1)
    {
        writeline(socket_id, "Nao pode estar logado para se registar ");
        return;
    }

    cout << "USERNAME: " << username << endl;
    cout << "PASSWORD: " << password << endl;

    //cout << "INSERT INTO utilizador VALUES(DEFAULT, '" << username << "', '" << password << "', 1)";

    PGresult* res = executeSQL("SELECT * FROM utilizador WHERE utilizador.username = '" + username + "'");
    if (PQntuples(res) == 1)
    {
        writeline(socket_id, "Ja existe um utilizador com o username especificado. Por favor escolha outro username.");
        return;
    }
    else
    {
        executeSQL("INSERT INTO utilizador (username, password) VALUES('" + username + "', '" + password + "')");
        writeline(socket_id, "Registo bem sucedido.");
    }
}
//-----------------------------------------------------------------------------------------------------------//
//					          Funções login
//-----------------------------------------------------------------------------------------------------------//

void mpp_login(int socket_id, string line)
{
     vector<string> parts = split(line, ' ');

     if(mpp_is_logged(socket_id)==1){
        writeline(socket_id, "Esta Online");
        return;
}
         string comando = parts[0];
         string username = parts[1];
         string password = parts[2];

      //   istringstream iss(line);
        // iss >> comando >> username >> password;


         std::string playername=username;
    PGresult* res = executeSQL("SELECT * FROM utilizador WHERE utilizador.username = '" + username + "'AND utilizador.password =  '"+ password + "'");
        if (PQntuples(res) != 1)
        {
        writeline(socket_id, "Username incorreto ou Palavra passe incorreta");
        return ;
        }

        for (std::map<string,int>::iterator it=sockets.begin(); it!=sockets.end(); ++it){
           std::cout << it->first << " => " << it->second << '\n';
           return;
        }


    sockets[username] = socket_id;
    usernames[socket_id] = username;
    writeline(socket_id, "login bem sucedido.");
    executeSQL("UPDATE utilizador SET status = 'true' WHERE utilizador.username = '" + username + "'");
 return ;
}

//-----------------------------------------------------------------------------------------------------------//
//					          Funções set admin
//-----------------------------------------------------------------------------------------------------------//
void mpp_setadmin(string line, int socket_id)
{

    vector<string> parts = split(line, ' ');
    string admin_pass;

      if(!mpp_is_logged(socket_id)){
        writeline(socket_id, "Tem de estar logado");
        return;
    }
    string comando = parts[0];
    string username = parts[1];
    string password = parts[2];

          writeline(socket_id,"Insira a password de administrador: ");
          readline(socket_id,admin_pass);

                 if( admin_pass=="admin")
                   {
                     PGresult* res = executeSQL("UPDATE utilizador SET admin = TRUE WHERE utilizador.username ='" + username + "'");
                     writeline(socket_id, "Privilegios alterados");
                     return;

                  }
                       writeline(socket_id,"Erro! utilizador não detem esse privilegio!\n\n");

            return;
    }

//FUNÇOES SUPORTE AO USUARIO
//-----------------------------------------------------------------------------------------------------------//
//					          Funções logout
//-----------------------------------------------------------------------------------------------------------//
void mpp_logout(string line, int socket_id)
{
     vector<string> parts = split(line, ' ');

     string comando = parts[0];
     string username = parts[1];
     string password = parts[2];


 if(!mpp_is_logged(socket_id)){
        writeline(socket_id, "Tem de estar logado");
        return;
    }

    executeSQL("UPDATE utilizador SET status = 'false' WHERE utilizador.username = '" + username + "'");
    executeSQL("UPDATE utilizador SET admin = 'false' WHERE utilizador.username = '" + username + "'");

    writeline(socket_id, "Logout efetuado com sucesso.");

    sockets.erase(username);
    usernames.erase(socket_id);

}

//-----------------------------------------------------------------------------------------------------------//
//					            Funções help
//-----------------------------------------------------------------------------------------------------------//

void mpp_help(int socket_id)      {

    writeline(socket_id,"\\mpp_register <username> <password> - Verifica se o utilizador existe no servidor. Caso isso não se verifique, cria-o na base de dados e associa a password ao mesmo.");
    writeline(socket_id,"_________________________________________________________________________");
    writeline(socket_id,"\\mpp_identify <username> - Verifica se o utilizador existe na base de dados e se já está ligado. Se ainda não existir, o username passa a estar associado a este utilizador. Serve para as pessoas se poderem ligar sem terem de criar uma conta.");
    writeline(socket_id,"_________________________________________________________________________");
    writeline(socket_id,"\\shutdown - Comando para desligar o servidor.");
    writeline(socket_id,"_________________________________________________________________________");
    writeline(socket_id, "\\mpp_kick <username> - Comando para o administrador do servidor eliminar utilizadores.");
    writeline(socket_id,"_________________________________________________________________________");
    writeline(socket_id, "\\mpp_help <comando> - Especifica a função de um comando de ajuda específico.");
    writeline(socket_id,"_________________________________________________________________________");
    writeline(socket_id, "\\mpp_create - Permite adição de novas questões na base de dados.");
    writeline(socket_id,"_________________________________________________________________________");
    writeline(socket_id, "\\mpp_login <username> <password> - Verifica a existência do utilizador na base de dados, em caso afirmativo guarda todas as informações relativas às atividades do utilizador.");
    writeline(socket_id,"_________________________________________________________________________");
    writeline(socket_id, "\\mpp_logout <username> - Desliga o utilizador do servidor.");
    writeline(socket_id,"_________________________________________________________________________");
    writeline(socket_id, "\\mpp_jogar - Comando para dar início à partida.");
    writeline(socket_id,"_________________________________________________________________________");
    writeline(socket_id, "\\mpp_challenge- Função que permite ao utilizador desafiar outro utilizador.");
    writeline(socket_id,"_________________________________________________________________________");
    writeline(socket_id, "\\top10 - Organiza as pontuações do utilizador e retorna as 10 melhores pontuações.");
    writeline(socket_id,"_________________________________________________________________________");
    writeline(socket_id, "\\fifty_fifty - Função que permite suporte à resposta do utilizador reduzindo as opções de resposta para metade.");
}


//-----------------------------------------------------------------------------------------------------------//
//					          Funções shutdown
//-----------------------------------------------------------------------------------------------------------//
void mpp_shutdown (int socket_id, string line) {
    if(!mpp_is_logged(socket_id)){
        writeline(socket_id, "Tem de estar login");
        return;
    }
    string comando;
    istringstream iss(line);
    iss >> comando;

    string id_lg_user = intToString(socket_id);

    PGresult* res = executeSQL("SELECT nome FROM utilizador, tipo_utilizador WHERE utilizador.id = '" + id_lg_user + "' AND utilizador.user_type = tipo_utilizador.id");

    string user_type(PQgetvalue(res, 0, 0));

    if(PQntuples(res) == 1 && user_type == ADMIN){
        broadcast(socket_id, "O servidor ira fechar em 30 segundos.\n");
        writeline(socket_id, "O servidor ira fechar em 30 segundos.\n");

        sleep(30);

        executeSQL("UPDATE utilizador SET online = 'false'");

        closeDB();
        close(socket_id);
        exit (0);
        }
    else {
        writeline(socket_id, "Nao tem permissoes para este comando.\n");
        return;
        }
}

//FUNÇOES DE INTERAÇÃO COM O JOGO E MULTI - JOGADO

//-----------------------------------------------------------------------------------------------------------//
//					          Funções top10
//-----------------------------------------------------------------------------------------------------------//

void mpp_top10(int socket_id){
    writeline(socket_id,"\t\t****************************************");
    writeline(socket_id,"\t\t________________________________________");
    writeline(socket_id,"\t\t                **TOP 10**              ");
    writeline(socket_id,"\t\t________________________________________");
    writeline(socket_id,"\t\t****************************************");
    PGresult * res;
        res = executeSQL("SELECT username, score FROM utilizador ORDER BY score DESC");
                for(int i = 0; i < 10; i++)
                writeline(socket_id, "\t\t\t" + intToString(i+1) + "º " + string(PQgetvalue(res,i,0)) + " --> " + string(PQgetvalue(res,i,1)) + "€" + "\n");
    writeline(socket_id,"\t\t________________________________________\n\n");

    return ;
}
//-----------------------------------------------------------------------------------------------------------//
//					          Funções 50-50
//-----------------------------------------------------------------------------------------------------------//

void mpp_fifty_fifty(string pergunta){
 int aux, cont;
 string resposta1;
   string resposta2;
      string resposta3;
         string resposta4;

   string alinea1;
       string alinea2;
           string alinea3;
               string alinea4;
                   string perg;
    vector<string> items2;

    vector<string> items = split(pergunta,'|');


    for (aux=0 ; aux<5 ; aux++) {
        vector<string> items2 = split(items[aux], '_');
        if (aux ==0) perg = items2[1];
        if (aux ==1){ alinea1 = items2[1]; resposta1 = items2[0]; }
        if (aux ==2){ alinea2 = items2[1]; resposta2 = items2[0]; }
        if (aux ==3){ alinea3 = items2[1]; resposta3 = items2[0]; }
        if (aux ==4){ alinea4 = items2[1]; resposta4 = items2[0]; }
     return ;
    }
    cont=0;
    while(cont<2){
    if(resposta1=="e") { cont++; alinea1 = " ";}
       if(resposta2=="e") { cont++; alinea2 = " "; }
          if(resposta3=="e") { cont++; alinea3 = " ";  }
             if(resposta4=="e") { cont++; alinea4 = " ";  }

    }
}

//-----------------------------------------------------------------------------------------------------------//
//					          Funções challenge
//-----------------------------------------------------------------------------------------------------------//

void mpp_challenge(int socket_id, string line)
{
   string estado;
   string username_d;
   string username_a;
   int plays;

   plays=1;
        vector<string> parts = split(line, ' ');
        string comando = parts[0];
        string username1 = parts[1];
        string username2 = parts[1];
        //stringstream ss;
        //ss<<username1<<"CHALLENGE:"<<username2;

         PGresult* res = executeSQL("SELECT username FROM utilizador WHERE utilizador.username = '" + username2 +"'");

          string id_user1 = username1;
          string id_user2(PQgetvalue(res,0,0));
          int socket_id2 = sockets[username2];

          res = executeSQL("SELECT * FROM jogada WHERE jogada.user_d = '" + username1 + "' AND user_f = '" + username2 + "' AND estado = 'pendente'");
          PGresult* query = executeSQL("SELECT * FROM jogada WHERE jogada.user_d = '" + username1 + "' AND user_f = '" + username2 + "' AND estado = 'decorrer'");
          if(PQntuples(query) == 1){
              writeline(socket_id, "Está a ocorrer um jogo.");
              return;
          }
          if(PQntuples(res) == 1)
          {
              writeline(socket_id, "Nao pode desafiar enquanto nao terminar o jogo em curso.");
              return;

       // executeSQL("INSERT INTO jogada VALUES(DEFAULT, '" + username1 + "', '" + username2 + "', 'pendente', '" + num_jogadas + "');");
          executeSQL("INSERT INTO jogada VALUES(DEFAULT, '" + username1 + "', '" + username2 + "', 'pendente');");
          writeline(socket_id2, "Voce foi desafiado para um jogo por " + usernames[socket_id] + " .");

      }else{
          writeline(socket_id, "O utilizador nao existe");
          return; //caso o desafiante não exista...

      writeline(socket_id, "Desafio realizado");
  }
 }

//FUNÇOES INTERNAS DO JOGO - CHECK FUNTIONS
//-----------------------------------------------------------------------------------------------------------//
//					          VERIFICA SE ESTA LOGADO
//-----------------------------------------------------------------------------------------------------------//
bool mpp_is_logged(int socket_id )
{
    map<int, string>::iterator it = usernames.find(socket_id);
    return (it != usernames.end());
}

//-----------------------------------------------------------------------------------------------------------//
//					          VERIFICA SE ESTA NA DB
//-----------------------------------------------------------------------------------------------------------//

int mpp_identify (int socket_id){

    string username;

    writeline(socket_id,"Introduza o nome do usuario a ser identificado!\n\n");
    readline(socket_id,username);

   stringstream t;
   t << "SELECT username FROM utilizador WHERE username ='" + username + "'";
   PGresult* res = executeSQL(t.str());

  if((PQresultStatus(res) == PGRES_TUPLES_OK) && (PQntuples(res) == 1))
    {
      writeline(socket_id,"Esse utilizador existe e esta Offline!\n\n");
      t << "SELECT status FROM utilizador WHERE username ='" + username + "'";

      PGresult* res = executeSQL(t.str());
             bool status(PQgetvalue(res, 0, 0));
             if (status == true)  writeline(socket_id,"Esse utilizador existe e esta online!\n\n");
            return 1;
      }else{
            writeline(socket_id,"Esse utilizador nao existe!\n\n");
            return 0;
            }

 }
//-----------------------------------------------------------------------------------------------------------//
//					          EXPULSAR
//-----------------------------------------------------------------------------------------------------------//
void mpp_kick (int socket_id, string line)
{
    string username;
    string admin_pass;
    string expulsa;

                    writeline(socket_id, "Para explusar usuario é preciso ter privilegios de administrador");
                    writeline(socket_id, "digite seu username");
                    readline(socket_id,username);
                    writeline(socket_id, "Insira a password de administrador:");
                    readline(socket_id,admin_pass);

                 if( admin_pass=="admin")
                   {
                     PGresult* res = executeSQL("UPDATE utilizador SET admin = TRUE WHERE utilizador.username ='" + username + "'");
                     writeline(socket_id, "Privilegios alterados");
                     writeline(socket_id, "Privilegios alterados");

                    writeline(socket_id,"Digite o nome do usuario que deseja expulsar");
                    readline(socket_id,expulsa);

                     executeSQL("UPDATE utilizador SET status = 'false' WHERE utilizador.username = '" + expulsa + "'");

                     sockets.erase(expulsa);
                     usernames.erase(sockets[expulsa]);
                     std::map<string,int>::iterator it ;

                     for (it=sockets.begin(); it!=sockets.end(); ++it)
                        cout << it->first << " => " << it->second << '\n';

                     writeline(socket_id, "usuario desligado com susseso!");

                    return;

                   }
                       writeline(socket_id,"Erro! utilizador não detem esse privilegio!\n\n");

                    return;

}

//-----------------------------------------------------------------------------------------------------------//
//					          CRIA QUESTÕES
//-----------------------------------------------------------------------------------------------------------//


void mpp_create(int socket_id)
{
string username;
string admin_pass;
string pergunta;


    if(!mpp_is_logged(socket_id)==1){
       writeline(socket_id, "Tem de estar Online");
      return;
      }

    writeline(socket_id, "Para criar quetão é preciso ter privilegios de administrador");
    writeline(socket_id, "digite seu username");
    readline(socket_id,username);
    writeline(socket_id, "Insira a password de administrador:");
    readline(socket_id,admin_pass);

 if( admin_pass=="admin")
   {

     writeline(socket_id, "Privilegios alterados");
     writeline(socket_id, "Privilegios alterados");


     writeline(socket_id, "c_questao|c_resposta1|e_resposta2|e_resposta3|e_resposta4.");
        writeline(socket_id, "Digite a quetão no fromato:");
        writeline(socket_id, "c-certo & e-errado e a posão fica a seu critério");

        readline(socket_id,pergunta);

        PGresult* res = executeSQL("SELECT COUNT(*) FROM pergunta");
        int i = atoi(PQgetvalue(res, 0, 0));
        i=i+1;

        stringstream ss;
        ss << "INSERT INTO pergunta (numero, perg) VALUES('"<<i<<"' , '" + pergunta + "');";
        PGresult* k = executeSQL(ss.str());

          if((PQresultStatus(res) == PGRES_TUPLES_OK) && (PQntuples(res) == 1)) writeline(socket_id, "QUESTAO ADICIONADA A BASE DE DADOS COM EXITO!");

     return;
    }


    return ;
}

//----------------------------------------------------------------------------------------------------------//
//					  					FUNÇOES DE CONEXÃO COM A BASE DE DADOS
//----------------------------------------------------------------------------------------------------------//

void initDB()
{
    conn = PQconnectdb("host='vdbm.fe.up.pt' user='sinf15g67' password='mateuspedropedro' dbname='sinf15g67'");

  if (!conn)  {
    cout << "Não foi possivel ligar a base de dados" << endl;
    exit(-1);
  }

  if (PQstatus(conn) != CONNECTION_OK)  {
    cout << "Não foi possivel ligar a base de dados" << endl;
    exit(-1);
  }
}


//UMA VEZ QUE A CONEXÃO COM O SERVIDOR FOI ESTABELECIDA COM SUCESSO, AS FUNÇOES AQUI DESCRITAS SERVEM PARA EXECUTAR COMANDOS SQL
PGresult *executeSQL(string sql)    	 	   //RECEBE COMANDO QUE DEFINE CONEXÃO
{
  PGresult* res = PQexec(conn, sql.c_str());  // EXECUTA COMANDO PARA DETERMINAR CONEXÃO COM SERVIDOR E ESPERA PELO RESULTADO

  //SE O RESULTADA FOR DIFERRENTE DE 'PGRES_COMMAND_OK' OU 'PGRES_TUPLES_OK' CONEXÃO NÃO ESTABELECIDA
    if (!(PQresultStatus(res) == PGRES_COMMAND_OK || PQresultStatus(res) == PGRES_TUPLES_OK)){
    cout << "Não foi possí­vel executar o comando!" << endl;
    cout << " >>> " << sql << endl;
    return NULL;
  }
   return res;
}
void closeDB()
{
  PQfinish(conn);
}
//----------------------------------------------------------------------------------------------------------//
//				 					MANIPULAÇÃO - sockets
//----------------------------------------------------------------------------------------------------------//

void writeline(int socket_id, string line) {
    string tosend = line + "\n";
    write(socket_id, tosend.c_str(), tosend.length());
}

/* LÊ string  DA MAP sockets  E RETORNA false SE ESTIVER VAZIA */

bool readline(int socket_id, string &line) {

  /* buffer de tamanho 1025 para ter espaço
     para o \0 que indica o fim de string*/
  char buffer[1025];

  /* inicializar a string */
  line = "";

  /* Enquanto não encontrarmos o fim de linha
     vamos lendo mais dados da stream */
  while (line.find('\n') == string::npos) {
    // leu n carateres. se for zero chegamos ao fim
    int n = read(socket_id, buffer, 1024); // ler do socket
    if (n == 0) return false; // nada para ser lido -> socket fechado
    buffer[n] = 0; // colocar o \0 no fim do buffer
    line += buffer; // acrescentar os dados lidos à string
  }

  // Retirar o \r\n (lemos uma linha mas não precisamos do \r\n)
  line.erase(line.end() - 1);
  line.erase(line.end() - 1);
  return true;
}

/* Envia uma mensagem para todos os clientes ligados exceto 1 */
void broadcast (int origin, string text) {
   /* Usamos um ostringstream para construir uma string
      Funciona como um cout mas em vez de imprimir no ecrã
      imprime numa string */
   ostringstream message;
   message << origin << " said: " << text;

   // Iterador para sets de inteiros
   set<int>::iterator it;
   for (it = clients.begin(); it != clients.end(); it++)
     if (*it != origin) writeline(*it, message.str());
}



//* ESCREVE CHAVE int  DA MAP clients E DETERMINA A FUNÇÃO SELECIONADA
void* cliente(void* args)
{
  int socket_id = *(int*)args; //recebe o argumento
  string line;
  clients.insert(socket_id); // insere valor recebido

  cout << "Client connected: " << socket_id << endl;
    mpp_start(socket_id,SOMA,II);



  while (readline(socket_id, line)) {

    //mpp is a prefix to distinguish from C++ protected keywords
    if (line.size()==0)     		     			writeline(socket_id, "Comando inexistente!");
    else if (line.find("\\") == string::npos)		writeline(socket_id, "Nao foi possivel identificar o comando.");
    else if (line.find("\\mpp_help") ==0 )       	mpp_help(socket_id);
    else if (line.find("\\mpp_register")==0)  	    mpp_register( socket_id,  line);
    else if (line.find("\\mpp_is_logged")==0)       mpp_is_logged(socket_id);
    else if (line.find("\\mpp_identify")==0)        mpp_identify (socket_id);
    else if (line.find("\\mpp_setadmin")==0)        mpp_setadmin( line,  socket_id);
    else if (line.find("\\mpp_kick")==0)            mpp_kick(socket_id, line);
    else if (line.find("\\mpp_login")==0)           mpp_login(socket_id,  line);
    else if (line.find("\\mpp_challenge")==0)       mpp_challenge(socket_id, line);
    else if (line.find("\\mpp_logout")==0)          mpp_logout( line,  socket_id);
    else if (line.find("\\mpp_shutdown")==0)        mpp_shutdown (socket_id, line) ;
    else if (line.find("\\mpp_create")==0)    	    mpp_create (socket_id);
    else if (line.find("\\mpp_start")==0)    	    mpp_start (socket_id,SOMA,II);
    else if (line.find("\\mpp_top10")==0)    	    mpp_top10 (socket_id);
    else if (line.find("\\mpp_jogar")==0)           mpp_jogar(socket_id,  line);

  }
  cout << "Socket " << socket_id << " said: " << line << endl;
  broadcast(socket_id, line);
  cout << "Client disconnected: " << socket_id << endl;
  clients.erase(socket_id);

  // Fechar o socket
  close(socket_id);

  return 0;
}
