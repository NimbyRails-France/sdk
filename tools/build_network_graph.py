"""Build a bounded, offline technical graph from export_network_graph JSON.
No proximity-based edges. Every signal query is checked against the C++ SDK.
"""
import collections
import datetime
import hashlib
import json
import pathlib
import shutil
import sys

source = pathlib.Path(sys.argv[1])
output = pathlib.Path(sys.argv[2])
raw = json.loads(source.read_text(encoding="utf-8"))
xmin, ymin, xmax, ymax = raw["regionBounds"]
all_tracks = {t["id"]: t for t in raw["tracks"]}
tracks = {i: t for i, t in all_tracks.items() if xmin <= t["x"] <= xmax and ymin <= t["y"] <= ymax}
signals = {s["id"]: s for s in raw["signals"] if s["track"] in tracks}
junctions = [j for j in raw["junctions"] if j["main"] in tracks and j["branch"] in tracks]
attachment = {j["branch"]: j for j in junctions}
forks = collections.defaultdict(list)
fractions = {t: {0., 1.} for t in tracks}
signal_events = collections.defaultdict(list)
for s in signals.values():
    fractions[s["track"]].add(s["fraction"])
    signal_events[(s["track"], s["fraction"])].append(s["id"])
for j in junctions:
    fractions[j["main"]].add(j["fraction"])
    forks[(j["main"], j["fraction"])].append(j)
fractions = {t: sorted(fs) for t, fs in fractions.items()}

def transitions(event, direction):
    track, f = event
    for j in forks.get(event, []):
        if j["mainDirection"] == direction:
            yield (j["branch"], 0. if j["branchDirection"] == 1 else 1.), j["branchDirection"]
    fs = fractions[track]
    i = fs.index(f) + direction
    if 0 <= i < len(fs):
        yield (track, fs[i]), direction
        return
    neighbor = tracks[track]["b" if direction == 1 else "a"]
    if neighbor in tracks:
        t = tracks[neighbor]
        a, b = t["a"] == track, t["b"] == track
        if a != b:
            yield (neighbor, 0. if a else 1.), 1 if a else -1
    elif neighbor == "0x0" and track in attachment:
        j = attachment[track]
        if direction == -j["branchDirection"]:
            yield (j["main"], j["fraction"]), -j["mainDirection"]

# Merge only two ports of the same native attachment, never nearby coordinates.
parent = {}
def find(e):
    parent.setdefault(e, e)
    if parent[e] != e:
        parent[e] = find(parent[e])
    return parent[e]
def union(a, b):
    parent[find(b)] = find(a)
for e in signal_events:
    find(e)
for j in junctions:
    union((j["main"], j["fraction"]), (j["branch"], 0. if j["branchDirection"] == 1 else 1.))
boundaries = {}
for t in tracks:
    for f, d in [(0., -1), (1., 1)]:
        if not list(transitions((t, f), d)):
            find((t, f))
            n = tracks[t]["b" if d == 1 else "a"]
            boundaries[(t, f)] = {"reason": "export_boundary" if n in all_tracks and n not in tracks else "unobserved_connection", "outsideTrack": n}

from network_geometry import make_point_resolver
point = make_point_resolver(all_tracks, raw["junctions"])

groups = collections.defaultdict(list)
for e in parent:
    groups[find(e)].append(e)
nodes, event_node, node_by_id = [], {}, {}
for root, events in sorted(groups.items()):
    sigs = sorted({sid for e in events for sid in signal_events.get(e, [])})
    js = [j for j in junctions if find((j["main"], j["fraction"])) == root]
    kind = "junction" if js else "signal" if sigs else "boundary"
    key = "J:" + js[0]["branch"] if js else "S:" + sigs[0] if sigs else "B:" + root[0] + (":A" if root[1] == 0 else ":B")
    xy = point(*root)
    n = {"id": key, "kind": kind, "signalIds": sigs, "junctionIds": [j["branch"] for j in js], "x": xy[0], "y": xy[1],
         "ports": [{"track": t, "fraction": f} for t, f in events], "boundaries": [boundaries[e] for e in events if e in boundaries]}
    for e in events:
        event_node[e] = key
    for sid in sigs:
        signals[sid]["node"] = key
    node_by_id[key] = n
    nodes.append(n)

# Compress unmarked degree-two chain points, keeping the full native track list.
edge_map = {}
for n in nodes:
    for port in n["ports"]:
        start = (port["track"], port["fraction"])
        for d in [-1, 1]:
            pending = [(e, nd, [start[0]], [point(*start)]) for e, nd in transitions(start, d)]
            seen = set()
            while pending:
                e, nd, chain, points = pending.pop()
                if (e, nd) in seen:
                    continue
                seen.add((e, nd))
                if e[0] != chain[-1]:
                    chain = chain + [e[0]]
                xy = point(*e)
                if xy != points[-1]:
                    points = points + [xy]
                dest = event_node.get(e)
                if dest == n["id"]:
                    # Internal transition between the two ports of this very
                    # attachment is not a second physical edge leaving it.
                    continue
                if dest and dest != n["id"]:
                    # Canonical physical edge; retain both observed orientations.
                    forward = (n["id"], dest, tuple(chain))
                    reverse = (dest, n["id"], tuple(reversed(chain)))
                    canonical = min(forward, reverse)
                    if canonical not in edge_map:
                        edge_map[canonical] = {"id": "E:" + str(len(edge_map)), "source": n["id"], "target": dest,
                            "tracks": chain, "points": points + [point(*e)], "directions": []}
                    orientation = {"from": n["id"], "fromTrack": start[0], "fromDirection": d, "toTrack": e[0], "toDirection": nd}
                    if orientation not in edge_map[canonical]["directions"]:
                        edge_map[canonical]["directions"].append(orientation)
                    continue
                for ne, nn in transitions(e, nd):
                    pending.append((ne, nn, chain, points))
edges = list(edge_map.values())
edge_lookup = {}
for edge in edges:
    edge_lookup[(edge['source'],edge['target'],tuple(edge['tracks']))] = edge['id']
    edge_lookup[(edge['target'],edge['source'],tuple(reversed(edge['tracks'])))] = edge['id']

def path_edges(events):
    previous = event_node[events[0]]
    chain = [events[0][0]]
    result = []
    for event in events[1:]:
        if event[0] != chain[-1]:
            chain.append(event[0])
        marker = event_node.get(event)
        if marker:
            if marker != previous:
                edge = edge_lookup.get((previous,marker,tuple(chain)))
                if edge is None:
                    raise RuntimeError(f'No exact physical edge: {previous} -> {marker}: {chain}')
                result.append(edge)
            previous = marker
            chain = [event[0]]
    return result

# Independent graph walk, matched for EVERY exported query against the SDK.
# One representative path per destination is retained, not an enumeration of routes.
queries = []
for q in raw["nextSignals"]:
    origin = signals[q["signal"]]
    start = (origin["track"], origin["fraction"])
    pending = collections.deque([(start, q["direction"], [origin["node"]], [start])])
    seen, found = set(), {}
    while pending:
        e, d, path, events = pending.popleft()
        if (e, d) in seen:
            continue
        seen.add((e, d))
        marker = event_node.get(e)
        if marker and marker != path[-1]:
            path = path + [marker]
        hits = [i for i in signal_events.get(e, []) if i != origin["id"]]
        if hits:
            for sid in hits:
                found.setdefault(sid, {"signal": sid, "arrivalDirection": d, "nodes": path, "edgeIds":path_edges(events)})
            continue
        for ne, nd in transitions(e, d):
            pending.append((ne, nd, path, events + [ne]))
    if set(found) != set(q["next"]):
        raise RuntimeError(f"SDK mismatch: {q['signal']} direction {q['direction']}: {set(found)} != {set(q['next'])}")
    queries.append(dict(q, paths=list(found.values())))

edge_pairs = {frozenset((e["source"],e["target"])) for e in edges}
for q in queries:
    for path in q["paths"]:
        for a,b in zip(path["nodes"],path["nodes"][1:]):
            if frozenset((a,b)) not in edge_pairs:
                raise RuntimeError(f"Missing drawn connection {a} -> {b}")

# Independent Path walk: stop at a facing Path or a reversed OneWay.
# NoWay without known exceptions is terminal; balises are passed.
forward_queries = []
def stops_at_no_way(signal):
    count = signal.get('exceptionCount')
    return signal['kind'] == 5 and not (type(count) is int and count > 0)
for origin in signals.values():
    direction = -origin["direction"] if origin["kind"] == 4 else origin["direction"]
    pending = collections.deque([((origin["track"], origin["fraction"]), direction, [origin["node"]])])
    seen, found = set(), {}
    incomplete = False
    while pending and not stops_at_no_way(origin):
        e, d, path = pending.popleft()
        if (e, d) in seen:
            continue
        seen.add((e, d))
        marker = event_node.get(e)
        if marker and marker != path[-1]:
            path = path + [marker]
        hits = [sid for sid in signal_events.get(e, []) if sid != origin["id"]
                ]
        stop = False
        for sid in hits:
            target = signals[sid]
            forward = -target["direction"] if target["kind"] == 4 else target["direction"]
            if stops_at_no_way(target):
                found.setdefault(sid, {"signal":sid, "arrivalDirection":d, "nodes":path, "terminal":True})
                stop = True
            elif target["kind"] == 0 and forward != d:
                stop = True
            elif target["kind"] == 4 and forward == d:
                found.setdefault(sid, {"signal": sid, "arrivalDirection": d, "nodes": path})
                stop = True
        if stop:
            continue
        successors = list(transitions(e, d))
        if not successors:
            incomplete = True
        pending.extend((ne, nd, path) for ne, nd in successors)
    forward_queries.append({"signal":origin["id"], "direction":direction, "next":sorted(found),
                            "paths":list(found.values()), "incomplete":incomplete, "truncated":False})

station_tracks = collections.defaultdict(list)
for t in tracks.values():
    station_tracks[t["station"]].append(t)
stations = []
for st in raw["stations"]:
    ts = station_tracks.get(st["id"])
    if ts:
        stations.append(dict(st, x=sum(t["x"] for t in ts)/len(ts), y=sum(t["y"] for t in ts)/len(ts), tracks=[t["id"] for t in ts]))
capture = datetime.datetime.fromtimestamp(raw["capturedUnixMs"]/1000, datetime.timezone.utc).isoformat()
data = {"schema": "nimby-technical-graph/1", "title": "Étoile lyonnaise", "capturedAt": capture,
        "source": raw["source"], "gameSha256": raw["gameSha256"], "processId": raw["processId"],
        "bounds": raw["regionBounds"], "scope": "Ouest lyonnais (Tassin, Lentilly) · Vaise · Perrache · Vénissieux · Saint-Fons · Saint-Priest · Collonges-au-Mont-d’Or · Sathonay–Rillieux",
        "notes": ["Capture figée, non atomique. Les états ne sont pas actualisés en direct.",
            "Connexions natives uniquement ; aucune liaison créée par proximité.",
            "Positions de dessin approximatives. Distances et courbures non contractuelles.",
            "Les ID de signal et de voie sont natifs. J: utilise l’ID natif du tronçon de branche, pas un ID d’aiguille du moteur.",
            "Les chemins représentent des possibilités sans retournement, pas un itinéraire réservé ni une autorisation de passage.",
            "Les flèches sont des directions de parcours. Le sens natif d’un signal Path n’est pas une autorisation de passage.",
            "Une coupure au bord de l’export est distincte d’une connexion non observée."],
        "validation": {"sdkQueriesChecked": len(queries), "mismatches": 0, "truncated": sum(q["truncated"] for q in queries)},
        "stations": stations, "tracks": list(tracks.values()), "signals": list(signals.values()), "junctions": junctions,
        "nodes": nodes, "edges": edges, "queries": queries, "forwardQueries": forward_queries}
output.mkdir(parents=True, exist_ok=True)
text = json.dumps(data, ensure_ascii=False, separators=(",", ":"))
(output/"network.json").write_text(text, encoding="utf-8")
(output/"network-data.js").write_text("window.NETWORK_DATA=" + text.replace("</", "<\\/") + ";\n", encoding="utf-8")
template = pathlib.Path(__file__).parent/"network-graph"
if template.exists():
    for f in template.iterdir():
        if f.is_file():
            shutil.copyfile(f, output/f.name)
    # A refreshed HTML page must request the matching data and renderer,
    # even when a browser still has an older export in its asset cache.
    page = output/'index.html'
    html = page.read_text(encoding='utf-8')
    for asset in ('network-data.js', 'app.js', 'style.css'):
        revision = hashlib.sha256((output/asset).read_bytes()).hexdigest()[:16]
        html = html.replace('"'+asset+'"', '"'+asset+'?v='+revision+'"')
    page.write_text(html, encoding='utf-8')
print(json.dumps({"tracks":len(tracks),"signals":len(signals),"junctions":len(junctions),"nodes":len(nodes),"edges":len(edges),"stations":len(stations),"validation":data["validation"]},indent=2))
