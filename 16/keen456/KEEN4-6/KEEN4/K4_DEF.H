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

#ifndef __K4_DEF__
#define __K4_DEF__

/*
=============================================================================

						GLOBAL CONSTANTS

=============================================================================
*/

#if GRMODE == CGAGR
#define MINMEMORY 280000l
#else
#define MINMEMORY 310000l
#endif

#define STARPALETTE   {0, 1, 2, 3, 4, 16, 6, 7, 31, 31, 31, 31, 31, 31, 31, 31, 0}
#define INTROPALETTE  {0, 24, 24, 7, 1, 1, 1, 1, 17, 17, 17, 17, 19, 19, 19, 19, 0}
#define SHRINKPALETTE {0, 24, 24, 7, 1, 1, 1, 1, 17, 17, 17, 17, 19, 19, 19, 24, 0}

#define HIGHSCORE_LEFT	24
#define HIGHSCORE_TOP	51
#define HIGHSCORE_RIGHT	296
#define HIGHSCORE_MAP	19

#define STATUS_PRESSKEY_X 160

#define WORLDMAPNAME "Shadowlands"
#define DROPSNAME "DROPS"

#define STARWARSMUSIC	12
#define ENDINGMUSIC	7

// levels in this range can NOT be re-entered (BWB level should be > MAXDONELEVEL)
#define MINDONELEVEL 1
#define MAXDONELEVEL 17

#define INACTIVATEDIST 4

/*
=============================================================================

						K4_SPEC DEFINITIONS

=============================================================================
*/

extern char far swtext[];
extern char far *levelnames[GAMELEVELS];
extern char far *levelenter[GAMELEVELS];

void ScanInfoPlane(void);
void PrincessLindsey(void);
void RescueJanitor(void);
void CantSwim(void);
void GotScuba(void);
void RescuedMember(void);

extern statetype s_keenswimslow1;
extern statetype s_keenswimslow2;
extern statetype s_keenswim1;
extern statetype s_keenswim2;
extern statetype s_kbubble1;
extern statetype s_kbubble2;
extern statetype s_kbubble3;
extern statetype s_kbubble4;
void SpawnSwimKeen(Sint16 x, Sint16 y);
void SpawnKbubble(objtype *ob);
void T_KeenSwimSlow(objtype *ob);
void T_KeenSwim(objtype *ob);
void C_KeenSwim(objtype *ob, objtype *hit);
void R_KeenSwim(objtype *ob);

/*
=============================================================================

						K4_ACT1 DEFINITIONS

=============================================================================
*/

extern statetype s_miragia0;
extern statetype s_miragia1;
extern statetype s_miragia2;
extern statetype s_miragia3;
extern statetype s_miragia4;
extern statetype s_miragia5;
extern statetype s_miragia6;
extern statetype s_miragia7;
void SpawnMiragia(Sint16 x, Sint16 y);
void T_Miragia0(objtype *ob);
void T_Miragia1(objtype *ob);
void T_Miragia2(objtype *ob);
void T_Miragia3(objtype *ob);
void T_Miragia4(objtype *ob);
void T_Miragia5(objtype *ob);
void T_Miragia6(objtype *ob);
void T_Miragia7(objtype *ob);

extern statetype s_bonus1;
extern statetype s_bonus2;
extern statetype s_bonusrise;
extern statetype s_splash1;
extern statetype s_splash2;
extern statetype s_splash3;
extern Uint16 bonusshape[];
void SpawnBonus(Sint16 x, Sint16 y, Sint16 type);
void SpawnSplash(Sint16 x, Sint16 y);
void T_Bonus(objtype *ob);

extern statetype s_councilwalk1;
extern statetype s_councilwalk2;
extern statetype s_councilstand;
void SpawnCouncil(Sint16 x, Sint16 y);
void T_Council(objtype *ob);

extern statetype s_slugwalk1;
extern statetype s_slugwalk2;
extern statetype s_slugpiss1;
extern statetype s_slugstun;
extern statetype s_slugstunalt;
extern statetype s_slugslime;
extern statetype s_slugslime2;
void SpawnSlug(Sint16 x, Sint16 y);
void T_Slug(objtype *ob);
void T_SlugPiss(objtype *ob);
void C_Slug(objtype *ob, objtype *hit);

extern statetype s_mushroom1;
extern statetype s_mushroom2;
void SpawnMadMushroom(Sint16 x, Sint16 y);
void T_Mushroom(objtype *ob);
void C_Mushroom(objtype *ob, objtype *hit);
void R_Mushroom(objtype *ob);

extern statetype s_egg;
extern statetype s_eggbroke;
extern statetype s_eggchip1;
extern statetype s_eggchip2;
extern statetype s_eggchip3;
extern statetype s_eggbirdpause;
extern statetype s_eggbirdwalk1;
extern statetype s_eggbirdwalk2;
extern statetype s_eggbirdwalk3;
extern statetype s_eggbirdwalk4;
extern statetype s_eggbirdfly1;
extern statetype s_eggbirdfly2;
extern statetype s_eggbirdfly3;
extern statetype s_eggbirdfly4;
extern statetype s_eggbirddrop;
extern statetype s_eggbirdstun;
extern statetype s_eggbirdstun2;
extern statetype s_eggbirdstun3;
extern statetype s_eggbirdstun4;
extern statetype s_eggbirdstun5;
void SpawnEggbird(Sint16 x, Sint16 y);
void T_EggUnstun(objtype *ob);
void SpawnEggbirdOut(Sint16 x, Sint16 y);
void C_Egg(objtype *ob, objtype *hit);
void T_Eggbird(objtype *ob);
void T_EggbirdFly(objtype *ob);
void C_Eggbird(objtype *ob, objtype *hit);
void C_EggbirdStun(objtype *ob, objtype *hit);
void R_Eggbird(objtype *ob);
void R_EggbirdDrop(objtype *ob);
void R_Chip(objtype *ob);
void R_Eggbirdfly(objtype *ob);

extern statetype s_arach1;
extern statetype s_arach2;
extern statetype s_arach3;
extern statetype s_arach4;
extern statetype s_arachstun;
extern statetype s_arachstun2;
extern statetype s_arachstun3;
extern statetype s_arachstun4;
extern statetype s_arachstun5;
void SpawnArachnut(Sint16 x, Sint16 y);
void T_Arach(objtype *ob);
void C_Arach(objtype *ob, objtype *hit);
void C_ArachStun(objtype *ob, objtype *hit);

extern statetype s_pestfly1;
extern statetype s_pestfly2;
extern statetype s_squashedpest;
extern statetype s_pestrest1;
extern statetype s_pestrest2;
extern statetype s_pestrest3;
extern statetype s_pestrest4;
extern statetype s_pestrest5;
extern statetype s_pestrest6;
extern statetype s_pestrest7;
extern statetype s_pestrest8;
extern statetype s_pestrest9;
extern statetype s_pestrest10;
extern statetype s_pestrest11;
extern statetype s_pestrest12;
extern statetype s_pestrest13;
extern statetype s_pestrest14;
extern statetype s_pestrest15;
extern statetype s_pestrest16;
extern statetype s_pestrest17;
void SpawnSkypest(Sint16 x, Sint16 y);
void T_PestFly(objtype *ob);
void C_PestFly(objtype *ob, objtype *hit);
void C_Squashable(objtype *ob, objtype *hit);
void T_PestRest(objtype *ob);
void R_Pest(objtype *ob);

/*
=============================================================================

						K4_ACT2 DEFINITIONS

=============================================================================
*/

extern statetype s_worm;
extern statetype s_wormpeek1;
extern statetype s_wormpeek2;
extern statetype s_wormpeek3;
extern statetype s_wormpeek4;
extern statetype s_wormpeek5;
extern statetype s_wormpeek6;
extern statetype s_wormpeek7;
extern statetype s_wormpeek8;
extern statetype s_wormbite1;
extern statetype s_wormbite2;
extern statetype s_wormbite3;
extern statetype s_wormbite4;
extern statetype s_wormbite5;
extern statetype s_wormstun;
void SpawnWormMouth(Sint16 x, Sint16 y);
void T_WormLookRight(objtype *ob);
void T_WormLook(objtype *ob);
void T_WormLookLeft(objtype *ob);
void T_Worm(objtype *ob);
void C_Worm(objtype *ob, objtype *hit);
void C_WormKill(objtype *ob, objtype *hit);

extern statetype s_cloudsleep;
extern statetype s_cloudwake;
extern statetype s_cloud;
extern statetype s_cloudalign;
extern statetype s_cloudcharge;
extern statetype s_cloudattack1;
extern statetype s_cloudattack2;
extern statetype s_cloudattack3;
extern statetype s_cloudattack4;
extern statetype s_cloudattack5;
extern statetype s_cloudattack6;
extern statetype s_cloudattack7;
extern statetype s_cloudattack8;
extern statetype s_cloudattack9;
extern statetype s_bolt1;
extern statetype s_bolt2;
extern statetype s_bolt3;
extern statetype s_bolt4;
extern statetype s_bolt5;
extern statetype s_bolt6;
void SpawnCloudster(Sint16 x, Sint16 y);
void T_Cloud(objtype *ob);
void T_CloudAlign(objtype *ob);
void R_Cloud(objtype *ob);
void T_CloudShoot(objtype *ob);
void C_CloudSleep(objtype *ob, objtype *hit);

extern statetype s_berkefloat1;
extern statetype s_berkefloat2;
extern statetype s_berkefloat3;
extern statetype s_berkefloat4;
extern statetype s_berkethrow1;
extern statetype s_berkethrow2;
extern statetype s_berkethrow3;
extern statetype s_berkethrow4;
extern statetype s_berkethrow5;
extern statetype s_berkethrow6;
extern statetype s_berkethrow7;
extern statetype s_berkethrow8;
extern statetype s_berkethrow9;
extern statetype s_berkethrow10;
extern statetype s_berkethrow11;
extern statetype s_berkethrow12;
extern statetype s_fire1;
extern statetype s_fire2;
extern statetype s_fireland1;
extern statetype s_fireland2;
extern statetype s_fireland3;
extern statetype s_fireland4;
extern statetype s_fireland5;
extern statetype s_fireland6;
extern statetype s_fireland7;
extern statetype s_fireland8;
extern statetype s_fireland9;
void SpawnBerkeloid(Sint16 x, Sint16 y);
void BerkeThink(objtype *ob);
void BerkeThrowThink(objtype *ob);
void BerkeThrowDone(objtype *ob);
void C_Berke(objtype *ob, objtype *hit);
void FireReact(objtype *ob);
void BerkeDrawReact(objtype *ob);
void BerkeWalkReact(objtype *ob);

extern statetype s_footsmoke1;
extern statetype s_footsmoke2;
extern statetype s_footsmoke3;
extern statetype s_footsmoke4;
extern statetype s_inch1;
extern statetype s_inch2;
extern statetype s_footchange;	//never used!
extern statetype s_footwait;
void SpawnInchworm(Sint16 x, Sint16 y);
void SpawnFoot(Sint16 x, Sint16 y);
void InchThink(objtype *ob);
void InchContact(objtype *ob, objtype *hit);
void FootContact(objtype *ob, objtype *hit);

extern statetype s_bounderup1;
extern statetype s_bounderup2;
extern statetype s_bounderside1;
extern statetype s_bounderside2;
extern statetype s_bounderstun;
extern statetype s_bounderstun2;
void SpawnBounder(Sint16 x, Sint16 y);
void C_Bounder(objtype *ob, objtype *hit);
void R_Bounder(objtype *ob);

extern statetype s_lick1;
extern statetype s_lick2;
extern statetype s_lick3;
extern statetype s_lick4;
extern statetype s_licklick1;
extern statetype s_licklick2;
extern statetype s_licklick3;
extern statetype s_licklick4;
extern statetype s_licklick5;
extern statetype s_licklick6;
extern statetype s_licklick7;
extern statetype s_licklick8;
extern statetype s_lickstun;
extern statetype s_lickstun2;
void SpawnLick(Sint16 x, Sint16 y);
void LickJumpThink(objtype *ob);
void LickContact(objtype *ob, objtype *hit);
void LickKillContact(objtype *ob, objtype *hit);
void LickAirReact(objtype *ob);

extern statetype s_platform;
void SpawnPlatform(Sint16 x, Sint16 y, Sint16 dir);
void T_Platform(objtype *ob);
void R_Platform(objtype *ob);

extern statetype s_dropplatsit;
extern statetype s_dropplatfall;
extern statetype s_dropplatrise;
void SpawnDropPlat(Sint16 x, Sint16 y);
void T_DropPlatSit(objtype *ob);
void T_DropPlatFall(objtype *ob);
void T_DropPlatRise(objtype *ob);

/*
=============================================================================

						K4_ACT3 DEFINITIONS

=============================================================================
*/

extern statetype s_eaterstand1;
extern statetype s_eaterstand2;
extern statetype s_eatertport1;
extern statetype s_eatertport2;
extern statetype s_eatertport3;
extern statetype s_eatertport4;
extern statetype s_eatertport5;
extern statetype s_eatertport6;
extern statetype s_eatertport7;
extern statetype s_eatertport8;
extern statetype s_eaterjump1;
extern statetype s_eaterjump2;
extern statetype s_eaterjump3;
extern statetype s_eaterjump4;
extern statetype s_eaterstun;
extern statetype s_eaterstun2;
extern statetype s_eatenbonus1;
extern statetype s_eatenbonus2;
extern statetype s_eatenbonus3;
extern statetype s_eatenbonus4;
void SpawnEater(Sint16 x, Sint16 y);
void T_EaterJump(objtype *ob);
void T_EaterTeleport(objtype *ob);
void C_Eater(objtype *ob, objtype *hit);
void EaterInTile(objtype *ob);
void R_EaterAir(objtype *ob);

extern statetype s_mimrock;
extern statetype s_mimsneak1;
extern statetype s_mimsneak2;
extern statetype s_mimsneak3;
extern statetype s_mimsneak4;
extern statetype s_mimsneak5;
extern statetype s_mimsneak6;
extern statetype s_mimbonk1;
extern statetype s_mimbonk2;
extern statetype s_mimbonk3;
extern statetype s_mimbounce;
extern statetype s_mimstun;
extern statetype s_mimstun2;
void SpawnMimrock(Sint16 x, Sint16 y);
void T_MimrockWait(objtype *ob);
void T_MimrockSneak(objtype *ob);
void C_Mimrock(objtype *ob, objtype *hit);
void C_MimLethal(objtype *ob, objtype *hit);
void R_MimAir(objtype *ob);
void R_MimBounce(objtype *ob);

extern statetype s_dopefish1;
extern statetype s_dopefish2;
extern statetype s_dopeattack;
extern statetype s_dopeeat;
extern statetype s_dopeburp1;
extern statetype s_dopeburp2;
extern statetype s_dopereturn;
extern statetype s_dopefood;
extern statetype s_keendopefood;
extern statetype s_keendieslow;
extern statetype s_bubble1;
extern statetype s_bubble2;
extern statetype s_bubble3;
extern statetype s_bubble4;
void SpawnDopefish(Sint16 x, Sint16 y);
void T_EatenKeen(objtype *ob);
void T_Dope(objtype *ob);
void T_DopeHunt(objtype *ob);
void T_DopeReturn(objtype *ob);
void T_Burp(objtype *ob);
void T_Bubble(objtype *ob);
void C_Dope(objtype *ob, objtype *hit);
void R_Fish(objtype *ob);

extern statetype s_schoolfish1;
extern statetype s_schoolfish2;
void SpawnSchoolfish(Sint16 x, Sint16 y);
void T_SchoolFish(objtype *ob);

extern statetype s_pixie;
extern statetype s_pixielook;
extern statetype s_pixieshoot;
extern statetype s_pixieshoot2;
extern statetype s_pixiefire1;
extern statetype s_pixiefire2;
extern statetype s_pixiefire3;
extern statetype s_pixiefire4;
void SpawnPixie(Sint16 x, Sint16 y);
void T_Pixie(objtype *ob);
void T_PixieCheck(objtype *ob);
void T_PixieShoot(objtype *ob);
void R_Mshot(objtype *ob);

extern statetype s_mine;
extern statetype s_mineboom1;
extern statetype s_mineboom2;
void SpawnMine(Sint16 x, Sint16 y, Sint16 dir);
void C_Mine(objtype *ob, objtype *hit);

extern statetype s_lindsey1;
extern statetype s_lindsey2;
extern statetype s_lindsey3;
extern statetype s_lindsey4;
void SpawnLindsey(Sint16 x, Sint16 y);
void T_Lindsey(objtype *ob);

extern statetype s_dartthrower;
extern statetype s_dart1;
extern statetype s_dart2;
extern statetype s_dartup1;
extern statetype s_dartup2;
extern statetype s_dartdown1;
extern statetype s_dartdown2;
void SpawnDartShooter(Sint16 x, Sint16 y, Sint16 dir);
void T_DartShoot(objtype *ob);
void R_DartThrower(objtype *ob);

extern statetype s_scuba;
void SpawnScuba(Sint16 x, Sint16 y);
void C_Scuba(objtype *ob, objtype *hit);

#endif