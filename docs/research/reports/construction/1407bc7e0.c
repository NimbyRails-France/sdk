
longlong * FUN_1407bc7e0(longlong param_1)

{
  undefined8 *puVar1;
  undefined8 uVar2;
  longlong *plVar3;
  longlong *plVar4;
  longlong lVar5;
  undefined8 *puVar6;
  undefined8 *puVar7;
  undefined8 *puVar8;
  longlong *plVar9;
  undefined4 local_res8;
  undefined4 uStackX_c;
  longlong local_48 [4];
  
  FUN_14031bea0(&local_res8);
  plVar4 = (longlong *)CONCAT44(uStackX_c,local_res8);
  plVar4[1] = *(longlong *)(param_1 + 0x228);
  plVar4[2] = *(longlong *)(param_1 + 0x230);
  puVar6 = *(undefined8 **)(param_1 + 0x240);
  if (puVar6 < *(undefined8 **)(param_1 + 0x248)) {
    *puVar6 = plVar4;
    *(longlong *)(param_1 + 0x240) = *(longlong *)(param_1 + 0x240) + 8;
    goto LAB_1407bc905;
  }
  lVar5 = (longlong)puVar6 - *(longlong *)(param_1 + 0x238) >> 3;
  puVar6 = (undefined8 *)0x0;
  if (lVar5 == 0) {
    lVar5 = 1;
LAB_1407bc86a:
    puVar6 = (undefined8 *)thunk_FUN_140983da8(lVar5 * 8);
  }
  else {
    lVar5 = lVar5 * 2;
    if (lVar5 != 0) goto LAB_1407bc86a;
  }
  puVar1 = *(undefined8 **)(param_1 + 0x240);
  puVar8 = puVar6;
  for (puVar7 = *(undefined8 **)(param_1 + 0x238); puVar7 != puVar1; puVar7 = puVar7 + 1) {
    uVar2 = *puVar7;
    *puVar7 = 0;
    *puVar8 = uVar2;
    puVar8 = puVar8 + 1;
  }
  *puVar8 = plVar4;
  plVar3 = *(longlong **)(param_1 + 0x240);
  for (plVar9 = *(longlong **)(param_1 + 0x238); plVar9 != plVar3; plVar9 = plVar9 + 1) {
    puVar1 = (undefined8 *)*plVar9;
    if (puVar1 != (undefined8 *)0x0) {
      (**(code **)*puVar1)(puVar1,1);
    }
  }
  if (*(void **)(param_1 + 0x238) != (void *)0x0) {
    free(*(void **)(param_1 + 0x238));
  }
  *(undefined8 **)(param_1 + 0x238) = puVar6;
  *(undefined8 **)(param_1 + 0x240) = puVar8 + 1;
  *(undefined8 **)(param_1 + 0x248) = puVar6 + lVar5;
LAB_1407bc905:
  lVar5 = *(longlong *)(param_1 + 0x250);
  local_res8 = (**(code **)(*plVar4 + 0x28))(plVar4);
  FUN_14074c5a0(lVar5 + 0x4e8,local_48);
  *(longlong *)(local_48[0] + 8) = *(longlong *)(local_48[0] + 8) + 1;
  plVar4[3] = *(longlong *)(local_48[0] + 8);
  return plVar4;
}

