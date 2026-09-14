
undefined8 * FUN_140486ab0(undefined8 *param_1,undefined8 *param_2,undefined8 *param_3)

{
  undefined8 *puVar1;
  undefined8 uVar2;
  undefined8 *puVar3;
  longlong lVar4;
  undefined1 local_res8;
  
  if (param_1 != param_2) {
    lVar4 = (longlong)param_3 - (longlong)param_1;
    puVar3 = param_1 + 4;
    do {
      *param_3 = puVar3[-4];
      param_3 = param_3 + 9;
      *(undefined8 *)((longlong)puVar3 + lVar4 + -0x18) = puVar3[-3];
      uVar2 = puVar3[-2];
      puVar3[-2] = &DAT_140a6d430;
      *(undefined8 *)((longlong)puVar3 + lVar4 + -0x10) = uVar2;
      uVar2 = puVar3[-1];
      puVar3[-1] = 0;
      *(undefined8 *)((longlong)puVar3 + lVar4 + -8) = uVar2;
      uVar2 = *puVar3;
      *puVar3 = 0;
      *(undefined8 *)((longlong)puVar3 + lVar4) = uVar2;
      uVar2 = puVar3[1];
      puVar3[1] = 0;
      *(undefined8 *)((longlong)puVar3 + lVar4 + 8) = uVar2;
      *(undefined1 *)((longlong)puVar3 + lVar4 + 0x10) = local_res8;
      *(undefined1 *)((longlong)puVar3 + lVar4 + 0x18) = *(undefined1 *)(puVar3 + 3);
      *(undefined1 *)((longlong)puVar3 + lVar4 + 0x19) = *(undefined1 *)((longlong)puVar3 + 0x19);
      *(undefined1 *)((longlong)puVar3 + lVar4 + 0x1a) = *(undefined1 *)((longlong)puVar3 + 0x1a);
      *(undefined8 *)((longlong)puVar3 + lVar4 + 0x20) = puVar3[4];
      puVar1 = puVar3 + 5;
      puVar3[4] = 0;
      puVar3 = puVar3 + 9;
    } while (puVar1 != param_2);
  }
  return param_3;
}

