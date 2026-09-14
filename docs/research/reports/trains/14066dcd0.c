// Candidate VA 14066dcd0; RVA 0x66dcd0
// Ghidra inferred prototype: undefined FUN_14066dcd0()

void FUN_14066dcd0(longlong *param_1,longlong *param_2)

{
  code *pcVar1;
  undefined8 *puVar2;
  longlong lVar3;
  undefined8 uVar4;
  longlong *plVar5;
  longlong *plVar6;
  undefined8 *puVar7;
  longlong lVar8;
  int local_res8 [2];
  
  *(undefined4 *)((longlong)param_2 + 0x2c) = 0x43480000;
  *(undefined1 *)(param_2 + 5) = 1;
  *(undefined1 *)(param_2 + 4) = 1;
  *(undefined4 *)((longlong)param_2 + 0x24) = 3;
  (**(code **)(*param_2 + 8))(param_2);
  plVar6 = param_2 + 3;
  if (*(longlong *)(*param_1 + 0x20) != *(longlong *)(*param_1 + 0x18)) {
    *(undefined1 *)plVar6 = 1;
    *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
    *(undefined1 *)((longlong)param_2 + 0x4c) = 1;
    *(undefined4 *)(param_2 + 10) = 1;
    pcVar1 = *(code **)(*param_2 + 0xa8);
    uVar4 = FUN_1402d82e0("search_section_lines","Lines");
    (*pcVar1)(param_2,uVar4,0x11);
    puVar2 = *(undefined8 **)(*param_1 + 0x20);
    for (puVar7 = *(undefined8 **)(*param_1 + 0x18); puVar7 != puVar2; puVar7 = puVar7 + 7) {
      *(undefined1 *)plVar6 = 1;
      plVar5 = puVar7 + 2;
      *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
      local_res8[0] = 0;
      if (0xf < (ulonglong)puVar7[5]) {
        plVar5 = (longlong *)*plVar5;
      }
      (**(code **)(*param_2 + 0xd8))(param_2,plVar5,0x11,local_res8);
      if (local_res8[0] != 0) {
        *(undefined8 *)param_1[1] = *puVar7;
      }
    }
    *(undefined4 *)((longlong)param_2 + 0x34) = 0x40c00000;
    *(undefined1 *)(param_2 + 6) = 1;
    (**(code **)(*param_2 + 200))(param_2);
  }
  if (*(longlong *)(*param_1 + 0x38) != *(longlong *)(*param_1 + 0x30)) {
    *(undefined1 *)plVar6 = 1;
    *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
    *(undefined1 *)((longlong)param_2 + 0x4c) = 1;
    *(undefined4 *)(param_2 + 10) = 1;
    pcVar1 = *(code **)(*param_2 + 0xa8);
    uVar4 = FUN_1402d82e0("search_section_stations","Stations");
    (*pcVar1)(param_2,uVar4,0x11);
    puVar2 = *(undefined8 **)(*param_1 + 0x38);
    for (puVar7 = *(undefined8 **)(*param_1 + 0x30); puVar7 != puVar2; puVar7 = puVar7 + 7) {
      *(undefined1 *)plVar6 = 1;
      plVar5 = puVar7 + 2;
      *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
      local_res8[0] = 0;
      if (0xf < (ulonglong)puVar7[5]) {
        plVar5 = (longlong *)*plVar5;
      }
      (**(code **)(*param_2 + 0xd8))(param_2,plVar5,0x11,local_res8);
      if (local_res8[0] != 0) {
        *(undefined8 *)param_1[2] = *puVar7;
      }
    }
    *(undefined4 *)((longlong)param_2 + 0x34) = 0x40c00000;
    *(undefined1 *)(param_2 + 6) = 1;
    (**(code **)(*param_2 + 200))(param_2);
  }
  if (*(longlong *)(*param_1 + 0x50) != *(longlong *)(*param_1 + 0x48)) {
    *(undefined1 *)(param_2 + 3) = 1;
    *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
    *(undefined1 *)((longlong)param_2 + 0x4c) = 1;
    *(undefined4 *)(param_2 + 10) = 1;
    pcVar1 = *(code **)(*param_2 + 0xa8);
    uVar4 = FUN_1402d82e0("search_section_trains","Trains");
    (*pcVar1)(param_2,uVar4,0x11);
    puVar2 = *(undefined8 **)(*param_1 + 0x50);
    for (puVar7 = *(undefined8 **)(*param_1 + 0x48); puVar7 != puVar2; puVar7 = puVar7 + 7) {
      *(undefined1 *)(param_2 + 3) = 1;
      plVar6 = puVar7 + 2;
      *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
      local_res8[0] = 0;
      if (0xf < (ulonglong)puVar7[5]) {
        plVar6 = (longlong *)*plVar6;
      }
      (**(code **)(*param_2 + 0xd8))(param_2,plVar6,0x11,local_res8);
      if (local_res8[0] != 0) {
        *(undefined8 *)param_1[3] = *puVar7;
      }
    }
    *(undefined4 *)((longlong)param_2 + 0x34) = 0x40c00000;
    *(undefined1 *)(param_2 + 6) = 1;
    (**(code **)(*param_2 + 200))(param_2);
  }
  if (*(longlong *)(*param_1 + 0x68) != *(longlong *)(*param_1 + 0x60)) {
    *(undefined1 *)(param_2 + 3) = 1;
    *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
    *(undefined1 *)((longlong)param_2 + 0x4c) = 1;
    *(undefined4 *)(param_2 + 10) = 1;
    pcVar1 = *(code **)(*param_2 + 0xa8);
    uVar4 = FUN_1402d82e0("search_section_schedules","Schedules");
    (*pcVar1)(param_2,uVar4,0x11);
    puVar2 = *(undefined8 **)(*param_1 + 0x68);
    for (puVar7 = *(undefined8 **)(*param_1 + 0x60); puVar7 != puVar2; puVar7 = puVar7 + 7) {
      *(undefined1 *)(param_2 + 3) = 1;
      plVar6 = puVar7 + 2;
      *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
      local_res8[0] = 0;
      if (0xf < (ulonglong)puVar7[5]) {
        plVar6 = (longlong *)*plVar6;
      }
      (**(code **)(*param_2 + 0xd8))(param_2,plVar6,0x11,local_res8);
      if (local_res8[0] != 0) {
        *(undefined8 *)param_1[4] = *puVar7;
      }
    }
    *(undefined4 *)((longlong)param_2 + 0x34) = 0x40c00000;
    *(undefined1 *)(param_2 + 6) = 1;
    (**(code **)(*param_2 + 200))(param_2);
  }
  if (*(longlong *)(*param_1 + 0x80) != *(longlong *)(*param_1 + 0x78)) {
    *(undefined1 *)(param_2 + 3) = 1;
    *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
    *(undefined1 *)((longlong)param_2 + 0x4c) = 1;
    *(undefined4 *)(param_2 + 10) = 1;
    pcVar1 = *(code **)(*param_2 + 0xa8);
    uVar4 = FUN_1402d82e0("search_section_train_models","Purchase train");
    (*pcVar1)(param_2,uVar4,0x11);
    lVar3 = *(longlong *)(*param_1 + 0x80);
    for (lVar8 = *(longlong *)(*param_1 + 0x78); lVar8 != lVar3; lVar8 = lVar8 + 0x38) {
      *(undefined1 *)(param_2 + 3) = 1;
      plVar6 = (longlong *)(lVar8 + 0x10);
      *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
      local_res8[0] = 0;
      if (0xf < *(ulonglong *)(lVar8 + 0x28)) {
        plVar6 = (longlong *)*plVar6;
      }
      (**(code **)(*param_2 + 0xd8))(param_2,plVar6,0x11,local_res8);
      if (local_res8[0] != 0) {
        *(undefined8 *)param_1[5] = *(undefined8 *)(lVar8 + 8);
      }
    }
    *(undefined4 *)((longlong)param_2 + 0x34) = 0x40c00000;
    *(undefined1 *)(param_2 + 6) = 1;
    (**(code **)(*param_2 + 200))(param_2);
  }
  if (((longlong *)param_1[6])[1] != *(longlong *)param_1[6]) {
    *(undefined1 *)(param_2 + 3) = 1;
    *(undefined4 *)((longlong)param_2 + 0x1c) = 0xa0;
    *(undefined1 *)((longlong)param_2 + 0x4c) = 1;
    *(undefined4 *)(param_2 + 10) = 1;
    pcVar1 = *(code **)(*param_2 + 0xa8);
    uVar4 = FUN_1402d82e0("search_section_world","World");
    (*pcVar1)(param_2,uVar4,0x11);
    lVar3 = ((longlong *)param_1[6])[1];
    for (lVar8 = *(longlong *)param_1[6]; lVar8 != lVar3; lVar8 = lVar8 + 0x78) {
      FUN_14066c580(param_1[7],param_2,lVar8);
    }
  }
                    /* WARNING: Could not recover jumptable at 0x00014066e207. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*param_2 + 0x18))(param_2);
  return;
}


// Incoming references
// 0xc327c8 DATA caller none
// 0xb29e98 DATA caller none
// 0xb29ebc DATA caller none
// 0x6a36f7 UNCONDITIONAL_CALL caller 1406a3360
// 0x6a3d3f UNCONDITIONAL_CALL caller 1406a3360
