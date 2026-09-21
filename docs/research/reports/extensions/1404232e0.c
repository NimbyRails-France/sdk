// Candidate VA 1404232e0; RVA 0x4232e0
// Ghidra inferred prototype: undefined FUN_1404232e0()

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_1404232e0(longlong *param_1,undefined8 param_2,longlong param_3,longlong param_4,
                  longlong *param_5)

{
  longlong *plVar1;
  double ****ppppdVar2;
  int iVar3;
  ulonglong uVar4;
  ulonglong *puVar5;
  char cVar6;
  void *pvVar7;
  undefined8 *puVar8;
  undefined8 uVar9;
  undefined8 *puVar10;
  longlong lVar11;
  double ***pppdVar12;
  longlong lVar13;
  ulonglong uVar14;
  double ****ppppdVar15;
  undefined1 uVar16;
  double *pdVar17;
  ulonglong uVar18;
  longlong lVar19;
  longlong lVar20;
  ulonglong *puVar21;
  double ****ppppdVar22;
  longlong lVar23;
  double ***local_e8;
  double ***local_e0;
  longlong *local_d8;
  ulonglong *local_d0;
  ulonglong *local_c8;
  ulonglong local_c0;
  ulonglong local_b0;
  ulonglong *local_a8;
  ulonglong *puStack_a0;
  undefined8 local_98;
  undefined8 local_90;
  double **local_88;
  undefined8 uStack_80;
  undefined8 local_78;
  double **local_70;
  undefined8 uStack_68;
  undefined8 local_60;

  *(undefined4 *)(param_3 + 0x48) = *(undefined4 *)(param_4 + 0x90);
  plVar1 = (longlong *)(param_3 + 0x10);
  lVar11 = *plVar1;
  lVar20 = *(longlong *)(param_3 + 0x18) - lVar11 >> 4;
  lVar13 = (*(longlong *)(param_4 + 0x78) - *(longlong *)(param_4 + 0x70) >> 5) *
           -0x5555555555555555;
  if ((((lVar20 != lVar13) ||
       (*(longlong *)(param_3 + 0x30) - *(longlong *)(param_3 + 0x28) >> 4 != lVar13)) ||
      (*(longlong *)(param_3 + 8) != *(longlong *)(param_4 + 0x88))) &&
     (param_5[1] - *param_5 >> 5 == lVar13)) {
    local_a8 = (ulonglong *)0x0;
    puStack_a0 = (ulonglong *)0x0;
    local_98 = 0;
    uVar18 = 0;
    local_d8 = plVar1;
    if (lVar20 != 0) {
      lVar13 = 0;
      do {
        if ((ulonglong)(*(longlong *)(param_3 + 0x30) - *(longlong *)(param_3 + 0x28) >> 4) <=
            uVar18) break;
        local_d0 = (ulonglong *)FUN_1404394f0(&local_a8,*(longlong *)(param_3 + 0x28) + lVar13);
        FUN_14043bec0((longlong)*(char *)(lVar11 + lVar13 + 8) + 1,&local_d0,lVar11 + lVar13);
        uVar18 = uVar18 + 1;
        lVar13 = lVar13 + 0x10;
        lVar11 = *plVar1;
      } while (uVar18 < (ulonglong)(*(longlong *)(param_3 + 0x18) - lVar11 >> 4));
    }
    FUN_1403455e0();
    *(undefined8 *)(param_3 + 0x30) = *(undefined8 *)(param_3 + 0x28);
    local_88 = (double **)_DAT_140aac8b0;
    uStack_80 = _UNK_140aac8b8;
    local_78 = 0x7fffffffffffffff;
    local_70 = (double **)_DAT_140aac8c0;
    uStack_68 = _UNK_140aac8c8;
    local_60 = DAT_140aac590;
    local_c0 = 0;
    lVar11 = *(longlong *)(param_4 + 0x70);
    local_d0 = puStack_a0;
    local_c8 = local_a8;
    if ((*(longlong *)(param_4 + 0x78) - lVar11 >> 5) * -0x5555555555555555 != 0) {
      local_b0 = ((longlong)puStack_a0 - (longlong)local_a8 >> 3) * -0x5555555555555555;
      do {
        pdVar17 = (double *)0x0;
        lVar13 = local_c0 * 0x60;
        ppppdVar22 = (double ****)(local_c0 * 0x20 + *param_5);
        puVar21 = local_c8;
        if (0 < (longlong)local_b0) {
          uVar18 = local_b0;
          do {
            uVar14 = uVar18 >> 1;
            if (puVar21[uVar14 * 3] < *(ulonglong *)(lVar13 + 0x20 + lVar11)) {
              puVar21 = puVar21 + uVar14 * 3 + 3;
              uVar14 = uVar18 + (-1 - uVar14);
            }
            uVar18 = uVar14;
          } while (0 < (longlong)uVar14);
        }
        if (((puVar21 == local_d0) || (*(ulonglong *)(lVar13 + 0x20 + lVar11) < *puVar21)) ||
           (puVar21 == puVar21 + 3)) {
          puVar21 = local_d0;
        }
        if (puVar21 != local_d0) {
          pdVar17 = (double *)(puVar21 + 1);
        }
        puVar10 = *(undefined8 **)(param_3 + 0x30);
        local_e8 = (double ***)ppppdVar22;
        if (puVar10 < *(undefined8 **)(param_3 + 0x38)) {
          *puVar10 = 0;
          puVar10[1] = 0;
          *(longlong *)(param_3 + 0x30) = *(longlong *)(param_3 + 0x30) + 0x10;
          puVar8 = *(undefined8 **)(param_3 + 0x30);
        }
        else {
          lVar20 = (longlong)puVar10 - *(longlong *)(param_3 + 0x28) >> 4;
          if (lVar20 == 0) {
            lVar19 = 1;
LAB_140423560:
            lVar20 = lVar19 << 4;
            puVar10 = (undefined8 *)thunk_FUN_140983da8(lVar20);
          }
          else {
            lVar19 = lVar20 * 2;
            if (lVar19 != 0) goto LAB_140423560;
            puVar10 = (undefined8 *)0x0;
            lVar20 = lVar20 << 5;
          }
          pvVar7 = *(void **)(param_3 + 0x28);
          puVar8 = puVar10;
          if (pvVar7 != *(void **)(param_3 + 0x30)) {
            uVar18 = (longlong)*(void **)(param_3 + 0x30) - (longlong)pvVar7;
            pvVar7 = memmove(puVar10,pvVar7,uVar18);
            puVar8 = (undefined8 *)((longlong)pvVar7 + (uVar18 & 0xfffffffffffffff0));
          }
          *puVar8 = 0;
          puVar8[1] = 0;
          puVar8 = puVar8 + 2;
          if (*(void **)(param_3 + 0x28) != (void *)0x0) {
            free(*(void **)(param_3 + 0x28));
          }
          *(undefined8 **)(param_3 + 0x28) = puVar10;
          *(undefined8 **)(param_3 + 0x30) = puVar8;
          *(longlong *)(param_3 + 0x38) = lVar20 + (longlong)puVar10;
        }
        puVar8[-2] = *(undefined8 *)(lVar13 + 0x20 + lVar11);
        puVar10 = *(undefined8 **)(lVar13 + 0x48 + lVar11);
        if (*(undefined8 **)(lVar13 + 0x50 + lVar11) == puVar10) {
          uVar9 = 0;
        }
        else {
          uVar9 = *puVar10;
        }
        puVar8[-1] = uVar9;
        puVar10 = (undefined8 *)local_d8[1];
        if (puVar10 < (undefined8 *)local_d8[2]) {
          *puVar10 = 0;
          *(undefined1 *)(puVar10 + 1) = 0;
          local_d8[1] = local_d8[1] + 0x10;
          puVar10 = (undefined8 *)local_d8[1];
        }
        else {
          lVar20 = (longlong)puVar10 - *local_d8 >> 4;
          if (lVar20 == 0) {
            lVar20 = 1;
LAB_14042362f:
            local_e0 = (double ***)thunk_FUN_140983da8(lVar20 << 4);
          }
          else {
            lVar20 = lVar20 * 2;
            if (lVar20 != 0) goto LAB_14042362f;
            local_e0 = (double ***)0x0;
          }
          puVar10 = (undefined8 *)FUN_14033ae70(*local_d8,local_d8[1],local_e0);
          *puVar10 = 0;
          *(undefined1 *)(puVar10 + 1) = 0;
          puVar10 = puVar10 + 2;
          lVar19 = local_d8[1];
          for (lVar23 = *local_d8; lVar23 != lVar19; lVar23 = lVar23 + 0x10) {
            FUN_1403385b0(lVar23);
            ppppdVar22 = (double ****)local_e8;
          }
          if ((void *)*local_d8 != (void *)0x0) {
            free((void *)*local_d8);
          }
          *local_d8 = (longlong)local_e0;
          local_d8[1] = (longlong)puVar10;
          local_d8[2] = (longlong)(local_e0 + lVar20 * 2);
        }
        ppppdVar2 = (double ****)(puVar10 + -2);
        iVar3 = *(int *)(lVar13 + 0x28 + lVar11);
        if (iVar3 == 9) {
          ppppdVar15 = (double ****)&local_88;
          if ((ppppdVar22 != (double ****)0x0) && (*(char *)(ppppdVar22 + 3) == '\0')) {
            ppppdVar15 = ppppdVar22;
          }
          local_e0 = ppppdVar15[1];
          if ((pdVar17 != (double *)0x0) && (*(char *)(pdVar17 + 1) == '\0')) {
            local_e0 = (double ***)*pdVar17;
          }
          ppppdVar22 = &local_e0;
          if ((longlong)local_e0 < (longlong)*ppppdVar15) {
            ppppdVar22 = ppppdVar15;
          }
          if ((longlong)ppppdVar15[2] < (longlong)local_e0) {
            ppppdVar22 = ppppdVar15 + 2;
          }
          pppdVar12 = *ppppdVar22;
          if (*(char *)(puVar10 + -1) != '\0') {
            FUN_1403385b0(ppppdVar2);
            *(undefined1 *)(puVar10 + -1) = 0;
          }
          puVar10[-2] = pppdVar12;
          goto LAB_140423f0b;
        }
        if (iVar3 == 0xc) {
          ppppdVar15 = (double ****)&local_70;
          if ((ppppdVar22 != (double ****)0x0) && (*(char *)(ppppdVar22 + 3) == '\x01')) {
            ppppdVar15 = ppppdVar22;
          }
          local_e0 = ppppdVar15[1];
          if ((pdVar17 != (double *)0x0) && (*(char *)(pdVar17 + 1) == '\x01')) {
            local_e0 = (double ***)*pdVar17;
          }
          ppppdVar22 = &local_e0;
          if ((double)local_e0 < (double)*ppppdVar15) {
            ppppdVar22 = ppppdVar15;
          }
          if ((double)ppppdVar15[2] < (double)local_e0) {
            ppppdVar22 = ppppdVar15 + 2;
          }
          pppdVar12 = *ppppdVar22;
          if (*(char *)(puVar10 + -1) == '\x01') {
            *ppppdVar2 = pppdVar12;
          }
          else {
            FUN_1403385b0(ppppdVar2);
            *ppppdVar2 = pppdVar12;
            *(undefined1 *)(puVar10 + -1) = 1;
          }
          goto LAB_140423f0b;
        }
        if (iVar3 == 2) {
          uVar16 = 0;
          if ((ppppdVar22 != (double ****)0x0) && (*(char *)(ppppdVar22 + 3) == '\x02')) {
            uVar16 = *(undefined1 *)ppppdVar22;
          }
          if ((pdVar17 != (double *)0x0) && (*(char *)(pdVar17 + 1) == '\x02')) {
            uVar16 = *(undefined1 *)pdVar17;
          }
          if (*(char *)(puVar10 + -1) != '\x02') {
            FUN_1403385b0(ppppdVar2);
            *(undefined1 *)(puVar10 + -1) = 2;
          }
          *(undefined1 *)(puVar10 + -2) = uVar16;
          goto LAB_140423f0b;
        }
        if (iVar3 == 0xe) {
          pppdVar12 = (double ***)0x0;
          ppppdVar15 = (double ****)0x0;
          if ((ppppdVar22 != (double ****)0x0) && (*(char *)(ppppdVar22 + 3) == '\x03')) {
            ppppdVar15 = ppppdVar22;
          }
          if ((pdVar17 != (double *)0x0) && (*(char *)(pdVar17 + 1) == '\x03')) {
            pppdVar12 = (double ***)*pdVar17;
          }
          uVar9 = param_2;
          lVar20 = FUN_1402a7b10(param_2,*(undefined8 *)(lVar13 + 0x38 + lVar11));
          if (lVar20 != 0) {
            for (puVar8 = (undefined8 *)**(undefined8 **)(lVar20 + 0x48);
                puVar8 != *(undefined8 **)(lVar20 + 0x48); puVar8 = (undefined8 *)*puVar8) {
              if ((double ***)puVar8[0xb] == pppdVar12) goto LAB_14042398a;
            }
          }
          if (ppppdVar15 == (double ****)0x0) {
            lVar11 = FUN_1402a7b10(uVar9,*(undefined8 *)(lVar13 + 0x38 + lVar11));
            if ((lVar11 != 0) && (*(ulonglong **)(lVar11 + 0x80) != *(ulonglong **)(lVar11 + 0x88)))
            {
              uVar18 = **(ulonglong **)(lVar11 + 0x80);
              uVar14 = (((((((uVar18 >> 8 & 0xff ^
                             (uVar18 & 0xff ^ 0xcbf29ce484222325) * 0x100000001b3) * 0x100000001b3 ^
                            uVar18 >> 0x10 & 0xff) * 0x100000001b3 ^ uVar18 >> 0x18 & 0xff) *
                           0x100000001b3 ^ uVar18 >> 0x20 & 0xff) * 0x100000001b3 ^
                         uVar18 >> 0x28 & 0xff) * 0x100000001b3 ^ uVar18 >> 0x30 & 0xff) *
                        0x100000001b3 ^ uVar18 >> 0x38) * 0x100000001b3 &
                       *(ulonglong *)(lVar11 + 0x70);
              lVar13 = *(longlong *)(*(longlong *)(lVar11 + 0x58) + 8 + uVar14 * 0x10);
              if (lVar13 == *(longlong *)(lVar11 + 0x48)) {
LAB_140423979:
                lVar13 = 0;
              }
              else {
                uVar4 = *(ulonglong *)(lVar13 + 0x10);
                while (uVar18 != uVar4) {
                  if (lVar13 == *(longlong *)(*(longlong *)(lVar11 + 0x58) + uVar14 * 0x10))
                  goto LAB_140423979;
                  lVar13 = *(longlong *)(lVar13 + 8);
                  uVar4 = *(ulonglong *)(lVar13 + 0x10);
                }
              }
              if ((lVar13 != 0) && (lVar13 != *(longlong *)(lVar11 + 0x48))) {
                pppdVar12 = *(double ****)(lVar13 + 0x58);
              }
            }
          }
          else {
            pppdVar12 = *ppppdVar15;
          }
LAB_14042398a:
          if (*(char *)(puVar10 + -1) != '\x03') {
            FUN_1403385b0(ppppdVar2);
            *(undefined1 *)(puVar10 + -1) = 3;
          }
          puVar10[-2] = pppdVar12;
          goto LAB_140423f0b;
        }
        if (iVar3 == 0xd) {
          if (*(longlong *)(lVar13 + 0x38 + lVar11) == *(longlong *)(*param_1 + 800)) {
            cVar6 = *(char *)(puVar10 + -1);
            if (cVar6 != '\x04') {
              FUN_1403385b0();
              *(undefined1 *)(puVar10 + -1) = 4;
              cVar6 = '\x04';
            }
            puVar10[-2] = 0;
          }
          else if (*(longlong *)(lVar13 + 0x38 + lVar11) == *(longlong *)(*param_1 + 0x348)) {
            cVar6 = *(char *)(puVar10 + -1);
            if (cVar6 != '\x05') {
              FUN_1403385b0();
              *(undefined1 *)(puVar10 + -1) = 5;
              cVar6 = '\x05';
            }
            puVar10[-2] = 0;
          }
          else {
            if (*(longlong *)(lVar13 + 0x38 + lVar11) == *(longlong *)(*param_1 + 0x370)) {
              cVar6 = *(char *)(puVar10 + -1);
              if (cVar6 != '\x06') {
                FUN_1403385b0();
                *(undefined1 *)(puVar10 + -1) = 6;
                cVar6 = '\x06';
              }
              goto LAB_140423b95;
            }
            if (*(longlong *)(lVar13 + 0x38 + lVar11) != *(longlong *)(*param_1 + 0x398))
            goto LAB_140423a94;
            cVar6 = *(char *)(puVar10 + -1);
            if (cVar6 != '\a') {
              FUN_1403385b0();
              *(undefined1 *)(puVar10 + -1) = 7;
              cVar6 = '\a';
            }
            puVar10[-2] = 0;
          }
LAB_140423ba2:
          if ((pdVar17 != (double *)0x0) && (*(char *)(pdVar17 + 1) == cVar6)) {
            local_e8 = (double ***)ppppdVar2;
            FUN_140345ea0((longlong)*(char *)(pdVar17 + 1) + 1,&local_e8,pdVar17);
          }
          goto LAB_140423f0b;
        }
LAB_140423a94:
        if (*(int *)(lVar13 + 0x28 + lVar11) == 0xd) {
          if (*(longlong *)(lVar13 + 0x38 + lVar11) == *(longlong *)(*param_1 + 0x3c0)) {
            cVar6 = *(char *)(puVar10 + -1);
            if (cVar6 != '\b') {
              FUN_1403385b0();
              *(undefined1 *)(puVar10 + -1) = 8;
              cVar6 = '\b';
            }
            puVar10[-2] = 0;
          }
          else if (*(longlong *)(lVar13 + 0x38 + lVar11) == *(longlong *)(*param_1 + 1000)) {
            cVar6 = *(char *)(puVar10 + -1);
            if (cVar6 != '\t') {
              FUN_1403385b0();
              *(undefined1 *)(puVar10 + -1) = 9;
              cVar6 = '\t';
            }
            puVar10[-2] = 0;
          }
          else {
            if (*(longlong *)(lVar13 + 0x38 + lVar11) == *(longlong *)(*param_1 + 0x438)) {
              cVar6 = *(char *)(puVar10 + -1);
              if (cVar6 != '\n') {
                FUN_1403385b0();
                *(undefined1 *)(puVar10 + -1) = 10;
                cVar6 = '\n';
              }
            }
            else {
              if (*(longlong *)(lVar13 + 0x38 + lVar11) == *(longlong *)(*param_1 + 0x410)) {
                cVar6 = *(char *)(puVar10 + -1);
                if (cVar6 != '\v') {
                  FUN_1403385b0();
                  *(undefined1 *)(puVar10 + -1) = 0xb;
                  cVar6 = '\v';
                }
                puVar10[-2] = 0;
                goto LAB_140423ba2;
              }
              if (*(longlong *)(lVar13 + 0x38 + lVar11) != *(longlong *)(*param_1 + 0x460))
              goto LAB_140423bd2;
              cVar6 = *(char *)(puVar10 + -1);
              if (cVar6 != '\f') {
                FUN_1403385b0();
                *(undefined1 *)(puVar10 + -1) = 0xc;
                cVar6 = '\f';
              }
            }
LAB_140423b95:
            puVar10[-2] = 0;
          }
          goto LAB_140423ba2;
        }
LAB_140423bd2:
        lVar20 = *param_1;
        lVar19 = *(longlong *)(lVar13 + 0x38 + lVar11);
        if (((lVar19 == *(longlong *)(lVar20 + 0x4b0)) && (*(int *)(lVar13 + 0x28 + lVar11) == 0xd))
           && (*(int *)(lVar13 + 0x30 + lVar11) == 1)) {
          ppppdVar22 = (double ****)FUN_140983da8(0x18);
          *ppppdVar22 = (double ***)0x0;
          ppppdVar22[1] = (double ***)0x0;
          ppppdVar22[2] = (double ***)0x0;
          local_e8 = (double ***)ppppdVar22;
          if (*(char *)(puVar10 + -1) == '\x0e') {
            ppppdVar15 = (double ****)*ppppdVar2;
            *ppppdVar2 = (double ***)ppppdVar22;
          }
          else {
            FUN_1403385b0(ppppdVar2);
            *(undefined1 *)(puVar10 + -1) = 0xff;
            FUN_1403440f0(ppppdVar2);
            *(undefined1 *)(puVar10 + -1) = 0xe;
            ppppdVar15 = ppppdVar22;
          }
          goto LAB_140423def;
        }
        if (((lVar19 != *(longlong *)(lVar20 + 0x4d8)) || (*(int *)(lVar13 + 0x28 + lVar11) != 0xd))
           || (*(int *)(lVar13 + 0x30 + lVar11) != 1)) {
          if (((lVar19 != *(longlong *)(lVar20 + 0x500)) ||
              (*(int *)(lVar13 + 0x28 + lVar11) != 0xd)) || (*(int *)(lVar13 + 0x30 + lVar11) != 1))
          {
            if (((lVar19 == *(longlong *)(lVar20 + 0x528)) &&
                (*(int *)(lVar13 + 0x28 + lVar11) == 0xd)) &&
               (*(int *)(lVar13 + 0x30 + lVar11) == 1)) {
              local_e8 = (double ***)FUN_140983da8(0x18);
              *local_e8 = (double **)0x0;
              local_e8[1] = (double **)0x0;
              local_e8[2] = (double **)0x0;
              local_e0 = local_e8;
              FUN_140438ce0(ppppdVar2,&local_e0);
              ppppdVar15 = (double ****)local_e0;
              goto LAB_140423def;
            }
            if (((*(longlong *)(lVar13 + 0x38 + lVar11) != *(longlong *)(lVar20 + 0xb90)) ||
                (*(int *)(lVar13 + 0x28 + lVar11) != 0xd)) ||
               (*(int *)(lVar13 + 0x30 + lVar11) != 1)) goto LAB_140423f0b;
            local_e8 = (double ***)FUN_140983da8(0x18);
            *local_e8 = (double **)0x0;
            local_e8[1] = (double **)0x0;
            *local_e8 = (double **)0x0;
            local_e8[1] = (double **)0x0;
            local_e8[2] = (double **)0x0;
            local_e0 = local_e8;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
            switch(*(undefined1 *)(puVar10 + -1)) {
            case 0xe:
              goto switchD_140423cca_caseD_21;
            case 0xf:
              goto switchD_140423cca_caseD_22;
            case 0x10:
              goto switchD_140423cca_caseD_23;
            case 0x11:
              goto switchD_140423cca_caseD_24;
            default:
              local_e0 = *ppppdVar2;
              *ppppdVar2 = local_e8;
            }
            goto LAB_140423ee8;
          }
          local_e8 = (double ***)FUN_140983da8(0x18);
          *local_e8 = (double **)0x0;
          local_e8[1] = (double **)0x0;
          local_e8[2] = (double **)0x0;
          local_e0 = local_e8;
          FUN_140438c80(ppppdVar2,&local_e0);
          ppppdVar15 = (double ****)local_e0;
          goto LAB_140423def;
        }
        ppppdVar22 = (double ****)FUN_140983da8(0x18);
        *ppppdVar22 = (double ***)0x0;
        ppppdVar22[1] = (double ***)0x0;
        ppppdVar22[2] = (double ***)0x0;
        local_e8 = (double ***)ppppdVar22;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
        switch(*(undefined1 *)(puVar10 + -1)) {
        case 0x10:
          FUN_14033ba80(ppppdVar2);
          goto LAB_140423d0f;
        case 0x11:
          FUN_14033ba80(ppppdVar2);
LAB_140423d0f:
          *(undefined1 *)(puVar10 + -1) = 0xff;
          local_90 = FUN_140983da8(0x18);
          pppdVar12 = (double ***)FUN_14031f8f0(local_90,ppppdVar22);
          *ppppdVar2 = pppdVar12;
          *(undefined1 *)(puVar10 + -1) = 0xf;
          ppppdVar15 = ppppdVar22;
LAB_140423def:
          if (ppppdVar15 != (double ****)0x0) {
            if (*ppppdVar15 != (double ***)0x0) {
              free(*ppppdVar15);
            }
            free(ppppdVar15);
          }
          if ((pdVar17 != (double *)0x0) && (*(char *)(pdVar17 + 1) == *(char *)(puVar10 + -1)))
          goto LAB_140423f00;
          goto LAB_140423f0b;
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x16:
        case 0x17:
        case 0x18:
        case 0x19:
        case 0x1a:
        case 0x1b:
        case 0x1c:
        case 0x1d:
        case 0x1e:
        case 0x1f:
          break;
        case 0x20:
          FUN_14033ba80(ppppdVar2);
          break;
        case 0x21:
switchD_140423cca_caseD_21:
          FUN_14033ba80(ppppdVar2);
          break;
        case 0x22:
switchD_140423cca_caseD_22:
          FUN_14033ba80(ppppdVar2);
          break;
        case 0x23:
switchD_140423cca_caseD_23:
          FUN_14033ba80(ppppdVar2);
          break;
        case 0x24:
switchD_140423cca_caseD_24:
          FUN_14033ba80(ppppdVar2);
          break;
        default:
          ppppdVar15 = (double ****)*ppppdVar2;
          *ppppdVar2 = (double ***)ppppdVar22;
          goto LAB_140423def;
        }
        *(undefined1 *)(puVar10 + -1) = 0xff;
        FUN_140345130(ppppdVar2);
        *(undefined1 *)(puVar10 + -1) = 0xd;
LAB_140423ee8:
        FUN_14033ba80(&local_e0);
        if ((pdVar17 != (double *)0x0) && (*(char *)(pdVar17 + 1) == *(char *)(puVar10 + -1))) {
LAB_140423f00:
          FUN_140345dd0(ppppdVar2,pdVar17);
        }
LAB_140423f0b:
        local_c0 = local_c0 + 1;
        lVar11 = *(longlong *)(param_4 + 0x70);
      } while (local_c0 <
               (ulonglong)((*(longlong *)(param_4 + 0x78) - lVar11 >> 5) * -0x5555555555555555));
    }
    puVar5 = local_d0;
    *(undefined8 *)(param_3 + 8) = *(undefined8 *)(param_4 + 0x88);
    for (puVar21 = local_c8; puVar21 != puVar5; puVar21 = puVar21 + 3) {
      FUN_1403385b0(puVar21 + 1);
    }
    if (local_c8 != (ulonglong *)0x0) {
      free(local_c8);
    }
  }
  return;
}


// Incoming references
// 0xc22850 DATA caller none
// 0x427920 UNCONDITIONAL_CALL caller 1404277a0
// 0x439038 UNCONDITIONAL_CALL caller 140438e10
// 0x4926f8 UNCONDITIONAL_CALL caller 1404925d0
// 0x30da60 UNCONDITIONAL_CALL caller 14030d940
