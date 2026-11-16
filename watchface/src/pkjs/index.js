var Clay = require('@rebble/clay');
var messageKeys=require('message_keys');
var transport=require('./transport').create(function(value,ok,fail){Pebble.sendAppMessage(value,ok,fail);},function(fn,delay){setTimeout(fn,delay);});
var defaults = {Palette: 0, Background: 0, Foreground: 0xFFFFFF, Accent: 0xAAAAAA, TimeFormat: 0, DateFormat: 1, ShowSeconds: 1, DigitalTime: 0, ShowBattery: 1, LeftDial: 0, WeatherUnits: 0};

function integer(value, fallback, maximum) {
  if (value === undefined || value === null || value === '' || typeof value === 'boolean') return fallback;
  var number = Number(value);
  return isFinite(number) && Math.floor(number) === number && number >= 0 && number <= maximum ? number : fallback;
}
function toggle(value, fallback) {return typeof value === 'boolean' ? Number(value) : integer(value, fallback, 1);}
function normalize(raw) {
  raw = raw || {};
  return {
    Palette: integer(raw.Palette, defaults.Palette, 6),
    Background: integer(raw.Background, defaults.Background, 0xFFFFFF),
    Foreground: integer(raw.Foreground, defaults.Foreground, 0xFFFFFF),
    Accent: integer(raw.Accent, defaults.Accent, 0xFFFFFF),
    TimeFormat: integer(raw.TimeFormat, defaults.TimeFormat, 2),
    DateFormat: integer(raw.DateFormat, defaults.DateFormat, 4),
    ShowSeconds: toggle(raw.ShowSeconds, defaults.ShowSeconds),
    DigitalTime: toggle(raw.DigitalTime, defaults.DigitalTime),
    ShowBattery: toggle(raw.ShowBattery, defaults.ShowBattery),
    LeftDial: integer(raw.LeftDial, defaults.LeftDial, 3),
    WeatherUnits: integer(raw.WeatherUnits, defaults.WeatherUnits, 1)
  };
}
var clay = new Clay([
  {type: 'heading', defaultValue: 'PBL-2500'},
  {type: 'text', defaultValue: 'An analog dial with luminous hands, a small-seconds subdial and a three-cell fan date.'},
  {type: 'select', messageKey: 'TimeFormat', label: 'Optional digital time format', defaultValue: '0', options: [
    {label: 'Watch default', value: '0'}, {label: '12-hour', value: '1'}, {label: '24-hour', value: '2'}
  ]},
  {type: 'select', messageKey: 'DateFormat', label: 'Date', defaultValue: '1', options: [
    {label: 'Off', value: '0'}, {label: 'Fan day window', value: '1'},
    {label: 'YYYY-MM-DD', value: '2'}, {label: 'MM/DD/YYYY', value: '3'}, {label: 'DD/MM/YYYY', value: '4'}
  ]},
  {type: 'heading', defaultValue: 'Left dial'},
  {type: 'select', messageKey: 'LeftDial', label: 'Display', defaultValue: '0', options: [
    {label: 'Seconds (default)', value: '0'}, {label: 'Weather', value: '1'},
    {label: 'Battery percentage', value: '2'}, {label: "Today's steps", value: '3'}
  ]},
  {type: 'toggle', messageKey: 'ShowSeconds', label: 'Show left dial', defaultValue: true},
  {type: 'select', messageKey: 'WeatherUnits', label: 'Weather temperature', defaultValue: '0', options: [
    {label: 'Fahrenheit', value: '0'}, {label: 'Celsius', value: '1'}
  ]},
  {type: 'text', defaultValue: 'Weather uses your phone location, rounded to about 1 km, and Open-Meteo.com. Location is requested only with the Weather dial visible. Refreshes about every 30 minutes; data over two hours old is hidden. Phone connection, Internet and location permission required. Steps uses today’s Pebble Health total; unavailable data shows --. Counts over 9,999 are abbreviated.'},
  {type: 'toggle', messageKey: 'ShowBattery', label: 'Bottom battery percentage', defaultValue: true},
  {type: 'toggle', messageKey: 'DigitalTime', label: 'Digital time footer', defaultValue: false},
  {type: 'heading', defaultValue: 'Colors'},
  {type: 'select', messageKey: 'Palette', label: 'Palette', defaultValue: '0', options: [
    {label: 'Graphite', value: '0'}, {label: 'Steel', value: '1'},
    {label: 'Olive', value: '2'}, {label: 'Ivory', value: '3'}, {label: 'Navy', value: '4'},
    {label: 'Gold', value: '5'}, {label: 'Custom colors', value: '6'}
  ]},
  {type: 'text', defaultValue: 'Select Custom colors to use the choices below. The accent colors the small seconds hand. The analog dial is always twelve-hour; the time-format setting controls the optional footer. Choose contrasting text and background colors.'},
  {type: 'color', messageKey: 'Background', label: 'Background', defaultValue: '000000', sunlight: false, allowGray: true},
  {type: 'color', messageKey: 'Foreground', label: 'Hands & labels', defaultValue: 'FFFFFF', sunlight: false, allowGray: true},
  {type: 'color', messageKey: 'Accent', label: 'Seconds hand', defaultValue: 'AAAAAA', sunlight: false, allowGray: true},
  {type: 'submit', defaultValue: 'Save to watch'}
], null, {autoHandleEvents: false});

function sendSaved() {
  var raw = localStorage.getItem('clay-settings');
  if (!raw) return false;
  try { transport.send(normalize(JSON.parse(raw))); weather.refresh();return true; } catch (error) { console.log('PBL-2500 settings could not be loaded'); }
}
function settingsNow(){try{return normalize(JSON.parse(localStorage.getItem('clay-settings')||'{}'));}catch(e){return normalize({});}}
var weather=require('./weather').create({
  settings:settingsNow, storage:localStorage, now:function(){return Math.floor(Date.now()/1000);},
  send:function(value){transport.send(value);},
  locate:function(ok,fail){
    if(typeof navigator==='undefined'||!navigator.geolocation){fail();return;}
    navigator.geolocation.getCurrentPosition(ok,fail,{timeout:10000,maximumAge:900000,enableHighAccuracy:false});
  },
  request:function(url,ok,fail){
    var xhr=new XMLHttpRequest();xhr.open('GET',url,true);xhr.timeout=15000;
    xhr.onload=function(){if(xhr.status!==200){var retry=Number(xhr.getResponseHeader('Retry-After'));fail({permanent:xhr.status>=400 && xhr.status<500 && xhr.status!==408 && xhr.status!==425 && xhr.status!==429,retryAfter:isFinite(retry)&&retry>0?Math.min(retry,7200):0});return;}try{ok(JSON.parse(xhr.responseText));}catch(e){fail({permanent:true});}};
    xhr.onerror=fail;xhr.ontimeout=fail;xhr.send();
  }
});
Pebble.addEventListener('ready', function(){if(!sendSaved())weather.refresh();});
Pebble.addEventListener('appmessage',function(event){if(event && event.payload && (event.payload.WeatherRequest || event.payload[messageKeys.WeatherRequest]))weather.refresh();});
Pebble.addEventListener('showConfiguration', function() { Pebble.openURL(clay.generateUrl()); });
Pebble.addEventListener('webviewclosed', function(event) {
  if (!event || !event.response || event.response === 'CANCELLED') return;
  try {
    clay.getSettings(event.response, false);
    sendSaved();
  } catch (error) { console.log('PBL-2500 settings were not saved'); }
});

module.exports = {normalize: normalize, defaults: defaults};
