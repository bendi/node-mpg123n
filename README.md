node-mpg123n
============

nodejs native bindings to [mpg123](http://mpg123.de) interactive mode (-R option) that compile and run under MSVC++

Installation
------------
`node-mpg123n` comes bundled with its own copy of `libmpg123` and `mpg123` application (v. 1.15.3), so 
there's no need to have `mpg123` installed on your system.

Interactive mode has been rewritten (control_generic.c) so it's possible to interact with nodejs. Some additional
changes were made in order to have it compile and run under windows operating system.

Thanks to the following you can compile and install `node-mpg123n` using npm:

```bash
npm install mpg123n
```

[node-gyp environment setup](https://github.com/TooTallNate/node-gyp#installation)

Example
------------
Here's an example of how `node-mpg123n` can be used to start and stop a single song. The path
to an mp3 file is passed from command-line.

``` javascript
var mpg123n = require('mpg123n');

var player = new mpg123n.Player();

player.on('play', function() {
	console.log("playing!");
});
player.on('stop', function() {
	console.log("stopped!");
});
player.on('pause', function() {
	console.log('paused!');
});
player.on('error', function() {
	console.log('Houston we have a problem!');
});
player.play(process.argv[2]);
```

API
------------
### Methods
 * Player.play - path to a file
 * Player.stop 
 * Player.pause 
 * Player.jump - offset in seconds, eg. 10s
 * Player.volume - volume in percent eg. 10%

### Events
 * play
 * stop
 * pause
 * error

Projects using mpg123n
------------
### [Maracuyá - Virutal Jukebox](http://maracuya-jukebox.com)
 * contains precompiled version of mpg123n
 * embeds nodejs
 * utilizes a simple yet powerful interface

