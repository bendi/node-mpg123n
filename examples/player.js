var Player = require('../lib/player.js');

var player = new Player();

player.on('play', function() {
	console.log("playing!");
});
player.on('stop', function() {
	console.log("stopped!");
});
player.on('pause', function() {
	console.log('paused!');
});
player.play(process.argv[2]);