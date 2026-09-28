
void FUN_1403a81f0(longlong *param_1,undefined8 *param_2)

{
  double *pdVar1;
  int *piVar2;
  undefined8 *puVar3;
  undefined8 *puVar4;
  void *pvVar5;
  void *_Memory;
  double dVar6;
  double dVar7;
  ulonglong uVar8;
  undefined8 *puVar9;
  double *pdVar10;
  int *piVar11;
  double *pdVar12;
  int *piVar13;
  uint uVar14;
  longlong *plVar15;
  longlong lVar16;
  ulonglong uVar17;
  undefined8 local_res8;
  undefined8 local_res10;
  undefined1 local_88 [24];
  undefined1 local_70 [8];
  longlong *local_68;
  ulonglong local_60;
  undefined8 local_58;
  undefined4 local_50;
  undefined8 local_4c;
  
  uVar14 = *(uint *)(param_2 + 6);
  uVar17 = 0;
  if (((int)uVar14 < 0) ||
     ((int)(*(longlong *)(*(longlong *)(*param_1 + 0x408) + 0x128) -
            *(longlong *)(*(longlong *)(*param_1 + 0x408) + 0x120) >> 3) * -0x49249249 < (int)uVar14
     )) {
    *(undefined4 *)(param_2 + 6) = 0;
    uVar14 = 0;
  }
  if (*(char *)((ulonglong)uVar14 * 0x38 + 0x31 +
               *(longlong *)(*(longlong *)(*param_1 + 0x408) + 0x120)) == '\0') {
    *(undefined1 *)((longlong)param_2 + 0x6c) = 1;
  }
  local_50 = 0x3f800000;
  local_4c = 0x40000000;
  local_60 = 1;
  local_68 = &DAT_140b5abc0;
  local_58 = 0;
  for (lVar16 = (longlong)(param_2[0x10] - param_2[0xf]) >> 3; 0 < lVar16; lVar16 = lVar16 + -1) {
    FUN_140362640(local_70,local_88);
  }
  param_2[0x10] = param_2[0xf];
  puVar9 = (undefined8 *)local_68[local_60];
  puVar3 = (undefined8 *)*local_68;
  plVar15 = local_68;
  if (puVar3 == (undefined8 *)0x0) {
    plVar15 = local_68 + 1;
    lVar16 = *plVar15;
    while (lVar16 == 0) {
      plVar15 = plVar15 + 1;
      lVar16 = *plVar15;
    }
    puVar3 = (undefined8 *)*plVar15;
  }
  while (puVar3 != puVar9) {
    puVar4 = (undefined8 *)param_2[0x10];
    if (puVar4 < (undefined8 *)param_2[0x11]) {
      param_2[0x10] = puVar4 + 1;
      *puVar4 = *puVar3;
    }
    else {
      FUN_140253b10(param_2 + 0xf,puVar3);
    }
    puVar3 = (undefined8 *)puVar3[1];
    while (puVar3 == (undefined8 *)0x0) {
      plVar15 = plVar15 + 1;
      puVar3 = (undefined8 *)*plVar15;
    }
  }
  if (*(char *)(param_2 + 1) != '\0') {
    puVar9 = param_2 + 2;
    param_2[4] = 0;
    if (0xf < (ulonglong)param_2[5]) {
      puVar9 = (undefined8 *)*puVar9;
    }
    *(undefined1 *)puVar9 = 0;
  }
  dVar7 = DAT_140aabcf8;
  dVar6 = DAT_140aab838;
  local_res10 = DAT_140aabcf8;
  local_res8 = DAT_140aab838;
  pdVar1 = (double *)(param_2 + 9);
  pdVar12 = (double *)&local_res8;
  if (DAT_140aab838 <= *pdVar1) {
    pdVar12 = pdVar1;
  }
  pdVar10 = (double *)&local_res10;
  if (*pdVar1 <= DAT_140aabcf8) {
    pdVar10 = pdVar12;
  }
  *pdVar1 = *pdVar10;
  local_res10._4_4_ = (undefined4)((ulonglong)dVar7 >> 0x20);
  local_res10 = (double)CONCAT44(local_res10._4_4_,1);
  local_res8._4_4_ = (uint)((ulonglong)dVar6 >> 0x20);
  uVar14 = local_res8._4_4_;
  local_res8 = (double)CONCAT44(local_res8._4_4_,0xffffffff);
  piVar2 = (int *)(param_2 + 0xb);
  piVar13 = (int *)&local_res8;
  if (-2 < *piVar2) {
    piVar13 = piVar2;
  }
  piVar11 = (int *)&local_res10;
  if (*piVar2 < 2) {
    piVar11 = piVar13;
  }
  *piVar2 = *piVar11;
  local_res10 = (double)CONCAT44(local_res10._4_4_,4);
  local_res8 = (double)((ulonglong)local_res8._4_4_ << 0x20);
  piVar2 = (int *)((longlong)param_2 + 0x5c);
  piVar13 = (int *)&local_res8;
  if (-1 < *piVar2) {
    piVar13 = piVar2;
  }
  piVar11 = (int *)&local_res10;
  if (*piVar2 < 5) {
    piVar11 = piVar13;
  }
  *piVar2 = *piVar11;
  local_res10 = (double)CONCAT44(local_res10._4_4_,3);
  local_res8 = (double)((ulonglong)uVar14 << 0x20);
  piVar2 = (int *)(param_2 + 0xc);
  piVar13 = (int *)&local_res8;
  if (-1 < *piVar2) {
    piVar13 = piVar2;
  }
  piVar11 = (int *)&local_res10;
  if (*piVar2 < 4) {
    piVar11 = piVar13;
  }
  *piVar2 = *piVar11;
  FUN_1404277a0(*param_1 + 0x1508,*param_1,*param_2,param_2 + 0x12);
  uVar8 = local_60;
  plVar15 = local_68;
  if (local_60 != 0) {
    do {
      _Memory = (void *)plVar15[uVar17];
      while (_Memory != (void *)0x0) {
        pvVar5 = *(void **)((longlong)_Memory + 8);
        free(_Memory);
        _Memory = pvVar5;
      }
      plVar15[uVar17] = 0;
      uVar17 = uVar17 + 1;
    } while (uVar17 < uVar8);
  }
  local_58 = 0;
  if (1 < local_60) {
    free(local_68);
  }
  return;
}

