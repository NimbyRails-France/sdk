
undefined8 * FUN_140303590(undefined8 *param_1)

{
  undefined8 *puVar1;
  undefined8 *puVar2;

  puVar2 = (undefined8 *)FUN_140983da8(0x58);
  *puVar2 = 0;
  puVar2[1] = 0;
  puVar2[2] = 0;
  puVar2[3] = 0;
  puVar2[4] = 0;
  puVar2[5] = 0;
  puVar2[6] = 0;
  puVar2[7] = 0;
  puVar2[8] = 0;
  puVar2[9] = 0;
  puVar2[10] = 0;
  *puVar2 = nimby::model::cmd::train::Intervention::vftable;
  puVar1 = puVar2 + 5;
  puVar2[1] = 0;
  puVar2[2] = 0;
  puVar2[3] = 0;
  *(undefined4 *)((longlong)puVar2 + 0x41) = 0;
  *(undefined2 *)((longlong)puVar2 + 0x45) = 0;
  *(undefined1 *)((longlong)puVar2 + 0x47) = 0;
  *puVar1 = puVar1;
  puVar2[6] = puVar1;
  puVar2[7] = 0;
  *(undefined1 *)(puVar2 + 8) = 0;
  puVar2[9] = 0;
  *param_1 = puVar2;
  return param_1;
}


// ref 140c17288 caller none
// ref 14030ea6c caller 14030e2d0
// ref 14030ea73 caller 14030e2d0
