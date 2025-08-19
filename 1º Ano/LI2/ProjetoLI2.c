#include <ncurses.h>
#include <stdlib.h>  // rand() srand()
#include <stdbool.h> // boll
#include <time.h>    // time()
#include <string.h>  // strcmp() strncpy()

#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define MAGENTA 5
#define CYAN 6

int py, px;                 // Coordenadas player
int sy, sx;                 // Coordenadas salas 
int hp = 50;                // Vida do player
int r = 0;                  // Numero de salas posicionadas 
bool p = 0;                 // Player posicionado 
bool e = 0;                 // Numero de monstros posicionados 
int e_derrotada;            // Inimigos derrotados 
int snivel = 1;             // Niveis das salas

// Personagens e habilidades 
char personagem[8] = {0}; 
int agilidade;              
int ataque;

struct mobs{
    int y;
    int x;
    int vida;
    int tipo;
    bool acordado;
};

struct mobs mob[10];


int desenhar_salas(int rows, int cols, char **map){

    // Coloca os caracteres nos respetivos sitios no map
    for (int y = 0; y < rows; y++){

        for (int x = 0; x < cols; x++){

            // Chao vazio
            if (map[y][x] == ' ')
                mvaddch(y, x, ' ');
            // Escadas
            else if (map[y][x] == '=')
            {
                attron(A_BOLD); // Serve para por mais branco o = -
                mvaddch(y, x, '=');
                standend();
            }
            // Menu durante o jogo
            else if (y == 0 || y == rows - 1)
                mvaddch(y, x, ' ');
            // Barreiras
            else if (map[y][x] == '&')
                mvaddch(y, x, '&');
            // Paredes
            else if (map[y][x] == '#')
                mvaddch(y, x, '#');
            // Mobs
            else if (map[y][x] == 'e')
            {
                for (int m = 0; m < 10; m++)
                {
                    if(mob[m].y == y && mob[m].x == x)
                    {
                        if(mob[m].vida < snivel / 2 + 2)
                        {
                            attron(COLOR_PAIR(RED));
                                mvaddch(y, x, mob[m].tipo);
                            attroff(COLOR_PAIR(RED));
                        }

                        else if(mob[m].vida < snivel + 2)
                        {
                            attron(COLOR_PAIR(YELLOW));
                                mvaddch(y, x, mob[m].tipo);
                            attroff(COLOR_PAIR(YELLOW));
                        }
                        else
                            mvaddch(y, x, mob[m].tipo);
                    }
                }
            }
        }
    }

    attron(A_BOLD); // Serve para por mais branco o @
    mvaddch(py,px,'@'); // Desenhar jogador
    standend();

    return 0;
}

int movimento_mob(char **map){

    int distancia_y, distancia_x;

    for (int m = 0; m < 10; m++){

        if(mob[m].vida < 1)
            continue;

        distancia_y = abs(mob[m].y - py);
        distancia_x = abs(mob[m].x - px);
        
        // verificar monstros acordados
        if (!(rand() % (snivel + 1))){} // se for diferente de 0, 1, 2 no caso do primeiro nivel( se for 0 nao faz nada caso seja 1 ou 2 avança para o else if)
     

        // Distancia a que os mobs acordam, quanto maior o nivel maior é o alcance 
        else if (distancia_y < snivel + 3 - agilidade && distancia_x < snivel + 3 - agilidade)
            mob[m].acordado = 1;

        if(mob[m].acordado == 0)
            continue;
        
        int direcao_y = mob[m].y;
        int direcao_x = mob[m].x;
        
        if(mob[m].tipo == 'a' || mob[m].tipo == 'c'){ // Estupida e Esperta

            // Movimento frente, trás, esquerda e direita 
            if(distancia_y > distancia_x)
            {
                if(direcao_y > py)
                    direcao_y -= 1; // Desce uma posiçao atras do jogador
                else 
                    direcao_y += 1;
            }
            else
            {
                if(direcao_x > px)
                    direcao_x -= 1;
                else
                    direcao_x += 1;
            }

            // Se falhar testar movimento na diagonal
            if(map[direcao_y][direcao_x] == '#' || map[direcao_y][direcao_x] == '&')
            {
                int direcao_y = mob[m].y;
                int direcao_x = mob[m].x;

                if(direcao_y > py)
                    direcao_y -= 1;
                else 
                    direcao_y += 1;

                if(direcao_x > px)
                    direcao_x -= 1;
                else 
                    direcao_x += 1;
            }
            
            // Se o mob estiver preso move para o vazio mais proximo
            if(map[direcao_y][direcao_x] == '&' || map[direcao_y][direcao_x] == '#')
            {
                int direcao_y = mob[m].y;
                int direcao_x = mob[m].x;

                if(distancia_y > distancia_x)
                {
                    if(direcao_x > px)
                        direcao_x  -= 1;
                    else 
                        direcao_x += 1;
                }
                else
                {
                    if(direcao_y > py)
                        direcao_y -= 1;
                    else
                        direcao_y += 1;
                }

            }
        }

        if(mob[m].tipo == 'b'){ //Cobarde

            // Movimento frente, trás, esquerda e direita 
            if(distancia_y > distancia_x)
            {
                if(direcao_y < py)
                    direcao_y -= 1; // fuga para cima
                else 
                    direcao_y += 1; // fuga para baixo
            }
            else
            {
                if(direcao_x < px)
                    direcao_x -= 1; // fuga para a esquerda
                else
                    direcao_x += 1; // fuga para a direita
            }

            // Movimento na Diagonal
            if(map[direcao_y][direcao_x] == '#' || map[direcao_y][direcao_x] == '&')
            {
                int direcao_y = mob[m].y;
                int direcao_x = mob[m].x;

                if(direcao_y < py)
                    direcao_y -= 1; // fuga na diagonal para cima
                else 
                    direcao_y += 1; // fuga na diagonal para baixo

                if(direcao_x < px)
                    direcao_x -= 1; // fuga na diagonal para a esquerda
                else 
                    direcao_x += 1; // fuga na diagonal para a direita
            }
            
            // Se o mob estiver preso
            if(map[direcao_y][direcao_x] == '&' || map[direcao_y][direcao_x] == '#'){
                
                int direcao_y = mob[m].y;
                int direcao_x = mob[m].x;

                if(distancia_y > distancia_x)
                {
                    if(direcao_x < px)
                        direcao_x  -= 1; // fuga para a esquerda
                    else 
                        direcao_x += 1; // fuga para a direita
                }
                else
                {
                    if(direcao_y < py)
                        direcao_y -= 1; // fuga para cima
                    else
                        direcao_y += 1; // fuga para baixo
                }

            }
        }


        // Se a distancia entre o mob e o player for de 1 ou menor o player perde vida
        if(distancia_y <= 1 && distancia_x <= 1){

            hp -= snivel / 2 + 1;
        }
        else if(map[direcao_y][direcao_x] == ' '){

            map[mob[m].y][mob[m].x] = ' ';
            mob[m].y = direcao_y;
            mob[m].x = direcao_x;
            map[mob[m].y][mob[m].x] = 'e';
        }
    }

  return 0;
}

int combate(char **map, int y_dir, int x_dir){

    for (int m = 0; m < 10; m++)
    {
        if(mob[m].y == y_dir && mob[m].x == x_dir)
        {
            if(mob[m].vida <= 0) 
            {
                map[mob[m].y][mob[m].x] = ' ';
                e_derrotada += 1;
                
                if(mob[m].tipo == 'b'){
                    if(hp <= 49) hp += rand() % 1;
                }

                if(mob[m].tipo == 'a' || mob[m].tipo == 'c'){
                    if(hp <= 47) hp += rand() % 3;
                }

            } 
            else
                mob[m].vida -= ataque;
        }
    }

    return 0;
}

int movimento(int c, char **map){

    int y_dir = py, x_dir = px;

    if (c == KEY_UP){
        y_dir--;
    }

    else if (c == KEY_DOWN){
        y_dir++;
    }
    
    else if (c == KEY_LEFT){
        x_dir--;   
    }

    else if (c == KEY_RIGHT){
        x_dir++;
    }

    else if (c == '=' && e_derrotada >= snivel + 1){
        r = 0;
        p = 0;
        e = 0;
        return 1;
    }

    if(map[y_dir][x_dir] == ' ' || map[y_dir][x_dir] == '='){
        py = y_dir;
        px = x_dir;
    }

    else if(map[y_dir][x_dir] == 'e')
        combate(map, y_dir, x_dir);

    return 0;

}

int desenhar_mobs(int rows, int cols, char **map){

    if (e == 0)
    {
        int y, x;

        for (int m = 0; m < 10; m++)
        {
            do
            {
                y = rand() % rows;
                x = rand() % cols;

            } while (map[y][x] != ' ');

            mob[m].y = y;
            mob[m].x = x;

            mob[m].vida = rand() % snivel + 2; // No 1 nivel a vida varia de 2 a 3
    

            if(snivel == 1 && !(rand() % 3))
                mob[m].vida = 1;

            if(rand() % 2)
                mob[m].vida = snivel + 2;
            

            mob[m].tipo = 97 + (rand() % 3); // caracteres dos mobs a(estupida), b(cobarde), c(esperta)

            mob[m].acordado = 0;

            map[mob[m].y][mob[m].x] = 'e';
        }

        e = 1;
    }

    return 0;
}

int gerar_player(int rows, int cols, char **map){

    if (!p)
    {
        int dy;
        int dx;

        do
        {
            py = rand() % rows;
            px = rand() % cols;

            // distancia das escadas
            dy = abs(py - sy);
            dx = abs(px - sx);

            if (map[py][px] == ' ' && (dy > 50 + snivel / 2 || dx > 50 + snivel / 2))
                break;
        } while (1);

        p = 1;
    }

    return 0;
}

int gen_salas(int rows, int cols, char **map){

    // Criaçao das salas
    if (!r){ // mesma coisa que ter r == 0;

        int ry, rx;                                // Coordenadas sala
        int r_tamanhox, r_tamanhoy;
        int r_centroy = 0, r_centrox = 0;          // centro das salas
        int r_centroy_antiga, r_centrox_antiga;
        int r_num = rand() % 5 + 3;                // numero possivel de salas 
        bool juntas;
        int loop = 0;
    
        // Completa mapa com os caracteres
        for (int y = 0; y < rows; y++)
        {
            for (int x = 0; x < cols; x++)
            {
                // limites
                if (y == 0 || y == 1 || y == rows - 2 || x == 0 || x == cols - 1 || y == rows)
                    map[y][x] = '&';
                // paredes
                else
                    map[y][x] = '#';
            }
        }

        while (r < r_num) // Numero de salas posicionadas menor que o n de salas geradas
        {
            do
            {
                juntas = 0;

                do
                {
                    ry = rand() % (rows - 4) + 1; // coordenadas vao de 1 a 43 
                    rx = rand() % (cols - 4) + 1; // coordenadas vao de 1 a 157

                    r_tamanhoy = rand() % 7 + 5;    // tamanho vai de 5 linhas a 13 linhas 
                    r_tamanhox = rand() % 13 + 10;  // tamanho vai de 10 colunas a 23 colunas

                } while (ry < 1 || rx < 1); // se as coordenadas geradas forem menor que 1 ele gera novas

                loop++;

                if (loop > 100) 
                {
                    ry = 7;
                    rx = 8;
                    r_tamanhoy = 2;
                    r_tamanhoy = 2;
                    break;
                }

                // verificar dungeons juntas
                for (int y = ry; y < ry + r_tamanhoy; y++)  // y menor que a coordenada final da altura da sala
                {
                    for (int x = rx; x < rx + r_tamanhox; x++) // x menor que a coordenda final da altura da sala
                    {
                        if (map[y][x] == ' ' || map[y][x] == '&' || map[y + 2][x] == ' ' ||
                            map[y - 2][x] == ' ' || map[y][x + 2] == ' ' || map[y][x - 2] == ' ')
                        {
                            juntas = 1;
                            y = ry + r_tamanhoy; 
                            break;               
                        }
                    }
                }

            } while (juntas); // se no fim isto se verificar verdade vai se repetir este loop gerando novas coordenadas para a sala

            // Preencher o mapa[y][x] com as coordenadas da dungeon
            for (int y = ry; y < ry + r_tamanhoy; y++)
            {
                for (int x = rx; x < rx + r_tamanhox; x++)
                {
                    if (map[y][x] != '&' && y >= 0 && y < rows - 1 && x >= 0 && x < cols)
                    {
                        map[y][x] = ' ';
                    }
                }
            }

            r++; 

            // Centro da sala anteriror
            if (r > 1)
            {
                r_centroy_antiga = r_centroy;
                r_centrox_antiga = r_centrox;
            }

            // Centro da sala
            r_centroy = ry + (r_tamanhoy / 2);
            r_centrox = rx + (r_tamanhox / 2);
            
            // Criar corredores se ja tivermos pelo menos 2 salas
            if (r > 1)
            {

                int corredor_y;

                for (corredor_y = r_centroy_antiga; corredor_y != r_centroy;) // se tiver na mm linha faz logo x 
                {
                    map[corredor_y][r_centrox_antiga] = ' ';

                    if (r_centroy_antiga < r_centroy){
                        corredor_y++;
                    }
                    else if (r_centroy_antiga > r_centroy){
                        corredor_y--;
                    }
                }

                int corredor_x;

                for (corredor_x = r_centrox_antiga; corredor_x != r_centrox;) // se tiver na mm coluna faz so o y
                {
                    map[corredor_y][corredor_x] = ' ';

                    if (r_centrox_antiga < r_centrox){
                        corredor_x++;
                    }
                    else if (r_centrox_antiga > r_centrox){
                        corredor_x--;
                    }
                }
            }
        }

        // gerar escadas

        do
        {
            sy = rand() % (rows - 3);
            sx = rand() % (cols - 3);

        } while (map[sy][sx] != ' ');
        map[sy][sx] = '=';

    }

    return 0;
}

int caracteres(int c){

    ataque = 1;
    agilidade = 0;
    e = 0;
    p = 0;
    r = 0;
    snivel = 1;

    if(c == '1'){
        if(!strcmp(personagem, "Homem") && !strcmp(personagem, "Mulher"))
            c = '1';
    }

    if(c == '2'){
        if(!strcmp(personagem, "Homem") && !strcmp(personagem, "Mulher"))
            c = '2';
    }
    

    switch (c){

        case '1':
        {
            ataque += 2;
            agilidade = -3;
            strncpy(personagem, "Homem\0", 6);
            break;
        }

        case '2':
        {
            ataque += 0;
            agilidade = 1;
            strncpy(personagem, "Mulher\0", 7);
            break;
        }

    }
    

    return 0;
}

int salas(int c, int rows, int cols, char **map){

    bool nivel = false; 

    srand(time(NULL));

    if((c == '1' || c == '2') && snivel == 1)
        caracteres(c);
    

    gen_salas(rows, cols, map);

    gerar_player(rows, cols, map);

    desenhar_mobs(rows, cols, map);

    if(c != 0)
    {
        nivel = movimento(c, map);   
    }

    movimento_mob(map);

    desenhar_salas(rows, cols, map);
    

    // Menu dentro do jogo
    mvprintw(rows - 1, 0, " %s -> HP: %d  \t  Ataque: %d  \t  Mobs derrotados: %d \t Nivel: %d", personagem, hp, ataque, e_derrotada, snivel);

    if(map[py][px] == '='){
        attron(A_BOLD);
        mvprintw(0, cols/3, "           < Escadas para descer na dungeon >");
        standend();
    }

    if(nivel){
        clear();
        mvprintw(rows/2, cols/2, "Nivel: %d! (pressionar 'Enter')", ++snivel);
    }

    if(c == 'r'){

        clear();
        printw("\n\n\n\n\n\n\n\n\n");
        attron(A_BOLD);
        printw(  "\t\t\t\t\t\t\t\t\t\tPAUSA\n\n\n\n\n\n\n\n\n");

        printw("\t\t\t\t\tNivel Alcançado: %d\n\n \t\t\t\t\tMonstros Derrotados: %d\n\n\n\n", snivel, e_derrotada);
        printw("\t\t\t\t\tPressina 'ENTER' para voltar");
        attroff(A_BOLD);

        c = getch();
    }


    if(hp <= 0){

        clear();
        printw("\n\n\n\n\n\n\n\n\n\n\n\n");
        attron(A_BOLD);
        printw(  "\t\t\t\t\t\t#     #  #######  ######   ######   #######  #######  #######  #######\n"
                 "\t\t\t\t\t\t# # # #  #     #  #     #  #     #  #        #           #     #      \n"
                 "\t\t\t\t\t\t#  #  #  #     #  ######   ######   #######  #######     #     #######\n"
                 "\t\t\t\t\t\t#     #  #     #  #   #    #   #    #              #     #     #      \n"
                 "\t\t\t\t\t\t#     #  #######  #     #  #     #  #######  #######     #     #######\n\n\n\n\n\n\n\n\n");

        printw("\t\t\tNivel Alcançado: %d\t\t\t\t\t\t\t\t\t\tPressione 'ESC' para sair!!\n\n \t\t\tMonstros Derrotados: %d", snivel, e_derrotada);
        attroff(A_BOLD);


    }


    c = getch();

    return c;
}

int janela_inicial(){

    int c = 0;

    printw("\n\n\n\n\n\n\n");
    attron(A_BOLD);
    printw("\t\t\t\t\t\t\t        ######    #######   #######   #     #\n"
           "\t\t\t\t\t\t\t        #     #       #         #     # # # # \n"
           "\t\t\t\t\t\t\t        #     #   #   #     #   #     #  #  #\n"
           "\t\t\t\t\t\t\t        #     #   #   #     #   #     #     # \n"
           "\t\t\t\t\t\t\t        ######    #####     #####     #     #\n");
    attroff(A_BOLD);

    printw("\n\n\n\n\n\n\t\t\t      |");
    attron(A_BOLD | COLOR_PAIR(MAGENTA));
    printw("Objetivo do jogo:");
    attroff(A_BOLD | COLOR_PAIR(MAGENTA));
    printw("|\n\n");
    attron(A_BOLD);
    printw("\t\t\t\t -> Chegar ao nivel mais baixo possivel da dungeon, para isso tens que matar os monstros de forma que\n"
           "\t\t\t\t     chegues as escadas ou tentares passar por eles sem que eles reparem em ti, tu é que ESCOLHES! ");


    printw("\n\n\n\n\t\t\t\t\t\t         Escolha o seu personagem:");
    attroff(A_BOLD);

    printw("  |");
    attron(A_BOLD | COLOR_PAIR(MAGENTA));
    printw("1) Homem");
    attroff(A_BOLD | COLOR_PAIR(MAGENTA));
    printw("|");

    printw("  |");
    attron(A_BOLD | COLOR_PAIR(MAGENTA));
    printw("2) Mulher");   
    attroff(A_BOLD | COLOR_PAIR(MAGENTA));
    printw("|\n\n");
    
    printw("\t\t\t\t\t\t\t     ");
    attron(A_BOLD | COLOR_PAIR(MAGENTA));
    printw("pressiona '?' para ver os seus atributos");
    attroff(A_BOLD | COLOR_PAIR(MAGENTA));
    printw("|");

    printw("\n\n\n\n\t\t\t      |");
    attron(A_BOLD | COLOR_PAIR(CYAN));
    printw("Comandos:");
    attroff(A_BOLD | COLOR_PAIR(CYAN));
    printw("|\n\n");

    attron(A_BOLD);
    printw("\t\t\t\t -> Movimentar/Atacar: Setas;\n\n");
    printw("\t\t\t\t -> Próximo nivel: '=';");
    attroff(A_BOLD);


    c = getch();

    if(c == '?'){

        clear();
        printw("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\t\t\t\t\t\t\t\t\t    |");
        attron(A_BOLD | COLOR_PAIR(BLUE));
        printw("Personagens:");
        attroff(A_BOLD | COLOR_PAIR(BLUE));
        printw("|\n\n\n\n");

        attron(A_BOLD);
        printw("\t\t\t\t\t\t\t  1 -> |Homem:   ++Ataque|   --Agilidade|   +Vida|\n\n");
        printw("\t\t\t\t\t\t\t   2 -> |Mulher:   -Ataque|   +Agilidade|   Vida|\n\n\n\n\n\n");
        printw("\t\t\t\t\t Entao qual vais escolher 1 ou 2?");
        attroff(A_BOLD);

        c = getch();

    }

    return c;

}

int main(){

    int c; // Input
    int rows, cols;

    initscr(); // Inicia a biblioteca curses

    start_color();
    use_default_colors();

    init_pair(RED, COLOR_RED, COLOR_BLACK);
    init_pair(GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(CYAN, COLOR_CYAN, COLOR_BLACK);

    keypad(stdscr, 1); // Permite o uso de teclas especiais, como as setas

    noecho();    // Desabilita a exibição das teclas digitadas pelo usuário
    curs_set(0); // Esconde o cursor

    rows = 46;
    cols = 160;

    char **map = malloc(rows * sizeof(char *)); // duplo apontador e malloc aloca a memoria para o numero de linhas e sizeof... determina o tamanho necessario

    for (int i = 0; i < rows; i++)
    {
        map[i] = malloc(cols * sizeof(char));
    }
    
    // janela inicial
    c = janela_inicial();

    if (c == 27) // 27 == 'ESC'
    {
        endwin();
        return 0;
    }


    while (1)
    {
        c = salas(c, rows, cols, map);
        if (c == 27)
        {
            endwin();
            return 0;
        }
    }

    for (int i = 0; i < rows; i++)
    {
        free(map[i]);
    }
    free(map);

}