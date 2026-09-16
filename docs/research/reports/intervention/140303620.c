
longlong FUN_140303620(longlong param_1,longlong param_2,undefined8 param_3,longlong param_4)

{
  int iVar1;
  undefined8 *puVar2;
  undefined8 *_Memory;
  undefined8 uVar3;
  longlong lVar4;
  undefined8 uVar5;
  undefined1 local_b0 [48];
  undefined1 local_80 [48];
  undefined1 local_50 [56];

  iVar1 = *(int *)(param_1 + 0x20);
  if (iVar1 == 0) {
    uVar3 = FUN_14031ff10(local_80,param_1 + 0x28);
  }
  else {
    if (iVar1 == 1) {
      lVar4 = FUN_14031ff10(local_b0,param_1 + 0x28);
      uVar3 = FUN_14031ff10(local_50,lVar4);
      uVar5 = FUN_14044cb10(param_4 + 0xa0,local_80);
      FUN_14044c9b0(param_4 + 0xa0,param_3,param_4,uVar5,uVar3);
      _Memory = *(undefined8 **)(lVar4 + 0x10);
      while (_Memory != (undefined8 *)0x0) {
        FUN_1402450d0(lVar4,*_Memory);
        puVar2 = (undefined8 *)_Memory[1];
        free(_Memory);
        _Memory = puVar2;
      }
      goto LAB_140303734;
    }
    if (iVar1 == 2) {
      uVar3 = 1;
    }
    else {
      if (iVar1 != 3) goto LAB_140303734;
      uVar3 = 0;
    }
    uVar3 = FUN_14044cc70(param_4 + 0xa0,local_b0,param_3,param_4,uVar3);
  }
  FUN_14044c9b0(param_4 + 0xa0,param_3,param_4,uVar3);
LAB_140303734:
  *(longlong *)(param_4 + 0x2120) = *(longlong *)(param_4 + 0x2120) + 1;
  *(undefined1 *)(param_2 + 0x1f0) = 0;
  return param_2;
}


// ref 140c17294 caller none
// ref 140a6d648 caller none
