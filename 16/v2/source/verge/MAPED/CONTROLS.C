/*
Copyright (C) 1998 BJ Eirich (aka vecna)
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public Lic
See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/


// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// CHANGELOG:
// <zero 5.7.99>
// + fixed ScreenShot() to properly save screenshots with
//   the sequential filenames.
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// ================================= Data ====================================

// ================================= Code ====================================

int CheckMouseTabs()
{ int i;

  for (i=0; i<10; i++)
  {
    if (i<6 && i>=numlayers) continue;
    if (mb==1 && mx>17+(i*10) && mx<26+(i*10) && my>(tsy-7) && my<tsy)
    {   layertoggle[i]=layertoggle[i] ^ 1; WaitRelease(); return 1; }
    if (mb==2 && mx>17+(i*10) && mx<26+(i*10) && my>(tsy-7) && my<tsy)
    {   el=i; WaitRelease(); return 1; }
  }
  return 0;
}

HAL()
{ int i;

  Message("I'm sorry dave, I can't do that...",600);
  i=systemtime+400;
  while (systemtime<i)
  {
    RenderMap();
    RenderGUI();
    ShowPage();
  }
  Message("Just kidding! :)",200);
}

void WritePCXLine(unsigned char *p,int len,FILE *pcxf)
{ int i;
  unsigned char byt, samect, repcode;

  i=0;
  do
  {   byt=p[i++];
      samect=1;
      while (samect<(unsigned) 63 && i<len && byt==p[i])
      {
         samect++;
         i++;
      }
      if (samect>1 || (byt & 0xC0) != 0)
      {
         repcode=0xC0 | samect;
         fwrite(&repcode,1,1,pcxf);
      }
      fwrite(&byt,1,1,pcxf);
  } while (i<len);
}

acopytile(int x, int y, char *src, char *pt)
{
  char *d;
  int h;

  h=16;

  y<<=6;
  d=pt+y+(y<<2)+x;

  for (; h; h--)
  {
    memcpy(d,src,16);
    src+=16;
    d+=320;
  }
}

extern FILE *pcxf;

void WritePalette(FILE *f)
{ char b;
  int i;

  for (i=0; i<768; i++)
      pal[i]=pal[i] << 2;

  b=12; fwrite(&b, 1, 1, f);
  fwrite(pal, 1, 768, f);

  for (i=0; i<768; i++)
      pal[i]=pal[i] >> 2;
}

void OutputVSPpcx()
{ int i,j,_x,_y;
  unsigned char *ptr;
  char b1;
  short int w1,w2;

  // First figure out the filename

  i=0;
  ptr=&vspname;
  while (*ptr != '.')
  {
    strbuf[i]=*ptr;
    ptr++;
    i++;
  }
  strbuf[i++]='.';
  strbuf[i++]='P';
  strbuf[i++]='C';
  strbuf[i++]='X';
  strbuf[i++]=0;

  pcxf=fopen(strbuf,"wb");

// Write PCX header

   b1=10; fwrite(&b1, 1, 1, pcxf); // manufacturer always = 10
   b1=5; fwrite(&b1, 1, 1, pcxf);  // version = 3.0, >16 colors
   b1=1; fwrite(&b1, 1, 1, pcxf);  // encoding always = 1
   b1=8; fwrite(&b1, 1, 1, pcxf);  // 8 bits per pixel, for 256 colors
   w1=0; fwrite(&w1, 1, 2, pcxf);  // xmin = 0;
   w1=0; fwrite(&w1, 1, 2, pcxf);  // ymin = 0;
 w1=319; fwrite(&w1, 1, 2, pcxf);  // xmax = 319;

 // At this point we need to figure out how many vertical rows tall the
 // PCX needs to be in order to accomidate the number of tiles in the VSP.

 w2=numtiles/18; w2++;
 w2=(w2*17); fwrite(&w2, 1, 2, pcxf);  // ymax = ?;
 w1=320; fwrite(&w1, 1, 2, pcxf);      // hres = 320;
 w2++; fwrite(&w2, 1, 2, pcxf);        // vres = ?;

 fwrite(screen, 1, 48, pcxf);  // 16-color palette data. Who knows what's
                               // actually in here. It doesn't matter since
                               // the 256-color palette is stored elsewhere.

 b1=0; fwrite(&b1, 1, 1, pcxf);   // reserved always = 0.
 b1=1; fwrite(&b1, 1, 1, pcxf);   // number of color planes. Just 1 for 8bit.
 w1=320; fwrite(&w1, 1, 2, pcxf); // number of bytes per line

 w1=0; fwrite(&w1, 1, 1, pcxf);
 fwrite(screen, 1, 59, pcxf);          // filler

 // The header is written. Now we need to generate a large buffer where we'll
 // "draw" the PCX, and then encode the data and save the PCX.

 ptr=(char *) malloc(320*w2);
 memset(ptr,255,(320*w2));            // Give the PCX a white background

 // Now we "draw" the tiles into the buffer.

 for (i=0; i<numtiles; i++)
 {
    j=i/18;
    _y=1+(j*17);
    j=i-(j*18);
    _x=1+(j*17);
    acopytile(_x,_y,vsp+(i*256),ptr);
 }

 // And now we save the rest of the PCX.

 for (w1=0; w1<w2; w1++)
     WritePCXLine(ptr+(w1*320),320,pcxf);

  WritePalette(pcxf);
  fclose(pcxf);

  vfree(ptr);
}

void ScreenShot()
{
  unsigned char b1;
  unsigned short int w1;
  int i,n; //zero 5.7.99
  char fnamestr[13];
  static int ss=0;
  FILE *pcxf;

//--- zero 5.7.99
  n=0;
  do
  {
    sprintf(fnamestr,"%d.pcx",n);
    pcxf=fopen(fnamestr,"r");
    i=(int)pcxf;
    if(pcxf) fclose(pcxf);
    n++;
  } while(i);
  n--;

  // Takes a snapshot of the current screen.

   sprintf(fnamestr,"%d.pcx",n);
//---

   pcxf=fopen(&fnamestr,"wb");
   ss++;

// Write PCX header

   b1=10; fwrite(&b1, 1, 1, pcxf); // manufacturer always = 10
   b1=5; fwrite(&b1, 1, 1, pcxf);  // version = 3.0, >16 colors
   b1=1; fwrite(&b1, 1, 1, pcxf);  // encoding always = 1
   b1=8; fwrite(&b1, 1, 1, pcxf);  // 8 bits per pixel, for 256 colors
   w1=0; fwrite(&w1, 1, 2, pcxf);  // xmin = 0;
   w1=0; fwrite(&w1, 1, 2, pcxf);  // ymin = 0;
 w1=sx-1; fwrite(&w1, 1, 2, pcxf);  // xmax = 319;
 w1=sy-1; fwrite(&w1, 1, 2, pcxf);  // ymax = 199;
 w1=sx; fwrite(&w1, 1, 2, pcxf);  // hres = 320;
 w1=sy; fwrite(&w1, 1, 2, pcxf);  // vres = 200;

 fwrite(screen,1,48,pcxf);

 b1=0; fwrite(&b1, 1, 1, pcxf);   // reserved always = 0.
 b1=1; fwrite(&b1, 1, 1, pcxf);   // number of color planes. Just 1 for 8bit.
 w1=sx; fwrite(&w1, 1, 2, pcxf); // number of bytes per line

 w1=0; fwrite(&w1, 1, 1, pcxf);
 fwrite(screen, 1, 59, pcxf);          // filler

 for (w1=0; w1<sy; w1++)
     WritePCXLine(screen+((w1+16)*tsx)+16, sx, pcxf);

 WritePalette(pcxf);
 fclose(pcxf);
 timer_count=0;
}

// aen -- put this out here because the PollMovement() routine i added
//        needs access to them.

int ms_right=0;
int ms_down=0;
int ms_left=0;
int ms_up=0;

void PollMovement() // aen
{
  int xmax, ymax;

  // d'oh! :p
  ms_right=0;
  ms_down=0;
  ms_left=0;
  ms_up=0;

  xmax = (layer[0].sizex * 16) - sx;
  ymax = (layer[0].sizey * 16) - sy;

  // aen: made all scroll code apply movement first, correct afterwards. if
  // not done this way, bugs crop up with the movement when holding page
  // down, to scroll to the bottom of the map (in tile scroll), then holding
  // the up arrow key to scroll back up to the top (will scroll into the
  // negatives).

  if (mouse_scroll)
  {
    if (my<18) ms_up=1;
    if (my>(tsy-2)) ms_down=1;
    if (mx>334) ms_right=1;
    if (mx<18) ms_left=1;
  }

  if (key[SCAN_UP] || ms_up)
  {
    if (scrollmode)
    {
       ywin -= map_scroll_y;
       if (ywin < 0) ywin = 0;
    }
    else
    {
      if (key[SCAN_UP] || (!(systemtime%10)))
      {
        key[SCAN_UP]=0;
        ywin -= 16;
        if (ywin < 0) ywin = 0;
        systemtime++;
      }
    }
  }
  if (key[SCAN_DOWN] || ms_down)
  {
    if (scrollmode)
    {
      ywin += map_scroll_y;
      if (ywin > ymax)
        ywin = ymax;
    }
    else
    {
      if (key[SCAN_DOWN] || (!(systemtime%10)))
      {
        key[SCAN_DOWN]=0;
        ywin += 16;
        if (ywin > ymax)
          ywin = ymax;
        systemtime++;
      }
    }
  }
  if (key[SCAN_RIGHT] || ms_right)
  {
    if (scrollmode)
    {
      xwin += map_scroll_x;
      if (xwin > xmax)
        xwin = xmax;
    }
    else
    {
      if (key[SCAN_RIGHT] || (!(systemtime%10)))
      {
         key[SCAN_RIGHT]=0;
         xwin += 16;
         if (xwin > xmax)
           xwin = xmax;
         systemtime++;
      }
    }
  }
  if (key[SCAN_LEFT] || ms_left)
  {
    if (scrollmode)
    {
      xwin -= map_scroll_x;
      if (xwin < 0) xwin=0;
    }
    else
    {
       if (key[SCAN_LEFT] || (!(systemtime%10)))
       {
         key[SCAN_LEFT]=0;
         xwin -= 16;
         if (xwin < 0) xwin = 0;
         systemtime++;
       }
    }
  }
}

ProcessControls()
{
  int xmax, ymax;

  xmax = (layer[0].sizex * 16) - sx;
  ymax = (layer[0].sizey * 16) - sy;

  if (key[SCAN_F5]) { key[SCAN_F5]=0; ShellMAP(); }
  if (key[SCAN_F6]) { key[SCAN_F6]=0; CompileAll(); }
  if (key[SCAN_F8]) { key[SCAN_F8]=0; ShellVERGE(); }
  if (key[SCAN_ALT] && key[SCAN_D]) { key[SCAN_D]=0; ShellToDOS(); }
  if (key[SCAN_ALT] && key[SCAN_L]) LoadMAPDialog();
  if (key[SCAN_ALT] && key[SCAN_V]) ShellEditMAP();
  if (key[SCAN_ALT] && key[SCAN_S]) ShellEditSystem();
  if (key[SCAN_ALT] && key[SCAN_N]) NewMAP();
  if (key[SCAN_ALT] && key[SCAN_P]) MPDialog();
  if (!key[SCAN_ALT] && key[SCAN_P]) { pasting^=1; key[SCAN_P]=0; } // aen

  if (key[SCAN_A] && el<6)
  {
    lt++;
    if (lt==numtiles) lt=0;
    key[SCAN_A]=0;
  }
  if (key[SCAN_Z] && el<6)
  {
    if (lt) lt--;
    else lt=numtiles-1;
    key[SCAN_Z]=0;
  }
  if (key[SCAN_A] && el==7)
  {
    curzone++;
    key[SCAN_A]=0;
  }
  if (key[SCAN_Z] && el==7)
  {
    curzone--;
    key[SCAN_Z]=0;
  }
  if (key[SCAN_S])
  {
    rt++;
    if (rt==numtiles) rt=0;
    key[SCAN_S]=0;
  }
  if (key[SCAN_X])
  {
    if (rt) rt--;
    else rt=numtiles-1;
    key[SCAN_X]=0;
  }
  if (key[SCAN_F10])
  {
    key[SCAN_F10]=0;
    if (random(0,255)<15) HAL();
    SaveMAP(mapname);
    SaveVSP(vspname);
    CompileMAP();
    Message("MAP/VSP saved.",100);
    modified=0;
  }
  if (key[SCAN_C])
  {
    key[SCAN_C]=0;
    sprintf(strbuf,"Left: %d Right: %d", lt, rt);
    Message(strbuf, 300);
  }
  if (key[SCAN_M])
  {
    key[SCAN_M]=0;
    GenerateMiniVSP();
    MiniMAP();
  }
  if (key[SCAN_H])
  {
    key[SCAN_H]=0;
    mh=mh^1;
    if (mh) Message("MAP Tile Highlight enabled.",100);
       else Message("MAP Tile Highlight disabled.",100);
  }

  // ***
  // movement code moved to PollMovement()
  // ***

  if (key[SCAN_PGUP])
  {
    key[SCAN_PGUP]=0;
    ywin -= sy;
    if (ywin < 0)
      ywin = 0;
  }
  if (key[SCAN_HOME])
  {
    key[SCAN_HOME]=0;
    xwin -= sx;
    if (xwin < 0)
      xwin = 0;
  }
  if (key[SCAN_END])
  {
    key[SCAN_END]=0;
    xwin += sx;
    if (xwin > xmax)
      xwin = xmax;
  }
  if (key[SCAN_PGDN])
  {
    key[SCAN_PGDN]=0;
    ywin += sy;
    if (ywin > ymax)
      ywin = ymax;
  }

  if (key[SCAN_CTRL] && el<6)
  {
    key[SCAN_CTRL]=0;
    TileSelector();
  }
  if (key[SCAN_CTRL] && el==7)
  {
    key[SCAN_CTRL]=0;
    ZoneEdDialog();
  }

  if (key[SCAN_TAB])
  {
    key[SCAN_TAB]=0;
    if (scrollmode)
    {
      scrollmode=0;
      xwin=xwin/16; xwin=xwin*16;
      ywin=ywin/16; ywin=ywin*16;
      Message("Tile scroll.",150);
    }
    else
    {
      scrollmode=1;
      Message("Pixel scroll.",150);
    }
  }

  if (key[SCAN_1])
  {
     if (key[SCAN_LSHIFT] || key[SCAN_RSHIFT])
     {
       layertoggle[0]=0;
       return;
     }
     layertoggle[0]=1;
     layertoggle[6]=0; layertoggle[7]=0;
     layertoggle[8]=0; layertoggle[9]=0;
     el=0;
     key[SCAN_1]=0;
  }
  if (key[SCAN_2])
  {
     if (key[SCAN_LSHIFT] || key[SCAN_RSHIFT])
     {
       layertoggle[1]=0;
       return;
     }
     key[SCAN_2]=0;
     layertoggle[6]=0; layertoggle[7]=0;
     layertoggle[8]=0; layertoggle[9]=0;
     if (numlayers>1)
     { layertoggle[1]=1;
       el=1; }
  }
  if (key[SCAN_3])
  {
     if (key[SCAN_LSHIFT] || key[SCAN_RSHIFT])
     {
       layertoggle[2]=0;
       return;
     }
     key[SCAN_3]=0;
     layertoggle[6]=0; layertoggle[7]=0;
     layertoggle[8]=0; layertoggle[9]=0;
     if (numlayers>2)
     { layertoggle[2]=1;
       el=2; }
  }
  if (key[SCAN_4])
  {
     if (key[SCAN_LSHIFT] || key[SCAN_RSHIFT])
     {
       layertoggle[3]=0;
       return;
     }
     key[SCAN_4]=0;
     layertoggle[6]=0; layertoggle[7]=0;
     layertoggle[8]=0; layertoggle[9]=0;
     if (numlayers>3)
     { layertoggle[3]=1;
       el=3; }
  }
  if (key[SCAN_5])
  {
     if (key[SCAN_LSHIFT] || key[SCAN_RSHIFT])
     {
       layertoggle[4]=0;
       return;
     }
     key[SCAN_5]=0;
     layertoggle[6]=0; layertoggle[7]=0;
     layertoggle[8]=0; layertoggle[9]=0;
     if (numlayers>4)
     { layertoggle[4]=1;
       el=4; }
  }
  if (key[SCAN_6])
  {
     if (key[SCAN_LSHIFT] || key[SCAN_RSHIFT])
     {
       layertoggle[5]=0;
       return;
     }
     key[SCAN_6]=0;
     layertoggle[6]=0; layertoggle[7]=0;
     layertoggle[8]=0; layertoggle[9]=0;
     if (numlayers>5)
     { layertoggle[5]=1;
       el=5; }
  }
  if (key[SCAN_O])
  {
     key[SCAN_O]=0;
     layertoggle[6]=1; layertoggle[7]=0;
     layertoggle[8]=0; layertoggle[9]=0;
     el=6;
  }
  if (key[SCAN_N])
  {
     key[SCAN_N]=0;
     layertoggle[6]=0; layertoggle[7]=1;
     layertoggle[8]=0; layertoggle[9]=0;
     el=7;
  }
  if (key[SCAN_E])
  {
     key[SCAN_E]=0;
     layertoggle[6]=0; layertoggle[7]=0;
     layertoggle[8]=1; layertoggle[9]=0;
     el=8;
  }
  if (key[SCAN_T])
  {
     key[SCAN_T]=0;
     layertoggle[6]=0; layertoggle[7]=0;
     layertoggle[8]=0; layertoggle[9]=1;
     el=9;
  }
  if (key[SCAN_F9])
  { // aen
    key[SCAN_F9]=0;
    mouse_scroll^=1;
    sprintf(strbuf,"Mouse scroll %sabled.", mouse_scroll ? "en" : "dis");
    Message(strbuf, 100);
  }
  if (key[SCAN_F11])
  {
    key[SCAN_F11]=0;
    ScreenShot();
    Message("Screen capture saved.",300);
  }
  if (key[SCAN_F12])
  {
    key[SCAN_F12]=0;
    OutputVSPpcx();
    Message("PCX file exported.",300);
  }
  if (key[SCAN_LANGLE])
  {
      key[SCAN_LANGLE]=0;
      ShutdownVideo();
      vm=0;
      InitVideo(0);
      set_intensity(63);
      InitMouse();

      // aen -- gotta recalc this if you're in a lower res at the bottom of
      //        the map and jump to a higher res.
      xmax = (layer[0].sizex * 16) - sx;
      ymax = (layer[0].sizey * 16) - sy;
      if (xwin > xmax) xwin=xmax;
      if (ywin > ymax) ywin=ymax;
  }
  if (key[SCAN_RANGLE])
  {
      key[SCAN_RANGLE]=0;
      ShutdownVideo();
      vm=1;
      InitVideo(1);
      set_intensity(63);
      InitMouse();

      // aen -- gotta recalc this if you're in a lower res at the bottom of
      //        the map and jump to a higher res.
      xmax = (layer[0].sizex * 16) - sx;
      ymax = (layer[0].sizey * 16) - sy;
      if (xwin > xmax) xwin=xmax;
      if (ywin > ymax) ywin=ymax;
  }

  if (CheckMouseTabs()) return;

  if (mb>=3) MainMenu(mx-4,my-5);
  if (key[SCAN_ESC]) DoMainMenu();

  if (mb==1 && key[SCAN_SLASH] && el<6)
  {
    lt=layers[el][((((ywin*layer[el].pmulty/layer[el].pdivy)+my-16)/16) *
             layer[el].sizex)+(((xwin*layer[el].pmultx/layer[el].pdivx)+mx-16)/16)];

    return;
  }

  if (mb==2 && key[SCAN_SLASH] && el<6)
  {
    rt=layers[el][((((ywin*layer[el].pmulty/layer[el].pdivy)+my-16)/16) *
             layer[el].sizex)+(((xwin*layer[el].pmultx/layer[el].pdivx)+mx-16)/16)];
    return;
  }

  // aen; these must come before the tile plotting code just below
  // to work correctly.
  if (mb && el<6 && key[SCAN_LSHIFT] && !shifted)
  {
    selx1=(((xwin*layer[el].pmultx/layer[el].pdivx)+mx-16)/16);
    sely1=(((ywin*layer[el].pmulty/layer[el].pdivy)+my-16)/16);
    selx2=selx1;
    sely2=sely1;
    shifted=mb;
    return;
  }
  if (mb && el<6 && shifted)
  {
    selx2=(((xwin*layer[el].pmultx/layer[el].pdivx)+mx-16)/16);
    sely2=(((ywin*layer[el].pmulty/layer[el].pdivy)+my-16)/16);
    return;
  }
  if (!mb && el<6 && shifted)
  { int i,j;
    int x1,y1,x2,y2;

    x1=selx1;
    y1=sely1;
    x2=selx2;
    y2=sely2;

    if (x2<x1) x2^=x1,x1^=x2,x2^=x1;
    if (y2<y1) y2^=y1,y1^=y2,y2^=y1;

    copybuf_wide=x2-x1+1;
    copybuf_deep=y2-y1+1;

    if (shifted==2)
    {
      // block fill
      modified=1;
      for (j=0; j<copybuf_deep; j++)
      {
        for (i=0; i<copybuf_wide; i++)
          layers[el][((y1+j)*layer[el].sizex)+(x1+i)]=lt;
      }
    }

    if (shifted==1)
    {
      modified=1;
      if (copybuf) vfree(copybuf);
      copybuf=(word *)valloc(copybuf_wide*copybuf_deep*2, "copybuf", 0);

      // copy
      for (j=0; j<copybuf_deep; j++)
      {
        for (i=0; i<copybuf_wide; i++)
          copybuf[(j*copybuf_wide)+i]=layers[el][((y1+j)*layer[el].sizex)+(x1+i)];
      }
      pasting=1;
    }
    selx1=sely1=0;
    selx2=sely2=0;
    shifted=0;
  }
  if (mb==1 && el<6 && !shifted && pasting)
  { int a,b,i,j;

    a=(((xwin*layer[el].pmultx/layer[el].pdivx)+mx-16)/16);
    b=(((ywin*layer[el].pmulty/layer[el].pdivy)+my-16)/16);

    // paste
    for (j=0; j<copybuf_deep; j++)
    {
      for (i=0; i<copybuf_wide; i++)
      {
        if (b+j<layer[el].sizey && a+i<layer[el].sizex)
          layers[el][((b+j)*layer[el].sizex)+(a+i)]=copybuf[(j*copybuf_wide)+i];
      }
    }
  }

  if (mb==1 && el<6 && !shifted && !pasting)
  {
    if (mx>335) mx=334;
    modified=1;
    layers[el][((((ywin*layer[el].pmulty/layer[el].pdivy)+my-16)/16) *
               layer[el].sizex)+(((xwin*layer[el].pmultx/layer[el].pdivx)+mx-16)
               /16)]=lt;
  }
  if (mb==2 && el<6 && !shifted)
  {
    if (mx>335) mx=334;
    modified=1;
    layers[el][((((ywin*layer[el].pmulty/layer[el].pdivy)+my-16)/16) *
               layer[el].sizex)+(((xwin*layer[el].pmultx/layer[el].pdivx)+mx-16)
               /16)]=rt;
  }

  if (mb==1 && el==6)
  {
   modified=1;
   obstruct[((((ywin*layer[0].pmulty/layer[0].pdivy)+my-16)/16) *
               layer[0].sizex)+(((xwin*layer[0].pmultx/layer[0].pdivx)+mx-16)
               /16)]=1;
  }
  if (mb==2 && el==6)
  {
   modified=1;
   obstruct[((((ywin*layer[0].pmulty/layer[0].pdivy)+my-16)/16) *
               layer[0].sizex)+(((xwin*layer[0].pmultx/layer[0].pdivx)+mx-16)
               /16)]=0;
  }
  if (mb==1 && el==7 && (key[SCAN_LSHIFT] || key[SCAN_RSHIFT]))
  {
   curzone=zone[((((ywin*layer[0].pmulty/layer[0].pdivy)+my-16)/16) *
               layer[0].sizex)+(((xwin*layer[0].pmultx/layer[0].pdivx)+mx-16)
               /16)];
   WaitRelease();
   ZoneEdDialog();
   return;
  }
  if (mb==1 && el==7)
  {
   modified=1;
   zone[((((ywin*layer[0].pmulty/layer[0].pdivy)+my-16)/16) *
               layer[0].sizex)+(((xwin*layer[0].pmultx/layer[0].pdivx)+mx-16)
               /16)]=curzone;
  }
  if (mb==2 && el==7)
  {
   modified=1;
   zone[((((ywin*layer[0].pmulty/layer[0].pdivy)+my-16)/16) *
               layer[0].sizex)+(((xwin*layer[0].pmultx/layer[0].pdivx)+mx-16)
               /16)]=0;
  }
  if (mb==1 && el==8)
  {
     WaitRelease();
     ProcessEntity((xwin+(mx-16))/16,(ywin+(my-16))/16);
     modified=1;
  }
  if (el==8 && EntityThere((mx-16+xwin)/16,(my+ywin-16)/16) &&
      key[SCAN_DEL])
  {
    WaitRelease();
    DeleteEntity((mx-16+xwin)/16,(my-16+ywin)/16);
    modified=1;
  }
}
