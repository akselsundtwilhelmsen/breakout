unsigned long long __attribute__((used)) VGAaddress = 0xc8000000; // Memory storing pixels
unsigned long long __attribute__((used)) UARTaddress = 0xFF201000; // Memory storing pixels
unsigned int __attribute__((used)) red = 0x0000F0F0;
unsigned int __attribute__((used)) green = 0x00000F0F;
unsigned int __attribute__((used)) blue = 0x000000FF;
unsigned int __attribute__((used)) white = 0x0000FFFF;
unsigned int __attribute__((used)) black = 0x0;

unsigned char n_cols = 10; // <- This variable might change depending on the size of the game. Supported value range: [1,18]

char *won = "You Won";       // DON'T TOUCH THIS - keep the string as is
char *lost = "You Lost";     // DON'T TOUCH THIS - keep the string as is
unsigned short height = 240; // DON'T TOUCH THIS - keep the value as is
unsigned short width = 320;  // DON'T TOUCH THIS - keep the value as is
char font8x8[128][8];        // DON'T TOUCH THIS - this is a forward declaration
/**************************************************************************************************/

/***
 * TODO: Define your variables below this comment
 */

/***
 * You might use and modify the struct/enum definitions below this comment
 */
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
GameState currentState = Stopped;

/***
 * Here follow the C declarations for our assembly functions
 */

// TODO: Add a C declaration for the ClearScreen assembly procedure
void ClearScreen();
void SetPixel(unsigned int x_coord, unsigned int y_coord, unsigned int color);
void DrawBlock(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned int color);
void DrawBar(unsigned int y);
int ReadUart();
void WriteUart(char c);

/***
 * Now follow the assembly implementations
 */

asm("ClearScreen: \n\t"
	"push {r0, r1, r2, r3, r4, r5, r6, lr} \n\t"
	"mov r0, #0 \n\t"
	"mov r1, #0 \n\t"
	"mov r2, #0x07E0 \n\t" // height * width
	"mov r3, #320 \n\t"
	"mov r4, #240 \n\t"
	"bl DrawBlock \n\t"
	"pop {r0, r1, r2, r3, r4, r5, r6, lr} \n\t"
	"bx lr \n\t");


// assumes R0 = x-coord, R1 = y-coord, R2 = colorvalue
asm("SetPixel: \n\t"
	"push {r0, r1, r2, r3, lr} \n\t"
	"//mov r0, #20 // (input) \n\t"
	"//mov r1, #30 // (input) \n\t"
	"//mov r2, #0xFFFFFFFF // (input) \n\t"
	"ldr r3, =VGAaddress \n\t"
	"ldr r3, [r3] \n\t"
	"lsl r1, r1, #10 \n\t"
	"lsl r0, r0, #1 \n\t"
	"add r1, r0 \n\t"
	"strh r2, [r3, r1] \n\t"
	"pop {r0, r1, r2, r3, lr} \n\t"
	"bx lr \n\t");


// TODO: Implement the DrawBlock function in assembly. You need to accept 5 parameters, as outlined in the c declaration above (unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned int color)
// assume R0 = x-coord, R1 = y-coord, R2 = colorvalue, R3 = width, R4 = height
asm("DrawBlock: \n\t"
	"	push {r0, r1, r2, r3, r4, r5, r6, lr} \n\t"
	"	//mov r0, #100 // x (input) \n\t"
	"	//mov r1, #40 // y (input) \n\t"
	"	//mov r2, #0xFFFFFFFF // color (input) \n\t"
	"	//mov r3, #120 // width (input) \n\t"
	"	//mov r4, #140 // height (input) \n\t"
	"	add r5, r0, r3 // final x \n\t"
	"	add r6, r1, r4 // final y \n\t"
	"DrawLoop: \n\t"
	"	bl SetPixel \n\t"
	"	add r0, r0, #1 \n\t"
	"	cmp r0, r5 \n\t"
	"	bne DrawLoop // don't loop if x value is final \n\t"
	"	subs r0, r0, r3 // reset x \n\t"
	"	add r1, r1, #1 \n\t"
	"	cmp r1, r6 \n\t"
	"	bne DrawLoop // don't loop if y value is final \n\t"
	"	pop {r0, r1, r2, r3, r4, r5, r6, lr} \n\t"
	"	bx lr \n\t");


// TODO: Impelement the DrawBar function in assembly. You need to accept the parameter as outlined in the c declaration above (unsigned int y)
// assumes R0 = y-coord
asm("DrawBar: \n\t"
	"	push {r0, r1, r2, r3, r4, lr} \n\t"
	"	mov r1, r0 // y is first input \n\t"
	"	mov r0, #0 \n\t"
	"	//mov r1, #120 // (input) \n\t"
	"	mov r2, #0xFFFFFFFF \n\t"
	"	mov r3, #7 \n\t"
	"	mov r4, #45 \n\t"
	"	bl DrawBlock \n\t"
	"	pop {r0, r1, r2, r3, r4, lr} \n\t"
	"	bx lr \n\t");

asm("ReadUart: \n\t"
	"	push {r0, r1, lr} \n\t"
	"	ldr r1, =UARTaddress \n\t"
	"	ldr r0, [r1] \n\t"
	"	pop {r0, r1, lr} \n\t"
	"	bx lr \n\t");

asm("WriteUart: \n\t"
	"	push {lr} \n\t"
	"	pop {lr} \n\t"
	"	bx lr \n\t");


// TODO: Add the WriteUart assembly procedure here that respects the WriteUart C declaration on line 46

// TODO: Implement the C functions below
void draw_ball()
{
}

void draw_playing_field()
{
}

void update_game_state()
{
    if (currentState != Running)
    {
        return;
    }

    // TODO: Check: game won? game lost?

    // TODO: Update balls position and direction

    // TODO: Hit Check with Blocks
    // HINT: try to only do this check when we potentially have a hit, as it is relatively expensive and can slow down game play a lot
}

void update_bar_state()
{
    int remaining = 0;
    // TODO: Read all chars in the UART Buffer and apply the respective bar position updates
    // HINT: w == 77, s == 73
    // HINT Format: 0x00 'Remaining Chars':2 'Ready 0x80':2 'Char 0xXX':2, sample: 0x00018077 (1 remaining character, buffer is ready, current character is 'w')
}

void write(char *str)
{
    // TODO: Use WriteUart to write the string to JTAG UART
}

void play()
{
    ClearScreen();
    // HINT: This is the main game loop
    while (1)
    {
        update_game_state();
        update_bar_state();
        if (currentState != Running)
        {
            break;
        }
        draw_playing_field();
        draw_ball();
        DrawBar(120); // TODO: replace the constant value with the current position of the bar
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
    // Hint: This is draining the UART buffer
    int remaining = 0;
    do
    {
        unsigned long long out = ReadUart();
        if (!(out & 0x8000))
        {
            // not valid - abort reading
            return;
        }
        remaining = (out & 0xFF0000) >> 4;
    } while (remaining > 0);

    // TODO: You might want to reset other state in here
}

void wait_for_start()
{
    // TODO: Implement waiting behaviour until the user presses either w/s
}

int main(int argc, char *argv[])
{
    ClearScreen();

    // HINT: This loop allows the user to restart the game after loosing/winning the previous game
    while (1)
    {
        wait_for_start();
        play();
        reset();
        if (currentState == Exit)
        {
            break;
        }
    }
    return 0;
}
