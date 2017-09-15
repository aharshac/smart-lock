var liveServer = require("live-server");
 
var params = {
    host: "0.0.0.0", // Set the address to bind to. Defaults to 0.0.0.0 or process.env.IP. 
    root: "./src", // Set root directory that's being served. Defaults to cwd. 
};
liveServer.start(params);