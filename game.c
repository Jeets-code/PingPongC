/**
   Game module source code
   @group 101
   @author Jared Jones Pankhurst and Manjeet Panwar
   @date 05-oct-2020
   @brief This module is the main game module which allows 2 players to
    play the game by using provided API modules author @ @author  M. P. Hayes, UCECE
    and standard C library.
*/

#include "system.h"
#include "pio.h"
#include "tinygl.h"
#include "pacer.h"
#include "navswitch.h"
#include "../fonts/font5x7_1.h"
#include "ball.h"
#include "ir_uart.h"
#include "led.h"
#include <string.h>
#include <stdlib.h>

//Global variable for the delay
//To give each task the same amount of time
//The value can be changed for what we need
#define PACER_RATE 500
#define MESSAGE_RATE 10

//Global variable for player score that will change as the game is played
//This is what will be displayed on the Led matrix after each round is played
//The game will end after a player reaches the score limit e.g. 7


/* The following two arrays are taken from lab2-ex5.c*/
//This may or may not be allowed
// --------------------------------------------------------
static const pio_t rows[] =
{
    LEDMAT_ROW1_PIO, LEDMAT_ROW2_PIO, LEDMAT_ROW3_PIO,
    LEDMAT_ROW4_PIO, LEDMAT_ROW5_PIO, LEDMAT_ROW6_PIO,
    LEDMAT_ROW7_PIO
};


static const pio_t cols[] =
{
    LEDMAT_COL1_PIO, LEDMAT_COL2_PIO, LEDMAT_COL3_PIO,
    LEDMAT_COL4_PIO, LEDMAT_COL5_PIO
};

// ---------------------------------------------------------



//function to display the paddle, given an array of the 3 rows that the paddle will use
//i.e. display_paddle({rows[2], rows[3], rows[4]}) (will display the paddle in the middle of the 5th column)
void display_paddle(int row_indices[])
{

    for (int col = 0; col < 4; col++) {
        pio_output_high(cols[col]);
    }

    //column 5 will always be set to low while each round is played
    pio_output_low(cols[4]);

    //Set desired rows to low
    //for each row, if it is in the display rows array: set to low
    //else set to high
    int iterator = 0;
    for (int row = 0; row < 7; row++) {
        if (row == row_indices[iterator]) {
            pio_output_low(rows[row]);
            iterator++;
        } else {
            pio_output_high(rows[row]);
        }


    }


}


/*If the ball has missed the paddle. It sends winner to the other player,
 * and displays loser on your display.*/
int check_loss(tinygl_point_t ball, int play_game, int row_indices[])
{
    play_game = 1;
    if (ball.x == 4 && (ball.y < row_indices[0] || ball.y > row_indices[2])) {
        tinygl_draw_point(ball, 0);
        ir_uart_putc('W');
        tinygl_text("L");
        play_game = 0;
    }

    return play_game;
}



/*If the fun kit receives the word "Winner", it will display it on the display*/
int check_win(int play_game, int y_value)
{
    play_game = 1;
    if (y_value == 'W') {
        tinygl_text("W");
        play_game = 0;

    }

    return play_game;
}




/*Main function that controls how the game runs.
 * Also includes sending and receiving of the ball.*/
int main (void)
{
    //paddle rows are initially the middle 3 rows before the player moves it
    int row_indices[] = {2, 3, 4};
    int inverted_rows[] = {6, 5, 4, 3, 2, 1, 0};

    //Initialising ball start position
    int ball_x = 1;
    int ball_y = 0;
    int play_game = 1;


    //Count for multiplexing.
    //i.e. to display paddle and ball but not other leds on the matrix
    int multi_count = 0;
    int counter = 0; //every 10 counts the ball will be displayed in a new position

    //Initializing the game modules
    system_init ();
    led_init();
    ir_uart_init();
    navswitch_init();
    tinygl_font_set (&font5x7_1);



    //Initialise the pins as outputs for the led matrix
    for (int row = 0; row < 7; row++) {
        pio_config_set(rows[row], PIO_OUTPUT_HIGH);
    }

    for (int col = 0; col < 5; col++) {
        pio_config_set(cols[col], PIO_OUTPUT_HIGH);
    }


    //Initialising the ball itself
    int use_ball = 0;
    int sent = 1;
    led_set(LED1, 1);


    tinygl_point_t ball = ball_init(ball_x, ball_y);

    //Initialises the delay time
    pacer_init (PACER_RATE);

    //Looping to run the game
    while (1)
    {
        pacer_wait ();
        //Call the navswitch update function.
        navswitch_update();

        //waits until navswitch is pushed before game starts
        if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
            use_ball = 1;
            sent = 0;
        }

        //Updating tinygl
        tinygl_update();

        // If play game i.e Neither player won or lost, run loop
        if (play_game == 1) {
            if (multi_count % 2 == 0) {

                //subtracts 1 of each index to send the paddle right
                if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
                    //Checks if the paddle is already at the furtherest right it can be
                    if (row_indices[0] == 0) {
                        continue;
                    } else {
                        for (int i = 0; i < 3; i++) {
                            row_indices[i]--;
                        }
                    }

                }

                //adds 1 to each index to send the paddle left
                if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {

                    //Checks if the paddle is already at the furtherest left it can be
                    if (row_indices[2] == 6) {
                        continue;
                    } else {
                        for (int i = 0; i < 3; i++) {
                            row_indices[i]++;
                        }
                    }


                }
                display_paddle(row_indices);
            } else {
                //displays the ball when multi_count = 1 mod 2


                if (ir_uart_read_ready_p() && sent == 1) {
                    ball.y = ir_uart_getc();
                    int y_value = ball.y;
                    play_game = check_win(play_game, y_value);
                    if (play_game == 1) {
                        if (ball.y < 10 || ball.y > -10) {
                            ball.y = inverted_rows[ball.y];
                            ball.x = 0;
                            use_ball = 1;
                            sent = 0;
                        }
                    }
                }

                /*draw ball on the board of the player that push the NAV
                 at the start of the game */
                if (use_ball == 1) {
                    counter++;

                    //turns the paddle column off so there isnt a fouth led on when the paddle is hard to a side

                    if (counter >= 100) {
                        pio_output_high(cols[4]);
                        counter = 0;
                        tinygl_draw_point(ball, 0);
                        tinygl_point_t previous = ball;
                        ball = move_ball(previous, previous.x, previous.y, row_indices);

                        if (ball.x < 0) {
                            ir_uart_putc(ball.y);
                            tinygl_draw_point(ball, 0);
                            use_ball = 0;
                            sent = 1;
                        }


                        if (use_ball == 1) {
                            tinygl_draw_point(ball, 1);
                            play_game = check_loss(ball, play_game, row_indices);
                        }
                    }
                }
            }
            multi_count++;
        }
    }
}
