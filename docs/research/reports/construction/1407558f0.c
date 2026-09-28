
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_1407558f0(longlong *param_1,undefined1 param_2,longlong param_3,longlong param_4)

{
  undefined8 *puVar1;
  ulonglong uVar2;
  undefined8 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  longlong lVar7;
  undefined8 uVar8;
  longlong lVar9;
  char *pcVar10;
  void *pvVar11;
  undefined8 ****ppppuVar12;
  undefined8 *puVar13;
  undefined8 ***local_a8 [3];
  ulonglong local_90;
  char *local_88;
  undefined8 uStack_80;
  undefined8 local_78;
  ulonglong uStack_70;
  void *local_68 [3];
  ulonglong local_50;
  void *local_48 [3];
  ulonglong local_30;
  
  lVar7 = 0;
  lVar9 = *(longlong *)(param_3 + 600);
  if (lVar9 == 0) {
                    /* WARNING: Subroutine does not return */
    abort();
  }
  if (param_4 == 0) {
    *param_1 = 0;
  }
  else {
    lVar7 = FUN_14032c3b0(*(longlong *)(lVar9 + 0x428) + 0x80,param_4);
    *param_1 = lVar7;
    lVar7 = FUN_1403ca140(lVar9 + 0x430,param_4);
  }
  param_1[1] = lVar7;
  param_1[3] = *(longlong *)(lVar9 + 0x740);
  *(char *)((longlong)param_1 + 0x24) = (char)param_1[4];
  *(undefined1 *)((longlong)param_1 + 0x25) = *(undefined1 *)((longlong)param_1 + 0x21);
  *(undefined1 *)((longlong)param_1 + 0x26) = *(undefined1 *)((longlong)param_1 + 0x22);
  *(undefined1 *)((longlong)param_1 + 0x27) = *(undefined1 *)((longlong)param_1 + 0x23);
  FUN_14028fc90(local_48,param_4);
  local_88 = (char *)0x0;
  uStack_80 = 0;
  local_88 = (char *)FUN_140003270(0x20);
  uVar3 = s___station_editor_tag_picker_140a9c008._8_8_;
  local_78 = _DAT_140aacad0;
  uStack_70 = _UNK_140aacad8;
  *(undefined8 *)local_88 = s___station_editor_tag_picker_140a9c008._0_8_;
  *(undefined8 *)(local_88 + 8) = uVar3;
  *(undefined8 *)(local_88 + 0x10) = s___station_editor_tag_picker_140a9c008._16_8_;
  *(undefined2 *)(local_88 + 0x18) = s___station_editor_tag_picker_140a9c008._24_2_;
  local_88[0x1a] = s___station_editor_tag_picker_140a9c008[0x1a];
  local_88[0x1b] = '\0';
  FUN_14029c760(local_68,param_2);
  FUN_1405752e0(param_1 + 6,local_68);
  if (0xf < local_50) {
    pvVar11 = local_68[0];
    if ((0xfff < local_50 + 1) &&
       (pvVar11 = *(void **)((longlong)local_68[0] + -8),
       0x1f < (ulonglong)((longlong)local_68[0] + (-8 - (longlong)pvVar11)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pvVar11);
  }
  if (0xf < uStack_70) {
    pcVar10 = local_88;
    if ((0xfff < uStack_70 + 1) &&
       (pcVar10 = *(char **)(local_88 + -8), (char *)0x1f < local_88 + (-8 - (longlong)pcVar10))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pcVar10);
  }
  if (0xf < local_30) {
    pvVar11 = local_48[0];
    if ((0xfff < local_30 + 1) &&
       (pvVar11 = *(void **)((longlong)local_48[0] + -8),
       0x1f < (ulonglong)((longlong)local_48[0] + (-8 - (longlong)pvVar11)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pvVar11);
  }
  uVar8 = FUN_14028fc90(local_68,param_4);
  local_88 = (char *)0x0;
  uStack_80 = 0;
  local_88 = (char *)FUN_140003270(0x20);
  uVar3 = s___station_editor_walk_links_140a9bfe8._8_8_;
  local_78 = _DAT_140aacad0;
  uStack_70 = _UNK_140aacad8;
  *(undefined8 *)local_88 = s___station_editor_walk_links_140a9bfe8._0_8_;
  *(undefined8 *)(local_88 + 8) = uVar3;
  *(undefined8 *)(local_88 + 0x10) = s___station_editor_walk_links_140a9bfe8._16_8_;
  *(undefined2 *)(local_88 + 0x18) = s___station_editor_walk_links_140a9bfe8._24_2_;
  local_88[0x1a] = s___station_editor_walk_links_140a9bfe8[0x1a];
  local_88[0x1b] = '\0';
  FUN_14029c760(local_48,param_2,&local_88,uVar8);
  FUN_1407549e0(param_1 + 0x2c,param_3);
  if (0xf < uStack_70) {
    pcVar10 = local_88;
    if ((0xfff < uStack_70 + 1) &&
       (pcVar10 = *(char **)(local_88 + -8), (char *)0x1f < local_88 + (-8 - (longlong)pcVar10))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pcVar10);
  }
  if (0xf < local_50) {
    pvVar11 = local_68[0];
    if ((0xfff < local_50 + 1) &&
       (pvVar11 = *(void **)((longlong)local_68[0] + -8),
       0x1f < (ulonglong)((longlong)local_68[0] + (-8 - (longlong)pvVar11)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pvVar11);
  }
  FUN_14028fc90(local_48,param_4);
  local_88 = (char *)0x0;
  uStack_80 = 0;
  local_88 = (char *)FUN_140003270(0x30);
  uVar3 = s___station_editor_stop_selection__140a9c480._8_8_;
  local_78 = _DAT_140aacb90;
  uStack_70 = _UNK_140aacb98;
  *(undefined8 *)local_88 = s___station_editor_stop_selection__140a9c480._0_8_;
  *(undefined8 *)(local_88 + 8) = uVar3;
  uVar6 = s___station_editor_stop_selection__140a9c480._28_4_;
  uVar5 = s___station_editor_stop_selection__140a9c480._24_4_;
  uVar4 = s___station_editor_stop_selection__140a9c480._20_4_;
  *(undefined4 *)(local_88 + 0x10) = s___station_editor_stop_selection__140a9c480._16_4_;
  *(undefined4 *)(local_88 + 0x14) = uVar4;
  *(undefined4 *)(local_88 + 0x18) = uVar5;
  *(undefined4 *)(local_88 + 0x1c) = uVar6;
  *(undefined4 *)(local_88 + 0x20) = s___station_editor_stop_selection__140a9c480._32_4_;
  *(undefined2 *)(local_88 + 0x24) = s___station_editor_stop_selection__140a9c480._36_2_;
  local_88[0x26] = s___station_editor_stop_selection__140a9c480[0x26];
  local_88[0x27] = '\0';
  FUN_14029c760(local_a8,param_2);
  lVar9 = *param_1;
  if ((undefined8 ****)(param_1 + 0x3d) != local_a8) {
    ppppuVar12 = local_a8;
    if (0xf < local_90) {
      ppppuVar12 = (undefined8 ****)local_a8[0];
    }
    FUN_140030630(param_1 + 0x3d,ppppuVar12);
  }
  *(undefined4 *)(param_1 + 0x41) = 0x43c80000;
  FUN_14033b350(param_1 + 0x42);
  param_1[0x45] = 0;
  param_1[0x46] = 0;
  if (lVar9 != 0) {
    puVar1 = *(undefined8 **)(lVar9 + 0x338);
    for (puVar13 = *(undefined8 **)(lVar9 + 0x330); puVar13 != puVar1; puVar13 = puVar13 + 1) {
      if (*(longlong *)(param_3 + 600) == 0) {
                    /* WARNING: Subroutine does not return */
        abort();
      }
      lVar9 = FUN_14033f630(*(longlong *)(*(longlong *)(param_3 + 600) + 0x428) + 0x380,*puVar13);
      if (lVar9 != 0) {
        uVar2 = param_1[0x43];
        if (uVar2 < (ulonglong)param_1[0x44]) {
          param_1[0x43] = uVar2 + 200;
          FUN_1402f7860(uVar2,lVar9);
        }
        else {
          FUN_1403be0f0(param_1 + 0x42,lVar9);
        }
      }
    }
  }
  FUN_140002d30(local_a8);
  if (0xf < uStack_70) {
    pcVar10 = local_88;
    if ((0xfff < uStack_70 + 1) &&
       (pcVar10 = *(char **)(local_88 + -8), (char *)0x1f < local_88 + (-8 - (longlong)pcVar10))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pcVar10);
  }
  if (0xf < local_30) {
    pvVar11 = local_48[0];
    if ((0xfff < local_30 + 1) &&
       (pvVar11 = *(void **)((longlong)local_48[0] + -8),
       0x1f < (ulonglong)((longlong)local_48[0] + (-8 - (longlong)pvVar11)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pvVar11);
  }
  if ((char)param_1[0x4b] != '\0') {
    FUN_140002d30(param_1 + 0x47);
    *(undefined1 *)(param_1 + 0x4b) = 0;
  }
  *(undefined1 *)((longlong)param_1 + 0x261) = 0;
  *(undefined1 *)(param_1 + 0x4d) = 0;
  *(undefined1 *)(param_1 + 0x4e) = 0;
  *(undefined1 *)(param_1 + 0x4f) = 0;
  *(undefined1 *)(param_1 + 0x50) = 0;
  *(undefined1 *)(param_1 + 0x51) = 0;
  *(undefined1 *)((longlong)param_1 + 0x28d) = 0;
  *(undefined1 *)(param_1 + 0x53) = 0;
  return;
}

