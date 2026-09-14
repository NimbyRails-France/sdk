// Candidate VA 14073b1f0; RVA 0x73b1f0
// Ghidra inferred prototype: undefined FUN_14073b1f0()

void FUN_14073b1f0(undefined8 *param_1)

{
  longlong *plVar1;
  int *piVar2;
  int iVar3;
  longlong lVar4;
  longlong *plVar5;
  void *pvVar6;
  longlong lVar7;
  
  FUN_140348960(param_1 + 0xba);
  if (*(int *)(param_1 + 0xbd) == 0) {
    if ((void *)param_1[0x93] != (void *)0x0) {
      free((void *)param_1[0x93]);
    }
    lVar4 = param_1[0x91];
    for (lVar7 = param_1[0x90]; lVar7 != lVar4; lVar7 = lVar7 + 0x28) {
      FUN_14034bc60(lVar7);
    }
    if ((void *)param_1[0x90] != (void *)0x0) {
      free((void *)param_1[0x90]);
    }
    FUN_14034c2a0(param_1 + 0x85);
    FUN_14034bf90(param_1 + 0x7a);
    FUN_140347150(param_1 + 0x4e);
    FUN_14032a340(param_1[0x4b],param_1[0x4c]);
    if ((void *)param_1[0x4b] != (void *)0x0) {
      free((void *)param_1[0x4b]);
    }
    FUN_14034bc60(param_1 + 0x48);
    plVar5 = (longlong *)param_1[0x45];
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
    FUN_14034bd60(param_1 + 0x41);
    pvVar6 = (void *)param_1[0x28];
    if (pvVar6 != (void *)0x0) {
      FUN_14034e2c0(pvVar6);
      free(pvVar6);
    }
    if (param_1[0x20] != 0) {
      FUN_14034ddf0();
    }
    pvVar6 = (void *)param_1[0x18];
    if (pvVar6 != (void *)0x0) {
      FUN_14034f700(pvVar6);
      free(pvVar6);
    }
    pvVar6 = (void *)param_1[0x10];
    if (pvVar6 != (void *)0x0) {
      FUN_14034e2c0(pvVar6);
      free(pvVar6);
    }
    pvVar6 = (void *)*param_1;
    if (pvVar6 != (void *)0x0) {
      FUN_14034e910(pvVar6);
      free(pvVar6);
    }
    return;
  }
                    /* WARNING: Subroutine does not return */
  terminate();
}


// Incoming references
// 0xc36f38 DATA caller none
// 0x740a8e UNCONDITIONAL_CALL caller 140740a40
