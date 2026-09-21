// Candidate VA 1405e4f40; RVA 0x5e4f40
// Ghidra inferred prototype: undefined FUN_1405e4f40()

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_1405e4f40(longlong param_1,undefined8 *param_2,longlong param_3)

{
  int *piVar1;
  void *pvVar2;
  char cVar3;
  int iVar4;
  longlong lVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  undefined8 ****_Memory;
  undefined8 *****pppppuVar8;
  void *pvVar9;
  char *pcVar10;
  undefined1 *puVar11;
  char *pcVar12;
  char *****pppppcVar13;
  int *piVar14;
  longlong lVar15;
  int *piVar16;
  longlong lVar17;
  undefined8 ****local_758;
  undefined8 uStack_750;
  undefined8 local_748;
  ulonglong local_740;
  char ****local_738;
  undefined8 ****ppppuStack_730;
  undefined8 local_728;
  ulonglong local_720;
  undefined8 ****local_718;
  undefined8 ****ppppuStack_710;
  undefined8 ****local_708;
  ulonglong local_700;
  longlong local_6f8;
  undefined8 ****ppppuStack_6f0;
  int local_6e8;
  undefined4 uStack_6e4;
  undefined1 local_6e0 [8];
  undefined8 local_6d8;
  ulonglong local_6d0;
  char local_6c0;
  undefined8 **local_6b8 [2];
  undefined8 local_6a8;
  ulonglong local_6a0;
  undefined8 *local_698;
  undefined1 local_690 [32];
  undefined1 local_670 [32];
  undefined8 local_650;
  undefined8 uStack_648;
  undefined8 local_640;
  undefined4 local_638;
  undefined8 local_634;
  undefined8 uStack_62c;
  undefined1 local_624;
  undefined1 local_618 [32];
  undefined1 local_5f8 [32];
  undefined1 local_5d8 [68];
  int local_594;
  undefined1 local_548 [32];
  undefined1 local_528 [176];
  undefined **local_478;
  undefined1 *local_470;
  undefined8 local_468;
  ulonglong local_460;
  undefined1 local_458 [512];
  undefined **local_258;
  undefined1 *local_250;
  undefined8 local_248;
  ulonglong local_240;
  undefined1 local_238 [512];

  lVar17 = -1;
  if (*(int *)(param_1 + 8) == 5) {
    FUN_1405e46f0();
  }
  else if (*(int *)(param_1 + 8) == 6) {
    local_738 = (char ****)0x0;
    ppppuStack_730 = (undefined8 ****)0x0;
    local_738 = (char ****)FUN_140003270(0x20);
    uVar6 = s___mm_save_file_name_140a8ba20._8_8_;
    local_728 = 0x13;
    local_720 = 0x1f;
    *local_738 = (char ***)s___mm_save_file_name_140a8ba20._0_8_;
    local_738[1] = (char ***)uVar6;
    *(undefined2 *)(local_738 + 2) = s___mm_save_file_name_140a8ba20._16_2_;
    *(char *)((longlong)local_738 + 0x12) = s___mm_save_file_name_140a8ba20[0x12];
    *(char *)((longlong)local_738 + 0x13) = '\0';
    lVar5 = FUN_1402d82e0("save_file_name","Save file name");
    local_758 = (undefined8 ****)0x0;
    uStack_750 = 0;
    local_748 = 0;
    local_740 = 0;
    lVar15 = -1;
    do {
      lVar15 = lVar15 + 1;
    } while (*(char *)(lVar5 + lVar15) != '\0');
    FUN_140002c00(&local_758,lVar5);
    local_698 = param_2;
    FUN_140019ca0(local_690,&local_758);
    FUN_140019ca0(local_670,&local_738);
    local_640 = 0;
    local_634 = 0;
    uStack_62c = 0;
    local_624 = 1;
    local_638 = 0x860;
    local_650 = _DAT_140a82210;
    uStack_648 = _UNK_140a82218;
    FUN_1405f4e50(&local_698,&local_718,param_1);
    FUN_140002d30(local_670);
    FUN_140002d30(local_690);
    if (0xf < local_740) {
      FUN_140003040(&local_758,local_758);
    }
    if (0xf < local_720) {
      FUN_140003040(&local_738,local_738);
    }
  }
  uVar6 = *param_2;
  FUN_1402d82e0("error_writing_file_desc","The system does not allow to write in the selected file")
  ;
  FUN_1402d82e0("error_writing_file","Error writing to file");
  FUN_14072f2e0(uVar6,param_2 + 0x13);
  uVar6 = FUN_1402dc390(param_1 + 0x18,&local_6e8);
  FUN_140247b90(&local_718,uVar6);
  FUN_140498e70(local_6b8,&local_718);
  if (7 < local_700) {
    _Memory = local_718;
    if ((0xfff < local_700 * 2 + 2) &&
       (_Memory = (undefined8 ****)local_718[-1],
       0x1f < (ulonglong)((longlong)local_718 + (-8 - (longlong)_Memory)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(_Memory);
  }
  local_708 = (undefined8 ****)0x0;
  local_700 = 7;
  local_718 = (undefined8 ****)((ulonglong)local_718 & 0xffffffffffff0000);
  FUN_140025470(&local_6e8);
  local_738 = (char ****)FUN_1402d82e0("ovewrite_name","Overwrite {}?");
  ppppuStack_730 = (undefined8 ****)0xffffffffffffffff;
  do {
    ppppuStack_730 = (undefined8 ****)((longlong)ppppuStack_730 + 1);
  } while (*(char *)((longlong)local_738 + (longlong)ppppuStack_730) != '\0');
  local_718 = (undefined8 ****)local_6b8;
  if (0xf < local_6a0) {
    local_718 = (undefined8 ****)CONCAT62(local_6b8[0]._2_6_,local_6b8[0]._0_2_);
  }
  ppppuStack_710 = (undefined8 ****)local_6a8;
  ppppuStack_6f0 = &local_718;
  local_468 = 0;
  local_478 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
  local_470 = local_458;
  local_460 = 500;
  local_6f8 = 0xd;
  FUN_140022830(&local_478,&local_738,&local_6f8);
  local_758 = (undefined8 *****)0x0;
  uStack_750 = 0;
  local_748 = 0;
  local_740 = 0;
  FUN_140002c00(&local_758,local_470);
  local_478 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
  if (local_470 != local_458) {
    puVar11 = local_470;
    if ((0xfff < local_460) &&
       (puVar11 = *(undefined1 **)(local_470 + -8),
       (undefined1 *)0x1f < local_470 + (-8 - (longlong)puVar11))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(puVar11);
  }
  uVar6 = *param_2;
  local_738 = (char ****)FUN_1402d82e0("cancel","Cancel");
  ppppuStack_730 = (undefined8 ****)FUN_1402d82e0("overwrite","Overwrite");
  local_718 = (undefined8 ****)0x0;
  ppppuStack_710 = (undefined8 ****)0x0;
  local_708 = (undefined8 ****)0x0;
  local_718 = (undefined8 ****)thunk_FUN_140983da8(0x10);
  ppppuStack_710 = local_718 + 2;
  local_708 = ppppuStack_710;
  memmove(local_718,&local_738,0x10);
  pppppuVar8 = &local_758;
  if (0xf < local_740) {
    pppppuVar8 = (undefined8 *****)local_758;
  }
  uVar7 = FUN_1402d82e0("overwrite_file","Overwrite file");
  iVar4 = FUN_14072f090(uVar6,param_2 + 0x13,uVar7,pppppuVar8,&local_718,0);
  if (iVar4 == 0) {
    *(undefined4 *)(param_1 + 0x10) = 0;
    *(undefined4 *)(param_1 + 8) = 5;
  }
  else if (iVar4 == 1) {
    *(undefined4 *)(param_1 + 0x10) = 4;
  }
  if (0xf < local_740) {
    pppppuVar8 = (undefined8 *****)local_758;
    if ((0xfff < local_740 + 1) &&
       (pppppuVar8 = (undefined8 *****)local_758[-1],
       0x1f < (ulonglong)((longlong)local_758 + (-8 - (longlong)pppppuVar8)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pppppuVar8);
  }
  local_748 = 0;
  local_740 = 0xf;
  local_758 = (undefined8 ****)((ulonglong)local_758 & 0xffffffffffffff00);
  if (0xf < local_6a0) {
    pvVar2 = (void *)CONCAT62(local_6b8[0]._2_6_,local_6b8[0]._0_2_);
    pvVar9 = pvVar2;
    if ((0xfff < local_6a0 + 1) &&
       (pvVar9 = *(void **)((longlong)pvVar2 + -8),
       0x1f < (ulonglong)((longlong)pvVar2 + (-8 - (longlong)pvVar9)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pvVar9);
  }
  uVar6 = FUN_1402dc390(param_1 + 0x18,&local_718);
  FUN_140247b90(local_6b8,uVar6);
  FUN_140498e70(&local_6e8,local_6b8);
  if (7 < local_6a0) {
    pvVar2 = (void *)CONCAT62(local_6b8[0]._2_6_,local_6b8[0]._0_2_);
    pvVar9 = pvVar2;
    if ((0xfff < local_6a0 * 2 + 2) &&
       (pvVar9 = *(void **)((longlong)pvVar2 + -8),
       0x1f < (ulonglong)((longlong)pvVar2 + (-8 - (longlong)pvVar9)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pvVar9);
  }
  local_6a8 = 0;
  local_6a0 = 7;
  local_6b8[0]._0_2_ = 0;
  FUN_140025470(&local_718);
  local_6f8 = FUN_1402d82e0("ovewrite_other_desc",
                            "{} was saved with a different game version. Do you want to overwrite it, or rename it to keep its original contents, then save the game?"
                           );
  do {
    lVar17 = lVar17 + 1;
  } while (*(char *)(local_6f8 + lVar17) != '\0');
  local_718 = (undefined8 ****)&local_6e8;
  if (0xf < local_6d0) {
    local_718 = (undefined8 ****)CONCAT44(uStack_6e4,local_6e8);
  }
  ppppuStack_710 = (undefined8 ****)local_6d8;
  ppppuStack_730 = &local_718;
  local_248 = 0;
  local_258 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
  local_250 = local_238;
  local_240 = 500;
  local_738 = (char ****)0xd;
  ppppuStack_6f0 = (undefined8 ****)lVar17;
  FUN_140022830(&local_258,&local_6f8,&local_738);
  local_758 = (undefined8 *****)0x0;
  uStack_750 = 0;
  local_748 = 0;
  local_740 = 0;
  FUN_140002c00(&local_758,local_250);
  local_258 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
  if (local_250 != local_238) {
    puVar11 = local_250;
    if ((0xfff < local_240) &&
       (puVar11 = *(undefined1 **)(local_250 + -8),
       (undefined1 *)0x1f < local_250 + (-8 - (longlong)puVar11))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(puVar11);
  }
  uVar6 = *param_2;
  local_738 = (char ****)FUN_1402d82e0("cancel","Cancel");
  ppppuStack_730 = (undefined8 ****)FUN_1402d82e0("overwrite","Overwrite");
  local_728 = FUN_1402d82e0("rename_save","Rename and save");
  local_718 = (undefined8 ****)0x0;
  ppppuStack_710 = (undefined8 ****)0x0;
  local_708 = (undefined8 ****)0x0;
  local_718 = (undefined8 ****)thunk_FUN_140983da8(0x18);
  ppppuStack_710 = local_718 + 3;
  local_708 = ppppuStack_710;
  memmove(local_718,&local_738,0x18);
  pppppuVar8 = &local_758;
  if (0xf < local_740) {
    pppppuVar8 = (undefined8 *****)local_758;
  }
  uVar7 = FUN_1402d82e0("overwrite_other_version_file","Overwrite other version file");
  iVar4 = FUN_14072f090(uVar6,param_2 + 0x13,uVar7,pppppuVar8,&local_718,0);
  if (iVar4 == 0) {
    *(undefined4 *)(param_1 + 0x10) = 0;
    *(undefined4 *)(param_1 + 8) = 5;
  }
  else if (iVar4 == 1) {
    *(undefined4 *)(param_1 + 0x10) = 4;
  }
  else if (iVar4 == 2) {
    *(undefined4 *)(param_1 + 0x10) = 5;
  }
  if (0xf < local_740) {
    FUN_140003040(&local_758,local_758);
  }
  local_748 = 0;
  local_740 = 0xf;
  local_758 = (undefined8 ****)((ulonglong)local_758 & 0xffffffffffffff00);
  if (0xf < local_6d0) {
    pvVar2 = (void *)CONCAT44(uStack_6e4,local_6e8);
    pvVar9 = pvVar2;
    if ((0xfff < local_6d0 + 1) &&
       (pvVar9 = *(void **)((longlong)pvVar2 + -8),
       0x1f < (ulonglong)((longlong)pvVar2 + (-8 - (longlong)pvVar9)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pvVar9);
  }
  if (*(int *)(param_1 + 0x10) == 1) {
    cVar3 = FUN_1402469a0(param_1 + 0x18);
    if (cVar3 == '\0') {
      *(undefined4 *)(param_1 + 0x10) = 4;
    }
    else {
      FUN_14049d7d0(local_618);
      FUN_140246270(local_618,param_1 + 0x18);
      FUN_14049c7e0(local_618);
      *(uint *)(param_1 + 0x10) = (local_594 == 0xe6) + 2;
      FUN_140002d30(local_528);
      FUN_140002d30(local_548);
      FUN_140002d30(local_5d8);
      FUN_140002d30(local_5f8);
      FUN_140025470(local_618);
    }
  }
  iVar4 = *(int *)(param_1 + 0x10);
  if (iVar4 == 3) {
    pcVar12 = "Overwrite file";
    pcVar10 = "overwrite_file";
  }
  else {
    if (iVar4 != 2) {
      if (iVar4 == 4) {
        FUN_140247b90(&local_6e8,param_1 + 0x18);
        FUN_14073f270(param_3,&local_6e8);
        FUN_1405de980(param_1);
        *(undefined4 *)(param_1 + 8) = 0;
        *(undefined4 *)(param_1 + 0x10) = 0;
      }
      else if (iVar4 == 5) {
        FUN_140247b90(&local_738,param_1 + 0x18);
        uVar6 = FUN_1402dc390(&local_738,local_6b8);
        FUN_140247b90(&local_718,uVar6);
        FUN_140025470(local_6b8);
        uVar6 = FUN_1402f1ba0(&local_6e8,L"OLD ",&local_718);
        FUN_1402522e0(&local_718,uVar6);
        FUN_140025470(&local_6e8);
        pppppuVar8 = &local_718;
        if (7 < local_700) {
          pppppuVar8 = (undefined8 *****)local_718;
        }
        local_758 = (undefined8 ****)0x0;
        uStack_750 = 0;
        local_748 = 0;
        local_740 = 0;
        FUN_140025200(&local_758,pppppuVar8,local_708);
        FUN_1402dc1d0(&local_738);
        uVar6 = FUN_14026cc30(&local_738,&local_758);
        if (7 < local_740) {
          FUN_1400260a0(uVar6,local_758);
        }
        pppppcVar13 = &local_738;
        if (7 < local_720) {
          pppppcVar13 = (char *****)local_738;
        }
        lVar17 = param_1 + 0x18;
        if (7 < *(ulonglong *)(param_1 + 0x30)) {
          lVar17 = *(longlong *)(param_1 + 0x18);
        }
        iVar4 = FUN_140983b4c(lVar17,pppppcVar13);
        if (iVar4 == 0) {
          FUN_140247b90(&local_6e8,param_1 + 0x18);
          FUN_14073f270(param_3,&local_6e8);
        }
        else {
          uVar6 = *param_2;
          uVar7 = FUN_1402d82e0("error_writing_file","Error writing to file");
          FUN_14081d6d0(uVar6,uVar7,0,1);
        }
        FUN_1405de980(param_1);
        *(undefined4 *)(param_1 + 8) = 0;
        *(undefined4 *)(param_1 + 0x10) = 0;
        FUN_140025470(&local_718);
        FUN_140025470(&local_738);
      }
      goto LAB_1405e58a3;
    }
    pcVar12 = "Overwrite other version file";
    pcVar10 = "overwrite_other_version_file";
  }
  uVar6 = *param_2;
  uVar7 = FUN_1402d82e0(pcVar10,pcVar12);
  FUN_14081d6d0(uVar6,uVar7,0,1);
LAB_1405e58a3:
  piVar16 = *(int **)(param_3 + 0x180);
  piVar1 = *(int **)(param_3 + 0x188);
  if (piVar16 != piVar1) {
    piVar14 = piVar16 + 2;
    do {
      local_6e8 = *piVar16;
      local_6c0 = -1;
      cVar3 = (char)piVar14[8];
      if (cVar3 != -1) {
        if (cVar3 == '\0') {
          FUN_140019ca0(local_6e0,piVar14);
        }
        else {
          FUN_140247b90();
        }
        local_6c0 = cVar3 != '\0';
      }
      puVar11 = local_6e0;
      if (local_6c0 != '\0') {
        puVar11 = (undefined1 *)0x0;
      }
      if ((local_6e8 == 0) && (puVar11 != (undefined1 *)0x0)) {
        uVar6 = *param_2;
        uVar7 = FUN_1402d82e0("error_writing_file","Error writing to file");
        FUN_14081d6d0(uVar6,uVar7,0,1);
      }
      if (local_6c0 != -1) {
        if (local_6c0 == '\0') {
          FUN_140002d30(local_6e0);
        }
        else {
          FUN_140025470();
        }
      }
      piVar16 = piVar16 + 0xc;
      piVar14 = piVar14 + 0xc;
    } while (piVar16 != piVar1);
  }
  return;
}


// Incoming references
// 0xc2ffe4 DATA caller none
// 0x5ea86e UNCONDITIONAL_CALL caller 1405ea700
