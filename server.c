#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>

#define MAX 1024
#define PROTOCOL 0
#define PORT 9002

typedef struct Movie {
	int id;
	char title[MAX];
	char genre[MAX];
	char director[MAX];
	int year;
    char cover_url[MAX];
    int active;
} Movie;

Movie filmes[MAX];
int filmes_count;

void bzero_movie(Movie movie) {
    bzero(movie.title, MAX);
    bzero(movie.genre, MAX);
    bzero(movie.director, MAX);
    bzero(movie.cover_url, MAX);
}

void print_database() {
    int i;
    for (i = 0; i < filmes_count; i++) {
        printf("%d\n", filmes[i].id);
        printf("%s", filmes[i].title);
        printf("%s", filmes[i].genre);
        printf("%s", filmes[i].director);
        printf("%d\n", filmes[i].year);
        printf("%s", filmes[i].cover_url);
        printf("%d\n", filmes[i].active);
    }
}

void read_filmes_from_database() {
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int n;

    Movie tmp;

    fp = fopen("database.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    getline(&line, &len, fp);
    filmes_count = atoi(line);

    for (n = 0; n < filmes_count; n++) {
        bzero_movie(tmp);
        getline(&line, &len, fp);
        tmp.id = atoi(line);
        getline(&line, &len, fp);
        strlcpy(tmp.title, line, MAX);
        getline(&line, &len, fp);
        strlcpy(tmp.genre, line, MAX);
        getline(&line, &len, fp);
        strlcpy(tmp.director, line, MAX);
        getline(&line, &len, fp);
        tmp.year = atoi(line);
        getline(&line, &len, fp);
        strlcpy(tmp.cover_url, line, MAX);
        getline(&line, &len, fp);
        tmp.active = atoi(line);
        filmes[n] = tmp;
    }

    fclose(fp);
    if (line)
        free(line);

    return;
}

void write_filmes_to_database() {
    FILE *fp;
    int n;

    print_database();

    fp = fopen("database.txt", "w");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    fprintf(fp, "%d\n", filmes_count);
    for (n = 0; n < filmes_count; n++) {
        fprintf(fp, "%d\n%s%s%s%d\n%s%d\n", filmes[n].id, filmes[n].title, filmes[n].genre, filmes[n].director, filmes[n].year, filmes[n].cover_url, filmes[n].active);
    }

    fclose(fp);
}

void remove_filme(int client_socket) {
    char buff[MAX], prompt_message[MAX] = "Qual o ID do filme a ser removido?\n";
    int id;

    write(client_socket, prompt_message, sizeof(prompt_message));
    
    bzero(buff, MAX);
    read(client_socket, buff, sizeof(buff));
    id = atoi(buff);

    if (id < filmes_count) {
        filmes[id].active = 0;
        bzero(prompt_message, MAX);
        snprintf(prompt_message, sizeof(prompt_message), "O filme \n%sFoi removido com sucesso\nAperte <Enter> para continuar...\n", filmes[id].title);
        write(client_socket, prompt_message, sizeof(prompt_message));
        read(client_socket, buff, sizeof(buff));
    }

    write_filmes_to_database();
}

void list_filme_by_id(int client_socket) {
    char filme_str[MAX], buff[MAX], prompt_message[MAX] = "Qual o ID do filme a ser exibido?\n";
    int id, n;

    write(client_socket, prompt_message, sizeof(prompt_message));
    bzero(buff, MAX);
    read(client_socket, buff, sizeof(buff));
    id = atoi(buff);

    for (n = 0; n < filmes_count; n++) {
        if (filmes[n].active > 0 && filmes[n].id == id) {
            bzero(filme_str, MAX);
            snprintf(filme_str, sizeof(filme_str), "ID: %d\nTítulo: %sGênero: %sDiretor: %sAno de Lançamento: %d\n\nAperte <Enter> para continuar...\n", filmes[n].id, filmes[n].title, filmes[n].genre, filmes[n].director, filmes[n].year);
            write(client_socket, filme_str, sizeof(filme_str));
            read(client_socket, buff, sizeof(buff));
        }
    }
}

void list_filme_by_genre(int client_socket) {
    char filme_str[MAX], genre[MAX], buff[MAX], prompt_message[MAX] = "Qual o gênero do filme a ser exibido?\n";
    int n;

    write(client_socket, prompt_message, sizeof(prompt_message));
    bzero(buff, MAX);
    read(client_socket, buff, sizeof(buff));
    strcpy(genre, buff);

    for (n = 0; n < filmes_count; n++) {
        if (filmes[n].active > 0 && strcasestr(filmes[n].genre, genre) != NULL) {
            bzero(filme_str, MAX);
            snprintf(filme_str, sizeof(filme_str), "ID: %d\nTítulo: %sGênero: %sDiretor: %sAno de Lançamento: %d\n\nAperte <Enter> para continuar...\n", filmes[n].id, filmes[n].title, filmes[n].genre, filmes[n].director, filmes[n].year);
            write(client_socket, filme_str, sizeof(filme_str));
            read(client_socket, buff, sizeof(buff));
        }
    }
}

void list_titles(int client_socket) {
    int n;
    char filme_str[MAX], buff[MAX];

    for (n = 0; n < filmes_count; n++) {
        if (filmes[n].active > 0) {
            bzero(filme_str, MAX);
            snprintf(filme_str, sizeof(filme_str), "ID: %d\nTítulo: %s\nAperte <Enter> para continuar...\n", filmes[n].id, filmes[n].title);
            write(client_socket, filme_str, sizeof(filme_str));
            read(client_socket, buff, sizeof(buff));
        }
    }
}

void list_filmes(int client_socket) {
    int n;
    char filme_str[MAX], buff[MAX];

    for (n = 0; n < filmes_count; n++) {
        if (filmes[n].active > 0) {
            bzero(filme_str, MAX);
            snprintf(filme_str, sizeof(filme_str), "ID: %d\nTítulo: %sGênero: %sDiretor: %sAno de Lançamento: %d\n\nAperte <Enter> para continuar...\n", filmes[n].id, filmes[n].title, filmes[n].genre, filmes[n].director, filmes[n].year);
            write(client_socket, filme_str, sizeof(filme_str));
            read(client_socket, buff, sizeof(buff));
        }
    }
}

void add_genre(int client_socket) {
    char filme_str[MAX], buff[MAX], prompt_message[MAX] = "Qual o ID do filme a ser modificado?\n";
    int id, n;

    write(client_socket, prompt_message, sizeof(prompt_message));
    bzero(buff, MAX);
    read(client_socket, buff, sizeof(buff));
    id = atoi(buff);

    bzero(prompt_message, MAX);
    strcpy(prompt_message,"Qual o gênero a ser adicionado?\n");

    write(client_socket, prompt_message, sizeof(prompt_message));
    bzero(buff, MAX);
    read(client_socket, buff, sizeof(buff));

    filmes[id].genre[strlen(filmes[id].genre)-1] = '\0';
    strcat(filmes[id].genre, ", ");
    strcat(strcat(filmes[id].genre, "\0"), buff);

    write_filmes_to_database();
}

void add_filme(client_socket) {
    char filme_str[MAX], buff[MAX], prompt_message[MAX] = "Qual o título do filme a ser adicionado?\n";
    int id, n;
    Movie filme;

    filme.id = filmes_count;
    filme.active = 1;
    strcpy(filme.cover_url, "https://picsum.photos/100\n");

    write(client_socket, prompt_message, sizeof(prompt_message));
    bzero(buff, MAX);
    read(client_socket, buff, sizeof(buff));
    strcpy(filme.title, buff);

    bzero(prompt_message, MAX);
    strcpy(prompt_message,"Qual(is) o(s) gênero(s) do filme? (separados por \", \")\n");

    write(client_socket, prompt_message, sizeof(prompt_message));
    bzero(buff, MAX);
    read(client_socket, buff, sizeof(buff));
    strcpy(filme.genre, buff);

    bzero(prompt_message, MAX);
    strcpy(prompt_message,"Qual o diretor do filme?\n");

    write(client_socket, prompt_message, sizeof(prompt_message));
    bzero(buff, MAX);
    read(client_socket, buff, sizeof(buff));
    strcpy(filme.director, buff);

    bzero(prompt_message, MAX);
    strcpy(prompt_message,"Qual o ano de lançamento do filme?\n");

    write(client_socket, prompt_message, sizeof(prompt_message));
    bzero(buff, MAX);
    read(client_socket, buff, sizeof(buff));
    filme.year = atoi(buff);

    filmes[filmes_count] = filme;

    filmes_count++;

    write_filmes_to_database();
}

void application_filmes(int client_socket) {

    read_filmes_from_database();

    write_filmes_to_database();

    char buff[MAX], prompt_message[MAX] = "";;
    int n;

    // infinite loop for chat
    for (;;) {
        bzero(prompt_message, MAX);
        strcat(prompt_message, "Bem vindo ao Projeto 1 de MC833 1S2022\n");
        strcat(prompt_message, "Selecione uma opção abaixo pelo número correspondente, ou digite \'sair\' para finalizar a aplicação\n\n");
        strcat(prompt_message, "1 - Cadastrar um novo filme\n");
        strcat(prompt_message, "2 - Acrescentar um gênero a um filme\n");
        strcat(prompt_message, "3 - Listar todos os títulos com seus IDs\n");
        strcat(prompt_message, "4 - Listar filmes de um gênero apenas\n");
        strcat(prompt_message, "5 - Listar todos os filmes\n");
        strcat(prompt_message, "6 - Listar filme a partir do ID\n");
        strcat(prompt_message, "7 - Remover filme a partir do ID\n\n");
        write(client_socket, prompt_message, sizeof(prompt_message));

        bzero(buff, MAX);
        read(client_socket, buff, sizeof(buff));
        printf("From client: %s", buff);

        if (strcmp(buff,"1\n") == 0) {
			printf("\nCadastrando filme...");
            add_filme(client_socket);
            printf("\nCadastro finalizado\n");
            
		} else if (strcmp(buff,"2\n") == 0) {
			printf("\nAdicionando genero...\n");
            add_genre(client_socket);

		} else if (strcmp(buff,"3\n") == 0) {
			printf("\nListando títulos...\n");
            list_titles(client_socket);

		} else if (strcmp(buff,"4\n") == 0) {
			printf("Listando títulos por genero");
            list_filme_by_genre(client_socket);

		} else if (strcmp(buff,"5\n") == 0) {
			printf("\nListando todos filmes...\n");
            list_filmes(client_socket);

		} else if (strcmp(buff,"6\n") == 0) {
			printf("\nMostrando informações do filme...\n");
            list_filme_by_id(client_socket);

		} else if (strcmp(buff,"7\n") == 0) {
			printf("\nRemovendo filme...\n");
            remove_filme(client_socket);

		} else if (strncmp("sair", buff, 4) == 0) {
            printf("Encerrando aplicação...\n");
            break;
        }
        
    }

}

int main() {

    int server_socket;
    char server_message[MAX] = "Servidor encontrado.\nIniciando aplicação \"filmes\"\n\n";

    server_socket = socket(AF_INET, SOCK_STREAM, PROTOCOL);

    if (server_socket == -1) {
        printf("socket creation failed...\n");
        return(0);
    }
    else {
        printf("Socket successfully created..\n");
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address)) != 0) {
        printf("socket binding failed...\n");
        return(0);
    }
    else {
        printf("socket successfully binded..\n");
    }

    if (listen(server_socket, 5) != 0) {
        printf("listen failed...\n");
        return(0);
    }
    else {
        printf("server listening..\n");
    }

    int client_socket;
    client_socket = accept(server_socket, NULL, NULL);

    if (client_socket < 0) {
        printf("server accept failed...\n");
        return 0;
    }
    else {
        printf("server accepted the client...\n");
    }

    send(client_socket, server_message, sizeof(server_message), 0);
    
    application_filmes(client_socket);

    close(server_socket);

    return 0;
}
