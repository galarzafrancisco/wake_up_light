// Imports
// -----------------------------------------------------------------------------
const axios = require('axios');

const config = require('./../../config');
const env = config['env'];

// Import a router
const apiRoutes = require('express').Router();


apiRoutes.get('/env', (req, res) => {
    res.send(process.env);
});

apiRoutes.get('/wake_up_light', (req, main_response) => {
    const url = config[env]['wake_up_api_url']
    console.log('Request to /wake_up_light');
    //console.log(req);
    axios.get(url)
        .then((sub_request_response) => {
            const data = sub_request_response.data
            console.log(data)
            main_response.status(200).send(sub_request_response.data.toString())
        })
        .catch((error) => {
            main_response.status(404).send(error)
        })
});
  
// Put your routes here
apiRoutes.all('*', (req, res) => {
    res.status(200).json({message: 'Welcome to the api!'});
})


// Export the router
module.exports = apiRoutes;
