// Candidate VA 1403a7830; RVA 0x3a7830
// Ghidra inferred prototype: undefined FUN_1403a7830()

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 FUN_1403a7830(undefined8 *param_1,undefined8 param_2)

{
  int iVar1;
  undefined4 uVar2;
  void *pvVar3;
  ulonglong uVar4;
  ulonglong uVar5;
  undefined8 uVar6;
  uint *puVar7;
  ulonglong *puVar8;
  undefined8 *puVar9;
  undefined1 *puVar10;
  void *pvVar11;
  uint local_7e8;
  uint uStack_7e4;
  undefined8 uStack_7e0;
  ulonglong local_7d8;
  ulonglong uStack_7d0;
  undefined *local_7c8;
  uint *puStack_7c0;
  ulonglong local_7b8;
  ulonglong uStack_7b0;
  uint local_7a8;
  undefined4 uStack_7a4;
  undefined4 uStack_7a0;
  undefined4 uStack_79c;
  ulonglong local_798;
  ulonglong uStack_790;
  uint local_788;
  undefined4 uStack_784;
  undefined4 uStack_780;
  undefined4 uStack_77c;
  ulonglong local_778;
  ulonglong uStack_770;
  void *local_768;
  undefined8 uStack_760;
  ulonglong local_758;
  ulonglong uStack_750;
  ulonglong local_748 [4];
  void *local_728;
  undefined8 uStack_720;
  ulonglong local_718;
  ulonglong uStack_710;
  void *local_708;
  undefined8 uStack_700;
  ulonglong local_6f8;
  ulonglong uStack_6f0;
  void *local_6e0;
  undefined8 uStack_6d8;
  ulonglong local_6d0;
  ulonglong uStack_6c8;
  undefined1 local_6c0 [32];
  undefined1 local_6a0 [40];
  undefined **local_678;
  undefined1 *local_670;
  undefined8 local_668;
  ulonglong local_660;
  undefined1 local_658 [512];
  undefined **local_458;
  undefined1 *local_450;
  undefined8 local_448;
  ulonglong local_440;
  undefined1 local_438 [512];
  undefined **local_238;
  undefined1 *local_230;
  undefined8 local_228;
  ulonglong local_220;
  undefined1 local_218 [528];
  
  uVar5 = _UNK_140aac918;
  uVar4 = _DAT_140aac910;
  if (*(char *)(param_1 + 1) == '\0') {
    FUN_140019ca0(param_2,param_1 + 2);
    return param_2;
  }
  uStack_6d8 = 0;
  local_6d0 = _DAT_140aac910;
  uStack_6c8 = _UNK_140aac918;
  local_6e0 = (void *)0x0;
  iVar1 = *(int *)(param_1 + 6);
  local_7a8 = (uint)((ulonglong)*param_1 >> 0x10);
  uVar2 = (undefined4)*param_1;
  if (iVar1 == 0) {
    uStack_784 = uStack_7e4;
    uStack_780 = (uint)uStack_7e0;
    uStack_77c = uStack_7e0._4_4_;
    local_778 = CONCAT44(uStack_7e4,uVar2) & 0xffffffff0000ffff;
    uStack_770 = CONCAT44(uStack_7e0._4_4_,(uint)uStack_7e0);
    local_7e8 = 0x22;
    uStack_7e4 = 0;
    uStack_7e0 = &local_788;
    local_228 = 0;
    local_238 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
    local_230 = local_218;
    local_220 = 500;
    local_7c8 = &DAT_140a60a18;
    puStack_7c0 = (uint *)0x5;
    local_788 = local_7a8;
    FUN_140022830(&local_238,&local_7c8,&local_7e8);
    local_7a8 = 0;
    uStack_7a4 = 0;
    uStack_7a0 = 0;
    uStack_79c = 0;
    local_798 = 0;
    uStack_790 = 0;
    FUN_140002c00(&local_7a8,local_230);
    local_238 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
    if (local_230 != local_218) {
      puVar10 = local_230;
      if ((0xfff < local_220) &&
         (puVar10 = *(undefined1 **)(local_230 + -8),
         (undefined1 *)0x1f < local_230 + (-8 - (longlong)puVar10))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(puVar10);
    }
    puVar9 = (undefined8 *)FUN_14029cb50(&local_7a8,0,&DAT_140a4d470);
    local_708 = (void *)*puVar9;
    uStack_700 = puVar9[1];
    local_6f8 = puVar9[2];
    uStack_6f0 = puVar9[3];
    *(undefined1 *)puVar9 = 0;
    puVar9[2] = 0;
    puVar9[3] = 0xf;
    uVar6 = FUN_1402d82e0("signal_kind_name_oneway","One way");
    FUN_14029a380(param_2,uVar6);
    if (0xf < uStack_6f0) {
      pvVar11 = local_708;
      if ((0xfff < uStack_6f0 + 1) &&
         (pvVar11 = *(void **)((longlong)local_708 + -8),
         0x1f < (ulonglong)((longlong)local_708 + (-8 - (longlong)pvVar11)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pvVar11);
    }
    local_6f8 = uVar4;
    uStack_6f0 = uVar5;
    local_708 = (void *)((ulonglong)local_708 & 0xffffffffffffff00);
    if (0xf < uStack_790) {
      pvVar3 = (void *)CONCAT44(uStack_7a4,local_7a8);
      pvVar11 = pvVar3;
      if ((0xfff < uStack_790 + 1) &&
         (pvVar11 = *(void **)((longlong)pvVar3 + -8),
         0x1f < (ulonglong)((longlong)pvVar3 + (-8 - (longlong)pvVar11)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pvVar11);
    }
    local_798 = uVar4;
    uStack_790 = uVar5;
    local_7a8 = local_7a8 & 0xffffff00;
    if (uVar5 < 0x10) {
      return param_2;
    }
    pvVar11 = local_6e0;
    if ((0xfff < uVar5 + 1) &&
       (pvVar11 = *(void **)((longlong)local_6e0 + -8),
       0x1f < (ulonglong)((longlong)local_6e0 + (-8 - (longlong)pvVar11)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
  }
  else if (iVar1 == 3) {
    uStack_7a4 = uStack_7e4;
    uStack_7a0 = (uint)uStack_7e0;
    uStack_79c = uStack_7e0._4_4_;
    local_798 = CONCAT44(uStack_7e4,uVar2) & 0xffffffff0000ffff;
    uStack_790 = CONCAT44(uStack_7e0._4_4_,(uint)uStack_7e0);
    local_7e8 = 0x22;
    uStack_7e4 = 0;
    uStack_7e0 = &local_7a8;
    local_448 = 0;
    local_458 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
    local_450 = local_438;
    local_440 = 500;
    local_7c8 = &DAT_140a60a18;
    puStack_7c0 = (uint *)0x5;
    FUN_140022830(&local_458,&local_7c8,&local_7e8);
    local_788 = 0;
    uStack_784 = 0;
    uStack_780 = 0;
    uStack_77c = 0;
    local_778 = 0;
    uStack_770 = 0;
    FUN_140002c00(&local_788,local_450);
    local_458 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
    if (local_450 != local_438) {
      puVar10 = local_450;
      if ((0xfff < local_440) &&
         (puVar10 = *(undefined1 **)(local_450 + -8),
         (undefined1 *)0x1f < local_450 + (-8 - (longlong)puVar10))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(puVar10);
    }
    puVar9 = (undefined8 *)FUN_14029cb50(&local_788,0,&DAT_140a4d470);
    local_728 = (void *)*puVar9;
    uStack_720 = puVar9[1];
    local_718 = puVar9[2];
    uStack_710 = puVar9[3];
    *(undefined1 *)puVar9 = 0;
    puVar9[2] = 0;
    puVar9[3] = 0xf;
    uVar6 = FUN_1402d82e0("signal_kind_block_balise","Block balise");
    FUN_14029a380(param_2,uVar6);
    if (0xf < uStack_710) {
      pvVar11 = local_728;
      if ((0xfff < uStack_710 + 1) &&
         (pvVar11 = *(void **)((longlong)local_728 + -8),
         0x1f < (ulonglong)((longlong)local_728 + (-8 - (longlong)pvVar11)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pvVar11);
    }
    local_718 = uVar4;
    uStack_710 = uVar5;
    local_728 = (void *)((ulonglong)local_728 & 0xffffffffffffff00);
    if (0xf < uStack_770) {
      pvVar3 = (void *)CONCAT44(uStack_784,local_788);
      pvVar11 = pvVar3;
      if ((0xfff < uStack_770 + 1) &&
         (pvVar11 = *(void **)((longlong)pvVar3 + -8),
         0x1f < (ulonglong)((longlong)pvVar3 + (-8 - (longlong)pvVar11)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pvVar11);
    }
    local_778 = uVar4;
    uStack_770 = uVar5;
    local_788 = local_788 & 0xffffff00;
    if (uVar5 < 0x10) {
      return param_2;
    }
    pvVar11 = local_6e0;
    if ((0xfff < uVar5 + 1) &&
       (pvVar11 = *(void **)((longlong)local_6e0 + -8),
       0x1f < (ulonglong)
              ((longlong)local_6e0 + (-8 - (longlong)*(void **)((longlong)local_6e0 + -8))))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
  }
  else {
    if (iVar1 != 4) {
      if (iVar1 == 5) {
        uVar6 = FUN_140487640(local_6a0);
        puVar8 = (ulonglong *)FUN_14029cb50(uVar6,0,&DAT_140a4d470,1);
        local_7c8 = (undefined *)*puVar8;
        puStack_7c0 = (uint *)puVar8[1];
        local_7b8 = puVar8[2];
        uStack_7b0 = puVar8[3];
        *(undefined1 *)puVar8 = 0;
        puVar8[2] = 0;
        puVar8[3] = 0xf;
        uVar6 = FUN_1402d82e0("signal_kind_name_noway","No way");
        FUN_14029a380(param_2,uVar6,&local_7c8);
        if (0xf < uStack_7b0) {
          FUN_140003040(&local_7c8,local_7c8);
        }
        local_7b8 = uVar4;
        uStack_7b0 = uVar5;
        local_7c8 = (undefined *)((ulonglong)local_7c8 & 0xffffffffffffff00);
        FUN_140002d30(local_6a0);
        return param_2;
      }
      if (iVar1 != 6) {
        FUN_1404875b0(param_2);
        return param_2;
      }
      uVar6 = FUN_140487640(local_6c0);
      puVar7 = (uint *)FUN_14029cb50(uVar6,0,&DAT_140a4d470,1);
      local_7e8 = *puVar7;
      uStack_7e4 = puVar7[1];
      uStack_7e0._0_4_ = puVar7[2];
      uStack_7e0._4_4_ = puVar7[3];
      local_7d8 = *(ulonglong *)(puVar7 + 4);
      uStack_7d0 = *(ulonglong *)(puVar7 + 6);
      *(undefined1 *)puVar7 = 0;
      puVar7[4] = 0;
      puVar7[5] = 0;
      puVar7[6] = 0xf;
      puVar7[7] = 0;
      uVar6 = FUN_1402d82e0("signal_kind_name_marker","Marker");
      FUN_14029a380(param_2,uVar6,&local_7e8);
      if (0xf < uStack_7d0) {
        FUN_140003040(&local_7e8,CONCAT44(uStack_7e4,local_7e8));
      }
      local_7d8 = uVar4;
      uStack_7d0 = uVar5;
      local_7e8 = local_7e8 & 0xffffff00;
      FUN_140002d30(local_6c0);
      return param_2;
    }
    uStack_7a4 = uStack_7e4;
    uStack_7a0 = (uint)uStack_7e0;
    uStack_79c = uStack_7e0._4_4_;
    local_798 = CONCAT44(uStack_7e4,uVar2) & 0xffffffff0000ffff;
    uStack_790 = CONCAT44(uStack_7e0._4_4_,(uint)uStack_7e0);
    puStack_7c0 = &local_7a8;
    local_7e8 = 0x40a60a18;
    uStack_7e4 = 1;
    uStack_7e0._0_4_ = 5;
    uStack_7e0._4_4_ = 0;
    local_668 = 0;
    local_678 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
    local_670 = local_658;
    local_660 = 500;
    local_7c8 = (undefined *)0x22;
    FUN_140022830(&local_678,&local_7e8,&local_7c8);
    local_748[0] = 0;
    local_748[1] = 0;
    local_748[2] = 0;
    local_748[3] = 0;
    FUN_140002c00(local_748,local_670);
    local_678 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
    if (local_670 != local_658) {
      puVar10 = local_670;
      if ((0xfff < local_660) &&
         (puVar10 = *(undefined1 **)(local_670 + -8),
         (undefined1 *)0x1f < local_670 + (-8 - (longlong)puVar10))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(puVar10);
    }
    puVar9 = (undefined8 *)FUN_14029cb50(local_748,0,&DAT_140a4d470);
    local_768 = (void *)*puVar9;
    uStack_760 = puVar9[1];
    local_758 = puVar9[2];
    uStack_750 = puVar9[3];
    *(undefined1 *)puVar9 = 0;
    puVar9[2] = 0;
    puVar9[3] = 0xf;
    uVar6 = FUN_1402d82e0("signal_kind_path","Path signal");
    FUN_14029a380(param_2,uVar6);
    if (0xf < uStack_750) {
      pvVar11 = local_768;
      if ((0xfff < uStack_750 + 1) &&
         (pvVar11 = *(void **)((longlong)local_768 + -8),
         0x1f < (ulonglong)((longlong)local_768 + (-8 - (longlong)pvVar11)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pvVar11);
    }
    local_758 = uVar4;
    uStack_750 = uVar5;
    local_768 = (void *)((ulonglong)local_768 & 0xffffffffffffff00);
    if (0xf < local_748[3]) {
      FUN_140003040(local_748,local_748[0]);
    }
    local_748[2] = uVar4;
    local_748[3] = uVar5;
    local_748[0] = local_748[0] & 0xffffffffffffff00;
    if (uVar5 < 0x10) {
      return param_2;
    }
    pvVar11 = local_6e0;
    uStack_7e0 = (uint *)CONCAT44(uStack_7e0._4_4_,(uint)uStack_7e0);
    if ((0xfff < uVar5 + 1) &&
       (pvVar11 = *(void **)((longlong)local_6e0 + -8),
       uStack_7e0 = (uint *)CONCAT44(uStack_7e0._4_4_,(uint)uStack_7e0),
       0x1f < (ulonglong)
              ((longlong)local_6e0 + (-8 - (longlong)*(void **)((longlong)local_6e0 + -8))))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
  }
  free(pvVar11);
  return param_2;
}


// Incoming references
// 0xc1ebf0 DATA caller none
// 0x577552 UNCONDITIONAL_CALL caller 140577230
// 0x584b1b UNCONDITIONAL_CALL caller 140584a10
// 0x6717f2 UNCONDITIONAL_CALL caller 1406715e0
// 0x75564c UNCONDITIONAL_CALL caller 140755460
// 0x79ee3a UNCONDITIONAL_CALL caller 14079ed90
// 0x7a1031 UNCONDITIONAL_CALL caller 1407a0a40
// 0x6ba786 UNCONDITIONAL_CALL caller 1406b9c40
