/*
 * Copyright (c) 2013, Marek Będkowski <marek@bedkowski.pl>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <v8.h>
#include <node.h>
#include <node_buffer.h>
#include "node_pointer.h"
#include "node_mpg123.h"
#include "debug.h"

using namespace v8;
using namespace node;

namespace mpg123n {

struct httpdata htd;
audio_output_t *ao = NULL;

#define UNWRAP_LOOP_DATA \
  HandleScope scope; \
  control_generic_loop_data *loop_data = reinterpret_cast<control_generic_loop_data *>(UnwrapPointer(args[0]));

Handle<Value> node_mpg123_init (const Arguments& args) {
  HandleScope scope;
  return scope.Close(Integer::New(mpg123_init()));
}


Handle<Value> node_mpg123_exit (const Arguments& args) {
  HandleScope scope;
  mpg123_exit();
  return Undefined();
}


Handle<Value> node_mpg123_new (const Arguments& args) {
  HandleScope scope;

  // TODO: Accept an input decoder String
  int error = MPG123_OK;
  mpg123_handle *mh = mpg123_new(NULL, &error);

  Local<Value> rtn;
  if (error == MPG123_OK) {
    httpdata_init(&htd);
    
    audio_capabilities(ao, mh);
  	
    load_equalizer(mh);
    
    mpg123_control_init(mh);
    
    control_generic_loop_data *data = new control_generic_loop_data;
    data->mh = mh;
    data->ao = ao;
    data->req.data = data;
    data->silent = 0;
    data->mode = MODE_STOPPED;
    data->command = COMMAND_EMPTY;
    
    rtn = Local<Value>::New(WrapPointer(data));
  } else {
    rtn = Local<Value>::New(Integer::New(error));
  }
  return scope.Close(rtn);
}

void player_loop(control_generic_loop_data *data)
{
  uv_queue_work(uv_default_loop(), &data->req,
    node_mpg123_loop_async,
    (uv_after_work_cb)node_mpg123_loop_after);
}

void node_mpg123_loop_async (uv_work_t *req) {
	control_generic_loop_data *r = (control_generic_loop_data *)req->data;

	if (r->command != COMMAND_EMPTY) {
		int clean = 1;
		switch(r->command) {
			case COMMAND_STOP_AND_PLAY:
				clean = 0;
				r->command = COMMAND_PLAY;
			case COMMAND_STOP:
				mpg123_control_stop(r->mh);
				break;
			case COMMAND_PAUSE:
				mpg123_control_pause();
				break;
			case COMMAND_JUMP:
				//debug("Jumping %s", r->arg);
				mpg123_control_jump(r->mh, r->arg);
				break;
			case COMMAND_PLAY:
				//debug("13: Do real play %s", r->arg);
				mpg123_control_play(r->mh, r->arg);
				break;
		}
		if (clean) {
			if (r->arg != NULL) {
				delete r->arg;
				r->arg = NULL;
			}
			r->command = COMMAND_EMPTY;
		}
	}

	r->mode = mpg123_control_loop(r->mh, r->ao, r->tv, r->fds, r->silent);
}

void node_mpg123_loop_after (uv_work_t *req) {
	control_generic_loop_data *r = (control_generic_loop_data *)req->data;
	
	Handle<Value> argv[1] = { Integer::New(r->mode) };
	
	TryCatch try_catch;
	
	if (r->command != COMMAND_PLAY) {
		r->callback->Call(Context::GetCurrent()->Global(), 1, argv);
	}

	if (r->command != COMMAND_EMPTY || r->mode == MODE_PLAYING) {
		player_loop(r);
	} else if (r->mode == MODE_STOPPED) {
		r->callback.Dispose();
	}
	
	if (try_catch.HasCaught()) {
    	FatalException(try_catch);
  	}
}

char* stringArgToStr(const v8::Local<v8::Value> arg) { 
  v8::String::Utf8Value v8Str(arg); 
  char *cStr = (char*) malloc(strlen(*v8Str) + 1); 
  strcpy(cStr, *v8Str); 
  return cStr; 
}
   
Handle<Value> node_mpg123_play(const Arguments& args) {
	UNWRAP_LOOP_DATA;
	
	char *path = stringArgToStr(args[1]);
	Local<Function> callback = Local<Function>::Cast(args[2]);
	
	if (!path) {
		//debug("Path is %s - cannot open.", path);
		return scope.Close(Number::New(0));
	} else {
		//debug("OK... going to finally open %s.", path);
		loop_data->command = COMMAND_STOP_AND_PLAY;
		loop_data->arg = path;
		loop_data->callback = Persistent<Function>::New(callback);
		if (loop_data->mode != MODE_PLAYING) {
			player_loop(loop_data);
		}
		return scope.Close(Number::New(1));
	}
}

Handle<Value> node_mpg123_stop(const Arguments& args) {
	UNWRAP_LOOP_DATA;
	
	if (loop_data->mh != NULL && loop_data->mode != MODE_STOPPED) { 
		loop_data->command = COMMAND_STOP;
	}	
	
	return Undefined();
}

Handle<Value> node_mpg123_pause(const Arguments& args) {
	UNWRAP_LOOP_DATA;
	
	
	if (loop_data->mh != NULL && loop_data->mode != MODE_STOPPED) { 

		loop_data->command = COMMAND_PAUSE;
		
		if (loop_data->mode == MODE_PAUSED) {
			player_loop(loop_data);
		}
	}	
	
	return Undefined();
}

Handle<Value> node_mpg123_volume(const Arguments& args) {
	UNWRAP_LOOP_DATA;
	
	char *arg = stringArgToStr(args[1]);
	
	if (loop_data->mh != NULL) {
		mpg123_control_volume(loop_data->mh, arg);
	}
	
	delete arg;
		
	return Undefined();
}

Handle<Value> node_mpg123_jump(const Arguments& args) {
	UNWRAP_LOOP_DATA;
	
	char *arg = stringArgToStr(args[1]);
	
	if (loop_data->mh != NULL && loop_data->mode != MODE_STOPPED) { 
		loop_data->command = COMMAND_JUMP;
		loop_data->arg = arg;
	}

	return Undefined();
}

Handle<Value> node_mpg123_safe_buffer (const Arguments& args) {
  HandleScope scope;
  
  init_output(&ao);
  return scope.Close(Number::New(mpg123_safe_buffer()));
}
void InitMPG123(Handle<Object> target) {
  HandleScope scope;

#define CONST_INT(value) \
  target->Set(String::NewSymbol(#value), Integer::New(value), \
      static_cast<PropertyAttribute>(ReadOnly|DontDelete));

  // mpg123_errors
  CONST_INT(MPG123_DONE);  /**< Message: Track ended. Stop decoding. */
  CONST_INT(MPG123_NEW_FORMAT);  /**< Message: Output format will be different on next call. Note that some libmpg123 versions between 1.4.3 and 1.8.0 insist on you calling mpg123_getformat() after getting this message code. Newer verisons behave like advertised: You have the chance to call mpg123_getformat(), but you can also just continue decoding and get your data. */
  CONST_INT(MPG123_NEED_MORE);  /**< Message: For feed reader: "Feed me more!" (call mpg123_feed() or mpg123_decode() with some new input data). */
  CONST_INT(MPG123_ERR);      /**< Generic Error */
  CONST_INT(MPG123_OK);       /**< Success */
  CONST_INT(MPG123_BAD_OUTFORMAT);   /**< Unable to set up output format! */
  CONST_INT(MPG123_BAD_CHANNEL);    /**< Invalid channel number specified. */
  CONST_INT(MPG123_BAD_RATE);    /**< Invalid sample rate specified.  */
  CONST_INT(MPG123_ERR_16TO8TABLE);  /**< Unable to allocate memory for 16 to 8 converter table! */
  CONST_INT(MPG123_BAD_PARAM);    /**< Bad parameter id! */
  CONST_INT(MPG123_BAD_BUFFER);    /**< Bad buffer given -- invalid pointer or too small size. */
  CONST_INT(MPG123_OUT_OF_MEM);    /**< Out of memory -- some malloc() failed. */
  CONST_INT(MPG123_NOT_INITIALIZED);  /**< You didn't initialize the library! */
  CONST_INT(MPG123_BAD_DECODER);    /**< Invalid decoder choice. */
  CONST_INT(MPG123_BAD_HANDLE);    /**< Invalid mpg123 handle. */
  CONST_INT(MPG123_NO_BUFFERS);    /**< Unable to initialize frame buffers (out of memory?). */
  CONST_INT(MPG123_BAD_RVA);      /**< Invalid RVA mode. */
  CONST_INT(MPG123_NO_GAPLESS);    /**< This build doesn't support gapless decoding. */
  CONST_INT(MPG123_NO_SPACE);    /**< Not enough buffer space. */
  CONST_INT(MPG123_BAD_TYPES);    /**< Incompatible numeric data types. */
  CONST_INT(MPG123_BAD_BAND);    /**< Bad equalizer band. */
  CONST_INT(MPG123_ERR_NULL);    /**< Null pointer given where valid storage address needed. */
  CONST_INT(MPG123_ERR_READER);    /**< Error reading the stream. */
  CONST_INT(MPG123_NO_SEEK_FROM_END);/**< Cannot seek from end (end is not known). */
  CONST_INT(MPG123_BAD_WHENCE);    /**< Invalid 'whence' for seek function.*/
  CONST_INT(MPG123_NO_TIMEOUT);    /**< Build does not support stream timeouts. */
  CONST_INT(MPG123_BAD_FILE);    /**< File access error. */
  CONST_INT(MPG123_NO_SEEK);     /**< Seek not supported by stream. */
  CONST_INT(MPG123_NO_READER);    /**< No stream opened. */
  CONST_INT(MPG123_BAD_PARS);    /**< Bad parameter handle. */
  CONST_INT(MPG123_BAD_INDEX_PAR);  /**< Bad parameters to mpg123_index() and mpg123_set_index() */
  CONST_INT(MPG123_OUT_OF_SYNC);  /**< Lost track in bytestream and did not try to resync. */
  CONST_INT(MPG123_RESYNC_FAIL);  /**< Resync failed to find valid MPEG data. */
  CONST_INT(MPG123_NO_8BIT);  /**< No 8bit encoding possible. */
  CONST_INT(MPG123_BAD_ALIGN);  /**< Stack aligmnent error */
  CONST_INT(MPG123_NULL_BUFFER);  /**< NULL input buffer with non-zero size... */
  CONST_INT(MPG123_NO_RELSEEK);  /**< Relative seek not possible (screwed up file offset) */
  CONST_INT(MPG123_NULL_POINTER); /**< You gave a null pointer somewhere where you shouldn't have. */
  CONST_INT(MPG123_BAD_KEY);   /**< Bad key value given. */
  CONST_INT(MPG123_NO_INDEX);  /**< No frame index in this build. */
  CONST_INT(MPG123_INDEX_FAIL);  /**< Something with frame index went wrong. */
  CONST_INT(MPG123_BAD_DECODER_SETUP);  /**< Something prevents a proper decoder setup */
  CONST_INT(MPG123_MISSING_FEATURE);  /**< This feature has not been built into libmpg123. */
  CONST_INT(MPG123_BAD_VALUE); /**< A bad value has been given, somewhere. */
  CONST_INT(MPG123_LSEEK_FAILED); /**< Low-level seek failed. */
  CONST_INT(MPG123_BAD_CUSTOM_IO); /**< Custom I/O not prepared. */
  CONST_INT(MPG123_LFS_OVERFLOW); /**< Offset value overflow during translation of large file API calls -- your client program cannot handle that large file. */

  /* mpg123_enc_enum */
  CONST_INT(MPG123_ENC_8);
  CONST_INT(MPG123_ENC_16);
  CONST_INT(MPG123_ENC_24);
  CONST_INT(MPG123_ENC_32);
  CONST_INT(MPG123_ENC_SIGNED);
  CONST_INT(MPG123_ENC_FLOAT);
  CONST_INT(MPG123_ENC_SIGNED_16);
  CONST_INT(MPG123_ENC_UNSIGNED_16);
  CONST_INT(MPG123_ENC_UNSIGNED_8);
  CONST_INT(MPG123_ENC_SIGNED_8);
  CONST_INT(MPG123_ENC_ULAW_8);
  CONST_INT(MPG123_ENC_ALAW_8);
  CONST_INT(MPG123_ENC_SIGNED_32);
  CONST_INT(MPG123_ENC_UNSIGNED_32);
  CONST_INT(MPG123_ENC_SIGNED_24);
  CONST_INT(MPG123_ENC_UNSIGNED_24);
  CONST_INT(MPG123_ENC_FLOAT_32);
  CONST_INT(MPG123_ENC_FLOAT_64);
  CONST_INT(MPG123_ENC_ANY);

  /* mpg123_channelcount */
  CONST_INT(MPG123_MONO);
  CONST_INT(MPG123_STEREO);

  /* mpg123_channels */
  CONST_INT(MPG123_LEFT);
  CONST_INT(MPG123_RIGHT);
  CONST_INT(MPG123_LR);

  CONST_INT(MPG123_ID3);
  CONST_INT(MPG123_NEW_ID3);
  CONST_INT(MPG123_ICY);
  CONST_INT(MPG123_NEW_ICY);
  
  /* control_generic */
  CONST_INT(MODE_STOPPED);
  CONST_INT(MODE_PLAYING);
  CONST_INT(MODE_PAUSED);

  NODE_SET_METHOD(target, "mpg123_init", 		node_mpg123_init);
  NODE_SET_METHOD(target, "mpg123_exit", 		node_mpg123_exit);
  NODE_SET_METHOD(target, "mpg123_new", 		node_mpg123_new);  
  NODE_SET_METHOD(target, "mpg123_play", 		node_mpg123_play);  
  NODE_SET_METHOD(target, "mpg123_stop", 		node_mpg123_stop);  
  NODE_SET_METHOD(target, "mpg123_pause", 		node_mpg123_pause);  
  NODE_SET_METHOD(target, "mpg123_jump", 		node_mpg123_jump);
  NODE_SET_METHOD(target, "mpg123_volume", 		node_mpg123_volume);  
  NODE_SET_METHOD(target, "mpg123_safe_buffer", node_mpg123_safe_buffer);
}

} // nodelame namespace
