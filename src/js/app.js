var suncalc = require('suncalc');
suncalc.addTime(-16.1, 'alot_hashachar', 0);
suncalc.addTime(-11.5, 'misheyakir', 0);
suncalc.addTime(-8.5, 0, 'tzeit');

function locationSuccess(pos) {
  // We will request the weather here
  var lat = pos.coords.latitude;
  var long = pos.coords.longitude;
  var today = new Date();
  var sunTimes = suncalc.getTimes( today,  lat, long);
  
  var sunset = sunTimes.sunset;
  var sunrise = sunTimes.sunrise;
  var zhour = (sunset-sunrise) / 12;
  var halachicTimes = {
    'ALOS': sunTimes.alot_hashachar,
    'MISHEYAKIR' : sunTimes.misheyakir,
    'NEITZ' : sunrise,
    'SHMA_GRA': new Date(sunrise.getTime()+(zhour*3)),
    'TEFILA_GRA':new Date(sunrise.getTime()+(zhour*4)),
    'CHATZOS':new Date(sunrise.getTime()+(zhour*6)),
    'MINCHA_GEDOLA' : new Date(sunrise.getTime()+(zhour*6)+(zhour/2)),
    'SHKIA' : sunset,
    'TZAIS' : sunTimes.tzeit
  };
   
  for(var key in halachicTimes){
    console.log(key + halachicTimes[key]);
  }
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
