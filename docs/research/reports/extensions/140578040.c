// Candidate VA 140578040; RVA 0x578040
// Ghidra inferred prototype: undefined FUN_140578040()

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140578040(longlong param_1)

{
  undefined8 *puVar1;
  undefined8 uVar2;
  ulonglong uVar3;
  ulonglong *puVar4;
  undefined1 *puVar5;
  void *pvVar6;
  void *pvVar7;
  longlong lVar8;
  ulonglong uVar9;
  ulonglong *puVar10;
  void **ppvVar11;
  ulonglong uVar12;
  ulonglong uVar13;
  undefined *local_5a8;
  undefined8 uStack_5a0;
  undefined8 local_598;
  ulonglong uStack_590;
  ulonglong local_588;
  undefined4 local_580;
  undefined4 uStack_57c;
  undefined4 uStack_578;
  undefined4 uStack_574;
  undefined8 local_570;
  ulonglong uStack_568;
  void *local_560;
  ulonglong uStack_558;
  ulonglong local_550;
  ulonglong uStack_548;
  void *local_540;
  ulonglong uStack_538;
  ulonglong local_530;
  ulonglong uStack_528;
  undefined *local_518;
  undefined8 uStack_510;
  undefined4 local_508;
  undefined4 uStack_504;
  undefined **ppuStack_500;
  undefined8 local_4f8;
  undefined **ppuStack_4f0;
  undefined *local_4e8;
  undefined8 uStack_4e0;
  undefined8 local_4d8;
  undefined **ppuStack_4d0;
  undefined *local_4c8;
  undefined8 uStack_4c0;
  undefined *local_4b8;
  undefined8 uStack_4b0;
  ulonglong *local_4a8 [2];
  undefined4 local_498;
  undefined4 uStack_494;
  undefined4 uStack_490;
  undefined4 uStack_48c;
  ulonglong *local_488 [2];
  undefined **local_478;
  undefined1 *local_470;
  undefined8 local_468;
  ulonglong local_460;
  undefined1 local_458 [512];
  undefined **local_258;
  undefined1 *local_250;
  undefined8 local_248;
  ulonglong local_240;
  undefined1 local_238 [528];

  uVar3 = _UNK_140aac918;
  uVar2 = _DAT_140aac910;
  uVar12 = 0;
  lVar8 = *(longlong *)(param_1 + 0x70);
  uVar13 = uVar12;
  if (*(longlong *)(param_1 + 0x78) - lVar8 >> 4 != 0) {
    do {
      puVar1 = (undefined8 *)(uVar13 + lVar8);
      if (puVar1 != (undefined8 *)0x0) {
        if (*(char *)(puVar1 + 1) == '\0') {
          local_518 = (undefined *)*puVar1;
          uStack_510 = uStack_4b0;
          local_4f8 = 3;
          ppuStack_4f0 = &local_518;
          local_4e8 = &DAT_140a49948;
          uStack_4e0 = 2;
          local_468 = 0;
          local_478 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
          local_470 = local_458;
          local_460 = 500;
          local_508 = 3;
          uStack_504 = 0;
          local_5a8 = &DAT_140a49948;
          uStack_5a0 = 2;
          local_4b8 = local_518;
          ppuStack_500 = ppuStack_4f0;
          FUN_140022830(&local_478,&local_5a8,&local_508);
          local_560 = (void *)0x0;
          uStack_558 = 0;
          local_550 = 0;
          uStack_548 = 0;
          FUN_140002c00(&local_560,local_470);
          local_478 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
          if (local_470 != local_458) {
            puVar5 = local_470;
            if ((0xfff < local_460) &&
               (puVar5 = *(undefined1 **)(local_470 + -8),
               (undefined1 *)0x1f < local_470 + (-8 - (longlong)puVar5))) {
                    /* WARNING: Subroutine does not return */
              _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
            }
            free(puVar5);
          }
          puVar10 = (ulonglong *)(*(longlong *)(param_1 + 0x88) + uVar13);
          puVar4 = (ulonglong *)FUN_140586ba0(param_1 + 200,puVar10);
          if ((puVar4 == *(ulonglong **)(param_1 + 0xd0)) || (*puVar10 < *puVar4)) {
            uStack_5a0 = 0;
            local_588 = *puVar10;
            local_580 = 0;
            uStack_57c = 0;
            uStack_578 = 0;
            uStack_574 = 0;
            local_570 = uVar2;
            uStack_568 = uVar3;
            local_598 = uVar2;
            uStack_590 = uVar3;
            local_5a8 = (undefined *)0x0;
            if (((puVar4 == *(ulonglong **)(param_1 + 0xd0)) || (local_588 < *puVar4)) &&
               ((puVar4 == *(ulonglong **)(param_1 + 200) || (puVar4[-5] < local_588)))) {
              puVar4 = (ulonglong *)FUN_140587e50(param_1 + 200,puVar4);
            }
            else {
              FUN_140587bb0(param_1 + 200,local_4a8);
              puVar4 = local_4a8[0];
            }
            if (0xf < uStack_568) {
              pvVar7 = (void *)CONCAT44(uStack_57c,local_580);
              pvVar6 = pvVar7;
              if ((0xfff < uStack_568 + 1) &&
                 (pvVar6 = *(void **)((longlong)pvVar7 + -8),
                 0x1f < (ulonglong)((longlong)pvVar7 + (-8 - (longlong)pvVar6)))) {
                    /* WARNING: Subroutine does not return */
                _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
              }
              free(pvVar6);
            }
          }
          ppvVar11 = (void **)(puVar4 + 1);
          uVar9 = uStack_548;
          pvVar6 = local_560;
          if (ppvVar11 != &local_560) {
            if (0xf < puVar4[4]) {
              pvVar6 = *ppvVar11;
              pvVar7 = pvVar6;
              if ((0xfff < puVar4[4] + 1) &&
                 (pvVar7 = *(void **)((longlong)pvVar6 - 8),
                 0x1f < (ulonglong)((longlong)pvVar6 + (-8 - (longlong)pvVar7)))) {
                    /* WARNING: Subroutine does not return */
                _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
              }
              free(pvVar7);
            }
            *ppvVar11 = local_560;
            puVar4[2] = uStack_558;
            puVar4[3] = local_550;
            puVar4[4] = uStack_548;
            uVar9 = 0xf;
            local_560 = (void *)((ulonglong)local_560 & 0xffffffffffffff00);
            pvVar6 = local_560;
          }
        }
        else {
          if ((puVar1 == (undefined8 *)0x0) || (*(char *)(puVar1 + 1) != '\x01'))
          goto LAB_1405785a3;
          local_498 = *(undefined4 *)puVar1;
          uStack_494 = *(undefined4 *)((longlong)puVar1 + 4);
          local_5a8 = (undefined *)*puVar1;
          uStack_5a0 = CONCAT44(uStack_48c,uStack_490);
          local_4d8 = 10;
          ppuStack_4d0 = &local_5a8;
          local_4c8 = &DAT_140a83e50;
          uStack_4c0 = 6;
          local_248 = 0;
          local_258 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
          local_250 = local_238;
          local_240 = 500;
          local_508 = 10;
          uStack_504 = 0;
          local_518 = &DAT_140a83e50;
          uStack_510 = 6;
          ppuStack_500 = ppuStack_4d0;
          FUN_140022830(&local_258,&local_518,&local_508);
          local_540 = (void *)0x0;
          uStack_538 = 0;
          local_530 = 0;
          uStack_528 = 0;
          FUN_140002c00(&local_540,local_250);
          local_258 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
          if (local_250 != local_238) {
            puVar5 = local_250;
            if ((0xfff < local_240) &&
               (puVar5 = *(undefined1 **)(local_250 + -8),
               (undefined1 *)0x1f < local_250 + (-8 - (longlong)puVar5))) {
                    /* WARNING: Subroutine does not return */
              _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
            }
            free(puVar5);
          }
          puVar10 = (ulonglong *)(*(longlong *)(param_1 + 0x88) + uVar13);
          puVar4 = (ulonglong *)FUN_140586ba0(param_1 + 200,puVar10);
          if ((puVar4 == *(ulonglong **)(param_1 + 0xd0)) || (*puVar10 < *puVar4)) {
            uStack_5a0 = 0;
            local_588 = *puVar10;
            local_580 = 0;
            uStack_57c = 0;
            uStack_578 = 0;
            uStack_574 = 0;
            local_570 = uVar2;
            uStack_568 = uVar3;
            local_598 = uVar2;
            uStack_590 = uVar3;
            local_5a8 = (undefined *)0x0;
            if (((puVar4 == *(ulonglong **)(param_1 + 0xd0)) || (local_588 < *puVar4)) &&
               ((puVar4 == *(ulonglong **)(param_1 + 200) || (puVar4[-5] < local_588)))) {
              puVar4 = (ulonglong *)FUN_140587e50(param_1 + 200,puVar4);
            }
            else {
              FUN_140587bb0(param_1 + 200,local_488);
              puVar4 = local_488[0];
            }
            if (0xf < uStack_568) {
              pvVar7 = (void *)CONCAT44(uStack_57c,local_580);
              pvVar6 = pvVar7;
              if ((0xfff < uStack_568 + 1) &&
                 (pvVar6 = *(void **)((longlong)pvVar7 + -8),
                 0x1f < (ulonglong)((longlong)pvVar7 + (-8 - (longlong)pvVar6)))) {
                    /* WARNING: Subroutine does not return */
                _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
              }
              free(pvVar6);
            }
          }
          ppvVar11 = (void **)(puVar4 + 1);
          uVar9 = uStack_528;
          pvVar6 = local_540;
          if (ppvVar11 != &local_540) {
            if (0xf < puVar4[4]) {
              pvVar6 = *ppvVar11;
              pvVar7 = pvVar6;
              if ((0xfff < puVar4[4] + 1) &&
                 (pvVar7 = *(void **)((longlong)pvVar6 - 8),
                 0x1f < (ulonglong)((longlong)pvVar6 + (-8 - (longlong)pvVar7)))) {
                    /* WARNING: Subroutine does not return */
                _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
              }
              free(pvVar7);
            }
            *ppvVar11 = local_540;
            puVar4[2] = uStack_538;
            puVar4[3] = local_530;
            puVar4[4] = uStack_528;
            local_540 = (void *)((ulonglong)local_540 & 0xffffffffffffff00);
            uVar9 = 0xf;
            pvVar6 = local_540;
          }
        }
        if (0xf < uVar9) {
          pvVar7 = pvVar6;
          if ((0xfff < uVar9 + 1) &&
             (pvVar7 = *(void **)((longlong)pvVar6 + -8),
             0x1f < (ulonglong)((longlong)pvVar6 + (-8 - (longlong)pvVar7)))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pvVar7);
        }
      }
LAB_1405785a3:
      uVar12 = uVar12 + 1;
      lVar8 = *(longlong *)(param_1 + 0x70);
      uVar13 = uVar13 + 0x10;
    } while (uVar12 < (ulonglong)(*(longlong *)(param_1 + 0x78) - lVar8 >> 4));
  }
  return;
}


// Incoming references
// 0xc2e4f0 DATA caller none
// 0x578960 UNCONDITIONAL_CALL caller 140578690
