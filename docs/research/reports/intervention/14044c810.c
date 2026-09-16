
void FUN_14044c810(longlong param_1,longlong param_2,undefined8 param_3,longlong param_4)

{
  longlong *plVar1;
  longlong lVar2;
  longlong lVar3;
  longlong lVar4;
  longlong lVar5;
  longlong *plVar6;

  if (param_4 != 0) {
    lVar2 = FUN_14045f300(param_1,param_4);
    lVar3 = FUN_1404594d0(param_1 + 0x30);
    if ((lVar2 != 0) && (lVar3 != 0)) {
      plVar1 = *(longlong **)(lVar2 + 0x90);
      for (plVar6 = *(longlong **)(lVar2 + 0x88); plVar6 != plVar1; plVar6 = plVar6 + 4) {
        lVar5 = *plVar6;
        if ((lVar5 != 0) && (lVar4 = FUN_14045f300(param_1,lVar5), lVar4 != 0)) {
          FUN_14043e300(lVar4);
          lVar5 = FUN_14033f780(param_2 + 0x200,lVar5);
          if (lVar5 != 0) {
            FUN_14044bb80(lVar4,lVar5);
          }
        }
      }
      lVar5 = FUN_14045f300(param_1,param_4);
      if (((lVar5 != 0) && (*(char *)(lVar5 + 0x5d0) != '\0')) &&
         (*(longlong *)(lVar5 + 0x5a8) != 0)) {
        FUN_14033f7f0(*(longlong *)(param_2 + 0x780) + 0x180);
      }
      FUN_140465d20();
      FUN_140418b70(lVar3 + 8);
      *(undefined8 *)(lVar3 + 0x38) = 0;
      FUN_14043e300(lVar2);
      lVar3 = FUN_14033f780(param_2 + 0x200,param_4);
      if (lVar3 != 0) {
        FUN_14044bb80(lVar2,lVar3);
      }
    }
  }
  return;
}


// ref 140c236f0 caller none
// ref 140b081a4 caller none
// ref 140b081fc caller none
// ref 14044ca10 caller 14044c9b0
// ref 140440e12 caller 140440e00
