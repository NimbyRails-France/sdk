
void FUN_1407832e0(longlong param_1,longlong param_2)

{
  undefined1 uVar1;
  longlong lVar2;
  undefined8 uVar3;
  void *pvVar4;
  undefined1 local_d8 [16];
  undefined1 local_c8 [104];
  void *local_60;
  void *local_48;
  void *local_40;
  
  FUN_1407831b0();
  if (*(longlong *)(param_1 + 0x18) != 0) {
    lVar2 = FUN_1407bc7e0(*(undefined8 *)(param_2 + 0x28));
    uVar1 = *(undefined1 *)(*(longlong *)(param_2 + 8) + 0xf9);
    uVar3 = FUN_1407830a0(param_1,local_d8,*(undefined8 *)(param_2 + 0x18),uVar1,
                          *(undefined8 *)(param_2 + 0x48),*(undefined8 *)(param_2 + 0x40),uVar1);
    FUN_1404d70c0(lVar2 + 0x20,uVar3);
    for (pvVar4 = local_48; pvVar4 != local_40; pvVar4 = (void *)((longlong)pvVar4 + 0x50)) {
      FUN_14030dbd0(pvVar4);
    }
    if (local_48 != (void *)0x0) {
      free(local_48);
    }
    if (local_60 != (void *)0x0) {
      free(local_60);
    }
    FUN_140002d30(local_c8);
    FUN_1402531c0(*(longlong *)(param_2 + 0x30) + 0x60);
  }
  return;
}

