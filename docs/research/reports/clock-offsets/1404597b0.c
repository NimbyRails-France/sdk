
undefined8 * FUN_1404597b0(undefined8 param_1,undefined8 param_2)

{
  short sVar1;
  undefined8 *puVar2;
  undefined8 uVar3;

  uVar3 = param_2;
  puVar2 = (undefined8 *)FUN_14045f300();
  if (puVar2 == (undefined8 *)0x0) {
    sVar1 = (short)((ulonglong)uVar3 >> 0x30);
    if ((sVar1 != -1) && (sVar1 == 5)) {
      puVar2 = (undefined8 *)FUN_14045f230(param_1);
      sVar1 = (short)((ulonglong)*puVar2 >> 0x30);
      if ((sVar1 == -1) || (sVar1 == 0)) {
LAB_14045981a:
        memset(puVar2 + 1,0,0x630);
        FUN_140459860(puVar2);
        *puVar2 = param_2;
        return puVar2;
      }
      if ((ushort)*puVar2 < (ushort)param_2) {
        FUN_140351080(puVar2);
        goto LAB_14045981a;
      }
    }
    puVar2 = (undefined8 *)0x0;
  }
  return puVar2;
}
