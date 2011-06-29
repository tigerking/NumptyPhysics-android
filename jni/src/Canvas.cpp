/*
 * This file is part of NumptyPhysics
 * Copyright (C) 2008 Tim Edmonds
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 */

#include <string>
#include "Common.h"
#include "Config.h"
#include "Canvas.h"
#include "Path.h"

#ifndef TIGERKING_FIX
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#define Window X11Window //oops
#include <SDL/SDL_syswm.h>
#ifndef WIN32
#include <X11/X.h>
#include <X11/Xlib.h>
#endif
#undef Window

#else
#include <SDL.h>
#include <SDL_image.h>
#include "ndk_log.h"
#endif



// extract RGB colour components as 8bit values from RGB888
#define R32(p) (((p)>>16)&0xff)
#define G32(p) (((p)>>8)&0xff)
#define B32(p) ((p)&0xff)

// extract RGB colour components as 8bit values from RGB565
#define R16(p) (((p)>>8)&0xf8)
#define G16(p) (((p)>>3)&0xfc)
#define B16(p) (((p)<<3)&0xf8)

#define R16G16B16_TO_RGB888(r,g,b) \
  ((((r)<<8)&0xff0000) | ((g)&0x00ff00) | (((b)>>8)))

#define R16G16B16_TO_RGB565(r,g,b) \
  ((Uint16)( (((r)&0xf800) | (((g)>>5)&0x07e0) | (((b)>>11))&0x001f) ))

#define RGB888_TO_RGB565(p) \
  ((Uint16)( (((p)>>8)&0xf800) | (((p)>>5)&0x07e0) | (((p)>>3)&0x001f) ))


void ExtractRgb( uint32 c, int& r, int &g, int &b ) 
{
  r = R32(c); g = G32(c); b = B32(c);
}

void ExtractRgb( uint16 c, int& r, int &g, int &b )
{
  r = R16(c); g = G16(c); b = B16(c);
}


template <typename PIX> 
inline void AlphaBlend( PIX& p, int cr, int cg, int cb, int a, int ia )
{
  throw "not implemented";
}

inline void AlphaBlend( Uint16& p, int cr, int cg, int cb, int a, int ia )
{ //565
  p = R16G16B16_TO_RGB565( a * cr + ia * R16(p),
			   a * cg + ia * G16(p),
			   a * cb + ia * B16(p) );
}

inline void AlphaBlend( Uint32& p, int cr, int cg, int cb, int a, int ia )

{ //888
  p = R16G16B16_TO_RGB888( a * cr + ia * R32(p),
			   a * cg + ia * G32(p),
			   a * cb + ia * B32(p) );
}

#define ALPHA_MAX 0xff

template <typename PIX, unsigned W>
struct AlphaBrush
{
  int m_r, m_g, m_b, m_c;
  inline AlphaBrush( PIX c )
  {
    m_c = c;
    ExtractRgb( c, m_r, m_g, m_b );
  }
  inline void ink( PIX* pix, int step, int a ) 
  {
    int ia = ALPHA_MAX - a;
    int o=-W/2+1;
    AlphaBlend( *(pix+o*step), m_r, m_g, m_b, a, ia );
    for ( ; o<=W/2; o++ ) {
      *(pix+o*step) = m_c;
    } 
    AlphaBlend( *(pix+o*step), m_r, m_g, m_b, ia, a );
  }
};

template <typename PIX>
struct AlphaBrush<PIX,1>
{
  int m_r, m_g, m_b, m_c;
  inline AlphaBrush( PIX c )
  {
    m_c = c;
    ExtractRgb( c, m_r, m_g, m_b );
  }
  inline void ink( PIX* pix, int step, int a ) 
  {
    int ia = ALPHA_MAX - a;
    AlphaBlend( *(pix-step), m_r, m_g, m_b, a, ia );
    AlphaBlend( *(pix), m_r, m_g, m_b, ia, a );
  }
};

template <typename PIX>
struct AlphaBrush<PIX,3>
{
  int m_r, m_g, m_b, m_c;
  inline AlphaBrush( PIX c )
  {
    m_c = c;
    ExtractRgb( c, m_r, m_g, m_b );
  }
  inline void ink( PIX* pix, int step, int a ) 
  {
    int ia = ALPHA_MAX - a;
    AlphaBlend( *(pix-step), m_r, m_g, m_b, a, ia );
    *(pix) = m_c;
    AlphaBlend( *(pix+step), m_r, m_g, m_b, ia, a );
  }
};



template <typename PIX, unsigned THICK> 
void renderLine( void *buf,
		 int byteStride,
		 int x1, int y1, int x2, int y2,
		 PIX color )
{  
  PIX *pix = (PIX*)((char*)buf+byteStride*y1) + x1;
  int lg_delta, sh_delta, cycle, lg_step, sh_step;
  int alpha, alpha_step, alpha_reset;
  int pixStride = byteStride/sizeof(PIX);
  AlphaBrush<PIX,THICK> brush( color );

  lg_delta = x2 - x1;
  sh_delta = y2 - y1;
  lg_step = SGN(lg_delta);
  lg_delta = ABS(lg_delta);
  sh_step = SGN(sh_delta);
  sh_delta = ABS(sh_delta);
  if ( sh_step < 0 )  pixStride = -pixStride;

  // in theory should be able to do this with just a single step
  // variable - ie: combine cycle and alpha as in wu algorithm
  if (sh_delta < lg_delta) {
    cycle = lg_delta >> 1;
    alpha = ALPHA_MAX >> 1;
    alpha_step = -(ALPHA_MAX * sh_delta/(lg_delta+1));
    alpha_reset = alpha_step < 0 ? ALPHA_MAX : 0;
    int count = lg_step>0 ? x2-x1 : x1-x2;
    while ( count-- ) {
      brush.ink( pix, pixStride, alpha );
      cycle += sh_delta;
      alpha += alpha_step;
      pix += lg_step;
      if (cycle > lg_delta) {
	cycle -= lg_delta;
	alpha = alpha_reset;
	pix += pixStride;
      }
    }
  } else {
    cycle = sh_delta >> 1;
    alpha = ALPHA_MAX >> 1;
    alpha_step = -lg_step * ABS(ALPHA_MAX * lg_delta/(sh_delta+1));
    alpha_reset = alpha_step < 0 ? ALPHA_MAX : 0;
    int count = sh_step>0 ? y2-y1 : y1-y2;
    while ( count-- ) {
      brush.ink( pix, 1, alpha );
      cycle += lg_delta;
      alpha += alpha_step;
      pix += pixStride;
      if (cycle > sh_delta) {
	cycle -= sh_delta;
	alpha = alpha_reset;
	pix += lg_step;
      }
    }
  }
}


#define SURFACE(cANVASpTR) ((SDL_Surface*)((cANVASpTR)->m_state))

Canvas::Canvas( int w, int h )
  : m_state(NULL),
    m_bgColour(0),
    m_bgImage(NULL)
{
  m_state = SDL_CreateRGBSurface( SDL_SWSURFACE, w, h, 
				  SDL_GetVideoInfo()->vfmt->BitsPerPixel,
				  SDL_GetVideoInfo()->vfmt->Rmask,
				  SDL_GetVideoInfo()->vfmt->Gmask,
				  SDL_GetVideoInfo()->vfmt->Bmask,
				  SDL_GetVideoInfo()->vfmt->Amask );
  resetClip();
}


Canvas::Canvas( State state )
  : m_state(state),
    m_bgColour(0),
    m_bgImage(NULL)
{
  resetClip();
}

Canvas::~Canvas()
{
}

int Canvas::width() const
{
  return SURFACE(this)->w;
}

int Canvas::height() const
{
  return SURFACE(this)->h;
}

int Canvas::makeColour( int r, int g, int b ) const
{
  return SDL_MapRGB( SURFACE(this)->format, r, g, b );
}

int Canvas::makeColour( int c ) const
{
  return SDL_MapRGB( SURFACE(this)->format,
		     (c>>16)&0xff, (c>>8)&0xff, (c>>0)&0xff );
}

void Canvas::resetClip()
{
  if ( m_state ) {
    setClip( 0, 0, width(), height() );
  } else {
    setClip( 0, 0, 0, 0 );
  }
}

void Canvas::setClip( int x, int y, int w, int h )
{
  m_clip = Rect(x,y,x+w-1,y+h-1);
}

void Canvas::setBackground( int c )
{
  m_bgColour = c;
}

void Canvas::setBackground( Canvas* bg )
{
  m_bgImage = bg;
}

void Canvas::clear()
{
  if ( m_bgImage ) {
    SDL_BlitSurface( SURFACE(m_bgImage), NULL, SURFACE(this), NULL );
  } else {
    SDL_FillRect( SURFACE(this), NULL, m_bgColour );
  }
}

void Canvas::fade() 
{
  Uint32 bpp;
  int count = width()*height();
  bpp = SURFACE(this)->format->BytesPerPixel;
  char* row = (char*)SURFACE(this)->pixels;

  SDL_LockSurface(SURFACE(this));
  switch ( bpp ) {
  case 2: 
    for ( int i=0;i<count;i++) {
      ((Uint16*)row)[i] = (((Uint16*)row)[i]>>1) & 0x7bef;
    }
    break;
  case 4:
    for ( int i=0;i<count;i++) {
      ((Uint32*)row)[i] = (((Uint32*)row)[i]>>1) & 0x7f7f7f;
    }
    break;
  }
  SDL_UnlockSurface(SURFACE(this));
}

Canvas* Canvas::scale( int factor ) const
{
  Canvas *c = new Canvas( width()/factor, height()/factor );  
  if ( c ) {
    if ( factor==4 && SURFACE(this)->format->BytesPerPixel==2 ) {
      const uint16 MASK2LSB = 0xe79c;
      int dpitch = SURFACE(c)->pitch / sizeof(uint16_t);
      int spitch = SURFACE(this)->pitch / sizeof(uint16_t);
      uint16_t *drow = (uint16_t*)SURFACE(c)->pixels;
      for ( int y=0;y<c->height();y++ ) {
	for ( int x=0;x<c->width();x++ ) {
          uint16 p = 0;
	  uint16_t *srow = (uint16_t*)SURFACE(this)->pixels
  	                    + (y*spitch+x)*factor;
	  for ( int yy=0;yy<factor;yy++ ) {
            uint16 q = 0;
	    for ( int xx=0;xx<factor;xx++ ) {
	      q += (srow[xx]&MASK2LSB)>>2;
	    }
            p += (q&MASK2LSB)>>2;
            srow += spitch;
	  }
          drow[x] = p;
	}
	drow += dpitch;
      }
    } else {
      for ( int y=0;y<c->height();y++ ) {
	for ( int x=0;x<c->width();x++ ) {
	  int r=0,g=0,b=0;
	  Uint8 rr,gg,bb;
	  for ( int yy=0;yy<factor;yy++ ) {
	    for ( int xx=0;xx<factor;xx++ ) {
	      SDL_GetRGB( readPixel( x*factor+xx, y*factor+yy ),
			  SURFACE(this)->format, &rr,&gg,&bb );
	      r += rr;
	      g += gg;
	      b += bb;
	    }
	  }
	  int div = factor*factor;
	  c->drawPixel( x, y, makeColour(r/div,g/div,b/div) );
	}
      }
    }
  }
  return c;
}


void Canvas::clear( const Rect& r )
{
  if ( m_bgImage ) {
    SDL_Rect srcRect = { r.tl.x, r.tl.y, r.br.x-r.tl.x+1, r.br.y-r.tl.y+1 };
    SDL_BlitSurface( SURFACE(m_bgImage), &srcRect, SURFACE(this), &srcRect );
  } else {
    drawRect( r, m_bgColour );
  }
}

void Canvas::drawImage( Canvas *canvas, int x, int y )
{
  SDL_Rect dest = { x, y, 0, 0 };
  SDL_BlitSurface( SURFACE(canvas), NULL, SURFACE(this), &dest );
}

void Canvas::drawPixel( int x, int y, int c )
{
  Uint32 bpp, ofs;

  bpp = SURFACE(this)->format->BytesPerPixel;
  ofs = SURFACE(this)->pitch*y;
  char* row = (char*)SURFACE(this)->pixels + ofs;

  SDL_LockSurface(SURFACE(this));
  switch ( bpp ) {
  case 2: ((Uint16*)row)[x] = c; break;
  case 4: ((Uint32*)row)[x] = c; break;
  }
  SDL_UnlockSurface(SURFACE(this));
}

int Canvas::readPixel( int x, int y ) const
{
  Uint32 bpp, ofs;
  int c;

  bpp = SURFACE(this)->format->BytesPerPixel;
  ofs = SURFACE(this)->pitch*y;
  char* row = (char*)SURFACE(this)->pixels + ofs;

  SDL_LockSurface(SURFACE(this));
  switch ( bpp ) {
  case 2: c = ((Uint16*)row)[x]; break;
  case 4: c = ((Uint32*)row)[x]; break;
  default: c=0; break;
  }
  SDL_UnlockSurface(SURFACE(this));
  return c;
}

void Canvas::drawLine( int x1, int y1, int x2, int y2, int color )
{  
  int lg_delta, sh_delta, cycle, lg_step, sh_step;
  lg_delta = x2 - x1;
  sh_delta = y2 - y1;
  lg_step = SGN(lg_delta);
  lg_delta = ABS(lg_delta);
  sh_step = SGN(sh_delta);
  sh_delta = ABS(sh_delta);
  if (sh_delta < lg_delta) {
    cycle = lg_delta >> 1;
    while (x1 != x2) {
      drawPixel( x1, y1, color);
      cycle += sh_delta;
      if (cycle > lg_delta) {
	cycle -= lg_delta;
	y1 += sh_step;
      }
      x1 += lg_step;
    }
    drawPixel( x1, y1, color);
  }
  cycle = sh_delta >> 1;
  while (y1 != y2) {
    drawPixel( x1, y1, color);
    cycle += lg_delta;
    if (cycle > sh_delta) {
      cycle -= sh_delta;
      x1 += lg_step;
    }
    y1 += sh_step;
  }
  drawPixel( x1, y1, color);
}

void Canvas::drawPath( const Path& path, int color, bool thick )
{
  // allow for thick lines in clipping
  Rect clip = m_clip;
  clip.tl.x++; clip.tl.y++;
  clip.br.x--; clip.br.y--;

  int i=0;
  const int n = path.numPoints();
 
  for ( ; i<n && !clip.contains( path.point(i) ); i++ ) {
    //skip clipped start pt
  }
  i++;
  SDL_LockSurface(SURFACE(this));
  for ( ; i<n; i++ ) {
    // pt i-1 is guranteed to be inside clipping    
    const Vec2& p2 = path.point(i);
    if ( clip.contains( p2 ) ) {
      const Vec2& p1 = path.point(i-1);
      switch ( SURFACE(this)->format->BytesPerPixel ) {
      case 2:      
	renderLine<Uint16,3>( SURFACE(this)->pixels,
			      SURFACE(this)->pitch,
			      p1.x, p1.y, p2.x, p2.y, color );
	break;
      case 4:
	renderLine<Uint32,3>( SURFACE(this)->pixels,
			      SURFACE(this)->pitch,
			      p1.x, p1.y, p2.x, p2.y, color );
	break;
      }
    } else {
      for ( ; i<n && !clip.contains( path.point(i) ); i++ ) {
	//skip until we find a unclipped pt - this will be p1 next
	//time around
      }
    }
  }
  SDL_UnlockSurface(SURFACE(this));
  
}

void Canvas::drawRect( int x, int y, int w, int h, int c, bool fill )
{
  if ( fill ) {
    SDL_Rect r = { x, y, w, h };
    SDL_FillRect( SURFACE(this), &r, c );
  } else {
    SDL_Rect f = { x, y, w, h };
    SDL_Rect r;
    r=f; r.h=1; SDL_FillRect( SURFACE(this), &r, c );
    r.y+=f.h-1; SDL_FillRect( SURFACE(this), &r, c );
    r=f; r.w=1; SDL_FillRect( SURFACE(this), &r, c );
    r.x+=f.w-1; SDL_FillRect( SURFACE(this), &r, c );
  }
}

void Canvas::drawRect( const Rect& r, int c, bool fill )
{
  drawRect( r.tl.x, r.tl.y, r.br.x-r.tl.x, r.br.y-r.tl.y, c, fill );
}

void Canvas::drawWorldLine( b2Vec2 pos1, b2Vec2 pos2, int color, bool thick )
{
  if ( pos1.y < -5.0 || pos1.y >= 45.0
       || pos2.y < -5.0 || pos2.y >= 45.0 
       || pos1.x <= -40.0 || pos1.x >= 40.0
       || pos2.x <= -40.0 || pos2.x >= 40.0 ) {
    //printf("drop %f,%f-%f,%f\n",pos1.x,pos1.y,pos2.x,pos2.y);
    return; 
  } 

  if ( thick ) {
    b2Vec2 dir = (pos2 - pos1);
    dir.Normalize();
    b2Vec2 norm( -dir.y, dir.x );
    norm = 1/PIXELS_PER_METREf * norm;
    
    drawLine( (int)(pos1.x*PIXELS_PER_METREf+CANVAS_WIDTHf/2.0),
	      (int)(CANVAS_HEIGHTf-CANVAS_GROUNDf-pos1.y*PIXELS_PER_METREf),
	      (int)(pos2.x*PIXELS_PER_METREf+CANVAS_WIDTHf/2.0),
	      (int)(CANVAS_HEIGHTf-CANVAS_GROUNDf-pos2.y*PIXELS_PER_METREf),
	      color );	
    pos1 += norm; pos2 += norm;
    drawLine( (int)(pos1.x*PIXELS_PER_METREf+CANVAS_WIDTHf/2.0),
	      (int)(CANVAS_HEIGHTf-CANVAS_GROUNDf-pos1.y*PIXELS_PER_METREf),
	      (int)(pos2.x*PIXELS_PER_METREf+CANVAS_WIDTHf/2.0),
	      (int)(CANVAS_HEIGHTf-CANVAS_GROUNDf-pos2.y*PIXELS_PER_METREf),
	      color );	
    pos1 -= norm; pos2 -= norm;
    pos1 -= norm; pos2 -= norm;
    drawLine( (int)(pos1.x*PIXELS_PER_METREf+CANVAS_WIDTHf/2.0),
	      (int)(CANVAS_HEIGHTf-CANVAS_GROUNDf-pos1.y*PIXELS_PER_METREf),
	      (int)(pos2.x*PIXELS_PER_METREf+CANVAS_WIDTHf/2.0),
	      (int)(CANVAS_HEIGHTf-CANVAS_GROUNDf-pos2.y*PIXELS_PER_METREf),
	      color );	
  } else {
    drawLine( (int)(pos1.x*PIXELS_PER_METREf+CANVAS_WIDTHf/2.0),
	      (int)(CANVAS_HEIGHTf-CANVAS_GROUNDf-pos1.y*PIXELS_PER_METREf),
	      (int)(pos2.x*PIXELS_PER_METREf+CANVAS_WIDTHf/2.0),
	      (int)(CANVAS_HEIGHTf-CANVAS_GROUNDf-pos2.y*PIXELS_PER_METREf),
	      color );	
  }

}


void Canvas::drawWorldPath( const Path& path, int color, bool thick )
{
  for ( int i=1; i<path.numPoints(); i++ ) {
    drawWorldLine( path.point(i-1), path.point(i), color, thick );
  }  
}



Window::Window( int w, int h, const char* title, const char* winclass )
{
  LOG_ENTER();
  
  if ( winclass ) {
    char s[80];
    snprintf(s,80,"SDL_VIDEO_X11_WMCLASS=%s",winclass);
    putenv(s);
  }

#ifndef TIGERKING_FIX  
  if ( title ) {
    SDL_WM_SetCaption( title, title );
  }
#endif

#ifdef USE_HILDON
  LOGD("USE_HILDON enabled");
  m_state = SDL_SetVideoMode( w, h, 16, SDL_SWSURFACE);
  SDL_WM_ToggleFullScreen( SURFACE(this) );
  SDL_ShowCursor( SDL_DISABLE );
#else
  LOGD("SDL_SetVideoMode( %d,%d , 0, SDL_SWSURFACE)", w,h);
  m_state = SDL_SetVideoMode( w, h, 0, SDL_SWSURFACE);
  LOGD("SDL_SetVideoMode, return %d", m_state);
#endif
  if ( SURFACE(this) == NULL ) {
    //throw "Unable to set 800x480 video";
    LOGE("Unable to set 800x480 video");
    exit(-200);
  }
  resetClip();

#ifdef USE_HILDON
  SDL_SysWMinfo sys;
  SDL_VERSION( &sys.version );
  SDL_GetWMInfo( &sys );
  printf("X11 window =%08x\n",sys.info.x11.window);
  printf("X11 fswindow =%08x\n",sys.info.x11.fswindow);
  printf("X11 wmwindow =%08x\n",sys.info.x11.wmwindow);

  uint32_t pid = getpid();
  XChangeProperty( sys.info.x11.display,
		   sys.info.x11.wmwindow,
		   XInternAtom (sys.info.x11.display, "_NET_WM_PID", False),
		   XA_CARDINAL, 32, PropModeReplace,
		   (unsigned char*)&pid, 1 );
#endif

  LOG_LEAVE();
}


void Window::update( const Rect& r )
{
  if ( r.tl.x < width() && r.tl.y < height() ) {
    int x1 = MAX( 0, r.tl.x );
    int y1 = MAX( 0, r.tl.y );
    int x2 = MIN( width()-1, r.br.x );
    int y2 = MIN( height()-1, r.br.y );
    int w  = MAX( 0, x2-x1+1 );
    int h  = MAX( 0, y2-y1+1 );
    if ( w > 0 && h > 0 ) {
      SDL_UpdateRect( SURFACE(this), x1, y1, w, h );
    }
  }
}

void Window::raise()
{
#ifndef TIGERKING_FIX

  SDL_SysWMinfo sys;
  SDL_VERSION( &sys.version );
  SDL_GetWMInfo( &sys );

#ifndef WIN32
  // take focus...
  XEvent ev = { 0 };
  ev.xclient.type         = ClientMessage;
  ev.xclient.window       = sys.info.x11.wmwindow;
  ev.xclient.message_type = XInternAtom (sys.info.x11.display,
					 "_NET_ACTIVE_WINDOW", False);
  ev.xclient.format       = 32;
  //all xewv.xclient.data==0 -> older spec?
  
  XSendEvent (sys.info.x11.display,
	      DefaultRootWindow(sys.info.x11.display),
	      False,
	      NoEventMask, //SubstructureRedirectMask,
	      &ev);    
  XSync( sys.info.x11.display, False );
#endif
  //XRaiseWindow( sys.info.x11.display, sys.info.x11.window );

#endif 
}


void Window::setSubName( const char *sub )
{
#ifdef USE_HILDON
  SDL_SysWMinfo sys;
  SDL_VERSION( &sys.version );
  SDL_GetWMInfo( &sys );

  XChangeProperty( sys.info.x11.display,
		   sys.info.x11.wmwindow,
		   XInternAtom (sys.info.x11.display, "_MB_WIN_SUB_NAME", False),
		   XA_STRING, 8, PropModeReplace,
		   (unsigned char*)sub, strlen(sub) );
#endif
}


Image::Image( const char* file, bool alpha )
{
  alpha = false;
  
#ifndef TIGERKING_FIX  
  std::string f( DEFAULT_RESOURCE_PATH "/" );
#else
   char path[256];

   std::string f( getResoucePath(path) );
   

#endif
  LOGD("IMG_Load(%s,%s)...", getResoucePath(path),  file);
  
  SDL_Surface* img = IMG_Load((f+file).c_str());
  if ( img ) {
    LOGD("loaded image %s\n",(f+file).c_str());
    if ( alpha ) {
      m_state = SDL_DisplayFormatAlpha( img );
    } else {
      m_state = SDL_DisplayFormat( img );
//       SDL_SetColorKey( SURFACE(this),
// 		       SDL_SRCCOLORKEY|SDL_RLEACCEL,
// 		       makeColour( 0x00ff00ff ) );
    }
    if ( m_state ) {
      SDL_FreeSurface( img );
    } else {
      LOGW("warning image %s not converted to display format\n",(f+file).c_str());
      m_state = img;
    }
  } else {
    //throw "image not found";
    LOGE("Image::Image, image not found");
    exit(-200);
  }
  resetClip();
}



int Canvas::writeBMP( const char* filename ) const
{
  typedef struct {
    unsigned short int type;                 /* Magic identifier            */
    unsigned int size;                       /* File size in bytes          */
    unsigned short int reserved1, reserved2;
    unsigned int offset;                     /* Offset to image data, bytes */
  } BMPHEADER;
  
  typedef struct {
    unsigned int size;               /* Header size in bytes      */
    int width,height;                /* Width and height of image */
    unsigned short int planes;       /* Number of colour planes   */
    unsigned short int bits;         /* Bits per pixel            */
    unsigned int compression;        /* Compression type          */
    unsigned int imagesize;          /* Image size in bytes       */
    int xresolution,yresolution;     /* Pixels per meter          */
    unsigned int ncolours;           /* Number of colours         */
    unsigned int importantcolours;   /* Important colours         */
  } BMPINFOHEADER;
    
  int w = width();
  int h = height();
  BMPHEADER     head = { 'B'|('M'<<8), 14+40+w*h*3, 0, 0, 0 };
  BMPINFOHEADER info = { 40, w, h, 1, 24, 0, w*h*3, 100, 100, 0, 0 };
  
  FILE *f = fopen( filename, "wb" );
  if ( f ) {
    Uint32 bpp;
    bpp = SURFACE(this)->format->BytesPerPixel;

    fwrite( &head, 14, 1, f );
    fwrite( &info, 40, 1, f );
    for ( int y=h-1; y>=0; y-- ) {
      for ( int x=0; x<w; x++ ) {
	int p = readPixel( x, y );
	if ( bpp==2 ) {
	  p = R16G16B16_TO_RGB888( R16(p), G16(p), B16(p) );
	}
	fwrite( &p, 3, 1, f );
      }
    }
    fclose(f);
    return 1;
  }
  return 0;
}
