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
#define ANIMACION 5
#define MAX_DEATH_HEADS 12
#define CADENCIA_DEATH_HEAD 60

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
	bool visible;
} Isaac;

typedef struct
{
	C2D_Sprite spr;
	int posx, posy;
	int enemyHp;
	float dx, dy;
	bool visible;
} deathHead;


static C2D_SpriteSheet spriteSheet, enemiesSpriteSheet, isaacSheet, deathHeadSheet;
static Sprite deathHeadSprites[4];
static Sprite isaacSprites[18];
static Isaac mainIsaac;
static Sprite background;
static Sprite enemies[MAX_ENEMIES];
static size_t numEnemies = MAX_ENEMIES/2;
static int lastMove = 0;
static int contCaminar = 0;
static int contadorDeathHead = 0;
static int nDeathHeads = 0;
static deathHead deathHeads[MAX_DEATH_HEADS];

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

	numImages = C2D_SpriteSheetCount(deathHeadSheet);

	for (size_t i = 0; i < numImages; i++)
	{
		Sprite* sprite = &deathHeadSprites[i];
		C2D_SpriteFromSheet(&sprite->spr, deathHeadSheet, i);
		C2D_SpriteSetCenter(&sprite->spr, 0.5f, 0.5f);
		C2D_SpriteSetPos(&sprite->spr, rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
		C2D_SpriteSetDepth(&sprite->spr, 0.3f);
		sprite->enemyHp = INIT_ENEMY_HP;
		sprite->dx = rand()*4.0f/RAND_MAX - 2.0f;
		sprite->dy = rand()*4.0f/RAND_MAX - 2.0f;
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

static void deathHeadPos(deathHead *death){
	death->spr.params.pos.x = death->posx;
	death->spr.params.pos.y = death->posy;
}

static void isaacSpritePos(){
	mainIsaac.head.params.pos.x = mainIsaac.posx;
	mainIsaac.head.params.pos.y = mainIsaac.posy;
	mainIsaac.body.params.pos.x = mainIsaac.posx;
	mainIsaac.body.params.pos.y = mainIsaac.posy+15;
}

static void isaacStanding(){
	mainIsaac.head = isaacSprites[0].spr;
	mainIsaac.body = isaacSprites[4].spr;
}

static void initCharacter(){
	mainIsaac.posx = 50;
	mainIsaac.posy = 50;
	isaacStanding();
	isaacSpritePos();
	mainIsaac.characterSpeed = INIT_CHARACTER_SPEED;
	mainIsaac.characterHp = INIT_CHARACTER_HP;
}

static void initDeathHeads(){
	for (size_t i = 0; i < MAX_DEATH_HEADS; i++)
	{
		deathHeads[i].spr = deathHeadSprites[0].spr;
		deathHeads[i].posx = rand() % SCREEN_WIDTH;
		deathHeads[i].posy = rand() % SCREEN_HEIGHT;
		deathHeads[i].dx = rand()*4.0f/RAND_MAX - 2.0f;
		deathHeads[i].dy = rand()*4.0f/RAND_MAX - 2.0f;
		deathHeads[i].visible = true;
	}
}

static void moveDeathHeads(){
	for (size_t i = 0; i < MAX_DEATH_HEADS; i++)
	{
		deathHead* sprite = &deathHeads[i];
		C2D_SpriteMove(&sprite->spr, sprite->dx, sprite->dy);

		// Check for collision with the screen boundaries
		if ((sprite->spr.params.pos.x < sprite->spr.params.pos.w / 2.0f && sprite->dx < 0.0f) ||
			(sprite->spr.params.pos.x > (SCREEN_WIDTH-(sprite->spr.params.pos.w / 2.0f)) && sprite->dx > 0.0f))
			sprite->dx = -sprite->dx;

		if ((sprite->spr.params.pos.y < sprite->spr.params.pos.h / 2.0f && sprite->dy < 0.0f) ||
			(sprite->spr.params.pos.y > (SCREEN_HEIGHT-(sprite->spr.params.pos.h / 2.0f)) && sprite->dy > 0.0f))
			sprite->dy = -sprite->dy;
	}
	
}

static void moveEnemies() {
	for (size_t i = 0; i < numEnemies; i++)
	{
		Sprite* sprite = &enemies[i];
		C2D_SpriteMove(&sprite->spr, sprite->dx, sprite->dy);
		C2D_SpriteRotateDegrees(&sprite->spr, 1.0f);

		// Check for collision with the screen boundaries
		if ((sprite->spr.params.pos.x < sprite->spr.params.pos.w / 2.0f && sprite->dx < 0.0f) ||
			(sprite->spr.params.pos.x > (SCREEN_WIDTH-(sprite->spr.params.pos.w / 2.0f)) && sprite->dx > 0.0f))
			sprite->dx = -sprite->dx;

		if ((sprite->spr.params.pos.y < sprite->spr.params.pos.h / 2.0f && sprite->dy < 0.0f) ||
			(sprite->spr.params.pos.y > (SCREEN_HEIGHT-(sprite->spr.params.pos.h / 2.0f)) && sprite->dy > 0.0f))
			sprite->dy = -sprite->dy;
	}
}

static void moveUp(){
	if (lastMove != 1)
	{
		lastMove = 1;
		contCaminar = 0;
	}
	if (contCaminar == 0)
	{
		mainIsaac.body = isaacSprites[5].spr;
	}
	if (contCaminar == ANIMACION)
	{
		mainIsaac.body = isaacSprites[6].spr;
	}
	if (contCaminar == ANIMACION*2)
	{
		mainIsaac.body = isaacSprites[7].spr;
	}
	if (contCaminar == ANIMACION*3)
	{
		mainIsaac.body = isaacSprites[8].spr;
		contCaminar = 0;
	}
	if(!(mainIsaac.posy<=0)){
		mainIsaac.posy = mainIsaac.posy - mainIsaac.characterSpeed;
	}
}

static void moveDown(){
	if (lastMove != 2)
	{
		lastMove = 2;
		contCaminar = 0;
	}
	if (contCaminar == 0)
	{
		mainIsaac.body = isaacSprites[5].spr;
	}
	if (contCaminar == ANIMACION)
	{
		mainIsaac.body = isaacSprites[6].spr;
	}
	if (contCaminar == ANIMACION*2)
	{
		mainIsaac.body = isaacSprites[7].spr;
	}
	if (contCaminar == ANIMACION*3)
	{
		mainIsaac.body = isaacSprites[8].spr;
		contCaminar = 0;
	}
	if (contCaminar == ANIMACION*4)
	{
		contCaminar = 0;
	}
	if(!(mainIsaac.posy>=SCREEN_HEIGHT)){
		mainIsaac.posy = mainIsaac.posy + mainIsaac.characterSpeed;
	}
}

static void moveRight(){
	if (lastMove != 3)
	{
		lastMove = 3;
		contCaminar = 0;
	}
	if (contCaminar == 0)
	{
		mainIsaac.body = isaacSprites[9].spr;
	}
	if (contCaminar == ANIMACION)
	{
		mainIsaac.body = isaacSprites[10].spr;
	}
	if (contCaminar == ANIMACION*2)
	{
		mainIsaac.body = isaacSprites[11].spr;
	}
	if (contCaminar == ANIMACION*3)
	{
		contCaminar = 0;
	}
	if(!(mainIsaac.posx>=SCREEN_WIDTH)){
		mainIsaac.posx = mainIsaac.posx + mainIsaac.characterSpeed;
	}
}

static void moveLeft(){
	if (lastMove != 4)
	{
		lastMove = 4;
		contCaminar = 0;
	}
	if (contCaminar == 0)
	{
		mainIsaac.body = isaacSprites[12].spr;
	}
	if (contCaminar == ANIMACION)
	{
		mainIsaac.body = isaacSprites[13].spr;
	}
	if (contCaminar == ANIMACION*2)
	{
		mainIsaac.body = isaacSprites[14].spr;
	}
	if (contCaminar == ANIMACION*3)
	{
		contCaminar = 0;
	}
	if(!(mainIsaac.posx<=0)){
		mainIsaac.posx = mainIsaac.posx - mainIsaac.characterSpeed;
	}
}
//Logica del disparo del personaje
static void shootUp(){
	mainIsaac.head = isaacSprites[2].spr;
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
	for (size_t i = 0; i < MAX_DEATH_HEADS; i++)
	{
		if (deathHeads[i].visible == true)
		{
			C2D_DrawSprite(&deathHeads[i].spr);
		}
	}
}

static void drawIsaac(){
	C2D_DrawSprite(&mainIsaac.body);
	C2D_DrawSprite(&mainIsaac.head);
}

static void drawScene(){
	//Draw Background
	C2D_DrawSprite(&background.spr);
	drawIsaac();
	drawEnemies();
}

static void movePlayer(u32 kHeld){
	if (kHeld & KEY_UP){
			moveUp();
			contCaminar++;
		}

		if (kHeld & KEY_DOWN){
			moveDown();
			contCaminar++;
		}

		if (kHeld & KEY_RIGHT){
			moveRight();
			contCaminar++;
		}

		if (kHeld & KEY_LEFT){
			moveLeft();
			contCaminar++;
		}
}

static void shootPlayer(u32 kHeld){
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
}

static void playerStanding(u32 kUp){
	if((kUp & KEY_UP) || (kUp & KEY_DOWN) || (kUp & KEY_LEFT) || (kUp & KEY_RIGHT)){
			contCaminar = 0;
			lastMove = 0;
			isaacStanding();
		}
}

static void loadSheets(){
	isaacSheet = C2D_SpriteSheetLoad("romfs:/gfx/isaac.t3x");
	if (!isaacSheet) svcBreak(USERBREAK_PANIC);

	spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	if (!spriteSheet) svcBreak(USERBREAK_PANIC);

	enemiesSpriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/enemiesSprites.t3x");
	if (!enemiesSpriteSheet) svcBreak(USERBREAK_PANIC);

	deathHeadSheet = C2D_SpriteSheetLoad("romfs:/gfx/deathHeadSprites.t3x");
	if (!deathHeadSheet) svcBreak(USERBREAK_PANIC);
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
	loadSheets();

	// Initialize background
	initBackground();

	// Initialize enemies
	initEnemies();
	initDeathHeads();

	// Initialize sprites
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
		movePlayer(kHeld);

		//Programar Disparos
		shootPlayer(kHeld);

		u32 kUp = hidKeysUp();
		//Player Standing
		playerStanding(kUp);

		moveEnemies();
		moveDeathHeads();
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
	C2D_SpriteSheetFree(deathHeadSheet);
	C2D_SpriteSheetFree(isaacSheet);
	C2D_SpriteSheetFree(enemiesSpriteSheet);

	// Deinit libs
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	romfsExit();
	return 0;
}
