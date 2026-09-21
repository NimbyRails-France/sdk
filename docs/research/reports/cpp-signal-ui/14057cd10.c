// Candidate VA 14057cd10; RVA 0x57cd10
// Ghidra inferred prototype: undefined FUN_14057cd10()

char FUN_14057cd10(undefined8 *param_1,undefined8 param_2)

{
  undefined8 *puVar1;
  undefined8 uVar2;
  undefined8 *puVar3;
  undefined8 *puVar4;
  ulonglong uVar5;
  longlong *plVar6;
  longlong *plVar7;
  longlong *plVar8;
  longlong lVar9;
  undefined8 *puVar10;
  undefined8 *puVar11;
  longlong lVar12;
  longlong *plVar13;
  ulonglong uVar14;
  longlong *plVar15;
  longlong lVar16;
  longlong *plVar17;
  longlong *plVar18;
  char cVar19;
  bool bVar20;
  undefined1 local_res8 [8];
  longlong local_38;
  ulonglong local_30;

  *(undefined1 *)(param_1 + 0xe) = *(undefined1 *)((longlong)param_1 + 0x71);
  FUN_140573630(param_1 + 0x12);
  cVar19 = '\0';
  plVar18 = (longlong *)0x0;
  if ((*(char *)(param_1 + 0xe) != '\0') && (*(char *)(param_1 + 0x2e) != '\0')) {
    lVar9 = param_1[0x2d];
    plVar6 = (longlong *)param_1[0x40];
    plVar8 = param_1 + 0x3e;
    if (plVar6 != (longlong *)0x0) {
      do {
        plVar7 = plVar6;
        if (lVar9 <= plVar6[4]) {
          plVar7 = plVar6 + 1;
          plVar8 = plVar6;
        }
        plVar6 = (longlong *)*plVar7;
      } while (plVar6 != (longlong *)0x0);
      if ((plVar8 != param_1 + 0x3e) && (plVar8[4] <= lVar9)) {
        plVar6 = (longlong *)param_1[0x4b];
        plVar7 = plVar18;
        plVar15 = plVar18;
        if (plVar6 != param_1 + 0x4a) {
          do {
            for (plVar13 = (longlong *)plVar6[5]; plVar13 != (longlong *)plVar6[6];
                plVar13 = plVar13 + 1) {
              if (*plVar13 == lVar9) {
                plVar7 = (longlong *)plVar6[4];
                plVar15 = *(longlong **)(plVar8[5] + 0x20);
              }
            }
            plVar13 = (longlong *)*plVar6;
            if (plVar13 == (longlong *)0x0) {
              plVar13 = (longlong *)plVar6[2];
              plVar17 = plVar18;
              if (plVar6 == (longlong *)*plVar13) {
                do {
                  plVar6 = plVar13;
                  plVar13 = (longlong *)plVar6[2];
                } while (plVar6 == (longlong *)*plVar13);
                plVar17 = (longlong *)*plVar6;
              }
              if (plVar17 != plVar13) {
                plVar6 = plVar13;
              }
            }
            else {
              for (plVar17 = (longlong *)plVar13[1]; plVar6 = plVar13, plVar17 != (longlong *)0x0;
                  plVar17 = (longlong *)plVar17[1]) {
                plVar13 = plVar17;
              }
            }
          } while (plVar6 != param_1 + 0x4a);
          if ((plVar7 != (longlong *)0x0) && (plVar15 != (longlong *)0x0)) {
            plVar6 = (longlong *)param_1[0x51];
            plVar18 = (longlong *)param_1[0x50];
            uVar14 = (longlong)plVar6 - (longlong)plVar18 >> 3;
            while (uVar5 = uVar14, 0 < (longlong)uVar5) {
              uVar14 = uVar5 >> 1;
              if (plVar18[uVar14] < lVar9) {
                plVar18 = plVar18 + uVar14 + 1;
                uVar14 = uVar5 + (-1 - uVar14);
              }
            }
            if ((plVar18 == plVar6) || (plVar8 = plVar18 + 1, lVar9 < *plVar18)) {
              plVar8 = plVar18;
            }
            if (plVar18 == plVar8) {
              plVar18 = plVar6;
            }
            if (plVar18 == plVar6) {
              lVar9 = FUN_140585910(param_2);
            }
            else {
              lVar9 = FUN_140585750();
            }
            cVar19 = '\x01';
            *(undefined8 *)(lVar9 + 0x20) = *param_1;
            uVar2 = param_1[1];
            *(longlong **)(lVar9 + 0x38) = plVar15;
            *(longlong **)(lVar9 + 0x30) = plVar7;
            *(undefined8 *)(lVar9 + 0x28) = uVar2;
          }
        }
      }
    }
  }
  lVar9 = param_1[0xc];
  lVar16 = param_1[0xb];
  if (lVar16 != lVar9) {
    puVar1 = param_1 + 2;
    do {
      local_38 = *(longlong *)(lVar16 + 0x20);
      local_30 = *(ulonglong *)(lVar16 + 0x28);
      puVar3 = (undefined8 *)param_1[4];
      puVar11 = puVar1;
      while (puVar3 != (undefined8 *)0x0) {
        if (((longlong)puVar3[4] < local_38) ||
           (((longlong)puVar3[4] <= local_38 && ((ulonglong)puVar3[5] < local_30)))) {
          bVar20 = true;
        }
        else {
          bVar20 = false;
        }
        puVar10 = puVar3 + 1;
        puVar4 = puVar3;
        if (bVar20) {
          puVar10 = puVar3;
          puVar4 = puVar11;
        }
        puVar11 = puVar4;
        puVar3 = (undefined8 *)*puVar10;
      }
      if (((puVar11 == puVar1) || (local_38 < (longlong)puVar11[4])) ||
         ((local_38 <= (longlong)puVar11[4] && (local_30 < (ulonglong)puVar11[5])))) {
        puVar11 = (undefined8 *)FUN_140586c70(puVar1,local_res8,local_38,puVar11,&local_38);
        puVar11 = (undefined8 *)*puVar11;
      }
      bVar20 = *(char *)((longlong)puVar11 + 0x111) != '\0';
      if (bVar20) {
        lVar12 = FUN_140582aa0(param_2);
        *(undefined8 *)(lVar12 + 0x20) = puVar11[6];
        *(undefined8 *)(lVar12 + 0x28) = puVar11[7];
        FUN_140344bb0(lVar12 + 0x30);
        puVar11[0x13] = 0;
      }
      if ((cVar19 != '\0') || (bVar20)) {
        cVar19 = '\x01';
      }
      lVar16 = lVar16 + 0x30;
    } while (lVar16 != lVar9);
  }
  FUN_14057bf70(param_1 + 0x37);
  *(undefined1 *)(param_1 + 0xe) = *(undefined1 *)((longlong)param_1 + 0x71);
  if (param_1 + 8 != param_1 + 0xf) {
    FUN_140342d30(param_1 + 8,param_1[0xf],param_1[0x10]);
  }
  return cVar19;
}


// Incoming references
// 0xc2e550 DATA caller none
// 0xb1d26c DATA caller none
// 0xb1d2a0 DATA caller none
// 0x581f09 UNCONDITIONAL_CALL caller 140581d70
// 0x5ad97a UNCONDITIONAL_CALL caller 1405ad160
// 0x5b3287 UNCONDITIONAL_CALL caller 1405b1360
// 0x66f90d UNCONDITIONAL_CALL caller 14066f4e0
// 0x70eb4c UNCONDITIONAL_CALL caller 14070e370
// 0x6f7198 UNCONDITIONAL_CALL caller 1406f6f50
// 0x757c82 UNCONDITIONAL_CALL caller 140757a90
// 0x7a0548 UNCONDITIONAL_CALL caller 14079f4d0
// 0x7fe58c UNCONDITIONAL_CALL caller 1407fd860
