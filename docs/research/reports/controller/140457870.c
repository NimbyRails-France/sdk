
void FUN_140457870(longlong param_1,undefined8 *param_2,longlong *param_3,longlong *param_4,
                  char param_5,undefined8 param_6,undefined8 param_7,undefined8 param_8)

{
  longlong lVar1;
  ulonglong *puVar2;
  double dVar3;
  double dVar4;
  uint uVar5;
  longlong lVar6;
  double dVar7;
  char cVar8;
  longlong *plVar9;
  char cVar10;
  int iVar11;
  ulonglong uVar12;
  int iVar13;
  longlong lVar14;
  longlong lVar15;
  byte bVar16;
  longlong lVar17;
  int local_res8 [2];
  undefined8 *local_res10;
  longlong *local_res18;
  int local_res20;
  double local_d8;
  longlong local_d0;
  ulonglong local_c8;
  undefined8 local_c0;
  undefined8 *local_b8;
  longlong local_b0;
  undefined1 local_a8 [104];

  lVar14 = *param_4;
  local_b8 = (undefined8 *)(param_1 + 0x28);
  if (lVar14 == 0) {
    return;
  }
  local_res10 = param_2;
  local_res18 = param_3;
  plVar9 = (longlong *)FUN_14032c420(*local_b8,lVar14);
  if (plVar9 == (longlong *)0x0) {
    return;
  }
  cVar8 = *(char *)((longlong)param_4 + 0x11);
  local_d8 = (double)param_4[1];
  local_c0 = 0;
  if (cVar8 != '\0') {
    if ((*plVar9 == lVar14) && (*(char *)((longlong)plVar9 + 0x2c) != cVar8)) {
      local_d8 = DAT_140aabd08 - local_d8;
    }
    if (*plVar9 == lVar14) {
      cVar10 = (char)param_4[2];
      if (*(char *)((longlong)plVar9 + 0x2c) != cVar8) {
        cVar10 = -(char)param_4[2];
      }
      goto LAB_140457910;
    }
  }
  cVar10 = (char)param_4[2];
LAB_140457910:
  dVar3 = local_d8;
  lVar1 = *(longlong *)(param_1 + 8);
  iVar13 = 0;
  local_res8[0] = (int)cVar10;
  uVar12 = param_3[1] - *param_3 >> 5;
  local_d0 = lVar14;
  cVar8 = FUN_1403e1220(param_3,lVar1,param_2 + 0x10);
  iVar11 = (int)uVar12;
  if ((cVar8 == '\0') || (param_5 == '\0')) {
    bVar16 = 0;
  }
  else {
    bVar16 = 1;
    iVar13 = iVar11 + -1;
  }
  uVar5 = (bVar16 ^ 1) * 2;
  local_res20 = uVar5 - 1;
  if (0 < iVar11) {
    local_c8 = uVar12 & 0xffffffff;
    lVar17 = (longlong)iVar13 << 5;
    local_b0 = (ulonglong)uVar5 * 0x20 + -0x20;
    do {
      for (puVar2 = *(ulonglong **)
                     (*(longlong *)(lVar1 + 0x38) +
                     (*(ulonglong *)(lVar17 + *param_3) % (ulonglong)*(uint *)(lVar1 + 0x40)) * 8);
          puVar2 != (ulonglong *)0x0; puVar2 = (ulonglong *)puVar2[0x54]) {
        if (*(ulonglong *)(lVar17 + *param_3) == *puVar2) {
          if (puVar2 != *(ulonglong **)
                         (*(longlong *)(lVar1 + 0x38) + *(longlong *)(lVar1 + 0x40) * 8)) {
            FUN_14043f670(local_a8,local_b8,*local_res10,(double)*(float *)((longlong)puVar2 + 0xfc)
                          ,param_6,param_7,&local_d0,&local_c0,&local_d8,local_res8,0,0,0);
            dVar7 = local_d8;
            dVar4 = local_d8;
            if (bVar16 != 0) {
              dVar4 = dVar3;
              dVar3 = local_d8;
            }
            plVar9 = (longlong *)FUN_140457750(param_8);
            lVar15 = lVar14;
            lVar6 = local_d0;
            if (bVar16 == 0) {
              lVar15 = local_d0;
              lVar6 = lVar14;
            }
            *plVar9 = (longlong)(puVar2 + 1);
            plVar9[3] = lVar15;
            plVar9[4] = (longlong)dVar3;
            plVar9[5] = (longlong)dVar4;
            *(int *)(plVar9 + 1) = iVar13;
            plVar9[2] = lVar6;
            lVar14 = local_d0;
            param_3 = local_res18;
            dVar3 = dVar7;
          }
          break;
        }
      }
      iVar13 = iVar13 + local_res20;
      lVar17 = lVar17 + local_b0;
      local_c8 = local_c8 - 1;
    } while (local_c8 != 0);
  }
  return;
}
