// Candidate VA 140740a40; RVA 0x740a40
// Ghidra inferred prototype: undefined FUN_140740a40()

void FUN_140740a40(longlong *param_1)

{
  longlong *plVar1;
  int *piVar2;
  int iVar3;
  longlong lVar4;
  longlong *plVar5;
  void *_Memory;
  
  lVar4 = param_1[0x69];
  if ((lVar4 != 0) && (1 < (byte)(*(char *)(lVar4 + 8) - 3U))) {
    (**(code **)(*(longlong *)ThreadLocalStoragePointer + 0x20))(lVar4);
  }
  FUN_14073b1f0(param_1 + 0xa8);
  FUN_1403508d0(param_1 + 0x9d);
  if (1 < (ulonglong)param_1[0x9f]) {
    free((void *)param_1[0x9e]);
  }
  FUN_1402ec870(param_1 + 0x8c);
  FUN_1402ec870(param_1 + 0x81);
  if ((void *)param_1[0x7e] != (void *)0x0) {
    free((void *)param_1[0x7e]);
  }
  if ((void *)param_1[0x7b] != (void *)0x0) {
    free((void *)param_1[0x7b]);
  }
  if ((void *)param_1[0x78] != (void *)0x0) {
    free((void *)param_1[0x78]);
  }
  if ((void *)param_1[0x75] != (void *)0x0) {
    free((void *)param_1[0x75]);
  }
  FUN_140002d30(param_1 + 0x71);
  plVar5 = (longlong *)param_1[0x69];
  if (plVar5 != (longlong *)0x0) {
    LOCK();
    lVar4 = *plVar5;
    *plVar5 = *plVar5 + -1;
    UNLOCK();
    if (lVar4 == 1) {
      (**(code **)plVar5[3])();
    }
  }
  if (param_1[0x65] != 0) {
    FUN_14073d9d0();
  }
  FUN_1406031c0(param_1[0x62],param_1[99]);
  if ((void *)param_1[0x62] != (void *)0x0) {
    free((void *)param_1[0x62]);
  }
  if ((void *)param_1[0x5f] != (void *)0x0) {
    free((void *)param_1[0x5f]);
  }
  FUN_14073a8b0(param_1 + 0x5c);
  FUN_1404e2d80(param_1 + 6);
  if (param_1[5] != 0) {
    FUN_14073da70();
  }
  _Memory = (void *)param_1[4];
  if (_Memory != (void *)0x0) {
    FUN_1404f0c40(_Memory);
    free(_Memory);
  }
  plVar5 = (longlong *)param_1[3];
  if (plVar5 != (longlong *)0x0) {
    LOCK();
    plVar1 = plVar5 + 1;
    lVar4 = *plVar1;
    *(int *)plVar1 = (int)*plVar1 + -1;
    UNLOCK();
    if ((int)lVar4 == 1) {
      (**(code **)*plVar5)(plVar5);
      LOCK();
      piVar2 = (int *)((longlong)plVar5 + 0xc);
      iVar3 = *piVar2;
      *piVar2 = *piVar2 + -1;
      UNLOCK();
      if (iVar3 == 1) {
        (**(code **)(*plVar5 + 8))(plVar5);
      }
    }
  }
  plVar5 = (longlong *)param_1[1];
  if (plVar5 != (longlong *)0x0) {
    (**(code **)(*plVar5 + 0x20))(plVar5,1);
  }
  param_1 = (longlong *)*param_1;
  if (param_1 != (longlong *)0x0) {
    (**(code **)(*param_1 + 0x20))(param_1,1);
  }
  return;
}


// Incoming references
// 0xc37220 DATA caller none
// 0x73ac9c UNCONDITIONAL_CALL caller 14073ac80
// 0x9a9e15 UNCONDITIONAL_CALL caller 1409a9e00
