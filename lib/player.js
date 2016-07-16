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

var binding = require('./bindings');
var Mpg123n = binding.Mpg123n;
var inherits = require('util').inherits;
var EventEmitter = require('events').EventEmitter;
var debug = require('debug')('marakuja:player');

module.exports = Player;

/**
 * Some constants.
 */

var MPG123_OK = binding.MPG123_OK;
var MPG123_DONE = binding.MPG123_DONE;
var MPG123_NEW_ID3 = binding.MPG123_NEW_ID3;
var MPG123_NEED_MORE = binding.MPG123_NEED_MORE;
var MPG123_NEW_FORMAT = binding.MPG123_NEW_FORMAT;

var mode2event = {};
mode2event[binding.MODE_STOPPED] 	= 'end';
mode2event[binding.MODE_PLAYING] 	= 'play';
mode2event[binding.MODE_PAUSED] 	= 'pause';

/**
 * One-time calls...
 */

binding.mpg123_init();
process.once('exit', binding.mpg123_exit);

/**
 * The recommended size of the "output" buffer when calling mpg123_read().
 */

function Player (opts) {
	EventEmitter.apply(this, arguments);
	this.mh = new Mpg123n(opts ? opts.decoder : null);  
	this.prevMode = binding.MODE_STOPPED;

	debug('created new Decoder instance');
}

inherits(Player, EventEmitter);

Player.prototype.play = function(path) {
	console.log('play');
	this.mh.play(path, this._modeHandler.bind(this));
};

Player.prototype.stop = function() {
	this.mh.stop();
};

Player.prototype.pause = function(path) {
	this.mh.pause();
};

Player.prototype.jump = function(v) {
	this.mh.jump((v>0 ? "+" : "") + v + "s");
};

Player.prototype.volume = function(v) {
	// make sure it's String - conversion in c++ depends on it
	this.mh.volume("" + v);
};

Player.prototype._modeHandler = function(mode) {
	if (mode !== this.prevMode) {
		var event = mode2event[mode];
		try {
			if (event !== undefined) {
				this.emit(event);
			} else {
				this.emit('error', {status:mode});
			}
			debug("Emitting event: " + event);
		} catch(e) {
			this.emit('error', {status:mode});
			console.error("Error when emiting event: ", event, ", error: ", e);
		}
	}
	this.prevMode = mode;
};