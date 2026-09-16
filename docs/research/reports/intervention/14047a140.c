
void FUN_14047a140(longlong param_1,undefined8 param_2,undefined8 param_3)

{
  undefined8 uVar1;
  undefined4 uVar2;
  longlong *plVar3;
  undefined8 *puVar4;
  longlong *plVar5;
  longlong *plVar6;
  undefined8 *puVar7;
  longlong local_48 [4];

  plVar3 = *(longlong **)(param_1 + 0x338);
  puVar7 = (undefined8 *)*plVar3;
  plVar6 = plVar3;
  if (puVar7 == (undefined8 *)0x0) {
    puVar7 = (undefined8 *)plVar3[1];
    plVar5 = plVar3;
    while (plVar6 = plVar5 + 1, puVar7 == (undefined8 *)0x0) {
      puVar7 = (undefined8 *)plVar5[2];
      plVar5 = plVar6;
    }
  }
  puVar4 = (undefined8 *)plVar3[*(longlong *)(param_1 + 0x340)];
  while (puVar7 != puVar4) {
    FUN_140461980(param_1 + 0x28,local_48,param_3,puVar7,*puVar7);
    FUN_140450420(local_48[0] + 8,puVar7 + 1);
    puVar7 = (undefined8 *)puVar7[200];
    while (puVar7 == (undefined8 *)0x0) {
      plVar3 = plVar6 + 1;
      plVar6 = plVar6 + 1;
      puVar7 = (undefined8 *)*plVar3;
    }
  }
  plVar3 = *(longlong **)(param_1 + 0x368);
  puVar7 = (undefined8 *)*plVar3;
  plVar6 = plVar3;
  if (puVar7 == (undefined8 *)0x0) {
    puVar7 = (undefined8 *)plVar3[1];
    plVar5 = plVar3;
    while (plVar6 = plVar5 + 1, puVar7 == (undefined8 *)0x0) {
      puVar7 = (undefined8 *)plVar5[2];
      plVar5 = plVar6;
    }
  }
  puVar4 = (undefined8 *)plVar3[*(longlong *)(param_1 + 0x370)];
  while (puVar7 != puVar4) {
    uVar1 = puVar7[1];
    uVar2 = *(undefined4 *)(puVar7 + 2);
    FUN_140484b50(param_1 + 0x58,local_48,param_3,puVar7,*puVar7);
    *(undefined8 *)(local_48[0] + 8) = uVar1;
    *(undefined4 *)(local_48[0] + 0x10) = uVar2;
    puVar7 = (undefined8 *)puVar7[3];
    while (puVar7 == (undefined8 *)0x0) {
      plVar3 = plVar6 + 1;
      plVar6 = plVar6 + 1;
      puVar7 = (undefined8 *)*plVar3;
    }
  }
  plVar3 = *(longlong **)(param_1 + 0x398);
  puVar7 = (undefined8 *)*plVar3;
  plVar6 = plVar3;
  if (puVar7 == (undefined8 *)0x0) {
    puVar7 = (undefined8 *)plVar3[1];
    plVar5 = plVar3;
    while (plVar6 = plVar5 + 1, puVar7 == (undefined8 *)0x0) {
      puVar7 = (undefined8 *)plVar5[2];
      plVar5 = plVar6;
    }
  }
  puVar4 = (undefined8 *)plVar3[*(longlong *)(param_1 + 0x3a0)];
  while (puVar7 != puVar4) {
    uVar1 = puVar7[1];
    FUN_140484430(param_1 + 0x88,local_48,param_3,puVar7,*puVar7);
    *(undefined8 *)(local_48[0] + 8) = uVar1;
    puVar7 = (undefined8 *)puVar7[2];
    while (puVar7 == (undefined8 *)0x0) {
      plVar3 = plVar6 + 1;
      plVar6 = plVar6 + 1;
      puVar7 = (undefined8 *)*plVar3;
    }
  }
  FUN_140350830(param_1 + 0x330);
  FUN_1403508d0(param_1 + 0x360);
  FUN_140325a40(param_1 + 0x390);
  return;
}


// ref 140c25118 caller none
// ref 14047a81d caller 14047a5f0
// ref 14047c7fd caller 14047a5f0
// ref 14047c90f caller 14047a5f0
