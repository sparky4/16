/* Reconstructed Commander Keen 4-6 Source Code
 * Copyright (C) 2021 K1n9_Duk3
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

#ifndef __GFX_H__
#define __GFX_H__

//#include "VERSION.H"

//////////////////////////////////////
//
// Graphics .H file for .CK4
// not IGRAB-ed :)
//
//////////////////////////////////////

//
// Lump creation macros
//

#define START_LUMP(actualname, dummyname) actualname, dummyname=actualname-1,
#define END_LUMP(actualname, dummyname) dummyname, actualname=dummyname-1,

//
// Amount of each data item
//

//common numbers:
#define NUMCHUNKS    NUMGRCHUNKS
#define NUMFONT      3
#define NUMFONTM     0
#define NUMPICM      3
#define NUMTILE8     108	// BUG: only 104 tiles exist in EGAGRAPH!
#define NUMTILE8M    36		// BUG: only 20 tiles exist in EGAGRAPH!
#define NUMTILE32    0
#define NUMTILE32M   0

//episode-specific numbers:
#define NUMPICS      115
#define NUMSPRITES   397
#define NUMTILE16    1296
#define NUMTILE16M   2916
#define NUMEXTERNS   16

//
// File offsets for data items
//
#define STRUCTPIC    0
#define STRUCTPICM   1
#define STRUCTSPRITE 2

#define STARTFONT    3
#define STARTFONTM   (STARTFONT+NUMFONT)
#define STARTPICS    (STARTFONTM+NUMFONTM)
#define STARTPICM    (STARTPICS+NUMPICS)
#define STARTSPRITES (STARTPICM+NUMPICM)
#define STARTTILE8   (STARTSPRITES+NUMSPRITES)
#define STARTTILE8M  (STARTTILE8+1)
#define STARTTILE16  (STARTTILE8M+1)
#define STARTTILE16M (STARTTILE16+NUMTILE16)
#define STARTTILE32  (STARTTILE16M+NUMTILE16M)
#define STARTTILE32M (STARTTILE32+NUMTILE32)
#define STARTEXTERNS (STARTTILE32M+NUMTILE32M)

typedef enum {
	LASTFONT=STARTPICS-1,

	//
	// PICS
	//

	H_HELPPIC,                   // 6
	H_LARROWPIC,                 // 7
	H_RARROWPIC,                 // 8
	H_ESCPIC,                    // 9
	H_ENTERPIC,                  // 10
	DUMMYPIC,                    // 11
	H_STORY1PIC,                 // 12
	H_STORY2PIC,                 // 13
	H_STORY3PIC,                 // 14
	H_STORY4PIC,                 // 15
	STORY5PIC,                   // 16
	STORY6PIC,                   // 17
	STORY7PIC,                   // 18
	STORY8PIC,                   // 19
	ITEM1PIC,                    // 20
	ITEM2PIC,                    // 21
	ITEM3PIC,                    // 22
	ITEM4PIC,                    // 23
	ITEM5PIC,                    // 24
	ITEM6PIC,                    // 25
	ITEM7PIC,                    // 26
	ITEM8PIC,                    // 27
	ITEM9PIC,                    // 28
	ARACHNUTPIC,                 // 29
	BERKELOISPIC,                // 30
	BOUNDERPIC,                  // 31
	COUNCILMEMBERPIC,            // 32
	DOPEFISHPIC,                 // 33
	INCHWORMPIC,                 // 34
	LICKPIC,                     // 35
	MADMUSHROOMPIC,              // 36
	POISONSLIGPIC,               // 37
	PRINCESSLINDSEYPIC,          // 38
	SCHOOLFISHPIC,               // 39
	SKYPESTPIC,                  // 40
	SPRITEPIC,                   // 41
	WORMOUTHPIC,                 // 42
	ENDOFTEXTPIC,                // 43
	H_MCPIC,                     // 44
	H_HANDPIC,                   // 45
	H_VISAPIC,                   // 46
	H_FLASHARROW1PIC,            // 47
	H_FLASHARROW2PIC,            // 48
	ENDINDG1PIC,                 // 49
	ENDINDG2PIC,                 // 50
	ENDINDG3PIC,                 // 51
	ENDINDG4PIC,                 // 52
	ENDINDG5PIC,                 // 53
	ENDINDG6PIC,                 // 54
	ENDINDG7PIC,                 // 55
	ENDINDG8PIC,                 // 56
	ENDINDG9PIC,                 // 57
	ENDINDG10PIC,                // 58
	ENDINDG11PIC,                // 59
	ENDINDG12PIC,                // 60
	ENDINDG13PIC,                // 61
	ENDINDG14PIC,                // 62
	ENDINDG15PIC,                // 63
	ENDINDG16PIC,                // 64
	ENDINDG17PIC,                // 65
	ENDINDG18PIC,                // 66
	ENDINDG19PIC,                // 67
	ENDINDG20PIC,                // 68
	ENDINDG21PIC,                // 69
	ENDINDG22PIC,                // 70
	ENDINDG23PIC,                // 71
	ENDINDG24PIC,                // 72
	ENDINDG25PIC,                // 73
	ENDINDG26PIC,                // 74
	ENDINDG27PIC,                // 75
	ENDINDG28PIC,                // 76
	ENDINDG29PIC,                // 77
	ENDINDG30PIC,                // 78
	H_IDLOGOPIC,                 // 79
	H_TOPWINDOWPIC,              // 80
	H_LEFTWINDOWPIC,             // 81
	H_RIGHTWINDOWPIC,            // 82
	H_BOTTOMINFOPIC,             // 83
	H_BOTTOMWINDOWPIC,           // 84
	H_BARPIC,                    // 85
	H_KEEN5PIC,                  // 86
	H_KEEN6PIC,                  // 87

	START_LUMP(CONTROLS_LUMP_START, __CONTROLSSTART)
	CP_MAINMENUPIC,              // 88
	CP_NEWGAMEMENUPIC,           // 89
	CP_LOADMENUPIC,              // 90
	CP_SAVEMENUPIC,              // 91
	CP_CONFIGMENUPIC,            // 92
	CP_SOUNDMENUPIC,             // 93
	CP_MUSICMENUPIC,             // 94
	CP_KEYBOARDMENUPIC,          // 95
	CP_KEYMOVEMENTPIC,           // 96
	CP_KEYBUTTONPIC,             // 97
	CP_JOYSTICKMENUPIC,          // 98
	CP_OPTIONSMENUPIC,           // 99
	CP_PADDLEWARPIC,             // 100
	CP_QUITPIC,                  // 101
	CP_JOYSTICKPIC,              // 102
	CP_MENUSCREENPIC,            // 103
	END_LUMP(CONTROLS_LUMP_END, __CONTROLSEND)

	IDSOFTPIC,                   // 104
	PROGTEAMPIC,                 // 105
	ARTISTPIC,                   // 106
	DIRECTORPIC,                 // 107
	SW_BACKGROUNDPIC,            // 108
	TITLEPICPIC,                 // 109
	ORACLEPIC,                   // 110
	KEENTALK1PIC,                // 111
	KEENTALK2PIC,                // 112
	KEENMADPIC,                  // 113
	LINDSEYPIC,                  // 114
	KEENCOUNT1PIC,               // 115
	KEENCOUNT2PIC,               // 116
	KEENCOUNT3PIC,               // 117
	KEENCOUNT4PIC,               // 118
	KEENCOUNT5PIC,               // 119
	KEENCOUNT6PIC,               // 120

	//
	// MASKED PICS
	//

	CP_MENUMASKPICM,             // 121
	CORDPICM,                    // 122
	METALPOLEPICM,               // 123

	//
	// SPRITES
	//

	START_LUMP(PADDLE_LUMP_START, __PADDLESTART)
	PADDLESPR,                   // 124
	BALLSPR,                     // 125
	BALL1PIXELTOTHERIGHTSPR,     // 126
	BALL2PIXELSTOTHERIGHTSPR,    // 127
	BALL3PIXELSTOTHERIGHTSPR,    // 128
	END_LUMP(PADDLE_LUMP_END, __PADDLEEND)

	DEMOPLAQUESPR,               // 129

	//player lump:
	START_LUMP(KEEN_LUMP_START, __KEENSTART)
	KEENSTANDRSPR,               // 130
	KEENRUNR1SPR,                // 131
	KEENRUNR2SPR,                // 132
	KEENRUNR3SPR,                // 133
	KEENRUNR4SPR,                // 134
	KEENJUMPR1SPR,               // 135
	KEENJUMPR2SPR,               // 136
	KEENJUMPR3SPR,               // 137
	KEENSTANDLSPR,               // 138
	KEENRUNL1SPR,                // 139
	KEENRUNL2SPR,                // 140
	KEENRUNL3SPR,                // 141
	KEENRUNL4SPR,                // 142
	KEENJUMPL1SPR,               // 143
	KEENJUMPL2SPR,               // 144
	KEENJUMPL3SPR,               // 145
	KEENLOOKUSPR,                // 146
	KEENWAITR1SPR,               // 147
	KEENWAITR2SPR,               // 148
	KEENWAITR3SPR,               // 149
	KEENSITREAD1SPR,             // 150
	KEENSITREAD2SPR,             // 151
	KEENSITREAD3SPR,             // 152
	KEENSITREAD4SPR,             // 153
	KEENREAD1SPR,                // 154
	KEENREAD2SPR,                // 155
	KEENREAD3SPR,                // 156
	KEENSTOPREAD1SPR,            // 157
	KEENSTOPREAD2SPR,            // 158
	KEENWATCHSPR,                // 159
	KEENLOOKD1SPR,               // 160
	KEENLOOKD2SPR,               // 161
	KEENDIE1SPR,                 // 162
	KEENDIE2SPR,                 // 163
	STUNSTARS1SPR,               // 164
	STUNSTARS2SPR,               // 165
	STUNSTARS3SPR,               // 166
	KEENSHOOTLSPR,               // 167
	KEENJLSHOOTLSPR,             // 168
	KEENJSHOOTDSPR,              // 169
	KEENJSHOOTUSPR,              // 170
	KEENSHOOTUSPR,               // 171
	KEENSHOOTRSPR,               // 172
	KEENJRSHOOTRSPR,             // 173
	STUN1SPR,                    // 174
	STUN2SPR,                    // 175
	STUN3SPR,                    // 176
	STUN4SPR,                    // 177
	STUNHIT1SPR,                 // 178
	STUNHIT2SPR,                 // 179
	KEENSHINNYR1SPR,             // 180
	KEENSHINNYR2SPR,             // 181
	KEENSHINNYR3SPR,             // 182
	KEENSLIDED1SPR,              // 183
	KEENSLIDED2SPR,              // 184
	KEENSLIDED3SPR,              // 185
	KEENSLIDED4SPR,              // 186
	KEENSHINNYL1SPR,             // 187
	KEENSHINNYL2SPR,             // 188
	KEENSHINNYL3SPR,             // 189
	KEENPLSHOOTUSPR,             // 190
	KEENPRSHOOTUSPR,             // 191
	KEENPRSHOOTDSPR,             // 192
	KEENPLSHOOTDSPR,             // 193
	KEENPSHOOTLSPR,              // 194
	KEENPSHOOTRSPR,              // 195
	KEENENTER1SPR,               // 196
	KEENENTER2SPR,               // 197
	KEENENTER3SPR,               // 198
	KEENENTER4SPR,               // 199
	KEENENTER5SPR,               // 200
	KEENHANGLSPR,                // 201
	KEENHANGRSPR,                // 202
	KEENCLIMBEDGEL1SPR,          // 203
	KEENCLIMBEDGEL2SPR,          // 204
	KEENCLIMBEDGEL3SPR,          // 205
	KEENCLIMBEDGEL4SPR,          // 206
	KEENCLIMBEDGER1SPR,          // 207
	KEENCLIMBEDGER2SPR,          // 208
	KEENCLIMBEDGER3SPR,          // 209
	KEENCLIMBEDGER4SPR,          // 210
	KEENPOGOR1SPR,               // 211
	KEENPOGOR2SPR,               // 212
	KEENPOGOL1SPR,               // 213
	KEENPOGOL2SPR,               // 214
	DROPSPLASH1SPR,              // 215
	DROPSPLASH2SPR,              // 216
	DROPSPLASH3SPR,              // 217
	BONUS100UPSPR,               // 218
	BONUS100SPR,                 // 219
	BONUS200SPR,                 // 220
	BONUS500SPR,                 // 221
	BONUS1000SPR,                // 222
	BONUS2000SPR,                // 223
	BONUS5000SPR,                // 224
	BONUS1UPSPR,                 // 225
	BONUSCLIPSPR,                // 226
	END_LUMP(KEEN_LUMP_END, __KEENEND)

	START_LUMP(SUGAR1_LUMP_START, __SUGAR1START)
	SUGAR1ASPR,                  // 227
	SUGAR1BSPR,                  // 228
	END_LUMP(SUGAR1_LUMP_END, __SUGAR1END)

	START_LUMP(SUGAR2_LUMP_START, __SUGAR2START)
	SUGAR2ASPR,                  // 229
	SUGAR2BSPR,                  // 230
	END_LUMP(SUGAR2_LUMP_END, __SUGAR2END)

	START_LUMP(SUGAR3_LUMP_START, __SUGAR3START)
	SUGAR3ASPR,                  // 231
	SUGAR3BSPR,                  // 232
	END_LUMP(SUGAR3_LUMP_END, __SUGAR3END)

	START_LUMP(SUGAR4_LUMP_START, __SUGAR4START)
	SUGAR4ASPR,                  // 233
	SUGAR4BSPR,                  // 234
	END_LUMP(SUGAR4_LUMP_END, __SUGAR4END)

	START_LUMP(SUGAR5_LUMP_START, __SUGAR5START)
	SUGAR5ASPR,                  // 235
	SUGAR5BSPR,                  // 236
	END_LUMP(SUGAR5_LUMP_END, __SUGAR5END)

	START_LUMP(SUGAR6_LUMP_START, __SUGAR6START)
	SUGAR6ASPR,                  // 237
	SUGAR6BSPR,                  // 238
	END_LUMP(SUGAR6_LUMP_END, __SUGAR6END)

	START_LUMP(ONEUP_LUMP_START, __ONEUPSTART)
	ONEUPASPR,                   // 239
	ONEUPBSPR,                   // 240
	END_LUMP(ONEUP_LUMP_END, __ONEUPEND)

	DOORSPR,                     // 241

	START_LUMP(KEYGEM_LUMP_START, __KEYGEMSTART)
	REDGEM1SPR,                  // 242
	REDGEM2SPR,                  // 243
	YELLOWGEM1SPR,               // 244
	YELLOWGEM2SPR,               // 245
	BLUEGEM1SPR,                 // 246
	BLUEGEM2SPR,                 // 247
	GREENGEM1SPR,                // 248
	GREENGEM2SPR,                // 249
	BONUSGEMSPR,                 // 250
	END_LUMP(KEYGEM_LUMP_END, __KEYGEMEND)

	START_LUMP(AMMO_LUMP_START, __AMMOSTART)
	STUNCLIP1SPR,                // 251
	STUNCLIP2SPR,                // 252
	END_LUMP(AMMO_LUMP_END, __AMMOEND)

	SCOREBOXSPR,                 // 253

	START_LUMP(WORLDKEEN_LUMP_START, __WORLDKEENSTART)
	WORLDKEENL1SPR,              // 254
	WORLDKEENL2SPR,              // 255
	WORLDKEENL3SPR,              // 256
	WORLDKEENR1SPR,              // 257
	WORLDKEENR2SPR,              // 258
	WORLDKEENR3SPR,              // 259
	WORLDKEENU1SPR,              // 260
	WORLDKEENU2SPR,              // 261
	WORLDKEENU3SPR,              // 262
	WORLDKEEND1SPR,              // 263
	WORLDKEEND2SPR,              // 264
	WORLDKEEND3SPR,              // 265
	WORLDKEENDR1SPR,             // 266
	WORLDKEENDR2SPR,             // 267
	WORLDKEENDR3SPR,             // 268
	WORLDKEENDL1SPR,             // 269
	WORLDKEENDL2SPR,             // 270
	WORLDKEENDL3SPR,             // 271
	WORLDKEENUL1SPR,             // 272
	WORLDKEENUL2SPR,             // 273
	WORLDKEENUL3SPR,             // 274
	WORLDKEENUR1SPR,             // 275
	WORLDKEENUR2SPR,             // 276
	WORLDKEENUR3SPR,             // 277
	WORLDKEENWAVE1SPR,           // 278
	WORLDKEENWAVE2SPR,           // 279
	WORLDKEENSWIMU1SPR,          // 280
	WORLDKEENSWIMU2SPR,          // 281
	WORLDKEENSWIMR1SPR,          // 282
	WORLDKEENSWIMR2SPR,          // 283
	WORLDKEENSWIMD1SPR,          // 284
	WORLDKEENSWIMD2SPR,          // 285
	WORLDKEENSWIML1SPR,          // 286
	WORLDKEENSWIML2SPR,          // 287
	WORLDKEENSWIMUR1SPR,         // 288
	WORLDKEENSWIMUR2SPR,         // 289
	WORLDKEENSWIMDR1SPR,         // 290
	WORLDKEENSWIMDR2SPR,         // 291
	WORLDKEENSWIMDL1SPR,         // 292
	WORLDKEENSWIMDL2SPR,         // 293
	WORLDKEENSWIMUL1SPR,         // 294
	WORLDKEENSWIMUL2SPR,         // 295
	WOLRDKEENRIDE1SPR,           // 296
	WOLRDKEENRIDE2SPR,           // 297
	FLAGFLIP1SPR,                // 298
	FLAGFLIP2SPR,                // 299
	FLAGFLIP3SPR,                // 300
	FLAGFLIP4SPR,                // 301
	FLAGFLIP5SPR,                // 302
	FLAGFALL1SPR,                // 303
	FLAGFALL2SPR,                // 304
	FLAGFLAP1SPR,                // 305
	FLAGFLAP2SPR,                // 306
	FLAGFLAP3SPR,                // 307
	FLAGFLAP4SPR,                // 308
	END_LUMP(WORLDKEEN_LUMP_END, __WORLDKEENEND)

	START_LUMP(SCUBAKEEN_LUMP_START, __SCUBAKEENSTART)
	SCUBAKEENL1SPR,              // 309
	SCUBAKEENL2SPR,              // 310
	SCUBAKEENR1SPR,              // 311
	SCUBAKEENR2SPR,              // 312
	SCUBAKEENDEAD1SPR,           // 313
	SCUBAKEENDEAD2SPR,           // 314
	END_LUMP(SCUBAKEEN_LUMP_END, __SCUBAKEENEND)

	START_LUMP(SLUG_LUMP_START, __SLUGSTART)
	SLUGWALKR1SPR,               // 315
	SLUGWALKR2SPR,               // 316
	SLUGPISSRSPR,                 // 317
	SLUGSTUN1SPR,             // 318
	SLUGSTUN2SPR,             // 319
	SLUGWALKL1SPR,               // 320
	SLUGWALKL2SPR,               // 321
	SLUGPISSLSPR,                 // 322
	SLUGSLIME1SPR,               // 323
	SLUGSLIME2SPR,               // 324
	END_LUMP(SLUG_LUMP_END, __SLUGEND)

	START_LUMP(MADMUSHROOM_LUMP_START, __MADMUSHROOMSTART)
	MADMUSHROOML1SPR,            // 325
	MADMUSHROOML2SPR,            // 326
	MADMUSHROOMR1SPR,            // 327
	MADMUSHROOMR2SPR,            // 328
	END_LUMP(MADMUSHROOM_LUMP_END, __MADMUSHROOMEND)

	START_LUMP(LINDSEY_LUMP_START, __LINDSEYSTART)
	LINDSEY1SPR,                 // 329
	LINDSEY2SPR,                 // 330
	LINDSEY3SPR,                 // 331
	LINDSEY4SPR,                 // 332
	END_LUMP(LINDSEY_LUMP_END, __LINDSEYEND)

	START_LUMP(INCHWORM_LUMP_START, __INCHWORMSTART)
	INCHWORMR1SPR,               // 333
	INCHWORMR2SPR,               // 334
	INCHWORML1SPR,               // 335
	INCHWORML2SPR,               // 336
	FOOTSPR,                     // 337
	END_LUMP(INCHWORM_LUMP_END, __INCHWORMEND)

	START_LUMP(EATER_LUMP_START, __EATERSTART)
	EATERSTAND1SPR,              // 338
	EATERSTAND2SPR,              // 339
	EATERJUMPR1SPR,              // 340
	EATERJUMPR2SPR,              // 341
	EATERJUMPR3SPR,              // 342
	EATERJUMPL1SPR,              // 343
	EATERJUMPL2SPR,              // 344
	EATERJUMPL3SPR,              // 345
	EATENBONUS1SPR,              // 346
	EATENBONUS2SPR,              // 347
	EATENBONUS3SPR,              // 348
	EATENBONUS4SPR,              // 349
	SMOKE1SPR,                   // 350
	SMOKE2SPR,                   // 351
	SMOKE3SPR,                   // 352
	SMOKE4SPR,                   // 353
	SMOKE5SPR,                   // 354
	EATERSTUNSPR,                // 355
	END_LUMP(EATER_LUMP_END, __EATEREND)

	START_LUMP(COUNCIL_LUMP_START, __COUINCILSTART)
	COUNCILWALKR1SPR,            // 356
	COUNCILWALKR2SPR,            // 357
	COUNCILWALKL1SPR,            // 358
	COUNCILWALKL2SPR,            // 359
	COUNCILTHINKLSPR,            // 360
	COUNCILTHINKRSPR,            // 361
	END_LUMP(COUNCIL_LUMP_END, __COUNCILEND)

	START_LUMP(EGG_LUMP_START, __EGGSTART)
	EGGSPR,                      // 362
	EGGBROKESPR,                // 363
	EGGCHIP1SPR,                // 364
	EGGCHIP2SPR,                // 365
	EGGCHIP3SPR,                // 366
	END_LUMP(EGG_LUMP_END, __EGGEND)

	START_LUMP(EGGBIRD_LUMP_START, __EGGBIRDSTART)
	BIRDWALKR1SPR,               // 367
	BIRDWALKR2SPR,               // 368
	BIRDWALKR3SPR,               // 369
	BIRDWALKR4SPR,               // 370
	BIRDWALKL1SPR,               // 371
	BIRDWALKL2SPR,               // 372
	BIRDWALKL3SPR,               // 373
	BIRDWALKL4SPR,               // 374
	BIRDFLY1SPR,                 // 375
	BIRDFLY2SPR,                 // 376
	BIRDFLY3SPR,                 // 377
	BIRDFLY4SPR,                 // 378
	BIRDSTUNSPR,                 // 379
	END_LUMP(EGGBIRD_LUMP_END, __EGGBIRDEND)

	START_LUMP(DARTS_LUMP_START, __DARTSSTART)
	DARTU1SPR,                   // 380
	DARTU2SPR,                   // 381
	DARTD1SPR,                   // 382
	DARTD2SPR,                   // 383
	DARTR1SPR,                   // 384
	DARTR2SPR,                   // 385
	DARTL1SPR,                   // 386
	DARTL2SPR,                   // 387
	END_LUMP(DARTS_LUMP_END, __DARTSEND)

	START_LUMP(MIMROCK_LUMP_START, __MIMROCKSTART)
	MIMROCKSPR,                  // 388
	MIMROCKWALKL1SPR,            // 389
	MIMROCKWALKL2SPR,            // 390
	MIMROCKWALKL3SPR,            // 391
	MIMROCKWALKL4SPR,            // 392
	MIMROCKWALKR1SPR,            // 393
	MIMROCKWALKR2SPR,            // 394
	MIMROCKWALKR3SPR,            // 395
	MIMROCKWALKR4SPR,            // 396
	MIMROCKJUMPR1SPR,            // 397
	MIMROCKJUMPR2SPR,            // 398
	MIMROCKJUMPR3SPR,            // 399
	MIMROCKJUMPL1SPR,            // 400
	MIMROCKJUMPL2SPR,            // 401
	MIMROCKJUMPL3SPR,            // 402
	MINROCKSTUNSPR,              // 403
	END_LUMP(MIMROCK_LUMP_END, __MIMROCKEND)

	START_LUMP(DOPEFISH_LUMP_START, __DOPEFISHSTART)
	DOPEFISHSWIMR1SPR,           // 404
	DOPEFISHSWIMR2SPR,           // 405
	DOPEFISHHUNGRYRSPR,          // 406
	DOPEFISHBURP1SPR,            // 407
	DOPEFISHBURP2SPR,            // 408
	BIGBUBBLE1SPR,               // 409
	BIGBUBBLE2SPR,               // 410
	BIGBUBBLE3SPR,               // 411
	BIGBUBBLE4SPR,               // 412
	SMALLBUBBLE1SPR,             // 413
	SMALLBUBBLE2SPR,             // 414
	SMALLBUBBLE3SPR,             // 415
	SMALLBUBBLE4SPR,             // 416
	MEDIUMBUBBLESPR,             // 417
	DOPEFISHSWIML1SPR,           // 418
	DOPEFISHSWIML2SPR,           // 419
	DOPEFISHHUNGRYLSPR,          // 420
	END_LUMP(DOPEFISH_LUMP_END, __DOPEFISHEND)

	START_LUMP(SCHOOLFISH_LUMP_START, __SCHOOLFISHSTART)
	SCHOOLFISHL1SPR,             // 421
	SCHOOLFISHL2SPR,             // 422
	SCHOOLFISHR1SPR,             // 423
	SCHOOLFISHR2SPR,             // 424
	END_LUMP(SCHOOLFISH_LUMP_END, __SCHOOLFISHEND)

	START_LUMP(ARACHNUT_LUMP_START, __ARACHNUTSTART)
	ARACHNUTWALK1SPR,            // 425
	ARACHNUTWALK2SPR,            // 426
	ARACHNUTWALK3SPR,            // 427
	ARACHNUTWALK4SPR,            // 428
	ARACHNUTSTUNSPR,             // 429
	END_LUMP(ARACHNUT_LUMP_END, __ARACHNUTEND)

	SCUBASPR,                    // 430

	START_LUMP(SPRITE_LUMP_START, __SPRITESTART)
	SPRITEFLOATSPR,              // 431
	SPRITEAIMLSPR,               // 432
	SPRITESHOOTLSPR,             // 433
	SPRITEAIMRSPR,               // 434
	SPRITESHOOTRSPR,             // 435
	SPRITESHOT1SPR,              // 436
	SPRITESHOT2SPR,              // 437
	SPRITESHOT3SPR,              // 438
	SPRITESHOT4SPR,              // 439
	END_LUMP(SPRITE_LUMP_END, __SPRITEEND)

	START_LUMP(MINE_LUMP_START, __MINESTART)
	MINESPR,                     // 440
	MINEEXPLODE1SPR,             // 441
	MINEEXPLODE2SPR,             // 442
	END_LUMP(MINE_LUMP_END, __MINEEND)

	START_LUMP(SKYPEST_LUMP_START, __SKYPESTSTART)
	SKYPESTFLYL1SPR,             // 443
	SKYPESTFLYL2SPR,             // 444
	SKYPESTFLYR1SPR,             // 445
	SKYPESTFLYR2SPR,             // 446
	SKYPESTSIT1SPR,              // 447
	SKYPESTSIT2SPR,              // 448
	SKYPESTSIT3SPR,              // 449
	SKYPESTSIT4SPR,              // 450
	SKYPESTSIT5SPR,              // 451
	SKYPESTSIT6SPR,              // 452
	SKYPESTSIT7SPR,              // 453
	SKYPESTSIT8SPR,              // 454
	SKYPESTSIT9SPR,              // 455
	SKYPESTSQUASHEDSPR,          // 456
	END_LUMP(SKYPEST_LUMP_END, __SKYPESTEND)

	START_LUMP(WORMOUTH_LUMP_START, __WORMOUTHSTART)
	WORMOUTHSPR,                 // 457
	WORMOUTHPEEKR1SPR,           // 458
	WORMOUTHPEEKR2SPR,           // 459
	WORMOUTHPEEKL1SPR,           // 460
	WORMOUTHPEEKL2SPR,           // 461
	WORMOUTHBITER1SPR,           // 462
	WORMOUTHBITER2SPR,           // 463
	WORMOUTHBITER3SPR,           // 464
	WORMOUTHBITEL1SPR,           // 465
	WORMOUTHBITEL2SPR,           // 466
	WORMOUTHBITEL3SPR,           // 467
	WORMOUTHSTUNSPR,             // 468
	END_LUMP(WORMOUTH_LUMP_END, __WORMOUTHEND)

	START_LUMP(LICK_LUMP_START, __LICKSTART)
	LICKMOVER1SPR,               // 469
	LICKMOVER2SPR,               // 470
	LICKMOVER3SPR,               // 471
	LICKMOVER4SPR,               // 472
	LICKMOVEL1SPR,               // 473
	LICKMOVEL2SPR,               // 474
	LICKMOVEL3SPR,               // 475
	LICKMOVEL4SPR,               // 476
	LICKATTACKR1SPR,             // 477
	LICKATTACKR2SPR,             // 478
	LICKATTACKR3SPR,             // 479
	LICKATTACKL1SPR,             // 480
	LICKATTACKL2SPR,             // 481
	LICKATTACKL3SPR,             // 482
	LICKSTUNSPR,                 // 483
	END_LUMP(LICK_LUMP_END, __LICKEND)

	START_LUMP(PLATFORM_LUMP_START, __PLATFORMSTART)
	PLATFORMSPR,                 // 484
	PLATSIDETHRUST1SPR,          // 485
	PLATSIDETHRUST2SPR,          // 486
	PLATRTHRUST1SPR,             // 487
	PLATRTHRUST2SPR,             // 488
	PLATLTHRUST1SPR,             // 489
	PLATLTHRUST2SPR,             // 490
	END_LUMP(PLATFORM_LUMP_END, __PLATFORMEND)

	START_LUMP(BOUNDER_LUMP_START, __BOUNDERSTART)
	BOUNDERL1SPR,                // 491
	BOUNDERL2SPR,                // 492
	BOUNDERR1SPR,                // 493
	BOUNDERR2SPR,                // 494
	BOUNDERC1SPR,                // 495
	BOUNDERC2SPR,                // 496
	BOUNDERSTUNSPR,           // 497
	END_LUMP(BOUNDER_LUMP_END, __BOUNDEREND)

	START_LUMP(THUNDERCLOUD_LUMP_START, __THUNDERCLOUDSTART)
	CLOUDSPR,                    // 498
	CLOUDACTIVESPR,              // 499
	CLOUDCHARGESPR,              // 500
	BOLT1SPR,               // 501
	BOLT2SPR,               // 502
	END_LUMP(THUNDERCLOUD_LUMP_END, __THUNDERCLOUDEND)

	START_LUMP(BERKELOID_LUMP_START, __BERKELOIDSTART)
	BERKEWALKL1SPR,              // 503
	BERKEWALKL2SPR,              // 504
	BERKEWALKL3SPR,              // 505
	BERKEWALKL4SPR,              // 506
	BERKEWALKR1SPR,              // 507
	BERKEWALKR2SPR,              // 508
	BERKEWALKR3SPR,              // 509
	BERKEWALKR4SPR,              // 510
	BERKETHROWL1SPR,             // 511
	BERKETHROWL2SPR,             // 512
	BERKETHROWR1SPR,             // 513
	BERKETHROWR2SPR,             // 514
	FIREBALL1SPR,                // 515
	FIREBALL2SPR,                // 516
	FIREBALL3SPR,                // 517
	FIREBALL4SPR,                // 518
	END_LUMP(BERKELOID_LUMP_END, __BERKELOIDEND)

	START_LUMP(MOON_LUMP_START, __MOONSTART)
	KEENMOON1SPR,                // 519
	KEENMOON2SPR,                // 520
	END_LUMP(MOON_LUMP_END, __MOONEND)

	//
	// TILES (these don't need names)
	//

	LASTTILE=STARTEXTERNS-1,

	//
	// EXTERNS
	//

	ORDERSCREEN,                 // 4735
	BIGCOMMANDER,                // 4736
	BIGKEEN,                     // 4737
	OUTOFMEM,                    // 4738

	//texts
	T_HELPART,                   // 4739
	T_STORYART,                  // 4740
	T_CONTRART,                  // 4741
	T_IDART,                     // 4742
	T_ENDART,                    // 4743
	T_DEMOART,                   // 4744
	T_ORDERART,                  // 4745

	//demos
	DEMO0,                       // 4746
	DEMO1,                       // 4747
	DEMO2,                       // 4748
	DEMO3,                       // 4749
	DEMO4,                       // 4750

	NUMGRCHUNKS
} graphicnums;

#undef START_LUMP
#undef END_LUMP

#endif //__GFX_H__