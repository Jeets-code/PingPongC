/**
   Ball module source code
   @group 101
   @author Jared Jones Pankhurst and Manjeet Panwar
   @date 06-oct-2020
   @brief This module provides the ball functionality to the game by
    using the provided API module tinygl.h and system.h @author  M. P. Hayes, UCECE.
    Module includes methods to initialise and move ball.
*/


#include "system.h"
#include "tinygl.h"
#include "ball.h"


//sets the initial amount the coordinates of the ball will increment by
int increment_x = 1;
int increment_y = 1;

//Creates an instance of the ball, then returns it
tinygl_point_t ball_init(int x, int y)
{
    tinygl_point_t ball = tinygl_point(x, y);
    return ball;
}


/*Fully controls the movement of the ball, with logic on what to do when at an edge.
 * The ball position is updated, then the ball is returned.
 * The underlying idea is inspired by the bounce2.c file in the apps directory*/
tinygl_point_t move_ball(tinygl_point_t ball, int x, int y, int row_indices[])
{
    //Moves the x and y coordinates of the ball by the increment values
    if (x == 3 && (y == 6 || y == 0)) {
        x += increment_x;
        ball.x = x;
        ball.y = y;
        return ball;
    } else if (x != 4 && (y >= row_indices[0] || y <= row_indices[2])) {
        x += increment_x;
        y += increment_y;
    }


    //if the ball is in the 5th column and hit the paddle, or at the 1st column
    if ((x == 4 && (y >= row_indices[0] && y <= row_indices[2]))) {
        x -= increment_x * 2;
        increment_x = -increment_x;
    //if the ball missed the paddle
    //The ball will stay at those coordinates
    } else if ((x == 4 && (y < row_indices[0] || y > row_indices[2])) || x < 0) {

        if (x < 0) {
            increment_x = -increment_x;
        }
        x = x;
        y = y;
        ball.x = x;
        ball.y = y;
        return ball;
    }

    //checks if the ball is at an edge
    //Because of conditionals above, we don't need to check if the ball is in the same column as the paddle.
    if (y > 6 || y < 0) {
        y -= increment_y * 2;
        increment_y = -increment_y;
    }

    ball.x = x;
    ball.y = y;
    return ball;
}
