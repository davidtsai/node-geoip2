#/bin/bash

url=https://geolite.maxmind.com/download/geoip/database/GeoLite2-City.tar.gz

echo Downloading Maxmind Geo City database
curl $url -o db.tar.gz
tar zxvf db.tar.gz

rm -rf databases
mv GeoLite2-City* databases
rm db.tar.gz