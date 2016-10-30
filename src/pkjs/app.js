// Import the Clay package
var Clay = require('pebble-clay');
// Load our Clay configuration file
var clayConfig = require('./config');
// Initialize Clay
var clay = new Clay(clayConfig);

function locationSuccess(pos) {
  // We will request the weather here
  var lat = pos.coords.latitude;
  var long = pos.coords.longitude;
  
  var sendDictionary={
    'LONGITUDE':long.toString(),
    'LATITUDE': lat.toString()
  };


  //  Send to Pebble
  Pebble.sendAppMessage(sendDictionary,
                        function(e) {
                          console.log('Location sent to Pebble successfully!');
                        },
                        function(e) {
                          console.log('Error sending zmanim info to Pebble!');
                        }

                       );
  Pebble.addEventListener('appmessage',
                          function(e) {
                            console.log('AppMessage received!');
                            getWeather();
                          }                     
                         );
}

function locationError(err) {
  console.log('Error requesting location!');
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
                        function(e) {
                          console.log('PebbleKit JS ready!');
                          getWeather();
                        }
                       );
Pebble.addEventListener('appmessage', 
                        function(e) {
                          console.log('PebbleKit JS ready!');
                          getWeather();
                        }
                       );
