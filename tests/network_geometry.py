"""Geometry regressions with synthetic IDs, translated and rotated networks."""
import math
import pathlib
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parents[1]/'tools'))
from network_geometry import make_point_resolver


class GeometryTests(unittest.TestCase):
    def test_crossovers_in_any_region_and_orientation(self):
        for angle in (0, .7, math.pi, 4.2):
            for offset in ((0, 0), (800000, -4000000)):
                def transform(x, y):
                    return (offset[0]+x*math.cos(angle)-y*math.sin(angle),
                            offset[1]+x*math.sin(angle)+y*math.cos(angle))
                tracks = {}
                def track(i, a, b, x, y):
                    x,y = transform(x,y)
                    tracks[i] = dict(id=i,a=a,b=b,x=x,y=y)
                for prefix,x in [('L',0),('R',10)]:
                    track(prefix+'a','none',prefix,x,-100)
                    track(prefix,prefix+'a',prefix+'b',x,0)
                    track(prefix+'b',prefix,'none',x,100)
                # Deliberately misleading branch centres must not distort X.
                for i,a,b in [('a','none','b'),('b','a','none'),
                              ('c','none','d'),('d','c','none')]:
                    track(i,a,b,1000,2000)
                junctions = [dict(branch=i,main=m,fraction=f,branchDirection=d)
                             for i,m,f,d in [('a','L',0,1),('b','R',1,-1),
                                             ('c','L',1,1),('d','R',0,-1)]]
                point = make_point_resolver(tracks,junctions)
                for j in junctions:
                    self.assertEqual(point(j['branch'],0 if j['branchDirection']==1 else 1),
                                     point(j['main'],j['fraction']))
                for i,f in [('a',1),('b',0),('c',1),('d',0)]:
                    for actual,expected in zip(point(i,f),transform(5,0)):
                        self.assertAlmostEqual(actual,expected,places=7)


if __name__ == '__main__':
    unittest.main()
