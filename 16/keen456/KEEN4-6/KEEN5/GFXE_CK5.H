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
// Graphics .H file for .CK5
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
#define NUMPICS      93
#define NUMSPRITES   346
#define NUMTILE16    1512
#define NUMTILE16M   2952
#define NUMEXTERNS   17

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
	// Lump Start

	LASTFONT=STARTPICS-1,

	START_LUMP(HELP_LUMP_START, __HELPSTART)
	H_HELPPIC,                   // 6
	H_LARROWPIC,                 // 7
	H_RARROWPIC,                 // 8
	H_ESCPIC,                    // 9
	H_ENTERPIC,                  // 10
	H_BOTTOMINSTRPIC,            // 11
	H_GUMPIC,                    // 12
	H_MARSHMALLOWPIC,            // 13
	H_CHOCMILKPIC,               // 14
	H_TARTSTIXPIC,               // 15
	H_STOOPIESPIC,               // 16
	H_SUGARPIC,                  // 17
	H_VITALINPIC,                // 18
	H_STUNNERPIC,                // 19
	H_GEMPIC,                    // 20
	H_KEGPIC,                    // 21
	H_ENDOFTEXTPIC,              // 22
	H_HELPMENUPIC,               // 23
	H_HANDPIC,                   // 24
	H_ARROWSENTERESCPIC,         // 25
	H_FLASHARROW1PIC,            // 26
	H_FLASHARROW2PIC,            // 27
	H_TOPWINDOWPIC,              // 28
	H_LEFTWINDOWPIC,             // 29
	H_RIGHTWINDOWPIC,            // 30
	H_BOTTOMINFOPIC,             // 31
	H_BOTTOMWINDOWPIC,           // 32
	H_BARPIC,                    // 33
	H_SPARKYPIC,                 // 34
	H_AMPTONPIC,                 // 35
	H_SLICESTARPIC,              // 36
	H_VOLTEFACEPIC,              // 37
	H_ROBOREDPIC,                // 38
	H_SHELLEYPIC,                // 39
	H_SPIROGRIPPIC,              // 40
	H_MINEPIC,                   // 41
	H_SPINDREDPIC,               // 42
	H_SHIKADIPIC,                // 43
	H_SPHEREFULPIC,              // 44
	H_PETPIC,                    // 45
	H_MASTERPIC,                 // 46
	H_IDLOGOPIC,                 // 47
	H_STORY1PIC,                 // 48
	H_STORY2PIC,                 // 49
	H_STORY3PIC,                 // 50
	H_STORY4PIC,                 // 51
	H_VISAPIC,                   // 52
	H_MCPIC,                     // 53
	H_KEENTHUMBSUPPIC,           // 54
	H_END1PIC,                   // 55
	H_END2PIC,                   // 56
	H_END3PIC,                   // 57
	H_END4PIC,                   // 58
	H_END5PIC,                   // 59
	H_END6PIC,                   // 60
	H_END7PIC,                   // 61
	H_END8PIC,                   // 62
	H_CONGRATSPIC,               // 63
	H_KEENFEEDSPIC,              // 64
	H_DOORCARDPIC,               // 65
	H_KEEN6PIC,                  // 66
	END_LUMP(HELP_LUMP_END, __HELPEND)

	START_LUMP(CONTROLS_LUMP_START, __CONTROLSSTART)
	CP_MAINMENUPIC,              // 67
	CP_NEWGAMEMENUPIC,           // 68
	CP_LOADMENUPIC,              // 69
	CP_SAVEMENUPIC,              // 70
	CP_CONFIGMENUPIC,            // 71
	CP_SOUNDMENUPIC,             // 72
	CP_MUSICMENUPIC,             // 73
	CP_KEYBOARDMENUPIC,          // 74
	CP_KEYMOVEMENTPIC,           // 75
	CP_KEYBUTTONPIC,             // 76
	CP_JOYSTICKMENUPIC,          // 77
	CP_OPTIONSMENUPIC,           // 78
	CP_PADDLEWARPIC,             // 79
	CP_QUITPIC,                  // 80
	CP_JOYSTICKPIC,              // 81
	CP_MENUSCREENPIC,            // 82
	END_LUMP(CONTROLS_LUMP_END, __COLTROLSEND)

	START_LUMP(_LUMP_START, __START)
	IDSOFTPIC,                   // 83
	PROGTEAMPIC,                 // 84
	ARTISTPIC,                   // 85
	DIRECTORPIC,                 // 86
	SW_BACKGROUNDPIC,            // 87
	TITLEPICPIC,                 // 88
	MILKYWAYPIC,                 // 89
	END_LUMP(_LUMP_END, __END)

	START_LUMP(KEENTALK_LUMP_START, __KEENTALKSTART)
	KEENTALK1PIC,                // 90
	KEENTALK2PIC,                // 91
	END_LUMP(KEENTALK_LUMP_END, __KEENTALKEND)

	START_LUMP(LOADING_LUMP_START, __LOADINGSTART)
	KEENCOUNT1PIC,               // 92
	KEENCOUNT2PIC,               // 93
	KEENCOUNT3PIC,               // 94
	KEENCOUNT4PIC,               // 95
	KEENCOUNT5PIC,               // 96
	KEENCOUNT6PIC,               // 97
	END_LUMP(LOADING_LUMP_END, __LOADINGEND)

	GAMEOVERPIC,                 // 98

	CP_MENUMASKPICM,             // 99
	CORDPICM,                    // 100
	METALPOLEPICM,               // 101

	//
	// SPRITES
	//

	START_LUMP(PADDLE_LUMP_START, __PADDLESTART)
	PADDLESPR,                   // 102
	BALLSPR,                     // 103
	BALL1PIXELTOTHERIGHTSPR,     // 104
	BALL2PIXELSTOTHERIGHTSPR,    // 105
	BALL3PIXELSTOTHERIGHTSPR,    // 106
	END_LUMP(PADDLE_LUMP_END, __PADDLEEND)

	DEMOPLAQUESPR,               // 107

	START_LUMP(KEEN_LUMP_START, __KEENSTART)
	KEENSTANDRSPR,               // 108
	KEENRUNR1SPR,                // 109
	KEENRUNR2SPR,                // 110
	KEENRUNR3SPR,                // 111
	KEENRUNR4SPR,                // 112
	KEENJUMPR1SPR,               // 113
	KEENJUMPR2SPR,               // 114
	KEENJUMPR3SPR,               // 115
	KEENSTANDLSPR,               // 116
	KEENRUNL1SPR,                // 117
	KEENRUNL2SPR,                // 118
	KEENRUNL3SPR,                // 119
	KEENRUNL4SPR,                // 120
	KEENJUMPL1SPR,               // 121
	KEENJUMPL2SPR,               // 122
	KEENJUMPL3SPR,               // 123
	KEENLOOKUSPR,                // 124
	KEENWAITR1SPR,               // 125
	KEENWAITR2SPR,               // 126
	KEENWAITR3SPR,               // 127
	KEENSITREAD1SPR,             // 128
	KEENSITREAD2SPR,             // 129
	KEENSITREAD3SPR,             // 130
	KEENSITREAD4SPR,             // 131
	KEENREAD1SPR,                // 132
	KEENREAD2SPR,                // 133
	KEENREAD3SPR,                // 134
	KEENSTOPREAD1SPR,            // 135
	KEENSTOPREAD2SPR,            // 136
	KEENLOOKD1SPR,               // 137
	KEENLOOKD2SPR,               // 138
	KEENONPLATSPR,               // 139
	KEENDIE1SPR,                 // 140
	KEENDIE2SPR,                 // 141
	KEENSTUNSPR,                 // 142
	STUNSTARS1SPR,               // 143
	STUNSTARS2SPR,               // 144
	STUNSTARS3SPR,               // 145
	KEENSHOOTLSPR,               // 146
	KEENJLSHOOTLSPR,             // 147
	KEENJSHOOTDSPR,              // 148
	KEENJSHOOTUSPR,              // 149
	KEENSHOOTUSPR,               // 150
	KEENSHOOTRSPR,               // 151
	KEENJRSHOOTRSPR,             // 152
	STUN1SPR,                    // 153
	STUN2SPR,                    // 154
	STUN3SPR,                    // 155
	STUN4SPR,                    // 156
	STUNHIT1SPR,                 // 157
	STUNHIT2SPR,                 // 158
	KEENSHINNYR1SPR,             // 159
	KEENSHINNYR2SPR,             // 160
	KEENSHINNYR3SPR,             // 161
	KEENSLIDED1SPR,              // 162
	KEENSLIDED2SPR,              // 163
	KEENSLIDED3SPR,              // 164
	KEENSLIDED4SPR,              // 165
	KEENSHINNYL1SPR,             // 166
	KEENSHINNYL2SPR,             // 167
	KEENSHINNYL3SPR,             // 168
	KEENPLSHOOTUSPR,             // 169
	KEENPRSHOOTUSPR,             // 170
	KEENPRSHOOTDSPR,             // 171
	KEENPLSHOOTDSPR,             // 172
	KEENPSHOOTLSPR,              // 173
	KEENPSHOOTRSPR,              // 174
	KEENENTER1SPR,               // 175
	KEENENTER2SPR,               // 176
	KEENENTER3SPR,               // 177
	KEENENTER4SPR,               // 178
	KEENENTER5SPR,               // 179
	KEENHANGLSPR,                // 180
	KEENHANGRSPR,                // 181
	KEENCLIMBEDGEL1SPR,          // 182
	KEENCLIMBEDGEL2SPR,          // 183
	KEENCLIMBEDGEL3SPR,          // 184
	KEENCLIMBEDGEL4SPR,          // 185
	KEENCLIMBEDGER1SPR,          // 186
	KEENCLIMBEDGER2SPR,          // 187
	KEENCLIMBEDGER3SPR,          // 188
	KEENCLIMBEDGER4SPR,          // 189
	KEENPOGOR1SPR,               // 190
	KEENPOGOR2SPR,               // 191
	KEENPOGOL1SPR,               // 192
	KEENPOGOL2SPR,               // 193
	BONUS100UPSPR,               // 194
	BONUS100SPR,                 // 195
	BONUS200SPR,                 // 196
	BONUS500SPR,                 // 197
	BONUS1000SPR,                // 198
	BONUS2000SPR,                // 199
	BONUS5000SPR,                // 200
	BONUS1UPSPR,                 // 201
	BONUSCLIPSPR,                // 202
	VIVAPOOF1SPR,                // 203
	VIVAPOOF2SPR,                // 204
	VIVAPOOF3SPR,                // 205
	VIVAPOOF4SPR,                // 206
	END_LUMP(KEEN_LUMP_END, __KEENEND)

	START_LUMP(KEYCARD_LUMP_START, __KEYCARDSTART)
	DOORCARD1SPR,                // 207
	DOORCARD2SPR,                // 208
	BONUSCARDSPR,                // 209
	END_LUMP(KEYCARD_LUMP_END, __KEYCARDEND)

	START_LUMP(SUGAR1_LUMP_START, __SUGAR1START)
	SUGAR1ASPR,                  // 210
	SUGAR1BSPR,                  // 211
	END_LUMP(SUGAR1_LUMP_END, __SUGAR1END)

	START_LUMP(SUGAR2_LUMP_START, __SUGAR2START)
	SUGAR2ASPR,                  // 212
	SUGAR2BSPR,                  // 213
	END_LUMP(SUGAR2_LUMP_END, __SUGAR2END)

	START_LUMP(SUGAR3_LUMP_START, __SUGAR3START)
	SUGAR3ASPR,                  // 214
	SUGAR3BSPR,                  // 215
	END_LUMP(SUGAR3_LUMP_END, __SUGAR3END)

	START_LUMP(SUGAR4_LUMP_START, __SUGAR4START)
	SUGAR4ASPR,                  // 216
	SUGAR4BSPR,                  // 217
	END_LUMP(SUGAR4_LUMP_END, __SUGAR4END)

	START_LUMP(SUGAR5_LUMP_START, __SUGAR5START)
	SUGAR5ASPR,                  // 218
	SUGAR5BSPR,                  // 219
	END_LUMP(SUGAR5_LUMP_END, __SUGAR5END)

	START_LUMP(SUGAR6_LUMP_START, __SUGAR6START)
	SUGAR6ASPR,                  // 220
	SUGAR6BSPR,                  // 221
	END_LUMP(SUGAR6_LUMP_END, __SUGAR6END)

	START_LUMP(ONEUP_LUMP_START, __ONEUPSTART)
	ONEUPASPR,                   // 222
	ONEUPBSPR,                   // 223
	END_LUMP(ONEUP_LUMP_END, __ONEUPEND)

	START_LUMP(KEYGEM_LUMP_START, __KEYGEMSTART)
	REDGEM1SPR,                  // 224
	REDGEM2SPR,                  // 225
	YELLOWGEM1SPR,               // 226
	YELLOWGEM2SPR,               // 227
	BLUEGEM1SPR,                 // 228
	BLUEGEM2SPR,                 // 229
	GREENGEM1SPR,                // 230
	GREENGEM2SPR,                // 231
	BONUSGEMSPR,                 // 232
	END_LUMP(KEYGEM_LUMP_END, __KEYGEMEND)

	START_LUMP(AMMO_LUMP_START, __AMMOSTART)
	STUNCLIP1SPR,                // 233
	STUNCLIP2SPR,                // 234
	END_LUMP(AMMO_LUMP_END, __AMMOEND)

	SCOREBOXSPR,                 // 235

	START_LUMP(LASER_LUMP_START, __LASERSTART)
	LASER1SPR,                   // 236
	LASER2SPR,                   // 237
	LASER3SPR,                   // 238
	LASER4SPR,                   // 239
	LASERHIT1SPR,                // 240
	LASERHIT2SPR,                // 241
	END_LUMP(LASER_LUMP_END, __LASEREND)

	START_LUMP(WORLDKEEN_LUMP_START, __WORLDKEENSTART)
	WORLDKEENL1SPR,              // 242
	WORLDKEENL2SPR,              // 243
	WORLDKEENL3SPR,              // 244
	WORLDKEENR1SPR,              // 245
	WORLDKEENR2SPR,              // 246
	WORLDKEENR3SPR,              // 247
	WORLDKEENU1SPR,              // 248
	WORLDKEENU2SPR,              // 249
	WORLDKEENU3SPR,              // 250
	WORLDKEEND1SPR,              // 251
	WORLDKEEND2SPR,              // 252
	WORLDKEEND3SPR,              // 253
	WORLDKEENDR1SPR,             // 254
	WORLDKEENDR2SPR,             // 255
	WORLDKEENDR3SPR,             // 256
	WORLDKEENDL1SPR,             // 257
	WORLDKEENDL2SPR,             // 258
	WORLDKEENDL3SPR,             // 259
	WORLDKEENUL1SPR,             // 260
	WORLDKEENUL2SPR,             // 261
	WORLDKEENUL3SPR,             // 262
	WORLDKEENUR1SPR,             // 263
	WORLDKEENUR2SPR,             // 264
	WORLDKEENUR3SPR,             // 265
	WORLDKEENWAVE1SPR,           // 266
	WORLDKEENWAVE2SPR,           // 267
	FLAGFLIP1SPR,                // 268
	FLAGFLIP2SPR,                // 269
	FLAGFLIP3SPR,                // 270
	FLAGFLIP4SPR,                // 271
	FLAGFLIP5SPR,                // 272
	FLAGFALL1SPR,                // 273
	FLAGFALL2SPR,                // 274
	FLAGFLAP1SPR,                // 275
	FLAGFLAP2SPR,                // 276
	FLAGFLAP3SPR,                // 277
	FLAGFLAP4SPR,                // 278
	SHOOTINGSTAR1SPR,            // 279
	SHOOTINGSTAR2SPR,            // 280
	WORLDTELSPARK1SPR,           // 281
	WORLDTELSPARK2SPR,           // 282
	END_LUMP(WORLDKEEN_LUMP_END, __WORLDKEENEND)

	START_LUMP(FUSE_LUMP_START, __FUSESTART)
	FUSEFLASH1SPR,               // 283
	FUSEFLASH2SPR,               // 284
	FUSEFLASH3SPR,               // 285
	END_LUMP(FUSE_LUMP_END, __FUSEEND)

	START_LUMP(STAREXPLODE_LUMP_START, __SMALLSPARKSTART)
	STAREXPLODE1SPR,             // 286
	STAREXPLODE2SPR,             // 287
	STAREXPLODE3SPR,             // 288
	STAREXPLODE4SPR,             // 289
	END_LUMP(STAREXPLODE_LUMP_END, __SMALLSPARKEND)

	START_LUMP(TELEPORT_LUMP_START, __TELEPORTSTART)
	TELEPORTSPARK1SPR,           // 290
	TELEPORTSPARK2SPR,           // 291
	TELEPORTZAP1SPR,             // 292
	TELEPORTZAP2SPR,             // 293
	END_LUMP(TELEPORT_LUMP_END, __TELEPORTEND)

	START_LUMP(SCOTTIE_LUMP_START, __KORATHSTART)
	SCOTTIEWALKL1SPR,            // 294
	SCOTTIEWALKL2SPR,            // 295
	SCOTTIEWALKL3SPR,            // 296
	SCOTTIEWALKL4SPR,            // 297
	SCOTTIEWALKR1SPR,            // 298
	SCOTTIEWALKR2SPR,            // 299
	SCOTTIEWALKR3SPR,            // 300
	SCOTTIEWALKR4SPR,            // 301
	SCOTTIEFACESPR,              // 302
	SCOTTIESTUNSPR,              // 303
	END_LUMP(SCOTTIE_LUMP_END, __KORATHEND)

	START_LUMP(MASTER_LUMP_START, __MASTERSTART)
	MASTER1SPR,                  // 304
	MASTER2SPR,                  // 305
	MASTER3SPR,                  // 306
	MASTER4SPR,                  // 307
	MASTERTELEPORT1SPR,          // 308
	MASTERTELEPORT2SPR,          // 309
	SHIKMASTERCASTRSPR,          // 310
	SHIKMASTERCASTLSPR,          // 311
	MASTERFLOORSPARK1SPR,        // 312
	MASTERFLOORSPARK2SPR,        // 313
	MASTERFLOORSPARK3SPR,        // 314
	MASTERFLOORSPARK4SPR,        // 315
	MASTERSHOT1SPR,              // 316
	MASTERSHOT2SPR,              // 317
	MASTERSHOT3SPR,              // 318
	MASTERSHOT4SPR,              // 319
	END_LUMP(MASTER_LUMP_END, __MASTEREND)

	START_LUMP(SHIKADI_LUMP_START, __SHIKADISTART)
	SHIKADI1SPR,                 // 320
	SHIKADI2SPR,                 // 321
	SHIKADI3SPR,                 // 322
	SHIKADI4SPR,                 // 323
	SHIKADIGRABRSPR,             // 324
	SHIKADIGRABLSPR,             // 325
	SHIKADIPOLESPARK1SPR,        // 326
	SHIKADIPOLESPARK2SPR,        // 327
	SHIKADIWALKR1SPR,            // 328
	SHIKADIWALKR2SPR,            // 329
	SHIKADIWALKR3SPR,            // 330
	SHIKADIWALKR4SPR,            // 331
	SHIKADIWALKL1SPR,            // 332
	SHIKADIWALKL2SPR,            // 333
	SHIKADIWALKL3SPR,            // 334
	SHIKADIWALKL4SPR,            // 335
	SHIKADISTUNSPR,              // 336
	END_LUMP(SHIKADI_LUMP_END, __SHIKADIEND)

	START_LUMP(SHOCKSHUND_LUMP_START, __SHOCKSHUNDSTART)
	PETSIT1SPR,                  // 337
	PETSIT2SPR,                  // 338
	PETRUNR1SPR,                 // 339
	PETRUNR2SPR,                 // 340
	PETRUNR3SPR,                 // 341
	PETRUNR4SPR,                 // 342
	PETRUNL1SPR,                 // 343
	PETRUNL2SPR,                 // 344
	PETRUNL3SPR,                 // 345
	PETRUNL4SPR,                 // 346
	PETJUMPLSPR,                 // 347
	PETJUMPRSPR,                 // 348
	PETBARKR1SPR,                // 349
	PETBARKR2SPR,                // 350
	PETBARKL1SPR,                // 351
	PETBARKL2SPR,                // 352
	PETSTUNSPR,                  // 353
	PETSPARK1SPR,                // 354
	PETSPARK2SPR,                // 355
	PETSPARKHIT1SPR,             // 356
	PETSPARKHIT2SPR,             // 357
	END_LUMP(SHOCKSHUND_LUMP_END, __SHOCKSHUNDEND)

	START_LUMP(SPHEREFUL_LUMP_START, __SPHEREFULSTART)
	SPHEREFUL1SPR,               // 358
	SPHEREFUL2SPR,               // 359
	SPHEREFUL3SPR,               // 360
	SPHEREFUL4SPR,               // 361
	SPHEREGUARD1SPR,             // 362
	SPHEREGUARD2SPR,             // 363
	SPHEREGUARD3SPR,             // 364
	SPHEREGUARD4SPR,             // 365
	END_LUMP(SPHEREFUL_LUMP_END, __SPHEREFULEND)

	START_LUMP(SPARKY_LUMP_START, __SPARKYSTART)
	SPARKYWALKL1SPR,             // 366
	SPARKYWALKL2SPR,             // 367
	SPARKYWALKL3SPR,             // 368
	SPARKYWALKL4SPR,             // 369
	SPARKYTURN1SPR,              // 370
	SPARKYTURN2SPR,              // 371
	SPARKYTURN3SPR,              // 372
	SPARKYWALKR1SPR,             // 373
	SPARKYWALKR2SPR,             // 374
	SPARKYWALKR3SPR,             // 375
	SPARKYWALKR4SPR,             // 376
	SPARKYSTUNSPR,               // 377
	END_LUMP(SPARKY_LUMP_END, __SPARKYEND)

	START_LUMP(MINE_LUMP_START, __MINESTART)
	SHIKADIMINESPR,              // 378
	SHIKADIMINEEYESPR,           // 379
	SHIKADIMINEPULSE1SPR,        // 380
	SHIKADIMINEPULSE2SPR,        // 381
	SHIKADIMINEBOOM1SPR,         // 382
	SHIKADIMINEBOOM2SPR,         // 383
	SHIKADIMINEPIECESPR,         // 384
	END_LUMP(MINE_LUMP_END, __MINEEND)

	START_LUMP(SLICESTAR_LUMP_START, __SLICESTARSTART)
	SLICESTARSPR,                // 385
	SLICESTARBOOMSPR,            // 386
	END_LUMP(SLICESTAR_LUMP_END, __SLICASTAREND)

	START_LUMP(ROBORED_LUMP_START, __ROBOREDSTART)
	ROBOREDRSPR,                 // 387
	ROBOREDLSPR,                 // 388
	ROBOSHOT1SPR,                // 389
	ROBOSHOT2SPR,                // 390
	ROBOSHOTHIT1SPR,             // 391
	ROBOSHOTHIT2SPR,             // 392
	END_LUMP(ROBORED_LUMP_END, __ROBOREDEND)

	START_LUMP(SPIRO_LUMP_START, __SPIROSTART)
	SPIROSITDSPR,                // 393
	SPIROSITLSPR,                // 394
	SPIROSITUSPR,                // 395
	SPIROSITRSPR,                // 396
	SPIROSPINULSPR,              // 397
	SPIROSPINURSPR,              // 398
	SPIROSPINDRSPR,              // 399
	SPIROSPINDLSPR,              // 400
	SPIROSPINDSPR,               // 401
	SPIROSPINLSPR,               // 402
	SPIROSPINUSPR,               // 403
	SPIROSPINRSPR,               // 404
	END_LUMP(SPIRO_LUMP_END, __SPIROEND)

	START_LUMP(AMPTON_LUMP_START, __AMPTONSTART)
	AMPTONWALKR1SPR,             // 405
	AMPTONWALKR2SPR,             // 406
	AMPTONWALKR3SPR,             // 407
	AMPTONWALKR4SPR,             // 408
	AMPTONFACESPR,               // 409
	AMPTONGRAB1SPR,              // 410
	AMPTONGRAB2SPR,              // 411
	AMTONWALKL1SPR,              // 412
	AMTONWALKL2SPR,              // 413
	AMTONWALKL3SPR,              // 414
	AMTONWALKL4SPR,              // 415
	AMPTONSTUNSPR,               // 416
	END_LUMP(AMPTON_LUMP_END, __AMPTONEND)

	START_LUMP(VOLTE_LUMP_START, __VOLTESTART)
	VOLTEFACE1SPR,               // 417
	VOLTEFACE2SPR,               // 418
	VOLTEFACE3SPR,               // 419
	VOLTEFACE4SPR,               // 420
	VOLTEFACESTUNSPR,            // 421
	END_LUMP(VOLTE_LUMP_END, __VOLTEEND)

	START_LUMP(SLOTPLAT_LUMP_START, __PINKPLATSTART)
	SLOTPLAT1SPR,                // 422
	SLOTPLAT2SPR,                // 423
	END_LUMP(SLOTPLAT_LUMP_END, __PINKPLATEND)

	START_LUMP(SPINDRED_LUMP_START, __SPINDREDSTART)
	SPINDRED1SPR,                // 424
	SPINDRED2SPR,                // 425
	SPINDRED3SPR,                // 426
	SPINDRED4SPR,                // 427
	END_LUMP(SPINDRED_LUMP_END, __SPINDREDEND)

	START_LUMP(SHELLEY_LUMP_START, __SHELLEYSTART)
	SHELLEYR1SPR,                // 428
	SHELLEYR2SPR,                // 429
	SHELLEYR3SPR,                // 430
	SHELLEYR4SPR,                // 431
	SHELLEYL1SPR,                // 432
	SHELLEYL2SPR,                // 433
	SHELLEYL3SPR,                // 434
	SHELLEYL4SPR,                // 435
	SHELLEYJUMPRSPR,             // 436
	SHELLEYFALLRSPR,             // 437
	SHELLEYJUMPLSPR,             // 438
	SHELLEYFALLLSPR,             // 439
	SHELLEYBOOM1SPR,             // 440
	SHELLEYBOOM2SPR,             // 441
	SHELLEYBOOM3SPR,             // 442
	SHELLEYBOOM4SPR,             // 443
	SHELLEYPIECE1SPR,            // 444
	SHELLEYPIECE2SPR,            // 445
	END_LUMP(SHELLEY_LUMP_END, __SHELLEYEND)

	START_LUMP(PLATFORM_LUMP_START, __PLATFORMSTART)
	PLATFORMSPR,                 // 446
	END_LUMP(PLATFORM_LUMP_END, __PLATFORMEND)

	START_LUMP(MINIPLAT_LUMP_START, __MINIPLATSTART)
	MINIPLATSPR,                // 447
	END_LUMP(MINIPLAT_LUMP_END, __MINIPLATEND)


	//
	// TILES (these don't need names)
	//

	LASTTILE=STARTEXTERNS-1,

	//
	// EXTERNS
	//

	//texts
	T_HELPART,                   // 4914
	T_CONTRART,                  // 4915
	T_STORYART,                  // 4916
	T_IDART,                     // 4917
	T_ENDART,                    // 4918
	T_ENDART2,                   // 4919
	T_ORDERART,                  // 4920

	ORDERSCREEN,                 // 4921
	BIGCOMMANDER,                // 4922
	BIGKEEN,                     // 4923
	OUTOFMEM,                    // 4924
	GALAXY,                      // 4925

	//demos
	DEMO0,                       // 4926
	DEMO1,                       // 4927
	DEMO2,                       // 4928
	DEMO3,                       // 4929
	DEMO4,                       // 4930

	NUMGRCHUNKS
} graphicnums;

#undef START_LUMP
#undef END_LUMP

#endif //__GFX_H__