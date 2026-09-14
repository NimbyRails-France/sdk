
undefined8 *
FUN_140486b80(longlong param_1,undefined8 *param_2,undefined8 param_3,longlong *param_4,
             ulonglong param_5)

{
  longlong *plVar1;
  void *_Memory;
  ulonglong *puVar2;
  ulonglong uVar3;
  longlong *plVar4;
  longlong lVar5;
  ulonglong uVar6;
  ulonglong uVar7;
  ulonglong uVar8;
  char local_res8 [4];
  uint local_resc;
  
  uVar6 = param_5 % (ulonglong)*(uint *)(param_1 + 0x10);
  plVar4 = *(longlong **)(*(longlong *)(param_1 + 8) + uVar6 * 8);
  if (plVar4 != (longlong *)0x0) {
    do {
      if (*param_4 == *plVar4) {
        if (plVar4 != (longlong *)0x0) {
          lVar5 = *(longlong *)(param_1 + 8);
          *param_2 = plVar4;
          *(undefined1 *)(param_2 + 2) = 0;
          param_2[1] = lVar5 + uVar6 * 8;
          return param_2;
        }
        break;
      }
      plVar4 = (longlong *)plVar4[10];
    } while (plVar4 != (longlong *)0x0);
  }
  FUN_14001db20(param_1 + 0x20,local_res8,(ulonglong)*(uint *)(param_1 + 0x10),
                *(undefined4 *)(param_1 + 0x18),1);
  plVar4 = (longlong *)thunk_FUN_140983da8(0x58);
  uVar7 = 0;
  *plVar4 = *param_4;
  plVar4[7] = 0;
  plVar4[8] = 0;
  plVar4[1] = 0;
  plVar4[2] = 0;
  plVar4[3] = (longlong)&DAT_140a6d430;
  plVar4[4] = 0;
  plVar4[5] = 0;
  plVar4[6] = 0;
  plVar4[9] = 0;
  plVar4[10] = 0;
  if (local_res8[0] != '\0') {
    uVar8 = (ulonglong)local_resc;
    uVar6 = param_5 % uVar8;
    lVar5 = FUN_140254df0();
    if (*(longlong *)(param_1 + 0x10) != 0) {
      do {
        _Memory = *(void **)(param_1 + 8);
        puVar2 = *(ulonglong **)((longlong)_Memory + uVar7 * 8);
        while (puVar2 != (ulonglong *)0x0) {
          uVar3 = *puVar2;
          *(ulonglong *)((longlong)_Memory + uVar7 * 8) = puVar2[10];
          puVar2[10] = *(ulonglong *)(lVar5 + (uVar3 % uVar8) * 8);
          *(ulonglong **)(lVar5 + (uVar3 % uVar8) * 8) = puVar2;
          _Memory = *(void **)(param_1 + 8);
          puVar2 = *(ulonglong **)((longlong)_Memory + uVar7 * 8);
        }
        uVar7 = uVar7 + 1;
      } while (uVar7 < *(ulonglong *)(param_1 + 0x10));
      if (1 < *(ulonglong *)(param_1 + 0x10)) {
        free(_Memory);
      }
    }
    *(ulonglong *)(param_1 + 0x10) = uVar8;
    *(longlong *)(param_1 + 8) = lVar5;
  }
  plVar1 = (longlong *)(param_1 + 8);
  plVar4[10] = *(longlong *)(*plVar1 + uVar6 * 8);
  *(longlong **)(*plVar1 + uVar6 * 8) = plVar4;
  lVar5 = *plVar1;
  *(longlong *)(param_1 + 0x18) = *(longlong *)(param_1 + 0x18) + 1;
  *param_2 = plVar4;
  param_2[1] = lVar5 + uVar6 * 8;
  *(undefined1 *)(param_2 + 2) = 1;
  return param_2;
}

