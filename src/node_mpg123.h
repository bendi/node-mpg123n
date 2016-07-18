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
#include "equalizerr.h"
#include "control_generic.h"

using namespace v8;


namespace mpg123n {

enum COMMAND {
  COMMAND_EMPTY=-1,
  COMMAND_STOP_AND_PLAY,
  COMMAND_STOP,
  COMMAND_PLAY,
  COMMAND_PAUSE,
  COMMAND_JUMP
};

struct control_generic_loop_data {
  uv_work_t req;
  mpg123_handle *mh;
  struct timeval tv;
  audio_output_t *ao;
  fd_set fds;
  int mode;
  COMMAND command;
  char* arg;

  char silent;

  Nan::Persistent<Function> callback;
};

void node_mpg123_loop_async (uv_work_t *req);
void node_mpg123_loop_after (uv_work_t *req);
void player_loop(control_generic_loop_data *data);

NAN_METHOD(Init);
NAN_METHOD(Exit);

class Mpg123n: public Nan::ObjectWrap {
public:
	static NAN_MODULE_INIT(Init);
private:
	explicit Mpg123n();
	~Mpg123n();

	static NAN_METHOD(New);
	static NAN_METHOD(Play);
	static NAN_METHOD(Stop);
	static NAN_METHOD(Pause);
	static NAN_METHOD(Jump);
	static NAN_METHOD(Volume);
	static Nan::Persistent<v8::Function> constructor;
	control_generic_loop_data *data;
};

} // mpg123n namespace
