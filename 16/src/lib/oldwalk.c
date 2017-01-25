void oldwalk(map_view_t *pip, player_t *player, word pn)
{
	//printf("player[%d].d=%d\n", pn, player[pn].enti.d);
	switch(player[pn].enti.d)
	{
		//no direction
		case 2:
			//0000pip[0].video->startclk = (*clockw);
		break;
		//right movement
		case 3:
			//printf("pip[0].page->tilesw=%d	", pip[0].page->tilesw); printf("pip[0].page->tw=%d\n", pip[0].page->tw);
			if(pip[0].tx >= 0 && pip[0].tx+pip[0].page->tw < pip[0].map->width && player[pn].enti.tx == pip[0].tx+pip[0].page->tilemidposscreenx &&
			!(pip[0].map->data[(player[pn].enti.tx)+(pip[0].map->width*(player[pn].enti.ty-1))] == 0))//!(player[pn].enti.tx+1 == TRIGGX && player[pn].enti.ty == TRIGGY))	//collision detection!
			{
				if(player[pn].enti.q<=player[pn].enti.spt)
				{
					INC_PER_FRAME;
					ANIMATESPRIFUN(pip, player, pn, 1);
					ScrollRight(pip, player, 3, pn);
					ScrollRight(pip, player, 2, pn);
					//mapScrollRight(pip, player, !(pip[0].video->p), pn);
					mapScrollRight(pip, player, (pip[0].video->p), pn);
					if(!pageflipflop) VL_ShowPage(pip[1].page, 0, 0);
					player[pn].enti.q++;
					//0000pip[0].video->clk = ((*clockw)-pip[0].video->startclk)/18.2;
				} else { player[pn].enti.q = 1; player[pn].enti.d = 2; player[pn].enti.tx++; }
			}
			else if(player[pn].enti.tx < pip[0].map->width && !(pip[0].map->data[(player[pn].enti.tx)+(pip[0].map->width*(player[pn].enti.ty-1))] == 0))//!(player[pn].enti.tx+1 == TRIGGX && player[pn].enti.ty == TRIGGY))
			{
				if(player[pn].enti.q<=player[pn].enti.spt)
				{
					INC_PER_FRAME;
					player[pn].enti.x+=(player[pn].enti.speed);
					ANIMATESPRIFUN(pip, player, pn, 0);
					if(!pageflipflop) VL_ShowPage(pip[1].page, 0, 0);
					player[pn].enti.q++;
				} else { player[pn].enti.q = 1; player[pn].enti.d = 2; player[pn].enti.tx++; }
			}
			else
			{
				if(!pageflipflop) modexCopyPageRegion(pip[1].page, pip[0].page, player[pn].enti.x, player[pn].enti.y-TILEWH, player[pn].enti.x, player[pn].enti.y-TILEWH, 24, 32);
#ifdef SPRITE
				modexDrawSpriteRegion(pip[0].page, player[pn].enti.x, player[pn].enti.y-TILEWH, 24, 32, 24, 32, PLAYERBMPDATAPTR);
#else
				modexClearRegion(pip[1].page, player[pn].enti.x, player[pn].enti.y-TILEWH, 24, 32, 14);
#endif
				if(!pageflipflop) VL_ShowPage(pip[1].page, 0, 0);
				player[pn].enti.d = 2;
			}
			player[pn].enti.triggerx = player[pn].enti.tx+1;
			player[pn].enti.triggery = player[pn].enti.ty;
		break;

		//left movement
		case 1:
			if(pip[0].tx > 0 && pip[0].tx+pip[0].page->tw <= pip[0].map->width && player[pn].enti.tx == pip[0].tx+pip[0].page->tilemidposscreenx &&
			!(pip[0].map->data[(player[pn].enti.tx-2)+(pip[0].map->width*(player[pn].enti.ty-1))] == 0))//!(player[pn].enti.tx-1 == TRIGGX && player[pn].enti.ty == TRIGGY))	//collision detection!
			{
				if(player[pn].enti.q<=player[pn].enti.spt)
				{
					INC_PER_FRAME;
					ANIMATESPRIFUN(pip, player, pn, 1);
					ScrollLeft(pip, player, 3, pn);
					ScrollLeft(pip, player, 2, pn);
					//mapScrollLeft(pip, player, !(pip[0].video->p), pn);
					mapScrollLeft(pip, player, (pip[0].video->p), pn);
					if(!pageflipflop) VL_ShowPage(pip[1].page, 0, 0);
					player[pn].enti.q++;
					//0000pip[0].video->clk = ((*clockw)-pip[0].video->startclk)/18.2;
				} else { player[pn].enti.q = 1; player[pn].enti.d = 2; player[pn].enti.tx--; }
			}
			else if(player[pn].enti.tx > 1 && !(pip[0].map->data[(player[pn].enti.tx-2)+(pip[0].map->width*(player[pn].enti.ty-1))] == 0))//!(player[pn].enti.tx-1 == TRIGGX && player[pn].enti.ty == TRIGGY))
			{
				if(player[pn].enti.q<=player[pn].enti.spt)
				{
					INC_PER_FRAME;
					player[pn].enti.x-=(player[pn].enti.speed);
					ANIMATESPRIFUN(pip, player, pn, 0);
					if(!pageflipflop) VL_ShowPage(pip[1].page, 0, 0);
					player[pn].enti.q++;
				} else { player[pn].enti.q = 1; player[pn].enti.d = 2; player[pn].enti.tx--; }
			}
			else
			{
				if(!pageflipflop) modexCopyPageRegion(pip[1].page, pip[0].page, player[pn].enti.x, player[pn].enti.y-TILEWH, player[pn].enti.x, player[pn].enti.y-TILEWH, 24, 32);
#ifdef SPRITE
				modexDrawSpriteRegion(pip[0].page, player[pn].enti.x, player[pn].enti.y-TILEWH, 24, 96, 24, 32, PLAYERBMPDATAPTR);
#else
				modexClearRegion(pip[1].page, player[pn].enti.x, player[pn].enti.y-TILEWH, 24, 32, 10);
#endif
				if(!pageflipflop) VL_ShowPage(pip[1].page, 0, 0);
				player[pn].enti.d = 2;
			}
			player[pn].enti.triggerx = player[pn].enti.tx-1;
			player[pn].enti.triggery = player[pn].enti.ty;
		break;

		//down movement
		case 4:
			if(pip[0].ty >= 0 && pip[0].ty+pip[0].page->th < pip[0].map->height && player[pn].enti.ty == pip[0].ty+pip[0].page->tilemidposscreeny &&
			!(pip[0].map->data[(player[pn].enti.tx-1)+(pip[0].map->width*(player[pn].enti.ty))] == 0))//!(player[pn].enti.tx == TRIGGX && player[pn].enti.ty+1 == TRIGGY))	//collision detection!
			{
				if(player[pn].enti.q<=player[pn].enti.spt)
				{
					INC_PER_FRAME;
					ANIMATESPRIFUN(pip, player, pn, 1);
					ScrollDown(pip, player, 3, pn);
					ScrollDown(pip, player, 2, pn);
					//mapScrollDown(pip, player, !(pip[0].video->p), pn);
					mapScrollDown(pip, player, (pip[0].video->p), pn);
					if(!pageflipflop) VL_ShowPage(pip[1].page, 0, 0);
					player[pn].enti.q++;
					//0000pip[0].video->clk = ((*clockw)-pip[0].video->startclk)/18.2;
				} else { player[pn].enti.q = 1; player[pn].enti.d = 2; player[pn].enti.ty++; }
			}
			else if(player[pn].enti.ty < pip[0].map->height && !(pip[0].map->data[(player[pn].enti.tx-1)+(pip[0].map->width*(player[pn].enti.ty))] == 0))//!(player[pn].enti.tx == TRIGGX && player[pn].enti.ty+1 == TRIGGY))
			{
				if(player[pn].enti.q<=player[pn].enti.spt)
				{
					INC_PER_FRAME;
					player[pn].enti.y+=(player[pn].enti.speed);
					ANIMATESPRIFUN(pip, player, pn, 0);
					if(!pageflipflop) VL_ShowPage(pip[1].page, 0, 0);
					player[pn].enti.q++;
				} else { player[pn].enti.q = 1; player[pn].enti.d = 2; player[pn].enti.ty++; }
			}
			else
			{
				if(!pageflipflop) modexCopyPageRegion(pip[1].page, pip[0].page, player[pn].enti.x, player[pn].enti.y-TILEWH, player[pn].enti.x, player[pn].enti.y-TILEWH, 24, 32);
#ifdef SPRITE
				modexDrawSpriteRegion(pip[0].page, player[pn].enti.x, player[pn].enti.y-TILEWH, 24, 64, 24, 32, PLAYERBMPDATAPTR);
#else
				modexClearRegion(pip[1].page, player[pn].enti.x, player[pn].enti.y-TILEWH, 24, 32, 9);
#endif
				if(!pageflipflop) VL_ShowPage(pip[1].page, 0, 0);
				player[pn].enti.d = 2;
			}
			player[pn].enti.triggerx = player[pn].enti.tx;
			player[pn].enti.triggery = player[pn].enti.ty+1;
		break;

		//up movement
		case 0:
			if(pip[0].ty > 0 && pip[0].ty+pip[0].page->th <= pip[0].map->height && player[pn].enti.ty == pip[0].ty+pip[0].page->tilemidposscreeny &&
			!(pip[0].map->data[(player[pn].enti.tx-1)+(pip[0].map->width*(player[pn].enti.ty-2))] == 0))//!(player[pn].enti.tx == TRIGGX && player[pn].enti.ty-1 == TRIGGY))	//collision detection!
			{
				if(player[pn].enti.q<=player[pn].enti.spt)
				{
					INC_PER_FRAME;
					ANIMATESPRIFUN(pip, player, pn, 1);
					ScrollUp(pip, player, 3, pn);
					ScrollUp(pip, player, 2, pn);
					//mapScrollUp(pip, player, !(pip[0].video->p), pn);
					mapScrollUp(pip, player, (pip[0].video->p), pn);
					if(!pageflipflop) VL_ShowPage(pip[1].page, 0, 0);
					player[pn].enti.q++;
					//0000pip[0].video->clk = ((*clockw)-pip[0].video->startclk)/18.2;
				} else { player[pn].enti.q = 1; player[pn].enti.d = 2; player[pn].enti.ty--; }
			}
			else if(player[pn].enti.ty > 1 && !(pip[0].map->data[(player[pn].enti.tx-1)+(pip[0].map->width*(player[pn].enti.ty-2))] == 0))//!(player[pn].enti.tx == TRIGGX &&  player[pn].enti.ty-1 == TRIGGY))
			{
				if(player[pn].enti.q<=player[pn].enti.spt)
				{
					INC_PER_FRAME;
					player[pn].enti.y-=(player[pn].enti.speed);
					ANIMATESPRIFUN(pip, player, pn, 0);
					if(!pageflipflop) VL_ShowPage(pip[1].page, 0, 0);
					player[pn].enti.q++;
				} else { player[pn].enti.q = 1; player[pn].enti.d = 2; player[pn].enti.ty--; }
			}
			else
			{
				if(!pageflipflop) modexCopyPageRegion(pip[1].page, pip[0].page, player[pn].enti.x, player[pn].enti.y-TILEWH, player[pn].enti.x, player[pn].enti.y-TILEWH, 24, 32);
#ifdef SPRITE
				modexDrawSpriteRegion(pip[0].page, player[pn].enti.x, player[pn].enti.y-TILEWH, 24, 0, 24, 32, PLAYERBMPDATAPTR);
#else
				modexClearRegion(pip[1].page, player[pn].enti.x, player[pn].enti.y-TILEWH, 24, 32, 12);
#endif
				if(!pageflipflop) VL_ShowPage(pip[1].page, 0, 0);
				player[pn].enti.d = 2;
			}
			player[pn].enti.triggerx = player[pn].enti.tx;
			player[pn].enti.triggery = player[pn].enti.ty-1;
		break;
	}
}
