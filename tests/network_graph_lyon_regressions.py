"""Optional regression cases for the Lyon fixture; never used to build geometry."""
def crossover_edge(a, b):
    return next(e for e in g['edges'] if {e['source'],e['target']}=={'J:'+a,'J:'+b})

crossovers = [crossover_edge('0x1000256330001','0x1000256340001'),
              crossover_edge('0x1000256350001','0x1000256360001')]
def side(a,b,p):
    return (b[0]-a[0])*(p[1]-a[1])-(b[1]-a[1])*(p[0]-a[0])
a,b = crossovers[0]['points'][0],crossovers[0]['points'][-1]
c,d = crossovers[1]['points'][0],crossovers[1]['points'][-1]
if not (side(a,b,c)*side(a,b,d)<0 and side(c,d,a)*side(c,d,b)<0):
    errors.append({'check':'Brignais crossover forms X'})
for edge in crossovers:
    a,b = edge['points'][0],edge['points'][-1]
    if any(abs(side(a,b,p))>1e-5 for p in edge['points']):
        errors.append({'check':'Brignais crossover has no displaced midpoint', 'edge':edge['id']})

# Check the requested Saint-Fons -> Venissieux corridor over oriented ports,
# preserving the same OneWay and NoWay boundaries as the viewer.
starts = [t for t in g['tracks'].values() if t['station']=='0x2000000d00004']
queue = collections.deque([((t['id'],f),d) for t in starts for f,d in [(0.,1),(1.,-1)]])
visited = set()
reached_stations = set()
while queue:
    event,d = queue.popleft()
    if (event,d) in visited:
        continue
    visited.add((event,d))
    reached_stations.add(g['tracks'][event[0]]['station'])
    encountered = [signals[sid] for sid in g['signal_events'].get(event,[])]
    if any(g['stops_at_no_way'](s) or (s['kind']==0 and forward(s)!=d) for s in encountered):
        continue
    queue.extend(transitions(event,d))
report['saintFonsConnections'] = {'venissieuxBv':'0x2000008a80001' in reached_stations,
                                 'venissieuxTriage':'0x2000000070002' in reached_stations}
def station_connection(source, target):
    queue = collections.deque([((t['id'],f),d) for t in g['tracks'].values()
                               if t['station']==source for f,d in [(0.,1),(1.,-1)]])
    parents = {state:None for state in queue}
    while queue:
        state = queue.popleft()
        event,d = state
        if g['tracks'][event[0]]['station']==target:
            path = []
            while state is not None:
                path.append(state)
                state = parents[state]
            path.reverse()
            track_ids = list(dict.fromkeys(e[0] for e,d in path))
            return {'reachable':True, 'tracks':track_ids,
                    'signals':[sid for e,d in path for sid in g['signal_events'].get(e,[])],
                    'minimumX':min(g['tracks'][tid]['x'] for tid in track_ids)}
        encountered = [signals[sid] for sid in g['signal_events'].get(event,[])]
        if any(g['stops_at_no_way'](s) or (s['kind']==0 and forward(s)!=d) for s in encountered):
            continue
        for next_state in transitions(event,d):
            if next_state not in parents:
                parents[next_state] = state
                queue.append(next_state)
    return {'reachable':False}

report['vaisePerracheConnections'] = {
    'vaiseToPerrache':station_connection('0x2000000d40001','0x20000038d0001'),
    'perracheToVaise':station_connection('0x20000038d0001','0x2000000d40001')}
for direction, connection in report['vaisePerracheConnections'].items():
    if not connection['reachable']:
        errors.append({'check':'Vaise-Perrache corridor', 'direction':direction})
west_query = next(q for q in g['forward_queries'] if q['signal']=='0x8000061ea0001')
report['westBranchConnection'] = west_query
if west_query['incomplete'] or '0x8000039e80001' not in west_query['next']:
    errors.append({'check':'west branch reaches next Path', 'signal':west_query['signal']})
