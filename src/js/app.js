var suncalc = require('suncalc');
suncalc.addTime(-16.1, 'alot_hashachar', 0);
suncalc.addTime(-11.5, 'misheyakir', 0);
suncalc.addTime(-8.5, 0, 'tzeit');
suncalc.addTime(16.1, 'mincha_gedola',0);

function locationSuccess(pos) {
  // We will request the weather here
  var lat = pos.coords.latitude;
  var long = pos.coords.longitude;
  var today = new Date();
  var sunTimes = suncalc.getTimes( today,  lat, long);
  console.log(sunTimes.tzeit);
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

    // Get the initial weather
    getWeather();
  }
);
