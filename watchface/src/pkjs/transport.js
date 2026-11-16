/* Serialize idempotent settings/weather messages: PebbleKit has one outbox. */
function create(send,schedule){
 var queue=[],busy=false;
 function drain(){
  if(busy || !queue.length)return;
  busy=true;var entry=queue[0],done=false;
  function finish(ok){if(done)return;done=true;
   if(!ok && entry.tries<2){entry.tries++;schedule(function(){busy=false;drain();},entry.tries*1000);return;}
   queue.shift();busy=false;drain();
  }
  try{send(entry.value,function(){finish(true);},function(){finish(false);});}catch(e){finish(false);}
 }
 return {send:function(value){queue.push({value:value,tries:0});drain();}};
}
module.exports={create:create};
