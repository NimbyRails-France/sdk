// Candidate VA 14070db60; RVA 0x70db60
// Ghidra inferred prototype: undefined FUN_14070db60()

void FUN_14070db60(longlong *param_1,longlong *param_2)

{
  void *pvVar1;
  void *pvVar2;
  longlong lVar3;
  void *_Memory;
  longlong lVar4;
  longlong lVar5;
  void *local_c8;
  void *pvStack_c0;
  void *local_b8;
  undefined8 local_a8;
  undefined8 uStack_a0;
  undefined8 local_98;
  undefined8 local_90;
  undefined8 local_88;
  undefined8 uStack_80;
  undefined8 local_78;
  undefined8 local_70;
  undefined8 local_68;
  undefined8 uStack_60;
  undefined8 local_58;
  undefined8 local_50;
  undefined8 local_48;
  undefined8 uStack_40;
  undefined8 local_38;
  undefined8 local_30;
  undefined8 local_28;
  undefined8 uStack_20;
  undefined8 local_18;
  undefined8 local_10;
  
  lVar3 = FUN_1402d82e0("sched_editor_tab_basics","Basics");
  local_a8 = 0;
  uStack_a0 = 0;
  local_98 = 0;
  local_90 = 0;
  lVar4 = -1;
  lVar5 = -1;
  do {
    lVar5 = lVar5 + 1;
  } while (*(char *)(lVar3 + lVar5) != '\0');
  FUN_140002c00(&local_a8,lVar3);
  lVar3 = FUN_1402d82e0("sched_editor_tab_orders","Orders");
  local_88 = 0;
  uStack_80 = 0;
  local_78 = 0;
  local_70 = 0;
  lVar5 = -1;
  do {
    lVar5 = lVar5 + 1;
  } while (*(char *)(lVar3 + lVar5) != '\0');
  FUN_140002c00(&local_88,lVar3);
  lVar3 = FUN_1402d82e0("sched_editor_tab_offsets","Offsets");
  local_68 = 0;
  uStack_60 = 0;
  local_58 = 0;
  local_50 = 0;
  lVar5 = -1;
  do {
    lVar5 = lVar5 + 1;
  } while (*(char *)(lVar3 + lVar5) != '\0');
  FUN_140002c00(&local_68,lVar3);
  lVar3 = FUN_1402d82e0("sched_editor_tab_trains","Trains");
  local_48 = 0;
  uStack_40 = 0;
  local_38 = 0;
  local_30 = 0;
  lVar5 = -1;
  do {
    lVar5 = lVar5 + 1;
  } while (*(char *)(lVar3 + lVar5) != '\0');
  FUN_140002c00(&local_48,lVar3);
  lVar3 = FUN_1402d82e0("sched_editor_tab_timetable","Timetable");
  local_28 = 0;
  uStack_20 = 0;
  local_18 = 0;
  local_10 = 0;
  do {
    lVar4 = lVar4 + 1;
  } while (*(char *)(lVar3 + lVar4) != '\0');
  FUN_140002c00(&local_28,lVar3,lVar4);
  local_c8 = (void *)0x0;
  pvStack_c0 = (void *)0x0;
  local_b8 = (void *)0x0;
  _Memory = (void *)thunk_FUN_140983da8(0xa0);
  pvVar1 = (void *)((longlong)_Memory + 0xa0);
  local_c8 = _Memory;
  pvStack_c0 = pvVar1;
  local_b8 = pvVar1;
  FUN_14041eae0(&local_a8,&stack0xfffffffffffffff8,_Memory);
  _eh_vector_destructor_iterator_(&local_a8,0x20,5,thunk_FUN_140002d30);
  *(undefined4 *)((longlong)param_2 + 0x34) = 0x41f00000;
  *(undefined1 *)(param_2 + 6) = 1;
  *(undefined1 *)(param_2 + 4) = 1;
  *(undefined4 *)((longlong)param_2 + 0x24) = 2;
  (**(code **)(*param_2 + 8))(param_2);
  *(undefined1 *)(param_2 + 3) = 1;
  *(undefined4 *)((longlong)param_2 + 0x1c) = 0x1e0;
  (**(code **)(*param_2 + 200))(param_2);
  FUN_140572a80(*param_1 + 0x700,&local_c8,param_2);
  *(undefined1 *)(param_2 + 3) = 1;
  *(undefined4 *)((longlong)param_2 + 0x1c) = 0x1e0;
  (**(code **)(*param_2 + 200))(param_2);
  (**(code **)(*param_2 + 0x18))(param_2);
  lVar3 = *(longlong *)param_1[1];
  pvVar2 = _Memory;
  if (lVar3 == 0) {
    FUN_1406f5a70(*param_1 + 0x740,param_1[2],param_1[3],param_2);
  }
  else {
    if (lVar3 == 1) {
      *(undefined1 *)(param_2 + 3) = 1;
      *(undefined4 *)((longlong)param_2 + 0x1c) = 0x1e0;
      *(undefined1 *)(param_2 + 4) = 1;
      *(undefined4 *)((longlong)param_2 + 0x24) = 2;
      (**(code **)(*param_2 + 8))(param_2);
      FUN_14055b8e0(param_2,&DAT_140b8e660);
      *(undefined4 *)((longlong)param_2 + 0x2c) = 0x43960000;
      *(undefined1 *)(param_2 + 5) = 1;
      *(undefined1 *)(param_2 + 3) = 1;
      *(undefined4 *)((longlong)param_2 + 0x1c) = 0x140;
      (**(code **)(*param_2 + 8))(param_2);
      FUN_1406fc1f0(*param_1 + 0x1158,param_1[2],param_1[3],param_2);
      FUN_1406fd000(*param_1 + 0xb88,param_1[2],param_1[3],*param_1 + 0x60,param_2);
      (**(code **)(*param_2 + 0x18))(param_2);
      FUN_14055b8e0(param_2,&DAT_140b8e660);
      *(undefined1 *)(param_2 + 3) = 1;
      *(undefined4 *)((longlong)param_2 + 0x1c) = 0x1e0;
      *(undefined1 *)(param_2 + 7) = 1;
      *(undefined4 *)((longlong)param_2 + 0x3c) = 0x40800000;
      (**(code **)(*param_2 + 8))(param_2);
      *(undefined4 *)((longlong)param_2 + 0x34) = 0x3f800000;
      *(undefined1 *)(param_2 + 6) = 1;
      (**(code **)(*param_2 + 200))(param_2);
      FUN_1407094b0(*param_1 + 0x1210,param_1[2],param_1[3],param_2);
    }
    else {
      if (lVar3 != 2) {
        if (lVar3 == 3) {
          FUN_14070c9a0(*param_1 + 0x1cc8,param_1[2],param_1[3],*param_1 + 0x280,param_2);
        }
        else if (lVar3 == 4) {
          FUN_14070a160(*param_1 + 0x1d28,param_1[2],param_1[3],param_2);
        }
        goto joined_r0x00014070df7b;
      }
      *(undefined1 *)(param_2 + 3) = 1;
      *(undefined4 *)((longlong)param_2 + 0x1c) = 0x1e0;
      *(undefined1 *)(param_2 + 4) = 1;
      *(undefined4 *)((longlong)param_2 + 0x24) = 2;
      (**(code **)(*param_2 + 8))(param_2);
      FUN_14055b8e0(param_2,&DAT_140b8e660);
      *(undefined1 *)(param_2 + 3) = 1;
      *(undefined4 *)((longlong)param_2 + 0x1c) = 0x1e0;
      (**(code **)(*param_2 + 8))(param_2);
      FUN_1407057f0(*param_1 + 0x1570,param_1[2],param_1[3],*param_1 + 0x60,param_2);
    }
    (**(code **)(*param_2 + 0x18))(param_2);
    (**(code **)(*param_2 + 0x18))(param_2);
  }
joined_r0x00014070df7b:
  for (; pvVar2 != pvVar1; pvVar2 = (void *)((longlong)pvVar2 + 0x20)) {
    FUN_140002d30(pvVar2);
  }
  free(_Memory);
  return;
}


// Incoming references
// 0xc359fc DATA caller none
// 0x71b2c7 UNCONDITIONAL_CALL caller 14071af30
// 0x71b90f UNCONDITIONAL_CALL caller 14071af30
