"""Audit every native event/port transition in an existing graph capture."""
import collections
import json
import pathlib
import runpy
import sys

root = pathlib.Path(__file__).resolve().parent.parent
import argparse
parser = argparse.ArgumentParser()
parser.add_argument('capture', nargs='?', default=str(root/'build/lyon-capture.json'))
parser.add_argument('output', nargs='?', default=str(root/'exports/lyon'))
parser.add_argument('--lyon-regressions', action='store_true')
args = parser.parse_args()
sys.argv = [str(root/'tools/build_network_graph.py'), args.capture, args.output]
g = runpy.run_path(sys.argv[0])
transitions = g['transitions']
errors = []
for node in g['nodes']:
    for port in node['ports']:
        xy = g['point'](port['track'], port['fraction'])
        if max(abs(xy[0]-node['x']), abs(xy[1]-node['y'])) > 1e-7:
            errors.append({'check':'coincident attachment ports', 'node':node['id']})
for edge in g['edges']:
    for endpoint, nid in [(edge['points'][0],edge['source']), (edge['points'][-1],edge['target'])]:
        node = g['node_by_id'][nid]
        if max(abs(endpoint[0]-node['x']), abs(endpoint[1]-node['y'])) > 1e-7:
            errors.append({'check':'edge meets node', 'edge':edge['id'], 'node':nid})
transition_count = 0
for track, fractions in g['fractions'].items():
    for fraction in fractions:
        for direction in (-1, 1):
            event = (track, fraction)
            for target, target_direction in transitions(event, direction):
                transition_count += 1
                if (event, -direction) not in list(transitions(target, -target_direction)):
                    errors.append({'check':'reverse transition', 'from':[event,direction], 'to':[target,target_direction]})

# Validate every turnout's divergence and merge, including native sign changes.
for j in g['junctions']:
    main = (j['main'], j['fraction'])
    branch = (j['branch'], 0. if j['branchDirection']==1 else 1.)
    if (branch,j['branchDirection']) not in list(transitions(main,j['mainDirection'])):
        errors.append({'check':'divergence', 'branch':j['branch']})
    if (main,-j['mainDirection']) not in list(transitions(branch,-j['branchDirection'])):
        errors.append({'check':'merge', 'branch':j['branch']})

signals = g['signals']
forward = lambda s: -s['direction'] if s['kind']==4 else s['direction']
counts = collections.Counter()
details = []
for q in g['queries']:
    if q['direction'] != forward(signals[q['signal']]):
        continue
    accepted, excluded = [], []
    for p in q['paths']:
        target = signals[p['signal']]
        reason = ('terminal' if g['stops_at_no_way'](target) else 'no_way_with_exceptions' if target['kind']==5 else 'reversed_oneway' if target['kind']==0 and forward(target)!=p['arrivalDirection'] else
                  'oneway_passed' if target['kind']==0 else 'non_path_ignored' if target['kind']!=4 else
                  'opposite_signal_passed' if forward(target)!=p['arrivalDirection'] else 'normal_direction')
        counts[reason] += 1
        (accepted if reason=='normal_direction' else excluded).append({'signal':p['signal'],'reason':reason,'arrivalDirection':p['arrivalDirection']})
    details.append({'signal':q['signal'],'firstFacing':accepted,'otherFirstEncounters':excluded,'nextSignals':next(f['next'] for f in g['forward_queries'] if f['signal']==q['signal']),'incomplete':q['incomplete']})

report = {'scope':'Captured network; topology and conservative normal-direction display, not live route permissions',
          'signals':len(signals), 'junctions':len(g['junctions']), 'nativeTransitionsChecked':transition_count,
          'firstEncounterClassification':dict(counts), 'incompleteSignals':sum(d['incomplete'] for d in details),
          'errors':errors, 'signalsDetail':details}
if args.lyon_regressions:
    exec(compile((root/'tests/network_graph_lyon_regressions.py').read_text(encoding='utf8'), 'network_graph_lyon_regressions.py', 'exec'))
(pathlib.Path(args.output)/'branch-audit.json').write_text(json.dumps(report,ensure_ascii=False,indent=2),encoding='utf8')
print(json.dumps({k:v for k,v in report.items() if k!='signalsDetail'},indent=2))
if errors:
    sys.exit(1)
