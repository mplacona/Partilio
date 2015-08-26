var http = require('http');
var path = require('path');
var express = require('express');
var twilio = require('twilio');
var Spark = require('spark');

// Create Express app
var app = express();
app.use(express.static(path.join(__dirname, 'public')));
Spark.login({ username: 'your_particle_user_name', password: 'your_particle_password' });

// Configuration for Twilio account and messaging service instance
var sid = "twilio_sid";
var tkn = "twilio_auth";

// Your messaging app instance - create this via REST API. Eventually you will
// do this through the portal.
var serviceSid = "twilio_service_sid";

// Generate a token
app.get('/token', function(request, response) {
    // A unique identity for this user (like an e-mail or username)
    var identity = request.query.identity;

    // A unique identifier for the device the user is connecting from
    var device = request.query.device;

    // Create a unique endpoint ID - a good path for creating a unique one is 
    // serviceSid:device:identity, but it can be any string that uniquely
    // identifies a user of your app on a given device.
    var endpointId = serviceSid + ':' + device + ':' + identity;

    // Generate a capability token we send to the client
    var generator = new twilio.Capability(sid, tkn);

    // Note that the TwiML app sid is not used by IP Messaging
    generator.allowClientOutgoing('Particle', {
        service_sid: serviceSid,
        endpoint_id: endpointId,
        identity: identity
    });

    // generate string token (JWT) we send to the client
    var tokenExpiresAfterSeconds = 60 * 60 * 60 * 12; // 12 hours
    var token = generator.generate(tokenExpiresAfterSeconds);

    // Send token to client in a JSON response
    response.send({
        token: token
    });
});

var callback = function (err, data) {
    if (err) {
        console.log('An error occurred while getting core attrs:', err);
    } else {
        console.log('Core attr retrieved successfully:', data);
    }
};

// Call a particle function
app.get('/publish', function (request, response) {
    Spark.callFunction('particle_name', 'sendMessage', request.query.body, callback);
    
    response.send({
        data : response.id
    });
});

// Start and mount express app
var port = process.env.PORT || 3000;
http.createServer(app).listen(port, function() {
    console.log('Express server started on *:' + port);
});
