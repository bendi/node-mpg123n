if (process.env.NODE_MPG123N_CI !== "true") {
    exports.Player = require('./lib/player');
} else {
    exports.Player = function () {
        throw new Error("mpg123n continuous integration mode - no output is supported");
    };
}