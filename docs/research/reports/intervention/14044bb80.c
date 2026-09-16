
void FUN_14044bb80(undefined8 *param_1,longlong param_2)

{
  int iVar1;
  longlong lVar2;

  FUN_140339970(param_1 + 1,param_2 + 0xc0);
  param_1[0xf] = *(undefined8 *)(param_2 + 0x128);
  iVar1 = 0;
  *(undefined4 *)(param_1 + 0x10) = 0;
  param_1[0x12] = param_1[0x11];
  lVar2 = param_1[1];
  if (0 < (int)(param_1[2] - lVar2 >> 5)) {
    do {
      lVar2 = lVar2 + (longlong)iVar1 * 0x20;
      *(undefined8 *)(lVar2 + 8) = *param_1;
      *(short *)(lVar2 + 0x10) = (short)iVar1;
      iVar1 = iVar1 + 1;
      lVar2 = param_1[1];
    } while (iVar1 < (int)(param_1[2] - lVar2 >> 5));
  }
  return;
}


// ref 140c236b4 caller none
// ref 14044c8cd caller 14044c810
// ref 14044c98f caller 14044c810
// ref 14047d50a caller 14047d410
// ref 140479880 caller 140479530
