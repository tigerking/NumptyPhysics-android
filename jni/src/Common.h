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

#ifndef COMMON_H
#define COMMON_H

#include "Box2D.h"
#define ARRAY_SIZE(aRR) (sizeof(aRR)/sizeof((aRR)[0]))
#define ASSERT(a)

#define FIXED_SHIFT 16
#define FIXED_ONE (1<<FIXED_SHIFT)
#define FLOAT_TO_FIXED(fLOAT) ((int)(fLOAT*(float)FIXED_ONE))
#define FIXED_TO_INT(iNT) ((iNT)>>FIXED_SHIFT)

struct Vec2 {
  Vec2() {}
  Vec2( const Vec2& o ) : x(o.x), y(o.y) {}
  explicit Vec2( const b2Vec2& o ) : x((int)o.x), y((int)o.y) {}
  Vec2( int xx, int yy ) : x(xx), y(yy) {}
  void operator+=( const Vec2& o ) { x+=o.x; y+=o.y; }
  void operator-=( const Vec2& o ) { x-=o.x; y-=o.y; }
  Vec2 operator-() { return Vec2(-x,-y); }
  void operator*=( int o ) { x*=o; y*=o; }
  bool operator==( const Vec2& o ) const { return x==o.x && y==y; }
  operator b2Vec2() const { return b2Vec2((float)x,(float)y); } 
  Vec2 operator+( const Vec2& b ) const { return Vec2(x+b.x,y+b.y); }
  Vec2 operator-( const Vec2& b ) const { return Vec2(x-b.x,y-b.y); }
  Vec2 operator/( int r ) const { return Vec2(x/r,y/r); }
  int x,y;
};

template <typename T> inline T MIN( T a, T b )
{
  return a < b ? a : b;
}

inline Vec2 MIN( const Vec2& a, const Vec2& b )
{
  Vec2 r;
  r.x = MIN(a.x,b.x);
  r.y = MIN(a.y,b.y);
  return r;
}

template <typename T> inline T MAX( T a, T b )
{
  return a >= b ? a : b;
}

inline Vec2 MAX( const Vec2& a, const Vec2& b )
{
  Vec2 r;
  r.x = MAX(a.x,b.x);
  r.y = MAX(a.y,b.y);
  return r;
}

#define SGN(a) ((a)<0?-1:1)
#define ABS(a) ((a)<0?-(a):(a))



struct Rect {
  Rect() {}
  Rect( const Vec2& atl, const Vec2& abr ) : tl(atl), br(abr) {} 
  Rect( int x1, int y1, int x2, int y2 ) : tl(x1,y1), br(x2,y2) {}
  void clear() { tl.x=tl.y=br.x=br.y=0; }
  bool isEmpty() { return tl.x==0 && br.x==0; }
  void expand( const Vec2& v ) { tl=MIN(tl,v); br=MAX(br,v); }
  void expand( const Rect& r ) { expand(r.tl); expand(r.br); }
  bool contains( const Vec2& p ) const {
    return p.x >= tl.x && p.x <= br.x && p.y >= tl.y && p.y <= br.y;
  }
  bool contains( const Rect& p ) const {
    return contains(p.tl) && contains(p.br);
  }
  bool intersects( const Rect& r ) const {
    return r.tl.x <= br.x
      && r.tl.y <= br.y
      && r.br.x >= tl.x 
      && r.br.y >= tl.y;
  }
  Vec2 centroid() const { return (tl+br)/2; }
  Rect operator+( const Vec2& b ) const {
    Rect r=*this;
    r.tl += b; r.br += b;
    return r;
  }
  Vec2 tl, br;
};


#endif //COMMON_H
