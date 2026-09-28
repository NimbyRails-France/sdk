
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_1407af120(undefined8 *param_1,longlong *param_2)

{
  longlong *plVar1;
  longlong lVar2;
  void *pvVar3;
  void *pvVar4;
  char cVar5;
  int iVar6;
  undefined8 uVar7;
  undefined8 *puVar8;
  char *pcVar9;
  undefined1 *puVar10;
  char *pcVar11;
  undefined8 **ppuVar12;
  code *pcVar13;
  void *pvVar14;
  undefined4 uVar15;
  longlong local_718;
  undefined8 **ppuStack_710;
  void *local_708;
  void *pvStack_700;
  undefined8 local_6f8;
  ulonglong uStack_6f0;
  longlong local_6e8;
  undefined8 **ppuStack_6e0;
  undefined8 *local_6d8;
  undefined8 uStack_6d0;
  void *local_6c8 [3];
  ulonglong local_6b0;
  undefined **local_6a8;
  undefined1 *local_6a0;
  undefined8 local_698;
  ulonglong local_690;
  undefined1 local_688 [512];
  undefined **local_488;
  undefined1 *local_480;
  undefined8 local_478;
  ulonglong local_470;
  undefined1 local_468 [512];
  undefined **local_268;
  undefined1 *local_260;
  undefined8 local_258;
  ulonglong local_250;
  undefined1 local_248 [528];
  
  *(undefined4 *)((longlong)param_2 + 0x34) = 0;
  *(undefined1 *)(param_2 + 6) = 1;
  *(undefined8 *)((longlong)param_2 + 0x3c) = 0x40800000;
  *(undefined8 *)((longlong)param_2 + 0x44) = 0;
  *(undefined1 *)(param_2 + 7) = 1;
  *(undefined4 *)((longlong)param_2 + 0x2c) = 0x44080000;
  *(undefined1 *)(param_2 + 5) = 1;
  *(undefined1 *)(param_2 + 4) = 1;
  *(undefined4 *)((longlong)param_2 + 0x24) = 2;
  (**(code **)(*param_2 + 8))(param_2);
  *(undefined4 *)((longlong)param_2 + 0x2c) = 0x43fa0000;
  *(undefined1 *)(param_2 + 5) = 1;
  *(undefined1 *)(param_2 + 7) = 1;
  *(undefined8 *)((longlong)param_2 + 0x3c) = 0;
  *(undefined8 *)((longlong)param_2 + 0x44) = 0;
  *(undefined1 *)(param_2 + 4) = 1;
  *(undefined4 *)((longlong)param_2 + 0x24) = 3;
  (**(code **)(*param_2 + 8))(param_2);
  plVar1 = param_1 + 1;
  if (*(char *)*param_1 == '\0') {
    pcVar13 = *(code **)(*param_2 + 0xb8);
    uVar7 = FUN_1402d82e0("editor_tip_zoom","Zoom in to edit");
    (*pcVar13)(param_2,DAT_140aac4b8,uVar7);
    goto LAB_1407afd59;
  }
  iVar6 = (**(code **)(**(longlong **)(*plVar1 + 0xd88) + 8))();
  lVar2 = *plVar1;
  if (iVar6 == 1) {
    if ((*(longlong *)(lVar2 + 0x18) == 0) && (*(longlong *)(lVar2 + 0x48) == 0)) {
      pcVar13 = *(code **)(*param_2 + 0xb8);
      uVar7 = FUN_1402d82e0("editor_tip_create_prompt_sel",
                            "To move or delete objects, you must select their control points");
      uVar15 = DAT_140aac4b8;
      (*pcVar13)(param_2,DAT_140aac4b8,uVar7);
      pcVar13 = *(code **)(*param_2 + 0xb8);
      uVar7 = FUN_1402d82e0("editor_tip_create_left1",
                            "Left click on object control points to select them");
      (*pcVar13)(param_2,uVar15,uVar7);
      pcVar13 = *(code **)(*param_2 + 0xb8);
      uVar7 = FUN_1402d82e0("editor_tip_create_left2",
                            "Left click on station name plates to edit station details");
      (*pcVar13)(param_2,uVar15,uVar7);
      cVar5 = FUN_14075e4b0(*plVar1 + 0xe8);
      if (cVar5 == '\0') {
        pcVar11 = "Ctrl-V to paste clipboard objects";
        pcVar9 = "editor_tip_paste";
        goto LAB_1407af8eb;
      }
    }
    else {
      pcVar13 = *(code **)(*param_2 + 0xb8);
      uVar7 = FUN_1402d82e0("editor_tip_create_drag",
                            "Drag any selected control point to move the selection");
      uVar15 = DAT_140aac4b8;
      (*pcVar13)(param_2,DAT_140aac4b8,uVar7);
      pcVar13 = *(code **)(*param_2 + 0xb8);
      uVar7 = FUN_1402d82e0("editor_tip_create_drag_alt",
                            "Alt and drag any selected control point to rotate the selection");
      (*pcVar13)(param_2,uVar15,uVar7);
      pcVar13 = *(code **)(*param_2 + 0xb8);
      uVar7 = FUN_1402d82e0("editor_tip_create_left3",
                            "Left click on control points to reset selection");
      (*pcVar13)(param_2,uVar15,uVar7);
      pcVar13 = *(code **)(*param_2 + 0xb8);
      uVar7 = FUN_1402d82e0("editor_tip_create_sleft",
                            "Shift left click to add or remove points from selection");
      (*pcVar13)(param_2,uVar15,uVar7);
      pcVar13 = *(code **)(*param_2 + 0xb8);
      uVar7 = FUN_1402d82e0("editor_tip_copy","Ctrl-C to copy selected objects into clipboard");
      (*pcVar13)(param_2,uVar15,uVar7);
      lVar2 = *plVar1;
      if ((*(longlong *)(lVar2 + 0xe8) != *(longlong *)(lVar2 + 0xf0)) ||
         (((*(longlong *)(lVar2 + 0x100) != *(longlong *)(lVar2 + 0x108) ||
           (*(longlong *)(lVar2 + 0x118) != *(longlong *)(lVar2 + 0x120))) ||
          (*(longlong *)(lVar2 + 0x130) != *(longlong *)(lVar2 + 0x138))))) {
        pcVar13 = *(code **)(*param_2 + 0xb8);
        uVar7 = FUN_1402d82e0("editor_tip_paste","Ctrl-V to paste clipboard objects");
        (*pcVar13)(param_2,uVar15,uVar7);
      }
      pcVar13 = *(code **)(*param_2 + 0xb8);
      uVar7 = FUN_1402d82e0("editor_tip_dupe","Ctrl-B to duplicate selected objects in-place");
      uVar7 = (*pcVar13)(param_2,uVar15,uVar7);
      ppuVar12 = (undefined8 **)0xffffffffffffffff;
      if (*(char *)param_1[2] != '\0') {
        pcVar13 = *(code **)(*param_2 + 0xb0);
        puVar8 = (undefined8 *)FUN_140726b70(uVar7,local_6c8,"tracks_unbp",1);
        local_718 = FUN_1402d82e0("editor_tip_create_rebp2",
                                  "{} to change selected built object(s) into blueprint object(s)");
        ppuStack_710 = (undefined8 **)0xffffffffffffffff;
        do {
          ppuStack_710 = (undefined8 **)((longlong)ppuStack_710 + 1);
        } while (*(char *)(local_718 + (longlong)ppuStack_710) != '\0');
        local_6d8 = puVar8;
        if (0xf < (ulonglong)puVar8[3]) {
          local_6d8 = (undefined8 *)*puVar8;
        }
        uStack_6d0 = puVar8[2];
        ppuStack_6e0 = &local_6d8;
        local_698 = 0;
        local_6a8 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
        local_6a0 = local_688;
        local_690 = 500;
        local_6e8 = 0xd;
        FUN_140022830(&local_6a8,&local_718,&local_6e8);
        local_708 = (void *)0x0;
        pvStack_700 = (void *)0x0;
        local_6f8 = 0;
        uStack_6f0 = 0;
        FUN_140002c00(&local_708,local_6a0);
        local_6a8 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
        if (local_6a0 != local_688) {
          puVar10 = local_6a0;
          if ((0xfff < local_690) &&
             (puVar10 = *(undefined1 **)(local_6a0 + -8),
             (undefined1 *)0x1f < local_6a0 + (-8 - (longlong)puVar10))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(puVar10);
        }
        (*pcVar13)(param_2,uVar15);
        if (0xf < uStack_6f0) {
          pvVar14 = local_708;
          if ((0xfff < uStack_6f0 + 1) &&
             (pvVar14 = *(void **)((longlong)local_708 + -8),
             0x1f < (ulonglong)((longlong)local_708 + (-8 - (longlong)pvVar14)))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pvVar14);
        }
        local_6f8 = _DAT_140aac910;
        uStack_6f0 = _UNK_140aac918;
        local_708 = (void *)((ulonglong)local_708 & 0xffffffffffffff00);
        if (0xf < local_6b0) {
          pvVar14 = local_6c8[0];
          if ((0xfff < local_6b0 + 1) &&
             (pvVar14 = *(void **)((longlong)local_6c8[0] + -8),
             0x1f < (ulonglong)((longlong)local_6c8[0] + (-8 - (longlong)pvVar14)))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pvVar14);
        }
      }
      if (*(char *)param_1[4] != '\0') {
        pcVar13 = *(code **)(*param_2 + 0xb0);
        puVar8 = (undefined8 *)FUN_140726b70();
        local_6e8 = FUN_1402d82e0("editor_tip_sel_track_flip",
                                  "{} to flip side of selected parallel tracks");
        do {
          ppuVar12 = (undefined8 **)((longlong)ppuVar12 + 1);
        } while (*(char *)(local_6e8 + (longlong)ppuVar12) != '\0');
        local_6d8 = puVar8;
        if (0xf < (ulonglong)puVar8[3]) {
          local_6d8 = (undefined8 *)*puVar8;
        }
        uStack_6d0 = puVar8[2];
        ppuStack_710 = &local_6d8;
        local_478 = 0;
        local_488 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
        local_480 = local_468;
        local_470 = 500;
        local_718 = 0xd;
        ppuStack_6e0 = ppuVar12;
        FUN_140022830(&local_488,&local_6e8,&local_718);
        local_708 = (void *)0x0;
        pvStack_700 = (void *)0x0;
        local_6f8 = 0;
        uStack_6f0 = 0;
        FUN_140002c00(&local_708,local_480);
        local_488 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
        if (local_480 != local_468) {
          puVar10 = local_480;
          if ((0xfff < local_470) &&
             (puVar10 = *(undefined1 **)(local_480 + -8),
             (undefined1 *)0x1f < local_480 + (-8 - (longlong)puVar10))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(puVar10);
        }
        (*pcVar13)(param_2,uVar15);
        if (0xf < uStack_6f0) {
          pvVar14 = local_708;
          if ((0xfff < uStack_6f0 + 1) &&
             (pvVar14 = *(void **)((longlong)local_708 + -8),
             0x1f < (ulonglong)((longlong)local_708 + (-8 - (longlong)pvVar14)))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pvVar14);
        }
        local_6f8 = _DAT_140aac910;
        uStack_6f0 = _UNK_140aac918;
        local_708 = (void *)((ulonglong)local_708 & 0xffffffffffffff00);
        if (0xf < local_6b0) {
          pvVar14 = local_6c8[0];
          if ((0xfff < local_6b0 + 1) &&
             (pvVar14 = *(void **)((longlong)local_6c8[0] + -8),
             0x1f < (ulonglong)((longlong)local_6c8[0] + (-8 - (longlong)pvVar14)))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pvVar14);
        }
      }
      pcVar13 = *(code **)(*param_2 + 0xb8);
      uVar7 = FUN_1402d82e0("editor_tip_create_del","Del to delete selected objects");
      (*pcVar13)(param_2,uVar15,uVar7);
      pcVar13 = *(code **)(*param_2 + 0xb8);
      uVar7 = FUN_1402d82e0("editor_tip_create_del_inline",
                            "Shift-Del to delete selected inner tracks");
      (*pcVar13)(param_2,uVar15,uVar7);
      pcVar11 = "Esc to cancel selection";
      pcVar9 = "editor_tip_create_esc";
LAB_1407af8eb:
      pcVar13 = *(code **)(*param_2 + 0xb8);
      uVar7 = FUN_1402d82e0(pcVar9,pcVar11);
      (*pcVar13)(param_2,uVar15,uVar7);
    }
    if (*(longlong *)(*plVar1 + 0xd80) == 0) goto LAB_1407afd59;
    pcVar13 = *(code **)(*param_2 + 0xb8);
    pcVar11 = "Ctrl-Z to undo the last edit";
    pcVar9 = "editor_tip_undo";
  }
  else {
    iVar6 = (**(code **)(**(longlong **)(lVar2 + 0xd88) + 8))();
    if (iVar6 != 0x16) {
      (**(code **)(**(longlong **)(*plVar1 + 0xd88) + 0x60))
                (*(longlong **)(*plVar1 + 0xd88),&local_708,param_1[3],param_1[5]);
      pvVar3 = pvStack_700;
      uVar15 = DAT_140aac4b8;
      for (pvVar14 = local_708; pvVar14 != pvVar3; pvVar14 = (void *)((longlong)pvVar14 + 0x20)) {
        (**(code **)(*param_2 + 0xb0))(param_2,uVar15,pvVar14);
      }
      pvVar14 = local_708;
      pvVar3 = pvStack_700;
      if (((char)uStack_6f0 != '\0') && (*(longlong *)(*plVar1 + 0xd80) != 0)) {
        pcVar13 = *(code **)(*param_2 + 0xb8);
        uVar7 = FUN_1402d82e0("editor_tip_undo","Ctrl-Z to undo the last edit");
        (*pcVar13)(param_2,uVar15,uVar7);
        pvVar14 = local_708;
        pvVar3 = pvStack_700;
      }
      for (; pvVar4 = pvStack_700, pvVar14 != pvStack_700;
          pvVar14 = (void *)((longlong)pvVar14 + 0x20)) {
        pvStack_700 = pvVar3;
        FUN_140002d30(pvVar14);
        pvVar3 = pvStack_700;
        pvStack_700 = pvVar4;
      }
      pvStack_700 = pvVar3;
      if (local_708 != (void *)0x0) {
        free(local_708);
      }
      goto LAB_1407afd59;
    }
    pcVar13 = *(code **)(*param_2 + 0xb8);
    uVar7 = FUN_1402d82e0("editor_tip_sel_signal_left","Left click to select a signal");
    uVar15 = DAT_140aac4b8;
    (*pcVar13)(param_2,DAT_140aac4b8,uVar7);
    if (*(longlong *)(*plVar1 + 0x78) != 0) {
      pcVar13 = *(code **)(*param_2 + 0xb8);
      uVar7 = FUN_1402d82e0("editor_tip_sel_signal_sleft",
                            "Shift left click to select additional signals");
      uVar7 = (*pcVar13)(param_2,uVar15,uVar7);
      pcVar13 = *(code **)(*param_2 + 0xb0);
      puVar8 = (undefined8 *)FUN_140726b70(uVar7,local_6c8,"track_flip_signal",1);
      local_6e8 = FUN_1402d82e0("editor_tip_sel_signal_flip",
                                "{} to flip direction of selected signals");
      ppuStack_6e0 = (undefined8 **)0xffffffffffffffff;
      do {
        ppuStack_6e0 = (undefined8 **)((longlong)ppuStack_6e0 + 1);
      } while (*(char *)(local_6e8 + (longlong)ppuStack_6e0) != '\0');
      local_6d8 = puVar8;
      if (0xf < (ulonglong)puVar8[3]) {
        local_6d8 = (undefined8 *)*puVar8;
      }
      uStack_6d0 = puVar8[2];
      ppuStack_710 = &local_6d8;
      local_258 = 0;
      local_268 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
      local_260 = local_248;
      local_250 = 500;
      local_718 = 0xd;
      FUN_140022830(&local_268,&local_6e8,&local_718);
      local_708 = (void *)0x0;
      pvStack_700 = (void *)0x0;
      local_6f8 = 0;
      uStack_6f0 = 0;
      FUN_140002c00(&local_708,local_260);
      local_268 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
      if (local_260 != local_248) {
        puVar10 = local_260;
        if ((0xfff < local_250) &&
           (puVar10 = *(undefined1 **)(local_260 + -8),
           (undefined1 *)0x1f < local_260 + (-8 - (longlong)puVar10))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(puVar10);
      }
      (*pcVar13)(param_2,uVar15);
      if (0xf < uStack_6f0) {
        pvVar14 = local_708;
        if ((0xfff < uStack_6f0 + 1) &&
           (pvVar14 = *(void **)((longlong)local_708 + -8),
           0x1f < (ulonglong)((longlong)local_708 + (-8 - (longlong)pvVar14)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pvVar14);
      }
      local_6f8 = _DAT_140aac910;
      uStack_6f0 = _UNK_140aac918;
      local_708 = (void *)((ulonglong)local_708 & 0xffffffffffffff00);
      if (0xf < local_6b0) {
        pvVar14 = local_6c8[0];
        if ((0xfff < local_6b0 + 1) &&
           (pvVar14 = *(void **)((longlong)local_6c8[0] + -8),
           0x1f < (ulonglong)((longlong)local_6c8[0] + (-8 - (longlong)pvVar14)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pvVar14);
      }
      pcVar13 = *(code **)(*param_2 + 0xb8);
      uVar7 = FUN_1402d82e0("editor_tip_sel_signal_del","Del to delete selected signals");
      (*pcVar13)(param_2,uVar15,uVar7);
    }
    if (*(longlong *)(*plVar1 + 0x78) == 1) {
      pcVar13 = *(code **)(*param_2 + 0xb8);
      uVar7 = FUN_1402d82e0("editor_tip_sel_signal_drag","Drag to move selected signal");
      (*pcVar13)(param_2,uVar15,uVar7);
    }
    pcVar13 = *(code **)(*param_2 + 0xb8);
    if (*(longlong *)(*plVar1 + 0x78) == 0) {
      pcVar11 = "Esc to go to track edit mode";
      pcVar9 = "editor_tip_sel_signal_esc";
    }
    else {
      pcVar11 = "Esc to clear selection";
      pcVar9 = "editor_tip_sel_signal_sel_esc";
    }
  }
  uVar7 = FUN_1402d82e0(pcVar9,pcVar11);
  (*pcVar13)(param_2,uVar15,uVar7);
LAB_1407afd59:
  (**(code **)(*param_2 + 0x18))(param_2);
  *(undefined8 *)((longlong)param_2 + 0x3c) = 0;
  *(undefined8 *)((longlong)param_2 + 0x44) = 0;
  *(undefined1 *)(param_2 + 7) = 1;
  *(undefined4 *)((longlong)param_2 + 0x2c) = 0x42100000;
  *(undefined1 *)(param_2 + 5) = 1;
  *(undefined4 *)((longlong)param_2 + 0x34) = 0x42100000;
  *(undefined1 *)(param_2 + 6) = 1;
  *(undefined1 *)(param_2 + 3) = 1;
  *(undefined4 *)((longlong)param_2 + 0x1c) = 0xc0;
  pvStack_700 = (void *)0x0;
  local_6f8 = 0;
  uStack_6f0 = 0xf;
  local_708 = (void *)0x0;
  uVar7 = FUN_1402d82e0("help_show_full","Show help");
  cVar5 = FUN_140787c10(&local_708,param_1[3],param_2,*(undefined1 *)(*plVar1 + 0x6c3),
                        "icon_all_help.svg",uVar7,&local_708);
  if (cVar5 != '\0') {
    *(bool *)(*plVar1 + 0x6c3) = *(char *)(*plVar1 + 0x6c3) == '\0';
  }
                    /* WARNING: Could not recover jumptable at 0x0001407afe3a. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*param_2 + 0x18))(param_2);
  return;
}

