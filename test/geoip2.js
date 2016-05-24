
var expect = require('chai').expect;
var geoip2 = require('../node-geoip2');

describe('geoip2', function() {
  context('with the default geoip2 database', function() {
    before(function() {
      geoip2.init();
    });
    describe('#lookupSimpleSync', function() {
      it('returns a record with the expected values', function() {
        var result = geoip2.lookupSimpleSync('67.183.57.64'); 
	expect(result.subdivision).to.equal('WA');
	expect(result.country).to.equal('US');
        expect(result.continent).to.equal('NA');
        expect(result.postal).to.equal('98275');
        expect(result.location.latitude).to.be.a('number');
        expect(result.location.longitude).to.be.a('number');
      });
      it('throws an exception when an invalid ip address is entered', function() {
        try {
          var result = geoip2.lookupSimpleSync('asdf');
        }
        catch(error) {
          expect(error).to.exist;
          return;
        }
        expect.fail('Expected lookupSimpleSync to throw');
      });
    });

    describe('#lookupSimple', function() {
      it('returns a record with the expected values', function(done) {
        geoip2.lookupSimple('67.183.57.64', function(error, result) {
	  expect(result.subdivision).to.equal('WA');
      	  expect(result.country).to.equal('US');
          expect(result.continent).to.equal('NA');
          expect(result.postal).to.equal('98275');
          expect(result.location.latitude).to.be.a('number');
          expect(result.location.longitude).to.be.a('number');
	  done();
	}); 
      });
    });
  });
});
