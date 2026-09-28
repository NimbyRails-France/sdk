
longlong * FUN_140320c00(longlong param_1)

{
  longlong *plVar1;
  longlong *plVar2;
  longlong *plVar3;
  longlong *plVar4;
  undefined8 uVar5;
  longlong *plVar6;
  longlong *plVar7;
  longlong *plVar8;
  
  plVar2 = (longlong *)FUN_14032c420();
  if (plVar2 == (longlong *)0x0) {
    return (longlong *)0x0;
  }
  plVar1 = *(longlong **)(param_1 + 0x70);
  plVar7 = *(longlong **)(param_1 + 0x68);
  if (plVar7 != plVar1) {
    plVar8 = plVar7 + 0xc;
    do {
      plVar3 = (longlong *)plVar8[2];
      if (plVar3 == (longlong *)0x0) {
LAB_140320c73:
        plVar3 = (longlong *)plVar8[-10];
        if (plVar3 != (longlong *)0x0) {
          plVar6 = plVar7;
          do {
            plVar4 = plVar3;
            if (*plVar2 <= plVar3[4]) {
              plVar4 = plVar3 + 1;
              plVar6 = plVar3;
            }
            plVar3 = (longlong *)*plVar4;
          } while (plVar3 != (longlong *)0x0);
          if ((plVar6 != plVar7) && (plVar6[4] <= *plVar2)) goto LAB_140320cb8;
        }
        uVar5 = FUN_1403331b0(plVar7,plVar2);
        FUN_14032cab0(uVar5,plVar2);
      }
      else {
        plVar6 = plVar8;
        do {
          plVar4 = plVar3;
          if (*plVar2 <= plVar3[4]) {
            plVar4 = plVar3 + 1;
            plVar6 = plVar3;
          }
          plVar3 = (longlong *)*plVar4;
        } while (plVar3 != (longlong *)0x0);
        if ((plVar6 == plVar8) || (*plVar2 < plVar6[4])) goto LAB_140320c73;
      }
LAB_140320cb8:
      plVar7 = plVar7 + 0x18;
      plVar8 = plVar8 + 0x18;
    } while (plVar7 != plVar1);
  }
  return plVar2;
}

