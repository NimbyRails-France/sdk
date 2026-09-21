// Candidate VA 14057c820; RVA 0x57c820
// Ghidra inferred prototype: undefined FUN_14057c820()

void FUN_14057c820(undefined8 *param_1,longlong param_2,undefined8 param_3,undefined8 param_4,
                  undefined8 *param_5)

{
  undefined8 *puVar1;
  uint uVar2;
  longlong lVar3;
  undefined8 *puVar4;
  undefined8 *puVar5;
  char *_Dst;
  undefined1 *_Memory;
  char *_Memory_00;
  undefined8 *puVar6;
  longlong lVar7;
  ulonglong _Size;
  undefined8 uVar8;
  undefined8 *puVar9;
  undefined8 *local_res8;
  char *local_2c8;
  char *pcStack_2c0;
  char *local_2b8;
  ulonglong local_2b0;
  longlong local_2a8;
  undefined8 *puStack_2a0;
  void *local_298;
  void *local_290;
  undefined8 local_280 [2];
  undefined8 local_270;
  undefined **local_258;
  undefined1 *local_250;
  undefined8 local_248;
  ulonglong local_240;
  undefined1 local_238 [512];

  *param_1 = param_3;
  param_1[1] = param_4;
  lVar3 = param_1[0xc];
  for (lVar7 = param_1[0xb]; lVar7 != lVar3; lVar7 = lVar7 + 0x30) {
    FUN_140002d30(lVar7);
  }
  param_1[0xc] = param_1[0xb];
  puVar1 = (undefined8 *)param_5[1];
  puVar9 = (undefined8 *)*param_5;
  local_res8 = puVar1;
  do {
    if (puVar9 == puVar1) {
      _Dst = (char *)0x0;
      *(undefined1 *)((longlong)param_1 + 0x71) = *(undefined1 *)(param_1 + 0xe);
      param_1[0x24] = 0;
      local_280[0] = *param_1;
      local_270 = param_1[1];
      puStack_2a0 = local_280;
      local_248 = 0;
      local_258 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
      local_250 = local_238;
      local_240 = 500;
      local_2a8 = 0x33;
      local_2c8 = "##ScriptStructInstancesTabs_{}_{}";
      pcStack_2c0 = (char *)0x21;
      FUN_140022830(&local_258,&local_2c8,&local_2a8);
      local_2c8 = (char *)0x0;
      pcStack_2c0 = (char *)0x0;
      local_2b8 = (char *)0x0;
      local_2b0 = 0;
      FUN_140002c00(&local_2c8,local_250);
      local_258 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
      if (local_250 != local_238) {
        _Memory = local_250;
        if ((0xfff < local_240) &&
           (_Memory = *(undefined1 **)(local_250 + -8),
           (undefined1 *)0x1f < local_250 + (-8 - (longlong)_Memory))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(_Memory);
      }
      FUN_140025630(param_1 + 0x26,&local_2c8);
      if (0xf < local_2b0) {
        _Memory_00 = local_2c8;
        if ((0xfff < local_2b0 + 1) &&
           (_Memory_00 = *(char **)(local_2c8 + -8),
           (char *)0x1f < local_2c8 + (-8 - (longlong)_Memory_00))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(_Memory_00);
      }
      *(undefined1 *)((longlong)param_1 + 0x151) = 0;
      *(undefined4 *)(param_1 + 0x25) = 0x43c60000;
      FUN_1402531c0(param_1 + 0x1e);
      param_1[0x2c] = param_1[0x2b];
      *(undefined1 *)(param_1 + 0x2e) = 0;
      *(undefined1 *)(param_1 + 0x30) = 0;
      *(undefined1 *)(param_1 + 0x32) = 0;
      FUN_140362640(param_1 + 0x12,&local_2c8);
      FUN_140424090(param_2 + 0x1508,&local_298,param_2,*param_1,param_1[1]);
      lVar3 = (longlong)local_290 - (longlong)local_298 >> 4;
      lVar7 = lVar3 * 0x10;
      if (lVar3 != 0) {
        _Dst = (char *)thunk_FUN_140983da8(lVar7);
      }
      local_2b8 = _Dst + lVar7;
      local_2c8 = _Dst;
      if (local_298 != local_290) {
        _Size = (longlong)local_290 - (longlong)local_298 & 0xfffffffffffffff0;
        pcStack_2c0 = _Dst;
        memmove(_Dst,local_298,_Size);
        _Dst = _Dst + _Size;
      }
      pcStack_2c0 = _Dst;
      FUN_14057c0a0(param_1 + 0x37,param_2,&local_2c8,param_5);
      if (param_1 + 0xf != param_1 + 8) {
        FUN_140342d30(param_1 + 0xf,param_1[8],param_1[9]);
      }
      if (local_298 != (void *)0x0) {
        free(local_298);
      }
      return;
    }
    uVar8 = *puVar9;
    lVar3 = FUN_14033f6a0(param_2 + 0x300,uVar8);
    if (((lVar3 != 0) && (lVar3 = FUN_1402a2e80(param_2 + 0x1518,uVar8), lVar3 != 0)) &&
       (puVar4 = (undefined8 *)FUN_1402a7900(lVar3 + 0x70,puVar9[8]), puVar4 != (undefined8 *)0x0))
    {
      puVar5 = (undefined8 *)FUN_1405855c0(param_1 + 0xb);
      puVar5[4] = *puVar9;
      puVar5[5] = puVar9[8];
      uVar2 = FUN_1402a67a0(puVar4 + 0xb);
      if ((((ulonglong)uVar2 <
            (ulonglong)(((longlong)(puVar4[0xc] - puVar4[0xb]) >> 3) * 0x6db6db6db6db6db7)) &&
          (puVar6 = (undefined8 *)((ulonglong)uVar2 * 0x38 + 8 + puVar4[0xb]),
          puVar6 != (undefined8 *)0x0)) && (*(char *)(puVar6 + 4) == '\x02')) {
        if (puVar5 != puVar6) {
          uVar8 = puVar6[2];
          puVar4 = puVar6;
          if (0xf < (ulonglong)puVar6[3]) {
            puVar4 = (undefined8 *)*puVar6;
          }
LAB_14057c994:
          FUN_140030630(puVar5,puVar4,uVar8);
        }
      }
      else if (puVar5 != puVar4) {
        uVar8 = puVar4[2];
        if (0xf < (ulonglong)puVar4[3]) {
          puVar4 = (undefined8 *)*puVar4;
        }
        goto LAB_14057c994;
      }
      local_2a8 = puVar5[4];
      puStack_2a0 = (undefined8 *)puVar5[5];
      puVar4 = param_1 + 2;
      puVar6 = puVar4;
      puVar5 = (undefined8 *)param_1[4];
      while (puVar5 != (undefined8 *)0x0) {
        if (((longlong)puVar5[4] < local_2a8) ||
           (((longlong)puVar5[4] <= local_2a8 && ((ulonglong)puVar5[5] < puStack_2a0)))) {
          puVar5 = (undefined8 *)*puVar5;
        }
        else {
          puVar6 = puVar5;
          puVar5 = (undefined8 *)puVar5[1];
        }
      }
      if (((puVar6 == puVar4) || (local_2a8 < (longlong)puVar6[4])) ||
         ((local_2a8 <= (longlong)puVar6[4] && (puStack_2a0 < (ulonglong)puVar6[5])))) {
        puVar6 = (undefined8 *)FUN_140586c70(puVar4,&local_res8,local_2a8,puVar6,&local_2a8);
        puVar6 = (undefined8 *)*puVar6;
      }
      FUN_140578690(puVar6 + 6,param_2,param_3,param_4,puVar9);
    }
    puVar9 = puVar9 + 10;
  } while( true );
}


// Incoming references
// 0xc2e544 DATA caller none
// 0x57fb88 UNCONDITIONAL_CALL caller 14057ea10
// 0x5ad403 UNCONDITIONAL_CALL caller 1405ad160
// 0x5b1d0f UNCONDITIONAL_CALL caller 1405b1360
// 0x66f7ef UNCONDITIONAL_CALL caller 14066f4e0
// 0x6f7982 UNCONDITIONAL_CALL caller 1406f7760
// 0x6f59cb UNCONDITIONAL_CALL caller 1406f57b0
// 0x757b31 UNCONDITIONAL_CALL caller 140757a90
// 0x79fb8d UNCONDITIONAL_CALL caller 14079f4d0
// 0x7fdc09 UNCONDITIONAL_CALL caller 1407fd860
