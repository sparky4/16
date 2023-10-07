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

#ifndef __K6_DEF__
#define __K6_DEF__

/*
=============================================================================

						GLOBAL CONSTANTS

=============================================================================
*/

#if GRMODE == CGAGR
#define MINMEMORY 255000l
#else
#define MINMEMORY 300000l
#endif

#define STARPALETTE   {0, 1, 24, 25, 4, 28, 6, 7, 31, 31, 31, 31, 31, 31, 31, 31, 0}
#define INTROPALETTE  {0, 5, 5, 21, 1, 1, 1, 1, 17, 17, 17, 17, 19, 19, 19, 19, 0}
#define SHRINKPALETTE {0, 5, 5, 21, 1, 1, 1, 1, 17, 17, 17, 17, 19, 19, 19,  5, 0}

#define HIGHSCORE_LEFT	40
#define HIGHSCORE_TOP	51
#define HIGHSCORE_RIGHT	280
#define HIGHSCORE_MAP	18

#define STATUS_PRESSKEY_X 120

#define WORLDMAPNAME	"Fribbulus Xax"
#define DROPSNAME	"VIVAS"

#define STARWARSMUSIC	13
#define ENDINGMUSIC	1

// levels in this range can NOT be re-entered (BWB level should be > MAXDONELEVEL)
#define MINDONELEVEL 1
#define MAXDONELEVEL 16

#define INACTIVATEDIST 4

//
// tiles for worldmap teleporters
//
#define TELEPORTERTILE1 2613	// tile animation for teleporting out
#define TELEPORTERTILE2 2629	// tile after teleporting out
#define TELEPORTERTILE3 TELEPORTERTILE1	// tile animation for teleporting in
#define TELEPORTERTILE4 TELEPORTERTILE2	// tile after teleporting in

#define TELEPORERTILEMASK 3	// animation has 4 frames

extern Sint16 groundslam;

//HACKs:
//#define US_ManualCheck() true
boolean US_ManualCheck(void);

/*
=============================================================================

						K6_SPEC DEFINITIONS

=============================================================================
*/

extern char far swtext[];
extern char far *levelnames[GAMELEVELS];
extern char far *levelenter[GAMELEVELS];

void ScanInfoPlane(void);

extern statetype s_keenstun;

void FlipBigSwitch(objtype *ob, boolean isup);
void GotSandwich(void);
void GotHook(void);
void GotPasscard(void);


/*
=============================================================================

						K6_ACT1 DEFINITIONS

=============================================================================
*/

extern Sint16 pdirx[];
extern Sint16 pdiry[];

void C_ClipSide(objtype *ob, objtype *hit);
void C_ClipTop(objtype *ob, objtype *hit);
void R_Land(objtype *ob);
void R_Bounce(objtype *ob);

extern statetype s_bonus1;
extern statetype s_bonus2;
extern statetype s_bonusfly1;
extern statetype s_bonusfly2;
extern statetype s_bonusrise;

extern statetype s_splash1;
extern statetype s_splash2;
extern statetype s_splash3;
extern statetype s_splash4;

extern Uint16 bonusshape[];

void SpawnBonus(Uint16 tileX, Uint16 tileY, Uint16 type);
void SpawnSplash(Uint16 tileX, Uint16 tileY);
void T_Bonus(objtype *ob);
void T_FlyBonus(objtype *ob);

extern statetype s_grabbiter1;
extern statetype s_grabbiter2;
extern statetype s_grabbitersleep1;
extern statetype s_grabbitersleep2;

void SpawnGrabbiter(Uint16 tileX, Uint16 tileY);
void C_Grabbiter(objtype *ob, objtype *hit);

extern statetype s_rocket;
extern statetype s_rocketfly1;
extern statetype s_rocketfly2;
extern statetype s_keenrocket;

void SpawnRocket(Uint16 tileX, Uint16 tileY, Uint16 state);
void T_Rocket(objtype *ob);
void C_Rocket(objtype *ob, objtype *hit);
void C_RocketFly(objtype *ob, objtype *hit);
void T_RocketFly(objtype *ob);

extern statetype s_grapplespot;
extern statetype s_throwrope1;
extern statetype s_throwrope2;
extern statetype s_climbrope1;
extern statetype s_climbrope2;
extern statetype s_maprope;
extern statetype s_mapropeshort;

void SpawnGrappleSpot(Uint16 tileX, Uint16 tileY, Uint16 type);
void T_ThrowRope(objtype *ob);
void T_ClimbRope(objtype *ob);
void C_GrappleSpot(objtype *ob, objtype *hit);

extern statetype s_satellitestopspot;
extern statetype s_worldkeensatellite;
extern statetype s_satellite1;
extern statetype s_satellite2;
extern statetype s_satellite3;
extern statetype s_satellite4;

void SpawnSatelliteStop(Uint16 tileX, Uint16 tileY, Uint16 type);
void SpawnSatellite(Uint16 tileX, Uint16 tileY);
void T_Satellite(objtype *ob);
void C_Satellite(objtype *ob, objtype *hit);
void R_WorldKeenSatellite(objtype *ob);

extern statetype s_sandwich;

void SpawnSandwich(Uint16 tileX, Uint16 tileY);

extern statetype s_hook;

void SpawnHook(Uint16 tileX, Uint16 tileY);

extern statetype s_passcard;

void SpawnPasscard(Uint16 tileX, Uint16 tileY);
void C_Molly(objtype *ob, objtype *hit);

extern statetype s_molly1;
extern statetype s_molly2;
extern statetype s_molly3;
extern statetype s_molly4;

void SpawnMolly(Uint16 tileX, Uint16 tileY);

extern statetype s_platform;

void SpawnPlatform(Uint16 tileX, Uint16 tileY, Sint16 dir);
void T_Platform(objtype *ob);

extern statetype s_dropplatsit;
extern statetype s_fallplatfall;
extern statetype s_fallplatrise;

void SpawnDropPlat(Uint16 tileX, Uint16 tileY);
void T_DropPlatSit(objtype *ob);
void T_DropPlatFall(objtype *ob);
void T_DropPlatRise(objtype *ob);

extern statetype s_staticplatform;

void SpawnStaticPlat(Uint16 tileX, Uint16 tileY);

extern statetype s_goplat;

void SpawnGoPlat(Uint16 tileX, Uint16 tileY, Sint16 dir);
void T_GoPlat(objtype *ob);
void R_GoPlat(objtype *ob);

extern statetype s_sneakplatsit;
extern statetype s_sneakplatdodge;
extern statetype s_sneakplatreturn;

void SpawnSneakPlat(Uint16 tileX, Uint16 tileY);
void T_SneakPlat(objtype *ob);

extern statetype s_bloogwalk1;
extern statetype s_bloogwalk2;
extern statetype s_bloogwalk3;
extern statetype s_bloogwalk4;
extern statetype s_bloogstun;

void SpawnBloog(Uint16 tileX, Uint16 tileY);
void T_BloogWalk(objtype *ob);
void C_Bloog(objtype *ob, objtype *hit);

extern statetype s_blooguardwalk1;
extern statetype s_blooguardwalk2;
extern statetype s_blooguardwalk3;
extern statetype s_blooguardwalk4;
extern statetype s_blooguardattack1;
extern statetype s_blooguardattack2;
extern statetype s_blooguardattack3;
extern statetype s_blooguardattack4;
extern statetype s_blooguardstun;

void SpawnBlooguard(Uint16 tileX, Uint16 tileY);
void T_BlooguardWalk(objtype *ob);
void T_BlooguardAttack(objtype *ob);
void C_Blooguard(objtype *ob, objtype *hit);
void R_Blooguard(objtype *ob);

extern statetype s_rbloogletwalk1;
extern statetype s_rbloogletwalk2;
extern statetype s_rbloogletwalk3;
extern statetype s_rbloogletwalk4;
extern statetype s_rbloogletstun;
extern statetype s_ybloogletwalk1;
extern statetype s_ybloogletwalk2;
extern statetype s_ybloogletwalk3;
extern statetype s_ybloogletwalk4;
extern statetype s_ybloogletstun;
extern statetype s_bbloogletwalk1;
extern statetype s_bbloogletwalk2;
extern statetype s_bbloogletwalk3;
extern statetype s_bbloogletwalk4;
extern statetype s_bbloogletstun;
extern statetype s_gbloogletwalk1;
extern statetype s_gbloogletwalk2;
extern statetype s_gbloogletwalk3;
extern statetype s_gbloogletwalk4;
extern statetype s_gbloogletstun;

void SpawnBlooglet(Uint16 tileX, Uint16 tileY, Sint16 type);
void C_Blooglet(objtype *ob, objtype *hit);

/*
=============================================================================

						K6_ACT2 DEFINITIONS

=============================================================================
*/

extern statetype s_nospikestand;
extern statetype s_nospikewalk1;
extern statetype s_nospikewalk2;
extern statetype s_nospikewalk3;
extern statetype s_nospikewalk4;
extern statetype s_nospikerun1;
extern statetype s_nospikerun2;
extern statetype s_nospikerun3;
extern statetype s_nospikerun4;
extern statetype s_nospikeconfused1;
extern statetype s_nospikeconfused2;
extern statetype s_nospikeconfused3;
extern statetype s_nospikefall;
extern statetype s_nospikestun;

void SpawnNospike(Uint16 tileX, Uint16 tileY);
void T_NospikeWalk(objtype *ob);
void T_NospikeRun(objtype *ob);
void C_Nospike(objtype *ob, objtype *hit);
void T_NospikeConfused(objtype* ob);
void R_NospikeConfused(objtype *ob);
void R_NospikeFall(objtype *ob);
void R_NospikeRun(objtype *ob);

extern statetype s_gikwalk1;
extern statetype s_gikwalk2;
extern statetype s_gikwalk3;
extern statetype s_gikjump;
extern statetype s_gikslide1;
extern statetype s_gikslide2;
extern statetype s_gikstand;

void SpawnGik(Uint16 tileX, Uint16 tileY);
void T_GikWalk(objtype *ob);
void T_GikSlide(objtype *ob);
void R_GikJump(objtype *ob);
void R_GikSlide(objtype *ob);

extern statetype s_cannon;
extern statetype s_cannonfire;
extern statetype s_cshot1;
extern statetype s_cshot2;
extern statetype s_cshot3;
extern statetype s_cshot4;
extern statetype s_cshothit1;
extern statetype s_cshothit2;

void SpawnCannon(Uint16 tileX, Uint16 tileY, Sint16 dir);
void T_Cannon(objtype *ob);
void C_CShot(objtype *ob, objtype *hit);
void R_CShot(objtype *ob);

extern statetype s_orbatrix1;
extern statetype s_orbatrix2;
extern statetype s_orbatrixcurl1;
extern statetype s_orbatrixcurl2;
extern statetype s_orbatrixcurl3;
extern statetype s_orbatrixuncurl1;
extern statetype s_orbatrixuncurl2;
extern statetype s_orbatrixidle1;
extern statetype s_orbatrixidle2;
extern statetype s_orbatrixidle3;
extern statetype s_orbatrixidle4;
extern statetype s_orbatrixbounce1;
extern statetype s_orbatrixbounce2;
extern statetype s_orbatrixbounce3;
extern statetype s_orbatrixbounce4;

void SpawnOrbatrix(Uint16 tileX, Uint16 tileY);
void T_OrbatrixFly(objtype *ob);
void C_Orbatrix(objtype *ob, objtype *hit);
void R_Orbatrix(objtype *ob);
void R_OrbatrixBounce(objtype *ob);
void T_OrbatrixCurl(objtype *ob);
void T_OrbatrixUncurl(objtype *ob);
void C_OrbatrixBounce(objtype *ob, objtype *hit);

extern statetype s_bipstand;
extern statetype s_bipwalk1;
extern statetype s_bipwalk2;
extern statetype s_bipwalk3;
extern statetype s_bipwalk4;
extern statetype s_bipsquished;

void T_BipWalk(objtype *ob);
void C_Bip(objtype *ob, objtype *hit);

extern statetype s_bipship;
extern statetype s_bipshipshot;
extern statetype s_bipshipturn1;
extern statetype s_bipshipturn2;
extern statetype s_bipshipturn3;
extern statetype s_bipshipturn4;
extern statetype s_bipshipexplode1;
extern statetype s_bipshipexplode2;
extern statetype s_bipshipexplode3;
extern statetype s_bipshipsmoke1;
extern statetype s_bipshipsmoke2;

void SpawnBipship(Uint16 tileX, Uint16 tileY);
void R_BipShot(objtype *ob);
void T_BipshipTurn(objtype *ob);
void T_BipshipFly(objtype *ob);
void T_BipshipExplode(objtype *ob);
void C_Bipship(objtype *ob, objtype *hit);

extern statetype s_flectstand;
extern statetype s_flectturn;
extern statetype s_flectwalk1;
extern statetype s_flectwalk2;
extern statetype s_flectwalk3;
extern statetype s_flectwalk4;
extern statetype s_flectstun;

void SpawnFlect(Uint16 tileX, Uint16 tileY);
void T_FlectStand(objtype *ob);
void T_FlectWalk(objtype *ob);
void C_Flect(objtype *ob, objtype *hit);
void R_Flect(objtype *ob);

/*
=============================================================================

						K6_ACT3 DEFINITIONS

=============================================================================
*/

extern statetype s_fleexwalk1;
extern statetype s_fleexwalk2;
extern statetype s_fleexrun1;
extern statetype s_fleexrun2;
extern statetype s_fleexrun3;
extern statetype s_fleexrun4;
extern statetype s_fleexlook1;
extern statetype s_fleexlook2;
extern statetype s_fleexstun;

void SpawnFleex(Uint16 tileX, Uint16 tileY);
void T_FleexWalk(objtype *ob);
void T_FleexLook(objtype *ob);
void C_Fleex(objtype *ob, objtype *hit);

extern statetype s_bobbajump1;
extern statetype s_bobbajump2;
extern statetype s_bobbastand;
extern statetype s_bobbaattack;
extern statetype s_bobbashot1;
extern statetype s_bobbashot2;
extern statetype s_bobbashot3;
extern statetype s_bobbashot4;
extern statetype s_bobbashot5;
extern statetype s_bobbashot6;
extern statetype s_bobbashot7;
extern statetype s_bobbashot8;
extern statetype s_bobbashotvanish1;
extern statetype s_bobbashotvanish2;
extern statetype s_bobbashotvanish3;

void SpawnBobba(Uint16 tileX, Uint16 tileY);
void T_BobbaShot(objtype *ob);
void T_BobbaStand(objtype *ob);
void C_Bobba(objtype *ob, objtype *hit);
void R_Bobba(objtype *ob);
void R_BobbaShot(objtype *ob);

extern statetype s_babobbajump1;
extern statetype s_babobbajump2;
extern statetype s_babobbastand;
extern statetype s_babobbaattack;
extern statetype s_babobbastun1;
extern statetype s_babobbastun2;
extern statetype s_babobbasleep1;
extern statetype s_babobbasleep2;
extern statetype s_babobbasleep3;
extern statetype s_babobbasleep4;
extern statetype s_babobbasleep5;
extern statetype s_babobbasleep6;
extern statetype s_babobbasleep7;
extern statetype s_babobbashot1;
extern statetype s_babobbashot2;
extern statetype s_babobbashot3;
extern statetype s_babobbashotvanish1;
extern statetype s_babobbashotvanish2;

void SpawnBabobba(Uint16 tileX, Uint16 tileY);
void T_BabobbaStand(objtype *ob);
void C_Babobba(objtype *ob, objtype *hit);
void C_BabobbaSleep(objtype *ob, objtype *hit);
void R_Babobba(objtype *ob);
void T_BabobbaShot(objtype *ob);
void T_BabobbaShotVanish(objtype *ob);

extern statetype s_blorb1;
extern statetype s_blorb2;
extern statetype s_blorb3;

void SpawnBlorb(Uint16 tileX, Uint16 tileY);
void R_Blorb(objtype *ob);

extern statetype s_ceilickhidden;
extern statetype s_ceilickattack1;
extern statetype s_ceilickattack2;
extern statetype s_ceilickattack3;
extern statetype s_ceilickattack4;
extern statetype s_ceilickattack5;
extern statetype s_ceilickattack6;
extern statetype s_ceilickattack7;
extern statetype s_ceilickattack8;
extern statetype s_ceilickattack9;
extern statetype s_ceilickattack10;
extern statetype s_ceilickattack11;
extern statetype s_ceilicklaugh1;
extern statetype s_ceilicklaugh2;
extern statetype s_ceilicklaugh3;
extern statetype s_ceilicklaugh4;
extern statetype s_ceilicklaugh5;
extern statetype s_ceilicklaugh6;
extern statetype s_ceilicklaugh7;
extern statetype s_ceilicklaugh8;
extern statetype s_ceilicklaugh9;
extern statetype s_ceilickstun;

void SpawnCeilick(Uint16 tileX, Uint16 tileY);
void T_CeilickHidden(objtype *ob);
void T_CeilickLaugh(objtype *ob);
void T_CeilickStunned(objtype *ob);
void C_Ceilick(objtype *ob, objtype *hit);

#endif