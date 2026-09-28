
undefined8 *
FUN_1407e5970(longlong param_1,undefined8 *param_2,undefined8 param_3,longlong param_4,
             longlong param_5)

{
  undefined8 *puVar1;
  ulonglong *puVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  ulonglong *puVar6;
  ulonglong uVar7;
  longlong *plVar8;
  undefined8 *puVar9;
  bool bVar10;
  float fVar11;
  float fVar12;
  char cVar13;
  int iVar14;
  longlong lVar15;
  undefined8 *puVar16;
  ulonglong *puVar17;
  undefined8 *puVar18;
  longlong *plVar19;
  ulonglong *puVar20;
  longlong lVar21;
  ulonglong *puVar22;
  undefined1 local_res20 [8];
  
  FUN_1407e1840(param_1 + 0x1b8,param_4,*(undefined8 *)(param_4 + 0x600));
  FUN_1407e1840(param_1 + 0x1f8,param_4,*(undefined8 *)(param_4 + 0x608));
  FUN_1407e1840(param_1 + 0x238,param_4,*(undefined8 *)(param_4 + 0x610));
  FUN_1407e1840(param_1 + 0x278,param_4,*(undefined8 *)(param_4 + 0x618));
  FUN_1407e1840(param_1 + 0x2b8,param_4,*(undefined8 *)(param_4 + 0x620));
  FUN_1407e1840(param_1 + 0x2f8);
  puVar1 = (undefined8 *)(param_1 + 0x338);
  FUN_14032a5d0(puVar1);
  *param_2 = 0;
  param_2[1] = 0;
  param_2[2] = 0;
  FUN_1407e2640(param_1 + 0x58);
  FUN_1407e2640(param_1);
  FUN_1407e2640(param_1 + 0x160);
  FUN_1407e2640(param_1 + 0xb0);
  FUN_1407e2640(param_1 + 0x108);
  fVar12 = DAT_140aabfe0;
  fVar11 = DAT_140aab608;
  puVar6 = *(ulonglong **)(param_4 + 0x68);
  puVar22 = *(ulonglong **)(param_4 + 0x60);
  do {
    if (puVar22 == puVar6) {
      return param_2;
    }
    uVar7 = *puVar22;
    lVar21 = 0;
    puVar20 = *(ulonglong **)
               (*(longlong *)(param_4 + 0x38) + (uVar7 % (ulonglong)*(uint *)(param_4 + 0x40)) * 8);
    puVar17 = puVar20;
    if (puVar20 != (ulonglong *)0x0) {
      do {
        lVar15 = lVar21 + 1;
        if (uVar7 != *puVar17) {
          lVar15 = lVar21;
        }
        puVar2 = puVar17 + 0x54;
        puVar17 = (ulonglong *)*puVar2;
        lVar21 = lVar15;
      } while ((ulonglong *)*puVar2 != (ulonglong *)0x0);
      if (lVar15 != 0) {
        do {
          if (uVar7 == *puVar20) goto LAB_1407e5b2c;
          puVar20 = (ulonglong *)puVar20[0x54];
        } while (puVar20 != (ulonglong *)0x0);
        puVar20 = *(ulonglong **)(*(longlong *)(param_4 + 0x38) + *(longlong *)(param_4 + 0x40) * 8)
        ;
LAB_1407e5b2c:
        if ((*(char *)((longlong)puVar20 + 0xfa) != '\0') || (*(char *)(param_5 + 1) != '\0')) {
          iVar3 = *(int *)((longlong)puVar20 + 0x134);
          if (*(char *)(param_1 + 0x58) != '\0') {
            if (*(char *)(param_1 + 0xa0) == '\0') {
              if (*(char *)(param_1 + 0xa1) != '\0') {
                bVar10 = iVar3 <= *(int *)(param_1 + 0xa8);
LAB_1407e5b80:
                if (!bVar10) goto LAB_1407e5da7;
              }
            }
            else {
              iVar4 = *(int *)(param_1 + 0xa4);
              if (*(char *)(param_1 + 0xa1) == '\0') {
                bVar10 = iVar4 <= iVar3;
                goto LAB_1407e5b80;
              }
              iVar5 = *(int *)(param_1 + 0xa8);
              iVar14 = iVar4;
              if (iVar5 < iVar4) {
                *(int *)(param_1 + 0xa4) = iVar5;
                *(int *)(param_1 + 0xa8) = iVar4;
                iVar14 = iVar5;
                iVar5 = iVar4;
              }
              if ((iVar3 < iVar14) || (iVar5 < iVar3)) goto LAB_1407e5da7;
            }
          }
          lroundf(*(float *)((longlong)puVar20 + 0x104) * fVar12);
          lroundf(*(float *)((longlong)puVar20 + 0x104) * fVar12);
          cVar13 = FUN_1407e26c0(param_1);
          if (cVar13 != '\0') {
            lroundf((float)(longlong)puVar20[0x24] * fVar11);
            lroundf((float)(longlong)puVar20[0x24] * fVar11);
            cVar13 = FUN_1407e26c0(param_1 + 0x160);
            if (((cVar13 != '\0') && (cVar13 = FUN_1407e26c0(param_1 + 0xb0), cVar13 != '\0')) &&
               (cVar13 = FUN_1407e26c0(param_1 + 0x108), cVar13 != '\0')) {
              cVar13 = FUN_1407e19b0(param_1 + 0x1b8);
              if ((((cVar13 != '\0') && (cVar13 = FUN_1407e19b0(param_1 + 0x1f8), cVar13 != '\0'))
                  && ((cVar13 = FUN_1407e19b0(param_1 + 0x238), cVar13 != '\0' &&
                      ((cVar13 = FUN_1407e19b0(param_1 + 0x2b8), cVar13 != '\0' &&
                       (cVar13 = FUN_1407e19b0(param_1 + 0x2f8), cVar13 != '\0')))))) &&
                 (cVar13 = FUN_1407e19b0(param_1 + 0x278), cVar13 != '\0')) {
                plVar8 = (longlong *)puVar20[0x1d];
                for (plVar19 = (longlong *)puVar20[0x1c]; plVar19 != plVar8; plVar19 = plVar19 + 1)
                {
                  puVar9 = *(undefined8 **)(param_1 + 0x348);
                  puVar16 = puVar1;
                  while (puVar9 != (undefined8 *)0x0) {
                    puVar18 = puVar9;
                    if (*plVar19 <= (longlong)puVar9[4]) {
                      puVar18 = puVar9 + 1;
                      puVar16 = puVar9;
                    }
                    puVar9 = (undefined8 *)*puVar18;
                  }
                  if ((puVar16 == puVar1) || (*plVar19 < (longlong)puVar16[4])) {
                    puVar16 = (undefined8 *)FUN_140586e00(puVar1,local_res20);
                    puVar16 = (undefined8 *)*puVar16;
                  }
                  puVar16[5] = puVar16[5] + 1;
                }
                FUN_1402e37f0(param_2);
              }
            }
          }
        }
      }
    }
LAB_1407e5da7:
    puVar22 = puVar22 + 1;
  } while( true );
}

