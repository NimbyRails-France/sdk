
longlong * FUN_1407b57c0(longlong param_1)

{
  undefined8 *puVar1;
  undefined8 uVar2;
  longlong *plVar3;
  longlong *_Dst;
  longlong lVar4;
  undefined8 *puVar5;
  undefined8 *puVar6;
  undefined8 *puVar7;
  longlong *plVar8;
  longlong local_48 [4];
  
  puVar5 = (undefined8 *)0x0;
  _Dst = (longlong *)FUN_140983da8(0x620);
  memset(_Dst,0,0x620);
  _Dst[1] = 0;
  _Dst[2] = 0;
  _Dst[3] = 0;
  *_Dst = (longlong)nimby::model::cmd::tn::Undo::vftable;
  FUN_140319110(_Dst + 4);
  _Dst[1] = *(longlong *)(param_1 + 0x228);
  _Dst[2] = *(longlong *)(param_1 + 0x230);
  puVar1 = *(undefined8 **)(param_1 + 0x240);
  if (puVar1 < *(undefined8 **)(param_1 + 0x248)) {
    *puVar1 = _Dst;
    *(longlong *)(param_1 + 0x240) = *(longlong *)(param_1 + 0x240) + 8;
    goto LAB_1407b5925;
  }
  lVar4 = (longlong)puVar1 - *(longlong *)(param_1 + 0x238) >> 3;
  if (lVar4 == 0) {
    lVar4 = 1;
LAB_1407b5880:
    puVar5 = (undefined8 *)thunk_FUN_140983da8(lVar4 * 8);
  }
  else {
    lVar4 = lVar4 * 2;
    if (lVar4 != 0) goto LAB_1407b5880;
  }
  puVar1 = *(undefined8 **)(param_1 + 0x240);
  puVar7 = puVar5;
  for (puVar6 = *(undefined8 **)(param_1 + 0x238); puVar6 != puVar1; puVar6 = puVar6 + 1) {
    uVar2 = *puVar6;
    *puVar6 = 0;
    *puVar7 = uVar2;
    puVar7 = puVar7 + 1;
  }
  *puVar7 = _Dst;
  plVar3 = *(longlong **)(param_1 + 0x240);
  for (plVar8 = *(longlong **)(param_1 + 0x238); plVar8 != plVar3; plVar8 = plVar8 + 1) {
    puVar1 = (undefined8 *)*plVar8;
    if (puVar1 != (undefined8 *)0x0) {
      (**(code **)*puVar1)(puVar1,1);
    }
  }
  if (*(void **)(param_1 + 0x238) != (void *)0x0) {
    free(*(void **)(param_1 + 0x238));
  }
  *(undefined8 **)(param_1 + 0x238) = puVar5;
  *(undefined8 **)(param_1 + 0x240) = puVar7 + 1;
  *(undefined8 **)(param_1 + 0x248) = puVar5 + lVar4;
LAB_1407b5925:
  lVar4 = *(longlong *)(param_1 + 0x250);
  (**(code **)(*_Dst + 0x28))(_Dst);
  FUN_14074c5a0(lVar4 + 0x4e8,local_48);
  *(longlong *)(local_48[0] + 8) = *(longlong *)(local_48[0] + 8) + 1;
  _Dst[3] = *(longlong *)(local_48[0] + 8);
  return _Dst;
}

