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

function xhrRequest(url, type, callback) {
	var xhr = new XMLHttpRequest();
	xhr.onload = function () {
		callback(this.responseText);
	};
	xhr.open(type, url);
	xhr.send();
};

// Assemble dictionary using our keys
function locationSuccess(pos) {
	console.log('Requesting weather');
	// We will request the weather here
	// Construct URL
	var url = 'http://api.openweathermap.org/data/2.5/weather?appid=' +
		'4b39ab94aa9d253245c8f937f133b1df' +
		'&q=Syktyvkar&units=metric'

	// Send request to OpenWeatherMap
	xhrRequest(url, 'GET', 
		function(responseText) {
			// responseText contains a JSON object with weather info
			var json = JSON.parse(responseText);

			// Temperature in Kelvin requires adjustment
			var temperature = Math.round(json.main.temp);
			console.log('Temperature is ' + temperature);

			// Conditions
			var conditions = json.weather[0].main;
			console.log('Conditions are ' + conditions);

			var dictionary = {
				'TEMPERATURE': temperature,
				'CONDITIONS': conditions
			};

			// Send to Pebble
			Pebble.sendAppMessage(dictionary,
				function(e) {
					console.log('Weather info sent to Pebble successfully!');
				},
				function(e) {
					console.log('Error sending weather info to Pebble!');
				}
			);
		}
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

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
	function(e) {
		console.log('AppMessage received!');
		getWeather();
	}
);
