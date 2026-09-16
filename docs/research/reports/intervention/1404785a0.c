
void FUN_1404785a0(longlong param_1,longlong param_2,int param_3,undefined8 param_4)

{
  void **ppvVar1;
  void *pvVar2;
  void *pvVar3;
  uint uVar4;
  undefined8 uVar5;
  void *pvVar6;
  int iVar7;
  void *local_a8;
  void *local_a0;
  undefined8 local_98;
  void *local_90;
  void *local_88;
  undefined8 local_80;
  void *local_78;
  void *local_70;
  undefined8 local_68;
  void *local_60;
  void *local_58;

  *(undefined8 *)(param_2 + 8) = *(undefined8 *)(param_1 + 0x2128);
  FUN_1404595b0(param_1 + 0xa0,&local_a8,(1 << ((byte)param_3 & 0x1f)) - 1U & (uint)param_4);
  ppvVar1 = (void **)(param_2 + 0x10);
  pvVar6 = local_a8;
  pvVar2 = local_a0;
  if (ppvVar1 != &local_a8) {
    FUN_140353640(ppvVar1);
    pvVar6 = *ppvVar1;
    *ppvVar1 = local_a8;
    pvVar2 = *(void **)(param_2 + 0x18);
    uVar5 = *(undefined8 *)(param_2 + 0x20);
    *(void **)(param_2 + 0x18) = local_a0;
    *(undefined8 *)(param_2 + 0x20) = local_98;
    local_a8 = pvVar6;
    local_98 = uVar5;
    local_a0 = pvVar2;
  }
  for (; pvVar3 = local_a0, pvVar6 != local_a0; pvVar6 = (void *)((longlong)pvVar6 + 0x638)) {
    local_a0 = pvVar2;
    FUN_140351080(pvVar6);
    pvVar2 = local_a0;
    local_a0 = pvVar3;
  }
  local_a0 = pvVar2;
  if (local_a8 != (void *)0x0) {
    free(local_a8);
  }
  iVar7 = 1 << ((byte)param_3 + 2 & 0x1f);
  uVar4 = iVar7 - 1U & (uint)param_4;
  FUN_1404592e0(param_1 + 0xd0,&local_90,uVar4,iVar7);
  ppvVar1 = (void **)(param_2 + 0x28);
  pvVar6 = local_90;
  pvVar2 = local_88;
  if (ppvVar1 != &local_90) {
    FUN_1403536a0(ppvVar1);
    pvVar6 = *ppvVar1;
    *ppvVar1 = local_90;
    pvVar2 = *(void **)(param_2 + 0x30);
    uVar5 = *(undefined8 *)(param_2 + 0x38);
    *(void **)(param_2 + 0x30) = local_88;
    *(undefined8 *)(param_2 + 0x38) = local_80;
    local_90 = pvVar6;
    local_80 = uVar5;
    local_88 = pvVar2;
  }
  for (; pvVar3 = local_88, pvVar6 != local_88; pvVar6 = (void *)((longlong)pvVar6 + 0x40)) {
    local_88 = pvVar2;
    FUN_14034b770((longlong)pvVar6 + 8);
    pvVar2 = local_88;
    local_88 = pvVar3;
  }
  local_88 = pvVar2;
  if (local_90 != (void *)0x0) {
    free(local_90);
  }
  FUN_1404861f0(param_1 + 0x40,&local_78,uVar4,iVar7);
  ppvVar1 = (void **)(param_2 + 0x40);
  pvVar6 = local_78;
  pvVar2 = local_70;
  if (ppvVar1 != &local_78) {
    FUN_140353700(ppvVar1);
    pvVar6 = *ppvVar1;
    *ppvVar1 = local_78;
    pvVar2 = *(void **)(param_2 + 0x48);
    uVar5 = *(undefined8 *)(param_2 + 0x50);
    *(void **)(param_2 + 0x48) = local_70;
    *(undefined8 *)(param_2 + 0x50) = local_68;
    local_78 = pvVar6;
    local_68 = uVar5;
    local_70 = pvVar2;
  }
  for (; pvVar3 = local_70, pvVar6 != local_70; pvVar6 = (void *)((longlong)pvVar6 + 0x178)) {
    local_70 = pvVar2;
    FUN_140351180(pvVar6);
    pvVar2 = local_70;
    local_70 = pvVar3;
  }
  local_70 = pvVar2;
  if (local_78 != (void *)0x0) {
    free(local_78);
  }
  uVar5 = FUN_140485e50(param_1 + 0x40,&local_60,param_4,param_3 + 2);
  FUN_140350460(param_2 + 0x58,uVar5);
  for (pvVar6 = local_60; pvVar6 != local_58; pvVar6 = (void *)((longlong)pvVar6 + 0x48)) {
    FUN_140353b70((longlong)pvVar6 + 0x10);
  }
  if (local_60 != (void *)0x0) {
    free(local_60);
  }
  return;
}


// ref 140c25094 caller none
// ref 1404f3ccb caller 1404f3520
