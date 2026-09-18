// Regression checks for map selection and route highlighting, without a browser.
const fs = require('node:fs');
const vm = require('node:vm');
const assert = require('node:assert/strict');
const path = require('node:path');
const root = path.resolve(__dirname, '..');
const elements = new Map(), strokes = [];
const context = new Proxy({measureText: s => ({width: s.length * 7}), stroke() { strokes.push({color:this.strokeStyle,width:this.lineWidth}); }}, {get(t,k) { return k in t ? t[k] : ()=>{}; }});
function element(id) {
 if (!elements.has(id)) elements.set(id, {value:id==='depth'?'1':'',checked:false,style:{},classList:{toggle(){}},replaceChildren(){},append(){},add(){},addEventListener(){},getContext:()=>context,getBoundingClientRect:()=>({width:1200,height:650}),textContent:''});
 return elements.get(id);
}
const sandbox = {console,document:{getElementById:element,createElement:()=>element(Symbol())},Option:function(){},ResizeObserver:class {observe(){}},requestAnimationFrame:()=>1,window:{devicePixelRatio:1},setTimeout};
vm.createContext(sandbox);
vm.runInContext(fs.readFileSync(path.join(root,'tools/network-graph/app.js'),'utf8').replace(/try\{load\(window.NETWORK_DATA\);.*?\n/,''),sandbox);
assert.ok(vm.runInContext(`(()=>{
 const ids=new Set(['a','b','c','d']),edges=[{source:'a',target:'d'},{source:'b',target:'c'}];
 const levels=new Map([['a',0],['b',0],['c',1],['d',1]]),p=layoutRoutes(ids,edges,levels);
 return p.size===4&&(p.get('a').y-p.get('b').y)*(p.get('d').y-p.get('c').y)>0;
})()`,sandbox),'Connected rows must remove the avoidable crossing between parallel branches');
sandbox.capture=JSON.parse(fs.readFileSync(path.join(root,'exports/lyon/network.json'),'utf8'));
vm.runInContext(`
load(capture);
globalThis.q = data.queries.find(q=>q.signal==='0x800000b0f0004'&&q.direction===-1);
direction=q.direction;
chooseNode(signals.get(q.signal).node);
`,sandbox);
assert.equal(vm.runInContext('mode',sandbox),'overview');
assert.equal(vm.runInContext('direction',sandbox),vm.runInContext('forwardDirection(selectedSignal())',sandbox));
assert.ok(vm.runInContext('scene.edges.length>0',sandbox),'Clicking a map signal must calculate its route');
vm.runInContext('draw()',sandbox);
assert.ok(strokes.some(s=>s.color==='#81e6c2'&&s.width===3.5),'Map must draw highlighted connections');
vm.runInContext(`direction=-direction;buildScene();`,sandbox);
assert.ok(vm.runInContext('scene.nodes.has(selected)',sandbox));
// Distinct signals at the same position must retain the exact selected ID.
vm.runInContext(`globalThis.original=selectedSignal();signals.set('test-colocated',{...original,id:'test-colocated'});chooseSignal('test-colocated',false);`,sandbox);
assert.equal(vm.runInContext('selectedSignal().id',sandbox),'test-colocated');
vm.runInContext(`chooseNode(data.nodes.find(n=>!n.signalIds.length).id);`,sandbox);
assert.equal(vm.runInContext('scene',sandbox),null,'Selecting a non-signal on the map clears stale highlighting');
vm.runInContext(`
load(capture);
globalThis.audit = [];
globalThis.badConnections = [];
const edgePairs=new Set(data.edges.map(e=>[e.source,e.target].sort().join('|')));
const edgeById=new Map(data.edges.map(e=>[e.id,e]));
globalThis.exactEdgesChecked=0;
for(const q of data.queries)for(const p of q.paths){
 if(p.edgeIds.length!==p.nodes.length-1)throw Error('Missing exact path edges');
 p.edgeIds.forEach((id,i)=>{const e=edgeById.get(id);if(!e||[e.source,e.target].sort().join('|')!==[p.nodes[i],p.nodes[i+1]].sort().join('|'))throw Error('Wrong physical edge');exactEdgesChecked++;});
}
for(const expected of data.forwardQueries||[]){
 const actual=forwardQuery(expected.signal,expected.direction);
 if(JSON.stringify([...actual.next].sort())!==JSON.stringify([...expected.next].sort()))audit.push({signal:expected.signal,expected:expected.next,actual:actual.next});
 for(const p of actual.paths)for(let i=1;i<p.nodes.length;i++)if(p.nodes[i]!==p.nodes[i-1]&&!edgePairs.has([p.nodes[i-1],p.nodes[i]].sort().join('|')))badConnections.push(expected.signal);
}
globalThis.target=forwardQuery('0x800000b0f0004',-1);
globalThis.layoutAudit={graphs:0,backwardEdges:0,sameColumnEdges:0,cut:0,target:null};
for(const depth of [1,2,3,4,8,Infinity])for(const s of signals.values()){
 $('depth').value=depth===Infinity?'all':String(depth);
 selected=s.node;selectedSignalId=s.id;buildScene();
 const expectedEdges=new Set();
 const queue=[{id:s.id,d:forwardDirection(s),step:0}],seen=new Set();
 for(let at=0;at<queue.length;at++){const f=queue[at],k=key(f.id,f.d);if(seen.has(k)||f.step>=depth)continue;seen.add(k);for(const p of forwardQuery(f.id,f.d).paths){for(const id of p.edgeIds)expectedEdges.add(id);queue.push({id:p.signal,d:p.arrivalDirection,step:f.step+1});}}
 if(JSON.stringify([...scene.physicalEdges].sort())!==JSON.stringify([...expectedEdges].sort()))throw Error('Map highlights extra or missing physical branches');
 const back=scene.edges.filter(e=>scene.positions.get(e.target).x<scene.positions.get(e.source).x).length;
 const same=scene.edges.filter(e=>scene.positions.get(e.target).x===scene.positions.get(e.source).x).length;
 layoutAudit.graphs++;layoutAudit.backwardEdges+=back;layoutAudit.sameColumnEdges+=same;layoutAudit.cut+=Number(scene.cut);
 if(s.id==='0x800000b0f0004'&&depth===1)layoutAudit.target={back,same,nodes:scene.nodes.size};
}
`,sandbox);
console.log('Forward query audit:',JSON.stringify(sandbox.audit));
assert.equal(sandbox.audit.length,0,'All signals must match direct native-port traversal');
assert.equal(sandbox.badConnections.length,0,'Every highlighted connection must exist in the native graph');
console.log('Layout audit:',JSON.stringify(sandbox.layoutAudit));
console.log('Exact physical edge references checked:',sandbox.exactEdgesChecked);
assert.equal(sandbox.layoutAudit.cut,0,'All chain depths must fit this captured network');
console.log('b0f0004 next:',JSON.stringify(sandbox.target.next));
fs.writeFileSync(path.join(root,'build/network-graph-audit.json'),JSON.stringify({signalsChecked:sandbox.capture.signals.length,mismatches:sandbox.audit,invalidConnections:sandbox.badConnections,layout:sandbox.layoutAudit,target:{id:'0x800000b0f0004',...sandbox.target}},null,2));
console.log('PASS: map click, highlighted strokes, direction, colocated IDs, cleared selection');
assert.equal(vm.runInContext('forwardDirection({kind:4,direction:1})',sandbox),-1);
assert.equal(vm.runInContext('forwardDirection({kind:4,direction:-1})',sandbox),1);
assert.equal(vm.runInContext('forwardDirection({kind:0,direction:1})',sandbox),1);
vm.runInContext(`
queryCache.clear();signals=new Map([
 ['a',{id:'a',node:'A',kind:4,direction:-1}],
 ['b',{id:'b',node:'B',kind:4,direction:1}],
 ['c',{id:'c',node:'C',kind:4,direction:-1}]
]);
queries=new Map([
 ['a/1',{paths:[{signal:'b',arrivalDirection:1,nodes:['A','B']}]}],
 ['b/1',{paths:[{signal:'c',arrivalDirection:1,nodes:['B','C']}]}]
]);
globalThis.forwardResult=forwardQuery('a',1);
`,sandbox);
assert.equal(JSON.stringify(sandbox.forwardResult.next),'["c"]','Pass the opposite Path and find the next facing signal');
assert.equal(JSON.stringify(sandbox.forwardResult.paths[0].nodes),'["A","B","C"]');
vm.runInContext("queryCache.clear();signals.get('b').kind=0;signals.get('b').direction=-1;globalThis.blockedOneWay=forwardQuery('a',1);",sandbox);
assert.equal(sandbox.blockedOneWay.next.length,0,'Block a reversed OneWay');
assert.equal(sandbox.blockedOneWay.blocked[0].signal,'b');
vm.runInContext("queryCache.clear();signals.get('b').direction=1;globalThis.allowedOneWay=forwardQuery('a',1);",sandbox);
assert.equal(JSON.stringify(sandbox.allowedOneWay.next),'["c"]','Pass a forward OneWay without ending the search');
vm.runInContext("queryCache.clear();signals.get('b').kind=5;globalThis.blockedNoWay=forwardQuery('a',1);",sandbox);
assert.equal(JSON.stringify(sandbox.blockedNoWay.next),'["b"]','Stop at NoWay');
assert.equal(sandbox.blockedNoWay.paths[0].terminal,true);
assert.equal(vm.runInContext("forwardQuery('b',1).next.length",sandbox),0,'A NoWay cannot restart the chain');
vm.runInContext("queryCache.clear();signals.get('b').exceptionCount=2;globalThis.exceptedNoWay=forwardQuery('a',1);",sandbox);
assert.equal(JSON.stringify(sandbox.exceptedNoWay.next),'["c"]','Pass a NoWay with native exceptions');
assert.equal(vm.runInContext("forwardQuery('b',1).next[0]",sandbox),'c','Allow starting from an excepted NoWay');
vm.runInContext("queryCache.clear();signals.get('b').exceptionCount=0;globalThis.noExceptions=forwardQuery('a',1);",sandbox);
assert.equal(JSON.stringify(sandbox.noExceptions.next),'["b"]','Stop at a NoWay with an empty exception list');
vm.runInContext("queryCache.clear();signals.get('b').kind=3;globalThis.ignoredBalise=forwardQuery('a',1);",sandbox);
assert.equal(JSON.stringify(sandbox.ignoredBalise.next),'["c"]','Pass through balise to the next Path');
assert.ok(!sandbox.ignoredBalise.paths[0].terminal);
assert.equal(vm.runInContext("forwardQuery('b',1).next[0]",sandbox),'c','A balise may continue the chain');
console.log('PASS: opposite Path traversal; OneWay enforced; NoWay terminates; balises passed');
vm.runInContext(`
globalThis.arrowChecks=[];
arrow=(a,b)=>arrowChecks.push({dx:b.x-a.x,dy:b.y-a.y});
mapArrows([{x:20,y:100},{x:220,y:100}]);
globalThis.forwardArrows=arrowChecks;arrowChecks=[];
mapArrows([{x:220,y:100},{x:20,y:100}]);
globalThis.reverseArrows=arrowChecks;arrowChecks=[];
mapArrows([{x:100,y:20},{x:100,y:220}]);
globalThis.verticalArrows=arrowChecks;
`,sandbox);
assert.ok(sandbox.forwardArrows.length>0&&sandbox.forwardArrows.every(a=>a.dx>0&&a.dy===0));
assert.ok(sandbox.reverseArrows.length>0&&sandbox.reverseArrows.every(a=>a.dx<0&&a.dy===0));
assert.ok(sandbox.verticalArrows.length>0&&sandbox.verticalArrows.every(a=>a.dx===0&&a.dy>0));
console.log('PASS: map arrows follow forward, reverse and vertical path geometry');
