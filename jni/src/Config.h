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

#ifndef CONFIG_H
#define CONFIG_H

//#define DEBUG_ON_VENUS 1

#ifdef DEBUG_ON_VENUS
#define CANVAS_WIDTH  854
#define CANVAS_HEIGHT 480
#define CANVAS_GROUND 30
#define CANVAS_WIDTHf  854.0f
#define CANVAS_HEIGHTf 480.0f
#define CANVAS_GROUNDf 30.0f

#else
#define CANVAS_WIDTH  800
#define CANVAS_HEIGHT 480
#define CANVAS_GROUND 30
#define CANVAS_WIDTHf  800.0f
#define CANVAS_HEIGHTf 480.0f
#define CANVAS_GROUNDf 30.0f
#endif

#define PIXELS_PER_METREf 10.0f
#define CLOSED_SHAPE_THREHOLDf 0.4f
#define SIMPLIFY_THRESHOLDf 1.0f //PIXELs //(1.0/PIXELS_PER_METREf)
#define MULTI_VERTEX_LIMIT (b2_maxShapesPerBody)

#ifdef USE_HILDON //maemo
#  define ITERATION_RATE    60 //fps
#  define RENDER_RATE       20 //fps
#  define SOLVER_ITERATIONS 10
#  define JOINT_TOLERANCE   4.0f //PIXELs
#  define SELECT_TOLERANCE  8.0f //PIXELS_PER_METREf)
#  define CLICK_TOLERANCE   16 //PIXELs 
#else
#  define ITERATION_RATE    60 //fps
#  define RENDER_RATE       30 //fps
#  define SOLVER_ITERATIONS 10
#  define JOINT_TOLERANCE   4.0f //PIXELs
#  define SELECT_TOLERANCE  5.0f //PIXELS_PER_METREf)
#  define CLICK_TOLERANCE   4 //PIXELs 
#endif

#define ITERATION_TIMESTEPf  (1.0f / (float)ITERATION_RATE)
#define RENDER_INTERVAL (1000/RENDER_RATE)

#define HIDE_STEPS (RENDER_RATE*4)

#define COLOUR_RED     0xb80000
#define COLOUR_YELLOW  0xffd700 
#define COLOUR_BLUE    0x000077 
#define COLOUR_GREEN   0x108710
#define COLOUR_BLACK   0x101010
#define COLOUR_BROWN   0x703010

#ifndef INSTALL_BASE_PATH
#  define INSTALL_BASE_PATH "/sdcard/.ltiger/numptyphysics"
#endif
#define DEFAULT_LEVEL_PATH INSTALL_BASE_PATH
#define DEFAULT_RESOURCE_PATH DEFAULT_LEVEL_PATH
#ifndef USER_BASE_PATH
# ifdef USE_HILDON //maemo
#  define USER_BASE_PATH "MyDocs/.games/NumptyPhysics"
# else
#  define USER_BASE_PATH ".numptyphysics"
# endif
#endif
#ifndef TIGERKING_FIX
#define USER_LEVEL_PATH USER_BASE_PATH
#else
#define USER_LEVEL_PATH INSTALL_BASE_PATH
#endif

#define DEMO_TEMP_FILE "/tmp/demo.nph"
#define HTTP_TEMP_FILE "/tmp/http.nph"
#define SEND_TEMP_FILE "/tmp/mailto:numptyphysics@gmail.com.nph"

#define ICON_SCALE_FACTOR 4

class Config
{
 public:
  static int x;
};

extern char *getLevelPath(char *path);
extern char *getResoucePath(char *path);

#endif //CONFIG_H
