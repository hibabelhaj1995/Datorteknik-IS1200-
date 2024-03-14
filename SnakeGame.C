#include <pic32mx.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

volatile int* portE = (volatile int*) 0xbf886110;    //Pekare för port E
int random_counter;
char direction = 'r';
int snake_x = 30;
int snake_y = 15;
int apple_x = 75;
int apple_y = 15;
int length = 5;
int randx;
int randy;
int score;

void playZone()
{
      int x,y;
       for(x = 0; x < 128; x++)
       {
        for(y = 0; y < 32; y++)
            grid[x][y] = 0;                      //Rensa skärm
       }
    int i,j;
    for(i = 0; i < 128; i++)
    {
        for(j = 0; j < 32; j++)                //Sätta på gränser
        {
            grid[0][j] = 1;
            grid[127][j] = 1;                        
        }
        grid[i][0]  = 1;                       
        grid[i][31] = 1;                                 
    }
}

void skapa_apple()
{
    grid[apple_x][apple_y] = 1;
    grid[apple_x-1][apple_y] = 1;
    grid[apple_x][apple_y-1] = 1;
    grid[apple_x-1][apple_y-1] = 1;
}
void random_apple ()                      //counter för random apple (Delen för random äpple tog vi från en annan students lab och det finns även på internet)
{
    if(IFS(0) & 0x1000)
    {
        random_counter++;
                                      //  rensar flagan
        IFSCLR(0) = 0x1000;
    }
     if(random_counter == 1)
     {

         randx = TMR3 % 123 + 4;
         randy = TMR3 % 26 + 4;
         random_counter = 0;
    }
}

void snake_creation()             
{    
    int snake[500][2];
    int length = 5;
    int Position = 15;
    int i;
    for(i = 0; i < length; i++)                //Snake strukturen och position
    {
        snake[i][0] = 10;                      // X koordinat
        snake[i][1] = Position;                // Y koordinat 
        Position++;
    }
    
    for(i = 0; i < length; i++)                //Rita snake
    {
      int  x = snake[i][0];
      int  y = snake[i][1];
      grid[x][y] = 1;   
        
    }

}

void snake_move()                                  // definerar rörelse av snake , då hela kroppen ska följa med
{
     int k = length-1;
     int i,j;
    for( i = 1; i < length ; i++)
    {
        for( j = 0; j < 2; j++)
        {
            snake[k][j] = snake[k-1][j];
        }
            k--;
    }
   snake[0][0] = snake_x;
   snake[0][1] = snake_y;

    for(i = 0; i < length; i++)
    {
         int x = snake[i][0];
         int y = snake[i][1];
        grid[x][y] = 1;
    }
}
void snake_direction()                   // definiera riktingarna beroende på knapparna
{
    
    int b1 = btn1();               // BTN 1
    int btn = getbtns();           // BTN 2 3 4
    
    if((direction =='r') || (direction =='l')){
        if(btn==1){
            direction = 'd';
            delay(100000);
        }
        else if (btn == 2){
            direction = 'u';
            delay(100000);
        }
    }
   
    if((direction =='u')|| (direction =='d')){
        if( b1){
            direction = 'r';
            delay(200000);
        }
        else if(btn == 4){
            direction = 'l';
            delay(200000);
        }
    }

    if ( direction == 'r')       snake_x += 1;
    
    if( direction == 'l')        snake_x -= 1;
    
    if( direction == 'u')        snake_y += 1;

    if( direction == 'd')        snake_y -= 1;
}
void eat_apple()
{
     if(snake_x == apple_x && snake_y == apple_y || snake_x == apple_x-1 && snake_y == apple_y
    || snake_x == apple_x && snake_y == apple_y-1 || snake_x == apple_x-1 && snake_y == apple_y-1)
    {
        grid[apple_x][apple_y] = 0;
        grid[apple_x-1][apple_y] = 0;
        grid[apple_x][apple_y-1] = 0;
        grid[apple_x-1][apple_y-1] = 0;
        apple_x = randx;
        apple_y = randy;
        length++;                 
        *portE +=1;               
        score++;
    }
}

void restart()
{
    // display dessa stringar
    char score2[] = "M";
    score2[0] = score + '0' ;
    display_string(1, "GAME OVER");
    display_string(2, "SCORE");
    display_string(3, score2);
    
    update_string();                             // display uppdatera
    delay(10000000);
    playZone(); 
    *portE = 0;                                 // portE nollställs                         
    score = 0;                                  // score nollställs
    
    // Snake görs om
   int i;
   for( i = 0; i < length; i++)          
     {
         snake[i][0] = 0;
         snake[i][1] = 0;
     }

    snake_x = 20;
    snake_y= 20;
    length = 5;
    update_grid();
}
void collision()
{
    int i;
    for( i = 0 ; i < length; i++)
    {
       if(snake[i][0] == snake_x && snake[i][1] == snake_y)                       //krocka med sig själv
        restart();
    }
    
     if((snake_x == 127)|| (snake_x == 0) ||(snake_y == 0) || (snake_y == 31))    // Krockar med en av vägarna
        restart();
}

// alla funktioner som ska anropas när spelet starta
void game (){
    playZone();
    eat_apple();
    snake_direction();
    skapa_apple();
    collision();
    snake_move();
    random_apple ();
    update_grid();
}
