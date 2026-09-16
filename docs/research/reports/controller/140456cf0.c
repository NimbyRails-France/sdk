
/* WARNING: Type propagation algorithm not settling */

void FUN_140456cf0(longlong param_1,undefined8 *param_2,longlong *param_3,longlong *param_4,
                  char param_5,undefined8 param_6,undefined8 param_7,undefined8 param_8)

{
  longlong lVar1;
  ulonglong *puVar2;
  char cVar3;
  longlong *plVar4;
  char cVar5;
  int iVar6;
  ulonglong uVar7;
  int iVar8;
  longlong lVar9;
  byte bVar10;
  int local_res8 [2];
  undefined8 *local_res10;
  longlong *local_res18;
  double local_res20;
  longlong local_a8 [2];
  undefined8 *local_98;
  undefined1 local_90 [80];

  lVar9 = *param_4;
  local_98 = (undefined8 *)(param_1 + 0x28);
  if (lVar9 == 0) {
    return;
  }
  local_res10 = param_2;
  local_res18 = param_3;
  plVar4 = (longlong *)FUN_14032c420(*local_98,lVar9);
  if (plVar4 == (longlong *)0x0) {
    return;
  }
  cVar3 = *(char *)((longlong)param_4 + 0x11);
  local_res20 = (double)param_4[1];
  local_a8[1] = 0;
  if (cVar3 != '\0') {
    if ((*plVar4 == lVar9) && (*(char *)((longlong)plVar4 + 0x2c) != cVar3)) {
      local_res20 = DAT_140aabd08 - local_res20;
    }
    if (*plVar4 == lVar9) {
      cVar5 = (char)param_4[2];
      if (*(char *)((longlong)plVar4 + 0x2c) != cVar3) {
        cVar5 = -(char)param_4[2];
      }
      goto LAB_140456d7c;
    }
  }
  cVar5 = (char)param_4[2];
LAB_140456d7c:
  lVar1 = *(longlong *)(param_1 + 8);
  iVar8 = 0;
  local_res8[0] = (int)cVar5;
  uVar7 = param_3[1] - *param_3 >> 5;
  local_a8[0] = lVar9;
  cVar3 = FUN_1403e1220(param_3,lVar1,param_2 + 0x10);
  iVar6 = (int)uVar7;
  if ((cVar3 == '\0') || (param_5 == '\0')) {
    bVar10 = 0;
  }
  else {
    bVar10 = 1;
    iVar8 = iVar6 + -1;
  }
  if (0 < iVar6) {
    lVar9 = (longlong)iVar8 << 5;
    uVar7 = uVar7 & 0xffffffff;
    do {
      for (puVar2 = *(ulonglong **)
                     (*(longlong *)(lVar1 + 0x38) +
                     (*(ulonglong *)(lVar9 + *param_3) % (ulonglong)*(uint *)(lVar1 + 0x40)) * 8);
          puVar2 != (ulonglong *)0x0; puVar2 = (ulonglong *)puVar2[0x54]) {
        if (*(ulonglong *)(lVar9 + *param_3) == *puVar2) {
          if (puVar2 != *(ulonglong **)
                         (*(longlong *)(lVar1 + 0x38) + *(longlong *)(lVar1 + 0x40) * 8)) {
            FUN_14043f670(local_90,local_98,*local_res10,(double)*(float *)((longlong)puVar2 + 0xfc)
                          ,param_6,param_7,local_a8,local_a8 + 1,&local_res20,local_res8,0,0,0);
            FUN_140446d60(param_8);
            param_3 = local_res18;
          }
          break;
        }
      }
      lVar9 = lVar9 + ((ulonglong)bVar10 ^ 1) * 0x40 + -0x20;
      uVar7 = uVar7 - 1;
    } while (uVar7 != 0);
  }
  return;
}
