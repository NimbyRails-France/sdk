
void FUN_1407575e0(longlong *param_1,undefined8 param_2,longlong param_3)

{
  undefined8 uVar1;
  ulonglong *puVar2;
  undefined8 *********pppppppppuVar3;
  int iVar4;
  longlong lVar5;
  longlong lVar6;
  longlong lVar7;
  longlong *plVar8;
  undefined8 **********ppppppppppuVar9;
  undefined8 *_Buf2;
  double dVar10;
  undefined8 *********local_48;
  undefined8 uStack_40;
  size_t local_38;
  ulonglong local_30;
  void *local_28;
  undefined8 local_20;
  
  if (*param_1 == 0) {
    return;
  }
  if (param_1[1] == 0) {
    return;
  }
  if ((char)param_1[0x4b] != '\0') {
    lVar5 = FUN_140758670(param_3);
    FUN_140757520(param_1,lVar5);
    if ((longlong *)(lVar5 + 0x28) != param_1 + 0x47) {
      FUN_140030630();
    }
  }
  *(undefined1 *)(param_1 + 5) = 0;
  if (*(char *)((longlong)param_1 + 0x261) == '\0') goto LAB_14075776a;
  lVar5 = FUN_140758670(param_3);
  FUN_140019ca0(&local_48,*param_1 + 0x20);
  FUN_140757520(param_1);
  pppppppppuVar3 = local_48;
  if ((*(char *)(*param_1 + 0x40) != '\0') && ((char)param_1[0x4c] == '\0')) {
    if (local_38 != 0) {
      lVar6 = param_1[1];
      _Buf2 = (undefined8 *)(lVar6 + 8);
      if (0xf < *(ulonglong *)(lVar6 + 0x20)) {
        _Buf2 = (undefined8 *)*_Buf2;
      }
      ppppppppppuVar9 = &local_48;
      if (0xf < local_30) {
        ppppppppppuVar9 = (undefined8 **********)local_48;
      }
      if ((local_38 != *(size_t *)(lVar6 + 0x18)) ||
         (iVar4 = memcmp(ppppppppppuVar9,_Buf2,local_38), iVar4 != 0)) goto LAB_14075770b;
    }
    if (lVar5 + 0x28 != param_1[1] + 8) {
      FUN_140030630();
    }
  }
LAB_14075770b:
  lVar6 = param_1[0x4c];
  *(char *)(lVar5 + 0x48) = (char)lVar6;
  if ((char)lVar6 == '\0') {
    *(undefined1 *)(param_1 + 5) = 1;
  }
  if (0xf < local_30) {
    ppppppppppuVar9 = (undefined8 **********)pppppppppuVar3;
    if ((0xfff < local_30 + 1) &&
       (ppppppppppuVar9 = (undefined8 **********)pppppppppuVar3[-1],
       0x1f < (ulonglong)((longlong)pppppppppuVar3 + (-8 - (longlong)ppppppppppuVar9)))) {
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(ppppppppppuVar9);
  }
LAB_14075776a:
  if ((char)param_1[0x4d] != '\0') {
    lVar5 = FUN_140758670(param_3);
    FUN_140757520(param_1);
    *(undefined4 *)(lVar5 + 0x50) = *(undefined4 *)((longlong)param_1 + 0x264);
  }
  if ((char)param_1[0x4e] != '\0') {
    lVar5 = FUN_140758670(param_3);
    FUN_140757520(param_1);
    *(undefined4 *)(lVar5 + 0x54) = *(undefined4 *)((longlong)param_1 + 0x26c);
  }
  if ((char)param_1[0x4f] != '\0') {
    lVar5 = FUN_140758670(param_3);
    FUN_140757520(param_1);
    *(undefined4 *)(lVar5 + 0x4c) = *(undefined4 *)((longlong)param_1 + 0x274);
  }
  if ((char)param_1[0x50] != '\0') {
    lVar5 = FUN_140758670(param_3);
    FUN_140757520(param_1);
    *(undefined4 *)(lVar5 + 0x58) = *(undefined4 *)((longlong)param_1 + 0x27c);
  }
  if ((char)param_1[0x51] != '\0') {
    lVar5 = FUN_140758670(param_3);
    FUN_140757520(param_1);
    *(undefined4 *)(lVar5 + 0x5c) = *(undefined4 *)((longlong)param_1 + 0x284);
  }
  if (*(char *)((longlong)param_1 + 0x28d) != '\0') {
    lVar5 = FUN_140758670(param_3);
    FUN_140757520(param_1);
    *(undefined1 *)(lVar5 + 0x60) = *(undefined1 *)((longlong)param_1 + 0x28c);
  }
  if ((char)param_1[0x53] != '\0') {
    lVar5 = FUN_1407584c0(param_3);
    *(undefined8 *)(lVar5 + 0x20) = *(undefined8 *)*param_1;
    *(longlong *)(lVar5 + 0x28) = param_1[0x52];
  }
  FUN_140573630(param_1 + 6);
  if ((char)param_1[0x22] != '\0') {
    FUN_14031f8f0(&local_28,*param_1 + 0x48);
    FUN_1403fcc70(&local_28,param_1[0x21]);
    lVar5 = FUN_140758670(param_3);
    FUN_140757520(param_1);
    if ((void **)(lVar5 + 0x68) != &local_28) {
      FUN_14032fe20((void **)(lVar5 + 0x68),local_28,local_20);
    }
    if (local_28 != (void *)0x0) {
      free(local_28);
    }
  }
  uVar1 = *(undefined8 *)*param_1;
  if ((char)param_1[0x3c] != '\0') {
    lVar6 = FUN_140758300(param_3);
    *(undefined8 *)(lVar6 + 0x20) = uVar1;
    *(longlong *)(lVar6 + 0x28) = param_1[0x3b];
    lVar5 = 0;
    for (puVar2 = *(ulonglong **)
                   (param_1[0x35] +
                   ((ulonglong)param_1[0x3b] % (ulonglong)*(uint *)(param_1 + 0x36)) * 8);
        puVar2 != (ulonglong *)0x0; puVar2 = (ulonglong *)puVar2[1]) {
      lVar7 = lVar5 + 1;
      if (param_1[0x3b] != *puVar2) {
        lVar7 = lVar5;
      }
      lVar5 = lVar7;
    }
    *(uint *)(lVar6 + 0x30) = (uint)(lVar5 != 0);
  }
  uVar1 = *(undefined8 *)*param_1;
  if (param_1[0x45] != 0) {
    lVar5 = FUN_1407584c0(param_3);
    *(undefined8 *)(lVar5 + 0x20) = uVar1;
    *(longlong *)(lVar5 + 0x28) = param_1[0x45];
  }
  if (param_1[0x46] != 0) {
    if (*(longlong *)(param_3 + 600) == 0) {
                    /* WARNING: Subroutine does not return */
      abort();
    }
    lVar5 = *(longlong *)(*(longlong *)(param_3 + 600) + 0x428);
    lVar6 = FUN_14033f630(lVar5 + 0x380);
    if (((lVar6 != 0) && (lVar7 = *(longlong *)(lVar6 + 0x40), lVar7 != 0)) &&
       (plVar8 = (longlong *)FUN_14032c420(lVar5,lVar7), plVar8 != (longlong *)0x0)) {
      if ((*(char *)(lVar6 + 0x51) == '\0') || (*plVar8 != lVar7)) {
        dVar10 = *(double *)(lVar6 + 0x48);
      }
      else {
        dVar10 = *(double *)(lVar6 + 0x48);
        if (*(char *)((longlong)plVar8 + 0x2c) != *(char *)(lVar6 + 0x51)) {
          dVar10 = DAT_140aabd08 - dVar10;
        }
      }
      FUN_1403898d0(plVar8,&local_48,dVar10,0);
      DAT_140b81900 = local_48;
      DAT_140b81908 = uStack_40;
      if (DAT_140b81920 != '\x01') {
        DAT_140b81920 = '\x01';
      }
    }
  }
  *(undefined1 *)(param_1 + 4) = *(undefined1 *)((longlong)param_1 + 0x24);
  *(undefined1 *)((longlong)param_1 + 0x21) = *(undefined1 *)((longlong)param_1 + 0x25);
  *(undefined1 *)((longlong)param_1 + 0x22) = *(undefined1 *)((longlong)param_1 + 0x26);
  *(undefined1 *)((longlong)param_1 + 0x23) = *(undefined1 *)((longlong)param_1 + 0x27);
  return;
}

