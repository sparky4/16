/* Reconstructed Commander Keen 4-6 Source Code
 * Copyright (C) 2021 K1n9_Duk3
 *
 * This file is loosely based on:
 * Keen Dreams Source Code
 * Copyright (C) 2014 Javier M. Chavez
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __CK_DEF__
#define __CK_DEF__

#include <BIOS.H>
#include <CONIO.H>

#include "ID_HEADS.H"

/*
=============================================================================

						 GLOBAL CONSTANTS & MACROS

=============================================================================
*/

#define MAXACTORS 100

#define GAMELEVELS 25

#define CONVERT_GLOBAL_TO_TILE(x)  ((x)>>(G_T_SHIFT))
#define CONVERT_TILE_TO_GLOBAL(x)  ((x)<<(G_T_SHIFT))
#define CONVERT_GLOBAL_TO_PIXEL(x) ((x)>>(G_P_SHIFT))
#define CONVERT_PIXEL_TO_GLOBAL(x) ((x)<<(G_P_SHIFT))
#define CONVERT_PIXEL_TO_TILE(x)   ((x)>>(P_T_SHIFT))
#define CONVERT_TILE_TO_PIXEL(x)   ((x)<<(P_T_SHIFT))

#define SPAWN_ADJUST_Y(y, h) (CONVERT_TILE_TO_GLOBAL(y) + (CONVERT_PIXEL_TO_GLOBAL(16-(h))))

#define ARRAYLENGTH(x) (sizeof(x)/sizeof(*(x)))

#define CA_UnmarkGrChunk(num) (grneeded[num] &= ~ca_levelbit)

#define SetPalette(pal) {_ES=FP_SEG(pal); _DX=FP_OFF(pal); _AX=0x1002; geninterrupt(0x10);}
#define SetPaletteEx(pal) {(pal)[16] = bordercolor; SetPalette(pal);}

//HACK IMPORTS:
void RFL_InitAnimList(void);
void CA_FreeGraphics(void);
void CA_SetGrPurge(void);

/*
Note:

The ID software memory manager doesn't care about the different purge levels.
Using PURGE_FIST is identical to using PURGE_LAST.
*/
#define PURGE_FIRST 3
#define PURGE_LAST 1

#define PLATFORMBLOCK 31
#define DIRARROWSTART 91
#define DIRARROWEND   (DIRARROWSTART+arrow_None)

/*
=============================================================================

						   GLOBAL TYPES

=============================================================================
*/

//SDL-style integer types - just to make future SDL ports easier
typedef unsigned int Uint16;
typedef signed int Sint16;
typedef unsigned char Uint8;
typedef signed char Sint8;
typedef unsigned long Uint32;
typedef signed long Sint32;
//Note: only the game code (CK_*.C, K?_*.C) uses these!

//some compile-time checks to make sure the ints have the correct size
#if (sizeof(Uint16) != 2)
#error 'Uint16' has wrong size
#elif (sizeof(Sint16) != 2)
#error 'Sint16' has wrong size
#elif (sizeof(Uint8) != 1)
#error 'Uint8' has wrong size
#elif (sizeof(Sint8) != 1)
#error 'Sint8' has wrong size
#elif (sizeof(Uint32) != 4)
#error 'Uint32' has wrong size
#elif (sizeof(Sint32) != 4)
#error 'Sint32' has wrong size
#endif

typedef enum {
	arrow_North,     // 0
	arrow_East,      // 1
	arrow_South,     // 2
	arrow_West,      // 3
	arrow_NorthEast, // 4
	arrow_SouthEast, // 5
	arrow_SouthWest, // 6
	arrow_NorthWest, // 7
	arrow_None       // 8
} arrowdirtype;

typedef enum {
	ex_stillplaying, //  0
	ex_died,         //  1
	ex_completed,    //  2
	ex_rescued,      //  3, only in Keen 4
	ex_warped,       //  4
	ex_resetgame,    //  5
	ex_loadedgame,   //  6
	ex_foot,         //  7, only in Keen 4
	ex_abortgame,    //  8
	ex_sandwich,     //  9, only in Keen 6
	ex_hook,         // 10, only in Keen 6
	ex_card,         // 11, only in Keen 6
	ex_molly,        // 12, only in Keen 6
	ex_portout,      // 13, only in Keen 5
	ex_fusebroke,    // 14, only in Keen 5
	ex_qedbroke,     // 15, only in Keen 5
	NUMEXITTYPES
} exittype;

typedef enum
{
	INTILE_NOTHING,        //  0
	INTILE_POLE,           //  1
	INTILE_DOOR,           //  2
	INTILE_DEADLY,         //  3
	INTILE_DROP,           //  4
	INTILE_SWITCH0,        //  5
	INTILE_SWITCH1,        //  6
	INTILE_GEMSOCKET0,     //  7
	INTILE_GEMSOCKET1,     //  8
	INTILE_GEMSOCKET2,     //  9
	INTILE_GEMSOCKET3,     // 10
	INTILE_SHORESOUTH,     // 11
	INTILE_SHOREWEST,      // 12
	INTILE_SHORENORTH,     // 13
	INTILE_SHOREEAST,      // 14
	INTILE_BRIDGESWITCH,   // 15
	INTILE_MOON,           // 16
	INTILE_DIRARROW,       // 17 (not used in the code, but assigned to tiles in Keen 5 & 6)
	INTILE_BRIDGE,         // 18
	INTILE_FORCEFIELD,     // 19
	INTILE_TELEPORT,       // 20
	INTILE_BONUS100,       // 21
	INTILE_BONUS200,       // 22
	INTILE_BONUS500,       // 23
	INTILE_BONUS1000,      // 24
	INTILE_BONUS2000,      // 25
	INTILE_BONUS5000,      // 26
	INTILE_EXTRALIFE,      // 27
	INTILE_AMMO,           // 28
	INTILE_29,             // 29 (never used)
	INTILE_FORCEFIELDEND,  // 30
	INTILE_AMPTONCOMPUTER, // 31
	INTILE_KEYCARDDOOR,    // 32
	INTILE_ELEVATORLEFT,   // 33
	INTILE_ELEVATORRIGHT,  // 34

	INTILE_FOREGROUND = 0x80
} intiletype;

#define INTILE_TYPEMASK (INTILE_FOREGROUND-1)

typedef enum
{
	nothing,         //  0
	inertobj,        //  1
	keenobj,         //  2
	stunshotobj,     //  3
#if defined KEEN4
	bonusobj,        //  4
	slugobj,         //  5
	oracleobj,       //  6
	classtype_7,     //  7, never used
	eggobj,          //  8
	madmushroomobj,  //  9
	arachnutobj,     // 10
	skypestobj,      // 11
	wormouthobj,     // 12
	thundercloudobj, // 13
	berkeloidobj,    // 14
	bounderobj,      // 15
	inchwormobj,     // 16
	footobj,         // 17
	lickobj,         // 18
	mimrockobj,      // 19
	platformobj,     // 20
	dopefishobj,     // 21
	schoolfishobj,   // 22
	pixieobj,        // 23
	lindseyobj,      // 24
	lightningobj,    // 25
	treasureeaterobj,// 26
	eggbirdobj,      // 27
	classtype_28,    // 28, never used
	classtype_29,    // 29, never used
	scubaobj,        // 30
	mshotobj,        // 31
	mineobj,         // 32
	stunnedobj,      // 33
	flagobj,         // 34
#elif defined KEEN5
	mshotobj,        //  4
	bonusobj,        //  5
	platformobj,     //  6
	stunnedobj,      //  7
	flagobj,         //  8
	sparkyobj,       //  9
	mineobj,         // 10
	slicestarobj,    // 11
	roboredobj,      // 12
	spirogripobj,    // 13
	amptonobj,       // 14
	cannonobj,       // 15
	volteobj,        // 16
	shelleyobj,      // 17, never used
	spindredobj,     // 18
	shikadimasterobj,// 19
	shikadiobj,      // 20
	petobj,          // 21
	spherefulobj,    // 22
	scottieobj,      // 23
	teleporterobj,   // 24
	qedobj,          // 25
#elif defined KEEN6
	mshotobj,        //  4
	bonusobj,        //  5
	platformobj,     //  6
	bloogobj,        //  7
	bloogletobj,     //  8
	classtype_9,     //  9, never used
	fleexobj,        // 10
	classtype_11,    // 11, never used
	mollyobj,        // 12
	babobbaobj,      // 13
	bobbaobj,        // 14
	classtype_15,    // 15
	nospikeobj,      // 16
	gikobj,          // 17
	cannonobj,       // 18
	orbatrixobj,     // 19
	bipobj,          // 20
	flectobj,        // 21
	blorbobj,        // 22
	ceilickobj,      // 23
	blooguardobj,    // 24
	stunnedobj,      // 25
	bipshipobj,      // 26
	sandwichobj,     // 27
	hookobj,         // 28
	passcardobj,     // 29
	grabbiterobj,    // 30
	rocketobj,       // 31
	grapplespotobj,  // 32
	satelliteobj,    // 33
	satellitestopobj,// 34
	flagobj,         // 35
#endif
	NUMCLASSTYPES
} classtype;

typedef struct statestruct
{
	Sint16 leftshapenum, rightshapenum;
	enum {step,slide,think,stepthink,slidethink} progress;
	boolean skippable;
	boolean pushtofloor;
	Sint16 tictime;
	Sint16 xmove;
	Sint16 ymove;
	void (*think) (struct objstruct*);
	void (*contact) (struct objstruct*, struct objstruct*);
	void (*react) (struct objstruct*);
	struct statestruct *nextstate;
} statetype;

typedef struct objstruct
{
	classtype obclass;
	enum {ac_no, ac_yes, ac_allways, ac_removable} active;
	boolean needtoreact;
	enum {cl_noclip, cl_midclip, cl_fullclip} needtoclip;
	Uint16 nothink;
	Uint16 x, y;
	Sint16 xdir, ydir;
	Sint16 xmove, ymove;
	Sint16 xspeed, yspeed;
	Sint16 ticcount;
	statetype *state;
	Uint16 shapenum;
	Uint16 priority;
	Uint16 left, top, right, bottom, midx;
	Uint16 tileleft, tiletop, tileright, tilebottom, tilemidx;
	Sint16 hitnorth, hiteast, hitsouth, hitwest;
	Sint16 temp1, temp2, temp3, temp4;
	void *sprite;
	struct objstruct *next, *prev;
} objtype;

typedef struct
{
	Uint16 worldx, worldy;
	boolean leveldone[GAMELEVELS];
	Sint32 score, nextextra;
	Sint16 ammo, drops;
#if defined KEEN4
	Sint16 wetsuit;
	Sint16 rescued;
#elif defined KEEN5
	boolean keycard;
	Sint16 destroyed;	// never used
	Sint16 numfuses;
#elif defined KEEN6
	Sint16 sandwichstate, hookstate, passcardstate, rocketstate;
#endif
	Sint16 keys[4];
	Sint16 mapon;
	Sint16 lives;
	Sint16 difficulty;
	objtype *riding;
} gametype;

/*
=============================================================================

						CK_MAIN DEFINITIONS

=============================================================================
*/

extern char str[80], str2[20];
extern boolean storedemo;

void SizeText(char *text, Uint16 *width, Uint16 *height);

/*
=============================================================================

						CK_DEMO DEFINITIONS

=============================================================================
*/

extern boolean scorescreenkludge;

void CheckLastScan(void);
#if GRMODE == EGAGR
void Terminator(void);
void StarWars(void);
#endif
void ShowTitle(void);
#if GRMODE == CGAGR
void ShowCredits(void);
#endif
void RunDemo(Sint16 num);
void DrawHighScores(void);
void CheckHighScore(Sint32 score, Sint16 completed);
void ShowHighScores(void);

/*
=============================================================================

						CK_GAME DEFINITIONS

=============================================================================
*/

void FreeGraphics(void);
void NewGame(void);
boolean SaveTheGame(Sint16 handle);
boolean LoadTheGame(Sint16 handle);
void ResetGame(void);
void SetupGameLevel(boolean loadnow);
void DialogDraw(char *title, Uint16 numcache);
void DialogUpdate(void);
void DialogFinish(void);
void StartDemoRecord(void);
void EndDemoRecord(void);
void GameLoop(void);
void HandleDeath(void);

/*
=============================================================================

						CK_PLAY DEFINITIONS

=============================================================================
*/

extern boolean singlestep, jumpcheat, godmode, keenkilled;
extern exittype playstate;
extern gametype gamestate;
extern objtype *new, *check, *player, *scoreobj;
extern Uint16 originxtilemax, originytilemax;
extern ControlInfo c;
extern boolean button2, button3;	// never used
extern objtype dummyobj;
extern Sint16 invincible;
extern boolean oldshooting, showscorebox, joypad;
extern Sint16 groundslam;
extern boolean debugok;
extern boolean jumpbutton, jumpheld, pogobutton, pogoheld, firebutton, fireheld, upheld;


void CheckKeys(void);
void StatusWindow(void);
void CenterActor(objtype *ob);
void WorldScrollScreen(objtype *ob);
void ScrollScreen(objtype *ob);
void InitObjArray(void);
Sint16 GetNewObj(boolean usedummy);
void RemoveObj(objtype *ob);
void GivePoints(Uint16 points);
void StopMusic(void);
void StartMusic(Uint16 num);
void PlayLoop(void);

/*
=============================================================================

						CK_TEXT DEFINITIONS

=============================================================================
*/

void HelpScreens(void);
void FinaleLayout(void);

/*
=============================================================================

						CK_STATE DEFINITIONS

=============================================================================
*/

extern Sint16 wallclip[8][16];

extern Sint16 xtry;
extern Sint16 ytry;
extern boolean playerkludgeclipcancel;

void MoveObjVert(objtype *ob, Sint16 ymove);
void MoveObjHoriz(objtype *ob, Sint16 xmove);
void PlayerBottomKludge(objtype *ob);
void PlayerTopKludge(objtype *ob);
void ClipToEnds(objtype *ob);
void ClipToSides(objtype *ob);
boolean CheckPosition(objtype *ob);
boolean StatePositionOk(objtype *ob, statetype *state);

#ifdef KEEN5
void CalcBounds(objtype *ob);
#endif

void ClipToWalls(objtype *ob);
void FullClipToWalls(objtype *ob);
void PushObj(objtype *ob);
void ClipToSpriteSide(objtype *push, objtype *solid);
void ClipToSpriteTop(objtype *push, objtype *solid);
void ClipToSprite(objtype *push, objtype *solid, boolean squish);
Sint16 DoActor(objtype *ob, Sint16 numtics);
void StateMachine(objtype *ob);
void NewState(objtype *ob, statetype *state);
void ChangeState(objtype *ob, statetype *state);
boolean OnScreen(objtype *ob);
void DoGravity(objtype *ob);
void DoWeakGravity(objtype *ob);
void DoTinyGravity(objtype *ob);
void AccelerateX(objtype *ob, Sint16 dir, Sint16 maxspeed);
void AccelerateXv(objtype *ob, Sint16 dir, Sint16 maxspeed);
void AccelerateY(objtype *ob, Sint16 dir, Sint16 maxspeed);
void FrictionX(objtype *ob);
void FrictionY(objtype *ob);
void StunObj(objtype *ob, objtype *shot, statetype *stunstate);
void T_Projectile(objtype *ob);
void T_WeakProjectile(objtype *ob);
void ProjectileThink1(objtype *ob);
void T_Velocity(objtype *ob);
void SetReactThink(objtype *ob);
void T_Stunned(objtype *ob);
void C_Lethal(objtype *ob, objtype *hit);
void R_Draw(objtype *ob);
void R_Walk(objtype *ob);
void R_WalkNormal(objtype *ob);
void BadState(void);
void R_Stunned(objtype *ob);

extern statetype sc_deadstate;
extern statetype sc_badstate;

/*
=============================================================================

						CK_KEEN DEFINITIONS

=============================================================================
*/

extern Uint16 bounceangle[8][8];
#ifndef KEEN4
extern arrowdirtype arrowflip[];
#endif

extern statetype s_keenstand;
extern statetype s_keenpauselook;
extern statetype s_keenwait1;
extern statetype s_keenwait2;
extern statetype s_keenwait3;
extern statetype s_keenwait4;
extern statetype s_keenwait5;
extern statetype s_keenwait6;
extern statetype s_keenmoon1;
extern statetype s_keenmoon2;
extern statetype s_keenmoon3;
extern statetype s_keenread;
extern statetype s_keenread2;
extern statetype s_keenread3;
extern statetype s_keenread4;
extern statetype s_keenread5;
extern statetype s_keenread6;
extern statetype s_keenread7;
extern statetype s_keenstopread;
extern statetype s_keenstopread2;
extern statetype s_keenstopread3;
extern statetype s_keenlookup;
extern statetype s_keenlookup2;
extern statetype s_keenlookdown;
extern statetype s_keenlookdown2;
extern statetype s_keenlookdown3;
extern statetype s_keenlookdown4;
extern statetype s_keendrop;
extern statetype s_keendead;
extern statetype s_keendie1;
extern statetype s_keendie2;
#ifdef KEEN4
extern statetype s_keensuitdie1;
extern statetype s_keensuitdie2;
#endif
extern statetype s_keenshoot1;
extern statetype s_keenshoot2;
extern statetype s_keenshootup1;
extern statetype s_keenshootup2;
extern statetype s_keenswitch;
extern statetype s_keenswitch2;
extern statetype s_keenkey;
extern statetype s_keenlineup;
extern statetype s_keenenter1;
extern statetype s_keenenter2;
extern statetype s_keenenter3;
extern statetype s_keenenter4;
extern statetype s_keenenter5;
extern statetype s_keenpole;
extern statetype s_keenclimb1;
extern statetype s_keenclimb2;
extern statetype s_keenclimb3;
extern statetype s_keenslide1;
extern statetype s_keenslide2;
extern statetype s_keenslide3;
extern statetype s_keenslide4;
extern statetype s_keenpoleshoot1;
extern statetype s_keenpoleshoot2;
extern statetype s_keenpoleshootup1;
extern statetype s_keenpoleshootup2;
extern statetype s_keenpoleshootdown1;
extern statetype s_keenpoleshootdown2;
extern statetype s_keenwalk1;
extern statetype s_keenwalk2;
extern statetype s_keenwalk3;
extern statetype s_keenwalk4;
extern statetype s_keenpogodown;
extern statetype s_keenpogo;
extern statetype s_keenpogo2;
extern statetype s_keenjump1;
extern statetype s_keenjump2;
extern statetype s_keenjump3;
extern statetype s_keenjump4;
extern statetype s_keenairshoot1;
extern statetype s_keenairshoot2;
extern statetype s_keenairshoot3;
extern statetype s_keenairshootup1;
extern statetype s_keenairshootup2;
extern statetype s_keenairshootup3;
extern statetype s_keenairshootdown1;
extern statetype s_keenairshootdown2;
extern statetype s_keenairshootdown3;
extern statetype s_keenholdon;
extern statetype s_keenholdon2;
extern statetype s_keenclimbup;
extern statetype s_keenclimbup2;
extern statetype s_keenclimbup3;
extern statetype s_keenclimbup4;
extern statetype s_keenclimbup5;

extern Sint16 slopespeed[8];
extern Sint16 polexspeed[3];

extern Sint16 shotsinclip[4];
extern Sint16 bonussound[];
extern Sint16 bonuspoints[];
extern Sint16 bonussprite[];

extern Uint16 zeromap;

extern Sint16 singlegravity;
extern Sint16 jumptime;
extern Sint32 leavepoletime;
extern Sint16 moonok;

void SpawnKeen(Sint16 x, Sint16 y, Sint16 dir);
boolean CheckGrabPole(objtype *ob);
boolean CheckEnterHouse(objtype *ob);
void WalkSound1(objtype *ob);
void WalkSound2(objtype *ob);
void KeenStandThink(objtype *ob);
void KeenPauseThink(objtype *ob);
void KeenReadThink(objtype *ob);
void KeenLookUpThink(objtype *ob);
void KeenLookDownThink(objtype *ob);
void KeenWalkThink(objtype *ob);
void T_LineUp(objtype *ob);
void KeenEnterThink(objtype *ob);
void KeenSwitchThink(objtype *ob);
void KeenKeyThink(objtype *ob);
void KeenAirThink(objtype *ob);
void KeenBounceThink(objtype *ob);
void KeenPogoThink(objtype *ob);
void PoleActions(objtype *ob);
void KeenPoleThink(objtype *ob);
void KeenClimbThink(objtype *ob);
void KeenDropThink(objtype *ob);
void KeenDropDownThink(objtype *ob);
void KeenHoldThink(objtype *ob);
void KeenShootThink(objtype *ob);
void T_PullUp1(objtype *ob);
void T_PullUp2(objtype *ob);
void T_PullUp3(objtype *ob);
void T_PulledUp(objtype *ob);
void KeenDieThink(objtype *ob);
void KillKeen(void);
void KeenContact(objtype *ob, objtype *hit);
void KeenPosContact(objtype *ob, objtype *hit);
void HandleRiding(objtype *ob);
void TileBonus(Uint16 x, Uint16 y, Uint16 bonus);
void GiveDrop(Uint16 x, Uint16 y);
void CheckInTiles(objtype *ob);
void KeenSimpleReact(objtype *ob);
void KeenStandReact(objtype *ob);
void KeenWalkReact(objtype *ob);
void KeenAirReact(objtype *ob);
void KeenPogoReact(objtype *ob);
void KeenPoleReact(objtype *ob);


/*
=============================================================================

						CK_KEEN2 DEFINITIONS

=============================================================================
*/

extern statetype s_score;
extern statetype s_demo;
void SpawnScore(void);
void UpdateScore(objtype *ob);
void DrawDemoPlaque(objtype *ob);

extern statetype s_worldkeen;
extern statetype s_worldkeenwave1;
extern statetype s_worldkeenwave2;
extern statetype s_worldkeenwave3;
extern statetype s_worldkeenwave4;
extern statetype s_worldkeenwave5;
extern statetype s_worldkeenwalk;
void SpawnWorldKeen(Sint16 x, Sint16 y);
#ifdef KEEN5
void SpawnWorldKeenPort(Uint16 tileX, Uint16 tileY);
#endif
void CheckEnterLevel(objtype *ob);
void T_KeenWorld(objtype *ob);
void T_KeenWorldWalk(objtype *ob);
void CheckWorldInTiles(objtype *ob);

#ifdef KEEN4
extern statetype s_keenonfoot1;
extern statetype s_keenonfoot2;
extern statetype s_worldswim;
void T_FootFly(objtype *ob);
void T_KeenWorldSwim(objtype *ob);
#endif

#ifdef KEEN5
extern statetype s_worldelevate;
void T_Elevate(objtype *ob);
#endif

extern statetype s_flagwave1;
extern statetype s_flagwave2;
extern statetype s_flagwave3;
extern statetype s_flagwave4;
void SpawnFlag(Sint16 x, Sint16 y);

#ifndef KEEN5
extern statetype s_throwflag0;
extern statetype s_throwflag1;
extern statetype s_throwflag2;
extern statetype s_throwflag3;
extern statetype s_throwflag4;
extern statetype s_throwflag5;
extern statetype s_throwflag6;
void SpawnThrowFlag(Sint16 x, Sint16 y);
void TossThink(objtype *ob);
void PathThink(objtype *ob);
void FlagAlign(objtype *ob);
#endif

extern statetype s_stunray1;
extern statetype s_stunray2;
extern statetype s_stunray3;
extern statetype s_stunray4;
extern statetype s_stunhit;
extern statetype s_stunhit2;
void SpawnShot(Uint16 x, Uint16 y, Direction dir);
void ExplodeShot(objtype *ob);
void T_Shot(objtype *ob);
void R_Shot(objtype *ob);

extern statetype s_door1;
extern statetype s_door2;
extern statetype s_door3;
void DoorOpen(objtype *ob);

#ifdef KEEN5
extern statetype s_carddoor;
void CardDoorOpen(objtype *ob);
#endif

/*
=============================================================================

						OTHER DEFINITIONS

=============================================================================
*/

#if defined KEEN4
#include "K4_DEF.H"
#elif defined KEEN5
#include "K5_DEF.H"
#elif defined KEEN6
#include "K6_DEF.H"
#endif

#endif