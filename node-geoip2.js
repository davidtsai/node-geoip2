
var mmdb = require('./lib/node_mmdb.node');
var path = require('path');

var _defaultPath = path.resolve(__dirname, './databases/GeoLite2-City.mmdb');
var _defaultDB   = null;

exports.MMDB = mmdb.MMDB;

exports.init = function(path) {

    _defaultDB = new mmdb.MMDB(path || _defaultPath);
    return _defaultDB;
}

exports.cleanup = function() {

    _defaultDB = null;
}

exports.lookup = function(address, callback) {

    if (!_defaultDB) {
        console.log("WARNING: ipgeo2 database not initialzed, initializing default now.");
        exports.init();
    }

    _defaultDB.lookup(address, callback);
}

exports.lookupSync = function(address) {

    if (!_defaultDB) {
        console.log("WARNING: ipgeo2 database not initialzed, initializing default now.");
        exports.init();
    }

    return _defaultDB.lookupSync(address);
}

function parseResult(result) {

    return {
        country:     result.country ? result.country.iso_code : undefined,
        continent:   result.continent ? result.continent.code : undefined,
        postal:      result.postal ? result.postal.code : undefined,
        city:        result.city && result.city.names ? result.city.names.en : undefined,
        location:    result.location,
        subdivision: result.subdivisions ? result.subdivisions[0].iso_code : undefined
    };
}

exports.lookupSimple = function(address, callback) {

    exports.lookup(address, function(error, result) {

        if (result) {
            callback(null, parseResult(result));
        }
        else {
            callback(error, null);
        }
    });
}

exports.lookupSimpleSync = function(address) {

    var result = exports.lookupSync(address);
    
    if (result) {
        return parseResult(result);
    }

    return null;
}
