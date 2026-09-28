
undefined8 *
FUN_14074c5a0(longlong param_1,undefined8 *param_2,undefined8 param_3,int *param_4,ulonglong param_5
             )

{
  longlong *plVar1;
  int iVar2;
  void *_Memory;
  int *piVar3;
  ulonglong uVar4;
  int *piVar5;
  longlong lVar6;
  ulonglong uVar7;
  ulonglong uVar8;
  ulonglong local_res8;
  
  uVar4 = param_5;
  local_res8 = param_5 % (ulonglong)*(uint *)(param_1 + 0x10);
  piVar5 = *(int **)(*(longlong *)(param_1 + 8) + local_res8 * 8);
  if (piVar5 != (int *)0x0) {
    do {
      if (*param_4 == *piVar5) {
        if (piVar5 != (int *)0x0) {
          lVar6 = *(longlong *)(param_1 + 8);
          *param_2 = piVar5;
          *(undefined1 *)(param_2 + 2) = 0;
          param_2[1] = lVar6 + local_res8 * 8;
          return param_2;
        }
        break;
      }
      piVar5 = *(int **)(piVar5 + 6);
    } while (piVar5 != (int *)0x0);
  }
  FUN_14001db20(param_1 + 0x20,&param_5,(ulonglong)*(uint *)(param_1 + 0x10),
                *(undefined4 *)(param_1 + 0x18),1);
  piVar5 = (int *)thunk_FUN_140983da8(0x20);
  uVar7 = 0;
  *piVar5 = *param_4;
  piVar5[2] = 0;
  piVar5[3] = 0;
  piVar5[4] = 0;
  piVar5[5] = 0;
  piVar5[6] = 0;
  piVar5[7] = 0;
  if ((char)param_5 != '\0') {
    uVar8 = (ulonglong)param_5._4_4_;
    local_res8 = uVar4 % uVar8;
    lVar6 = FUN_140254df0();
    if (*(longlong *)(param_1 + 0x10) != 0) {
      do {
        _Memory = *(void **)(param_1 + 8);
        piVar3 = *(int **)((longlong)_Memory + uVar7 * 8);
        while (piVar3 != (int *)0x0) {
          iVar2 = *piVar3;
          *(undefined8 *)((longlong)_Memory + uVar7 * 8) = *(undefined8 *)(piVar3 + 6);
          *(undefined8 *)(piVar3 + 6) =
               *(undefined8 *)(lVar6 + ((ulonglong)(longlong)iVar2 % uVar8) * 8);
          *(int **)(lVar6 + ((ulonglong)(longlong)iVar2 % uVar8) * 8) = piVar3;
          _Memory = *(void **)(param_1 + 8);
          piVar3 = *(int **)((longlong)_Memory + uVar7 * 8);
        }
        uVar7 = uVar7 + 1;
      } while (uVar7 < *(ulonglong *)(param_1 + 0x10));
      if (1 < *(ulonglong *)(param_1 + 0x10)) {
        free(_Memory);
      }
    }
    *(ulonglong *)(param_1 + 0x10) = uVar8;
    *(longlong *)(param_1 + 8) = lVar6;
  }
  plVar1 = (longlong *)(param_1 + 8);
  *(undefined8 *)(piVar5 + 6) = *(undefined8 *)(*plVar1 + local_res8 * 8);
  *(int **)(*plVar1 + local_res8 * 8) = piVar5;
  lVar6 = *plVar1;
  *(longlong *)(param_1 + 0x18) = *(longlong *)(param_1 + 0x18) + 1;
  *param_2 = piVar5;
  param_2[1] = lVar6 + local_res8 * 8;
  *(undefined1 *)(param_2 + 2) = 1;
  return param_2;
}

