/**
   Ball module header file
   @group 101
   @author Jared Jones Pankhurst and Manjeet Panwar
   @date 06-oct-2020
   @brief This header file provides the ball functionality to the other 
	modules.
*/


#ifndef BALL_H
#define BALL


//Defines the ball initialising function
tinygl_point_t ball_init(int x, int y);


//Defines the function that controls the movement of the ball
tinygl_point_t move_ball(tinygl_point_t ball, int x, int y, int row_indices[]);




#endif
