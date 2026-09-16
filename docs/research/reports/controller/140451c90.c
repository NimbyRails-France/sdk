
void FUN_140451c90(undefined8 param_1,longlong param_2,undefined8 param_3,ulonglong *param_4)

{
  char cVar1;
  longlong *plVar2;
  longlong lVar3;
  longlong lVar4;
  void *_Memory;
  undefined1 auVar5 [16];
  double dVar6;
  longlong *plVar7;
  longlong *plVar8;
  undefined8 *puVar9;
  byte bVar10;
  undefined8 *puVar11;
  double dVar12;
  double dVar13;
  undefined1 local_res8 [8];
  ulonglong *local_res20;
  ulonglong ****ppppuVar14;
  ulonglong ***local_148;
  undefined1 *local_140;
  char local_138 [8];
  longlong local_130;
  longlong local_128;
  ulonglong **local_120;
  undefined1 *local_118;
  void **local_110;
  undefined8 local_108;
  void *local_f8;
  undefined8 local_f0;
  undefined8 local_e8;
  undefined8 uStack_e0;
  undefined8 local_d8;
  undefined8 *puStack_d0;
  undefined8 local_c8;
  undefined8 uStack_c0;
  undefined8 local_b8;
  longlong lStack_b0;
  undefined1 local_98 [112];

  local_res20 = param_4;
  FUN_1404512d0(param_1,*param_4);
  local_f8 = (void *)0x0;
  local_f0 = 0;
  local_e8 = 0;
  uStack_e0 = 0;
  local_d8 = 0;
  puStack_d0 = (undefined8 *)0x0;
  local_c8 = 0;
  uStack_c0 = 0;
  local_b8 = 0;
  lStack_b0 = 0;
  FUN_140461850(&local_f8);
  local_res8[0] = 1;
  local_120 = &local_res20;
  local_118 = local_res8;
  local_110 = &local_f8;
  local_148 = &local_120;
  local_140 = &stack0x00000028;
  ppppuVar14 = &local_148;
  local_108 = param_1;
  FUN_140458750(param_2,param_3,local_res20);
  dVar6 = DAT_140aabd08;
  if ((((char)local_res20[0x96] != '\0') && ((char)local_res20[100] != '\0')) &&
     (*(char *)((longlong)local_res20 + 0x329) != '\0')) {
    plVar2 = (longlong *)local_res20[0x7e];
    for (plVar8 = (longlong *)local_res20[0x7d]; plVar8 != plVar2; plVar8 = plVar8 + 4) {
      lVar3 = *plVar8;
      if ((lVar3 != 0) &&
         (plVar7 = (longlong *)FUN_14032c420(*(undefined8 *)(param_2 + 0x428),lVar3),
         plVar7 != (longlong *)0x0)) {
        cVar1 = (char)plVar8[3];
        dVar13 = (double)plVar8[2];
        if ((cVar1 == '\0') || (*plVar7 != lVar3)) {
LAB_140451e00:
          dVar12 = (double)plVar8[1];
        }
        else {
          if (*(char *)((longlong)plVar7 + 0x2c) != cVar1) {
            dVar13 = dVar6 - dVar13;
          }
          if (*plVar7 != lVar3) goto LAB_140451e00;
          dVar12 = (double)plVar8[1];
          if (*(char *)((longlong)plVar7 + 0x2c) != cVar1) {
            dVar12 = dVar6 - dVar12;
          }
        }
        FUN_140451f70(&local_120,lVar3,dVar12,dVar13,ppppuVar14);
      }
    }
  }
  auVar5._8_8_ = 0;
  auVar5._0_8_ = *local_res20;
  lVar3 = SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar5,8) +
          SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar5,0);
  local_148 = (ulonglong ***)0x0;
  local_140 = (undefined1 *)((ulonglong)local_140 & 0xffffffffffffff00);
  FUN_14045d5a0(param_1,local_138,lVar3,local_res20,&local_148);
  lVar4 = *(longlong *)(local_128 + 0x58);
  if (local_138[0] == '\0') {
    FUN_14045ef00(local_130 * 0x60 + 8 + lVar4,&local_f8);
  }
  else {
    FUN_140461780(local_98);
    *(ulonglong *)(local_130 * 0x60 + lVar4) = *local_res20;
    FUN_140461780(lVar4 + 8 + local_130 * 0x60,local_98);
    FUN_1402ec870(local_98);
    bVar10 = (byte)lVar3 & 0x7f;
    *(byte *)(local_130 + *(longlong *)(local_128 + 0x50)) = bVar10;
    *(byte *)((local_130 - 0x10U & *(ulonglong *)(local_128 + 0x68)) +
              (ulonglong)((uint)*(ulonglong *)(local_128 + 0x68) & 0xf) + 1 +
             *(longlong *)(local_128 + 0x50)) = bVar10;
  }
  if ((char)local_140 != '\0') {
    _Mtx_unlock(local_148);
  }
  if (local_f8 != (void *)0x0) {
    puVar11 = (undefined8 *)(lStack_b0 + 8);
    puVar9 = puStack_d0;
    while (puVar9 < puVar11) {
      _Memory = (void *)*puVar9;
      puVar9 = puVar9 + 1;
      if (_Memory != (void *)0x0) {
        free(_Memory);
      }
    }
    if (local_f8 != (void *)0x0) {
      free(local_f8);
    }
  }
  return;
}
