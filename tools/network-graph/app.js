/* Offline viewer: no analytics, libraries, network calls or live-game writes. */
'use strict';
const $=id=>document.getElementById(id), esc=x=>String(x??'').replace(/[&<>"']/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));
const kinds={0:'Sens unique',1:'Arrêt de quai',3:'Balise',4:'Path',5:'Interdiction',6:'Marqueur'};
const colors={signal:'#80caff',junction:'#edb76b',boundary:'#8191a5'};
let data,nodes,signals,tracks,junctions,queries,stations,selected=null,selectedSignalId=null,selectedEdge=null,mode='overview',direction=1,limit=35,scene=null;
let camera={x:0,y:0,scale:1},size={w:800,h:600},drag=null,lastHits=[],hover=null,raf=0;
const canvas=$('graph'),ctx=canvas.getContext('2d');
const key=(id,d)=>id+'/'+d;
const queryCache=new Map();
// Signal_M_forward (RVA 0x427ef0) reverses the stored direction for Path.
const forwardDirection=s=>s.kind===4?-s.direction:s.direction;
const hasExceptions=s=>Number.isInteger(s.exceptionCount)&&s.exceptionCount>0;
const stopsAtNoWay=s=>s.kind===5&&!hasExceptions(s);
function forwardQuery(id,d){
 const cacheKey=key(id,d);if(queryCache.has(cacheKey))return queryCache.get(cacheKey);
 const first=signals.get(id),found=new Map(),blocked=new Map(),visited=new Set();
 if(stopsAtNoWay(first))return {next:[],paths:[],blocked:[],incomplete:false,truncated:false};
 const queue=[{id,d,nodes:[first.node],edgeIds:[]}];let incomplete=false,truncated=false;
 for(let at=0;at<queue.length;at++){
  const f=queue[at],k=key(f.id,f.d);if(visited.has(k))continue;visited.add(k);
  if(visited.size>4096){truncated=true;break;}
  const q=queries.get(k);if(!q){incomplete=true;continue;}
  incomplete ||= q.incomplete;truncated ||= q.truncated;
  for(const p of q.paths){
   const target=signals.get(p.signal),path={...p,nodes:[...f.nodes,...p.nodes.slice(1)],edgeIds:[...f.edgeIds,...(p.edgeIds||[])]};
   if(!target){incomplete=true;continue;}
   const facing=forwardDirection(target)===p.arrivalDirection;
   if(stopsAtNoWay(target)){
    if(p.signal!==id&&!found.has(p.signal))found.set(p.signal,{...path,terminal:true});
   }else if(target.kind===0&&!facing){
    blocked.set(key(p.signal,p.arrivalDirection),{...path,reason:'sens unique pris à revers'});
   }else if(target.kind===4&&facing){
    if(p.signal!==id&&!found.has(p.signal))found.set(p.signal,path);
   }else{
    // Keep the arrival direction: selecting this signal's own forward direction
    // here would reverse the train instead of passing the opposite-facing sign.
    queue.push({id:p.signal,d:p.arrivalDirection,nodes:path.nodes,edgeIds:path.edgeIds});
   }
  }
 }
 const result={next:[...found.keys()],paths:[...found.values()],blocked:[...blocked.values()],incomplete,truncated};
 queryCache.set(cacheKey,result);return result;
}
function load(d){
 queryCache.clear();
 if(d.schema!=='nimby-technical-graph/1'||!['nodes','edges','tracks','stations','signals','junctions','queries'].every(k=>Array.isArray(d[k])))throw Error('Format JSON non reconnu.');
 if(d.nodes.length>100000||d.edges.length>200000)throw Error('Ce fichier dépasse la taille prise en charge par cette vue.');
 const ids=new Set(d.nodes.map(n=>n.id));
 const edgeIds=new Set(d.edges.map(e=>e.id));
 if(d.queries.some(q=>!Array.isArray(q.paths)||q.paths.some(p=>!Array.isArray(p.edgeIds)||p.edgeIds.length!==p.nodes.length-1||p.edgeIds.some(id=>!edgeIds.has(id)))))throw Error('Export ancien ou incomplet : régénère le JSON avec les identifiants exacts des liaisons.');
 if(d.nodes.some(n=>typeof n.id!=='string'||!Number.isFinite(n.x)||!Number.isFinite(n.y))||d.edges.some(e=>!ids.has(e.source)||!ids.has(e.target))||d.signals.some(s=>typeof s.id!=='string'||!ids.has(s.node)))throw Error('Nœuds ou références invalides.');
 data=d;nodes=new Map(d.nodes.map(n=>[n.id,n]));signals=new Map(d.signals.map(s=>[s.id,s]));tracks=new Map(d.tracks.map(t=>[t.id,t]));junctions=new Map(d.junctions.map(j=>[j.branch,j]));queries=new Map(d.queries.map(q=>[key(q.signal,q.direction),q]));stations=new Map(d.stations.map(s=>[s.id,s]));
 for(const s of signals.values()){
  const n=nodes.get(s.node);let nearest=null,dist=Infinity;
  for(const st of stations.values()){const dd=Math.hypot(st.x-n.x,st.y-n.y);if(dd<dist){dist=dd;nearest=st;}}
  s.nearestStation=nearest?.name||'Gare non identifiée';s.nearestStationId=nearest?.id;
 }
 $('signal-count').textContent=d.signals.length.toLocaleString('fr');$('junction-count').textContent=d.junctions.length.toLocaleString('fr');$('track-count').textContent=d.tracks.length.toLocaleString('fr');
 $('capture').textContent='Capture du '+new Date(d.capturedAt).toLocaleString('fr-FR',{day:'2-digit',month:'short',hour:'2-digit',minute:'2-digit'});
 $('proof').textContent=(d.validation?.sdkQueriesChecked||0).toLocaleString('fr')+' parcours comparés au SDK';
 $('station').replaceChildren(new Option('Tout le périmètre',''));
 [...stations.values()].sort((a,b)=>a.name.localeCompare(b.name)).forEach(s=>$('station').add(new Option(s.name,s.id)));
 $('data-notes').replaceChildren(...d.notes.map(t=>{const p=document.createElement('p');p.textContent=t;return p;}));
 selected=null;selectedSignalId=null;selectedEdge=null;mode='overview';limit=35;$('search').value='';scene=null;
 renderResults();renderInspector();setMode('overview');
}
function filteredSignals(){
 const query=$('search').value.trim().toLowerCase(),st=stations.get($('station').value);
 return [...signals.values()].filter(s=>{
  const n=nodes.get(s.node);
  if(st&&Math.hypot(n.x-st.x,n.y-st.y)>1800)return false;
  return !query||[s.id,s.decimalId,s.track,s.nearestStation,kinds[s.kind]].some(x=>String(x).toLowerCase().includes(query));
 }).sort((a,b)=>a.id.localeCompare(b.id));
}
function renderResults(){
 const all=filteredSignals();$('result-count').textContent=all.length+' signaux'+($('station').value?' à proximité':'');
 const host=$('results');host.replaceChildren();
 for(const s of all.slice(0,limit)){
  const b=document.createElement('button');b.className='result'+(selectedSignalId===s.id?' active':'');
  b.innerHTML='<i></i><span><code>'+esc(s.id)+'</code><small>'+esc(kinds[s.kind]||'Type '+s.kind)+' · '+esc(s.nearestStation)+'</small></span>';
  b.title=s.id+' · '+s.decimalId;b.onclick=()=>chooseSignal(s.id,true);host.append(b);
 }
 if(!all.length){const p=document.createElement('p');p.className='muted';p.textContent='Aucun signal ne correspond à cette recherche.';host.append(p);}
 $('more').hidden=all.length<=limit;
}
function selectedSignal(){return signals.get(selectedSignalId)||null;}
function chooseSignal(id,focus=true,arrival){
 const s=signals.get(id);if(!s)return;selected=s.node;selectedSignalId=id;selectedEdge=null;direction=forwardDirection(s);
 renderResults();renderInspector();buildScene();
 if(focus)setMode('route');else{updateViewTitle();requestDraw();}
}
function chooseNode(id){
 const n=nodes.get(id);if(!n)return;
 selected=id;selectedSignalId=n.signalIds[0]||null;selectedEdge=null;if(selectedSignal())direction=forwardDirection(selectedSignal());renderInspector();renderResults();
 if(mode==='overview'||n.signalIds.length)buildScene();
 updateViewTitle();
 if(n.signalIds.length&&mode==='route')fit();else requestDraw();
}
function renderInspector(){
 const n=nodes.get(selected),s=selectedSignal(),host=$('inspector');
 if(selectedEdge){
  const e=selectedEdge;host.innerHTML='<div class="detail"><span class="eyebrow">LIAISON OBSERVÉE</span><h2>'+esc(e.id)+'</h2><p>Chaîne de '+e.tracks.length+' tronçons natifs entre deux événements du réseau.</p><dl><dt>Départ</dt><dd><code>'+esc(e.source)+'</code></dd><dt>Arrivée</dt><dd><code>'+esc(e.target)+'</code></dd></dl><h3>TRONÇONS TRAVERSÉS</h3>'+e.tracks.map(t=>'<p><code>'+esc(t)+'</code></p>').join('')+'</div>';return;
 }
 if(!n){host.innerHTML='<div class="empty"><span>↖</span><h2>Choisis un signal</h2><p>Clique sur un symbole ou cherche son ID pour explorer ses connexions.</p><p>Les IDs affichés viennent de ta partie. Les états correspondent au moment de la capture.</p></div>';return;}
 if(s){
  const q=forwardQuery(s.id,forwardDirection(s)),state=s.textureState===null?'Indisponible':String(s.textureState)+(s.textureDefault?' · valeur par défaut':'');
  host.innerHTML='<div class="detail"><span class="eyebrow">SIGNAL · '+esc(kinds[s.kind]||s.kind)+'</span><h2>'+esc(s.id)+'</h2><p>À proximité de '+esc(s.nearestStation)+'</p><button class="text-button" id="copy-id">Copier l’ID</button><dl><dt>ID décimal</dt><dd><code>'+esc(s.decimalId)+'</code></dd><dt>Voie</dt><dd><code>'+esc(s.track)+'</code></dd><dt>Fraction</dt><dd>'+s.fraction.toFixed(8)+'</dd><dt>Sens natif</dt><dd>'+(s.direction>0?'+1':'−1')+'</dd><dt>État texture</dt><dd>'+esc(state)+'</dd></dl><p class="state-note">État brut du SDK, figé à la capture. Ce nombre ne suffit pas à interpréter l’autorisation de passage.</p><p class="state-note">Path et sens uniques : les balises sont travers&eacute;es ; un NoWay avec exceptions est travers&eacute;, un NoWay sans exception arr&ecirc;te la recherche. Un Path oppos&eacute; est travers&eacute; sans changer de sens. Ce graphe ne valide pas les autorisations de circulation.</p><button id="show-route" class="inspect-action">Voir le graphe de ce signal →</button><h3>PROCHAINS SIGNAUX · '+(q?.next.length??0)+'</h3><div id="next-list"></div>'+(q.blocked.length?'<h3>BRANCHES NON PROLONG&Eacute;ES</h3>'+q.blocked.map(p=>'<p class="warn"><code>'+esc(p.signal)+'</code> &middot; '+esc(p.reason)+'</p>').join(''):'')+(q?.incomplete?'<p class="warn">Au moins un chemin atteint une limite du périmètre ou une connexion non observée.</p>':'')+(q?.truncated?'<p class="warn">Recherche tronquée : limite de parcours atteinte.</p>':'')+'</div>';
  if(s.kind===5)host.innerHTML+='<div class="detail"><h3>EXCEPTIONS DU NOWAY</h3><p>'+ (Number.isInteger(s.exceptionCount)?s.exceptionCount+' exception(s). '+(hasExceptions(s)?'Passage dans ce graphe ; la compatibilit&eacute; avec un train particulier n’est pas &eacute;valu&eacute;e.':'Fin du parcours.'):'Information indisponible : fin du parcours par prudence.')+'</p></div>';
  $('copy-id').onclick=async()=>{try{await navigator.clipboard.writeText(s.id);$('copy-id').textContent='ID copié';}catch{$('copy-id').textContent='Sélectionne l’ID ci-dessus pour le copier';}};
  $('show-route').onclick=()=>setMode('route');
  for(const dest of q?.next||[]){const b=document.createElement('button');b.className='next-button';b.innerHTML=esc(dest)+'<span>→</span>';b.onclick=()=>chooseSignal(dest,true,q.paths.find(p=>p.signal===dest)?.arrivalDirection);$('next-list').append(b);}
  if(!q?.next.length){$('next-list').textContent='Aucun autre signal retrouvé dans ce sens.';$('next-list').className='muted';}
 }else if(n.kind==='junction'){
  host.innerHTML='<div class="detail junction-detail"><span class="eyebrow">RACCORDEMENT D’AIGUILLE</span><h2>'+esc(n.junctionIds.join(' / '))+'</h2><p>ID natif du tronçon de branche. Le SDK ne fournit pas d’ID distinct pour l’aiguille.</p>'+n.junctionIds.map(id=>{const j=junctions.get(id);return '<dl><dt>Branche</dt><dd><code>'+esc(j.branch)+'</code></dd><dt>Principale</dt><dd><code>'+esc(j.main)+'</code></dd><dt>Fraction</dt><dd>'+j.fraction.toFixed(8)+'</dd><dt>Approche</dt><dd>'+j.mainDirection+'</dd><dt>Branche</dt><dd>'+j.branchDirection+'</dd></dl>';}).join('')+'<p>Connexion native validée dans les deux sens. La position des lames et l’itinéraire réservé ne sont pas déduits.</p></div>';
 }else{
  host.innerHTML='<div class="detail"><span class="eyebrow">EXTRÉMITÉ DU GRAPHE</span><h2>'+esc(n.ports[0].track)+'</h2>'+n.boundaries.map(b=>'<p>'+(b.reason==='export_boundary'?'La voie continue en dehors du périmètre exporté.':'Le SDK n’observe pas de connexion à cette extrémité. Ce n’est pas une preuve de butoir.')+'</p><code>'+esc(b.outsideTrack)+'</code>').join('')+'</div>';
 }
}
// Alternate barycentre sweeps group connected branches instead of sorting IDs.
// This affects drawing only; IDs, direction and native edge references survive.
function layoutRoutes(ids,edges,levels){
 const columns=new Map(),incoming=new Map(),outgoing=new Map();
 for(const id of ids){const rank=levels.get(id)||0;if(!columns.has(rank))columns.set(rank,[]);columns.get(rank).push(id);incoming.set(id,[]);outgoing.set(id,[]);}
 for(const e of edges){incoming.get(e.target).push(e.source);outgoing.get(e.source).push(e.target);}
 const ranks=[...columns.keys()].sort((a,b)=>a-b),order=new Map();
 const indexRows=()=>{for(const rows of columns.values())rows.forEach((id,i)=>order.set(id,i-(rows.length-1)/2));};
 for(const rows of columns.values())rows.sort();indexRows();
 for(let pass=0;pass<8;pass++){
  const forward=pass%2===0;
  for(const rank of forward?ranks:[...ranks].reverse()){
   const rows=columns.get(rank),scores=new Map();
   for(const id of rows){const adjacent=(forward?incoming:outgoing).get(id).filter(n=>forward?(levels.get(n)||0)<rank:(levels.get(n)||0)>rank);
    scores.set(id,adjacent.length?adjacent.reduce((sum,n)=>sum+order.get(n),0)/adjacent.length:order.get(id));}
   rows.sort((a,b)=>scores.get(a)-scores.get(b)||order.get(a)-order.get(b));
   rows.forEach((id,i)=>order.set(id,i-(rows.length-1)/2));
  }
 }
 const positions=new Map();
 for(const [rank,rows] of columns)rows.forEach((id,i)=>positions.set(id,{x:rank*420,y:(i-(rows.length-1)/2)*130}));
 return positions;
}
function buildScene(){
 hover=null;
 const root=selectedSignal();if(!root){scene=null;return;}direction=forwardDirection(root);
 const depth=$('depth').value==='all'?Infinity:Number($('depth').value),all=new Set([root.node]),physicalEdges=new Set(),edgeOrigins=new Map(),activeSignals=new Set([root.id]),pairs=new Map(),levels=new Map([[root.node,0]]),front=[{id:root.id,d:direction,step:0}],visited=new Set(),blocked=new Set(),incomplete=new Set();let cut=false;
 for(let at=0;at<front.length;at++){
  const f=front[at],k=key(f.id,f.d);if(visited.has(k)||f.step>=depth)continue;visited.add(k);
  const q=forwardQuery(f.id,f.d);
  for(const p of q.blocked)blocked.add(key(p.signal,p.arrivalDirection));
  if(q.incomplete)incomplete.add(f.id);
  cut ||= q.truncated;
  for(const path of q.paths){
   if(all.size+new Set(path.nodes.filter(n=>!all.has(n))).size>2000){cut=true;continue;}
   activeSignals.add(path.signal);
   for(const [i,id] of (path.edgeIds||[]).entries()){
    physicalEdges.add(id);
    if(!edgeOrigins.has(id))edgeOrigins.set(id,new Set());
    edgeOrigins.get(id).add(path.nodes[i]);
   }
   for(let i=0;i<path.nodes.length;i++){
    const id=path.nodes[i];all.add(id);
    if(i){const prev=path.nodes[i-1];pairs.set(prev+'>'+id,{source:prev,target:id});if(!levels.has(id))levels.set(id,(levels.get(prev)||0)+1);}
   }
   if(!path.terminal&&f.step+1<depth)front.push({id:path.signal,d:path.arrivalDirection,step:f.step+1});
  }
 }
 const positions=layoutRoutes(all,[...pairs.values()],levels);
 const outerTop=Math.min(...[...positions.values()].map(p=>p.y))-110;
 let lane=0;
 for(const e of pairs.values()){
  const a=positions.get(e.source),b=positions.get(e.target);
  if(b.x<=a.x||b.x-a.x>420)e.outerY=outerTop-(lane++)*24;
 }

 scene={nodes:all,activeSignals,physicalEdges,edgeOrigins,edges:[...pairs.values()],positions,cut,blocked,incomplete,processed:visited.size};
}
function setMode(next){
 if(next==='route'&&!selectedSignal()){
  const st=[...stations.values()].find(s=>/part dieu/i.test(s.name));
  const candidates=[...signals.values()].filter(s=>s.kind===4);
  candidates.sort((a,b)=>{const na=nodes.get(a.node),nb=nodes.get(b.node);return st?Math.hypot(na.x-st.x,na.y-st.y)-Math.hypot(nb.x-st.x,nb.y-st.y):a.id.localeCompare(b.id);});
  const s=candidates.find(s=>forwardQuery(s.id,forwardDirection(s)).next.length>1)||candidates[0];
  if(!s)return;selected=s.node;selectedSignalId=s.id;direction=forwardDirection(s);renderInspector();renderResults();
 }
 mode=next;$('overview').classList.toggle('active',mode==='overview');$('route').classList.toggle('active',mode==='route');$('depth-label').hidden=false;
 buildScene();updateViewTitle();fit();
}
function updateViewTitle(){
 const s=selectedSignal();$('view-kicker').textContent=mode==='overview'?'VUE D’ENSEMBLE':'PARCOURS TECHNIQUE';
 $('view-title').textContent=mode==='overview'?'Les connexions de l’étoile lyonnaise':s?.id||'Parcours';
 $('view-subtitle').textContent=scene?'Chaînage Path · '+scene.activeSignals.size+' signaux · '+scene.blocked.size+' branches arrêtées par un sens unique':'Positions schématiques · Path et respect des sens uniques';
 $('notice').hidden=!(scene?.cut||scene?.incomplete.size);$('notice').textContent=scene?.cut?'Vue tronquée. Réduis le chaînage pour inspecter chaque branche.':scene?.incomplete.size+' recherches atteignent une limite ou une connexion non observée.';
 $('footer-count').textContent=mode==='overview'?data.nodes.length+' événements · '+data.edges.length+' liaisons':(scene?.nodes.size||0)+' événements · '+(scene?.edges.length||0)+' liaisons';
}
function worldPosition(n){return mode==='route'?scene?.positions.get(n.id):{x:n.x,y:-n.y};}
function fit(){
 const ps=mode==='route'&&scene?[...scene.positions.values()]:data.nodes.map(n=>({x:n.x,y:-n.y}));if(!ps.length)return;
 let minx=Infinity,maxx=-Infinity,miny=Infinity,maxy=-Infinity;for(const p of ps){minx=Math.min(minx,p.x);maxx=Math.max(maxx,p.x);miny=Math.min(miny,p.y);maxy=Math.max(maxy,p.y);}
 if(mode==='route')for(const e of scene.edges)if(e.outerY!==undefined)miny=Math.min(miny,e.outerY);
 const margin=mode==='route'?280:800;
 camera={x:(minx+maxx)/2,y:(miny+maxy)/2,scale:Math.min((size.w-80)/(maxx-minx+margin),(size.h-160)/(maxy-miny+margin))};
 if(mode==='route'&&camera.scale<.58){camera.scale=.58;camera.x=(size.w/2-170)/camera.scale;camera.y=0;}
 updateViewTitle();requestDraw();
}
function screen(p){return {x:(p.x-camera.x)*camera.scale+size.w/2,y:(p.y-camera.y)*camera.scale+size.h/2};}
function requestDraw(){if(!raf)raf=requestAnimationFrame(()=>{raf=0;draw();});}
function arrow(a,b,color){const angle=Math.atan2(b.y-a.y,b.x-a.x);ctx.fillStyle=color;ctx.beginPath();ctx.moveTo(b.x,b.y);ctx.lineTo(b.x-7*Math.cos(angle-.45),b.y-7*Math.sin(angle-.45));ctx.lineTo(b.x-7*Math.cos(angle+.45),b.y-7*Math.sin(angle+.45));ctx.closePath();ctx.fill();}
function mapArrows(points,offset=0){
 const lengths=points.slice(1).map((p,i)=>Math.hypot(p.x-points[i].x,p.y-points[i].y));
 const total=lengths.reduce((a,b)=>a+b,0);if(total<24)return;
 let travelled=0,next=Math.min(45,total/2);
 for(let i=0;i<lengths.length;i++){
  const length=lengths[i];if(length<.01)continue;
  const a=points[i],b=points[i+1],ux=(b.x-a.x)/length,uy=(b.y-a.y)/length;
  for(;next<=travelled+length;next+=90){
   const along=next-travelled,p={x:a.x+ux*along-uy*offset,y:a.y+uy*along+ux*offset};
   if(p.x<0||p.y<0||p.x>size.w||p.y>size.h)continue;
   // Dark backing separates the arrowhead from its highlighted track.
   ctx.strokeStyle='#0b1521';ctx.lineWidth=5;ctx.beginPath();ctx.moveTo(p.x-ux*8,p.y-uy*8);ctx.lineTo(p.x,p.y);ctx.stroke();
   arrow({x:p.x-ux*9,y:p.y-uy*9},p,'#e5fff5');
  }
  travelled+=length;
 }
}
function draw(){
 if(!data)return;const ratio=window.devicePixelRatio||1;ctx.setTransform(ratio,0,0,ratio,0,0);ctx.clearRect(0,0,size.w,size.h);
 ctx.fillStyle='#213145';for(let x=22;x<size.w;x+=28)for(let y=18;y<size.h;y+=28)ctx.fillRect(x,y,1,1);
 const highlighted=new Set(scene?.nodes||[]);
 const isHighlighted=e=>scene?.physicalEdges.has(e.id)||(!scene&&selected&&(e.source===selected||e.target===selected));
 // Draw highlighted connections last so neighbouring tracks cannot hide them.
 const drawn=mode==='route'&&scene?scene.edges:[...data.edges].sort((a,b)=>Number(isHighlighted(a))-Number(isHighlighted(b)));
 for(const e of drawn){
  const an=nodes.get(e.source),bn=nodes.get(e.target),aa=worldPosition(an),bb=worldPosition(bn);if(!aa||!bb)continue;
  const a=screen(aa),b=screen(bb);const active=mode==='route'||isHighlighted(e);
  ctx.strokeStyle=e===selectedEdge?'#fff':active?'#81e6c2':scene?'#29394b':'#41566c';ctx.lineWidth=active?(mode==='overview'?3.5:1.9):1.05;ctx.beginPath();
  if(mode==='route'){
   const w=128*camera.scale,h=31*camera.scale;
   const focused=!hover||e.source===hover.id||e.target===hover.id;
   ctx.globalAlpha=focused?1:.13;
   const color=hover&&focused?'#ffffff':'#81e6c2';ctx.strokeStyle=color;
   const from={x:a.x+w,y:a.y},to={x:b.x-w,y:b.y};
   ctx.moveTo(from.x,from.y);
   if(e.outerY!==undefined){
    const top=screen({x:0,y:e.outerY}).y;
    ctx.lineTo(from.x+30*camera.scale,from.y);
    ctx.lineTo(from.x+30*camera.scale,top);
    ctx.lineTo(to.x-30*camera.scale,top);
    ctx.lineTo(to.x-30*camera.scale,to.y);
    ctx.lineTo(to.x,to.y);
   }else{
    const mid=(from.x+to.x)/2;ctx.bezierCurveTo(mid,from.y,mid,to.y,to.x,to.y);
   }
   ctx.stroke();arrow({x:to.x-12,y:to.y},to,color);ctx.globalAlpha=1;
  }else{
   const points=[a,...(e.points||[]).slice(1,-1).map(p=>screen({x:p[0],y:-p[1]})),b];
   ctx.moveTo(a.x,a.y);for(const p of points.slice(1))ctx.lineTo(p.x,p.y);ctx.stroke();
   if(active){
    const origins=scene?.edgeOrigins.get(e.id);
    if(origins?.has(e.source))mapArrows(points,origins.size>1?4:0);
    if(origins?.has(e.target))mapArrows([...points].reverse(),origins.size>1?4:0);
   }
  }
 }
 lastHits=[];
 const drawnNodes=mode==='route'&&scene?[...scene.nodes].map(id=>nodes.get(id)):data.nodes;
 for(const n of drawnNodes){
  const pos=worldPosition(n);if(!pos)continue;const p=screen(pos);if(p.x<-200||p.y<-80||p.x>size.w+200||p.y>size.h+80)continue;
  const active=n.id===selected,onPath=highlighted.has(n.id),reverseSignal=onPath&&n.signalIds.length&&!n.signalIds.some(id=>scene?.activeSignals.has(id)),color=reverseSignal?'#8191a5':mode==='overview'&&onPath?'#81e6c2':colors[n.kind]||colors.signal;
  if(mode==='route'){
   const w=256*camera.scale,h=62*camera.scale;ctx.fillStyle=active?'#1c3b4b':'#142637';ctx.strokeStyle=active?'#81e6c2':color;ctx.lineWidth=active?2:1;ctx.beginPath();ctx.roundRect(p.x-w/2,p.y-h/2,w,h,6*camera.scale);ctx.fill();ctx.stroke();
   ctx.fillStyle=color;ctx.font=`${10*camera.scale}px system-ui`;ctx.textAlign='left';ctx.fillText(n.kind==='junction'?'AIGUILLE · ID DE BRANCHE':n.signalIds.length&&stopsAtNoWay(signals.get(n.signalIds[0]))?'FIN DU PARCOURS':n.signalIds.length&&signals.get(n.signalIds[0])?.kind===5?'NO WAY AVEC EXCEPTIONS':n.signalIds.length&&signals.get(n.signalIds[0])?.kind===0?'SENS UNIQUE':n.signalIds.length&&signals.get(n.signalIds[0])?.kind!==4?'AUTRE TYPE · IGNORÉ':reverseSignal?'PATH · SENS OPPOSÉ':n.kind==='signal'?'SIGNAL · Path':'EXTRÉMITÉ',p.x-w/2+12*camera.scale,p.y-9*camera.scale);
   ctx.fillStyle='#e1edf8';ctx.font=`${13*camera.scale}px Consolas,monospace`;ctx.fillText(n.signalIds[0]||n.junctionIds[0]||n.ports[0].track,p.x-w/2+12*camera.scale,p.y+13*camera.scale);
   lastHits.push({n,p,w:w/2,h:h/2});
  }else{
   const r=active?6:onPath?4:n.kind==='signal'?3.1:2.2;ctx.fillStyle=color;
   if(active){ctx.strokeStyle='#81e6c2';ctx.lineWidth=2;ctx.beginPath();ctx.arc(p.x,p.y,11,0,Math.PI*2);ctx.stroke();}
   if(n.kind==='junction'){ctx.beginPath();ctx.moveTo(p.x,p.y-r-1);ctx.lineTo(p.x+r+1,p.y);ctx.lineTo(p.x,p.y+r+1);ctx.lineTo(p.x-r-1,p.y);ctx.closePath();ctx.fill();}
   else if(n.kind==='boundary')ctx.fillRect(p.x-r,p.y-r,2*r,2*r);else{ctx.beginPath();ctx.arc(p.x,p.y,r,0,Math.PI*2);ctx.fill();}
   if(active||$('labels').checked||camera.scale>.5){const label=(active&&selectedSignalId)||n.signalIds[0]||n.junctionIds[0]||n.ports[0].track;ctx.font='10px Consolas,monospace';ctx.textAlign='left';const width=ctx.measureText(label).width;ctx.fillStyle='#0b1521ec';ctx.fillRect(p.x+7,p.y-11,width+5,14);ctx.fillStyle=active?'#81e6c2':color;ctx.fillText(label,p.x+9,p.y);}
   lastHits.push({n,p,w:7,h:7});
  }
 }
 if(mode==='overview'){
  const occupied=[];ctx.textAlign='left';ctx.font='12px system-ui';
  for(const st of data.stations){const p=screen({x:st.x,y:-st.y});if(p.x<0||p.y<95||p.x>size.w||p.y>size.h-80)continue;if(occupied.some(q=>Math.abs(q.x-p.x)<100&&Math.abs(q.y-p.y)<24))continue;occupied.push(p);ctx.strokeStyle='#0b1521';ctx.lineWidth=5;ctx.strokeText(st.name,p.x+9,p.y-14);ctx.fillStyle='#d5e0ed';ctx.fillText(st.name,p.x+9,p.y-14);}
 }
}
function hit(x,y){let best=null,dist=Infinity;for(const h of lastHits){const dx=Math.abs(h.p.x-x),dy=Math.abs(h.p.y-y);if(dx<=h.w&&dy<=h.h&&dx*dx+dy*dy<dist){best=h.n;dist=dx*dx+dy*dy;}}return best;}
function zoom(factor,x=size.w/2,y=size.h/2){const before={x:(x-size.w/2)/camera.scale+camera.x,y:(y-size.h/2)/camera.scale+camera.y};camera.scale=Math.max(mode==='route'?.12:.005,Math.min(mode==='route'?3:30,camera.scale*factor));camera.x=before.x-(x-size.w/2)/camera.scale;camera.y=before.y-(y-size.h/2)/camera.scale;requestDraw();}
canvas.addEventListener('wheel',e=>{e.preventDefault();const r=canvas.getBoundingClientRect();zoom(Math.exp(-e.deltaY*.0018),e.clientX-r.left,e.clientY-r.top);},{passive:false});
canvas.onpointerdown=e=>{canvas.setPointerCapture(e.pointerId);drag={x:e.clientX,y:e.clientY,cx:camera.x,cy:camera.y,moved:false};$('tooltip').hidden=true;};
canvas.onpointermove=e=>{const r=canvas.getBoundingClientRect(),x=e.clientX-r.left,y=e.clientY-r.top;if(drag){const dx=e.clientX-drag.x,dy=e.clientY-drag.y;drag.moved ||= Math.hypot(dx,dy)>4;camera.x=drag.cx-dx/camera.scale;camera.y=drag.cy-dy/camera.scale;requestDraw();return;}const n=hit(x,y);if(hover!==n){hover=n;requestDraw();}canvas.style.cursor=n?'pointer':'grab';$('tooltip').hidden=!n;if(n){$('tooltip').textContent=(n.kind==='junction'?'Aiguille · branche native\n':n.kind==='signal'?'Signal\n':'Extrémité\n')+(n.signalIds[0]||n.junctionIds[0]||n.ports[0].track);$('tooltip').style.left=Math.min(x+18,size.w-290)+'px';$('tooltip').style.top=Math.min(y+18,size.h-90)+'px';}};
canvas.onpointerup=e=>{if(drag&&!drag.moved){const r=canvas.getBoundingClientRect(),n=hit(e.clientX-r.left,e.clientY-r.top);if(n)chooseNode(n.id);}drag=null;};canvas.onpointercancel=()=>drag=null;canvas.onpointerleave=()=>{$('tooltip').hidden=true;hover=null;requestDraw();};
canvas.onkeydown=e=>{if(e.key==='+'||e.key==='='){zoom(1.3);e.preventDefault();}if(e.key==='-'){zoom(1/1.3);e.preventDefault();}if(e.key==='0'){fit();e.preventDefault();}};
new ResizeObserver(()=>{const r=$('viewport').getBoundingClientRect();size={w:r.width,h:r.height};const dpr=window.devicePixelRatio||1;canvas.width=Math.round(size.w*dpr);canvas.height=Math.round(size.h*dpr);requestDraw();}).observe($('viewport'));
$('search').oninput=()=>{limit=35;renderResults();};$('search').onkeydown=e=>{if(e.key==='Enter'){const s=filteredSignals()[0];if(s)chooseSignal(s.id,true);}};
$('station').onchange=()=>{limit=35;renderResults();const st=stations.get($('station').value);if(st){if(mode!=='overview')setMode('overview');camera={x:st.x,y:-st.y,scale:Math.min(size.w,size.h)/3600};requestDraw();}};
$('clear').onclick=()=>{$('search').value='';$('station').value='';limit=35;renderResults();};$('more').onclick=()=>{limit+=50;renderResults();};$('overview').onclick=()=>setMode('overview');$('route').onclick=()=>setMode('route');$('fit').onclick=fit;$('zoom-in').onclick=()=>zoom(1.4);$('zoom-out').onclick=()=>zoom(1/1.4);$('labels').onchange=requestDraw;$('depth').onchange=()=>{buildScene();updateViewTitle();if(mode==='route')fit();else requestDraw();};
$('about').onclick=()=>$('data-dialog').showModal();$('close-dialog').onclick=()=>$('data-dialog').close();
$('download').onclick=()=>{const blob=new Blob([JSON.stringify(data,null,2)],{type:'application/json'}),url=URL.createObjectURL(blob),a=document.createElement('a');a.href=url;a.download='etoile-lyonnaise-network.json';a.click();setTimeout(()=>URL.revokeObjectURL(url),1000);};
$('upload').onchange=async e=>{const file=e.target.files[0];if(!file)return;try{if(file.size>50e6)throw Error('Fichier trop volumineux (50 Mo maximum).');load(JSON.parse(await file.text()));}catch(err){alert('Impossible d’ouvrir le graphe : '+err.message);}e.target.value='';};
function openLinkedExample(){
 const params=new URLSearchParams(window.location.search),id=params.get('signal')||params.get('node');
 const depth=params.get('depth');if(['1','2','3','4','8','all'].includes(depth))$('depth').value=depth;
 if(signals.has(id))chooseSignal(id,params.get('mode')==='route');
 else if(nodes.has(id))chooseNode(id);
 else {fit();return;}
 if(mode==='overview'){
  const n=nodes.get(selected);camera={x:n.x,y:-n.y,scale:n.kind==='junction'?1.5:.3};requestDraw();
 }
}
try{load(window.NETWORK_DATA);requestAnimationFrame(openLinkedExample);}catch(e){$('view-title').textContent='Chargement impossible';$('view-subtitle').textContent=e.message;}
// Read-only diagnostics for reproducible local UI checks.
window.graphDiagnostics=()=>({mode,selected,direction,nodes:scene?.nodes.size||data.nodes.length,edges:scene?.edges.length||data.edges.length,camera:{...camera},validation:data.validation});
