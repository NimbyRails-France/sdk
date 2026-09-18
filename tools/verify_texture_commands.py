"""Opt-in live texture test; always restore the test override on exit."""
import argparse
import ctypes as c
import os
from pathlib import Path
import time

parser = argparse.ArgumentParser()
parser.add_argument('dll', type=Path)
parser.add_argument('pid', type=int)
parser.add_argument('signal', type=lambda s:int(s,0))
parser.add_argument('texture_set')
parser.add_argument('--replacement-index',type=int,default=1)
args = parser.parse_args()

class Status(c.Structure):
    _fields_ = [('size',c.c_uint32),('thread',c.c_uint32)]+[(n,c.c_uint64) for n in
        ('callbacks','valid','applied','last_signal','last_rules','signal','expires')]

with os.add_dll_directory(str(args.dll.resolve().parent)):
    dll = c.CDLL(str(args.dll.resolve()))
force = dll.NimbyInternal_ForceSignalTexture
force.argtypes = [c.c_uint32,c.c_uint64,c.c_char_p,c.c_uint32]
force.restype = c.c_uint32
clear = dll.NimbyInternal_ClearTexturePreview
clear.argtypes = [c.c_uint32,c.c_uint64]
clear.restype = c.c_uint32
status = dll.NimbyInternal_TexturePreviewStatus
status.argtypes = [c.c_uint32,c.POINTER(Status)]
status.restype = c.c_uint32
def current():
    s=Status();s.size=c.sizeof(s)
    assert status(args.pid,c.byref(s))==0
    return s

before=current()
assert before.expires==0 or (before.expires!=2**64-1 and before.expires<int(time.monotonic()*1000)), 'An existing override must be restored before testing'
active=False
try:
    result=force(args.pid,args.signal,args.texture_set.encode(),0)
    assert result==0, f'Force rejected with SDK status {result}; check loaded texture set and signal'
    active=True
    assert current().expires==2**64-1
    time.sleep(1.2)
    first=current()
    assert first.expires==2**64-1
    assert force(args.pid,args.signal,args.texture_set.encode(),args.replacement_index)==0
    assert force(args.pid,args.signal,args.texture_set.encode(),2**32-1)!=0
    assert current().expires==2**64-1, 'Invalid replacement cleared the valid override'
    assert clear(args.pid,args.signal+1)!=0
    assert current().expires==2**64-1, 'Wrong signal cleared the override'
    time.sleep(.4)
    second=current()
    print(f'PASS: persistent mode, replacement, invalid index, wrong-signal clear; draw counts {first.applied}/{second.applied}')
finally:
    if active:
        result=clear(args.pid,args.signal)
        assert result==0, f'Restore failed with status {result}'
        restored=current()
        assert restored.expires==0
        time.sleep(.3)
        assert current().applied==restored.applied, 'Override still applied after restoration'
        print('PASS: native rendering restored')
