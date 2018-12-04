// Requirements
// -----------------------------------------------------------------------------
const express = require('express');




function initApp(callback) {
    // Create the app
    // -----------------------------------------------------------------------------
    const app = express()

    // Load the API routes
    const apiRoutes = require('./controllers/api/api');
    app.use('/api', apiRoutes);

    // // Mount controllers
    // app.use(require('./controllers'));

    // Default error page
    app.get('*', (req, res) => {
        res.status(404).send('Route not found :(')
    });

    callback(app);
}


// Exporting the app to be loaded on the server
module.exports = {
  initApp
  // app: app
};
