#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define HMAX 500
#define VMAX 500

Display *d;
Window w;
GC gc;
int fg, bg;

char bitmap[2][HMAX][VMAX];
int hsize, vsize, psize, cbn;


void bgpixel_show(int i, int j)
{
  XSetForeground(d, gc, bg);
  XFillRectangle(d, w, gc, 
		 (i-1)*psize, (j-1)*psize, 
		 psize-1, psize-1);
}


void fgpixel_show(int i, int j)
{
  XSetForeground(d, gc, fg);
  XFillRectangle(d, w, gc, 
		 (i-1)*psize, (j-1)*psize, 
		 psize-1, psize-1);
}


void pixel(int c, int i, int j)
{
  if (c<2 || c>3)
    {
      bitmap[1-cbn][i][j] = 0;
      bgpixel_show(i,j);
      return;
    }
  if (c==3)
    {
      bitmap[1-cbn][i][j] = 1;
      fgpixel_show(i,j);
      return;
    }
  bitmap[1-cbn][i][j] = bitmap[cbn][i][j];
}


bitmap_init()
{
  int i,j;
  
  for (i=1; i<=hsize; i++)
    for(j=1; j<=vsize; j++)
      if (bitmap[cbn][i][j] = rand() %2)
	fgpixel_show(i,j);
      else
	bgpixel_show(i,j);

  for (i=0; i<=hsize+1; i++)
    {
      bitmap[cbn][i][0] = 0;
      bitmap[1-cbn][i][0] = 0;
      bitmap[cbn][i][vsize+1] = 0;
      bitmap[1-cbn][i][vsize+1] = 0;
    }
  for (j=0; j<=vsize+1; j++)
    {
      bitmap[cbn][0][j] = 0;
      bitmap[1-cbn][0][j] = 0;
      bitmap[cbn][hsize+1][j] = 0;
      bitmap[1-cbn][hsize+1][j] = 0;
    }
}


bitmap_next()
{
  int i,j;
  int c;

  for (i=1; i<=hsize; i++)
    for(j=1; j<=vsize; j++)
      {
	c = bitmap[cbn][i-1][j-1] + bitmap[cbn][i-1][j] 
	  + bitmap[cbn][i-1][j+1] + bitmap[cbn][i][j-1] 
	  + bitmap[cbn][i][j+1]   + bitmap[cbn][i+1][j-1] 
	  + bitmap[cbn][i+1][j]   + bitmap[cbn][i+1][j+1];
	pixel(c,i,j);
      }
  cbn = 1-cbn;
}


void event_loop()
{
  XEvent e;
  while(1) {
    XNextEvent(d, &e);
    switch (e.type) {
    case KeyPress:   
      while(1)
	bitmap_next();
    default:
      break;
    }
  }
}


void window_init()
{
  XGCValues gcv;

  if((d=XOpenDisplay(NULL))==NULL) 
    {
      exit(-1);
    }

  bg = WhitePixel(d,DefaultScreen(d));
  fg = BlackPixel(d,DefaultScreen(d));

  w = XCreateSimpleWindow(d, RootWindow(d,DefaultScreen(d)),
			    0, 0, psize * hsize, psize * hsize, 
			    2, fg, bg);

  gcv.function = GXcopy;
  gcv.foreground = bg;
  gcv.background = fg;
  gc =  XCreateGC(d, w,
                  GCFunction | GCForeground | GCBackground, 
		  &gcv);
    	     
  XMapWindow(d,w);
  XSelectInput(d,w, 
	       EnterWindowMask | LeaveWindowMask | ButtonPressMask |
	       OwnerGrabButtonMask | ExposureMask | StructureNotifyMask |
	       KeyPressMask);
}


void main()
{
  hsize = 200;
  vsize = 200;
  psize = 4;
  cbn   = 0;
  window_init();
  bitmap_init();
  event_loop();
}






