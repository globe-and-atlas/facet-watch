"""Capture selectable left-dial fixtures from the parent's already-owned emulator."""
from pathlib import Path
import argparse,json,runpy,time,math
from PIL import Image,ImageDraw,ImageFont
ROOT=Path(__file__).resolve().parents[1]

def main():
 parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--monitor',type=int,required=True);args=parser.parse_args()
 native=runpy.run_path(str(ROOT/'execution/capture_native.py'));keys=json.loads((ROOT/'watchface/build/js/message_keys.json').read_text());uuid=json.loads((ROOT/'watchface/package.json').read_text())['pebble']['uuid']
 base={11013:0,'Palette':0,'LeftDial':0,'WeatherUnits':0,'ShowSeconds':1,'ShowBattery':1,'DigitalTime':0,'DateFormat':1,11000:10,11001:8,11002:30,11004:82,11009:5832,11010:1,11011:222,11012:2}
 cases=[('seconds',{}),('weather',{'LeftDial':1}),('battery',{'LeftDial':2}),('steps',{'LeftDial':3}),('weather-c',{'LeftDial':1,'WeatherUnits':1}),('weather-negative',{'LeftDial':1,11011:-126,11012:71}),('weather-min-c',{'LeftDial':1,'WeatherUnits':1,11011:-900}),('weather-min-f',{'LeftDial':1,11011:-900}),('weather-max-f',{'LeftDial':1,11011:650}),('weather-missing',{'LeftDial':1,11010:0}),('weather-stale',{'LeftDial':1,11010:2}),('steps-missing',{'LeftDial':3,11009:-1}),('steps-zero',{'LeftDial':3,11009:0}),('steps-9999',{'LeftDial':3,11009:9999}),('steps-99999',{'LeftDial':3,11009:99999}),('steps-999999',{'LeftDial':3,11009:999999}),('battery-zero',{'LeftDial':2,11004:0}),('battery-full',{'LeftDial':2,11004:100}),('hidden',{'LeftDial':3,'ShowSeconds':0})]
 for name,code in [('clear',0),('fog',45),('rain',61),('storm',95)]:cases.append((f'weather-{name}',{'LeftDial':1,11012:code}))
 for palette in range(1,6):
  for mode in range(1,4):cases.append((f'palette-{palette}-dial-{mode}',{'Palette':palette,'LeftDial':mode}))
 evidence=[]
 for name,change in cases:
  values={**base,**change};native['send'](keys,uuid,values);bg,fg,accent=map(native['rgb'],native['PALETTES'][values['Palette']]);peak=max(max(bg),max(fg),max(accent))
  for attempt in range(30):
   time.sleep(.1);im=native['screenshot'](args.monitor,peak)
   try:
    assert im.getpixel((0,0))==bg
    assert im.getpixel((100,114))==fg
    assert native['near_color'](im,native['point'](48,42),fg)
    if values['ShowSeconds']:
     assert im.getpixel((52,81))==native['mix'](native['PALETTES'][values['Palette']][0],native['PALETTES'][values['Palette']][1],1)
     assert (accent if values['LeftDial']==0 else fg) in set(im.crop((23,104,82,129)).get_flattened_data())
    else:
     # Leftmost circle rim is absent when the dial is hidden.
     assert im.getpixel((52,81))==bg
    break
   except AssertionError:
    if attempt==29:raise AssertionError(name)
  im.save(ROOT/f'media/dial-{name}.png')
  clearance=None
  if values['ShowSeconds']:
   native['send'](keys,uuid,{11013:1})
   for attempt in range(30):
    time.sleep(.1);mask=native['screenshot'](args.monitor,peak)
    ink=[math.hypot(x-52,y-114) for y in range(228) for x in range(200) if mask.getpixel((x,y))!=bg]
    if ink and max(ink)<=29:clearance=round(32-max(ink),2);break
   else:
    mask.save(ROOT/f'.tmp/clearance-{name}.png');raise AssertionError(f'{name}: text/bar reaches radius {max(ink) if ink else "empty"}; limit29')
   native['send'](keys,uuid,{11013:0})
  evidence.append({'case':name,'native':True,'minimum_border_gap_px':clearance});print('PASS:',name,clearance,flush=True)
 assert Image.open(ROOT/'media/dial-weather-missing.png').crop((23,129,82,144)).tobytes()!=Image.open(ROOT/'media/dial-weather-stale.png').crop((23,129,82,144)).tobytes(), 'Missing/stale labels differ'
 (ROOT/'.tmp/dial-checks.json').write_text(json.dumps(evidence,indent=2))
 board=Image.new('RGB',(928,306),(27,29,32));draw=ImageDraw.Draw(board);font=ImageFont.truetype('/System/Library/Fonts/Supplemental/Arial Bold.ttf',19)
 for i,name in enumerate(['seconds','weather','battery','steps']):
  x=16+i*232;draw.text((x,15),name.title(),font=font,fill='white');board.paste(Image.open(ROOT/f'media/dial-{name}.png'),(x,53))
 board.save(ROOT/'media/pbl-2500-left-dials.png')
 native['send'](keys,uuid,base)

if __name__=='__main__':main()
