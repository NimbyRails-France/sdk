
void FUN_1403635a0(longlong *param_1,ulonglong param_2)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  undefined1 *puVar3;
  undefined8 uVar4;
  longlong lVar5;
  longlong lVar6;
  undefined8 *puVar7;
  undefined8 *puVar8;
  undefined8 *puVar9;
  ulonglong uVar10;
  ulonglong uVar11;
  undefined1 *puVar12;
  undefined1 *puVar13;
  undefined1 local_res8;
  
  puVar7 = (undefined8 *)param_1[1];
  uVar11 = param_2;
  if ((ulonglong)((param_1[2] - (longlong)puVar7 >> 3) * 0x6db6db6db6db6db7) < param_2) {
    lVar5 = (longlong)puVar7 - *param_1 >> 3;
    lVar6 = lVar5 * 0x6db6db6db6db6db7;
    uVar11 = lVar5 * -0x2492492492492492;
    if (lVar6 == 0) {
      uVar11 = 1;
    }
    uVar10 = lVar6 + param_2;
    if (uVar11 < uVar10) {
      uVar11 = uVar10;
    }
    puVar7 = (undefined8 *)0x0;
    if (uVar11 != 0) {
      puVar7 = (undefined8 *)thunk_FUN_140983da8(uVar11 * 0x38);
    }
    puVar2 = (undefined1 *)param_1[1];
    puVar3 = (undefined1 *)*param_1;
    puVar9 = puVar7;
    if (puVar3 != puVar2) {
      puVar13 = (undefined1 *)((longlong)puVar7 + 0x29);
      do {
        uVar4 = *(undefined8 *)(puVar13 + (longlong)(puVar3 + (-0x29 - (longlong)puVar7)));
        *(undefined1 **)(puVar13 + (longlong)(puVar3 + (-0x29 - (longlong)puVar7))) = &DAT_140a6d430
        ;
        puVar1 = puVar13 + 0x38;
        *puVar9 = uVar4;
        puVar9 = puVar9 + 7;
        uVar4 = *(undefined8 *)(puVar1 + (longlong)(puVar3 + (-0x59 - (longlong)puVar7)));
        *(undefined8 *)(puVar1 + (longlong)(puVar3 + (-0x59 - (longlong)puVar7))) = 0;
        *(undefined8 *)(puVar13 + -0x21) = uVar4;
        uVar4 = *(undefined8 *)(puVar1 + (longlong)(puVar3 + (-0x51 - (longlong)puVar7)));
        *(undefined8 *)(puVar1 + (longlong)(puVar3 + (-0x51 - (longlong)puVar7))) = 0;
        *(undefined8 *)(puVar13 + -0x19) = uVar4;
        uVar4 = *(undefined8 *)(puVar1 + (longlong)(puVar3 + (-0x49 - (longlong)puVar7)));
        *(undefined8 *)(puVar1 + (longlong)(puVar3 + (-0x49 - (longlong)puVar7))) = 0;
        *(undefined8 *)(puVar13 + -0x11) = uVar4;
        puVar13[-9] = local_res8;
        puVar13[-1] = puVar1[(longlong)(puVar3 + (-0x39 - (longlong)puVar7))];
        *puVar13 = puVar1[(longlong)(puVar3 + (-0x38 - (longlong)puVar7))];
        puVar13[1] = puVar1[(longlong)(puVar3 + (-0x37 - (longlong)puVar7))];
        *(undefined8 *)(puVar13 + 7) =
             *(undefined8 *)(puVar1 + (longlong)(puVar3 + (-0x31 - (longlong)puVar7)));
        *(undefined8 *)(puVar1 + (longlong)(puVar3 + (-0x31 - (longlong)puVar7))) = 0;
        puVar12 = puVar13 + (longlong)(puVar3 + (0xf - (longlong)puVar7));
        puVar13 = puVar1;
      } while (puVar12 != puVar2);
    }
    if (param_2 != 0) {
      puVar8 = puVar9 + 2;
      uVar10 = param_2;
      do {
        puVar8[-2] = &DAT_140a6d430;
        puVar8[-1] = 0;
        *puVar8 = 0;
        puVar8[1] = 0;
        puVar8[4] = 0;
        puVar8 = puVar8 + 7;
        uVar10 = uVar10 - 1;
      } while (uVar10 != 0);
    }
    lVar6 = param_1[1];
    for (lVar5 = *param_1; lVar5 != lVar6; lVar5 = lVar5 + 0x38) {
      FUN_1403667b0(lVar5);
    }
    if ((void *)*param_1 != (void *)0x0) {
      free((void *)*param_1);
    }
    *param_1 = (longlong)puVar7;
    param_1[1] = (longlong)(puVar9 + param_2 * 7);
    param_1[2] = (longlong)(puVar7 + uVar11 * 7);
    return;
  }
  for (; uVar11 != 0; uVar11 = uVar11 - 1) {
    *puVar7 = &DAT_140a6d430;
    puVar7[1] = 0;
    puVar7[2] = 0;
    puVar7[3] = 0;
    puVar7[6] = 0;
    puVar7 = puVar7 + 7;
  }
  param_1[1] = param_1[1] + param_2 * 0x38;
  return;
}

