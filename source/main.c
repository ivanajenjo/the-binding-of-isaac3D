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
#define INIT_CHARACTER_SPEED 2
#define INIT_CHARACTER_HP 10
#define INIT_ENEMY_HP 10
#define MAX_ENEMIES 20

// Simple sprite struct
typedef struct
{
	C2D_Sprite spr;
	float dx, dy; // velocity
	int enemyHp;
	int characterHp;
} Sprite;

typedef struct
{
	C2D_Sprite head;
	C2D_Sprite body;
	int posx, posy; // velocity
	int characterHp, characterSpeed;
} Isaac;

static C2D_SpriteSheet spriteSheet;
static C2D_SpriteSheet enemiesSpriteSheet;
static C2D_SpriteSheet isaacSheet;
static Sprite isaacSprites[16];
static Isaac mainIsaac;
static Sprite mainCharacter;
static Sprite background;
static Sprite enemies[MAX_ENEMIES];
static size_t numEnemies = MAX_ENEMIES/2;
static int contadorDisparo = 0;
static bool disparando = false;

//---------------------------------------------------------------------------------
static void initBackground(){
//---------------------------------------------------------------------------------
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
	mainCharacter.characterHp = INIT_CHARACTER_HP;
}

//---------------------------------------------------------------------------------
static void initEnemies(){
//---------------------------------------------------------------------------------
	size_t numImages = C2D_SpriteSheetCount(enemiesSpriteSheet);

	for (size_t i = 0; i < MAX_ENEMIES; i++)
	{
		Sprite* sprite = &enemies[i];
		C2D_SpriteFromSheet(&sprite->spr, enemiesSpriteSheet, rand() % numImages);
		C2D_SpriteSetCenter(&sprite->spr, 0.5f, 0.5f);
		C2D_SpriteSetPos(&sprite->spr, rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
		C2D_SpriteSetRotation(&sprite->spr, C3D_Angle(rand()/(float)RAND_MAX));
		C2D_SpriteSetDepth(&sprite->spr, 0.3f);
		sprite->dx = rand()*4.0f/RAND_MAX - 2.0f;
		sprite->dy = rand()*4.0f/RAND_MAX - 2.0f;
		sprite->enemyHp = INIT_ENEMY_HP;
	}
}

static void initIsaacSprites(){
	size_t numImages = C2D_SpriteSheetCount(isaacSheet);

	for (size_t i = 0; i < numImages; i++)
	{
		Sprite* sprite = &isaacSprites[i];
		C2D_SpriteFromSheet(&sprite->spr, isaacSheet, i);
		C2D_SpriteSetCenter(&sprite->spr, 0.5f, 0.5f);
		C2D_SpriteSetPos(&sprite->spr, rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
		C2D_SpriteSetDepth(&sprite->spr, 0.3f);
		sprite->enemyHp = INIT_ENEMY_HP;
	}
}

static void isaacSpritePos(){
	mainIsaac.head.params.pos.x = mainIsaac.posx;
	mainIsaac.head.params.pos.y = mainIsaac.posy;
	mainIsaac.body.params.pos.x = mainIsaac.posx;
	mainIsaac.body.params.pos.y = mainIsaac.posy+15;
}

static void initCharacter(){
	mainIsaac.posx = 50;
	mainIsaac.posy = 50;
	mainIsaac.head = isaacSprites[0].spr;
	mainIsaac.body = isaacSprites[4].spr;
	isaacSpritePos();
	mainIsaac.characterSpeed = INIT_CHARACTER_SPEED;
	mainIsaac.characterHp = INIT_CHARACTER_HP;
}

//---------------------------------------------------------------------------------
static void moveSprites() {
//---------------------------------------------------------------------------------

}

static void moveUp(){
	if(!(mainIsaac.posy<=0)){
		mainIsaac.posy = mainIsaac.posy - mainIsaac.characterSpeed;
	}
}

static void moveDown(){
	if(!(mainIsaac.posy>=SCREEN_HEIGHT)){
		mainIsaac.posy = mainIsaac.posy + mainIsaac.characterSpeed;
	}
}

static void moveRight(){
	if(!(mainIsaac.posx>=SCREEN_WIDTH)){
		mainIsaac.posx = mainIsaac.posx + mainIsaac.characterSpeed;
	}
}

static void moveLeft(){
	if(!(mainIsaac.posx<=0)){
		mainIsaac.posx = mainIsaac.posx - mainIsaac.characterSpeed;
	}
	disparando = false;
}
//Logica del disparo del personaje
static void shootUp(){
	mainIsaac.head = isaacSprites[2].spr;
	disparando = true;
}

static void shootDown(){
	mainIsaac.head = isaacSprites[0].spr;
}

static void shootRight(){
	mainIsaac.head = isaacSprites[1].spr;
}

static void shootLeft(){
	mainIsaac.head = isaacSprites[3].spr;
}

static void drawEnemies(){
	// Render Enemies
	for (size_t i = 0; i < numEnemies; i++)
	{
		C2D_DrawSprite(&enemies[i].spr);
	}
}

static void drawIsaac(){
	C2D_DrawSprite(&mainIsaac.body);
	C2D_DrawSprite(&mainIsaac.head);
}

static void drawShooting(){
	isaacSprites[12].spr.params.pos.x = 300;
	isaacSprites[12].spr.params.pos.y = 300;
	C2D_DrawSprite(&isaacSprites[12].spr);
}

static void drawScene(){
	//Draw Background
	C2D_DrawSprite(&background.spr);
	C2D_DrawSprite(&mainCharacter.spr);
	drawIsaac();
	if (disparando == true)
	{
		drawShooting();
	}
	
	//drawEnemies();
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
	isaacSheet = C2D_SpriteSheetLoad("romfs:/gfx/isaac.t3x");
	if (!isaacSheet) svcBreak(USERBREAK_PANIC);

	spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	if (!spriteSheet) svcBreak(USERBREAK_PANIC);

	enemiesSpriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/enemiesSprites.t3x");
	if (!enemiesSpriteSheet) svcBreak(USERBREAK_PANIC);

	// Initialize background
	initBackground();

	// Initialize enemies
	initEnemies();

	// Initialize sprites
	initSprites();
	initIsaacSprites();

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
		isaacSpritePos();

		printf("The Binding of Ivan");
		printf("\x1b[2;1HCPU:     %6.2f%%\x1b[K", C3D_GetProcessingTime()*6.0f);
		printf("\x1b[3;1HGPU:     %6.2f%%\x1b[K", C3D_GetDrawingTime()*6.0f);
		printf("\x1b[4;1HCmdBuf:  %6.2f%%\x1b[K", C3D_GetCmdBufUsage()*100.0f);

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f));
		C2D_SceneBegin(top);
		//draw Scene Logic
		drawScene();
		
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
