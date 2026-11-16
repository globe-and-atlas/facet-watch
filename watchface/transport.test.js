var assert=require('assert'),Transport=require('./src/pkjs/transport');
var sent=[],callbacks=[],timers=[];
var queue=Transport.create(function(v,ok,fail){sent.push(v);callbacks.push({ok:ok,fail:fail});},function(fn,delay){timers.push(fn);});
queue.send({setting:1});queue.send({weather:2});assert.strictEqual(sent.length,1);callbacks[0].ok();assert.strictEqual(sent.length,2);assert.deepStrictEqual(sent[1],{weather:2});callbacks[1].fail();assert.strictEqual(timers.length,1);timers.shift()();assert.strictEqual(sent.length,3);callbacks[2].fail();timers.shift()();callbacks[3].fail();queue.send({last:3});assert.strictEqual(sent.length,5,'bounded retries release queue');callbacks[4].ok();
console.log('PASS: shared phone outbox serializes delayed ACKs and bounds retries');
