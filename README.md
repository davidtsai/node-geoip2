# node-geoip2
Maxmind GeoIP2 database API for geolocating ip addresses.

Fast native implementation by wrapping libmaxminddb to read MaxMind's GeoIP2 mmdb databases.

For ease of use, this module includes the GeoLite2 database created by MaxMind, available from http://www.maxmind.com.  To get more accurate results, this module can also be used with the GeoIP2 database provided by MaxMind.

## Installtion
`npm install --save node-geoip2`

## Usage
To look up an ip address:
```
var geoip2 = require('node-geoip2');
geoip2.init();
geoip2.lookupSimple("67.183.57.64", function(error, result) {
  if (error) {
    console.log("Error: %s", error);
  }
  else if (result) {
    console.log(result);
  }
});
```
Produces:
```
{ country: 'US',
  continent: 'NA',
  postal: '98275',
  city: 'Mukilteo',
  location:
   { latitude: 47.9445,
     longitude: -122.3046,
     metro_code: 819,
     time_zone: 'America/Los_Angeles' },
  subdivision: 'WA' }
```

You can also lookup the full data record for the address using the `lookup` method instead.




