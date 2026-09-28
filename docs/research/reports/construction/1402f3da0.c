
void FUN_1402f3da0(longlong param_1)

{
  undefined8 *puVar1;
  undefined8 *puVar2;
  
  if (*(char *)(param_1 + 0x1e8) != '\0') {
    FUN_1402f3e80();
  }
  puVar2 = *(undefined8 **)(param_1 + 0xf0);
  while (puVar2 != (undefined8 *)0x0) {
    FUN_1402450d0(param_1 + 0xe0,*puVar2);
    puVar1 = (undefined8 *)puVar2[1];
    free(puVar2);
    puVar2 = puVar1;
  }
  puVar2 = *(undefined8 **)(param_1 + 0xc0);
  while (puVar2 != (undefined8 *)0x0) {
    FUN_1402450d0(param_1 + 0xb0,*puVar2);
    puVar1 = (undefined8 *)puVar2[1];
    free(puVar2);
    puVar2 = puVar1;
  }
  puVar2 = *(undefined8 **)(param_1 + 0x90);
  while (puVar2 != (undefined8 *)0x0) {
    FUN_1402450d0(param_1 + 0x80,*puVar2);
    puVar1 = (undefined8 *)puVar2[1];
    free(puVar2);
    puVar2 = puVar1;
  }
  return;
}

