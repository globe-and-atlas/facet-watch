/* Weather runs only for the selected, visible Weather dial. No coordinates are stored. */
var MAX_AGE=7200, REFRESH=1800, RETRY=300;
function isInteger(n){return typeof n==='number' && isFinite(n) && Math.floor(n)===n;}
function create(env) {
  var busy=false,next=0,generation=0,failures=0;
  function enabled(){var s=env.settings();return s.LeftDial===1 && s.ShowSeconds===1;}
  function cached(){try {var c=JSON.parse(env.storage.getItem('pbl-weather')||'null');return valid(c)?c:null;}catch(e){return null;}}
  function valid(c){return c && isInteger(c.temp) && c.temp>=-900 && c.temp<=650 && isInteger(c.code) && [0,1,2,3,45,48,51,53,55,56,57,61,63,65,66,67,71,73,75,77,80,81,82,85,86,95,96,99].indexOf(c.code)>=0 && isInteger(c.time) && c.time>0 && env.now()-c.time>=-300 && env.now()-c.time<=MAX_AGE;}
  function deliver(c){if(!enabled())return;env.send(c?{WeatherTemp:c.temp,WeatherCode:c.code,WeatherTime:c.time}:{WeatherTime:0});}
  function cancel(){generation++;busy=false;}
  function refresh(){
    if(!enabled()){cancel();return;}
    var cache=cached();deliver(cache);
    if(busy || env.now()<next)return;
    if(cache && env.now()-cache.time<REFRESH){next=cache.time+REFRESH;return;}
    busy=true;var run=++generation;next=env.now()+RETRY;
    function active(){return generation===run && enabled();}
    function fail(error){if(!active())return;busy=false;failures++;var permanent=error && (error.permanent || error.code===1);var delay=permanent?REFRESH:Math.min(REFRESH,RETRY*Math.pow(2,Math.min(failures-1,3)));next=env.now()+Math.max(delay,error && error.retryAfter || 0);deliver(cached());}
    env.locate(function(position){
      if(!active())return;
      if(!position || !position.coords){fail({permanent:true});return;}
      var lat=position.coords.latitude,lon=position.coords.longitude;
      if(typeof lat!=='number' || typeof lon!=='number'){fail({permanent:true});return;}
      if(!isFinite(lat)||!isFinite(lon)||Math.abs(lat)>90||Math.abs(lon)>180){fail();return;}
      // Round to about a kilometre before requesting weather.
      var url='https://api.open-meteo.com/v1/forecast?latitude='+lat.toFixed(2)+'&longitude='+lon.toFixed(2)+'&current=temperature_2m,weather_code&temperature_unit=celsius&timeformat=unixtime';
      env.request(url,function(data){
        if(!active())return;
        var current=data && data.current;
        if(!current || typeof current.temperature_2m!=='number' || !isFinite(current.temperature_2m)){fail();return;}
        var value={temp:Math.round(current.temperature_2m*10),code:current.weather_code,time:current.time};
        if(!valid(value)){fail();return;}
        busy=false;failures=0;next=env.now()+REFRESH;
        env.storage.setItem('pbl-weather',JSON.stringify(value));deliver(value);
      },fail);
    },fail);
  }
  return {refresh:refresh,cancel:cancel,valid:valid};
}
module.exports={create:create};
