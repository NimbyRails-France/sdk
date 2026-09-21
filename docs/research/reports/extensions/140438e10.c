// Candidate VA 140438e10; RVA 0x438e10
// Ghidra inferred prototype: undefined FUN_140438e10()

void FUN_140438e10(longlong *param_1,longlong *param_2)

{
  undefined8 *puVar1;
  undefined8 *puVar2;
  longlong lVar3;
  undefined8 uVar4;
  longlong lVar5;
  ulonglong *puVar6;
  longlong lVar7;
  longlong *plVar8;
  longlong lVar9;
  undefined8 *puVar10;
  ulonglong uVar11;
  undefined8 *puVar12;

  puVar12 = (undefined8 *)*param_1;
  if (puVar12 != (undefined8 *)param_1[1]) {
    lVar3 = *param_2;
    do {
      uVar4 = *puVar12;
      lVar7 = FUN_14033f6a0(lVar3 + 0x300,uVar4);
      if (lVar7 == 0) {
LAB_140438f07:
        if ((puVar12 + 10 < (undefined8 *)param_1[1]) &&
           (lVar7 = (param_1[1] - (longlong)(puVar12 + 10) >> 4) * -0x3333333333333333, 0 < lVar7))
        {
          puVar10 = puVar12 + 0x10;
          do {
            puVar1 = puVar10 + -4;
            puVar10[-0x10] = puVar10[-6];
            puVar2 = puVar10 + -0xe;
            puVar10[-0xf] = puVar10[-5];
            if (puVar2 != puVar1) {
              FUN_1403455e0(puVar2);
              uVar4 = *puVar2;
              *puVar2 = *puVar1;
              *puVar1 = uVar4;
              uVar4 = puVar10[-0xd];
              puVar10[-0xd] = puVar10[-3];
              puVar10[-3] = uVar4;
              uVar4 = puVar10[-0xc];
              puVar10[-0xc] = puVar10[-2];
              puVar10[-2] = uVar4;
            }
            puVar1 = puVar10 + -1;
            puVar2 = puVar10 + -0xb;
            if (puVar2 != puVar1) {
              uVar4 = *puVar2;
              *puVar2 = uVar4;
              puVar10[-9] = puVar10[-9];
              puVar10[-10] = uVar4;
              uVar4 = *puVar2;
              *puVar2 = *puVar1;
              *puVar1 = uVar4;
              uVar4 = puVar10[-10];
              puVar10[-10] = *puVar10;
              *puVar10 = uVar4;
              uVar4 = puVar10[-9];
              puVar10[-9] = puVar10[1];
              puVar10[1] = uVar4;
            }
            lVar7 = lVar7 + -1;
            puVar10[-8] = puVar10[2];
            *(undefined4 *)(puVar10 + -7) = *(undefined4 *)(puVar10 + 3);
            puVar10 = puVar10 + 10;
          } while (0 < lVar7);
        }
        param_1[1] = param_1[1] + -0x50;
        FUN_14030dbd0(param_1[1]);
      }
      else {
        if ((*(char *)(lVar7 + 0xd8) != '\0') &&
           (*(longlong *)(lVar7 + 0xe0) == *(longlong *)(lVar7 + 0xe8))) {
          lVar5 = param_2[1];
          plVar8 = (longlong *)FUN_1402a2e80(lVar5 + 0x10,uVar4);
          if ((plVar8 != (longlong *)0x0) && (*plVar8 == plVar8[1])) {
            uVar11 = puVar12[8];
            plVar8 = plVar8 + 0xe;
            lVar9 = FUN_1402a7900(plVar8);
            if (lVar9 != 0) {
              for (puVar6 = *(ulonglong **)
                             (*(longlong *)(lVar7 + 0x128) +
                             (uVar11 % (ulonglong)*(uint *)(lVar7 + 0x130)) * 8);
                  puVar6 != (ulonglong *)0x0; puVar6 = (ulonglong *)puVar6[4]) {
                if (uVar11 == *puVar6) {
                  if (puVar6 != *(ulonglong **)
                                 (*(longlong *)(lVar7 + 0x128) + *(longlong *)(lVar7 + 0x130) * 8))
                  {
                    FUN_1404232e0(lVar5,plVar8,puVar12,lVar9,puVar6 + 1);
                    goto LAB_14043903d;
                  }
                  break;
                }
              }
            }
            goto LAB_140438f07;
          }
        }
LAB_14043903d:
        puVar12 = puVar12 + 10;
      }
    } while (puVar12 != (undefined8 *)param_1[1]);
  }
  FUN_140422630(param_1);
  return;
}


// Incoming references
// 0xc22a78 DATA caller none
// 0xb06a2c DATA caller none
// 0xb06a3c DATA caller none
// 0x4277d9 UNCONDITIONAL_CALL caller 1404277a0
// 0x426ee6 UNCONDITIONAL_CALL caller 140426cf0
// 0x427006 UNCONDITIONAL_CALL caller 140426cf0
