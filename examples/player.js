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
player.on('end', function() {
	console.log('ended!');
});
player.play(process.argv[2]);

// pause/resume
var paused = false;
function pauseResume() {
    player.pause();
    paused = !paused;
    console.log("Paused: " + paused);
}
