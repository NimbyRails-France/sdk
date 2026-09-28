
undefined8 * FUN_1402fda10(undefined8 *param_1)

{
  undefined8 uVar1;
  undefined8 *puVar2;
  undefined8 *local_res8 [4];
  
  local_res8[0] = param_1;
  puVar2 = (undefined8 *)FUN_14031bea0(local_res8);
  uVar1 = *puVar2;
  *puVar2 = 0;
  *param_1 = uVar1;
  if (local_res8[0] != (undefined8 *)0x0) {
    (**(code **)*local_res8[0])(local_res8[0],1);
  }
  return param_1;
}

