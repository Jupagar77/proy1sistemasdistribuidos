#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netdb.h>
#include <pthread.h>
#include <termios.h>


int _fCloseThreads, _conexionServidor;

int getch (void)
{
    int ch;
    struct termios oldt, newt;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON|ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
}

void* conectarCliente(){
    struct sockaddr_in cliente;
    socklen_t longc; 
    int conexion_cliente;
    char buffer[100];
    longc = sizeof(cliente);

    while(_fCloseThreads){

        printf("Estatus: %d\n", _fCloseThreads);

        while(conexion_cliente = accept(_conexionServidor, (struct sockaddr *)&cliente, &longc);){ //Espere conexiones de clients

        }
        
        // Error
        if(conexion_cliente<0)
        {
          printf("Error al aceptar trafico\n");
          close(_conexionServidor);
          return NULL;
        }

        // Conexion encontrada
        printf("Conectando con %s:%d\n", inet_ntoa(cliente.sin_addr),htons(cliente.sin_port));
        if(recv(conexion_cliente, buffer, 100, 0) < 0)
        { 
          printf("Error al recibir los datos\n");
          close(_conexionServidor);
          return NULL;
        } else {
          printf("%s\n", buffer);
          bzero((char *)&buffer, sizeof(buffer));
          send(conexion_cliente, "Recibido\n", 13, 0);
        }

    }
}

void* finishProgram()
{
    int ch;
    do{
        printf("%s", (char*)"Exit? \n");
        ch = getch();
        if(ch=='E'){
            printf("%s", (char*)"Yes \n");
            _fCloseThreads = 0;
            return 0;
        }
        printf("%s", (char*)"No \n");
    }while(1);
}


int main(int argc, char **argv){

  //https://es.wikibooks.org/wiki/Programaci%C3%B3n_en_C/Sockets
  //https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/
    
  // Validar argumentos
  if(argc<2)
  {
    printf("%s <puerto>\n",argv[0]);
    return 1;
  }

  // Declarar variables
  int puerto; 
  socklen_t longc; 
  struct sockaddr_in servidor, cliente;
  char buffer[100]; 
  
  // Inizializar variables
  puerto = atoi(argv[1]);
  _conexionServidor = socket(AF_INET, SOCK_STREAM, 0);
  bzero((char *)&servidor, sizeof(servidor)); 
  servidor.sin_family = AF_INET;
  servidor.sin_port = htons(puerto);
  servidor.sin_addr.s_addr = INADDR_ANY; 

  // Conectar puerto con servidor
  if(bind(_conexionServidor, (struct sockaddr *)&servidor, sizeof(servidor)) < 0)
  { 
    printf("Error al asociar el puerto a la conexion\n");
    close(_conexionServidor);
    return 1;
  }

  // Empiece a escuchar
  listen(_conexionServidor, 3); 
  printf("A la escucha en el puerto %d\n", ntohs(servidor.sin_port));
  
  pthread_t thread_arr[2];
  int iter_id[2];
  _fCloseThreads = 1;

  for (int iter=0; iter<2; iter++) {
    iter_id[iter] = iter;
    if(iter == 0){
        pthread_create(&thread_arr[iter], NULL, conectarCliente, &iter_id[iter]);
    } else{
        pthread_create(&thread_arr[iter], NULL, finishProgram, &iter_id[iter]);
    }
  }

  for (int iter=0;iter<2;iter++) {
    pthread_join(thread_arr[iter],NULL);
  }
  return 0;


  close(_conexionServidor);

  return 0;
}