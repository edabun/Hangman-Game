#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>

const char *word,*cat;
char guess[100];
char letter;
int wrong=0;
char w[100];
int server_sock, client_sock, port_no, client_size, n;
struct sockaddr_in server_addr, client_addr;

const char *wordBank(){
	const char *words[]={	"Phelan",
				"Rules of Survival",
				"Object Oriented Programming",
				"Janella Ooi",
				"Madrigal",
				"Storm Spirit",
				"John Sixto Santos",
				"Ateneo de Naga University",
				"Luc Mbah a Moute",
				"Farfetch'd"};
	const char *category[]={	
				"Building",
				"Video Game",
				"IT Subject",
				"Internet Celebrity",
				"Filipino Name",
				"DOTA Hero",
				"DCS Faculty",
				"School",
				"Basketball Player",
				"Pokemon"};
	int n = sizeof(words)/sizeof(const char *);	
	srand(time(0));
	int r=rand()%n;
	
	word=words[r];
	cat=category[r];
}

void hangman(int num){
	switch(num){
	case 5:
			printf("\n\t||===== ");
			printf("\n\t||    | ");
			printf("\n\t||   %cO/",'\\');
			printf("\n\t||    | ");
			printf("\n\t||   / %c",'\\');
			printf("\n\t||      ");
			break;
    case 4:
			printf("\n\t||===== ");
			printf("\n\t||    | ");
			printf("\n\t||   %cO/",'\\');
			printf("\n\t||    | ");
			printf("\n\t||     %c",'\\');
			printf("\n\t||      ");
			break;
    case 3:
			printf("\n\t||===== ");
			printf("\n\t||    | ");
			printf("\n\t||   %cO/",'\\');
			printf("\n\t||    | ");
			printf("\n\t||      ");
			printf("\n\t||      ");
			break;
	case 2:
			printf("\n\t||===== ");
			printf("\n\t||    | ");
			printf("\n\t||   %cO/",'\\');
			printf("\n\t||      ");
			printf("\n\t||      ");
			printf("\n\t||      ");
			break;
    case 1:
			printf("\n\t||===== ");
			printf("\n\t||    | ");
			printf("\n\t||   %cO ",'\\');
			printf("\n\t||      ");
			printf("\n\t||      ");
			printf("\n\t||      ");
			break;
    case 0:
			printf("\n\t||===== ");
			printf("\n\t||    | ");
			printf("\n\t||    O ");
			printf("\n\t||      ");
			printf("\n\t||      ");
			printf("\n\t||      ");
			break;
		      }//end of switch-case
    return;
}

void setWord(){
	printf("CATEGORY: %s\n",cat);
	for(int i=0;i<strlen(w);i++){
		if(w[i]==' '){
			guess[i]=' ';
			printf("%c",guess[i]);
		}
		else
			if(!isalpha(w[i])){
				guess[i]=w[i];
				printf("%c",guess[i]);
			}else{
				guess[i]='_';
				printf("%c",guess[i]);
				}
	}//end of for
	printf("\n");
}

void printWord(){
	printf("\nCATEGORY: %s\n",cat);
	for(int i=0;i<strlen(word);i++){
		printf("%c",guess[i]);
	}
	printf("\n");
}

int checkWinner(){
	if(strcmp(guess,word) && wrong!=5){
		//printf("You Won!\nThe word is %s.\n",word);
		return 1;
	}else
		if(!strcmp(guess,word) && wrong==5){
			return 1;
		}
}

int takeTurn(char l){

	for(int i=0;i<strlen(word);i++){
		if(tolower(l)==tolower(word[i])){
			guess[i]=word[i];
		}else{
			if(!strchr(word,tolower(l))){
				system("clear");
				wrong++;
				printf("Tries left: %d",5-wrong);
				hangman(wrong);
				printWord();
				break;
			}
		}
	}
	return 0;
}

void die_with_error(char *error_msg){
    printf("%s", error_msg);
    exit(-1);
}

int main(int argc,  char *argv[]){
    int client_sock,  port_no,  n;
    struct sockaddr_in server_addr;
    struct hostent *server;

    if (argc < 3) {
        printf("Usage: %s hostname port_no",  argv[0]);
        exit(1);
    }

    //printf("Client starting ...\n");
    // Create a socket using TCP
    client_sock = socket(AF_INET,  SOCK_STREAM,  0);
    if (client_sock < 0)
        die_with_error("Error: socket() Failed.");

    //printf("Looking for host '%s'...\n", argv[1]);
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        die_with_error("Error: No such host.");
    }
    //printf("Host found!\n");

    // Establish a connection to server
    port_no = atoi(argv[2]);
    bzero((char *) &server_addr,  sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&server_addr.sin_addr.s_addr,
         server->h_length);

    server_addr.sin_port = htons(port_no);

    //printf("Connecting to server at port %d...\n", port_no);
    if (connect(client_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
        die_with_error("Error: connect() Failed.");

      // Communicate
	char letter[256];
	int winner=1;

	system("clear");
	wordBank();

	n=recv(client_sock, w, 255, 0);
	if (n < 0) die_with_error("Error: recv() Failed.");


	setWord();
	printf("Tries left: %d\n",5-wrong);
	hangman(wrong);

	while(1){
			//recv
			bzero(letter, 256);
	        puts("\nYour teammate's turn.\n");
	        n = recv(client_sock, letter, 255, 0);
	       	takeTurn(letter[0]);
	       	system("clear");
	       	printWord();
	       	printf("\nTries left: %d",5-wrong);
	       	hangman(wrong);
	        if (n < 0) die_with_error("Error: recv() Failed.");

        if(winner!=checkWinner()){
				if(wrong==5){
					system("clear");
					hangman(wrong);
					printf("\nYou Lost!\nThe word is %s.\n",word);
					break;
				}else{
					system("clear");
					hangman(wrong);
					printf("\nYou Won!\nThe word is %s.\n",word);
					break;
				}
				return 0;
			}

		//send
		printf("\nPlayer 2 Enter a letter: ");

		while(1){
			bzero(letter,256);
			fgets(letter,255,stdin);

			if(!isalpha(tolower(letter[0]))){
				system("clear");
				wrong++;
				printf("Tries left: %d",5-wrong);
				hangman(wrong);
				printWord();	
				
				
			}else{
				system("clear");
				takeTurn(letter[0]);

				//system("clear");
				//printWord();
				//hangman(wrong);
			}

			n=send(client_sock,letter,strlen(letter),0);
			if(n<0) die_with_error("Error: send () Failed");
			system("clear");
			printWord();
			printf("\nTries left: %d",5-wrong);
			hangman(wrong);
			break;

			if(winner!=checkWinner()){
				if(wrong==5){
					system("clear");
					hangman(wrong);
					printf("\nYou Lost!\nThe word is %s.\n",word);
					break;
				}else{
					system("clear");
					hangman(wrong);
					printf("\nYou Won!\nThe word is %s.\n",word);
					break;
				}
				return 0;
			}
			}
		}

	close(client_sock);

    return 0;
}