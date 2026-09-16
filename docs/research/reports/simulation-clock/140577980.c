// Candidate VA 140577980; RVA 0x577980
// Ghidra inferred prototype: undefined FUN_140577980()

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

ulonglong FUN_140577980(longlong param_1,undefined8 param_2,longlong *param_3,undefined4 param_4,
                       undefined8 param_5,ulonglong param_6)

{
  longlong lVar1;
  code *pcVar2;
  ulonglong uVar3;
  longlong lVar4;
  undefined8 *puVar5;
  undefined8 uVar6;
  undefined1 *puVar7;
  void *pvVar8;
  undefined8 ****ppppuVar9;
  longlong lVar10;
  undefined8 *puVar11;
  undefined8 ***local_528;
  undefined8 uStack_520;
  undefined8 local_518;
  ulonglong uStack_510;
  undefined *local_508;
  ulonglong *puStack_500;
  ulonglong local_4f8;
  undefined8 **ppuStack_4f0;
  void *local_4e8;
  undefined8 uStack_4e0;
  undefined8 local_4d8;
  ulonglong uStack_4d0;
  undefined8 ***local_4c8 [2];
  undefined8 local_4b8;
  ulonglong local_4b0;
  void *local_4a8 [3];
  ulonglong local_490;
  undefined8 *local_488;
  undefined8 local_480;
  undefined8 ***local_478;
  undefined8 local_470;
  undefined **local_468;
  undefined1 *local_460;
  undefined8 local_458;
  ulonglong local_450;
  undefined1 local_448 [512];
  undefined **local_248;
  undefined1 *local_240;
  undefined8 local_238;
  ulonglong local_230;
  undefined1 local_228 [512];

  lVar4 = *(longlong *)(param_1 + 0x10);
  uVar3 = *(ulonglong *)(lVar4 + 0xb0);
  if (uVar3 <= param_6) {
    param_6 = param_6 - uVar3;
    uVar3 = (*(longlong *)(lVar4 + 0x20) - *(longlong *)(lVar4 + 0x10) >> 3) * 0x6db6db6db6db6db7 +
            -0x40 + (*(longlong *)(lVar4 + 0x30) - *(longlong *)(lVar4 + 0x38) >> 3) *
                    0x6db6db6db6db6db7 +
                    (*(longlong *)(lVar4 + 0x48) - *(longlong *)(lVar4 + 0x28) >> 3) * 0x40;
    if (param_6 < uVar3) {
      lVar1 = (*(longlong *)(lVar4 + 0x10) - *(longlong *)(lVar4 + 0x18) >> 3) * 0x6db6db6db6db6db7
              + param_6;
      lVar10 = lVar1 + 0x1000000;
      lVar10 = (longlong)((ulonglong)((uint)(lVar10 >> 0x3f) & 0x3f) + lVar10) >> 6;
      puVar11 = (undefined8 *)
                ((lVar1 + (lVar10 + -0x40000) * -0x40) * 0x38 +
                *(longlong *)(*(longlong *)(lVar4 + 0x28) + -0x200000 + lVar10 * 8));
      param_6._0_4_ = 0;
      lVar4 = FUN_14033f6a0(*(longlong *)(param_1 + 8) + 0x300,puVar11[2]);
      uVar3 = 0;
      if (lVar4 != 0) {
        *(undefined4 *)((longlong)param_3 + 0x34) = 0x42200000;
        *(undefined1 *)(param_3 + 6) = 1;
        *(undefined1 *)(param_3 + 3) = 1;
        *(undefined4 *)((longlong)param_3 + 0x1c) = 0xa0;
        *(undefined1 *)(param_3 + 7) = 1;
        *(undefined4 *)((longlong)param_3 + 0x3c) = param_4;
        param_3[8] = 0;
        *(undefined4 *)(param_3 + 9) = 0;
        *(undefined1 *)(param_3 + 4) = 1;
        *(undefined4 *)((longlong)param_3 + 0x24) = 3;
        (**(code **)(*param_3 + 0x10))(param_3,0,&param_6,&DAT_140a4d470);
        uVar3 = puVar11[1];
        *(ulonglong *)(param_1 + 0x30) = uVar3 / 10000;
        FUN_140572850(local_4c8,(*(longlong *)(param_1 + 0x28) * 100 + uVar3 / 10000) * 10000);
        local_4f8 = (ulonglong)puVar11[1] % 1000000;
        ppuStack_4f0 = (undefined8 **)uStack_520;
        puStack_500 = &local_4f8;
        local_458 = 0;
        local_468 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
        local_460 = local_448;
        local_450 = 500;
        local_508 = (undefined *)0x4;
        local_528 = (undefined8 ***)&DAT_140a83d88;
        uStack_520 = 7;
        FUN_140022830(&local_468,&local_528,&local_508);
        local_528 = (undefined8 ****)0x0;
        uStack_520 = 0;
        local_518 = 0;
        uStack_510 = 0;
        FUN_140002c00(&local_528,local_460);
        local_468 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
        if (local_460 != local_448) {
          puVar7 = local_460;
          if ((0xfff < local_450) &&
             (puVar7 = *(undefined1 **)(local_460 + -8),
             (undefined1 *)0x1f < local_460 + (-8 - (longlong)puVar7))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(puVar7);
        }
        ppppuVar9 = &local_528;
        if (0xf < uStack_510) {
          ppppuVar9 = (undefined8 ****)local_528;
        }
        FUN_1400254f0(local_4c8,ppppuVar9);
        if (0xf < uStack_510) {
          ppppuVar9 = (undefined8 ****)local_528;
          if ((0xfff < uStack_510 + 1) &&
             (ppppuVar9 = (undefined8 ****)local_528[-1],
             0x1f < (ulonglong)((longlong)local_528 + (-8 - (longlong)ppppuVar9)))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(ppppuVar9);
        }
        *(undefined4 *)((longlong)param_3 + 0x34) = 0x41800000;
        *(undefined1 *)(param_3 + 6) = 1;
        *(undefined1 *)(param_3 + 3) = 1;
        *(undefined4 *)((longlong)param_3 + 0x1c) = 0xa0;
        *(undefined1 *)(param_3 + 7) = 1;
        *(undefined4 *)((longlong)param_3 + 0x3c) = 0x40000000;
        *(undefined1 *)((longlong)param_3 + 0x4c) = 1;
        *(undefined4 *)(param_3 + 10) = 6;
        pcVar2 = *(code **)(*param_3 + 0xa0);
        puVar5 = (undefined8 *)FUN_140572550(local_4a8,*puVar11);
        local_488 = puVar5;
        if (0xf < (ulonglong)puVar5[3]) {
          local_488 = (undefined8 *)*puVar5;
        }
        local_480 = puVar5[2];
        local_478 = local_4c8;
        if (0xf < local_4b0) {
          local_478 = local_4c8[0];
        }
        local_470 = local_4b8;
        ppuStack_4f0 = &local_488;
        local_508 = &DAT_140a83d90;
        puStack_500 = (ulonglong *)0x8;
        local_238 = 0;
        local_248 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
        local_240 = local_228;
        local_230 = 500;
        local_4f8 = 0xdd;
        local_528 = (undefined8 ***)&DAT_140a83d90;
        uStack_520 = 8;
        FUN_140022830(&local_248,&local_528,&local_4f8);
        local_4e8 = (void *)0x0;
        uStack_4e0 = 0;
        local_4d8 = 0;
        uStack_4d0 = 0;
        FUN_140002c00(&local_4e8,local_240);
        local_248 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
        if (local_240 != local_228) {
          puVar7 = local_240;
          if ((0xfff < local_230) &&
             (puVar7 = *(undefined1 **)(local_240 + -8),
             (undefined1 *)0x1f < local_240 + (-8 - (longlong)puVar7))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(puVar7);
        }
        (*pcVar2)(param_3,&local_4e8);
        if (0xf < uStack_4d0) {
          pvVar8 = local_4e8;
          if ((0xfff < uStack_4d0 + 1) &&
             (pvVar8 = *(void **)((longlong)local_4e8 + -8),
             0x1f < (ulonglong)((longlong)local_4e8 + (-8 - (longlong)pvVar8)))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pvVar8);
        }
        local_4d8 = _DAT_140aac910;
        uStack_4d0 = _UNK_140aac918;
        local_4e8 = (void *)((ulonglong)local_4e8 & 0xffffffffffffff00);
        if (0xf < local_490) {
          pvVar8 = local_4a8[0];
          if ((0xfff < local_490 + 1) &&
             (pvVar8 = *(void **)((longlong)local_4a8[0] + -8),
             0x1f < (ulonglong)((longlong)local_4a8[0] + (-8 - (longlong)pvVar8)))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pvVar8);
        }
        *(undefined4 *)((longlong)param_3 + 0x34) = 0x41c00000;
        *(undefined1 *)(param_3 + 6) = 1;
        *(undefined1 *)(param_3 + 3) = 1;
        *(undefined4 *)((longlong)param_3 + 0x1c) = 0xa0;
        *(undefined1 *)(param_3 + 7) = 1;
        *(undefined4 *)((longlong)param_3 + 0x3c) = 0x40000000;
        *(undefined1 *)(param_3 + 4) = 1;
        *(undefined4 *)((longlong)param_3 + 0x24) = 2;
        (**(code **)(*param_3 + 8))(param_3);
        *(undefined4 *)((longlong)param_3 + 0x34) = 0x41800000;
        *(undefined1 *)(param_3 + 6) = 1;
        *(undefined1 *)(param_3 + 3) = 1;
        *(undefined4 *)((longlong)param_3 + 0x1c) = 0xa0;
        pcVar2 = *(code **)(*param_3 + 0xa0);
        uVar6 = FUN_1405775a0(param_1,local_4a8,puVar11);
        (*pcVar2)(param_3,uVar6);
        if (0xf < local_490) {
          pvVar8 = local_4a8[0];
          if ((0xfff < local_490 + 1) &&
             (pvVar8 = *(void **)((longlong)local_4a8[0] + -8),
             0x1f < (ulonglong)((longlong)local_4a8[0] + (-8 - (longlong)pvVar8)))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pvVar8);
        }
        (**(code **)(*param_3 + 0x18))(param_3);
        if (0xf < local_4b0) {
          ppppuVar9 = (undefined8 ****)local_4c8[0];
          if ((0xfff < local_4b0 + 1) &&
             (ppppuVar9 = (undefined8 ****)local_4c8[0][-1],
             0x1f < (ulonglong)((longlong)local_4c8[0] + (-8 - (longlong)ppppuVar9)))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(ppppuVar9);
        }
        uVar3 = (**(code **)(*param_3 + 0x18))(param_3);
        if ((int)param_6 != 0) {
          uVar6 = puVar11[1];
          *(undefined8 *)(param_1 + 0x38) = *puVar11;
          *(undefined8 *)(param_1 + 0x40) = uVar6;
          uVar6 = puVar11[3];
          *(undefined8 *)(param_1 + 0x48) = puVar11[2];
          *(undefined8 *)(param_1 + 0x50) = uVar6;
          uVar6 = puVar11[5];
          *(undefined8 *)(param_1 + 0x58) = puVar11[4];
          *(undefined8 *)(param_1 + 0x60) = uVar6;
          *(undefined8 *)(param_1 + 0x68) = puVar11[6];
          if (*(char *)(param_1 + 0x70) == '\0') {
            *(undefined1 *)(param_1 + 0x70) = 1;
          }
        }
      }
    }
  }
  return uVar3 & 0xffffffffffffff00;
}


// Incoming references
// 0xc2e4e4 DATA caller none
// 0xa83b18 DATA caller none
