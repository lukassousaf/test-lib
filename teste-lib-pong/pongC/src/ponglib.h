#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Estruturas que serão usadas no jogo
typedef struct
{
  // posição dos jogadores
  int x, y, score, win, lost, foundPlayer;
  char name[100];
} Player;

typedef struct
{
  // posição da bola 
  int x, y;
  // atributo de velocidade horizontal e vertical
  int velx, vely;
} Ball;

typedef struct
{
  int texW;
  int texH;
  TTF_Font * font;
  SDL_Color color;
} Texto;

typedef struct 
{
  char playerName[100];
  int v, l; 
} rank;

// Determina nome valido
int validName(Player * player);


// Registro do nome dos jogadores
int validRegister(Player * playerA, Player * playerB);


// Determina qtd de ranks cadastrados
int rankSize(char * path);

// Exibi os ranks 
void showRanking(char * path);

// Processa os ranks na memoria
void processRanking(char * path, rank * rank, Player * playerA, Player * playerB, int numberRank);

// Escreve os ranks num arquivo txt
void writeRanking(char * path, rank * rank, Player * playerA, Player * playerB, int numberRank);

// Processa os eventos do jogo 
int processEvents(SDL_Window *window, Player *playerA, Player *playerB, Ball * ball, Mix_Chunk * hitFx, int * stage);

// Renderiza o menu
void doRenderMenu(SDL_Renderer *renderer, Texto * textoVic);

// Rederiza o jogo
void doRenderGame(SDL_Renderer *renderer, Player * playerA, Player * playerB, Ball * ball, Texto * textoVic, int * stage);

