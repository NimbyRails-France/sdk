"""Optional author-declared project maturity, independent of release channels."""
import json
from pathlib import Path


def development_metadata(root):
    source = Path(root) / 'release-channels.json'
    if not source.is_file():
        return {}
    policy = json.loads(source.read_text(encoding='utf-8-sig'))
    if not isinstance(policy, dict):
        raise ValueError('release-channels.json must contain a JSON object')
    if 'developmentStatus' not in policy:
        return {}
    status = policy['developmentStatus']
    if not isinstance(status, str) or status not in ('in-development', 'stable'):
        raise ValueError('developmentStatus must be in-development or stable when present')
    return dict(developmentStatus=status)
