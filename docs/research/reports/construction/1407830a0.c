
undefined8 *
FUN_1407830a0(longlong param_1,undefined8 *param_2,undefined8 *param_3,undefined8 param_4,
             longlong param_5,longlong param_6,char param_7)

{
  undefined8 uVar1;
  undefined1 uVar2;
  undefined8 *puVar3;
  char cVar4;
  char cVar5;
  undefined4 local_28;
  undefined4 uStack_24;
  undefined6 uStack_16;
  
  FUN_1402f7860(param_2,param_6 + 0x148);
  *param_2 = 0;
  *(undefined1 *)(param_2 + 1) = 1;
  if (*(longlong *)(param_1 + 0x18) != 0) {
    puVar3 = (undefined8 *)FUN_14032c420(*param_3);
    if (puVar3 != (undefined8 *)0x0) {
      cVar4 = (char)*(undefined4 *)(param_1 + 0x28);
      if ((*(int *)(param_6 + 0x178) == 0) || (*(int *)(param_6 + 0x178) == 6)) {
        cVar4 = -cVar4;
      }
      uVar1 = *(undefined8 *)(param_1 + 0x20);
      local_28 = (undefined4)*puVar3;
      uStack_24 = (undefined4)((ulonglong)*puVar3 >> 0x20);
      cVar5 = -cVar4;
      if (*(char *)(param_5 + 5) == '\0') {
        cVar5 = cVar4;
      }
      cVar4 = -cVar5;
      if (param_7 == '\0') {
        cVar4 = cVar5;
      }
      uVar2 = *(undefined1 *)((longlong)puVar3 + 0x2c);
      *(undefined4 *)(param_2 + 8) = local_28;
      *(undefined4 *)((longlong)param_2 + 0x44) = uStack_24;
      *(int *)(param_2 + 9) = (int)uVar1;
      *(int *)((longlong)param_2 + 0x4c) = (int)((ulonglong)uVar1 >> 0x20);
      param_2[10] = CONCAT62(uStack_16,CONCAT11(uVar2,cVar4));
      *(undefined4 *)(param_2 + 0xb) = 0;
      if ((*(int *)(param_6 + 0x178) == 4) && (*(char *)(param_5 + 10) != '\0')) {
        *(undefined4 *)(param_2 + 0xb) = *(undefined4 *)(param_1 + 0x28);
      }
    }
  }
  return param_2;
}

