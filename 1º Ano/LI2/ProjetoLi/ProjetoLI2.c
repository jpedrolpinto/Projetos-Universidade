#include <ncurses.h>
#include <curses.h>
#include <stdlib.h>  // rand() srand()
#include <stdbool.h> // boll
#include <time.h>    // time()

#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define MAGENTA 5
#define CYAN 6

int py, px; // Coordenadas player
int ataque = 1;
int hp = 30;
int r = 0;
bool p = 0;
bool e = 0;
int e_derrotada;
int snivel = 1;

struct mobs
{
    int y;
    int x;
    int vida;
    int tipo;
    bool acordado;
};

struct mobs mob[10];

int desenhar_salas(int rows, int cols, char **map){

    // Coloca ' ' no map
    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < cols; x++)
        {
            // Chao vazio
            if (map[y][x] == ' ')
                mvaddch(y, x, ' ');
            // Escadas
            else if (map[y][x] == '=')
            {
                attron(A_BOLD); // Serve para por mais branco o = 
                mvaddch(y, x, '=');
                standend();
            }
            // UI
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

    mvprintw(rows - 1, 0, " HP: %d  \t  Ataque: %d  \t  Mobs derrotados: %d \t Nivel: %d", hp, ataque, e_derrotada, snivel);

    return 0;
}

int mov_mob(int rows, int cols, char **map){

    int distancia_y, distancia_x;

    for (int m = 0; m < 10; m++){

        if(mob[m].vida < 1)
            continue;

        distancia_y = abs(mob[m].y - py);
        distancia_x = abs(mob[m].x - px);
        
        // verificar monstros acordados
        if(distancia_y < 5 && distancia_x < 5)
            mob[m].acordado = 1;

        if(mob[m].acordado == 0)
            continue;
        
        int direcao_y = mob[m].y;
        int direcao_x = mob[m].x;
        
        if(mob[m].tipo != 'b'){ // Estupida

            // Movimento frente, trás, esquerda e direita 
            if(distancia_y > distancia_x)
            {
                if(direcao_y < py)
                    direcao_y++;
                else 
                    direcao_y--;
            }
            else
            {
                if(direcao_x < px)
                    direcao_x++;
                else
                    direcao_x--;
            }

            // Movimento na Diagonal
            if(map[direcao_y][direcao_x] == '#' || map[direcao_y][direcao_x] == '&')
            {
                int direcao_y = mob[m].y;
                int direcao_x = mob[m].x;

                if(direcao_y < py)
                    direcao_y++;
                else 
                    direcao_y--;

                if(direcao_x < px)
                    direcao_x++;
                else 
                    direcao_x--;
            }
            
            // Se o mob estiver preso
            if(map[direcao_y][direcao_x] == '&' || map[direcao_y][direcao_x] == '#')
            {
                if(distancia_y > distancia_x)
                {
                    if(direcao_y > py)
                        direcao_y++;
                    else 
                        direcao_y--;
                }
                else
                {
                    if(direcao_x < px)
                        direcao_x++;
                    else
                        direcao_x--;
                }

            }
        }

        if(mob[m].tipo == 'b'){ //Cobarde

            if ((mob[m].y + 5) != 'b' || (mob[m].x + 5) != 'b'){
                // Movimento frente, trás, esquerda e direita 
                if(distancia_y > distancia_x)
                {
                    if(direcao_y < py)
                        direcao_y--;
                    else 
                        direcao_y++;
                }
                else
                {
                    if(direcao_x < px)
                        direcao_x--;
                    else
                        direcao_x++;
                }

                // Movimento na Diagonal
                if(map[direcao_y][direcao_x] == '#' || map[direcao_y][direcao_x] == '&')
                {
                    int direcao_y = mob[m].y;
                    int direcao_x = mob[m].x;

                    if(direcao_y < py)
                        direcao_y--;
                    else 
                        direcao_y++;

                    if(direcao_x < px)
                        direcao_x--;
                    else 
                        direcao_x++;
                }
                
                // Se o mob estiver preso
                if(map[direcao_y][direcao_x] == '&' || map[direcao_y][direcao_x] == '#')
                {
                    if(distancia_y > distancia_x)
                    {
                        if(direcao_y > py)
                            direcao_y--;
                        else 
                            direcao_y++;
                    }
                    else
                    {
                        if(direcao_x < px)
                            direcao_x--;
                        else
                            direcao_x++;
                    }

                }
            }
        }


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
}

int combate(int cols, char **map, int y_dir, int x_dir){

    for (int m = 0; m < 10; m++)
    {
        if(mob[m].y == y_dir && mob[m].x == x_dir)
        {
            if(mob[m].vida <= 0) 
            {
                map[mob[m].y][mob[m].x] = ' ';
                e_derrotada += 1;
            } 
            else
                mob[m].vida -= ataque;
        }
    }

    return 0;
}

int movimento(int c, int cols, char **map){
    int y_dir = py, x_dir = px;

    if (c == KEY_UP)
    {
        y_dir = py - 1;
    }

    else if (c == KEY_DOWN)
    {
        y_dir = py + 1;
    }
    
    else if (c == KEY_LEFT)
    {
        x_dir = px - 1;   
    }

    else if (c == KEY_RIGHT)
    {
        x_dir = px + 1;
    }

    else if (c == '=')
    {
        r = 0;
        p = 0;
        e = 0;
        return 1;
    }

    if(map[y_dir][x_dir] == ' ' || map[y_dir][x_dir] == '=')
    {
        py = y_dir;
        px = x_dir;
    }
    else if(map[y_dir][x_dir] == 'e')
        combate(cols, map, y_dir, x_dir);

    return 0;

}

int desenhar_mobs(int rows, int cols, char **map){
    if (p == 0)
    {
        do
        {
            py = rand() % rows;
            px = rand() % cols;

        } 
        while (map[py][px] != ' ');

        map[py][px] = ' ';

        p = 1;
    }

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

int gen_salas(int rows, int cols, char **map){
    // Completa map com #
    if (r == 0)
    {
        for (int y = 0; y < rows; y++)
        {
            for (int x = 0; x < cols; x++)
            {
                // Linha de cima e linha de baixo
                if (y == 0 || y == 1 || y == rows - 2)
                {
                    map[y][x] = '&';
                }
                // Borders da esquerda e da direita
                else if ((x == 0 || x == cols - 1) && y != rows - 1)
                {
                    map[y][x] = '&';
                }

                else if (y == rows - 1)
                {
                    map[y][x] = '&';
                }
                // Bordas Interiores
                else
                    map[y][x] = '#';
            }
        }
    }

    // Criaçao da dungeon
    if (r == 0)
    {

        int r_tamanhox, r_tamanhoy;
        int ry, rx;               // Coordenadas dungeon
        int r_centroy, r_centrox; // centro das dungeons
        int r_centroy_antiga, r_centrox_antiga;
        int r_num = rand() % 7 + 7;

        while (r < r_num)
        {
            bool juntas;
            int loop = 0;

            do
            {
                juntas = 0;

                // criaçao do ponto de geraçao da dungeon
                do
                {
                    ry = rand() % rows - 3;
                    rx = rand() % cols - 3;

                    // tamanho da dungeon
                    r_tamanhoy = rand() % 6 + 4;
                    r_tamanhox = rand() % 12 + 8;

                } while (ry < 1 || rx < 1);

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
                for (int y = ry; y < ry + r_tamanhoy; y++)
                {
                    for (int x = rx; x < rx + r_tamanhox; x++)
                    {
                        if (map[y][x] == ' ' || map[y][x] == '&' || map[y + 2][x] == ' ' ||
                            map[y - 2][x] == ' ' || map[y][x + 2] == ' ' || map[y][x - 2] == ' ')
                        {
                            juntas = 1;
                            y = ry + r_tamanhoy; // sair do loop de fora
                            break;                // sair do loop interino
                        }
                    }
                }

            } while (juntas);

            // Preencher o mapa[y][x] com as coordenadas da dungeon
            for (int y = ry; y < ry + r_tamanhoy; y++)
            {
                for (int x = rx; x < rx + r_tamanhox; x++)
                {
                    if (map[y][x] != '&' && y >= 0 && y < rows - 1 &&
                        x >= 0 && x < cols)
                    {
                        map[y][x] = ' ';
                    }
                }
            }

            r++;

            // Centro da dungeon anteriror
            if (r > 0)
            {
                r_centroy_antiga = r_centroy;
                r_centrox_antiga = r_centrox;
            }

            // Centro da dungeon
            r_centroy = ry + (r_tamanhoy / 2);
            r_centrox = rx + (r_tamanhox / 2);
            
            // Criar corredores
            if (r > 1)
            {

                int corredor_y;

                for (corredor_y = r_centroy_antiga; corredor_y != r_centroy;)
                {
                    map[corredor_y][r_centrox_antiga] = ' ';

                    if (r_centroy_antiga < r_centroy)
                        corredor_y++;
                    else if (r_centroy_antiga > r_centroy)
                        corredor_y--;
                }

                for (int x = r_centrox_antiga; x != r_centrox;)
                {
                    map[corredor_y][x] = ' ';

                    if (r_centrox_antiga < r_centrox)
                        x++;
                    else if (r_centrox_antiga > r_centrox)
                        x--;
                }
            }
        }

        // gerar escadas 
        int sy, sx;

        do
        {
            sy = rand() % rows - 3;
            sx = rand() % cols - 3;

        } while (map[sy][sx] != ' ');
        map[sy][sx] = '=';

    }

    return 0;
}

int salas(int c, int rows, int cols, char **map){
    bool nivel; 
    srand(time(NULL));

    gen_salas(rows, cols, map);

    desenhar_mobs(rows, cols, map);

    if(c != 0)
    {
        nivel = movimento(c, cols, map);   
    }

    mov_mob(rows, cols, map);

    desenhar_salas(rows, cols, map);

    attron(A_BOLD); // Serve para por mais branco o @
    mvaddch(py,px,'@'); // Desenhar jogador
    standend();

    if(nivel){
        clear();
        mvprintw(0, 0, "Nivel: %d! (pressionar 'Enter')", ++snivel);
    }

    if(hp <= 0){
        clear();
        mvprintw(rows / 2, cols / 2 - 10, "MORRESTE\n\n\n\n\n\n\n\n\n"
                                          "\tNivel Alcançado: %d\n\n"
                                          "\tMonstros Derrotados: %d",
                                          snivel, e_derrotada);
    }

    c = getch();

    return c;
}

int main(){
    int c = 0; // Input
    int rows, cols;

    initscr();         // Inicia a biblioteca curses

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

    // adaptive way of the screen size:
    // getmaxyx(stdscr,rows,cols);
    // but this particular game designed to be played at tiny term:
    rows = 46;
    cols = 160;

    char **map = malloc(rows * sizeof(char *));

    for (int i = 0; i < rows; i++)
    {
        map[i] = malloc(cols * sizeof(char));
    }

    do
    {
        c = salas(c, rows, cols, map);
    } while (c != 27); // 27 'ESC'

    refresh();
    getch();
    endwin();

    for (int i = 0; i < rows; i++)
    {
        free(map[i]);
    }
    free(map);

    return 0;
}
