// Candidate VA 140442f40; RVA 0x442f40
// Ghidra inferred prototype: undefined FUN_140442f40()

undefined8 FUN_140442f40(longlong param_1,longlong *param_2,longlong *param_3)

{
  double dVar1;
  double dVar2;
  ulonglong *puVar3;
  longlong lVar4;
  undefined1 auVar5 [16];
  double *pdVar6;
  double *pdVar7;
  undefined8 uVar8;
  double *pdVar9;
  longlong lVar10;
  longlong *plVar11;
  ulonglong uVar12;
  longlong *plVar13;
  ulonglong *puVar14;
  undefined4 uVar15;
  undefined4 uVar16;
  undefined4 uVar17;
  undefined4 uVar18;
  ulonglong local_res20;
  undefined8 local_88;
  char local_80;
  longlong *local_78;
  longlong local_70;
  ulonglong local_68;
  longlong *local_60;
  longlong *local_58;
  
  puVar3 = (ulonglong *)param_3[1];
  puVar14 = (ulonglong *)*param_3;
  if (puVar14 == puVar3) {
    uVar8 = 2;
  }
  else {
    do {
      dVar1 = (double)puVar14[1];
      uVar15 = SUB84(dVar1,0);
      uVar16 = (undefined4)((ulonglong)dVar1 >> 0x20);
      local_res20 = *puVar14;
      lVar4 = *param_2;
      uVar8 = *(undefined8 *)(param_1 + 0x68);
      dVar2 = (double)puVar14[2];
      uVar17 = uVar15;
      uVar18 = uVar16;
      if (dVar2 <= dVar1) {
        uVar17 = SUB84(dVar2,0);
        uVar18 = (undefined4)((ulonglong)dVar2 >> 0x20);
      }
      dVar2 = (double)puVar14[2];
      if (dVar1 <= dVar2) {
        uVar15 = SUB84(dVar2,0);
        uVar16 = (undefined4)((ulonglong)dVar2 >> 0x20);
      }
      auVar5._8_8_ = 0;
      auVar5._0_8_ = local_res20;
      local_88 = 0;
      local_80 = '\0';
      FUN_14045f860(*(longlong *)(param_1 + 0xd8) + 0x880,&local_78,&local_res20,
                    SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar5,8) +
                    SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar5,0),&local_88);
      if (local_80 != '\0') {
        _Mtx_unlock(local_88);
      }
      if (local_78 != (longlong *)0x0) {
        plVar11 = (longlong *)local_60[1];
        if (plVar11 != (longlong *)local_60[2]) {
          pdVar9 = (double *)(plVar11 + 2);
          do {
            lVar10 = *plVar11;
            if (lVar10 != lVar4) {
              pdVar7 = pdVar9 + 1;
              pdVar6 = pdVar9;
              if (*pdVar9 < *pdVar7) {
                pdVar6 = pdVar7;
              }
              if ((double)CONCAT44(uVar18,uVar17) < *pdVar6) {
                pdVar6 = pdVar9;
                if (*pdVar7 < *pdVar9) {
                  pdVar6 = pdVar7;
                }
                if (*pdVar6 <= (double)CONCAT44(uVar16,uVar15) &&
                    (double)CONCAT44(uVar16,uVar15) != *pdVar6) goto LAB_1404431a5;
              }
            }
            plVar11 = plVar11 + 4;
            pdVar9 = pdVar9 + 4;
          } while (plVar11 != (longlong *)local_60[2]);
        }
      }
      FUN_140459ea0(uVar8,&local_78,local_res20);
      plVar11 = local_60;
      uVar12 = local_68;
      do {
        do {
          do {
            if ((local_78 == (longlong *)0x0) ||
               (((*local_78 == local_78[1] || (*(longlong *)(*local_78 + uVar12 * 8) == 0)) &&
                ((plVar11 == (longlong *)0x0 || (plVar11 == local_58)))))) goto LAB_1404431aa;
            plVar13 = plVar11;
            if ((*local_78 != local_78[1]) && (*(longlong *)(*local_78 + uVar12 * 8) != 0)) {
              plVar13 = (longlong *)(uVar12 * 0x20 + local_78[3]);
            }
            lVar10 = *local_78;
            if (local_78[1] - lVar10 >> 3 != 0) {
              while (*(longlong *)(lVar10 + uVar12 * 8) != 0) {
                uVar12 = (local_78[1] - *local_78 >> 3) - 1U & uVar12 + 1;
                lVar10 = *(longlong *)(*local_78 + uVar12 * 8);
                if (((lVar10 == local_70) || (lVar10 == 0)) ||
                   (lVar10 = *local_78, local_78[1] == lVar10)) break;
              }
            }
            if (((*local_78 == local_78[1]) || (*(longlong *)(*local_78 + uVar12 * 8) == 0)) &&
               (plVar11 < local_58)) {
              plVar11 = plVar11 + 4;
            }
            if (plVar13 == (longlong *)0x0) goto LAB_1404431aa;
            lVar10 = *plVar13;
          } while (lVar10 == lVar4);
          pdVar9 = (double *)(plVar13 + 2);
          pdVar6 = (double *)(plVar13 + 1);
          pdVar7 = pdVar6;
          if (*pdVar6 < *pdVar9) {
            pdVar7 = pdVar9;
          }
        } while (*pdVar7 <= (double)CONCAT44(uVar18,uVar17) &&
                 (double)CONCAT44(uVar18,uVar17) != *pdVar7);
        if (*pdVar9 < *pdVar6) {
          pdVar6 = pdVar9;
        }
      } while ((double)CONCAT44(uVar16,uVar15) < *pdVar6);
LAB_1404431a5:
      if (lVar10 != 0) {
        return 1;
      }
LAB_1404431aa:
      puVar14 = puVar14 + 4;
    } while (puVar14 != puVar3);
    uVar8 = 0;
  }
  return uVar8;
}


// Incoming references
// 0xc233b4 DATA caller none
// 0x445704 UNCONDITIONAL_CALL caller 140444230
// 0x44604d UNCONDITIONAL_CALL caller 140445fe0
