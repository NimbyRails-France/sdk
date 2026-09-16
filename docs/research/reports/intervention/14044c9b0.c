
undefined8 FUN_14044c9b0(undefined8 param_1,undefined8 param_2,double *param_3,longlong *param_4)

{
  longlong lVar1;
  undefined8 *puVar2;
  undefined8 *_Memory;
  double dVar3;
  longlong *plVar4;
  longlong *plVar5;
  longlong *plVar6;
  undefined4 uVar7;

  dVar3 = DAT_140aac130;
  plVar6 = (longlong *)param_4[1];
  if (plVar6 != param_4) {
    uVar7 = (undefined4)DAT_140aac700;
    do {
      lVar1 = plVar6[4];
      FUN_14044c810(param_1,param_2,param_3,lVar1);
      *param_3 = *param_3 - dVar3;
      FUN_140380bb0(param_3 + 0x47c,0,0xd,uVar7);
      FUN_140380bb0(param_3 + 0x47c,lVar1,0xd,uVar7);
      plVar4 = (longlong *)*plVar6;
      if (plVar4 == (longlong *)0x0) {
        plVar4 = (longlong *)plVar6[2];
        plVar5 = (longlong *)0x0;
        if (plVar6 == (longlong *)*plVar4) {
          do {
            plVar6 = plVar4;
            plVar4 = (longlong *)plVar6[2];
          } while (plVar6 == (longlong *)*plVar4);
          plVar5 = (longlong *)*plVar6;
        }
        if (plVar5 != plVar4) {
          plVar6 = plVar4;
        }
      }
      else {
        for (plVar5 = (longlong *)plVar4[1]; plVar6 = plVar4, plVar5 != (longlong *)0x0;
            plVar5 = (longlong *)plVar5[1]) {
          plVar4 = plVar5;
        }
      }
    } while (plVar6 != param_4);
  }
  _Memory = (undefined8 *)param_4[2];
  while (_Memory != (undefined8 *)0x0) {
    FUN_1402450d0(param_4,*_Memory);
    puVar2 = (undefined8 *)_Memory[1];
    free(_Memory);
    _Memory = puVar2;
  }
  return 0;
}


// ref 140c23744 caller none
// ref 1403036d7 caller 140303620
// ref 14030372f caller 140303620
