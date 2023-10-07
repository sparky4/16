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
// Graphics .H file for .CK6
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
#define NUMTILE8M    36		// BUG: only 12 tiles exist in EGAGRAPH!
#define NUMTILE32    0
#define NUMTILE32M   0

//episode-specific numbers:
#define NUMPICS      37
#define NUMSPRITES   390
#define NUMTILE16    2376
#define NUMTILE16M   2736
#define NUMEXTERNS   10

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

	H_END1PIC,                   // 6
	H_END2PIC,                   // 7
	H_END3PIC,                   // 8
	H_END4PIC,                   // 9
	H_END5PIC,                   // 10

	START_LUMP(CONTROLS_LUMP_START, __CONTROLSSTART)
	CP_MAINMENUPIC,              // 11
	CP_NEWGAMEMENUPIC,           // 12
	CP_LOADMENUPIC,              // 13
	CP_SAVEMENUPIC,              // 14
	CP_CONFIGMENUPIC,            // 15
	CP_SOUNDMENUPIC,             // 16
	CP_MUSICMENUPIC,             // 17
	CP_KEYBOARDMENUPIC,          // 18
	CP_KEYMOVEMENTPIC,           // 19
	CP_KEYBUTTONPIC,             // 20
	CP_JOYSTICKMENUPIC,          // 21
	CP_OPTIONSMENUPIC,           // 22
	CP_PADDLEWARPIC,             // 23
	CP_QUITPIC,                  // 24
	CP_JOYSTICKPIC,              // 25
	CP_MENUSCREENPIC,            // 26
	END_LUMP(CONTROLS_LUMP_END, __CONTROLSEND)

	H_FLASHARROW1PIC,            // 27
	H_FLASHARROW2PIC,            // 28
	IDSOFTPIC,                   // 29
	PROGTEAMPIC,                 // 30
	ARTISTPIC,                   // 31
	DIRECTORPIC,                 // 32
	SW_BACKGROUNDPIC,            // 33
	TITLEPICPIC,                 // 34
	KEENTALK1PIC,                // 35
	KEENTALK2PIC,                // 36
	KEENCOUNT1PIC,               // 37
	KEENCOUNT2PIC,               // 38
	KEENCOUNT3PIC,               // 39
	KEENCOUNT4PIC,               // 40
	KEENCOUNT5PIC,               // 41
	KEENCOUNT6PIC,               // 42

	//
	// MASKED PICS
	//

	CP_MENUMASKPICM,             // 43
	CORDPICM,                    // 44
	METALPOLEPICM,               // 45

	//
	// SPRITES
	//

	START_LUMP(PADDLE_LUMP_START, __PADDLESTART)
	PADDLESPR,                   // 46
	BALLSPR,                     // 47
	BALL1PIXELTOTHERIGHTSPR,     // 48
	BALL2PIXELSTOTHERIGHTSPR,    // 49
	BALL3PIXELSTOTHERIGHTSPR,    // 50
	END_LUMP(PADDLE_LUMP_END, __PADDLEEND)

	DEMOPLAQUESPR,               // 51

	START_LUMP(KEEN_LUMP_START, __KEENSTART)
	KEENSTANDRSPR,               // 52
	KEENRUNR1SPR,                // 53
	KEENRUNR2SPR,                // 54
	KEENRUNR3SPR,                // 55
	KEENRUNR4SPR,                // 56
	KEENJUMPR1SPR,               // 57
	KEENJUMPR2SPR,               // 58
	KEENJUMPR3SPR,               // 59
	KEENSTANDLSPR,               // 60
	KEENRUNL1SPR,                // 61
	KEENRUNL2SPR,                // 62
	KEENRUNL3SPR,                // 63
	KEENRUNL4SPR,                // 64
	KEENJUMPL1SPR,               // 65
	KEENJUMPL2SPR,               // 66
	KEENJUMPL3SPR,               // 67
	KEENLOOKUSPR,                // 68
	KEENWAITR1SPR,               // 69
	KEENWAITR2SPR,               // 70
	KEENWAITR3SPR,               // 71
	KEENSITREAD1SPR,             // 72
	KEENSITREAD2SPR,             // 73
	KEENSITREAD3SPR,             // 74
	KEENSITREAD4SPR,             // 75
	KEENREAD1SPR,                // 76
	KEENREAD2SPR,                // 77
	KEENREAD3SPR,                // 78
	KEENSTOPREAD1SPR,            // 79
	KEENSTOPREAD2SPR,            // 80
	KEENLOOKD1SPR,               // 81
	KEENLOOKD2SPR,               // 82
	KEENDIE1SPR,                 // 83
	KEENDIE2SPR,                 // 84
	KEENSTUNSPR,                 // 85
	STUNSTARS1SPR,               // 86
	STUNSTARS2SPR,               // 87
	STUNSTARS3SPR,               // 88
	KEENSHOOTLSPR,               // 89
	KEENJLSHOOTLSPR,             // 90
	KEENJSHOOTDSPR,              // 91
	KEENJSHOOTUSPR,              // 92
	KEENSHOOTUSPR,               // 93
	KEENSHOOTRSPR,               // 94
	KEENJRSHOOTRSPR,             // 95
	STUN1SPR,                    // 96
	STUN2SPR,                    // 97
	STUN3SPR,                    // 98
	STUN4SPR,                    // 99
	STUNHIT1SPR,                 // 100
	STUNHIT2SPR,                 // 101
	KEENSHINNYR1SPR,             // 102
	KEENSHINNYR2SPR,             // 103
	KEENSHINNYR3SPR,             // 104
	KEENSLIDED1SPR,              // 105
	KEENSLIDED2SPR,              // 106
	KEENSLIDED3SPR,              // 107
	KEENSLIDED4SPR,              // 108
	KEENSHINNYL1SPR,             // 109
	KEENSHINNYL2SPR,             // 110
	KEENSHINNYL3SPR,             // 111
	KEENPLSHOOTUSPR,             // 112
	KEENPRSHOOTUSPR,             // 113
	KEENPRSHOOTDSPR,             // 114
	KEENPLSHOOTDSPR,             // 115
	KEENPSHOOTLSPR,              // 116
	KEENPSHOOTRSPR,              // 117
	KEENENTER1SPR,               // 118
	KEENENTER2SPR,               // 119
	KEENENTER3SPR,               // 120
	KEENENTER4SPR,               // 121
	KEENENTER5SPR,               // 122
	KEENHANGLSPR,                // 123
	KEENHANGRSPR,                // 124
	KEENCLIMBEDGEL1SPR,          // 125
	KEENCLIMBEDGEL2SPR,          // 126
	KEENCLIMBEDGEL3SPR,          // 127
	KEENCLIMBEDGEL4SPR,          // 128
	KEENCLIMBEDGER1SPR,          // 129
	KEENCLIMBEDGER2SPR,          // 130
	KEENCLIMBEDGER3SPR,          // 131
	KEENCLIMBEDGER4SPR,          // 132
	KEENPOGOR1SPR,               // 133
	KEENPOGOR2SPR,               // 134
	KEENPOGOL1SPR,               // 135
	KEENPOGOL2SPR,               // 136
	BONUS100UPSPR,               // 137
	BONUS100SPR,                 // 138
	BONUS200SPR,                 // 139
	BONUS500SPR,                 // 140
	BONUS1000SPR,                // 141
	BONUS2000SPR,                // 142
	BONUS5000SPR,                // 143
	BONUS1UPSPR,                 // 144
	BONUSCLIPSPR,                // 145
	VIVASPLASH1SPR,              // 146
	VIVASPLASH2SPR,              // 147
	VIVASPLASH3SPR,              // 148
	VIVASPLASH4SPR,              // 149
	END_LUMP(KEEN_LUMP_END, __KEENEND)

	START_LUMP(SUGAR1_LUMP_START, __SUGAR1START)
	SUGAR1ASPR,                  // 150
	SUGAR1BSPR,                  // 151
	END_LUMP(SUGAR1_LUMP_END, __SUGAR1END)

	START_LUMP(SUGAR2_LUMP_START, __SUGAR2START)
	SUGAR2ASPR,                  // 152
	SUGAR2BSPR,                  // 153
	END_LUMP(SUGAR2_LUMP_END, __SUGAR2END)

	START_LUMP(SUGAR3_LUMP_START, __SUGAR3START)
	SUGAR3ASPR,                  // 154
	SUGAR3BSPR,                  // 155
	END_LUMP(SUGAR3_LUMP_END, __SUGAR3END)

	START_LUMP(SUGAR4_LUMP_START, __SUGAR4START)
	SUGAR4ASPR,                  // 156
	SUGAR4BSPR,                  // 157
	END_LUMP(SUGAR4_LUMP_END, __SUGAR4END)

	START_LUMP(SUGAR5_LUMP_START, __SUGAR5START)
	SUGAR5ASPR,                  // 158
	SUGAR5BSPR,                  // 159
	END_LUMP(SUGAR5_LUMP_END, __SUGAR5END)

	START_LUMP(SUGAR6_LUMP_START, __SUGAR6START)
	SUGAR6ASPR,                  // 160
	SUGAR6BSPR,                  // 161
	END_LUMP(SUGAR6_LUMP_END, __SUGAR6END)

	START_LUMP(ONEUP_LUMP_START, __ONEUPSTART)
	ONEUPASPR,                   // 162
	ONEUPBSPR,                   // 163
	END_LUMP(ONEUP_LUMP_END, __ONEUPEND)

	START_LUMP(KEYGEM_LUMP_START, __KEYGEMSTART)
	REDGEM1SPR,                  // 164
	REDGEM2SPR,                  // 165
	YELLOWGEM1SPR,               // 166
	YELLOWGEM2SPR,               // 167
	BLUEGEM1SPR,                 // 168
	BLUEGEM2SPR,                 // 169
	GREENGEM1SPR,                // 170
	GREENGEM2SPR,                // 171
	BONUSGEMSPR,                 // 172
	END_LUMP(KEYGEM_LUMP_END, __KEYGEMEND)

	START_LUMP(AMMO_LUMP_START, __AMMOSTART)
	STUNCLIP1SPR,                // 173
	STUNCLIP2SPR,                // 174
	END_LUMP(AMMO_LUMP_END, __AMMOEND)

	SCOREBOXSPR,                 // 175

	START_LUMP(LASER_LUMP_START, __LASERSTART)
	LASER1SPR,                   // 176
	LASER2SPR,                   // 177
	LASER3SPR,                   // 178
	LASER4SPR,                   // 179
	LASERHIT1SPR,                // 180
	LASERHIT2SPR,                // 181
	END_LUMP(LASER_LUMP_END, __LASEREND)

	START_LUMP(SANDWICH_LUMP_START, __SANDWICHSTART)
	SANDWICHSPR,                 // 182
	END_LUMP(SANDWICH_LUMP_END, __SANDWICHEND)

	START_LUMP(HOOK_LUMP_START, __ROPESTART)
	HOOKSPR,                     // 183
	END_LUMP(HOOK_LUMP_END, __ROPEEND)

	START_LUMP(WORLDKEEN_LUMP_START, __WORLDKEENSTART)
	WORLDKEENL1SPR,              // 184
	WORLDKEENL2SPR,              // 185
	WORLDKEENL3SPR,              // 186
	WORLDKEENR1SPR,              // 187
	WORLDKEENR2SPR,              // 188
	WORLDKEENR3SPR,              // 189
	WORLDKEENU1SPR,              // 190
	WORLDKEENU2SPR,              // 191
	WORLDKEENU3SPR,              // 192
	WORLDKEEND1SPR,              // 193
	WORLDKEEND2SPR,              // 194
	WORLDKEEND3SPR,              // 195
	WORLDKEENDR1SPR,             // 196
	WORLDKEENDR2SPR,             // 197
	WORLDKEENDR3SPR,             // 198
	WORLDKEENDL1SPR,             // 199
	WORLDKEENDL2SPR,             // 200
	WORLDKEENDL3SPR,             // 201
	WORLDKEENUL1SPR,             // 202
	WORLDKEENUL2SPR,             // 203
	WORLDKEENUL3SPR,             // 204
	WORLDKEENUR1SPR,             // 205
	WORLDKEENUR2SPR,             // 206
	WORLDKEENUR3SPR,             // 207
	WORLDKEENWAVE1SPR,           // 208
	WORLDKEENWAVE2SPR,           // 209
	ROCKETSPR,                   // 210
	ROCKETFLY1SPR,               // 211
	ROCKETFLY2SPR,               // 212
	SATELLITE1SPR,               // 213
	SATELLITE2SPR,               // 214
	SATELLITE3SPR,               // 215
	SATELLITE4SPR,               // 216
	GRABBITER1SPR,               // 217
	GRABBITER2SPR,               // 218
	GRABBITERSLEEP1SPR,          // 219
	GRABBITERSLEEP2SPR,          // 220
	WORLDKEENTRHOW1SPR,          // 221
	WORLDKEENTRHOW2SPR,          // 222
	WORLDKEENCLIMB1SPR,          // 223
	WORLDKEENCLIMB2SPR,          // 224
	ROPETHROW1SPR,               // 225
	ROPETHROW2SPR,               // 226
	WORLDKEENHANGSPR,            // 227
	FLAGFLIP1SPR,                // 228
	FLAGFLIP2SPR,                // 229
	FLAGFLIP3SPR,                // 230
	FLAGFLIP4SPR,                // 231
	FLAGFLIP5SPR,                // 232
	FLAGFALL1SPR,                // 233
	FLAGFALL2SPR,                // 234
	FLAGFLAP1SPR,                // 235
	FLAGFLAP2SPR,                // 236
	FLAGFLAP3SPR,                // 237
	FLAGFLAP4SPR,                // 238
	END_LUMP(WORLDKEEN_LUMP_END, __WORLDKEENEND)

	START_LUMP(FLEEX_LUMP_START, __FLEEXSTART)
	FLEEXWALKR1SPR,              // 239
	FLEEXWALKR2SPR,              // 240
	FLEEXWALKL1SPR,              // 241
	FLEEXWALKL2SPR,              // 242
	FLEEXLOOK1SPR,               // 243
	FLEEXLOOK2SPR,               // 244
	FLEEXSTUNSPR,             // 245
	END_LUMP(FLEEX_LUMP_END, __FLEEXEND)

	START_LUMP(CEILICK_LUMP_START, __CEILICKSTART)
	CEILICK1SPR,                 // 246
	CEILICK2SPR,                 // 247
	TONGUE1SPR,                  // 248
	TONGUE2SPR,                  // 249
	TONGUE3SPR,                  // 250
	TONGUE4SPR,                  // 251
	TONGUE5SPR,                  // 252
	CEILICKSTUNSPR,           // 253
	END_LUMP(CEILICK_LUMP_END, __CEILICKEND)

	START_LUMP(BLOOGUARD_LUMP_START, __BLOOGUARDSTART)
	BLOOGUARDWALKL1SPR,          // 254
	BLOOGUARDWALKL2SPR,          // 255
	BLOOGUARDWALKL3SPR,          // 256
	BLOOGUARDWALKL4SPR,          // 257
	BLOOGUARDWALKR1SPR,          // 258
	BLOOGUARDWALKR2SPR,          // 259
	BLOOGUARDWALKR3SPR,          // 260
	BLOOGUARDWALKR4SPR,          // 261
	BLOOGUARDSWINGL1SPR,         // 262
	BLOOGUARDSWINGL2SPR,         // 263
	BLOOGUARDSWINGL3SPR,         // 264
	BLOOGUARDSWINGR1SPR,         // 265
	BLOOGUARDSWINGR2SPR,         // 266
	BLOOGUARDSWINGR3SPR,         // 267
	BLOOGUARDSTUNSPR,         // 268
	END_LUMP(BLOOGUARD_LUMP_END, __BLOOGUARDEND)

	START_LUMP(BIPSHIP_LUMP_START, __BIPSHIPSTART)
	BIPSHIPRSPR,                 // 269
	BIPSHIPRTURN1SPR,            // 270
	BIPSHIPRTURN2SPR,            // 271
	BIPSHIPRTURN3SPR,            // 272
	BIPSHIPRTURN4SPR,            // 273
	BIPSHIPLSPR,                 // 274
	BIPSHIPLTURN1SPR,            // 275
	BIPSHIPLTURN2SPR,            // 276
	BIPSHIPLTURN3SPR,            // 277
	BIPSHIPLTURN4SPR,            // 278
	BIPSHIPEXPLODE1SPR,          // 279
	BIPSHIPEXPLODE2SPR,          // 280
	BIPSHIPEXPLODE3SPR,          // 281
	BIPSHIPEXPLODE4SPR,          // 282
	BIPSHIPEXPLODE5SPR,          // 283
	BIPSHIPSHOTSPR,              // 284
	END_LUMP(BIPSHIP_LUMP_END, __BIPSHIPEND)

	START_LUMP(BABOBBA_LUMP_START, __BABOBBASTART)
	BABOBBAL1SPR,                // 285
	BABOBBAL2SPR,                // 286
	BABOBBAL3SPR,                // 287
	BABOBBAR1SPR,                // 288
	BABOBBAR2SPR,                // 289
	BABOBBAR3SPR,                // 290
	BABOBBASHOT1SPR,             // 291
	BABOBBASHOT2SPR,             // 292
	BABOBBASTUNSPR,           // 293
	BABOBBASLEEP1SPR,            // 294
	BABOBBASLEEP2SPR,            // 295
	BABOBBASLEEP3SPR,            // 296
	BABOBBASLEEP4SPR,            // 297
	END_LUMP(BABOBBA_LUMP_END, __BABOBBAEND)

	START_LUMP(NOSPIKE_LUMP_START, __NOSPIKESTART)
	NOSPIKESTANDSPR,             // 298
	NOSPIKERUNR1SPR,             // 299
	NOSPIKERUNR2SPR,             // 300
	NOSPIKERUNR3SPR,             // 301
	NOSPIKERUNR4SPR,             // 302
	NOSPIKERUNL1SPR,             // 303
	NOSPIKERUNL2SPR,             // 304
	NOSPIKERUNL3SPR,             // 305
	NOSPIKERUNL4SPR,             // 306
	NOSPIKEWALKR1SPR,            // 307
	NOSPIKEWALKR2SPR,            // 308
	NOSPIKEWALKR3SPR,            // 309
	NOSPIKEWALKR4SPR,            // 310
	NOSPIKEWALKL1SPR,            // 311
	NOSPIKEWALKL2SPR,            // 312
	NOSPIKEWALKL3SPR,            // 313
	NOSPIKEWALKL4SPR,            // 314
	NOSPIKESTUNSPR,           // 315
	QUESTIONMARKSPR,             // 316
	END_LUMP(NOSPIKE_LUMP_END, __NOSPIKEEND)

	START_LUMP(FLECT_LUMP_START, __FLECTSTART)
	FLECTSTANDSPR,               // 317
	FLECTSTANDRSPR,              // 318
	FLECTWALKR1SPR,              // 319
	FLECTWALKR2SPR,              // 320
	FLECTWALKR3SPR,              // 321
	FLECTWALKR4SPR,              // 322
	FLECTSTANDLSPR,              // 323
	FLECTWALKL1SPR,              // 324
	FLECTWALKL2SPR,              // 325
	FLECTWALKL3SPR,              // 326
	FLECTWALKL4SPR,              // 327
	FLECTSTUNSPR,             // 328
	END_LUMP(FLECT_LUMP_END, __FLECTEND)

	START_LUMP(ORBATRIX_LUMP_START, __ORBATRIXSTART)
	ORBATRIX1SPR,                // 329
	ORBATRIX2SPR,                // 330
	ORBATRIX3SPR,                // 331
	ORBATRIX4SPR,                // 332
	ORBATRIXL1SPR,               // 333
	ORBATRIXL2SPR,               // 334
	ORBATRIXR1SPR,               // 335
	ORBATRIXR2SPR,               // 336
	ORBATRIXSPIN1SPR,            // 337
	ORBATRIXSPIN2SPR,            // 338
	ORBATRIXSPIN3SPR,            // 339
	ORBATRIXSPIN4SPR,            // 340
	ORBATRIXCURLSPR,             // 341
	END_LUMP(ORBATRIX_LUMP_END, __ORBATRIXEND)

	START_LUMP(BLOOG_LUMP_START, __BLOOGSTART)
	BLOOGWALKR1SPR,              // 342
	BLOOGWALKR2SPR,              // 343
	BLOOGWALKR3SPR,              // 344
	BLOOGWALKR4SPR,              // 345
	BLOOGWALKL1SPR,              // 346
	BLOOGWALKL2SPR,              // 347
	BLOOGWALKL3SPR,              // 348
	BLOOGWALKL4SPR,              // 349
	BLOOGSTUNSPR,             // 350
	END_LUMP(BLOOG_LUMP_END, __BLOOGEND)

	START_LUMP(RBLOOGLET_LUMP_START, __RBLOOGLETSTART)
	RBLOOGLETWALKR1SPR,          // 351
	RBLOOGLETWALKR2SPR,          // 352
	RBLOOGLETWALKR3SPR,          // 353
	RBLOOGLETWALKR4SPR,          // 354
	RBLOOGLETWALKL1SPR,          // 355
	RBLOOGLETWALKL2SPR,          // 356
	RBLOOGLETWALKL3SPR,          // 357
	RBLOOGLETWALKL4SPR,          // 358
	RBLOOGLETSTUNSPR,         // 359
	END_LUMP(RBLOOGLET_LUMP_END, __RBLOOGLETEND)

	START_LUMP(YBLOOGLET_LUMP_START, __YBLOOGLETSTART)
	YBLOOGLETWALKR1SPR,          // 360
	YBLOOGLETWALKR2SPR,          // 361
	YBLOOGLETWALKR3SPR,          // 362
	YBLOOGLETWALKR4SPR,          // 363
	YBLOOGLETWALKL1SPR,          // 364
	YBLOOGLETWALKL2SPR,          // 365
	YBLOOGLETWALKL3SPR,          // 366
	YBLOOGLETWALKL4SPR,          // 367
	YBLOOGLETSTUNSPR,         // 368
	END_LUMP(YBLOOGLET_LUMP_END, __YBLOOGLETEND)

	START_LUMP(BBLOOGLET_LUMP_START, __BBLOOGLETSTART)
	BBLOOGLETWALKR1SPR,          // 369
	BBLOOGLETWALKR2SPR,          // 370
	BBLOOGLETWALKR3SPR,          // 371
	BBLOOGLETWALKR4SPR,          // 372
	BBLOOGLETWALKL1SPR,          // 373
	BBLOOGLETWALKL2SPR,          // 374
	BBLOOGLETWALKL3SPR,          // 375
	BBLOOGLETWALKL4SPR,          // 376
	BBLOOGLETSTUNSPR,         // 377
	END_LUMP(BBLOOGLET_LUMP_END, __BBLOOGLETEND)

	START_LUMP(GBLOOGLET_LUMP_START, __GBLOOGLETSTART)
	GBLOOGLETWALKR1SPR,          // 378
	GBLOOGLETWALKR2SPR,          // 379
	GBLOOGLETWALKR3SPR,          // 380
	GBLOOGLETWALKR4SPR,          // 381
	GBLOOGLETWALKL1SPR,          // 382
	GBLOOGLETWALKL2SPR,          // 383
	GBLOOGLETWALKL3SPR,          // 384
	GBLOOGLETWALKL4SPR,          // 385
	GBLOOGLETSTUNSPR,         // 386
	END_LUMP(GBLOOGLET_LUMP_END, __GBLOOGLETEND)

	START_LUMP(GIK_LUMP_START, __GIKSTART)
	GIKWALKR1SPR,                // 387
	GIKWALKR2SPR,                // 388
	GIKWALKR3SPR,                // 389
	GIKWALKL1SPR,                // 390
	GIKWALKL2SPR,                // 391
	GIKWALKL3SPR,                // 392
	GIKJUMPLSPR,                 // 393
	GIKJUMPRSPR,                 // 394
	GIKSLIDER1SPR,               // 395
	GIKSLIDER2SPR,               // 396
	GIKSLIDEL1SPR,               // 397
	GIKSLIDEL2SPR,               // 398
	END_LUMP(GIK_LUMP_END, __GIKEND)

	START_LUMP(BLORB_LUMP_START, __BLORBSTART)
	BLORB1SPR,                   // 399
	BLORB2SPR,                   // 400
	BLORB3SPR,                   // 401
	END_LUMP(BLORB_LUMP_END, __BLORBEND)

	START_LUMP(BOBBA_LUMP_START, __BOBBASTART)
	BOBBAL1SPR,                  // 402
	BOBBAL2SPR,                  // 403
	BOBBAL3SPR,                  // 404
	BOBBAR1SPR,                  // 405
	BOBBAR2SPR,                  // 406
	BOBBAR3SPR,                  // 407
	BOBBASHOT1SPR,               // 408
	BOBBASHOT2SPR,               // 409
	BOBBASHOT3SPR,               // 410
	BOBBASHOT4SPR,               // 411
	BOBBASHOT5SPR,               // 412
	BOBBASHOT6SPR,               // 413
	END_LUMP(BOBBA_LUMP_END, __BOBBAEND)

	START_LUMP(BIP_LUMP_START, __BIPSTART)
	BIPSTANDSPR,                 // 414
	BIPWALKR1SPR,                // 415
	BIPWALKR2SPR,                // 416
	BIPWALKR3SPR,                // 417
	BIPWALKR4SPR,                // 418
	BIPWALKL1SPR,                // 419
	BIPWALKL2SPR,                // 420
	BIPWALKL3SPR,                // 421
	BIPWALKL4SPR,                // 422
	END_LUMP(BIP_LUMP_END, __BIPEND)

	START_LUMP(BIPSQUISHED_LUMP_START, __BIPSQUISHEDSTART)
	BIPSQUISHEDSPR,              // 423
	END_LUMP(BIPSQUISHED_LUMP_END, __BIPSQUISHEDEND)

	START_LUMP(PLATFORM_LUMP_START, __PLATFORMSTART)
	PLATFORMSPR,                 // 424
	PLATBIP1SPR,                 // 425
	PLATBIP2SPR,                 // 426
	PLATBIP3SPR,                 // 427
	PLATBIP4SPR,                 // 428
	PLATBIP5SPR,                 // 429
	PLATBIP6SPR,                 // 430
	PLATBIP7SPR,                 // 431
	PLATBIP8SPR,                 // 432
	END_LUMP(PLATFORM_LUMP_END, __PLATFORMEND)

	START_LUMP(MOLLY_LUMP_START, __MOLLYSTART)
	MOLLY1SPR,                   // 433
	MOLLY2SPR,                   // 434
	END_LUMP(MOLLY_LUMP_END, __MOLLYEND)

	START_LUMP(PASSCARD_LUMP_START, __PASSCARDSTART)
	PASSCARDSPR,                 // 435
	END_LUMP(PASSCARD_LUMP_END, __PASSCARDEND)

	//
	// TILES (these don't need names)
	//

	LASTTILE=STARTEXTERNS-1,

	//
	// EXTERNS
	//

	T_ENDART,                    // 5550

	ORDERSCREEN,                 // 5551
	BIGCOMMANDER,                // 5552
	BIGKEEN,                     // 5553
	OUTOFMEM,                    // 5554

	//demos
	DEMO0,                       // 5555
	DEMO1,                       // 5556
	DEMO2,                       // 5557
	DEMO3,                       // 5558
	DEMO4,                       // 5559

	NUMGRCHUNKS
} graphicnums;

#undef START_LUMP
#undef END_LUMP

#endif //__GFX_H__