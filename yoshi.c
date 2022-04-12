/* This files provides address values that exist in the system */

#define SDRAM_BASE            0xC0000000
#define FPGA_ONCHIP_BASE      0xC8000000
#define FPGA_CHAR_BASE        0xC9000000

/* Cyclone V FPGA devices */
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050
#define TIMER_BASE            0xFF202000
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030

/* VGA colors */
#define WHITE 0xFFFF
#define YELLOW 0xFFE0
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define GREY 0xC618
#define PINK 0xFC18
#define ORANGE 0xFC00

#define ABS(x) (((x) > 0) ? (x) : -(x))

/* Screen size. */
#define RESOLUTION_X 320
#define RESOLUTION_Y 240


#define FALSE 0
#define TRUE 1

#include <stdlib.h>
#include <stdio.h>
//function declaration
void clear_screen();
void draw_line(int x0, int x1, int y0, int y1, int color);
void swap(int *a, int *b);
void plot_pixel(int x, int y, short int line_color);
int abs(int x);
void wait_for_vsync();

//game functions
void draw_yoshi(int yoshi_position[]);
void draw_bowser(int bowser_position[]);
void game_over(int status);
int crash_checker(int yoshi_position[], int bowser_position[]);

volatile int pixel_buffer_start; // global variable

int main(void)
{
    //buffer code
	volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    // declare other variables(not shown)
    // initialize location and direction of rectangles(not shown)

    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the 
                                        // back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer
    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    clear_screen(); // pixel_buffer_start points to the pixel buffer

	//GAME CODE
	volatile int *pushbutton = 0xFF200050;
	
	//yoshi attributes
	int base_y = 119;
	int dy = 0; // -5 for up, 5 for down
	int yoshi_position[] = {50,119};
	
	//bowser attributes
	int base_b = 0;
	int dx = -5;
	int bowser_position[] = {320,144};
	
	//othr variables
	int key;
	int start = 1;
	int game_over_status = 0; //0=game still going, -1=game lost, 1=game won
	
	while(1){ 
		clear_screen(); //erase the back buffer first
		
		//**FOR BACKGROUND**//
		//draw_line(x1, y1, x2, y2, color);
		draw_line(0, 170, 340, 170, ORANGE);   // for ground
		
		//**FOR YOSHI**//
		//if yoshi has no y-increment:check the keys
		if(yoshi_position[1] == base_y){ //if yoshi currently not jmupin
			key = *pushbutton; //poll the keys
			if(key!=0){ //if key has been pressed
				dy = -5; //start incrementing up	
				key =0; //reset pushbutton
			}	
		}
		yoshi_position[1]+=dy;
		
		//draw yoshi
		draw_yoshi(yoshi_position);
		
		//check and switch the direction of yoshi
		if(yoshi_position[1] == 69){ //if y-increment 119-50=69
			dy = 5; //nextmove down
		}else if(yoshi_position[1] == base_y){ //if y-increment is 0
			dy = 0;//finished jumping
		}
		
		//**FOR BOWSER**//
		draw_bowser(bowser_position);
		
		if(bowser_position[0] == -25){bowser_position[0] = 320;}
		
		//**CHECK IF CRASHED**//
		int game_over_status = crash_checker(yoshi_position, bowser_position);
		
		if(game_over_status !=0){game_over(game_over_status);}
		
		
		//**INCREMENT BOTH DRAWINGS**//
		bowser_position[0]+=dx;
		
		
		//wait and swap buffers
		wait_for_vsync(); // swap front and back buffers on VGA vertical sync
		pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer

	}   
}

void game_over(int status){
	clear_screen();
	
}


//Check if Crashed
int crash_checker(int yoshi_position[], int bowser_position[]){
	//lower right corner of yoshi
	int x_yoshi = yoshi_position[0] +50;
	int y_yoshi = yoshi_position[1] +50;
	int yoshi_size =50;
	
	//upper left corner of yoshi
	int x_bowser = bowser_position[0];
	int y_bowser = bowser_position[1];
	int bowser_size = 25;
	
	//conditions of crashing
	if ((y_yoshi>=y_bowser) && (x_bowser<=x_yoshi) &&(x_bowser>=yoshi_position[0]-bowser_size)){
		draw_line(0, 50, 50, 50, RED);   // for ground
		return -1; //crashed
		
	}
	return 0; //didnt crash
	
}

//Drawing yoshi
void draw_yoshi(int yoshi_position[]){
	//drawning
	int i;
	int j;
	for(i=0;i<=50;i++){
		for(j=0;j<=50;j++){	
			//Step1: draw yoshi
			plot_pixel(yoshi_position[0] + i, yoshi_position[1]+ j, PINK);
			
		}
	}
}
//Drawing bowser
void draw_bowser(int bowser_position[]){
	//drawning
	int i;
	int j;
	for(i=0;i<=25;i++){
		for(j=0;j<=25;j++){	
			//Step1: draw yoshi
			plot_pixel(bowser_position[0] + i, bowser_position[1]+ j, GREEN);
			
		}
	}
}





void wait_for_vsync(){
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    register int status;

    *pixel_ctrl_ptr = 1;
	status = *(pixel_ctrl_ptr+3); //+3 = 0xc
    
	while((status & 0x01)!=0){
		status = *(pixel_ctrl_ptr+3);
	}
    return;
}  

//draw line code function
void draw_line(int x0, int y0, int x1, int y1, int color){
	int abs_y = abs(y1 - y0);
	int abs_x = abs(x1 - x0);
	int is_steep;
	if(abs_y>abs_x){
		is_steep = 1;
	}else{
		is_steep = 0;
	}
		
	if (is_steep){
		swap(&x0,&y0);
		swap(&x1,&y1);
	}
	if (x0>x1){
		swap(&x0,&x1);
		swap(&y0,&y1);
	}
	int deltax = x1 - x0;
	int deltay = abs(y1 - y0);
	int error = -(deltax / 2);
	int y = y0;
	int y_step;
	if (y0 < y1){
		y_step = 1; 
	}else{
		y_step = -1;
	}
	
	//drawing line
	for (int x=x0; x<=x1; x++){
		if(is_steep){
			plot_pixel(y,x, color);
		}else{
			plot_pixel(x,y, color);
		}
		error = error + deltay;
		if(error>0){
			y = y + y_step;
			error = error - deltax;
		}
	}
}

//clear_screen function
void clear_screen(){
	for(int x = 0; x <= 319; x++){
		for(int y = 0; y <= 239; y++){
			plot_pixel(x,y,0);
		}
	}
}

//swap function
void swap(int *a, int *b){
	int temp = *a;
	*a = *b;
	*b=temp;
}

//absolutve value function
int abs(int x){
	if (x < 0) {
        x = (-1) * x;
    }
	return x;
}

//plot pixel function
void plot_pixel(int x, int y, short int line_color){
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

