
void FUN_140479530(longlong param_1,longlong param_2,undefined8 *param_3)

{
  ulonglong uVar1;
  ulonglong *puVar2;
  undefined8 *puVar3;
  longlong lVar4;
  undefined8 *puVar5;
  undefined8 *puVar6;
  undefined8 *puVar7;
  undefined8 *puVar8;
  undefined8 *puVar9;
  undefined8 *local_res18;
  undefined1 local_58 [32];

  puVar9 = (undefined8 *)param_3[7];
  while (puVar9 != param_3 + 6) {
    if ((undefined8 *)param_3[2] != (undefined8 *)0x0) {
      puVar3 = (undefined8 *)param_3[2];
      puVar7 = param_3;
      do {
        if ((longlong)puVar3[4] < (longlong)puVar9[4]) {
          puVar5 = (undefined8 *)*puVar3;
        }
        else {
          puVar5 = (undefined8 *)puVar3[1];
          puVar7 = puVar3;
        }
        puVar3 = puVar5;
      } while (puVar5 != (undefined8 *)0x0);
      if ((puVar7 != param_3) && ((longlong)puVar7[4] <= (longlong)puVar9[4])) {
        FUN_140451820(param_1 + 0x518,puVar9[4]);
      }
    }
    puVar3 = (undefined8 *)*puVar9;
    if (puVar3 == (undefined8 *)0x0) {
      puVar3 = (undefined8 *)puVar9[2];
      puVar7 = (undefined8 *)0x0;
      if (puVar9 == (undefined8 *)*puVar3) {
        do {
          puVar9 = puVar3;
          puVar3 = (undefined8 *)puVar9[2];
        } while (puVar9 == (undefined8 *)*puVar3);
        puVar7 = (undefined8 *)*puVar9;
      }
      if (puVar7 != puVar3) {
        puVar9 = puVar3;
      }
    }
    else {
      for (puVar7 = (undefined8 *)puVar3[1]; puVar9 = puVar3, puVar7 != (undefined8 *)0x0;
          puVar7 = (undefined8 *)puVar7[1]) {
        puVar3 = puVar7;
      }
    }
  }
  puVar9 = (undefined8 *)param_3[0x25];
  while (puVar9 != param_3 + 0x24) {
    FUN_1404863d0(param_1 + 0x40,puVar9[4]);
    puVar3 = (undefined8 *)*puVar9;
    if (puVar3 == (undefined8 *)0x0) {
      puVar3 = (undefined8 *)puVar9[2];
      puVar7 = (undefined8 *)0x0;
      if (puVar9 == (undefined8 *)*puVar3) {
        do {
          puVar9 = puVar3;
          puVar3 = (undefined8 *)puVar9[2];
        } while (puVar9 == (undefined8 *)*puVar3);
        puVar7 = (undefined8 *)*puVar9;
      }
      if (puVar7 != puVar3) {
        puVar9 = puVar3;
      }
    }
    else {
      for (puVar7 = (undefined8 *)puVar3[1]; puVar9 = puVar3, puVar7 != (undefined8 *)0x0;
          puVar7 = (undefined8 *)puVar7[1]) {
        puVar3 = puVar7;
      }
    }
  }
  puVar9 = (undefined8 *)param_3[0x2b];
  while (puVar9 != param_3 + 0x2a) {
    uVar1 = puVar9[4];
    puVar2 = (ulonglong *)FUN_140486a40(param_1 + 0x40,uVar1);
    if (puVar2 != (ulonglong *)0x0) {
      FUN_140351180(puVar2);
      *puVar2 = uVar1 | 0xffff000000000000;
    }
    *(longlong *)(param_1 + 0x2120) = *(longlong *)(param_1 + 0x2120) + 1;
    puVar3 = (undefined8 *)*puVar9;
    if (puVar3 == (undefined8 *)0x0) {
      puVar3 = (undefined8 *)puVar9[2];
      puVar7 = (undefined8 *)0x0;
      if (puVar9 == (undefined8 *)*puVar3) {
        do {
          puVar9 = puVar3;
          puVar3 = (undefined8 *)puVar9[2];
        } while (puVar9 == (undefined8 *)*puVar3);
        puVar7 = (undefined8 *)*puVar9;
      }
      if (puVar7 != puVar3) {
        puVar9 = puVar3;
      }
    }
    else {
      for (puVar7 = (undefined8 *)puVar3[1]; puVar9 = puVar3, puVar7 != (undefined8 *)0x0;
          puVar7 = (undefined8 *)puVar7[1]) {
        puVar3 = puVar7;
      }
    }
  }
  puVar9 = (undefined8 *)param_3[0x1f];
  while (puVar9 != param_3 + 0x1e) {
    if ((undefined8 *)param_3[0x1a] != (undefined8 *)0x0) {
      puVar3 = (undefined8 *)param_3[0x1a];
      puVar7 = param_3 + 0x18;
      do {
        if ((longlong)puVar3[4] < (longlong)puVar9[4]) {
          puVar5 = (undefined8 *)*puVar3;
        }
        else {
          puVar5 = (undefined8 *)puVar3[1];
          puVar7 = puVar3;
        }
        puVar3 = puVar5;
      } while (puVar5 != (undefined8 *)0x0);
      if ((puVar7 != param_3 + 0x18) && ((longlong)puVar7[4] <= (longlong)puVar9[4])) {
        FUN_1404863d0(param_1 + 0x40);
        *(longlong *)(param_1 + 0x2120) = *(longlong *)(param_1 + 0x2120) + 1;
      }
    }
    puVar3 = (undefined8 *)*puVar9;
    if (puVar3 == (undefined8 *)0x0) {
      puVar3 = (undefined8 *)puVar9[2];
      puVar7 = (undefined8 *)0x0;
      if (puVar9 == (undefined8 *)*puVar3) {
        do {
          puVar9 = puVar3;
          puVar3 = (undefined8 *)puVar9[2];
        } while (puVar9 == (undefined8 *)*puVar3);
        puVar7 = (undefined8 *)*puVar9;
      }
      if (puVar7 != puVar3) {
        puVar9 = puVar3;
      }
    }
    else {
      for (puVar7 = (undefined8 *)puVar3[1]; puVar9 = puVar3, puVar7 != (undefined8 *)0x0;
          puVar7 = (undefined8 *)puVar7[1]) {
        puVar3 = puVar7;
      }
    }
  }
  local_res18 = param_3 + 0x60;
  puVar9 = (undefined8 *)param_3[0x6d];
  if (puVar9 != param_3 + 0x6c) {
    do {
      uVar1 = puVar9[4];
      puVar3 = (undefined8 *)FUN_14033f780(param_2 + 0x200);
      if (puVar3 != (undefined8 *)0x0) {
        FUN_1404597b0(param_1 + 0xa0,*puVar3);
        FUN_1404594d0(param_1 + 0xd0,*puVar3);
        FUN_1403bc3b0(param_1 + 0x100,puVar3);
        lVar4 = FUN_14045f300(param_1 + 0xa0);
        if (lVar4 != 0) {
          FUN_14044bb80(lVar4);
        }
      }
      lVar4 = *(longlong *)(param_1 + 0x2200);
      for (puVar2 = *(ulonglong **)
                     (*(longlong *)(lVar4 + 0x4c0) +
                     (uVar1 % (ulonglong)*(uint *)(lVar4 + 0x4c8)) * 8); puVar2 != (ulonglong *)0x0;
          puVar2 = (ulonglong *)puVar2[1]) {
        if (uVar1 == *puVar2) goto LAB_1404798f1;
      }
      puVar2 = (ulonglong *)thunk_FUN_140983da8(0x10);
      *puVar2 = uVar1;
      puVar2[1] = 0;
      FUN_1403414c0(lVar4 + 0x4b8,local_58);
LAB_1404798f1:
      puVar3 = (undefined8 *)*puVar9;
      if (puVar3 == (undefined8 *)0x0) {
        puVar3 = (undefined8 *)puVar9[2];
        puVar7 = (undefined8 *)0x0;
        if (puVar9 == (undefined8 *)*puVar3) {
          do {
            puVar9 = puVar3;
            puVar3 = (undefined8 *)puVar9[2];
          } while (puVar9 == (undefined8 *)*puVar3);
          puVar7 = (undefined8 *)*puVar9;
        }
        if (puVar7 != puVar3) {
          puVar9 = puVar3;
        }
      }
      else {
        for (puVar7 = (undefined8 *)puVar3[1]; puVar9 = puVar3, puVar7 != (undefined8 *)0x0;
            puVar7 = (undefined8 *)puVar7[1]) {
          puVar3 = puVar7;
        }
      }
    } while (puVar9 != param_3 + 0x6c);
  }
  puVar9 = local_res18 + 0x12;
  puVar3 = (undefined8 *)local_res18[0x13];
  puVar7 = local_res18;
  do {
    if (puVar3 == puVar9) {
      puVar9 = (undefined8 *)puVar7[7];
      while (puVar9 != puVar7 + 6) {
        if ((undefined8 *)puVar7[2] != (undefined8 *)0x0) {
          puVar3 = (undefined8 *)puVar9[4];
          puVar5 = (undefined8 *)puVar7[2];
          puVar8 = puVar7;
          do {
            if ((longlong)puVar5[4] < (longlong)puVar3) {
              puVar6 = (undefined8 *)*puVar5;
            }
            else {
              puVar6 = (undefined8 *)puVar5[1];
              puVar8 = puVar5;
            }
            puVar5 = puVar6;
          } while (puVar6 != (undefined8 *)0x0);
          if ((puVar8 != puVar7) && ((longlong)puVar8[4] <= (longlong)puVar3)) {
            local_res18 = puVar3;
            FUN_1404597b0(param_1 + 0xa0);
            FUN_1404594d0(param_1 + 0xd0,puVar3);
            FUN_1403bc3b0(param_1 + 0x100,&local_res18);
            *(longlong *)(param_1 + 0x2120) = *(longlong *)(param_1 + 0x2120) + 1;
          }
        }
        puVar3 = (undefined8 *)*puVar9;
        if (puVar3 == (undefined8 *)0x0) {
          puVar3 = (undefined8 *)puVar9[2];
          puVar5 = (undefined8 *)0x0;
          if (puVar9 == (undefined8 *)*puVar3) {
            do {
              puVar9 = puVar3;
              puVar3 = (undefined8 *)puVar9[2];
            } while (puVar9 == (undefined8 *)*puVar3);
            puVar5 = (undefined8 *)*puVar9;
          }
          if (puVar5 != puVar3) {
            puVar9 = puVar3;
          }
        }
        else {
          for (puVar5 = (undefined8 *)puVar3[1]; puVar9 = puVar3, puVar5 != (undefined8 *)0x0;
              puVar5 = (undefined8 *)puVar5[1]) {
            puVar3 = puVar5;
          }
        }
      }
      return;
    }
    uVar1 = puVar3[4];
    local_res18 = puVar7;
    puVar2 = (ulonglong *)FUN_14045f300(param_1 + 0xa0,uVar1);
    if (puVar2 != (ulonglong *)0x0) {
      FUN_140351080(puVar2);
      *puVar2 = uVar1 | 0xffff000000000000;
    }
    puVar2 = (ulonglong *)FUN_14045f1c0(param_1 + 0xd0);
    if (puVar2 != (ulonglong *)0x0) {
      FUN_14034b770(puVar2 + 1);
      *puVar2 = uVar1 | 0xffff000000000000;
    }
    for (puVar2 = *(ulonglong **)
                   (*(longlong *)(param_1 + 0x108) +
                   (uVar1 % (ulonglong)*(uint *)(param_1 + 0x110)) * 8); puVar2 != (ulonglong *)0x0;
        puVar2 = (ulonglong *)puVar2[1]) {
      if (uVar1 == *puVar2) goto LAB_140479a42;
    }
    puVar2 = (ulonglong *)thunk_FUN_140983da8(0x10);
    *puVar2 = uVar1;
    puVar2[1] = 0;
    FUN_1403414c0(param_1 + 0x100,local_58);
LAB_140479a42:
    *(longlong *)(param_1 + 0x2120) = *(longlong *)(param_1 + 0x2120) + 1;
    puVar5 = (undefined8 *)*puVar3;
    puVar7 = local_res18;
    if (puVar5 == (undefined8 *)0x0) {
      puVar5 = (undefined8 *)puVar3[2];
      puVar8 = (undefined8 *)0x0;
      if (puVar3 == (undefined8 *)*puVar5) {
        do {
          puVar3 = puVar5;
          puVar5 = (undefined8 *)puVar3[2];
        } while (puVar3 == (undefined8 *)*puVar5);
        puVar8 = (undefined8 *)*puVar3;
      }
      if (puVar8 != puVar5) {
        puVar3 = puVar5;
      }
    }
    else {
      for (puVar8 = (undefined8 *)puVar5[1]; puVar3 = puVar5, puVar8 != (undefined8 *)0x0;
          puVar8 = (undefined8 *)puVar8[1]) {
        puVar5 = puVar8;
      }
    }
  } while( true );
}
