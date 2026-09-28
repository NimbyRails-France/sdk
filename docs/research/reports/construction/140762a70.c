
void FUN_140762a70(longlong *param_1,undefined8 param_2)

{
  undefined8 *puVar1;
  ulonglong uVar2;
  longlong *plVar3;
  
  puVar1 = (undefined8 *)thunk_FUN_140983da8(0x610);
  FUN_140338ee0(puVar1 + 2,param_2);
  *puVar1 = param_1;
  puVar1[1] = param_1[1];
  *(undefined8 **)param_1[1] = puVar1;
  param_1[1] = (longlong)puVar1;
  uVar2 = param_1[2] + 1;
  param_1[2] = uVar2;
  do {
    if (uVar2 < 0x65) {
      if (uVar2 < 0xb) {
LAB_140762b4e:
        FUN_140320ec0(param_2);
        return;
      }
      uVar2 = 0;
      plVar3 = (longlong *)*param_1;
      if (plVar3 == param_1) goto LAB_140762b4e;
      do {
        uVar2 = uVar2 + plVar3[0x3c] * 0xe8 + plVar3[0xb4] * 200 + plVar3[0xc] * 0x4e8;
        plVar3 = (longlong *)*plVar3;
      } while (plVar3 != param_1);
      if (uVar2 < 0x2faf081) goto LAB_140762b4e;
    }
    plVar3 = (longlong *)*param_1;
    *(longlong *)(*plVar3 + 8) = plVar3[1];
    *(longlong *)plVar3[1] = *plVar3;
    FUN_140320ec0(plVar3 + 2);
    free(plVar3);
    param_1[2] = param_1[2] + -1;
    uVar2 = param_1[2];
  } while( true );
}

