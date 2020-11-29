#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "ponglib.h"



// Determina nome valido
int validName(Player * player)
{
  for(int i = 0; i < strlen(player->name); i++)
  {
    if(player->name[i] == ' ')
    {
      return 0;
    }
  }
  return 1;
}

// Registro do nome dos jogadores
int validRegister(Player * playerA, Player * playerB)
{
  char buffer[500];
  while(1)
  {
    printf("Digite o nome do Jogador A: ");
    gets(buffer);
    strcpy(playerA->name, buffer);
    
    printf("Digite o nome do Jogador B: ");
    gets(buffer);
    strcpy(playerB->name, buffer);
    
    if (validName(playerA) + validName(playerB) == 2)
    {
      if(strcmp(playerA->name, playerB->name) != 0)
      {
        return 0;
      }
      else
      {
        printf("Nomes dos jogadores precisam ser diferentes\n");
      }
    }
    else
    {
      printf("Nome digitado invalido. Digite sem espaco\n");
    }
  }
  return 1;
}

// Determina qtd de ranks cadastrados
int rankSize(char * path)
{
  FILE * rankingTxt = NULL;
  char ch;
  int numberRank = 0;

  rankingTxt = fopen(path, "r");
  while((ch=fgetc(rankingTxt))!=EOF) 
  {
      if(ch=='\n')
      {
         numberRank+=1;
      }
   }
   fclose(rankingTxt);
   return numberRank;
}

// Exibi os ranks 
void showRanking(char * path)
{
  char rankLine[100];
  FILE * rankingTxt = NULL;
  rankingTxt = fopen(path, "r");
  if (rankingTxt == NULL)
  {
    printf("ERROR %s", path);
  }
  else
  {
    printf("-----------Exibindo RANK-----------\n");
    char * readName;
    char * readV;
    char * readL;

    while (fgets(rankLine, 100, rankingTxt) != NULL)
    {
      readName = strtok(rankLine, " ");
      readV = strtok(NULL, " ");
      readL = strtok(NULL, " ");
      printf("Jogador: %s\tVitoria: %s\tDerrota: %s\n", readName, readV, readL);
    }

    fclose(rankingTxt);
  }
}

// Processa os ranks na memoria
void processRanking(char * path, rank * rank, Player * playerA, Player * playerB, int numberRank)
{
  // Load Rank
  int i = 0;
  FILE * rankingTxt = NULL;
  rankingTxt = fopen(path, "r");
  if (rankingTxt == NULL)
  {
    printf("ERROR %s", path);
  }
  else
  {
    char rankLine[100];
    char * lineBuffer;
    while (fgets(rankLine, 100, rankingTxt) != NULL)
    {
      lineBuffer = strtok(rankLine, " ");
      strcpy(rank[i].playerName, lineBuffer);
      lineBuffer = strtok(NULL, " ");
      rank[i].v = atoi(lineBuffer);
      lineBuffer = strtok(NULL, " ");
      rank[i].l = atoi(lineBuffer);
      i++;
    }
    fclose(rankingTxt);
    // Atualizar o RANK de nome conhecido
    for(i=0; i < numberRank; i++)
    {
      if(strcmp(rank[i].playerName, playerA->name) == 0)
      {
        playerA->foundPlayer = 1;
        if(playerA->win)
        {
          rank[i].v++;
        }
        else
        {
          rank[i].l++;
        }
      }
      if(strcmp(rank[i].playerName, playerB->name) == 0)
      {
        playerB->foundPlayer = 1;
        if(playerB->win)
        {
          rank[i].v++;
        }
        else
        {
          rank[i].l++;
        }
      }
    }
  }
}

// Escreve os ranks num arquivo txt
void writeRanking(char * path, rank * rank, Player * playerA, Player * playerB, int numberRank)
{
  FILE * rankingTxt = NULL;
  char buffer[500];
  rankingTxt = fopen(path, "w");
  if (rankingTxt == NULL)
  {
    printf("ERROR %s", path);
  }
  else
  {  
    for(int i = 0; i < numberRank ; i++)
    {
      sprintf(buffer, "%s %d %d\n", rank[i].playerName, rank[i].v, rank[i].l);
      fputs(buffer, rankingTxt);
    }
    // Novos Jogadores
    if(playerA->foundPlayer == 0)
    {
      sprintf(buffer, "%s %d %d\n", playerA->name, playerA->win, playerA->lost);
      fputs(buffer, rankingTxt);
    }
    if(playerB->foundPlayer == 0)
    {
      sprintf(buffer, "%s %d %d\n", playerB->name, playerB->win, playerB->lost);
      fputs(buffer, rankingTxt);
    }
    fclose(rankingTxt);
  } 
}

// Processa os eventos do jogo 
int processEvents(SDL_Window *window, Player *playerA, Player *playerB, Ball * ball, Mix_Chunk * hitFx, int * stage)
{

  SDL_Event event;

  float playerCenter, diag;

  while(SDL_PollEvent(&event))
  {
    switch(event.type)
    {
      case SDL_WINDOWEVENT_CLOSE:
      {
        if(window)
        {
          SDL_DestroyWindow(window);
          window = NULL;
          *stage = -1;
        }
      }
      break;
      case SDL_KEYDOWN:
      {
        switch(event.key.keysym.sym)
        {
          case SDLK_ESCAPE:
          *stage = -1;
          break;
        }
      }
      break;
      case SDL_QUIT:
        *stage = -1;
        break;
    }
  }
  
  const Uint8 *state = SDL_GetKeyboardState(NULL);
  if(state[SDL_SCANCODE_SPACE])
  {
    *stage = 2;
  }
  if(state[SDL_SCANCODE_W] && playerA->y > 5)
  {
    playerA->y -= 5;
  }
  if(state[SDL_SCANCODE_S] && playerA->y < 405)
  {
    playerA->y += 5;
  }
  
  if(state[SDL_SCANCODE_UP] && playerB->y > 5)
  {
    playerB->y -= 5;
  }
  if(state[SDL_SCANCODE_DOWN] && playerB->y < 405)
  {
    playerB->y += 5;
  }
  // Reset da Bola 
  if(state[SDL_SCANCODE_R])
  {
    ball->x = 220;
    ball->y = 120;
    ball->velx = 2;
    ball->vely = 0;
  }

  // bloco de lógica que começa partida
  if (*stage == 2)
  {
    ball->x += ball->velx;
    ball->y += ball->vely;
  }
  
  

  //colisão c/ a parede
  if (ball->y >= 480 || ball->y <= 0)
  {
    Mix_PlayChannel(-1, hitFx, 0);
    ball->vely *= -1; 
  } 
  
  // detecção de pontuação
  if (ball->x >= 640 || ball->x <= 0) 
  {
    if(ball->x >= 640)
    {
      playerA->score += 1;
      ball->x = 220;
      ball->y = 120;
      ball->velx = 2;
      ball->vely = 0;
    } 
    else if (ball->x <= 0) 
    {
      playerB->score += 1;
      ball->x = 220;
      ball->y = 120;
      ball->velx = -2;
      ball->vely = 0;      
    }
  }; 
  
  // colisão com o jogador
  if ((ball->y >= playerB->y && ball->y <= playerB->y + 100 && ball->x == playerB->x - 10) || (ball->y >= playerA->y && ball->y <= playerA->y + 100 && ball->x == playerA->x + 5))
  {
    if (ball->x > 320) 
    {
      Mix_PlayChannel(-1, hitFx, 0);
      printf("Ax: %d\n", playerA->x);
      printf("Ay: %d\n", playerA->y);
      printf("Ballx: %d\n", ball->x);
      printf("Bally: %d\n", ball->y);
      playerCenter = playerB->y + 35;
      diag = playerCenter - ball->y;
      ball->vely += diag * - 0.1;
      ball->velx *= -1;
    } 
    else 
    {
      Mix_PlayChannel(-1, hitFx, 0);
      printf("Bx: %d\n", playerB->x);
      printf("By: %d\n", playerB->y);
      printf("Ballx: %d\n", ball->x);
      printf("Bally: %d\n", ball->y);
      playerCenter = playerA->y + 35;
      diag = playerCenter - ball->y;
      ball->vely += diag * - 0.1;
      ball->velx *= -1;
    }
  }

  // fim de jogo
  if (playerA->score >= 3 || playerB->score >= 3 )
  {
    ball->x = 700;
    ball->y = 700;
    *stage = -1;
    if(playerA->score > playerB->score)
    {
      playerA->win = 1;
      playerB->lost = 1;
    }
    else
    {
      playerB->win = 1;
      playerA->lost = 1;
    }

  }

  return *stage;
}

// Renderiza o menu
void doRenderMenu(SDL_Renderer *renderer, Texto * textoVic)
{
  
  //Escolhe a cor azul para renderizar 
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  
  //Deixa a tela azul
  SDL_RenderClear(renderer);
  
  //Escolhe a cor branca para novos desenhos 
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  SDL_Surface * msgA = TTF_RenderText_Solid(textoVic->font, "PONG C", textoVic->color);
  SDL_Texture * msgTextA = SDL_CreateTextureFromSurface(renderer, msgA);
  SDL_QueryTexture(msgTextA, NULL, NULL, &textoVic->texW, &textoVic->texH);

  SDL_Surface * msgB = TTF_RenderText_Solid(textoVic->font, "Controle A: W / S", textoVic->color);
  SDL_Texture * msgTextB = SDL_CreateTextureFromSurface(renderer, msgB);
  SDL_QueryTexture(msgTextB, NULL, NULL, &textoVic->texW, &textoVic->texH);

  SDL_Surface * msgC = TTF_RenderText_Solid(textoVic->font, "Controle B: Seta Cima / Seta Baixo", textoVic->color);
  SDL_Texture * msgTextC = SDL_CreateTextureFromSurface(renderer, msgC);
  SDL_QueryTexture(msgTextC, NULL, NULL, &textoVic->texW, &textoVic->texH);
  
  SDL_Surface * msgD = TTF_RenderText_Solid(textoVic->font, "ESPACO - Para comecar", textoVic->color);
  SDL_Texture * msgTextD = SDL_CreateTextureFromSurface(renderer, msgD);
  SDL_QueryTexture(msgTextD, NULL, NULL, &textoVic->texW, &textoVic->texH);

  SDL_Surface * msgE = TTF_RenderText_Solid(textoVic->font, "ESC - Para finalizar jogo", textoVic->color);
  SDL_Texture * msgTextE = SDL_CreateTextureFromSurface(renderer, msgE);
  SDL_QueryTexture(msgTextE, NULL, NULL, &textoVic->texW, &textoVic->texH);

  
  SDL_Rect bemvindo = { 90, 40, textoVic->texW, textoVic->texH };
  SDL_Rect instrucaoA = { 90, 180, textoVic->texW, textoVic->texH };
  SDL_Rect instrucaoB = { 90, 240, textoVic->texW, textoVic->texH };
  SDL_Rect instrucaoC = { 90, 380, textoVic->texW, textoVic->texH };
  SDL_Rect instrucaoD = { 90, 420, textoVic->texW, textoVic->texH };
  

  SDL_RenderCopy(renderer, msgTextA, NULL, &bemvindo);
  SDL_RenderCopy(renderer, msgTextB, NULL, &instrucaoA);
  SDL_RenderCopy(renderer, msgTextC, NULL, &instrucaoB);
  SDL_RenderCopy(renderer, msgTextD, NULL, &instrucaoC);
  SDL_RenderCopy(renderer, msgTextE, NULL, &instrucaoD);
  SDL_RenderPresent(renderer);
}

// Rederiza o jogo
void doRenderGame(SDL_Renderer *renderer, Player * playerA, Player * playerB, Ball * ball, Texto * textoVic, int * stage)
{
  //Escolhe a cor azul para renderizar 
  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  
  //Deixa a tela azul
  SDL_RenderClear(renderer);
  
  //Escolhe a cor branca para novos desenhos 
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  // conversão do placar para str
  char scorePlayerAStr[10];
  char scorePlayerBStr[10];
  sprintf(scorePlayerAStr, "%i", playerA->score);
  sprintf(scorePlayerBStr, "%i", playerB->score);
  // sprintf(scorePlayerAStr, "%i", ball->x);
  // sprintf(scorePlayerBStr, "%i", ball->y);

  
  // Processamento do Placar - texto
  // criação do text do jogador A 
  SDL_Surface * surfaceA = TTF_RenderText_Solid(textoVic->font, scorePlayerAStr, textoVic->color);
  SDL_Texture * textureA = SDL_CreateTextureFromSurface(renderer, surfaceA);
  SDL_QueryTexture(textureA, NULL, NULL, &textoVic->texW, &textoVic->texH);

  // criação do text do jogador B
  SDL_Surface * surfaceB = TTF_RenderText_Solid(textoVic->font, scorePlayerBStr, textoVic->color);
  SDL_Texture * textureB = SDL_CreateTextureFromSurface(renderer, surfaceB);
  SDL_QueryTexture(textureB, NULL, NULL, &textoVic->texW, &textoVic->texH);
  


  // Desenho e renderiza os elementos do jogo na tela 
  SDL_Rect rectA = { playerA->x, playerA->y, 5, 70 };
  SDL_Rect rectB = { playerB->x, playerB->y, 5, 70 }; 
  SDL_Rect rectball = { ball->x, ball->y, 10, 10 };
  SDL_Rect net = { 320, 0 , 1, 640 };
  SDL_Rect scoreA = { 180, 120, textoVic->texW, textoVic->texH };
  SDL_Rect scoreB = { 390, 120, textoVic->texW, textoVic->texH };
  
  SDL_RenderFillRect(renderer, &rectA);
  SDL_RenderFillRect(renderer, &rectB);
  SDL_RenderFillRect(renderer, &rectball);
  SDL_RenderFillRect(renderer, &net);
  SDL_RenderCopy(renderer, textureA, NULL, &scoreA);
  SDL_RenderCopy(renderer, textureB, NULL, &scoreB);
  
  //Apresenta aquilo feito na tela 
  SDL_RenderPresent(renderer);

  // Prende o resultado na tela 
  if(*stage == -1 ) {
    SDL_Delay(3000);
  }

}

int main()
{
  int validNames = 1;
  int stage = 1;
  // char buffer[500];

  // Criar o jogadores
  Player playerA;
  Player playerB;
  
  printf("PONG C\n");
  printf("Insira nome sem espaco\n");

  while (validNames)
  {
    validNames = validRegister(&playerA, &playerB);
  }
  
  SDL_Window *window;                    // Declaração de janela 
  SDL_Renderer *renderer;                // Declaração de renderização
  
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);              // Inicializa o SDL2 
  TTF_Init();
  
  // Inicializando as peça do jogo com seus atributos
  playerA.x = 5;
  playerA.y = 190;
  playerA.score = 0;
  playerA.win = 0;
  playerA.lost = 0;
  playerA.foundPlayer = 0;
  

  playerB.x = 630;
  playerB.y = 190;
  playerB.score = 0;
  playerB.win = 0;
  playerB.lost = 0;
  playerB.foundPlayer = 0;

  Ball ball;
  ball.x = 220;
  ball.y = 120;
  ball.velx = 2;
  ball.vely = 0;

  Texto textMenu;
  textMenu.texW = 0;
  textMenu.texH = 0;
  textMenu.color.r = 255;
  textMenu.color.b = 255;
  textMenu.color.g = 255;
  textMenu.color.a = 255;
  textMenu.font = TTF_OpenFont("Roboto.ttf", 38);

  Texto textVictory;
  textVictory.texW = 0;
  textVictory.texH = 0;
  textVictory.color.r = 255;
  textVictory.color.b = 255;
  textVictory.color.g = 255;
  textVictory.color.a = 255;
  textVictory.font = TTF_OpenFont("Roboto.ttf", 120);

  // Abre canal de audio
  Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
  
  // Carrega o efeito sonoro
  Mix_Chunk * hitFx = Mix_LoadWAV("Hit.wav");
  
  // Criar a janela da aplicação com as seguintes configs: 
  window = SDL_CreateWindow("PongC",                           // título da janela
                            SDL_WINDOWPOS_UNDEFINED,           // posição inicial x 
                            SDL_WINDOWPOS_UNDEFINED,           // posição inicial y
                            640,                               // largura em pixels
                            480,                               // altura em pixels
                            0                                  // flags
                            );
  
  renderer = SDL_CreateRenderer(
                            window, 
                            -1, 
                            SDL_RENDERER_ACCELERATED
                            );

  //Esconde o mouse
  SDL_ShowCursor(0);

  //Loop principal do jogo
  while(stage > 0)
  {
    
    //Observar os eventos do jogo 
    stage = processEvents(window, &playerA, &playerB, &ball, hitFx, &stage);
    
    if(stage == 1)
    {
      // Renderiza o texto do menu
      doRenderMenu(renderer, &textMenu);
    }
    else
    {
      //Renderiza no display os eventos do jogo
      doRenderGame(renderer, &playerA, &playerB, &ball, &textVictory, &stage);
    }
    

    //Controla o tempo do interno do jogo
    SDL_Delay(10);
  }
  
  
  // Limpeza da memoria pós-jogo
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  TTF_CloseFont(textVictory.font);
  Mix_FreeChunk(hitFx);
  
  // Última etapa de limpeza
  TTF_Quit();
  Mix_CloseAudio();
  SDL_Quit();

  
  // Ranking Logics

  // Função para conhecer o numero de ranking cadastrado
  int numberRank = rankSize("ranking.txt");

  // Criação dinamica do estrutura dos ranks
  rank * readRank = malloc(sizeof(rank) * numberRank);

  // Carrega o arquivo de ranking na memoria 
  processRanking("ranking.txt",readRank, &playerA, &playerB, numberRank);

  // Grava no txt o ranking atualizado
  writeRanking("ranking.txt", readRank, &playerA, &playerB, numberRank);

  // EXIBIR RANKING
  showRanking("ranking.txt");
  
  // Libera memoria dinamica
  free(readRank);

  return 0;
}

