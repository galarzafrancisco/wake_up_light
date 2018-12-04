// server.js

// This is the entry point


// BASE SETUP
// =============================================================================
'use strict';


// Imports
// -----------------------------------------------------------------------------
const http = require('http');

const initApp = require('./app').initApp;
const config = require('./config');
const env = config['env'];

// Initialize a simple HTTP server
initApp((app) => {
  const server = http.createServer(app) // passing the express app as an argument will route the requests to the app (I guess)

  // Start the server
  server.listen(process.env.PORT || config[env]['port'], () => {
    console.log(`Server started on port ${server.address().port} :)`);
  });
});
