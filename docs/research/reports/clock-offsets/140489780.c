
void FUN_140489780(longlong param_1)

{
  void *pvVar1;
  void *pvVar2;
  longlong lVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  longlong *plVar6;
  longlong *plVar7;
  longlong *plVar8;
  longlong *plVar9;
  longlong *plVar10;
  longlong lVar11;
  longlong lVar12;
  void *_Memory;
  void *pvVar13;
  undefined1 local_28 [32];

  plVar10 = *(longlong **)(param_1 + 0x1170);
  _Memory = (void *)*plVar10;
  plVar6 = plVar10;
  if (_Memory == (void *)0x0) {
    _Memory = (void *)plVar10[1];
    plVar9 = plVar10;
    while (plVar6 = plVar9 + 1, _Memory == (void *)0x0) {
      _Memory = (void *)plVar9[2];
      plVar9 = plVar6;
    }
  }
  if (_Memory != (void *)plVar10[*(longlong *)(param_1 + 0x1178)]) {
    do {
      pvVar13 = *(void **)((longlong)_Memory + 0x5c0);
      plVar10 = plVar6;
      if (*(char *)((longlong)_Memory + 0x10) == '\0') {
        while (pvVar13 == (void *)0x0) {
          pvVar13 = (void *)plVar10[1];
          plVar10 = plVar10 + 1;
        }
      }
      else {
        while (pvVar13 == (void *)0x0) {
          pvVar13 = (void *)plVar10[1];
          plVar10 = plVar10 + 1;
        }
        pvVar1 = (void *)*plVar6;
        pvVar2 = *(void **)((longlong)pvVar1 + 0x5c0);
        if (pvVar1 == _Memory) {
          *plVar6 = (longlong)pvVar2;
        }
        else {
          for (; pvVar2 != _Memory; pvVar2 = *(void **)((longlong)pvVar2 + 0x5c0)) {
            pvVar1 = pvVar2;
          }
          *(undefined8 *)((longlong)pvVar1 + 0x5c0) = *(undefined8 *)((longlong)pvVar2 + 0x5c0);
        }
        FUN_14031f9e0((longlong)_Memory + 0x5a8);
        FUN_140002d30((longlong)_Memory + 0x48);
        if (*(char *)((longlong)_Memory + 0x40) != '\0') {
          FUN_140002d30((longlong)_Memory + 0x20);
        }
        free(_Memory);
        *(longlong *)(param_1 + 0x1180) = *(longlong *)(param_1 + 0x1180) + -1;
        *(longlong *)(param_1 + 0x1590) = *(longlong *)(param_1 + 0x1590) + 1;
      }
      plVar6 = plVar10;
      _Memory = pvVar13;
    } while (pvVar13 !=
             *(void **)(*(longlong *)(param_1 + 0x1170) + *(longlong *)(param_1 + 0x1178) * 8));
  }
  plVar10 = *(longlong **)(param_1 + 0xbf0);
  while (plVar10 != (longlong *)(param_1 + 0xbe8)) {
    if (0xff < (ulonglong)plVar10[8]) {
      plVar6 = (longlong *)FUN_140494160(param_1 + 0x1168,local_28);
      lVar3 = *plVar6;
      *(longlong *)(lVar3 + 8) = plVar10[8];
      *(char *)(lVar3 + 0x10) = (char)plVar10[9];
      FUN_140410ce0(lVar3 + 0x18,plVar10 + 10);
      plVar6 = plVar10 + 0x10;
      if ((longlong *)(lVar3 + 0x48) != plVar6) {
        if (0xf < (ulonglong)plVar10[0x13]) {
          plVar6 = (longlong *)*plVar6;
        }
        FUN_140030630(lVar3 + 0x48,plVar6,plVar10[0x12]);
      }
      lVar11 = 5;
      plVar6 = (longlong *)(lVar3 + 0x68);
      plVar9 = plVar10 + 0x14;
      do {
        plVar8 = plVar9;
        plVar7 = plVar6;
        lVar12 = plVar8[1];
        *plVar7 = *plVar8;
        plVar7[1] = lVar12;
        lVar12 = plVar8[3];
        plVar7[2] = plVar8[2];
        plVar7[3] = lVar12;
        lVar12 = plVar8[5];
        plVar7[4] = plVar8[4];
        plVar7[5] = lVar12;
        lVar12 = plVar8[7];
        plVar7[6] = plVar8[6];
        plVar7[7] = lVar12;
        lVar12 = plVar8[9];
        plVar7[8] = plVar8[8];
        plVar7[9] = lVar12;
        lVar12 = plVar8[0xb];
        plVar7[10] = plVar8[10];
        plVar7[0xb] = lVar12;
        lVar12 = plVar8[0xd];
        plVar7[0xc] = plVar8[0xc];
        plVar7[0xd] = lVar12;
        lVar12 = plVar8[0xf];
        plVar7[0xe] = plVar8[0xe];
        plVar7[0xf] = lVar12;
        lVar11 = lVar11 + -1;
        plVar6 = plVar7 + 0x10;
        plVar9 = plVar8 + 0x10;
      } while (lVar11 != 0);
      lVar11 = plVar8[0x11];
      lVar12 = 5;
      plVar7[0x10] = plVar8[0x10];
      plVar7[0x11] = lVar11;
      lVar11 = plVar8[0x13];
      plVar7[0x12] = plVar8[0x12];
      plVar7[0x13] = lVar11;
      plVar6 = (longlong *)(lVar3 + 0x308);
      plVar9 = plVar10 + 0x68;
      do {
        plVar8 = plVar9;
        plVar7 = plVar6;
        lVar11 = plVar8[1];
        *plVar7 = *plVar8;
        plVar7[1] = lVar11;
        lVar11 = plVar8[3];
        plVar7[2] = plVar8[2];
        plVar7[3] = lVar11;
        lVar11 = plVar8[5];
        plVar7[4] = plVar8[4];
        plVar7[5] = lVar11;
        lVar11 = plVar8[7];
        plVar7[6] = plVar8[6];
        plVar7[7] = lVar11;
        lVar11 = plVar8[9];
        plVar7[8] = plVar8[8];
        plVar7[9] = lVar11;
        lVar11 = plVar8[0xb];
        plVar7[10] = plVar8[10];
        plVar7[0xb] = lVar11;
        lVar11 = plVar8[0xd];
        plVar7[0xc] = plVar8[0xc];
        plVar7[0xd] = lVar11;
        lVar11 = plVar8[0xf];
        plVar7[0xe] = plVar8[0xe];
        plVar7[0xf] = lVar11;
        lVar12 = lVar12 + -1;
        plVar6 = plVar7 + 0x10;
        plVar9 = plVar8 + 0x10;
      } while (lVar12 != 0);
      uVar4 = *(undefined4 *)((longlong)plVar8 + 0x84);
      lVar11 = plVar8[0x11];
      uVar5 = *(undefined4 *)((longlong)plVar8 + 0x8c);
      *(int *)(plVar7 + 0x10) = (int)plVar8[0x10];
      *(undefined4 *)((longlong)plVar7 + 0x84) = uVar4;
      *(int *)(plVar7 + 0x11) = (int)lVar11;
      *(undefined4 *)((longlong)plVar7 + 0x8c) = uVar5;
      uVar4 = *(undefined4 *)((longlong)plVar8 + 0x94);
      lVar11 = plVar8[0x13];
      uVar5 = *(undefined4 *)((longlong)plVar8 + 0x9c);
      *(int *)(plVar7 + 0x12) = (int)plVar8[0x12];
      *(undefined4 *)((longlong)plVar7 + 0x94) = uVar4;
      *(int *)(plVar7 + 0x13) = (int)lVar11;
      *(undefined4 *)((longlong)plVar7 + 0x9c) = uVar5;
      if ((longlong *)(lVar3 + 0x5a8) != plVar10 + 0xbc) {
        FUN_14041cae0((longlong *)(lVar3 + 0x5a8),plVar10[0xbc],plVar10[0xbd]);
      }
      *(longlong *)(param_1 + 0x1590) = *(longlong *)(param_1 + 0x1590) + 1;
    }
    plVar6 = (longlong *)*plVar10;
    if (plVar6 == (longlong *)0x0) {
      plVar6 = (longlong *)plVar10[2];
      plVar9 = (longlong *)0x0;
      if (plVar10 == (longlong *)*plVar6) {
        do {
          plVar10 = plVar6;
          plVar6 = (longlong *)plVar10[2];
        } while (plVar10 == (longlong *)*plVar6);
        plVar9 = (longlong *)*plVar10;
      }
      if (plVar9 != plVar6) {
        plVar10 = plVar6;
      }
    }
    else {
      for (plVar9 = (longlong *)plVar6[1]; plVar10 = plVar6, plVar9 != (longlong *)0x0;
          plVar9 = (longlong *)plVar9[1]) {
        plVar6 = plVar9;
      }
    }
  }
  FUN_14038bbf0(param_1 + 0x428);
  return;
}
