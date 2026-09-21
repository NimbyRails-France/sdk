// Candidate VA 14047eae0; RVA 0x47eae0
// Ghidra inferred prototype: undefined FUN_14047eae0()

void FUN_14047eae0(longlong param_1,longlong param_2)

{
  longlong lVar1;
  double dVar2;
  ulonglong *puVar3;
  ulonglong uVar4;
  int iVar5;
  int iVar6;
  longlong *plVar7;
  longlong lVar8;
  ulonglong uVar9;
  ulonglong *puVar10;
  ulonglong *puVar11;
  ulonglong uVar12;
  ulonglong *puVar13;
  ulonglong uVar14;
  longlong *plVar15;
  longlong *plVar16;
  ulonglong uVar17;
  ulonglong uVar18;
  longlong *local_res18;
  int iStackX_24;
  ulonglong local_108 [2];
  undefined4 local_f8;
  undefined4 uStack_f4;
  undefined4 local_f0;
  undefined4 local_ec;
  ulonglong local_e8 [2];
  undefined8 local_d8;
  undefined4 local_d0;
  undefined4 local_cc;
  undefined4 local_c8;
  undefined4 local_c4;
  undefined4 local_c0;
  undefined4 local_bc;
  undefined8 local_b8;
  longlong *local_b0;
  longlong local_a8;
  ulonglong *local_a0 [2];
  ulonglong *local_90 [2];
  undefined1 local_80 [64];

  FUN_14047d6d0(param_1 + 0x23e0);
  local_f8 = *(undefined4 *)(param_1 + 0x2380);
  uStack_f4 = *(undefined4 *)(param_1 + 0x2384);
  local_f0 = *(undefined4 *)(param_1 + 0x23a8);
  local_ec = *(undefined4 *)(param_1 + 0x23ac);
  local_d8._0_4_ = *(undefined4 *)(param_1 + 0x2388);
  local_d8._4_4_ = *(undefined4 *)(param_1 + 0x238c);
  local_d0 = *(undefined4 *)(param_1 + 0x2390);
  local_cc = *(undefined4 *)(param_1 + 0x2394);
  local_c8 = *(undefined4 *)(param_1 + 0x2398);
  local_c4 = *(undefined4 *)(param_1 + 0x239c);
  local_c0 = *(undefined4 *)(param_1 + 0x23a0);
  local_bc = *(undefined4 *)(param_1 + 0x23a4);
  plVar7 = *(longlong **)(param_1 + 0x23f0);
  plVar16 = (longlong *)*plVar7;
  local_res18 = plVar7;
  if (plVar16 == (longlong *)0x0) {
    local_res18 = plVar7 + 1;
    plVar16 = (longlong *)*local_res18;
    while (plVar16 == (longlong *)0x0) {
      local_res18 = local_res18 + 1;
      plVar16 = (longlong *)*local_res18;
    }
  }
  local_b0 = (longlong *)plVar7[*(longlong *)(param_1 + 0x23f8)];
  if (plVar16 != local_b0) {
    local_a8 = param_1 + 0x23b0;
    do {
      plVar7 = (longlong *)FUN_1404834e0(local_a8,local_80);
      lVar1 = *plVar7;
      plVar7 = (longlong *)(lVar1 + 8);
      puVar11 = (ulonglong *)&local_f8;
      do {
        uVar18 = *puVar11;
        local_b8 = uVar18;
        uVar18 = local_b8;
        uVar17 = 0;
        local_b8._4_4_ = (int)(uVar18 >> 0x20);
        iVar6 = local_b8._4_4_;
        local_b8 = uVar18;
        do {
          dVar2 = (double)plVar16[(longlong)(int)uVar17 + 1];
          if (dVar2 != 0.0) {
            iVar5 = iVar6;
            if ((longlong)uVar18 < 0) {
              iVar5 = 0;
            }
            uVar14 = ((longlong)iVar5 | (longlong)(int)uVar18 << 0x20) << 0x10 |
                     (longlong)(int)uVar17 & 0xffffU;
            puVar10 = (ulonglong *)*plVar7;
            puVar3 = *(ulonglong **)(lVar1 + 0x10);
            uVar12 = (longlong)puVar3 - (longlong)puVar10 >> 4;
            puVar13 = puVar10;
            uVar9 = uVar12;
            while (uVar4 = uVar9, 0 < (longlong)uVar4) {
              uVar9 = uVar4 >> 1;
              if (puVar13[uVar9 * 2] < uVar14) {
                puVar13 = puVar13 + uVar9 * 2 + 2;
                uVar9 = uVar4 + (-1 - uVar9);
              }
            }
            if (((puVar13 == puVar3) || (uVar14 < *puVar13)) || (puVar13 == puVar13 + 2)) {
              while (uVar9 = uVar12, 0 < (longlong)uVar9) {
                uVar12 = uVar9 >> 1;
                if (puVar10[uVar12 * 2] < uVar14) {
                  puVar10 = puVar10 + uVar12 * 2 + 2;
                  uVar12 = uVar9 + (-1 - uVar12);
                }
              }
              if ((puVar10 == puVar3) || (uVar14 < *puVar10)) {
                local_108[1] = 0;
                puVar3 = (ulonglong *)*plVar7;
                local_108[0] = uVar14;
                if ((puVar10 == puVar3) || (puVar10[-2] < uVar14)) {
                  puVar13 = *(ulonglong **)(lVar1 + 0x10);
                  if ((puVar13 == *(ulonglong **)(lVar1 + 0x18)) || (puVar10 != puVar13)) {
                    FUN_140341ac0(plVar7,puVar10,local_108);
                  }
                  else {
                    *puVar13 = uVar14;
                    puVar13[1] = 0;
                    *(longlong *)(lVar1 + 0x10) = *(longlong *)(lVar1 + 0x10) + 0x10;
                  }
                  puVar10 = (ulonglong *)
                            (((longlong)puVar10 - (longlong)puVar3 >> 4) * 0x10 + *plVar7);
                }
                else {
                  FUN_140483e10(plVar7,local_a0,local_108);
                  puVar10 = local_a0[0];
                }
              }
              puVar10[1] = (ulonglong)dVar2;
            }
            else {
              puVar13[1] = (ulonglong)(dVar2 + (double)puVar13[1]);
            }
          }
          uVar17 = uVar17 + 1;
        } while (uVar17 < 0x2c);
        puVar11 = puVar11 + 1;
      } while (puVar11 != local_e8);
      if ((*plVar16 == 0) ||
         (lVar8 = FUN_14033f7f0(*(longlong *)(param_2 + 0x380) + 0x180), lVar8 != 0)) {
        plVar15 = &local_d8;
        do {
          uVar18 = 0;
          lVar8 = *plVar15;
          iStackX_24 = (int)((ulonglong)lVar8 >> 0x20);
          do {
            dVar2 = (double)plVar16[(longlong)(int)uVar18 + 1];
            if (dVar2 != 0.0) {
              iVar6 = iStackX_24;
              if (lVar8 < 0) {
                iVar6 = 0;
              }
              uVar9 = ((longlong)iVar6 | (longlong)(int)lVar8 << 0x20) << 0x10 |
                      (longlong)(int)uVar18 & 0xffffU;
              puVar11 = (ulonglong *)*plVar7;
              uVar17 = *(longlong *)(lVar1 + 0x10) - (longlong)puVar11 >> 4;
              while (uVar12 = uVar17, 0 < (longlong)uVar12) {
                uVar17 = uVar12 >> 1;
                if (puVar11[uVar17 * 2] < uVar9) {
                  puVar11 = puVar11 + uVar17 * 2 + 2;
                  uVar17 = uVar12 + (-1 - uVar17);
                }
              }
              if (((puVar11 == *(ulonglong **)(lVar1 + 0x10)) || (uVar9 < *puVar11)) ||
                 (puVar11 == puVar11 + 2)) {
                puVar11 = (ulonglong *)*plVar7;
                uVar17 = (longlong)*(ulonglong **)(lVar1 + 0x10) - (longlong)puVar11 >> 4;
                while (uVar12 = uVar17, 0 < (longlong)uVar12) {
                  uVar17 = uVar12 >> 1;
                  if (puVar11[uVar17 * 2] < uVar9) {
                    puVar11 = puVar11 + uVar17 * 2 + 2;
                    uVar17 = uVar12 + (-1 - uVar17);
                  }
                }
                puVar10 = *(ulonglong **)(lVar1 + 0x10);
                if ((puVar11 == puVar10) || (uVar9 < *puVar11)) {
                  local_e8[1] = 0;
                  local_e8[0] = uVar9;
                  puVar3 = (ulonglong *)*plVar7;
                  if ((puVar11 == puVar3) || (puVar11[-2] < uVar9)) {
                    if ((puVar10 == *(ulonglong **)(lVar1 + 0x18)) || (puVar11 != puVar10)) {
                      FUN_140341ac0(plVar7,puVar11,local_e8);
                    }
                    else {
                      *puVar10 = uVar9;
                      puVar10[1] = 0;
                      *(longlong *)(lVar1 + 0x10) = *(longlong *)(lVar1 + 0x10) + 0x10;
                    }
                    puVar11 = (ulonglong *)
                              (((longlong)puVar11 - (longlong)puVar3 >> 4) * 0x10 + *plVar7);
                  }
                  else {
                    FUN_140483e10(plVar7,local_90,local_e8);
                    puVar11 = local_90[0];
                  }
                }
                puVar11[1] = (ulonglong)dVar2;
              }
              else {
                puVar11[1] = (ulonglong)(dVar2 + (double)puVar11[1]);
              }
            }
            uVar18 = uVar18 + 1;
          } while (uVar18 < 0x2c);
          plVar15 = plVar15 + 1;
        } while (plVar15 != &local_b8);
      }
      plVar16 = (longlong *)plVar16[0x2d];
      while (plVar16 == (longlong *)0x0) {
        local_res18 = local_res18 + 1;
        plVar16 = (longlong *)*local_res18;
      }
    } while (plVar16 != local_b0);
  }
  FUN_140351f40(param_1 + 0x2418);
  FUN_140351f40(param_1 + 0x23e8);
  return;
}


// Incoming references
// 0xc2531c DATA caller none
// 0x477dd7 UNCONDITIONAL_CALL caller 140477ce0
// 0x478fb9 UNCONDITIONAL_CALL caller 140478f40
// 0x4f45ae UNCONDITIONAL_CALL caller 1404f3520
// 0x742332 UNCONDITIONAL_CALL caller 1407422c0
// 0x74e01f UNCONDITIONAL_CALL caller 14074e000
// 0x74e0dd UNCONDITIONAL_CALL caller 14074e0b0
// 0x74e1b0 UNCONDITIONAL_CALL caller 14074e180
