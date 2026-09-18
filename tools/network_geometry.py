"""Shared map geometry for native track attachments, independent of region and IDs."""

def make_point_resolver(all_tracks, junctions):
    geometry_attachments = {j['branch']: j for j in junctions}

    def point(tid, f, visiting=frozenset()):
        # Both ports of an attachment must share the main track's position.
        # A missing neighbour on a branch is its attachment, not its centre.
        if tid in visiting:
            raise RuntimeError(f'Cyclic geometry attachment: {tid}')
        t = all_tracks[tid]
        ends = []
        for end, field in [(0., 'a'), (1., 'b')]:
            j = geometry_attachments.get(tid)
            if j and end == (0. if j['branchDirection']==1 else 1.):
                xy = point(j['main'], j['fraction'], visiting | {tid})
            else:
                neighbor = all_tracks.get(t[field], t)
                neighbor_attachment = geometry_attachments.get(neighbor['id'])
                if j and neighbor_attachment and neighbor['id'] != tid:
                    # Two attached branch sections form a crossover. Their raw
                    # centres are not the attachment positions: averaging them
                    # bends both diagonals toward the same end of the crossover.
                    left = point(j['main'], j['fraction'], visiting | {tid})
                    right = point(neighbor_attachment['main'], neighbor_attachment['fraction'], visiting | {tid})
                    xy = [(left[i]+right[i])/2 for i in (0,1)]
                else:
                    xy = [(neighbor[k]+t[k])/2 for k in ('x','y')]
            ends.append(xy)
        return [ends[0][i]*(1-f)+ends[1][i]*f for i in (0,1)]
    return point
