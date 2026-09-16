
ulonglong * FUN_140470c30(longlong param_1,ulonglong *param_2)

{
  ulonglong *puVar1;

  for (puVar1 = *(ulonglong **)
                 (*(longlong *)(param_1 + 8) + (*param_2 % (ulonglong)*(uint *)(param_1 + 0x10)) * 8
                 ); puVar1 != (ulonglong *)0x0; puVar1 = (ulonglong *)puVar1[0xb8]) {
    if (*param_2 == *puVar1) goto LAB_140470c69;
  }
  puVar1 = *(ulonglong **)(*(longlong *)(param_1 + 8) + *(longlong *)(param_1 + 0x10) * 8);
LAB_140470c69:
  return puVar1 + 1;
}
