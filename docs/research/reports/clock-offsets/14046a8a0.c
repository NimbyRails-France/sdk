
void FUN_14046a8a0(longlong param_1,undefined8 *param_2)

{
  int iVar1;
  void *_Src;
  float fVar2;
  float fVar3;
  float fVar4;
  int iVar5;
  uint *puVar6;
  longlong lVar7;
  longlong lVar8;
  longlong lVar9;
  longlong lVar10;
  undefined8 *puVar11;
  void *pvVar12;
  undefined8 *puVar13;
  ulonglong uVar14;
  undefined4 *puVar15;
  uint uVar16;
  ulonglong uVar17;
  size_t _Size;
  longlong *plVar18;
  ulonglong *puVar19;
  ulonglong *puVar20;
  ulonglong uVar21;
  ulonglong *puVar22;
  float fVar23;
  ulonglong *local_res8;
  longlong *local_res18;
  undefined8 local_res20;
  uint *local_158;
  ulonglong *puStack_150;
  longlong local_148;
  longlong lStack_140;
  undefined1 local_138;
  undefined7 uStack_137;
  longlong *local_130;
  longlong local_128;
  ulonglong local_120;
  longlong *local_118;
  longlong *local_110;
  longlong local_100;
  undefined8 local_f8;
  undefined8 uStack_f0;
  undefined **local_e8;
  longlong local_e0;
  uint **local_d8;
  longlong *local_d0;
  undefined8 *local_c8;
  undefined ***local_b0;
  undefined8 local_a8;
  undefined8 uStack_a0;
  undefined8 local_98;

  FUN_140459ea0(*(longlong *)(param_1 + 0x58) + 0x98,&local_130);
  fVar4 = DAT_140aac408;
  fVar3 = DAT_140aac0a0;
  fVar2 = DAT_140aab6f0;
  local_res18 = local_118;
  uVar21 = local_120;
LAB_14046a945:
  if ((local_130 == (longlong *)0x0) ||
     (((*local_130 == local_130[1] || (*(longlong *)(*local_130 + uVar21 * 8) == 0)) &&
      ((local_118 == (longlong *)0x0 || (local_118 == local_110)))))) {
    plVar18 = (longlong *)0x0;
  }
  else {
    plVar18 = local_118;
    if ((*local_130 != local_130[1]) && (*(longlong *)(*local_130 + uVar21 * 8) != 0)) {
      plVar18 = (longlong *)(local_130[3] + uVar21 * 8);
    }
    lVar7 = *local_130;
    if (local_130[1] - lVar7 >> 3 != 0) {
      while (*(longlong *)(lVar7 + uVar21 * 8) != 0) {
        uVar21 = (local_130[1] - *local_130 >> 3) - 1U & uVar21 + 1;
        lVar7 = *(longlong *)(*local_130 + uVar21 * 8);
        if (((lVar7 == local_128) || (lVar7 == 0)) || (lVar7 = *local_130, local_130[1] == lVar7))
        break;
      }
    }
    if (((*local_130 == local_130[1]) || (*(longlong *)(*local_130 + uVar21 * 8) == 0)) &&
       (local_118 < local_110)) {
      local_118 = local_118 + 1;
      local_res18 = local_118;
    }
  }
  if (plVar18 != (longlong *)0x0) {
    if (*plVar18 == 0) {
      puVar6 = (uint *)0x0;
    }
    else {
      puVar6 = (uint *)FUN_14033f780(**(longlong **)(param_1 + 0x50) + 0x200);
      local_118 = local_res18;
    }
    if (*(longlong *)puVar6 != 0) {
      lVar7 = FUN_14045f300(*(undefined8 *)(param_1 + 0x60));
      lVar8 = FUN_14045f1c0(*(longlong *)(param_1 + 0x58) + 0x30);
      local_118 = local_res18;
      if (((lVar7 != 0) && (lVar8 != 0)) &&
         ((0 < *(int *)(lVar7 + 0x48) &&
          (((*(char *)(lVar7 + 0x4d0) != '\0' && (*(char *)(lVar7 + 0x510) != '\0')) &&
           (*(char *)(lVar7 + 0x558) != '\0')))))) {
        puVar19 = (ulonglong *)0x0;
        if (*(char *)(lVar7 + 0x4f0) != '\0') {
          if ((*(longlong *)(lVar7 + 0x4d8) == 0) ||
             (lVar9 = FUN_14033f710(**(longlong **)(param_1 + 0x50) + 0x280), lVar9 == 0))
          goto LAB_14046a945;
          for (puVar22 = *(ulonglong **)
                          (*(longlong *)(lVar9 + 0x1b8) +
                          (*(ulonglong *)(lVar7 + 0x4e0) % (ulonglong)*(uint *)(lVar9 + 0x1c0)) * 8)
              ; puVar22 != (ulonglong *)0x0; puVar22 = (ulonglong *)puVar22[0x3b]) {
            if (*(ulonglong *)(lVar7 + 0x4e0) == *puVar22) {
              if ((puVar22 != (ulonglong *)0x0) &&
                 (puVar19 = (ulonglong *)0x0,
                 puVar22 !=
                 *(ulonglong **)(*(longlong *)(lVar9 + 0x1b8) + *(longlong *)(lVar9 + 0x1c0) * 8)))
              {
                puVar19 = puVar22 + 1;
              }
              break;
            }
          }
          if (puVar19 == (ulonglong *)0x0) goto LAB_14046a945;
        }
        lVar9 = *(longlong *)(lVar7 + 0x4c0) + *(longlong *)(param_1 + 0x38) * -10000;
        if ((0 < lVar9) && (*(char *)(lVar7 + 0x54c) == '\0')) {
          if (*(longlong *)puVar6 == 0) {
            local_138 = 0;
            lStack_140 = 0;
            local_158 = puVar6;
            puStack_150 = puVar19;
            local_148 = lVar7;
          }
          else {
            lStack_140 = FUN_1404594d0(*(longlong *)(param_1 + 0x60) + 0x30);
            local_138 = 0;
            local_158 = puVar6;
            puStack_150 = puVar19;
            local_148 = lVar7;
            if (lStack_140 != 0) {
              uVar16 = (*(uint *)(param_1 + 0xa0) ^ *puVar6) % 0x7fffffff;
              if (uVar16 == 0) {
                uVar16 = 1;
              }
              if (0x708 < *(longlong *)(param_1 + 0x100) - *(longlong *)(lVar7 + 0x630)) {
                uVar17 = ((ulonglong)uVar16 * 0xbc8f >> 0x1f) +
                         (ulonglong)((uint)((ulonglong)uVar16 * 0xbc8f) & 0x7fffffff);
                uVar14 = (ulonglong)((int)uVar17 + 0x80000001);
                if (uVar17 < 0x7fffffff) {
                  uVar14 = uVar17;
                }
                *(ulonglong *)(lVar7 + 0x630) =
                     (ulonglong)(uint)((int)uVar14 + (int)(uVar14 / 600) * -600) +
                     *(longlong *)(param_1 + 0x100);
                if (*(char *)(lVar7 + 0x5d0) == '\0') {
                  local_100 = 0;
                }
                else {
                  local_100 = *(longlong *)(lVar7 + 0x5a8);
                }
                local_res20 = 0;
                if ((local_100 != 0) &&
                   (lVar10 = FUN_14033f7f0(**(longlong **)(param_1 + 0x48) + 0x180), lVar10 != 0)) {
                  local_res20 = *(undefined8 *)(lVar10 + 0x10);
                }
                local_138 = 1;
                local_a8 = 0;
                uStack_a0 = 0;
                local_98 = 0;
                local_e8 = std::
                           _Func_impl_no_alloc<`public:_void___cdecl_nimby::model::PaxUpdater::lost_check(std::linear_congruential_engine<unsigned_int,48271,0,2147483647>&___ptr64,nimby::model::PaxUpdater::Station&___ptr64,nimby::model::PaxUpdater::Train&___ptr64)___ptr64'::`2'::<lambda_1>,bool,nimby::model::Pax_const&___ptr64>
                           ::vftable;
                local_d8 = &local_158;
                local_d0 = &local_100;
                local_c8 = &local_res20;
                local_b0 = &local_e8;
                local_e0 = param_1;
                FUN_1404649b0(lStack_140 + 8,&local_e8);
              }
              iVar1 = *(int *)(lVar7 + 0x534);
              if ((iVar1 < 1) &&
                 ((*(int *)(lVar7 + 0x48) <= *(int *)(lVar8 + 0x38) ||
                  (*(char *)(lVar7 + 0x540) != '\0')))) {
LAB_14046add2:
                lVar7 = 0xe8;
              }
              else {
                if ((lVar9 < 5000000) &&
                   (*(float *)(lVar7 + 0x548) <= fVar4 && fVar4 != *(float *)(lVar7 + 0x548))) {
                  fVar23 = *(float *)(param_1 + 0x98) + fVar2;
                  if (fVar23 <= fVar3) {
                    fVar23 = fVar3;
                  }
                  iVar5 = lroundf(fVar23);
                  *(int *)(lVar7 + 0x4c8) = *(int *)(lVar7 + 0x4c8) + iVar5;
                  *(longlong *)(lVar7 + 0x4b8) = *(longlong *)(lVar7 + 0x4b8) + (longlong)iVar5;
                  *(longlong *)(lVar7 + 0x4c0) =
                       *(longlong *)(lVar7 + 0x4c0) + (longlong)iVar5 * 1000000;
                  *(float *)(lVar7 + 0x548) = fVar23 + *(float *)(lVar7 + 0x548);
                }
                lVar7 = 0x20;
                if (iVar1 < 1) goto LAB_14046add2;
              }
              puVar11 = *(undefined8 **)(lVar7 + 8 + (longlong)param_2);
              if (puVar11 < *(undefined8 **)(lVar7 + 0x10 + (longlong)param_2)) {
                *puVar11 = local_158;
                puVar11[1] = puStack_150;
                puVar11[2] = local_148;
                puVar11[3] = lStack_140;
                puVar11[4] = CONCAT71(uStack_137,local_138);
                plVar18 = (longlong *)(lVar7 + 8 + (longlong)param_2);
                *plVar18 = *plVar18 + 0x28;
              }
              else {
                lVar8 = (longlong)puVar11 - *(longlong *)(lVar7 + (longlong)param_2) >> 3;
                if (lVar8 * -0x3333333333333333 == 0) {
                  lVar8 = 1;
LAB_14046ae50:
                  puVar11 = (undefined8 *)thunk_FUN_140983da8(lVar8 * 0x28);
                }
                else {
                  lVar8 = lVar8 * -0x6666666666666666;
                  if (lVar8 != 0) goto LAB_14046ae50;
                  puVar11 = (undefined8 *)0x0;
                }
                pvVar12 = *(void **)(lVar7 + 8 + (longlong)param_2);
                _Src = *(void **)(lVar7 + (longlong)param_2);
                puVar13 = puVar11;
                if (_Src != pvVar12) {
                  _Size = (longlong)pvVar12 - (longlong)_Src;
                  pvVar12 = memmove(puVar11,_Src,_Size);
                  puVar13 = (undefined8 *)((longlong)pvVar12 + ((longlong)_Size >> 3) * 8);
                }
                *puVar13 = local_158;
                puVar13[1] = puStack_150;
                puVar13[2] = local_148;
                puVar13[3] = lStack_140;
                puVar13[4] = CONCAT71(uStack_137,local_138);
                pvVar12 = *(void **)(lVar7 + (longlong)param_2);
                if ((pvVar12 != (void *)0x0) &&
                   (pvVar12 != *(void **)(lVar7 + 0x20 + (longlong)param_2))) {
                  free(pvVar12);
                }
                *(undefined8 **)(lVar7 + (longlong)param_2) = puVar11;
                *(undefined8 **)(lVar7 + 8 + (longlong)param_2) = puVar13 + 5;
                *(undefined8 **)(lVar7 + 0x10 + (longlong)param_2) = puVar11 + lVar8 * 5;
              }
            }
          }
        }
      }
    }
    goto LAB_14046a945;
  }
  lVar7 = *(longlong *)(param_1 + 0x58);
  plVar18 = (longlong *)(lVar7 + 0x160);
  uVar21 = *(ulonglong *)*param_2;
  local_res8 = (ulonglong *)0x0;
  uVar14 = (uVar21 >> 0x21 ^ uVar21) * -0xae502812aa7333;
  uVar14 = (uVar14 >> 0x21 ^ uVar14) * -0x3b314601e57a13ad;
  lVar8 = *(longlong *)(lVar7 + 0x168);
  lVar9 = *plVar18;
  lVar10 = lVar8 - lVar9 >> 3;
  uVar14 = (uVar14 >> 0x21 ^ uVar14) & lVar10 - 1U;
  if (lVar10 != 0) {
    do {
      uVar14 = uVar14 & (lVar8 - lVar9 >> 3) - 1U;
      uVar17 = *(ulonglong *)(lVar9 + uVar14 * 8);
      if ((uVar17 == uVar21) || (uVar17 == 0)) break;
      uVar14 = uVar14 + 1;
      lVar8 = *(longlong *)(lVar7 + 0x168);
      lVar9 = *plVar18;
    } while (lVar8 != lVar9);
  }
  puVar19 = *(ulonglong **)
             (*(longlong *)(lVar7 + 0x198) + (uVar21 % (ulonglong)*(uint *)(lVar7 + 0x1a0)) * 8);
  do {
    puVar20 = (ulonglong *)0x0;
    puVar22 = (ulonglong *)0x0;
    if (puVar19 == (ulonglong *)0x0) {
LAB_14046affc:
      do {
        lVar8 = uVar14 * 8;
        if (((*plVar18 == *(longlong *)(lVar7 + 0x168)) || (*(longlong *)(*plVar18 + lVar8) == 0))
           && ((puVar22 == (ulonglong *)0x0 || (puVar22 == puVar20)))) {
          puVar19 = (ulonglong *)0x0;
        }
        else {
          puVar19 = puVar22;
          if ((*plVar18 != *(longlong *)(lVar7 + 0x168)) && (*(longlong *)(lVar8 + *plVar18) != 0))
          {
            puVar19 = (ulonglong *)(uVar14 * 0x10 + *(longlong *)(lVar7 + 0x178));
          }
          lVar9 = *plVar18;
          if (*(longlong *)(lVar7 + 0x168) - lVar9 >> 3 != 0) {
            while (*(longlong *)(lVar8 + lVar9) != 0) {
              uVar14 = (*(longlong *)(lVar7 + 0x168) - *plVar18 >> 3) - 1U & uVar14 + 1;
              lVar8 = uVar14 * 8;
              uVar17 = *(ulonglong *)(lVar8 + *plVar18);
              if (((uVar17 == uVar21) || (uVar17 == 0)) ||
                 (lVar9 = *plVar18, *(longlong *)(lVar7 + 0x168) == lVar9)) break;
            }
          }
          if (((*plVar18 == *(longlong *)(lVar7 + 0x168)) ||
              (*(longlong *)(*plVar18 + uVar14 * 8) == 0)) && (puVar22 < puVar20)) {
            puVar22 = puVar22 + 2;
          }
        }
        if (puVar19 == (ulonglong *)0x0) {
          return;
        }
        local_f8 = *puVar19;
        uStack_f0 = puVar19[1];
        for (puVar19 = *(ulonglong **)
                        (param_2[0x37] + (local_f8 % (ulonglong)*(uint *)(param_2 + 0x38)) * 8);
            puVar19 != (ulonglong *)0x0; puVar19 = (ulonglong *)puVar19[2]) {
          if (local_f8 == *puVar19) goto LAB_14046affc;
        }
        puVar15 = (undefined4 *)thunk_FUN_140983da8(0x18);
        *puVar15 = (undefined4)local_f8;
        puVar15[1] = local_f8._4_4_;
        puVar15[2] = (undefined4)uStack_f0;
        puVar15[3] = uStack_f0._4_4_;
        *(undefined8 *)(puVar15 + 4) = 0;
        FUN_14032f5a0(param_2 + 0x36);
        puVar20 = local_res8;
      } while( true );
    }
    if (uVar21 == *puVar19) {
      if ((puVar19 != (ulonglong *)0x0) &&
         (puVar20 = (ulonglong *)0x0, puVar22 = (ulonglong *)0x0,
         puVar19 != *(ulonglong **)(*(longlong *)(lVar7 + 0x198) + *(longlong *)(lVar7 + 0x1a0) * 8)
         )) {
        puVar22 = (ulonglong *)puVar19[1];
        local_res8 = (ulonglong *)puVar19[2];
        puVar20 = local_res8;
      }
      goto LAB_14046affc;
    }
    puVar19 = (ulonglong *)puVar19[4];
  } while( true );
}
