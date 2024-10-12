unsigned long long __attribute__((used)) VGAaddress = 0xc8000000; // Memory storing pixels
unsigned long long __attribute__((used)) UARTaddress = 0xFF201000; // UART base address
unsigned int __attribute__((used)) red = 0x0000F0F0;
unsigned int __attribute__((used)) green = 0x00000F0F;
unsigned int __attribute__((used)) blue = 0x000000FF;
unsigned int __attribute__((used)) white = 0x0000FFFF;
unsigned int __attribute__((used)) black = 0x0;

#define N_COLS 16
#define N_ROWS 16

char *won = "You Won";       
char *lost = "You Lost";     
unsigned short height = 240; 
unsigned short width = 320;  
#define SCREEN_HEIGHT = 240
#define SCREEN_WIDTH = 320
char font8x8[128][8];        

typedef struct _block
{
    unsigned char destroyed;
    unsigned char deleted;
    unsigned int pos_x;
    unsigned int pos_y;
    unsigned int color;
} Block;

typedef enum _gameState
{
    Stopped = 0,
    Running = 1,
    Won = 2,
    Lost = 3,
    Exit = 4,
} GameState;

typedef struct _ball
{
	signed int x_pos;
	signed int y_pos;
	signed int x_vel;
	signed int y_vel;
} Ball;

// playing field
GameState currentState = Stopped;
Ball ball = {50, 120, -1, 0}; // starting position

// ball
Block playing_field_blocks[N_COLS*N_ROWS];
unsigned int playing_field_start = 320 - 1 - (N_COLS*15);
unsigned int ball_diameter = 7;
unsigned int ball_speed = 1;

// bar
signed int bar_y = 109 ;
unsigned int bar_width = 7;
unsigned int bar_height = 45;
unsigned int bar_movement = 15;


void ClearScreen();
void SetPixel(unsigned int x_coord, unsigned int y_coord, unsigned int color);
void DrawBlock(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned int color);
void DrawBar(unsigned int y);
int ReadUart();
int ClearUart();
void WriteUart(char c);


asm("ClearScreen: \n\t"
	"	push {r0, r1, r2, r3, r4, lr} \n\t"
	"	mov r0, #0 \n\t"
	"	mov r1, #0 \n\t"
	"	mov r2, #320 \n\t"
	"	mov r3, #240 \n\t"
	"	mov r4, #0xFFFF \n\t"
	"	push {r4} \n\t" // Push color value to stack
	"	bl DrawBlock \n\t"
	"	pop {r4} \n\t"
	"	pop {r0, r1, r2, r3, r4, lr} \n\t"
	"	bx lr \n\t");

// assumes R0 = x-coord, R1 = y-coord, R2 = colorvalue
asm("SetPixel: \n\t"
	"	ldr r3, =VGAaddress \n\t"
	"	ldr r3, [r3] \n\t"
	"	lsl r0, r0, #1 \n\t"
	"	lsl r1, r1, #10 \n\t"
	"	add r1, r1, r0 \n\t"
	"	strh r2, [r3, r1] \n\t"
	"	bx lr \n\t");

// assumes R0 = x-coord, R1 = y-coord, R2 = width, R3 = height, sp+4 = color
asm("DrawBlock: \n\t"
	"	push {r4, r5, r6, r7, r8, lr} \n\t"
	"	mov r7, r0 \n\t" // staring x 
	"	add r5, r0, r2 \n\t" // final x 
	"	add r6, r1, r3 \n\t" // final y 
	"	ldr r2, [sp, #24] \n\t"
	"DrawLoop: \n\t"
	"	push {r0, r1, r3, lr} \n\t"
	"	bl SetPixel \n\t"
	"	pop {r0, r1, r3, lr} \n\t"
	"	add r0, r0, #1 \n\t"
	"	cmp r0, r5 \n\t"
	"	bne DrawLoop \n\t" // don't loop if x value is final 
	"	mov r0, r7 \n\t" // reset x 
	"	add r1, r1, #1 \n\t"
	"	cmp r1, r6 \n\t"
	"	bne DrawLoop \n\t" // don't loop if y value is final 
	"	pop {r4, r5, r6, r7, r8, lr} \n\t"
	"	bx lr \n\t");

// assumes R0 = y-coord
asm("DrawBar: \n\t"
	"	push {r4, lr} \n\t"
	"	mov r1, r0 \n\t" // y is first input
	"	mov r0, #0 \n\t"
	"	mov r2, #7 \n\t"
	"	mov r3, #45 \n\t"
	"	mov r4, #0 \n\t" // set color
	"	push {r4} \n\t"
	"	bl DrawBlock \n\t"
	"	pop {r4} \n\t"
	"	pop {r4, lr} \n\t"
	"	bx lr \n\t");

asm("ReadUart: \n\t"
	"	ldr r1, =0xFF201000 \n\t" // TODO change to UARTaddress
	"	ldr r0, [r1] \n\t"
	"	bx lr \n\t");

asm("WriteUart: \n\t"
	"	push {lr} \n\t"
	"	pop {lr} \n\t"
	"	bx lr \n\t");


void draw_ball(unsigned int x_old, unsigned int y_old, unsigned int x_new, unsigned int y_new)
{
	DrawBlock(x_old, y_old, ball_diameter, ball_diameter, 0xFFFF);
	DrawBlock(x_new, y_new, ball_diameter, ball_diameter, 0);
}

void draw_bar(unsigned int y_old, unsigned int y_new)
{
	if (y_old != y_new){
		DrawBlock(0, y_old,	7, 45, 0xFFFF); // TODO: could be optimized
		DrawBar(y_new); // TODO: drawbar is redundant
	}
}

void initialize_playing_field()
{
	for (int i = 0; i < N_COLS*N_ROWS; i++){
		unsigned int pos_x = playing_field_start + 15 * (i % N_COLS);
		unsigned int pos_y = 15 * (i / N_COLS);

		unsigned int blue = 15 - 16 * (pos_y / (15 * N_ROWS));
		unsigned int red = 31 - 32 * (pos_x - playing_field_start) / (15 * N_COLS);
		unsigned int green = 63 - (32 * pos_y / (15 * N_ROWS));
		red = red << 11;
		green = green << 5;
		unsigned int color = red+green+blue;

		playing_field_blocks[i] = (Block) {'0', '0', pos_x, pos_y, color};
	}
}

void draw_playing_field()
{
	for (int i = 0; i < N_COLS*N_ROWS; i++){
		Block currentBlock = playing_field_blocks[i];
		if (currentBlock.destroyed == '0') {
			DrawBlock(currentBlock.pos_x, currentBlock.pos_y, 15, 15, currentBlock.color);
		}
	}
}

void hit_check_bar(unsigned int ball_y_center)
{
	ball.x_pos = bar_width;
	if (ball_y_center >= bar_y && ball_y_center < bar_y + 15) {
		ball.x_vel = -1 * ball.x_vel;
		ball.y_vel = -ball_speed;
	} else if (ball_y_center >= bar_y + 15 && ball_y_center < bar_y + 30) {
		ball.x_vel = -1 * ball.x_vel;
		ball.y_vel = 0;
	} else if (ball_y_center >= bar_y + 30 && ball_y_center < bar_y + 45) {
		ball.x_vel = -1 * ball.x_vel;
		ball.y_vel = ball_speed;
	} else {
		currentState = Lost;
	}
}

signed int absolute(signed int a)
{
	if (a < 0){
		a = -a;
	}
	return a;
}

void hit_check_playing_field(unsigned int ball_x_center, unsigned int ball_y_center)
{
	for (int i = 0; i < N_COLS*N_ROWS; i++){
		Block *currentBlock = &playing_field_blocks[i];
		signed int x_diff = absolute(ball.x_pos+3-(currentBlock->pos_x+7));
		signed int y_diff = absolute(ball.y_pos+3-(currentBlock->pos_y+7));
		if (currentBlock->destroyed == '0'){
			if (x_diff < 10 && y_diff < 10){
				currentBlock->destroyed = '1';
				DrawBlock(currentBlock->pos_x, currentBlock->pos_y, 15, 15, 0xFFFF);
				if (x_diff > y_diff){
					ball.x_vel = -ball.x_vel;
				} else if (y_diff > x_diff){
					ball.y_vel = -ball.y_vel;
				} else {
					ball.x_vel = -ball.x_vel;
					ball.y_vel = -ball.y_vel;
				}
				return; // prevents multiple blocks from being destroyed
			}
		}
	}
}

void update_game_state()
{
    if (currentState != Running)
    {
        return;
    }
	// update position
	ball.x_pos += ball.x_vel;
	ball.y_pos += ball.y_vel;
	// calculate center for use in collision detection
	unsigned int ball_x_center = ball.x_pos + 3;
	unsigned int ball_y_center = ball.y_pos + 3;
	// y bounds check 
	if (ball.y_pos <= 0){
		ball.y_pos = 0;
		ball.y_vel = -1 * ball.y_vel;
	} else if (ball.y_pos >= height - ball_diameter) {
		ball.y_pos = height - ball_diameter - 1;
		ball.y_vel = -1 * ball.y_vel;
	}
	// x bounds check 
	if (ball.x_pos <= bar_width){
		hit_check_bar(ball_y_center);
	}
	if (ball.x_pos + ball_diameter >= width){
		currentState = Won;
	}
	hit_check_playing_field(ball_x_center, ball_y_center);
}

char read_uart_top() {
    unsigned long long out;
    int remaining;
    do {
        out = ReadUart();
        if (!(out & 0x8000)) {
            return 0;
        }
        remaining = (out & 0xFF0000) >> 4;
    } while (remaining > 0);
    return out & 0xFF;
}

void update_bar_state()
{
	int key = read_uart_top();
	if (key == 0x77){
		bar_y -= bar_movement;
	} else if (key == 0x73){
		bar_y += bar_movement;
	}
	// check for out of bounds
	if (bar_y <= 0) {
		bar_y = 0;
	} else if (bar_y >= height - bar_height) {
		bar_y = height - bar_height - 1;
	}
}

void write(char *str)
{
    // TODO: Use WriteUart to write the string to JTAG UART
}

void play()
{
    ClearScreen();
	DrawBar(bar_y);
    while (1)
    {
		// keep previous values for resetting the colors (slightly inefficient to redraw)
		unsigned int ball_x_old = ball.x_pos;
		unsigned int ball_y_old = ball.y_pos;
		unsigned int bar_y_old = bar_y;
    	draw_playing_field();
        update_bar_state();
        update_game_state();
        if (currentState != Running)
        {
            break;
        }
        draw_ball(ball_x_old, ball_y_old, ball.x_pos, ball.y_pos);
        draw_bar(bar_y_old, bar_y);
    }
    if (currentState == Won)
    {
        write(won);
    }
    else if (currentState == Lost)
    {
        write(lost);
    }
    else if (currentState == Exit)
    {
        return;
    }
    currentState = Stopped;
}

void reset()
{
    int remaining = 0;
    do
    {
        unsigned long long out = ReadUart();
        if (!(out & 0x8000))
        {
            // not valid, abort reading
            return;
        }
        remaining = (out & 0xFF0000) >> 4;
    } while (remaining > 0);

	Ball ball = {50, 120, -1, 0}; // starting position
}

void wait_for_start()
{
    while (currentState == Stopped) {
        char key = read_uart_top();
        if (key == 0x77 || key == 0x73) {
            currentState = Running;
        }
    }
}

int main(int argc, char *argv[])
{
    ClearScreen();

    while (1)
    {
        wait_for_start();
		initialize_playing_field();
        play();
    	ClearScreen();
        reset();
        if (currentState == Exit)
        {
            break;
        }
    }
    return 0;
}
