include "src\lib\scroll.h"
void main() {
	bitmap_t ptmp; // player sprite
	word q=1;
	const char *cpus;
	static int persist_aniframe = 0;    /* gonna be increased to 1 before being used, so 0 is ok for default */
	page_t screen, screen2, screen3;
	map_t map;
	map_view_t mv, mv2, mv3;
	map_view_t *bg, *spri, *mask;//, *tmp;
	byte *ptr;

	/* create the map */
	map = allocMap(MAPX,MAPY); //20x15 is the resolution of the screen you can make maps smaller than 20x15 but the null space needs to be drawn properly
	initMap(&map);
	mv.map = &map;
	mv2.map = &map;
	mv3.map = &map;

	/* draw the tiles */
	ptr = map.data;
	/*data\\*/
	ptmp = bitmapLoadPcx("ptmp.pcx"); // load sprite
	setkb(1);
	modexEnter();
	modexPalUpdate(ptmp.palette);
	screen = modexDefaultPage();
	screen.width += (TILEWH*2);
	screen.height += (TILEWH*2)+QUADWH;
	mv.page = &screen;
	screen2 = modexNextPage(mv.page);
	mv2.page = &screen2;
	screen3 = screen2;
	mv3.page = &screen3;

	/* set up paging */
	bg = &mv;
	spri = &mv2;
	mask = &mv3;

//TODO: LOAD map data and position the map in the middle of the screen if smaller then screen
	mapGoTo(bg, 0, 0);
	mapGoTo(spri, 0, 0);
	//mapGoTo(mask, 0, 0);

	//TODO: put player in starting position of spot
	//default player position on the viewable map
	player.tx = bg->tx + 10;
	player.ty = bg->ty + 8;
	player.x = player.tx*TILEWH;
	player.y = player.ty*TILEWH;
	player.triggerx = player.tx;
	player.triggery = player.ty+1;
	//TODO: erase player initial draw
	modexDrawSpriteRegion(spri->page, player.x-4, player.y-TILEWH, 24, 64, 24, 32, &ptmp);
	//temp draw trigger box
	modexClearRegion(spri->page, player.triggerx*16, player.triggery*16, 16, 16, 1);
	modexClearRegion(bg->page, player.triggerx*16, player.triggery*16, 16, 16, 1);
	modexShowPage(spri->page);
	while(!keyp(1))//!keyp(1))
	{
	//top left corner & bottem right corner of map veiw be set as map edge trigger since maps are actually square
	//to stop scrolling and have the player position data move to the edge of the screen with respect to the direction
	//when player.tx or player.ty == 0 or player.tx == 20 or player.ty == 15 then stop because that is edge of map and you do not want to walk of the map

	#define INC_PER_FRAME if(q&1) persist_aniframe++; if(persist_aniframe>4) persist_aniframe = 1;	

	if(keyp(77) && !keyp(75))
	{
		if(bg->tx >= 0 && bg->tx+20 < MAPX && player.tx == bg->tx + 10 && !(player.tx+1 == TRIGGX && player.ty == TRIGGY))
		{
			for(q=1; q<=(TILEWH/SPEED); q++)
			{
				INC_PER_FRAME;
				//animatePlayer(bg, spri, mask, 1, 1, player.x, player.y, persist_aniframe, q, &ptmp);
				animatePlayer(bg, spri, 1, 1, player.x, player.y, persist_aniframe, q, &ptmp);
				mapScrollRight(bg, SPEED, q);
				mapScrollRight(spri, SPEED, q);
				//mapScrollRight(mask, SPEED);
				modexShowPage(spri->page);
			}
			player.tx++;
		}
		else if(player.tx < MAPX && !(player.tx+1 == TRIGGX && player.ty == TRIGGY))
		{
			for(q=1; q<=(TILEWH/SPEED); q++)
			{
				INC_PER_FRAME;
				player.x+=SPEED;
				//animatePlayer(bg, spri, mask, 1, 0, player.x, player.y, persist_aniframe, q, &ptmp);
				animatePlayer(bg, spri, 1, 0, player.x, player.y, persist_aniframe, q, &ptmp);
				modexShowPage(spri->page);
			}
			player.tx++;
		}
		else
		{
			modexCopyPageRegion(spri->page, bg->page, player.x-4, player.y-TILEWH, player.x-4, player.y-TILEWH, 24, 32);
			modexDrawSpriteRegion(spri->page, player.x-4, player.y-TILEWH, 24, 32, 24, 32, &ptmp);
			modexShowPage(spri->page);
		}
		player.triggerx = player.tx+1;
		player.triggery = player.ty;
	}

	if(keyp(75) && !keyp(77))
	{
		if(bg->tx > 0 && bg->tx+20 <= MAPX && player.tx == bg->tx + 10 && !(player.tx-1 == TRIGGX && player.ty == TRIGGY))
		{
			for(q=1; q<=(TILEWH/SPEED); q++)
			{
				INC_PER_FRAME;
				//animatePlayer(bg, spri, mask, 3, 1, player.x, player.y, persist_aniframe, q, &ptmp);
				animatePlayer(bg, spri, 3, 1, player.x, player.y, persist_aniframe, q, &ptmp);
				mapScrollLeft(bg, SPEED, q);
				mapScrollLeft(spri, SPEED, q);
				//mapScrollLeft(mask, SPEED);
				modexShowPage(spri->page);
			}
			player.tx--;
		}
		else if(player.tx > 1 && !(player.tx-1 == TRIGGX && player.ty == TRIGGY))
		{
			for(q=1; q<=(TILEWH/SPEED); q++)
			{
				INC_PER_FRAME;
				player.x-=SPEED;
				//animatePlayer(bg, spri, mask, 3, 0, player.x, player.y, persist_aniframe, q, &ptmp);
				animatePlayer(bg, spri, 3, 0, player.x, player.y, persist_aniframe, q, &ptmp);
				modexShowPage(spri->page);
			}
			player.tx--;
		}
		else
		{
			modexCopyPageRegion(spri->page, bg->page, player.x-4, player.y-TILEWH, player.x-4, player.y-TILEWH, 24, 32);
			modexDrawSpriteRegion(spri->page, player.x-4, player.y-TILEWH, 24, 96, 24, 32, &ptmp);
			modexShowPage(spri->page);
		}
		player.triggerx = player.tx-1;
		player.triggery = player.ty;
	}

	if(keyp(80) && !keyp(72))
	{
		if(bg->ty >= 0 && bg->ty+15 < MAPY && player.ty == bg->ty + 8 && !(player.tx == TRIGGX && player.ty+1 == TRIGGY))
		{
			for(q=1; q<=(TILEWH/SPEED); q++)
			{
				INC_PER_FRAME;
				//animatePlayer(bg, spri, mask, 2, 1, player.x, player.y, persist_aniframe, q, &ptmp);
				animatePlayer(bg, spri, 2, 1, player.x, player.y, persist_aniframe, q, &ptmp);
				mapScrollDown(bg, SPEED, q);
				mapScrollDown(spri, SPEED, q);
				//mapScrollDown(mask, SPEED);
				modexShowPage(spri->page);
			}
			player.ty++;
		}
		else if(player.ty < MAPY && !(player.tx == TRIGGX && player.ty+1 == TRIGGY))
		{
			for(q=1; q<=(TILEWH/SPEED); q++)
			{
				INC_PER_FRAME;
				player.y+=SPEED;
				//animatePlayer(bg, spri, mask, 2, 0, player.x, player.y, persist_aniframe, q, &ptmp);
				animatePlayer(bg, spri, 2, 0, player.x, player.y, persist_aniframe, q, &ptmp);
				modexShowPage(spri->page);
			}
			player.ty++;
		}
		else
		{
			modexCopyPageRegion(spri->page, bg->page, player.x-4, player.y-TILEWH, player.x-4, player.y-TILEWH, 24, 32);
			modexDrawSpriteRegion(spri->page, player.x-4, player.y-TILEWH, 24, 64, 24, 32, &ptmp);
			modexShowPage(spri->page);
		}
		player.triggerx = player.tx;
		player.triggery = player.ty+1;
	}

	if(keyp(72) && !keyp(80))
	{
		if(bg->ty > 0 && bg->ty+15 <= MAPY && player.ty == bg->ty + 8 && !(player.tx == TRIGGX && player.ty-1 == TRIGGY))
		{
			for(q=1; q<=(TILEWH/SPEED); q++)
			{
				INC_PER_FRAME;
				//animatePlayer(bg, spri, mask, 0, 1, player.x, player.y, persist_aniframe, q, &ptmp);
				animatePlayer(bg, spri, 0, 1, player.x, player.y, persist_aniframe, q, &ptmp);
				mapScrollUp(bg, SPEED, q);
				mapScrollUp(spri, SPEED, q);
				//mapScrollUp(mask, SPEED);
				modexShowPage(spri->page);
			}
			player.ty--;
		}
		else if(player.ty > 1 && !(player.tx == TRIGGX &&  player.ty-1 == TRIGGY))
		{
			for(q=1; q<=(TILEWH/SPEED); q++)
			{
				INC_PER_FRAME;
				player.y-=SPEED;
				//animatePlayer(bg, spri, mask, 0, 0, player.x, player.y, persist_aniframe, q, &ptmp);
				modexShowPage(spri->page);
				animatePlayer(bg, spri, 0, 0, player.x, player.y, persist_aniframe, q, &ptmp);
			}
			player.ty--;
		}
		else
		{
			modexCopyPageRegion(spri->page, bg->page, player.x-4, player.y-TILEWH, player.x-4, player.y-TILEWH, 24, 32);
			modexDrawSpriteRegion(spri->page, player.x-4, player.y-TILEWH, 24, 0, 24, 32, &ptmp);
			modexShowPage(spri->page);
		}
		player.triggerx = player.tx;
		player.triggery = player.ty-1;
	}
	//modexClearRegion(mask->page, 66, 66, 2, 40, 0);

	if((player.triggerx == TRIGGX && player.triggery == TRIGGY) && keyp(KEY_ENTER))
	{
		short i;
		for(i=600; i>=400; i--)
		{
			sound(i);
		}
		nosound();
	}
	}

	modexLeave();
	setkb(0);
	printf("Project 16 scroll.exe\n");
	printf("tx: %d\n", bg->tx);
	printf("ty: %d\n", bg->ty);
	printf("player.x: %d\n", player.x);
	printf("player.y: %d\n", player.y);
	printf("player.tx: %d\n", player.tx);
	printf("player.ty: %d\n", player.ty);
	printf("player.triggx: %d\n", player.triggerx);
	printf("player.triggy: %d\n", player.triggery);
	printf("temporary player sprite 0: http://www.pixiv.net/member_illust.php?mode=medium&illust_id=45556867\n");
	printf("temporary player sprite 1: http://www.pixiv.net/member_illust.php?mode=medium&illust_id=44606385\n");
	printf("\n");
	switch(detectcpu())
	{
		case 0: cpus = "8086/8088 or 186/88"; break;
		case 1: cpus = "286"; break;
		case 2: cpus = "386 or newer"; break;
		default: cpus = "internal error"; break;
	}
	printf("detected CPU type: %s\n", cpus);
}
