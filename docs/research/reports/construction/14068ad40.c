
void FUN_14068ad40(longlong param_1)

{
  undefined8 *puVar1;
  
  puVar1 = *(undefined8 **)(param_1 + 0x7f0);
  if (puVar1 != (undefined8 *)0x0) {
    (**(code **)*puVar1)(puVar1,1);
  }
  FUN_140582130(param_1 + 0x6c0);
  FUN_140002d30(param_1 + 0x6a0);
  FUN_140322bc0(param_1 + 0x5e8);
  FUN_140002d30(param_1 + 0x5a8);
  if (*(longlong *)(param_1 + 0x560) != 0) {
    FUN_1405bff80();
  }
  FUN_140582130(param_1 + 0x430);
  FUN_14068ae30(param_1 + 0x228);
  FUN_14068add0(param_1 + 8);
  return;
}

