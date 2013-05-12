//
//  control_generic.h
//  mpg123proj
//
//  Created by Marek Będkowski on 28.02.2013.
//  Copyright (c) 2013 Marek Będkowski. All rights reserved.
//

#ifndef mpg123proj_control_generic_h
#define mpg123proj_control_generic_h

#include "mpg123app.h"
#include "mpg123.h"
#include "equalizerr.h"
#include "audio.h"
#include "xfermem.h"

#ifdef __cplusplus
extern "C" {
#endif
    
#define MODE_STOPPED 0
#define MODE_PLAYING 1
#define MODE_PAUSED 2
    
void mpg123_control_play(mpg123_handle *fr, char* path);
void mpg123_control_stop(mpg123_handle *fr);
void mpg123_control_jump(mpg123_handle *fr, char* arg);
void mpg123_control_volume(mpg123_handle *fr, char* arg);
void mpg123_control_pause();
void mpg123_control_init(mpg123_handle *fr);
int mpg123_control_loop(mpg123_handle *fr, audio_output_t *ao, struct timeval tv, fd_set fds, char silent);

#ifdef __cplusplus
}
#endif

#endif
