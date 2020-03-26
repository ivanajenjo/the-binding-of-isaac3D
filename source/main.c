// Simple citro2d sprite drawing example
// Images borrowed from:
//   https://kenney.nl/assets/space-shooter-redux
#include <citro2d.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240
#define CHARACTER_SPEED 8
#define CHARACTER_HP 10

// Simple sprite struct
typedef struct
{
	C2D_Sprite spr;
	float dx, dy; // velocity
} Sprite;

static C2D_SpriteSheet spriteSheet;
static Sprite mainCharacter;
static Sprite background;
static int currentSpeed;
static int currentHp;

static void initBackground(){
	C2D_SpriteFromSheet(&background.spr, spriteSheet, 1);
	C2D_SpriteSetCenter(&background.spr, 0.5f, 0.5f);
	C2D_SpriteSetPos(&background.spr, 0.5f, 0.5f);
	C2D_SpriteSetRotation(&background.spr, C3D_Angle(0));
	C2D_SpriteSetDepth(&background.spr, 0.1f);
}

//---------------------------------------------------------------------------------
static void initSprites() {
//---------------------------------------------------------------------------------
	C2D_SpriteFromSheet(&mainCharacter.spr, spriteSheet, 0);
	C2D_SpriteSetCenter(&mainCharacter.spr, 0.5f, 0.5f);
	C2D_SpriteSetPos(&mainCharacter.spr, rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
	C2D_SpriteSetRotation(&mainCharacter.spr, C3D_Angle(rand()/(float)RAND_MAX));
	C2D_SpriteSetDepth(&mainCharacter.spr, 0.3f);
}

static void initCharacter(){
	currentSpeed = CHARACTER_SPEED;
	currentHp = CHARACTER_HP;
}

//---------------------------------------------------------------------------------
static void moveSprites() {
//---------------------------------------------------------------------------------

}

static void moveUp(){
	if(!(mainCharacter.spr.params.pos.y<=0)){
		mainCharacter.spr.params.pos.y = mainCharacter.spr.params.pos.y - currentSpeed;
	}
}

static void moveDown(){
	if(!(mainCharacter.spr.params.pos.y>=SCREEN_HEIGHT)){
		mainCharacter.spr.params.pos.y = mainCharacter.spr.params.pos.y + currentSpeed;
	}
}

static void moveRight(){
	if(!(mainCharacter.spr.params.pos.x>=SCREEN_WIDTH)){
		mainCharacter.spr.params.pos.x = mainCharacter.spr.params.pos.x + currentSpeed;
	}
}

static void moveLeft(){
	if(!(mainCharacter.spr.params.pos.x<=0)){
		mainCharacter.spr.params.pos.x = mainCharacter.spr.params.pos.x - currentSpeed;
	}
}
//Logica del disparo del personaje
static void shootUp(){
	C2D_SpriteSetRotation(&mainCharacter.spr, C3D_Angle(0.0f));
}

static void shootDown(){
	C2D_SpriteSetRotation(&mainCharacter.spr, C3D_Angle(0.5f));
}

static void shootRight(){
	C2D_SpriteSetRotation(&mainCharacter.spr, C3D_Angle(0.25f));
}

static void shootLeft(){
	C2D_SpriteSetRotation(&mainCharacter.spr, C3D_Angle(0.75f));
}

//---------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
//---------------------------------------------------------------------------------
	// Init libs
	romfsInit();
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	consoleInit(GFX_BOTTOM, NULL);

	// Create screens
	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

	// Load graphics
	spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	if (!spriteSheet) svcBreak(USERBREAK_PANIC);

	// Initialize background
	initBackground();

	// Initialize sprites
	initSprites();

	printf("\x1b[8;1HPress Up to increment sprites");
	printf("\x1b[9;1HPress Down to decrement sprites");

	//iniciar Stats Personaje
	initCharacter();

	// Main loop
	while (aptMainLoop())
	{
		hidScanInput();

		// Respond to user input
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START){
			break; // break in order to return to hbmenu
		}
			
		u32 kHeld = hidKeysHeld();

		//Programar movimiento
		if (kHeld & KEY_UP){
			moveUp();
		}

		if (kHeld & KEY_DOWN){
			moveDown();
		}

		if (kHeld & KEY_RIGHT){
			moveRight();
		}

		if (kHeld & KEY_LEFT){
			moveLeft();
		}

		//Programar Disparos
		if (kHeld & KEY_A){
			shootRight();
		}

		if (kHeld & KEY_Y){
			shootLeft();
		}

		if (kHeld & KEY_X){
			shootUp();
		}

		if (kHeld & KEY_B){
			shootDown();
		}

		//moveSprites();

		printf("The Binding of Aivan");
		printf("\x1b[2;1HCPU:     %6.2f%%\x1b[K", C3D_GetProcessingTime()*6.0f);
		printf("\x1b[3;1HGPU:     %6.2f%%\x1b[K", C3D_GetDrawingTime()*6.0f);
		printf("\x1b[4;1HCmdBuf:  %6.2f%%\x1b[K", C3D_GetCmdBufUsage()*100.0f);

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f));
		C2D_SceneBegin(top);
		C2D_DrawSprite(&background.spr);
		C2D_DrawSprite(&mainCharacter.spr);
		C3D_FrameEnd(0);
	}

	// Delete graphics
	C2D_SpriteSheetFree(spriteSheet);

	// Deinit libs
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	romfsExit();
	return 0;
}
