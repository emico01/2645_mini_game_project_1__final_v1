/*      C++ Plane Game
*    ==========================
*
*    Function:               In the game, the user controls a square, and if the plane target catches up with the square 
                                or the square collides with a wall at a set amount, the user is killed and the game is over.

*    Required Libraries:     Joystick : https://github.com/ELECXJEL2645/Joystick
*                            N5110    : https://github.com/ELECXJEL2645/N5110
*
*    Authored by:            Emico Leon
*    Date:                   2023
*    Version:                1.0
*    Revision Date:          05/2023 
*    MBED Studio Version:    1.4.1
*    MBED OS Version:        6.12.0
*    Board:	                 NUCLEO L476RG*/

#include "mbed.h"
#include "Joystick.h" 
#include "N5110.h"

//Connections to the Board
//                  y     x
Joystick joystick(PC_3, PC_2);  //attach and create joystick object

//Pin assignment format:  lcd(IO, Ser_TX, Ser_RX, MOSI, SCLK, PWM)  
N5110 lcd(PC_7, PA_9, PB_10, PB_5, PB_3, PA_10);

DigitalIn joystick_button(PC_10);
InterruptIn pause_button(PC_11);
PwmOut buzzer(PA_15);

//#####################
//Initialisation variables

float y_pos = 15; //user(x,y)
float x_pos = 4 ;
float xp_pos = 42; //particle(x,y)
float yp_pos = 23;
float px_pos = 42; //plane(x,y)
float py_pos = 23;

int level = 2; //set initial level of the game

int timer = 0;
int game_timer = 0; // game timer to store final count

int rec = 0;
int collision = 1; 
int collision_counter = 0;
int mode = 0;

//######################
int user_collision = 1; //user input variables
int user_difficulty = 3;

volatile int g_buttonA_flag = 0; //To enable Pause functions

//#####################
//declaration of the plane graphics
const int PlaneN[13][13]={
{ 0,0,0,0,0,0,1,0,0,0,0,0,0, },
{ 0,0,0,0,0,1,0,1,0,0,0,0,0, },
{ 0,0,0,0,0,1,0,1,0,0,0,0,0, },
{ 0,0,0,0,1,0,0,0,1,0,0,0,0, },
{ 0,0,0,1,0,0,0,0,0,1,0,0,0, },
{ 0,0,1,0,0,1,0,1,0,0,1,0,0, },
{ 0,1,0,0,1,1,0,1,1,0,0,1,0, },
{ 1,0,1,1,0,1,0,1,0,1,1,0,1, },
{ 1,1,1,0,0,1,0,1,0,0,0,1,1, },
{ 0,0,0,0,1,0,0,0,1,0,0,0,0, },
{ 0,0,0,0,0,0,1,0,0,0,0,0,0, },
{ 0,0,0,1,0,1,0,1,0,1,0,0,0, },
{ 0,0,0,1,1,0,0,0,1,1,0,0,0, },
};

const int PlaneS[13][13]={
{ 0,0,0,1,1,0,0,0,1,1,0,0,0, },
{ 0,0,0,1,0,1,0,1,0,1,0,0,0, },
{ 0,0,0,0,0,0,1,0,0,0,0,0,0, },
{ 0,0,0,0,1,0,0,0,1,0,0,0,0, },
{ 1,1,1,0,0,1,0,1,0,0,0,1,1, },
{ 1,0,1,1,0,1,0,1,0,1,1,0,1, },
{ 0,1,0,0,1,1,0,1,1,0,0,1,0, },
{ 0,0,1,0,0,1,0,1,0,0,1,0,0, },
{ 0,0,0,1,0,0,0,0,0,1,0,0,0, },
{ 0,0,0,0,1,0,0,0,1,0,0,0,0, },
{ 0,0,0,0,0,1,0,1,0,0,0,0,0, },
{ 0,0,0,0,0,1,0,1,0,0,0,0,0, },
{ 0,0,0,0,0,0,1,0,0,0,0,0,0, },
};

const int PlaneW[13][13]={
{ 0,0,0,0,0,0,0,1,1,0,0,0,0, },
{ 0,0,0,0,0,0,1,0,1,0,0,0,0, },
{ 0,0,0,0,0,1,0,1,0,0,0,0,0, },
{ 0,0,0,0,1,0,0,1,0,0,0,1,1, },
{ 0,0,0,1,0,0,1,0,0,1,0,0,1, },
{ 0,1,1,0,0,1,1,1,1,0,0,1,0, },
{ 1,0,0,0,0,0,0,0,0,0,1,0,0, },
{ 0,1,1,0,0,1,1,1,1,0,0,1,0, },
{ 0,0,0,1,0,0,1,0,0,1,0,0,1, },
{ 0,0,0,0,1,0,0,1,0,0,0,1,1, },
{ 0,0,0,0,0,1,0,1,0,0,0,0,0, },
{ 0,0,0,0,0,0,1,0,1,0,0,0,0, },
{ 0,0,0,0,0,0,0,1,1,0,0,0,0, },
};

const int PlaneE[13][13]={
{ 0,0,0,0,1,1,0,0,0,0,0,0,0, },
{ 0,0,0,0,1,0,1,0,0,0,0,0,0, },
{ 0,0,0,0,1,1,0,1,0,0,0,0,0, },
{ 1,1,0,0,0,1,0,0,1,0,0,0,0, },
{ 1,0,0,1,0,0,1,0,0,1,0,0,0, },
{ 0,1,0,0,1,1,1,1,0,0,1,1,0, },
{ 0,0,1,0,0,0,0,0,0,0,0,0,1, },
{ 0,1,0,0,1,1,1,1,0,0,1,1,0, },
{ 1,0,0,1,0,0,1,0,0,1,0,0,0, },
{ 1,1,0,0,0,1,0,0,1,0,0,0,0, },
{ 0,0,0,0,1,1,0,1,0,0,0,0,0, },
{ 0,0,0,0,1,0,1,0,0,0,0,0,0, },
{ 0,0,0,0,1,1,0,0,0,0,0,0,0, },
};

const int PlaneNW[11][11]={
{ 1,1,0,0,0,0,0,0,0,0,0, },
{ 0,0,1,0,0,0,0,0,0,0,0, },
{ 1,0,0,1,1,1,1,1,1,1,1, },
{ 0,1,0,0,0,0,0,0,0,0,1, },
{ 0,1,0,0,0,1,1,1,1,1,0, },
{ 0,1,0,1,0,0,1,0,0,0,0, },
{ 0,1,0,1,1,0,0,1,1,1,0, },
{ 0,1,0,1,0,1,0,0,0,0,1, },
{ 0,1,0,1,0,1,0,1,1,1,1, },
{ 0,1,0,1,0,1,0,1,0,0,0, },
{ 0,1,1,0,0,0,1,1,0,0,0, },
};

const int PlaneSW[11][11]={
{ 0,1,1,0,0,0,1,1,0,0,0, },
{ 0,1,0,1,0,1,0,1,0,0,0, },
{ 0,1,0,1,0,1,0,1,1,1,1, },
{ 0,1,0,1,0,1,0,0,0,0,1, },
{ 0,1,0,1,1,0,0,1,1,1,0, },
{ 0,1,0,1,0,0,1,0,0,0,0, },
{ 0,1,0,0,0,1,1,1,1,1,0, },
{ 0,1,0,0,0,0,0,0,0,0,1, },
{ 1,0,0,1,1,1,1,1,1,1,1, },
{ 0,0,1,0,0,0,0,0,0,0,0, },
{ 1,1,0,0,0,0,0,0,0,0,0, },
};

const int PlaneNE[11][11]={
{ 0,0,0,0,0,0,0,0,0,1,1, },
{ 0,0,0,0,0,0,0,0,1,0,0, },
{ 1,1,1,1,1,1,1,1,0,0,1, },
{ 1,0,0,0,0,0,0,0,0,1,0, },
{ 0,1,1,1,1,1,0,0,0,1,0, },
{ 0,0,0,0,1,0,0,1,0,1,0, },
{ 0,1,1,1,0,0,1,1,0,1,0, },
{ 1,0,0,0,0,1,0,1,0,1,0, },
{ 1,1,1,1,0,1,0,1,0,1,0, },
{ 0,0,0,1,0,1,0,1,0,1,0, },
{ 0,0,0,1,1,0,0,0,1,1,0, },
};

const int PlaneSE[11][11]={
{ 0,0,0,1,1,0,0,0,1,1,0, },
{ 0,0,0,1,0,1,0,1,0,1,0, },
{ 1,1,1,1,0,1,0,1,0,1,0, },
{ 1,0,0,0,0,1,0,1,0,1,0, },
{ 0,1,1,1,0,0,1,1,0,1,0, },
{ 0,0,0,0,1,0,0,1,0,1,0, },
{ 0,1,1,1,1,1,0,0,0,1,0, },
{ 1,0,0,0,0,0,0,0,0,1,0, },
{ 1,1,1,1,1,1,1,1,0,0,1, },
{ 0,0,0,0,0,0,0,0,1,0,0, },
{ 0,0,0,0,0,0,0,0,0,1,1, },
};

 const int Intro[27][73]{
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,1,0,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,1,1,1,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
    { 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,},
    { 1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,},
    { 0,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,0,1,0,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,0,},
    { 0,0,0,1,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,1,0,0,0,},
    { 0,0,0,0,0,0,0,0,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,0,0,0,0,0,0,0,0,0,},
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,1,0,0,1,0,0,0,1,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,1,0,0,1,0,0,1,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,1,0,1,0,0,0,0,1,0,1,0,0,0,0,0,1,0,1,1,1,0,1,0,0,0,0,0,1,0,1,0,0,0,1,0,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,1,1,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0,1,1,0,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
    { 0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,},
    { 0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,},
    { 0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,},
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,0,0,0,0,1,0,0,0,0,0,1,1,1,1,1,0,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,1,0,0,0,1,1,1,1,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
    };


// Declaration of the subroutines
void collision_level1(int x_pos,int y_pos);
void collision_level2(int x_pos,int y_pos);
void collision_level3(int x_pos,int y_pos);
void collision_level4(int x_pos,int y_pos);

void draw_level1();
void draw_level2();
void draw_level3();
void draw_level4();

void play_note(int selection);
void play_sound(int freq);

void particle(int x_pos,int y);
void plane_controller(int x_pos,int y);
void get_joystick_direction();
void game_commander();
void graphics_handler();

void introduction();
void menu();
void game_over();
void pause_button_isr();

int main(){

    joystick.init();
    lcd.init(LPH7366_1);        //initialise for LPH7366-1 LCD (Options are LPH7366_1 and LPH7366_6)
    lcd.setContrast(0.55);      //set contrast to 55%
    lcd.setBrightness(0.5);     //set brightness to 50% (utilises the PWM)

    joystick_button.mode(PullUp); //initialise the buttons
    pause_button.mode(PullUp);

    introduction();
    ThisThread::sleep_for(500ms);
    menu();
    
    while (!(collision_counter==user_collision)) {
        lcd.clear();

        pause_button.fall(&pause_button_isr); //pause the game
       
        if (g_buttonA_flag) {
            menu();
            g_buttonA_flag = 0;
        }

        lcd.drawRect(0,0,84,48,FILL_TRANSPARENT); //draws screen boarder
        
        game_commander(); //draws the graphics
        graphics_handler();
        get_joystick_direction();
        particle(x_pos,y_pos);
        plane_controller(xp_pos,yp_pos);
     
        game_timer++;    //increment the timer
        lcd.refresh();
        ThisThread::sleep_for(30ms);

        if ((x_pos==xp_pos)&&(y_pos==yp_pos)){ //automatically kills the user when the target reaches the user
            collision_counter=user_collision; 
        }
    }
    game_over(); 
}

void particle(int x,int y){
   if ((timer%user_difficulty)==0){

    if((y > yp_pos) && (x > xp_pos)){
            yp_pos++;
            xp_pos++;

        }else if((y < yp_pos) && (x < xp_pos)){
            yp_pos--;
            xp_pos--;

        }else if((y < yp_pos) && (x > xp_pos)){
            yp_pos--;
            xp_pos++;

        }else if((y > yp_pos) && (x < xp_pos)){
            yp_pos++;
            xp_pos--;
            
        }else if((y == yp_pos) && (x < xp_pos)){
            xp_pos--;
        
        }else if((y == yp_pos) && (x > xp_pos)){
            xp_pos++;

        }else if((y > yp_pos) && (x == xp_pos)){
            yp_pos++;

        }else if((y < yp_pos) && (x == xp_pos)){
            yp_pos--;
        }
   }

   timer++;

   if (timer==100){timer=0;} //stops large value to be stored in timer
}

void plane_controller(int x,int y){

   if ((timer%(user_difficulty + 1))==0){
        if((y > py_pos) && (x > px_pos)){
            py_pos++;px_pos++; mode=1;
            
        }else if((y < py_pos) && (x < px_pos)){
            py_pos--; px_pos--; mode=2;

        }else if((y < py_pos) && (x > px_pos)){
            py_pos--; px_pos++; mode=3;

        }else if((y > py_pos) && (x < px_pos)){
            py_pos++; px_pos--; mode=4;
            
        }else if((y == py_pos) && (x < px_pos)){
            px_pos--;mode=5;
        
        }else if((y == py_pos) && (x > px_pos)){
            px_pos++;mode=6;

        }else if((y > py_pos) && (x == px_pos)){
            py_pos++; mode=7;

        }else if((y < py_pos) && (x == px_pos)){
            py_pos--; mode=8;
        }
   }
}

void graphics_handler(){
    if (mode==1){
        lcd.drawSprite(px_pos-11,py_pos-11, 11, 11, (int*)PlaneSE);
    }else if(mode==2) {
        lcd.drawSprite(px_pos+1,py_pos+1, 11, 11, (int*)PlaneNW);
    }else if(mode==3){
        lcd.drawSprite(px_pos-11,py_pos+1, 11, 11, (int*)PlaneNE);
    }else if(mode==4){
        lcd.drawSprite(px_pos+1,py_pos-11, 11, 11, (int*)PlaneSW);
    }else if(mode==5){
        lcd.drawSprite(px_pos,py_pos-5, 13, 13, (int*)PlaneW);
    }else if(mode==6){
        lcd.drawSprite(px_pos-13,py_pos-5, 13, 13, (int*)PlaneE);
    }else if(mode==7){
        lcd.drawSprite(px_pos-6,py_pos-12, 13, 13, (int*)PlaneS);
    }else if(mode==8){
        lcd.drawSprite(px_pos-5,py_pos+3, 13, 13, (int*)PlaneN);
    }

    lcd.drawRect(x_pos,y_pos,2,2,FILL_BLACK);   //draws point at position (x,y)
    
    //draw strike point
    lcd.drawCircle(xp_pos,yp_pos,2,FILL_TRANSPARENT);
    lcd.drawRect(xp_pos,yp_pos-1,1,3,FILL_BLACK);
    lcd.drawRect(xp_pos-1,yp_pos,3,1,FILL_BLACK);
}

void get_joystick_direction(){
     
     if(joystick.get_direction() == N){
            y_pos--;

        }else if(joystick.get_direction() == S){
            y_pos++;
        
        }else if(joystick.get_direction() == E){
            x_pos++;

        }else if(joystick.get_direction() == W){
            x_pos--;

        }else if(joystick.get_direction() == NE){
            y_pos--;
            x_pos++;

        }else if(joystick.get_direction() == NW){
            y_pos--;
            x_pos--;

        }else if(joystick.get_direction() == SE){
            y_pos++;
            x_pos++;

        }else if(joystick.get_direction() == SW){
            y_pos++;
            x_pos--;
        } else {}
}

void draw_level1(){
     //rec1
    lcd.drawRect(0,18,27,9, FILL_TRANSPARENT);
     //rec2
    lcd.drawRect(18,10,9,9, FILL_TRANSPARENT); 
     //rec3
    lcd.drawRect(18,2,54,9, FILL_TRANSPARENT);
     //rec4
    lcd.drawRect(63,10,9,27, FILL_TRANSPARENT);
     //rec5
    lcd.drawRect(54,36,18,9, FILL_TRANSPARENT); 
     //rec6
    lcd.drawRect(45,32,10,13, FILL_TRANSPARENT);
     //rec7
    lcd.drawRect(28,32,18,9, FILL_TRANSPARENT); 
     //rec8
    lcd.drawRect(28,40,9,8, FILL_TRANSPARENT);  

     //clear intersection rec1 & rec2
    lcd.drawRect(19,18,7,1,FILL_WHITE);
     //clear intersection rec3 & rec2
    lcd.drawRect(19,10,7,1,FILL_WHITE);    
     //clear intersection rec3 & rec4
    lcd.drawRect(64,10,7,1,FILL_WHITE);
    //clear intersection rec4 & rec5
    lcd.drawRect(64,36,7,1,FILL_WHITE);
    //clear intersection rec6 & rec5
    lcd.drawRect(54,37,1,7,FILL_WHITE);
    //clear intersection rec7 & rec6
    lcd.drawRect(45,33,1,7,FILL_WHITE);
    //clear intersection rec8 & rec7
    lcd.drawRect(29,40,7,1,FILL_WHITE);
}

void collision_level1(int x,int y){
        //identifying rec 1   
    if ((x > 0) && (x < 27) && (y > 18) && (y < 27)){rec=1;}
        //collision detection for rec 1
    if (rec==1){
        if (x < 0){x_pos = 84; level=4;}
        else if (x > 25){x_pos = 25; collision++;}
        if (y < 18){y_pos = 18; collision++;}
        else if (y > 25){y_pos = 25; collision++;}
    }
        //identifying rec 2   
    if ((x >= 17) && (x <= 26) && (y >= 10) && (y <= 17)){rec=2;}
         //collision detection for rec 2     
    if (rec==2){
        if (x < 18){x_pos = 18; collision++;}
        else if (x > 25){x_pos = 25; collision++;}
        if (y < 10){y_pos = 10;}
        else if (y > 18){y_pos = 18;}
    }
        //identifying rec 3   
    if ((x >= 17) && (x < 72) && (y > 2) && (y < 10)){rec=3;}  
        //collision detection for rec 3
    if (rec==3){
        if (x < 18){x_pos = 18; collision++;}
        else if (x > 70){x_pos = 70; collision++;}
        if (y < 2){y_pos = 2; collision++;}
        else if (y > 9){y_pos = 9; collision++;}
    }
        //identifying rec 4  
    if ((x > 62) && (x < 72) && (y >=10) && (y < 37)){rec=4;}
        //collision detection for rec 4
    if (rec==4){
        if (x < 63){x_pos = 63; collision++;}
        else if (x > 70){x_pos = 70; collision++;}
        if (y < 10){y_pos = 10;}
        else if (y > 36){y_pos = 36;}
    }
        //identifying rec 5  
    if ((x > 53) && (x < 72) && (y >= 36) && (y < 45)){rec=5;}
        //collision detection for rec 5       
    if (rec==5){
        if (x < 54){x_pos = 54;}
        else if (x > 70){x_pos = 70; collision++;}
        if (y < 36){y_pos = 36; collision++;}
        else if (y > 43){y_pos = 43; collision++;}
    }
        //identifying rec 6  
    if ((x > 44) && (x < 54) && (y > 30) && (y < 45)){rec=6;}
        //collision detection for rec 6      
    if (rec==6){
        if (x < 45){x_pos = 45 ; collision++;}
        else if (x > 53){x_pos = 53; collision++;}
        if (y < 32){y_pos = 32; collision++;}
        else if (y > 43){y_pos = 43; collision++;}
    }
        //identifying rec 7  
    if ((x >=27) && (x < 45) && (y > 30) && (y < 40)){rec=7;}
        //collision detection for rec 7      
    if (rec==7){
        if (x < 28){x_pos = 28; collision++;}
        else if (x > 44){x_pos = 44;}
        if (y < 32){y_pos = 32; collision++;}
        else if (y > 39){y_pos = 39; collision++;}
    }
        //identifying rec 8  
    if ((x >= 27) && (x < 36) && (y >=39) && (y < 49)){rec=8;}
        //collision detection for rec 8      
    if (rec==8){
        if (x < 28){x_pos = 28 ; collision++;}
        else if (x > 35){x_pos = 35; collision++;}
        if (y < 39){y_pos = 39;}
        else if (y > 46){y_pos = 0; level=2;}
    }
}

void draw_level2(){
     //rec1
    lcd.drawRect(0,10,17,9, FILL_TRANSPARENT);
     //rec2
    lcd.drawRect(8,18,9,19, FILL_TRANSPARENT); 
     //rec3
    lcd.drawRect(8,36,55,9, FILL_TRANSPARENT);
     //rec4
    lcd.drawRect(62,21,9,24, FILL_TRANSPARENT); 
     //rec5
    lcd.drawRect(53,13,18,9, FILL_TRANSPARENT);
     //rec6
    lcd.drawRect(45,13,9,14, FILL_TRANSPARENT);
     //rec7
    lcd.drawRect(36,18,10,9, FILL_TRANSPARENT); 
     //rec8
    lcd.drawRect(28,0,9,27, FILL_TRANSPARENT);
    
     //clear intersection rec1 & rec2
    lcd.drawRect(9,18,7,1,FILL_WHITE);
     //clear intersection rec3 & rec2
     lcd.drawRect(9,36,7,1,FILL_WHITE);  
     //clear intersection rec3 & rec4
    lcd.drawRect(62,37,1,7,FILL_WHITE);
     //clear intersection rec4 & rec5
    lcd.drawRect(63,21,7,1,FILL_WHITE);
     //clear intersection rec6 & rec5
    lcd.drawRect(53,14,1,7,FILL_WHITE);
     //clear intersection rec7 & rec6
    lcd.drawRect(45,19,1,7,FILL_WHITE); 
     //clear intersection rec8 & rec7
    lcd.drawRect(36,19,1,7,FILL_WHITE);   
}

void collision_level2(int x,int y){
     //identifying rec 8 
    if ((x >=27 ) && (x < 36) && (y >= 0) && (y <= 26)){rec=8;}
     //collision detection for rec 8
    if (rec==8){
        if (x < 28){x_pos = 28 ; collision++;}
        else if (x > 35){x_pos = 35; collision++;}
        if (y < 0){y_pos = 45;level=1;}
        else if (y > 25){y_pos = 25; collision++;}
    }
     //identifying rec 7 
    if ((x >= 36) && (x < 45) && (y >= 18) && (y <= 26)){rec=7;}
     //collision detection for rec 7       
    if (rec==7){
        if (x < 35){x_pos = 35;}
        else if (x > 45){x_pos = 45;}
        if (y < 18){y_pos = 18; collision++;}
        else if (y > 25){y_pos = 25; collision++;}
    }
     //identifying rec 6 
    if ((x >= 45) && (x < 53) && (y >= 12) && (y <= 26)){rec=6;}
     //collision detection for rec 6      
    if (rec==6){
        if (x < 45){x_pos = 45; collision++;}
        else if (x > 52){x_pos = 52; collision++;}
        if (y < 13){y_pos = 13; collision++;}
        else if (y > 25){y_pos = 25; collision++;}
    }
     //identifying rec 5   
    if ((x >= 53) && (x < 71) && (y >=13) && (y < 21)){rec=5;}
     //collision detection for rec 5
    if (rec==5){
        if (x < 52){x_pos = 52;}
        else if (x > 69){x_pos = 69; collision++;}
        if (y < 13){y_pos = 13; collision++;}
        else if (y > 20){y_pos = 20; collision++;}
    }
     //identifying rec 4 
    if ((x >= 62) && (x < 71) && (y >= 20) && (y < 45)){rec=4;}
     //collision detection for rec 4       
    if (rec==4){
        if (x < 62){x_pos = 62; collision++;}
        else if (x > 69){x_pos = 69; collision++;}
        if (y < 20){y_pos = 20;}
        else if (y > 43){y_pos = 43; collision++;}
    }
     //identifying rec 3 
    if ((x >= 7) && (x < 62) && (y >= 36) && (y < 45)){rec=3;}
     //collision detection for rec 3       
    if (rec==3){
        if (x < 8){x_pos = 8; collision++;}
        else if (x > 62){x_pos = 62;}
        if (y < 36){y_pos = 36; collision++;}
        else if (y > 43){y_pos = 43; collision++;}
    }
     //identifying rec 2 
    if ((x >=7) && (x <= 16) && (y >=17 ) && (y <= 36)){rec=2;}
     //collision detection for rec 2      
    if (rec==2){
        if (x < 8){x_pos = 8; collision++;}
        else if (x > 15){x_pos = 15; collision++;}
        if (y < 17){y_pos = 17;}
        else if (y > 36){y_pos = 36;}
    }
     //identifying rec 1 
    if ((x >= 0) && (x <= 16) && (y >=9) && (y <= 17)){rec=1;}
     //collision detection for rec 1       
    if (rec==1){
        if (x < 0){x_pos = 83; level=3;}
        else if (x > 15){x_pos = 15; collision++;}
        if (y < 10){y_pos = 10; collision++;}
        else if (y > 17){y_pos = 17; collision++;}
    }
}

void draw_level3(){
    //rec8
    lcd.drawRect(48,0,9,20, FILL_TRANSPARENT);
     //rec5
    lcd.drawRect(12,19,45,9, FILL_TRANSPARENT);
     //rec4
    lcd.drawRect(12,27,9,10, FILL_TRANSPARENT);
     //rec3
    lcd.drawRect(12,36,63,9, FILL_TRANSPARENT);
     //rec2
    lcd.drawRect(66,18,9,19, FILL_TRANSPARENT); 
    //rec1
    lcd.drawRect(66,10,18,9, FILL_TRANSPARENT);
    
     //clear intersection rec8 & rec5
    lcd.drawRect(49,19,7,1,FILL_WHITE);
     //clear intersection rec5 & rec4
    lcd.drawRect(13,27,7,1,FILL_WHITE);    
     //clear intersection rec4 & rec3
    lcd.drawRect(13,36,7,1,FILL_WHITE);
    //clear intersection rec3 & rec2
    lcd.drawRect(67,36,7,1,FILL_WHITE);
    //clear intersection rec2 & rec1
    lcd.drawRect(67,18,7,1,FILL_WHITE);
    
}

void collision_level3(int x,int y){
     //identifying rec 8     
    if ((x >=48 ) && (x < 57) && (y >= 0) && (y <= 19)){rec=8;}
     //collision detection for rec 8 
    if (rec==8){
        if (x < 49){x_pos = 49; collision++;}
        else if (x > 55){x_pos = 55; collision++;}
        if (y < 0){y_pos = 47; level=4;}
        else if (y > 19){y_pos = 19;}
    }
     //identifying rec 5 
    if ((x >= 11) && (x < 57) && (y >= 19) && (y <= 26)){rec=5;}
     //collision detection for rec 5        
    if (rec==5){
        if (x < 12){x_pos = 12; collision++;}
        else if (x > 55){x_pos = 55; collision++;}
        if (y < 19){y_pos = 19; collision++;}
        else if (y > 26){y_pos = 26; collision++;}
    }
     //identifying rec 4 
    if ((x >= 11) && (x < 20) && (y >= 26) && (y <= 36)){rec=4;}
     //collision detection for rec 4        
    if (rec==4){
        if (x < 12){x_pos = 12; collision++;}
        else if (x > 19){x_pos = 19; collision++;}
        if (y < 26){y_pos = 26;}
        else if (y > 36){y_pos = 36;}
    }
     //identifying rec 3    
    if ((x >= 11) && (x < 75) && (y >=36) && (y < 45)){rec=3;}
     //collision detection for rec 3 
    if (rec==3){
        if (x < 12){x_pos = 12; collision++;}
        else if (x > 73){x_pos = 73; collision++;}
        if (y < 36){y_pos = 36; collision++;}
        else if (y > 43){y_pos = 43; collision++;}
    }
     //identifying rec 2 
    if ((x >= 65) && (x < 75) && (y >= 17) && (y <= 36)){rec=2;}
     //collision detection for rec 2        
    if (rec==2){
        if (x < 66){x_pos = 66; collision++;}
        else if (x > 73){x_pos = 73; collision++;}
        if (y < 17){y_pos = 17;}
        else if (y > 36){y_pos = 36;}
    }
     //identifying rec 1 
    if ((x >= 65) && (x < 84) && (y >= 10) && (y < 17)){rec=1;}
     //collision detection for rec 1        
    if (rec==1){
        if (x < 66){x_pos = 66; collision++;}
        else if (x > 84){x_pos = 0; level=2;}
        if (y < 10){y_pos = 10; collision++;}
        else if (y > 17){y_pos = 17; collision++;}
    }

}

void draw_level4(){
    //rec1
    lcd.drawRect(66,18,18,9, FILL_TRANSPARENT);
     //rec2
    lcd.drawRect(66,10,9,9, FILL_TRANSPARENT);
     //rec3
    lcd.drawRect(12,2,63,9, FILL_TRANSPARENT);
     //rec4
    lcd.drawRect(12,10,9,17, FILL_TRANSPARENT); 
     //rec5
    lcd.drawRect(12,26,19,9, FILL_TRANSPARENT);
    //rec6
    lcd.drawRect(30,18,9,17, FILL_TRANSPARENT);
     //rec7
    lcd.drawRect(38,18,19,9, FILL_TRANSPARENT);
    //rec8
    lcd.drawRect(48,26,9,24, FILL_TRANSPARENT);
    
    //clear intersection rec1 & rec2
    lcd.drawRect(67,18,7,1,FILL_WHITE);
     //clear intersection rec3 & rec2
    lcd.drawRect(67,10,7,1,FILL_WHITE);    
     //clear intersection rec3 & rec4
    lcd.drawRect(13,10,7,1,FILL_WHITE);
    //clear intersection rec4 & rec5
    lcd.drawRect(13,26,7,1,FILL_WHITE);
    //clear intersection rec6 & rec5
    lcd.drawRect(30,27,1,7,FILL_WHITE);
    //clear intersection rec7 & rec6
    lcd.drawRect(38,19,1,7,FILL_WHITE);
    //clear intersection rec8 & rec7
    lcd.drawRect(49,26,7,1,FILL_WHITE);
}

void collision_level4(int x,int y){
     //identifying rec 1    
    if ((x >=65 ) && (x < 84) && (y >= 18) && (y <= 26)){rec=1;}
     //collision detection for rec 1    
    if (rec==1){
        if (x < 66){x_pos = 66; collision++;}
        else if (x > 84){x_pos = 0;level=1;}
        if (y < 18){y_pos = 18; collision++;}
        else if (y > 25){y_pos = 25; collision++;}
    }
     //identifying rec 2  
    if ((x >= 65) && (x < 75) && (y >= 9) && (y <= 18)){rec=2;}
     //collision detection for rec 2        
    if (rec==2){
        if (x < 66){x_pos = 66; collision++;}
        else if (x > 73){x_pos = 73; collision++;}
        if (y < 9){y_pos = 9;}
        else if (y > 18){y_pos = 18;}
    }
     //identifying rec 3  
    if ((x >= 11) && (x < 75) && (y >= 2) && (y <= 9)){rec=3;}
     //collision detection for rec 3       
    if (rec==3){
        if (x < 12){x_pos = 12; collision++;}
        else if (x > 73){x_pos = 73; collision++;}
        if (y < 2){y_pos = 2; collision++;}
        else if (y > 9){y_pos = 9; collision++;}
    }
     //identifying rec 4  
    if ((x >= 11) && (x < 20) && (y >=9) && (y <= 26)){rec=4;}
     //collision detection for rec 4   
    if (rec==4){
        if (x < 12){x_pos = 12; collision++;}
        else if (x > 19){x_pos = 19; collision++;}
        if (y < 9){y_pos = 9;}
        else if (y > 26){y_pos = 26;}
    }
     //identifying rec 5  
    if ((x >= 11) && (x <= 29) && (y >= 26) && (y <= 34)){rec=5;}
     //collision detection for rec 5        
    if (rec==5){
        if (x < 12){x_pos = 12; collision++;}
        else if (x > 30){x_pos = 30;}
        if (y < 26){y_pos = 26; collision++;}
        else if (y > 33){y_pos = 33; collision++;}
    }
     //identifying rec 6  
    if ((x > 29) && (x <= 36) && (y >= 18) && (y <= 34)){rec=6;}
     //collision detection for rec 6        
    if (rec==6){
        if (x < 30){x_pos = 30; collision++;}
        else if (x > 37){x_pos = 37; collision++;}
        if (y < 18){y_pos = 18; collision++;}
        else if (y > 33){y_pos = 33; collision++;}
    }
     //identifying rec 7  
    if ((x > 36) && (x <= 56) && (y >= 18) && (y <= 25)){rec=7;}
     //collision detection for rec 7        
    if (rec==7){
        if (x < 37){x_pos = 37;}
        else if (x > 55){x_pos = 55; collision++;}
        if (y < 18){y_pos = 18; collision++;}
        else if (y > 25){y_pos = 25; collision++;}
    }
     //identifying rec 8  
    if ((x > 47) && (x <= 56) && (y >= 25) && (y <= 48)){rec=8;}
     //collision detection for rec 8        
    if (rec==8){
        if (x < 48){x_pos = 48; collision++;}
        else if (x > 55){x_pos = 55; collision++;}
        if (y < 25){y_pos = 25;}
        else if (y > 47){y_pos = 0;level=3;}
    }
}

void game_commander(){
   
    char buffer[14];

    if ((collision%10)==0){
        collision_counter++;
        collision=1;
    }
    
    sprintf(buffer,"%d",collision_counter); // converts the value of collision

    if (level==1){
        draw_level1();
        collision_level1(x_pos,y_pos);
        lcd.printString(buffer,76,1);}
    if (level==2){
        draw_level2();
        collision_level2(x_pos,y_pos);
        lcd.printString(buffer,76,1);}
    if (level==3){
        draw_level3();
        collision_level3(x_pos,y_pos);
        lcd.printString(buffer,4,1);}
    if (level==4){
        draw_level4();
        collision_level4(x_pos,y_pos);
        lcd.printString(buffer,4,1);
    }   
}

void introduction(){
    int timer_intro = 1;
    int cover = 1;
    play_note(1);

    while (!(joystick_button==0)){
        lcd.clear();
        lcd.drawSprite(6,2,27, 73, (int*)Intro);
        lcd.printString("Press button",7,4);
        lcd.drawRect(0,0,84,48,FILL_TRANSPARENT);
        
        if(timer_intro%2==1){cover++;}
        if(cover>5){cover=1;}

        if (timer_intro%2==0){lcd.drawRect(6+(10*cover),41,4,4,FILL_BLACK);}

        lcd.refresh();
        ThisThread::sleep_for(150ms);
        
        timer_intro++;
        if(timer_intro>1000){timer_intro=1;}
    }   
}

void game_over(){
    char bufferN[14];
    int total_time = 0;
    total_time = game_timer*0.03;
    sprintf(bufferN,"%ds",total_time);
    
    lcd.clear();
    lcd.printString("Game-Over", 12, 1);
    lcd.printString("Game-Time:", 12, 2);
    lcd.printString(bufferN,29, 3);
    
    lcd.refresh();

    play_note(2);
    ThisThread::sleep_for(1000ms);
    
}

void play_note(int selection){
    if (selection==1){
        const int C_major_scale[] = {110,117,123,349,185,196,494,247};
        for(int i = 0; i < 8; i++){play_sound(C_major_scale[i]);}}

    else { 
        const int C_major_scale[] = {3136,3322,3520,3729,3951,4186,4435,4699};
        for(int i = 0; i < 8; i++){play_sound(C_major_scale[i]);}
    }

    buzzer.pulsewidth_us(0); //turn off buzzer
}

void play_sound(int freq){
    buzzer.period_us((float) 1000000.0f/ (float) freq);    //set the period of the pwm signal (in us)
    buzzer.pulsewidth_us(buzzer.read_period_us()/2);       //set pulse width of the pwm to 1/2 the period
    ThisThread::sleep_for(200ms);                          //play sound for 500ms
}

void menu(){
    //Buffers needed
    char collision_buffer[14];
    char difficulty[14];
    char brightness_buffer[14];

    //Initialisation of the parameters
    int collision_init = user_collision;
    int difficulty_init = user_difficulty;
    int brightness_init = 5;
    int mode_selection = 1;

    int state = 1; //used for toggling
 
    while (!(joystick_button==0)){

        lcd.clear();
    
        sprintf(collision_buffer,"%d",collision_init);
        sprintf(difficulty,"%d",difficulty_init);
        sprintf(brightness_buffer,"%d", brightness_init);

        lcd.printString(collision_buffer,71,2);
        lcd.printString(difficulty,71,3);
        lcd.printString(brightness_buffer,71,4);

        lcd.printString("Menu",26,0);
        lcd.printString("Collisions:",5,2);
        lcd.printString("Dificulty:",5,3);
        lcd.printString("Brightness:",5,4);
            
        lcd.drawRect(0,0,84,48,FILL_TRANSPARENT);
            
        state= !state;
        if (state==0){lcd.drawRect(78,8+mode_selection*8,1,7,FILL_TRANSPARENT);}

        if(joystick.get_direction()==N){mode_selection--;}
        if(joystick.get_direction()==S){mode_selection++;}

        if(mode_selection<1){mode_selection=3;}
        else if (mode_selection>3){mode_selection=1;}

        //######################################################

        if(mode_selection==1 && joystick.get_direction()==E){
            collision_init++;
            if (collision_init>6){collision_init=1;}
        }else if(mode_selection==1 && joystick.get_direction()==W){
            collision_init--;
            if (collision_init<1){collision_init=6;}
        }

        //########################################################

        if(mode_selection==2 && joystick.get_direction()==E){
            difficulty_init++;
            if (difficulty_init>3){difficulty_init=1;}
        }else if(mode_selection==2 && joystick.get_direction()==W){
            difficulty_init--;
            if (difficulty_init<1){difficulty_init=3;}
        }

        //########################################################

        if(mode_selection==3 && joystick.get_direction()==E){
            brightness_init++;
            if (brightness_init>9){brightness_init=1;}
        }else if(mode_selection==3 && joystick.get_direction()==W){
            brightness_init--;
            if (brightness_init<1){brightness_init=9;}
        }

        lcd.setBrightness(brightness_init*0.1); //see the brightness in real time
        
        lcd.refresh();
        ThisThread::sleep_for(200ms);
    }  
    //values are passed
    user_difficulty = difficulty_init;
    user_collision = collision_init;

    //resets the game
    collision_counter = 0;
    game_timer = 0;
}

void pause_button_isr(){
     g_buttonA_flag = 1;
}
