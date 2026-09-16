// Candidate VA 14036d0a0; RVA 0x36d0a0
// Ghidra inferred prototype: undefined FUN_14036d0a0()

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

char * FUN_14036d0a0(ulonglong *param_1,char *param_2,longlong param_3)

{
  ulonglong uVar1;
  longlong lVar2;
  size_t _Size;
  void *pvVar3;
  longlong *plVar4;
  longlong lVar5;
  undefined8 uVar6;
  undefined8 uVar7;
  undefined4 *_Dst;
  longlong lVar8;
  undefined8 *puVar9;
  undefined1 *puVar10;
  char *_Memory;
  void *pvVar11;
  ulonglong uVar12;
  longlong lVar13;
  double dVar14;
  ulonglong local_res8;
  undefined4 local_578;
  undefined4 uStack_574;
  undefined8 uStack_570;
  ulonglong local_568;
  ulonglong uStack_560;
  undefined8 local_558;
  undefined8 uStack_550;
  undefined4 local_548;
  undefined4 uStack_544;
  undefined4 uStack_540;
  undefined4 uStack_53c;
  char *local_538;
  undefined4 *puStack_530;
  undefined4 local_528;
  undefined4 uStack_524;
  undefined4 uStack_520;
  undefined4 uStack_51c;
  void *local_518;
  undefined8 uStack_510;
  undefined8 local_508;
  ulonglong uStack_500;
  void *local_4f0;
  undefined8 uStack_4e8;
  undefined8 local_4e0;
  ulonglong uStack_4d8;
  undefined1 local_4d0;
  undefined7 uStack_4cf;
  undefined8 local_4c0;
  ulonglong uStack_4b8;
  uint local_4b0;
  undefined8 local_4ac;
  undefined4 local_4a4;
  undefined8 local_4a0;
  undefined8 uStack_498;
  ulonglong local_490;
  undefined8 uStack_488;
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

  uVar7 = 0;
  local_res8 = *param_1;
  lVar8 = *(longlong *)(param_3 + 0x78);
  if (lVar8 == 0) {
LAB_14036d873:
    param_2[0] = '\0';
    param_2[1] = '\0';
    param_2[2] = '\0';
    param_2[3] = '\0';
    param_2[4] = '\0';
    param_2[5] = '\0';
    param_2[6] = '\0';
    param_2[7] = '\0';
    param_2[8] = '\0';
    param_2[9] = '\0';
    param_2[10] = '\0';
    param_2[0xb] = '\0';
    param_2[0xc] = '\0';
    param_2[0xd] = '\0';
    param_2[0xe] = '\0';
    param_2[0xf] = '\0';
    param_2[0x10] = '\a';
    param_2[0x11] = '\0';
    param_2[0x12] = '\0';
    param_2[0x13] = '\0';
    param_2[0x14] = '\0';
    param_2[0x15] = '\0';
    param_2[0x16] = '\0';
    param_2[0x17] = '\0';
    param_2[0x18] = '\x0f';
    param_2[0x19] = '\0';
    param_2[0x1a] = '\0';
    param_2[0x1b] = '\0';
    param_2[0x1c] = '\0';
    param_2[0x1d] = '\0';
    param_2[0x1e] = '\0';
    param_2[0x1f] = '\0';
    *(undefined4 *)param_2 = s_INVALID_140a5ebd0._0_4_;
    *(undefined2 *)(param_2 + 4) = s_INVALID_140a5ebd0._4_2_;
    param_2[6] = s_INVALID_140a5ebd0[6];
    param_2[7] = '\0';
    return param_2;
  }
  lVar13 = *(longlong *)(local_res8 + 0x428);
  plVar4 = (longlong *)FUN_14032c420(lVar13,lVar8);
  if (plVar4 == (longlong *)0x0) goto LAB_14036d873;
  lVar2 = plVar4[0x1a];
  if (lVar2 != 0) {
    lVar5 = FUN_14032c3b0(lVar13 + 0x80,lVar2);
    local_res8 = FUN_1403ca140(local_res8 + 0x430,lVar2);
    if ((lVar5 != 0) && (local_res8 != 0)) {
      uVar6 = uVar7;
      if (plVar4[2] != 0) {
        uVar6 = FUN_14032c420(lVar13);
      }
      if (plVar4[1] != 0) {
        uVar7 = FUN_14032c420(lVar13);
      }
      FUN_14038af60(plVar4,&local_518,uVar7,uVar6);
      puVar9 = (undefined8 *)(local_res8 + 8);
      if (*(char *)(lVar5 + 0x40) == '\0') {
        puVar9 = (undefined8 *)(lVar5 + 0x20);
      }
      _Size = puVar9[2];
      if (_Size == 0x7fffffffffffffff) {
                    /* WARNING: Subroutine does not return */
        FUN_140001c70();
      }
      if (0xf < (ulonglong)puVar9[3]) {
        puVar9 = (undefined8 *)*puVar9;
      }
      local_578 = 0;
      uStack_574 = 0;
      uStack_570._0_4_ = 0;
      uStack_570._4_4_ = 0;
      uVar1 = _Size + 1;
      uVar12 = 0xf;
      _Dst = &local_578;
      if (0xf < uVar1) {
        uVar12 = uVar1 | 0xf;
        if (uVar12 < 0x8000000000000000) {
          if (uVar12 < 0x16) {
            uVar12 = 0x16;
          }
        }
        else {
          uVar12 = 0x7fffffffffffffff;
        }
        _Dst = (undefined4 *)FUN_140003270(uVar12 + 1);
        local_578 = SUB84(_Dst,0);
        uStack_574 = (undefined4)((ulonglong)_Dst >> 0x20);
      }
      local_568 = uVar1;
      uStack_560 = uVar12;
      memcpy(_Dst,puVar9,_Size);
      *(char *)((longlong)_Dst + _Size) = (char)DAT_140a4d470;
      *(undefined1 *)((longlong)_Dst + uVar1) = 0;
      FUN_14029c760(param_2,local_res8 & 0xff);
      if (0xf < uStack_560) {
        pvVar3 = (void *)CONCAT44(uStack_574,local_578);
        pvVar11 = pvVar3;
        if ((0xfff < uStack_560 + 1) &&
           (pvVar11 = *(void **)((longlong)pvVar3 + -8),
           0x1f < (ulonglong)((longlong)pvVar3 + (-8 - (longlong)pvVar11)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pvVar11);
      }
      if (uStack_500 < 0x10) {
        return param_2;
      }
      pvVar11 = local_518;
      if ((0xfff < uStack_500 + 1) &&
         (pvVar11 = *(void **)((longlong)local_518 + -8),
         0x1f < (ulonglong)
                ((longlong)local_518 + (-8 - (longlong)*(void **)((longlong)local_518 + -8))))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      goto LAB_14036d86c;
    }
  }
  if ((*(char *)(param_3 + 0x89) == '\0') || (*plVar4 != lVar8)) {
    dVar14 = *(double *)(param_3 + 0x80);
  }
  else {
    dVar14 = *(double *)(param_3 + 0x80);
    if (*(char *)((longlong)plVar4 + 0x2c) != *(char *)(param_3 + 0x89)) {
      dVar14 = DAT_140aabd08 - dVar14;
    }
  }
  local_578 = SUB84(dVar14,0);
  uStack_574 = (undefined4)((ulonglong)dVar14 >> 0x20);
  uStack_570._0_4_ = (undefined4)uStack_550;
  uStack_570._4_4_ = uStack_550._4_4_;
  puStack_530 = &local_578;
  local_468 = 0;
  local_478 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
  local_470 = local_458;
  local_460 = 500;
  local_538 = (char *)0xa;
  local_558 = &DAT_140a6e034;
  uStack_550 = 6;
  FUN_140022830(&local_478,&local_558,&local_538);
  local_518 = (void *)0x0;
  uStack_510 = 0;
  local_508 = 0;
  uStack_500 = 0;
  FUN_140002c00(&local_518,local_470);
  local_478 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
  if (local_470 != local_458) {
    puVar10 = local_470;
    if ((0xfff < local_460) &&
       (puVar10 = *(undefined1 **)(local_470 + -8),
       (undefined1 *)0x1f < local_470 + (-8 - (longlong)puVar10))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(puVar10);
  }
  uVar7 = *(undefined8 *)(param_3 + 0x78);
  local_558 = (char *)(CONCAT44(local_558._4_4_,(int)uVar7) & 0xffffffff0000ffff);
  local_4b0 = (uint)(ushort)((ulonglong)uVar7 >> 0x30);
  local_4ac = CONCAT44((undefined4)uStack_570,uStack_574);
  local_4a4 = uStack_570._4_4_;
  local_4a0 = CONCAT44(uStack_574,(int)((ulonglong)uVar7 >> 0x10));
  uStack_498 = CONCAT44(uStack_570._4_4_,(undefined4)uStack_570);
  local_490 = (ulonglong)local_558;
  uStack_488 = uStack_550;
  uStack_570 = &local_4b0;
  local_538 = "{}:{}.{}";
  puStack_530 = (undefined4 *)0x8;
  local_248 = 0;
  local_258 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
  local_250 = local_238;
  local_240 = 500;
  local_578 = 0x222;
  uStack_574 = 0;
  local_558 = "{}:{}.{}";
  uStack_550 = 8;
  FUN_140022830(&local_258,&local_558,&local_578);
  local_578 = 0;
  uStack_574 = 0;
  uStack_570._0_4_ = 0;
  uStack_570._4_4_ = 0;
  local_568 = 0;
  uStack_560 = 0;
  FUN_140002c00(&local_578,local_250);
  local_258 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
  if (local_250 != local_238) {
    puVar10 = local_250;
    if ((0xfff < local_240) &&
       (puVar10 = *(undefined1 **)(local_250 + -8),
       (undefined1 *)0x1f < local_250 + (-8 - (longlong)puVar10))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(puVar10);
  }
  lVar8 = FUN_1402d82e0("waypoint","Waypoint");
  local_4f0 = (void *)0x0;
  uStack_4e8 = 0;
  local_4e0 = 0;
  uStack_4d8 = 0;
  lVar13 = -1;
  do {
    lVar13 = lVar13 + 1;
  } while (*(char *)(lVar8 + lVar13) != '\0');
  FUN_140002c00(&local_4f0,lVar8);
  puVar9 = (undefined8 *)FUN_1400254f0(&local_4f0,&DAT_140a4d470,1);
  local_538 = (char *)*puVar9;
  puStack_530 = (undefined4 *)puVar9[1];
  local_528 = *(undefined4 *)(puVar9 + 2);
  uStack_524 = *(undefined4 *)((longlong)puVar9 + 0x14);
  uStack_520 = *(undefined4 *)(puVar9 + 3);
  uStack_51c = *(undefined4 *)((longlong)puVar9 + 0x1c);
  puVar9[3] = 0xf;
  *(undefined1 *)puVar9 = 0;
  puVar9[2] = 0;
  FUN_14029c760(&local_4d0,local_res8 & 0xff,&local_538,&local_578);
  puVar9 = (undefined8 *)FUN_1400254f0(&local_4d0,&DAT_140a6e03c,3);
  local_558 = (char *)*puVar9;
  uStack_550 = puVar9[1];
  local_548 = *(undefined4 *)(puVar9 + 2);
  uStack_544 = *(undefined4 *)((longlong)puVar9 + 0x14);
  uStack_540 = *(undefined4 *)(puVar9 + 3);
  uStack_53c = *(undefined4 *)((longlong)puVar9 + 0x1c);
  *(undefined1 *)puVar9 = 0;
  puVar9[2] = 0;
  puVar9[3] = 0xf;
  FUN_14029c760(param_2,local_res8 & 0xff);
  if (0xf < CONCAT44(uStack_53c,uStack_540)) {
    pvVar11 = local_558;
    if ((0xfff < CONCAT44(uStack_53c,uStack_540) + 1) &&
       (pvVar11 = *(void **)((longlong)local_558 + -8),
       0x1f < (ulonglong)((longlong)local_558 + (-8 - (longlong)pvVar11)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pvVar11);
  }
  if (0xf < uStack_4b8) {
    pvVar3 = (void *)CONCAT71(uStack_4cf,local_4d0);
    pvVar11 = pvVar3;
    if ((0xfff < uStack_4b8 + 1) &&
       (pvVar11 = *(void **)((longlong)pvVar3 + -8),
       0x1f < (ulonglong)((longlong)pvVar3 + (-8 - (longlong)pvVar11)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pvVar11);
  }
  local_4c0 = _DAT_140aac910;
  uStack_4b8 = _UNK_140aac918;
  local_4d0 = 0;
  if (0xf < CONCAT44(uStack_51c,uStack_520)) {
    _Memory = local_538;
    if ((0xfff < CONCAT44(uStack_51c,uStack_520) + 1) &&
       (_Memory = *(char **)(local_538 + -8), (char *)0x1f < local_538 + (-8 - (longlong)_Memory)))
    {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(_Memory);
  }
  if (0xf < uStack_4d8) {
    pvVar11 = local_4f0;
    if ((0xfff < uStack_4d8 + 1) &&
       (pvVar11 = *(void **)((longlong)local_4f0 + -8),
       0x1f < (ulonglong)((longlong)local_4f0 + (-8 - (longlong)pvVar11)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pvVar11);
  }
  local_4f0 = (void *)((ulonglong)local_4f0 & 0xffffffffffffff00);
  local_4e0 = _DAT_140aac910;
  uStack_4d8 = _UNK_140aac918;
  if (0xf < uStack_560) {
    pvVar3 = (void *)CONCAT44(uStack_574,local_578);
    pvVar11 = pvVar3;
    if ((0xfff < uStack_560 + 1) &&
       (pvVar11 = *(void **)((longlong)pvVar3 + -8),
       0x1f < (ulonglong)((longlong)pvVar3 + (-8 - (longlong)pvVar11)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pvVar11);
  }
  if (uStack_500 < 0x10) {
    return param_2;
  }
  pvVar11 = local_518;
  if ((0xfff < uStack_500 + 1) &&
     (pvVar11 = *(void **)((longlong)local_518 + -8),
     0x1f < (ulonglong)((longlong)local_518 + (-8 - (longlong)pvVar11)))) {
                    /* WARNING: Subroutine does not return */
    _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
  }
LAB_14036d86c:
  free(pvVar11);
  return param_2;
}


// Incoming references
// 0xc1d1c8 DATA caller none
// 0x59051b UNCONDITIONAL_CALL caller 140590450
// 0x590daf UNCONDITIONAL_CALL caller 1405907a0
// 0x592d03 UNCONDITIONAL_CALL caller 140592c10
// 0x596c45 UNCONDITIONAL_CALL caller 1405961b0
// 0x5a8a1a UNCONDITIONAL_CALL caller 1405a86e0
// 0x5a9c14 UNCONDITIONAL_CALL caller 1405a9a40
// 0x5ab493 UNCONDITIONAL_CALL caller 1405aaba0
// 0x5ab84a UNCONDITIONAL_CALL caller 1405aaba0
// 0x5b017e UNCONDITIONAL_CALL caller 1405ada90
// 0x5b1233 UNCONDITIONAL_CALL caller 1405b0e60
// 0x6fdb6d UNCONDITIONAL_CALL caller 1406fd000
// 0x6fddbe UNCONDITIONAL_CALL caller 1406fd000
// 0x6fe09e UNCONDITIONAL_CALL caller 1406fd000
// 0x6ff97e UNCONDITIONAL_CALL caller 1406fd000
// 0x6ffdee UNCONDITIONAL_CALL caller 1406fd000
// 0x70035e UNCONDITIONAL_CALL caller 1406fd000
// 0x7036aa UNCONDITIONAL_CALL caller 140702780
// 0x703a9b UNCONDITIONAL_CALL caller 140702780
// 0x7040eb UNCONDITIONAL_CALL caller 140702780
// 0x806053 UNCONDITIONAL_CALL caller 140805710
// 0x6b0f23 UNCONDITIONAL_CALL caller 1406b0c40
// 0x6b0f82 UNCONDITIONAL_CALL caller 1406b0c40
// 0x6bb6fc UNCONDITIONAL_CALL caller 1406badf0
