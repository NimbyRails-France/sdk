// Candidate VA 140452250; RVA 0x452250
// Ghidra inferred prototype: undefined FUN_140452250()

char * FUN_140452250(undefined8 param_1,char *param_2)

{
  ulonglong *puVar1;
  double *pdVar2;
  undefined1 auVar3 [16];
  undefined8 *puVar4;
  undefined8 uVar5;
  longlong *plVar6;
  longlong lVar7;
  ulonglong uVar8;
  double *pdVar9;
  undefined8 *puVar10;
  undefined4 uVar11;
  undefined4 uVar12;
  double dVar13;
  undefined4 uVar14;
  undefined4 uVar15;
  double dVar16;
  
  if (*param_2 == '\0') {
    while (plVar6 = *(longlong **)(param_2 + 0x28), plVar6 != (longlong *)0x0) {
      puVar1 = (ulonglong *)(param_2 + 0x38);
      if (((*plVar6 == plVar6[1]) || (*(longlong *)(*plVar6 + *puVar1 * 8) == 0)) &&
         ((*(longlong *)(param_2 + 0x40) == 0 ||
          (*(longlong *)(param_2 + 0x40) == *(longlong *)(param_2 + 0x48))))) break;
      plVar6 = *(longlong **)(param_2 + 0x28);
      lVar7 = *plVar6;
      if ((lVar7 == plVar6[1]) ||
         (plVar6 = *(longlong **)(param_2 + 0x28), *(longlong *)(lVar7 + *puVar1 * 8) == 0)) {
        puVar10 = *(undefined8 **)(param_2 + 0x40);
      }
      else {
        puVar10 = (undefined8 *)(*puVar1 * 0x20 + plVar6[3]);
      }
      lVar7 = *plVar6;
      if (plVar6[1] - lVar7 >> 3 != 0) {
        while (*(longlong *)(lVar7 + *puVar1 * 8) != 0) {
          *puVar1 = *puVar1 + 1;
          plVar6 = *(longlong **)(param_2 + 0x28);
          uVar8 = (plVar6[1] - *plVar6 >> 3) - 1U & *puVar1;
          *puVar1 = uVar8;
          lVar7 = *(longlong *)(*plVar6 + uVar8 * 8);
          if (((lVar7 == *(longlong *)(param_2 + 0x30)) || (lVar7 == 0)) ||
             (lVar7 = **(longlong **)(param_2 + 0x28), (*(longlong **)(param_2 + 0x28))[1] == lVar7)
             ) break;
        }
      }
      lVar7 = **(longlong **)(param_2 + 0x28);
      if (((lVar7 == (*(longlong **)(param_2 + 0x28))[1]) ||
          (*(longlong *)(lVar7 + *(longlong *)(param_2 + 0x38) * 8) == 0)) &&
         (*(ulonglong *)(param_2 + 0x40) < *(ulonglong *)(param_2 + 0x48))) {
        *(ulonglong *)(param_2 + 0x40) = *(ulonglong *)(param_2 + 0x40) + 0x20;
      }
      if (puVar10 == (undefined8 *)0x0) break;
      lVar7 = FUN_14033f780(*(longlong *)(param_2 + 0x58) + 0x200,*puVar10);
      if (lVar7 != 0) {
        pdVar2 = (double *)(puVar10 + 2);
        pdVar9 = (double *)(puVar10 + 1);
        if ((double)puVar10[1] < *pdVar2) {
          pdVar9 = pdVar2;
        }
        if (*(double *)(param_2 + 8) <= *pdVar9) {
          pdVar9 = (double *)(puVar10 + 1);
          if (*pdVar2 < (double)puVar10[1]) {
            pdVar9 = pdVar2;
          }
          if (*pdVar9 < *(double *)(param_2 + 0x10) || *pdVar9 == *(double *)(param_2 + 0x10)) {
            *(longlong *)(param_2 + 0x68) = lVar7;
            param_2[0x60] = '\x01';
            return param_2 + 0x60;
          }
        }
      }
    }
    *param_2 = '\x01';
  }
  else if (*param_2 != '\x01') {
    return (char *)0x0;
  }
  if (param_2[1] != '\0') {
    puVar10 = *(undefined8 **)(param_2 + 0x20);
    while (puVar4 = *(undefined8 **)(param_2 + 0x18), puVar4 != puVar10) {
      uVar5 = *puVar4;
      auVar3 = *(undefined1 (*) [16])(puVar4 + 2);
      dVar13 = auVar3._0_8_;
      uVar14 = auVar3._8_4_;
      uVar15 = auVar3._12_4_;
      *(undefined8 **)(param_2 + 0x18) = puVar4 + 4;
      lVar7 = FUN_14033f780(*(longlong *)(param_2 + 0x58) + 0x200,uVar5);
      if (lVar7 != 0) {
        dVar16 = (double)CONCAT44(uVar15,uVar14);
        uVar11 = SUB84(dVar13,0);
        uVar12 = (int)((ulonglong)dVar13 >> 0x20);
        if (dVar13 <= dVar16) {
          uVar11 = uVar14;
          uVar12 = uVar15;
        }
        if (*(double *)(param_2 + 8) <= (double)CONCAT44(uVar12,uVar11)) {
          if (dVar16 <= dVar13) {
            dVar13 = dVar16;
          }
          if (dVar13 < *(double *)(param_2 + 0x10) || dVar13 == *(double *)(param_2 + 0x10)) {
            *(longlong *)(param_2 + 0x68) = lVar7;
            param_2[0x60] = '\0';
            return param_2 + 0x60;
          }
        }
      }
    }
  }
  *param_2 = '\x02';
  return (char *)0x0;
}


// Incoming references
// 0xc238f4 DATA caller none
// 0x454d54 DATA caller 140452f40
