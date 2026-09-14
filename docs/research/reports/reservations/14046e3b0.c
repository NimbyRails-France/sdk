// Candidate VA 14046e3b0; RVA 0x46e3b0
// Ghidra inferred prototype: undefined FUN_14046e3b0()

void FUN_14046e3b0(longlong *param_1,undefined8 param_2,longlong param_3,longlong param_4)

{
  longlong *plVar1;
  int *piVar2;
  int iVar3;
  longlong lVar4;
  longlong *plVar5;
  longlong *local_28;
  longlong *plStack_20;
  
  if (*param_1 == 0) {
    plVar5 = (longlong *)FUN_140983da8(0x2d8);
    *plVar5 = 0;
    plVar5[1] = 0;
    *(undefined4 *)(plVar5 + 1) = 1;
    *(undefined4 *)((longlong)plVar5 + 0xc) = 1;
    *plVar5 = (longlong)std::_Ref_count_obj2<nimby::model::lng::Network>::vftable;
    plVar1 = plVar5 + 2;
    memset(plVar1,0,0x2c8);
    FUN_140473d30(plVar1);
    local_28 = plVar1;
    plStack_20 = plVar5;
    FUN_140232230(param_1,&local_28);
    plVar1 = plStack_20;
    if (plStack_20 != (longlong *)0x0) {
      LOCK();
      plVar5 = plStack_20 + 1;
      lVar4 = *plVar5;
      *(int *)plVar5 = (int)*plVar5 + -1;
      UNLOCK();
      if ((int)lVar4 == 1) {
        (**(code **)*plStack_20)(plStack_20);
        LOCK();
        piVar2 = (int *)((longlong)plVar1 + 0xc);
        iVar3 = *piVar2;
        *piVar2 = *piVar2 + -1;
        UNLOCK();
        if (iVar3 == 1) {
          (**(code **)(*plVar1 + 8))(plVar1);
        }
      }
    }
    FUN_1403559c0(*param_1,param_3,*(undefined8 *)(param_3 + 0x410),param_3 + 0x428,param_3 + 0x780,
                  param_3 + 0x890);
    FUN_14035a1f0(*param_1);
    FUN_1404775e0(param_4 + 0x20);
    FUN_14035cbb0(*param_1);
  }
  if (param_1[4] == 0) {
    plVar5 = (longlong *)FUN_140983da8(0x160);
    *plVar5 = 0;
    plVar5[1] = 0;
    *(undefined4 *)(plVar5 + 1) = 1;
    *(undefined4 *)((longlong)plVar5 + 0xc) = 1;
    *plVar5 = (longlong)std::_Ref_count_obj2<nimby::model::lng::Tiles>::vftable;
    plVar1 = plVar5 + 2;
    memset(plVar1,0,0x150);
    FUN_140473fe0(plVar1);
    local_28 = plVar1;
    plStack_20 = plVar5;
    FUN_140232230(param_1 + 4,&local_28);
    plVar1 = plStack_20;
    if (plStack_20 != (longlong *)0x0) {
      LOCK();
      plVar5 = plStack_20 + 1;
      lVar4 = *plVar5;
      *(int *)plVar5 = (int)*plVar5 + -1;
      UNLOCK();
      if ((int)lVar4 == 1) {
        (**(code **)*plStack_20)(plStack_20);
        LOCK();
        piVar2 = (int *)((longlong)plVar1 + 0xc);
        iVar3 = *piVar2;
        *piVar2 = *piVar2 + -1;
        UNLOCK();
        if (iVar3 == 1) {
          (**(code **)(*plVar1 + 8))(plVar1);
        }
      }
    }
    FUN_140356d20(param_1[4],param_3,*(undefined8 *)(param_3 + 0x410),
                  *(undefined8 *)(param_3 + 0x1590),param_3 + 0x428);
    local_28 = (longlong *)0x0;
    plStack_20 = (longlong *)0x0;
    FUN_140357130(param_1[4],param_2,&local_28,0xffffffff);
  }
  return;
}


// Incoming references
// 0xc247e8 DATA caller none
// 0x46ccbb UNCONDITIONAL_CALL caller 14046cbd0
// 0x46e64a UNCONDITIONAL_CALL caller 14046e5e0
// 0x4a9b57 UNCONDITIONAL_CALL caller 1404a8980
