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

#ifndef __K5_DEF__
#define __K5_DEF__

/*
=============================================================================

						GLOBAL CONSTANTS

=============================================================================
*/

#if GRMODE == CGAGR
#define MINMEMORY 255000l
#else
#define MINMEMORY 310000l
#endif

#define STARPALETTE   {0, 1, 24, 30, 31, 28, 6, 7, 19, 19, 19, 19, 19, 19, 19, 19, 0}
#define INTROPALETTE  {0, 4, 4, 28, 1, 1, 1, 1, 17, 17, 17, 17, 19, 19, 19, 19, 0}
#define SHRINKPALETTE {0, 4, 4, 28, 1, 1, 1, 1, 17, 17, 17, 17, 19, 19, 19,  4, 0}

#define HIGHSCORE_LEFT	40
#define HIGHSCORE_TOP	35
#define HIGHSCORE_RIGHT	280
#define HIGHSCORE_MAP	15

#define STATUS_PRESSKEY_X 120

#define WORLDMAPNAME	"Armageddon"
#define DROPSNAME	"VITALIN"

#define STARWARSMUSIC	17
#define ENDINGMUSIC	14

// levels in this range can NOT be re-entered (BWB level should be > MAXDONELEVEL)
#define MINDONELEVEL 1
#define MAXDONELEVEL 17

#define INACTIVATEDIST 6

//
// tiles for worldmap teleporters
//
#define TELEPORTERTILE1 2687	// tile animation for teleporting out
#define TELEPORTERTILE2 1063	// tile after teleporting out
#define TELEPORTERTILE3 TELEPORTERTILE1	// tile animation for teleporting in
#define TELEPORTERTILE4 0	// tile after teleporting in

#define TELEPORERTILEMASK 1	// animation has 2 frames

/*
=============================================================================

						K5_SPEC DEFINITIONS

=============================================================================
*/

extern char far swtext[];
extern char far *levelnames[GAMELEVELS];
extern char far *levelenter[GAMELEVELS];

void OpenMapDoor(Sint16 tileX, Sint16 tileY);
void CloseMapDoor(Sint16 tileX, Sint16 tileY);
void ScanInfoPlane(void);
void GameOver(void);

void FinishedFuse(void);

/*
=============================================================================

						K5_ACT1 DEFINITIONS

=============================================================================
*/

extern Sint16 pdirx[];
extern Sint16 pdiry[];

Sint16 CheckSpawnShot(Uint16 x, Uint16 y, statetype *state);
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

extern statetype s_teleport1;
extern statetype s_teleport2;
extern statetype s_teleportzap1;
extern statetype s_teleportzap2;

void SpawnTeleport(void);

extern statetype s_fuseflash1;
extern statetype s_fuseflash2;
extern statetype s_fuseflash3;

void SpawnFuseFlash(Uint16 tileX, Uint16 tileY);

extern statetype s_deadmachine;

void SpawnDeadMachine(void);
void T_DeadMachine(objtype *ob);

extern statetype s_platform;
extern statetype s_slotplat1;
extern statetype s_slotplat2;

void SpawnPlatform(Uint16 tileX, Uint16 tileY, Sint16 dir, Sint16 type);
void T_Platform(objtype *ob);
void T_Slotplat(objtype *ob);

extern statetype s_dropplatsit;
extern statetype s_dropplatfall;
extern statetype s_dropplatrise;

void SpawnDropPlat(Uint16 tileX, Uint16 tileY);
void T_DropPlatSit(objtype *ob);
void T_DropPlatFall(objtype *ob);
void T_DropPlatRise(objtype *ob);

extern statetype s_statplat;

void SpawnStaticPlat(Uint16 tileX, Uint16 tileY);

extern statetype s_goplat;
extern statetype s_slotgoplat1;
extern statetype s_slotgoplat2;

void SpawnGoPlat(Uint16 tileX, Uint16 tileY, Sint16 dir, Sint16 type);
void T_GoPlat(objtype *ob);
void T_GoSlotPlat(objtype *ob);

extern statetype s_volte1;
extern statetype s_volte2;
extern statetype s_volte3;
extern statetype s_volte4;
extern statetype s_voltestun;

void SpawnVolte(Uint16 tileX, Uint16 tileY);
void T_Volte(objtype *ob);
void C_Volte(objtype *ob, objtype *hit);

extern statetype s_sneakplatsit;
extern statetype s_sneakplatdodge;
extern statetype s_sneakplatreturn;

void SpawnSneakPlat(Uint16 tileX, Uint16 tileY);
void T_SneakPlat(objtype *ob);

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

/*
=============================================================================

						K5_ACT2 DEFINITIONS

=============================================================================
*/

extern statetype s_sparkywalk1;
extern statetype s_sparkywalk2;
extern statetype s_sparkywalk3;
extern statetype s_sparkywalk4;
extern statetype s_sparkylook1;
extern statetype s_sparkylook2;
extern statetype s_sparkylook3;
extern statetype s_sparkylook4;
extern statetype s_sparkylook5;
extern statetype s_sparkylook6;
extern statetype s_sparkylook7;
extern statetype s_sparkylook8;
extern statetype s_sparkyspeed1;
extern statetype s_sparkyspeed2;
extern statetype s_sparkyspeed3;
extern statetype s_sparkyspeed4;
extern statetype s_sparkycharge1;
extern statetype s_sparkycharge2;
extern statetype s_sparkycharge3;
extern statetype s_sparkycharge4;
extern statetype s_sparkyturn1;
extern statetype s_sparkyturn2;
extern statetype s_sparkyturn3;
extern statetype s_sparkystun;

void SpawnSparky(Uint16 tileX, Uint16 tileY);
void T_Sparky(objtype *ob);
void T_ChargeCount(objtype *ob);
void T_SparkyLookL(objtype *ob);
void T_SparkyLookR(objtype *ob);
void T_RunSnd1(objtype *ob);
void T_RunSnd2(objtype *ob);
void C_Sparky(objtype *ob, objtype *hit);
void R_Sparky(objtype *ob);

extern statetype s_amptonwalk1;
extern statetype s_amptonwalk2;
extern statetype s_amptonwalk3;
extern statetype s_amptonwalk4;
extern statetype s_amptonturn;
extern statetype s_amptongrab1;
extern statetype s_amptongrab2;
extern statetype s_amptonclimb;
extern statetype s_amptonrelease1;
extern statetype s_amptonrelease2;
extern statetype s_amptonfiddle1;
extern statetype s_amptonfiddle2;
extern statetype s_amptonfiddle3;
extern statetype s_amptonfiddle4;
extern statetype s_amptonfiddle5;
extern statetype s_amptonstun;

void SpawnAmpton(Uint16 tileX, Uint16 tileY);
void T_Ampton(objtype *ob);
void T_AmptonClimb(objtype *ob);
void T_SetNoThink(objtype *ob);
void C_Ampton(objtype *ob, objtype *hit);
void R_Ampton(objtype *ob);

extern statetype s_slicestarslide;
extern statetype s_slicestarbounce;
extern statetype s_slicestarboom;

void SpawnSlicestarSlide(Uint16 tileX, Uint16 tileY, Sint16 dir);
void SpawnSlicestarBounce(Uint16 tileX, Uint16 tileY);
void C_Slicestar(objtype *ob, objtype *hit);
void R_Slicestar(objtype *ob);

extern statetype s_shellywalk1;
extern statetype s_shellywalk2;
extern statetype s_shellywalk3;
extern statetype s_shellywalk4;
extern statetype s_shellylook;
extern statetype s_shellylook2;
extern statetype s_shellyjump1;
extern statetype s_shellyjump2;
extern statetype s_shellydie;
extern statetype s_shellydieup;
extern statetype s_shellyboom1;
extern statetype s_shellyboom2;
extern statetype s_shellyboom3;
extern statetype s_shellyboom4;
extern statetype s_shellypiece1;
extern statetype s_shellypiece2;

void SpawnShelly(Uint16 tileX, Uint16 tileY);
void T_ShellyLook(objtype *ob);
void T_Turn(objtype *ob);
void T_ShellyFrag(objtype *ob);
void C_Shelly(objtype *ob, objtype *hit);
void R_Shelly(objtype *ob);
void R_Shell(objtype *ob);

/*
=============================================================================

						K5_ACT3 DEFINITIONS

=============================================================================
*/

extern statetype s_mine;
extern statetype s_minecenter;
extern statetype s_mineshift;
extern statetype s_mineboom1;
extern statetype s_mineboom2;
extern statetype s_mineboom3;
extern statetype s_mineboom4;
extern statetype s_mineboom5;
extern statetype s_mineboom6;
extern statetype s_minepiece;

void SpawnMine(Uint16 tileX, Uint16 tileY);
boolean MinePosCheck(Uint16 tileX, Uint16 tileY);
boolean Walk(objtype *ob, Sint16 dir);
void ChaseThink(objtype *ob);
void T_Mine(objtype *ob);
void C_Solid(objtype *ob, objtype *hit);
void C_MineFrag(objtype *ob, objtype *hit);
void T_MineCenter(objtype *ob);
void T_MineShift(objtype *ob);
void T_MineFrag(objtype *ob);
void R_Mine(objtype *ob);

extern statetype s_robored;
extern statetype s_roboredfire0;
extern statetype s_roboredfire1;
extern statetype s_roboredfire2;
extern statetype s_rshot1;
extern statetype s_rshot2;
extern statetype s_rshothit1;
extern statetype s_rshothit2;

void SpawnRoboRed(Uint16 tileX, Uint16 tileY);
void T_RoboRed(objtype *ob);
void C_RoboRed(objtype *ob, objtype *hit);
void T_RoboShoot(objtype *ob);
void C_RShot(objtype *ob, objtype *hit);
void R_RShot(objtype *ob);

extern statetype s_gripsitd;
extern statetype s_gripjumpd;
extern statetype s_gripsitl;
extern statetype s_gripjumpl;
extern statetype s_gripsitr;
extern statetype s_gripjumpr;
extern statetype s_gripsitu;
extern statetype s_gripjumpu;
extern statetype s_gripspin1;
extern statetype s_gripspin2;
extern statetype s_gripspin3;
extern statetype s_gripspin4;
extern statetype s_gripspin5;
extern statetype s_gripspin6;
extern statetype s_gripspin7;
extern statetype s_gripspin8;
extern statetype s_gripflyd;
extern statetype s_gripflyl;
extern statetype s_gripflyr;
extern statetype s_gripflyu;

void SpawnSpirogrip(Uint16 tileX, Uint16 tileY);
void T_SpiroLaunch(objtype *ob);
void R_SpiroFly(objtype *ob);

extern statetype s_spindred1;
extern statetype s_spindred2;
extern statetype s_spindred3;
extern statetype s_spindred4;

void SpawnSpindread(Uint16 tileX, Uint16 tileY);
void T_Spindread(objtype *ob);
void C_Spindread(objtype *ob, objtype *hit);
void R_Spindread(objtype *ob);

extern statetype s_master1;
extern statetype s_master2;
extern statetype s_master3;
extern statetype s_master4;
extern statetype s_mastershoot1;
extern statetype s_mastershoot2;
extern statetype s_mastertport1;
extern statetype s_mastertport2;
extern statetype s_mastertport3;
extern statetype s_mastertport4;
extern statetype s_mshot1;
extern statetype s_mshot2;
extern statetype s_mshot3;
extern statetype s_mshot4;
extern statetype s_mspray1;
extern statetype s_mspray2;
extern statetype s_mspray3;
extern statetype s_mspray4;

void SpawnMaster(Uint16 tileX, Uint16 tileY);
void T_Master(objtype *ob);
void T_MasterShoot(objtype *ob);
void C_Master(objtype *ob, objtype *hit);
void T_MasterTPort(objtype *ob);
void C_MShot(objtype *ob, objtype *hit);
void R_MShot(objtype *ob);
void R_MSpray(objtype *ob);

extern statetype s_shikadi1;
extern statetype s_shikadi2;
extern statetype s_shikadi3;
extern statetype s_shikadi4;
extern statetype s_shikadiwalk1;
extern statetype s_shikadiwalk2;
extern statetype s_shikadiwalk3;
extern statetype s_shikadiwalk4;
extern statetype s_shikadigrab;
extern statetype s_shikadigrab2;
extern statetype s_shikadistun;
extern statetype s_polespark1;
extern statetype s_polespark2;

void SpawnShikadi(Uint16 tileX, Uint16 tileY);
void T_Shikadi(objtype *ob);
void C_Shikadi(objtype *ob, objtype *hit);
void T_PoleShock(objtype *ob);
void T_PoleSpark(objtype *ob);
void R_Shikadi(objtype *ob);

extern statetype s_petsit1;
extern statetype s_petsit2;
extern statetype s_petbark1;
extern statetype s_petbark2;
extern statetype s_pet1;
extern statetype s_pet2;
extern statetype s_pet3;
extern statetype s_pet4;
extern statetype s_petjump;
extern statetype s_pshot1;
extern statetype s_pshot2;
extern statetype s_pshothot1;
extern statetype s_pshothot2;
extern statetype s_petstun;

void SpawnPet(Uint16 tileX, Uint16 tileY);
void T_Pet(objtype *ob);
void T_PetSit(objtype *ob);
void T_PetBark(objtype *ob);
void C_Pet(objtype *ob, objtype *hit);
void R_Pet(objtype *ob);
void R_PetJump(objtype *ob);
void C_PShot(objtype *ob, objtype *hit);
void R_PShot(objtype *ob);

extern statetype s_sphereful1;
extern statetype s_sphereful2;
extern statetype s_sphereful3;
extern statetype s_sphereful4;

void SpawnSphereful(Uint16 tileX, Uint16 tileY);
void T_Sphereful(objtype *ob);
void R_Sphereful(objtype *ob);

extern statetype s_scottie1;
extern statetype s_scottie2;
extern statetype s_scottie3;
extern statetype s_scottie4;
extern statetype s_scottieface;
extern statetype s_scottiestun;

void SpawnScottie(Uint16 tileX, Uint16 tileY);
void T_Scottie(objtype *ob);
void C_Scottie(objtype *ob, objtype *hit);

extern statetype s_qed;

void SpawnQed(Uint16 tileX, Uint16 tileY);

#endif