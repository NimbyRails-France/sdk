// Candidate VA 140740420; RVA 0x740420
// Ghidra inferred prototype: undefined FUN_140740420()

undefined8 FUN_140740420(longlong param_1,undefined8 param_2)

{
  void *_Src;
  void *_Dst;
  undefined8 uVar1;
  undefined8 *puVar2;
  code *pcVar3;
  undefined8 *_Memory;
  int iVar4;
  undefined8 uVar5;
  longlong *plVar6;
  longlong *plVar7;
  undefined8 uVar8;
  longlong lVar9;
  longlong lVar10;
  ulonglong uVar11;
  longlong *plVar12;
  longlong *plVar13;
  void *pvVar14;
  longlong lVar15;
  size_t _Size;
  float fVar16;
  float fVar17;
  LARGE_INTEGER local_res8;
  undefined8 local_res10;
  longlong local_res18;
  undefined8 local_res20;
  longlong local_c8;
  undefined8 local_c0;
  undefined8 local_b8;
  undefined8 local_a8;
  longlong local_a0;
  undefined8 local_98;
  undefined8 local_90;
  longlong *local_88;
  longlong *local_80;
  undefined8 local_78;
  undefined1 local_70 [48];

  lVar15 = *(longlong *)(param_1 + 0x2e0);
  *(longlong *)(param_1 + 0x2e0) = 0;
  uVar5 = *(undefined8 *)(param_1 + 0x2e8);
  *(undefined8 *)(param_1 + 0x2e8) = 0;
  uVar8 = *(undefined8 *)(param_1 + 0x2f0);
  *(undefined8 *)(param_1 + 0x2f0) = 0;
  local_res10 = param_2;
  local_res18 = lVar15;
  local_res20 = uVar5;
  local_a8 = uVar8;
  local_a0 = lVar15;
  local_98 = uVar5;
  local_90 = uVar8;
  FUN_14073d670();
  iVar4 = _Mtx_lock();
  if (iVar4 != 0) {
    std::_Throw_Cpp_error(5);
    pcVar3 = (code *)swi(3);
    uVar5 = (*pcVar3)();
    return uVar5;
  }
  if (*(int *)(param_1 + 0xa8c) == 0x7fffffff) {
    *(undefined4 *)(param_1 + 0xa8c) = 0x7ffffffe;
    std::_Throw_Cpp_error(6);
    pcVar3 = (code *)swi(3);
    uVar5 = (*pcVar3)();
    return uVar5;
  }
  plVar13 = *(longlong **)(param_1 + 0x798);
  *(undefined8 *)(param_1 + 0x798) = 0;
  plVar12 = *(longlong **)(param_1 + 0x7a0);
  *(undefined8 *)(param_1 + 0x7a0) = 0;
  local_78 = *(undefined8 *)(param_1 + 0x7a8);
  *(undefined8 *)(param_1 + 0x7a8) = 0;
  local_88 = plVar13;
  local_80 = plVar12;
  FUN_14032a340(0,0);
  *(undefined8 *)(param_1 + 0x7a0) = *(undefined8 *)(param_1 + 0x798);
  _Mtx_unlock(param_1 + 0xa40);
  if (plVar13 != plVar12) {
    plVar13 = plVar13 + 0x101;
    do {
      if (plVar13[-2] == *(longlong *)(param_1 + 0x4e0)) {
        local_res8.QuadPart = 0;
        QueryPerformanceCounter(&local_res8);
        lVar15 = local_res8.QuadPart - plVar13[-1];
        plVar6 = *(longlong **)(param_1 + 0x300);
        if (plVar6 < *(longlong **)(param_1 + 0x308)) {
          *(longlong **)(param_1 + 0x300) = plVar6 + 1;
          *plVar6 = lVar15;
        }
        else {
          lVar9 = (longlong)plVar6 - *(longlong *)(param_1 + 0x2f8) >> 3;
          if (lVar9 == 0) {
            lVar10 = 1;
LAB_1407405a4:
            lVar9 = lVar10 * 8;
            plVar6 = (longlong *)thunk_FUN_140983da8(lVar9);
          }
          else {
            lVar10 = lVar9 * 2;
            if (lVar10 != 0) goto LAB_1407405a4;
            plVar6 = (longlong *)0x0;
            lVar9 = lVar9 << 4;
          }
          pvVar14 = *(void **)(param_1 + 0x2f8);
          plVar7 = plVar6;
          if (pvVar14 != *(void **)(param_1 + 0x300)) {
            _Size = (longlong)*(void **)(param_1 + 0x300) - (longlong)pvVar14;
            pvVar14 = memmove(plVar6,pvVar14,_Size);
            plVar7 = (longlong *)((longlong)pvVar14 + ((longlong)_Size >> 3) * 8);
          }
          *plVar7 = lVar15;
          if (*(void **)(param_1 + 0x2f8) != (void *)0x0) {
            free(*(void **)(param_1 + 0x2f8));
          }
          *(longlong **)(param_1 + 0x2f8) = plVar6;
          *(longlong **)(param_1 + 0x300) = plVar7 + 1;
          *(longlong *)(param_1 + 0x308) = (longlong)plVar6 + lVar9;
        }
        pvVar14 = *(void **)(param_1 + 0x300);
        _Dst = *(void **)(param_1 + 0x2f8);
        lVar15 = (longlong)pvVar14 - (longlong)_Dst;
        while (0x14 < (ulonglong)(lVar15 >> 3)) {
          _Src = (void *)((longlong)_Dst + 8);
          if (_Src < pvVar14) {
            memmove(_Dst,_Src,(longlong)pvVar14 - (longlong)_Src);
          }
          pvVar14 = (void *)(*(longlong *)(param_1 + 0x300) + -8);
          *(void **)(param_1 + 0x300) = pvVar14;
          _Dst = *(void **)(param_1 + 0x2f8);
          lVar15 = (longlong)pvVar14 - (longlong)_Dst;
        }
        FUN_14074c5a0(param_1 + 0x4e8,&local_c8,pvVar14,plVar13,(longlong)(int)*plVar13);
        uVar11 = *(ulonglong *)(local_c8 + 0x10);
        if (*(ulonglong *)(local_c8 + 0x10) < (ulonglong)plVar13[1]) {
          uVar11 = plVar13[1];
        }
        *(ulonglong *)(local_c8 + 0x10) = uVar11;
      }
      plVar6 = plVar13 + 2;
      plVar13 = plVar13 + 0x103;
      uVar8 = local_a8;
      lVar15 = local_res18;
      uVar5 = local_res20;
    } while (plVar6 != plVar12);
  }
  fVar17 = 0.0;
  plVar13 = *(longlong **)(param_1 + 0x300);
  plVar12 = *(longlong **)(param_1 + 0x2f8);
  if (plVar13 != plVar12) {
    do {
      fVar17 = fVar17 + (float)*plVar12;
      plVar12 = plVar12 + 1;
    } while (plVar12 != plVar13);
    fVar17 = fVar17 / (float)(ulonglong)((longlong)plVar13 - *(longlong *)(param_1 + 0x2f8) >> 3);
  }
  if (DAT_140b5ebf0.QuadPart == -1) {
    QueryPerformanceFrequency(&local_res8);
    DAT_140b5ebf0 = local_res8;
  }
  fVar17 = fVar17 * DAT_140aac4dc;
  fVar16 = (float)DAT_140b5ebf0.QuadPart;
  uVar1 = *(undefined8 *)(param_1 + 0x4e0);
  local_res8.QuadPart = (LONGLONG)&local_c8;
  local_a0 = 0;
  local_98 = 0;
  local_90 = 0;
  local_c8 = lVar15;
  local_c0 = uVar5;
  local_b8 = uVar8;
  uVar8 = FUN_140749890(local_70,&local_88);
  uVar5 = local_res10;
  FUN_14073e8b0(local_res10,param_1,uVar8,&local_c8,param_1 + 0x3a8,uVar1,(int)(fVar17 / fVar16));
  plVar12 = local_80;
  plVar13 = local_88;
  if (local_88 != local_80) {
    plVar6 = local_88 + 7;
    do {
      FUN_140320ec0(plVar6 + 0x38);
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
      switch((char)plVar6[0x37]) {
      case '\x01':
        FUN_1402f3da0(plVar6 + -7);
        break;
      case '\x05':
        if ((char)plVar6[4] != '\0') {
          _Memory = (undefined8 *)*plVar6;
          while (_Memory != (undefined8 *)0x0) {
            FUN_1402450d0(plVar6 + -2,*_Memory);
            puVar2 = (undefined8 *)_Memory[1];
            free(_Memory);
            _Memory = puVar2;
          }
        }
      }
      plVar7 = plVar6 + 0xfc;
      plVar6 = plVar6 + 0x103;
    } while (plVar7 != plVar12);
  }
  if (plVar13 != (longlong *)0x0) {
    free(plVar13);
  }
  return uVar5;
}


// Incoming references
// 0xc37208 DATA caller none
// 0x727d7f UNCONDITIONAL_CALL caller 1407272d0
// 0x7283d8 UNCONDITIONAL_CALL caller 1407272d0
