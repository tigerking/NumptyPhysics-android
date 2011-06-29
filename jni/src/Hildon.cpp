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

#ifdef USE_HILDON
#include <stdio.h>
#include <string.h>
#include <glib-object.h>
#include <glibconfig.h>
#include <glib/gmacros.h>
#include <libosso.h>
#include <ossoemailinterface.h>

#include "Hildon.h"
#include "Config.h"
#include "happyhttp.h"
using namespace happyhttp;

#define NP_NAME       "NumptyPhysics"
#define NP_SERVICE    "org.maemo.garage.numptyphysics"
#define NP_OBJECT     "org/maemo/garage/numptyphysics" /* / ?? */
#define NP_INTERFACE  "org.maemo.garage.numptyphysics"
#define NP_VERSION    "1.0"
#define MAX_FILES 32


static struct HildonState {
  GMainContext   *gcontext;
  osso_context_t *osso;
  int   numFiles;
  char* files[MAX_FILES];
  FILE *httpFile;
  int   httpSize;
} g_state = {NULL,0};


static void http_begin_cb( const Response* r, void* userdata )
{
  switch ( r->getstatus() ) {
  case OK:
    g_state.httpSize = 0;
    break;
  default:
    fprintf(stderr,"http status=%d %s\n",r->getstatus(),r->getreason());
    g_state.httpSize = -1;
    break;
  }
}

static void http_data_cb( const Response* r, void* userdata,
		   const unsigned char* data, int numbytes )
{
  fwrite( data, 1, numbytes, g_state.httpFile );
  g_state.httpSize += numbytes;
}

static bool http_get( const char* uri,
		      const char* file )
{
  char* host = strdup(uri);
  char* e = strchr(host,'/');
  int port = 80;

  g_state.httpFile = fopen( HTTP_TEMP_FILE, "wt" );
  g_state.httpSize = -1;

  if ( e ) {
    *e = '\0';
  }
  e = strchr(host,':');
  if ( e ) {
    *e = '\0';
    port=atoi(e+1);
  }
  e = strchr(uri,'/');
  if ( e ) {
    try {
      fprintf(stderr,"http_get host=%s port=%d file=%s\n",host,port,e);
      Connection con( host, port );
      con.setcallbacks( http_begin_cb, http_data_cb, NULL, NULL );
      con.request("GET",e,NULL,NULL,0);
      while ( con.outstanding() ) {
	fprintf(stderr,"http_get pump\n");
	con.pump();
      }
    } catch ( Wobbly w ) {
      fprintf(stderr,"http_get wobbly: %s\n",w.what());
    }
  }

  fclose ( g_state.httpFile );
  free( host );
  return g_state.httpSize > 0;
}


static gint dbus_handler(const gchar *interface,
                         const gchar *method,
                         GArray *arguments,
                         gpointer data,
                         osso_rpc_t *retval)
{
  if (arguments == NULL) {
    return OSSO_OK;
  }

  if (g_ascii_strcasecmp(method, "mime_open") == 0) {
    for(unsigned i = 0; i < arguments->len; ++i) {
      osso_rpc_t val = g_array_index(arguments, osso_rpc_t, i);
      if (val.type == DBUS_TYPE_STRING && val.value.s != NULL) {
	char *f = NULL;
	fprintf(stderr,"hildon mime open \"%s\"\n",val.value.s);
	if ( strncmp(val.value.s,"file://",7)==0 
	     && g_state.numFiles < MAX_FILES ) {
	  f = val.value.s+7;
	} else if ( ( strncmp(val.value.s,"http://",7)==0 
		     || strncmp(val.value.s,"nptp://",7)==0 )
		    && http_get( val.value.s+7, HTTP_TEMP_FILE ) ) {
	  f = HTTP_TEMP_FILE;
	}
	if ( f ) {
	  if ( g_state.files[g_state.numFiles] ) {
	    g_free(g_state.files[g_state.numFiles]);
	  }
	  g_state.files[g_state.numFiles++] = g_strdup( f );
	}
      }
    }
  }

  return OSSO_OK;
}


Hildon::Hildon()
{
  if ( g_state.gcontext ) {
    throw "gmainloop already initialised";
  } else {
    g_type_init();
    g_state.gcontext = g_main_context_new();
  }
  if ( g_state.osso ) {
    throw "hildon already initialised";
  } else {
    g_state.osso = osso_initialize(NP_NAME, NP_VERSION, FALSE, g_state.gcontext);
    if (g_state.osso == NULL) {
      fprintf(stderr, "Failed to initialize libosso\n");
      return;
    }
    
    /* Set dbus handler to get mime open callbacks */
    if ( osso_rpc_set_cb_f(g_state.osso,
			   NP_SERVICE,
			   NP_OBJECT,
			   NP_INTERFACE,
			   dbus_handler, NULL) != OSSO_OK) {
      fprintf(stderr, "Failed to set mime callback\n");
      return;
    }
  }
}

Hildon::~Hildon()
{
  if ( g_state.osso ) {
    osso_deinitialize( g_state.osso );
  }
  if ( g_state.gcontext ) {
    g_main_context_unref( g_state.gcontext );
  }
}


void Hildon::poll()
{
  if ( g_main_context_iteration( g_state.gcontext, FALSE ) ) {
    //fprintf(stderr, "Hildon::poll event!\n");
  }
}

char *Hildon::getFile()
{
  if ( g_state.numFiles > 0 ) {
    return g_state.files[--g_state.numFiles];
  }
  return NULL;
}

bool Hildon::sendFile( char* to, char *file )
{
  GSList *l = g_slist_append( NULL, (gpointer)file );
  if ( l ) {
    if ( osso_email_files_email( g_state.osso, l ) == OSSO_OK ) {
      return true;
    }
  }
  return false;
}



#endif //USE_HILDON
