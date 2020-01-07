#/bin/bash


echo Not updating database - see https://dev.maxmind.com/geoip/geoip2/geolite2/
exit 0

url=https://geolite.maxmind.com/download/geoip/database/GeoLite2-City.tar.gz

echo Downloading Maxmind Geo City database
curl $url -o db.tar.gz
tar zxvf db.tar.gz

mv GeoLite2-City*/* databases
rm db.tar.gz