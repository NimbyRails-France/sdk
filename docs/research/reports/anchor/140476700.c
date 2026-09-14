
void FUN_140476700(longlong *param_1,ulonglong param_2)

{
  char *pcVar1;
  void *pvVar2;
  longlong lVar3;
  longlong lVar4;
  undefined8 uVar5;
  uint uVar6;
  undefined1 auVar7 [16];
  ushort uVar8;
  char cVar9;
  char cVar10;
  char cVar11;
  char cVar12;
  char cVar13;
  char cVar14;
  char cVar15;
  char cVar16;
  char cVar17;
  char cVar18;
  char cVar19;
  char cVar20;
  char cVar21;
  char cVar22;
  char cVar23;
  char cVar24;
  void *pvVar25;
  longlong lVar26;
  void *_Memory;
  ulonglong uVar27;
  ulonglong uVar28;
  longlong lVar29;
  undefined1 *puVar30;
  ulonglong uVar31;
  byte bVar32;
  undefined1 auVar33 [16];
  undefined1 auVar34 [16];
  undefined1 local_res8;
  
  pvVar2 = (void *)*param_1;
  lVar29 = param_1[1];
  lVar3 = param_1[3];
  uVar28 = param_2 + 0x18 & 0xfffffffffffffff8;
  pvVar25 = (void *)FUN_140003270(param_2 * 0x50 + uVar28);
  *param_1 = (longlong)pvVar25;
  param_1[1] = uVar28 + (longlong)pvVar25;
  memset(pvVar25,-0x80,param_2 + 0x10);
  *(undefined1 *)(param_2 + *param_1) = 0xff;
  param_1[6] = (param_2 - (param_2 >> 3)) - param_1[2];
  param_1[3] = param_2;
  cVar24 = UNK_140aad76f;
  cVar23 = UNK_140aad76e;
  cVar22 = UNK_140aad76d;
  cVar21 = UNK_140aad76c;
  cVar20 = UNK_140aad76b;
  cVar19 = UNK_140aad76a;
  cVar18 = UNK_140aad769;
  cVar17 = UNK_140aad768;
  cVar16 = UNK_140aad767;
  cVar15 = UNK_140aad766;
  cVar14 = UNK_140aad765;
  cVar13 = UNK_140aad764;
  cVar12 = UNK_140aad763;
  cVar11 = UNK_140aad762;
  cVar10 = UNK_140aad761;
  cVar9 = DAT_140aad760;
  if (lVar3 != 0) {
    puVar30 = (undefined1 *)(lVar29 + 0x42);
    lVar29 = 0;
    do {
      if (-1 < *(char *)(lVar29 + (longlong)pvVar2)) {
        auVar7._8_8_ = 0;
        auVar7._0_8_ = *(ulonglong *)(puVar30 + -0x42);
        uVar27 = param_1[3];
        lVar4 = *param_1;
        uVar28 = SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar7,8) +
                 SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar7,0);
        uVar31 = uVar28 >> 7 & uVar27;
        pcVar1 = (char *)(uVar31 + lVar4);
        auVar33[0] = -(*pcVar1 < cVar9);
        auVar33[1] = -(pcVar1[1] < cVar10);
        auVar33[2] = -(pcVar1[2] < cVar11);
        auVar33[3] = -(pcVar1[3] < cVar12);
        auVar33[4] = -(pcVar1[4] < cVar13);
        auVar33[5] = -(pcVar1[5] < cVar14);
        auVar33[6] = -(pcVar1[6] < cVar15);
        auVar33[7] = -(pcVar1[7] < cVar16);
        auVar33[8] = -(pcVar1[8] < cVar17);
        auVar33[9] = -(pcVar1[9] < cVar18);
        auVar33[10] = -(pcVar1[10] < cVar19);
        auVar33[0xb] = -(pcVar1[0xb] < cVar20);
        auVar33[0xc] = -(pcVar1[0xc] < cVar21);
        auVar33[0xd] = -(pcVar1[0xd] < cVar22);
        auVar33[0xe] = -(pcVar1[0xe] < cVar23);
        auVar33[0xf] = -(pcVar1[0xf] < cVar24);
        uVar8 = (ushort)(SUB161(auVar33 >> 7,0) & 1) | (ushort)(SUB161(auVar33 >> 0xf,0) & 1) << 1 |
                (ushort)(SUB161(auVar33 >> 0x17,0) & 1) << 2 |
                (ushort)(SUB161(auVar33 >> 0x1f,0) & 1) << 3 |
                (ushort)(SUB161(auVar33 >> 0x27,0) & 1) << 4 |
                (ushort)(SUB161(auVar33 >> 0x2f,0) & 1) << 5 |
                (ushort)(SUB161(auVar33 >> 0x37,0) & 1) << 6 |
                (ushort)(SUB161(auVar33 >> 0x3f,0) & 1) << 7 |
                (ushort)(SUB161(auVar33 >> 0x47,0) & 1) << 8 |
                (ushort)(SUB161(auVar33 >> 0x4f,0) & 1) << 9 |
                (ushort)(SUB161(auVar33 >> 0x57,0) & 1) << 10 |
                (ushort)(SUB161(auVar33 >> 0x5f,0) & 1) << 0xb |
                (ushort)(SUB161(auVar33 >> 0x67,0) & 1) << 0xc |
                (ushort)(SUB161(auVar33 >> 0x6f,0) & 1) << 0xd |
                (ushort)(SUB161(auVar33 >> 0x77,0) & 1) << 0xe | (ushort)(auVar33[0xf] >> 7) << 0xf;
        lVar26 = 0;
        while (uVar8 == 0) {
          lVar26 = lVar26 + 0x10;
          uVar31 = uVar31 + lVar26 & uVar27;
          pcVar1 = (char *)(uVar31 + lVar4);
          auVar34[0] = -(*pcVar1 < cVar9);
          auVar34[1] = -(pcVar1[1] < cVar10);
          auVar34[2] = -(pcVar1[2] < cVar11);
          auVar34[3] = -(pcVar1[3] < cVar12);
          auVar34[4] = -(pcVar1[4] < cVar13);
          auVar34[5] = -(pcVar1[5] < cVar14);
          auVar34[6] = -(pcVar1[6] < cVar15);
          auVar34[7] = -(pcVar1[7] < cVar16);
          auVar34[8] = -(pcVar1[8] < cVar17);
          auVar34[9] = -(pcVar1[9] < cVar18);
          auVar34[10] = -(pcVar1[10] < cVar19);
          auVar34[0xb] = -(pcVar1[0xb] < cVar20);
          auVar34[0xc] = -(pcVar1[0xc] < cVar21);
          auVar34[0xd] = -(pcVar1[0xd] < cVar22);
          auVar34[0xe] = -(pcVar1[0xe] < cVar23);
          auVar34[0xf] = -(pcVar1[0xf] < cVar24);
          uVar8 = (ushort)(SUB161(auVar34 >> 7,0) & 1) | (ushort)(SUB161(auVar34 >> 0xf,0) & 1) << 1
                  | (ushort)(SUB161(auVar34 >> 0x17,0) & 1) << 2 |
                  (ushort)(SUB161(auVar34 >> 0x1f,0) & 1) << 3 |
                  (ushort)(SUB161(auVar34 >> 0x27,0) & 1) << 4 |
                  (ushort)(SUB161(auVar34 >> 0x2f,0) & 1) << 5 |
                  (ushort)(SUB161(auVar34 >> 0x37,0) & 1) << 6 |
                  (ushort)(SUB161(auVar34 >> 0x3f,0) & 1) << 7 |
                  (ushort)(SUB161(auVar34 >> 0x47,0) & 1) << 8 |
                  (ushort)(SUB161(auVar34 >> 0x4f,0) & 1) << 9 |
                  (ushort)(SUB161(auVar34 >> 0x57,0) & 1) << 10 |
                  (ushort)(SUB161(auVar34 >> 0x5f,0) & 1) << 0xb |
                  (ushort)(SUB161(auVar34 >> 0x67,0) & 1) << 0xc |
                  (ushort)(SUB161(auVar34 >> 0x6f,0) & 1) << 0xd |
                  (ushort)(SUB161(auVar34 >> 0x77,0) & 1) << 0xe |
                  (ushort)(auVar34[0xf] >> 7) << 0xf;
        }
        uVar6 = 0;
        if (uVar8 != 0) {
          for (; (uVar8 >> uVar6 & 1) == 0; uVar6 = uVar6 + 1) {
          }
        }
        bVar32 = (byte)uVar28 & 0x7f;
        uVar27 = uVar6 + uVar31 & uVar27;
        *(byte *)(lVar4 + uVar27) = bVar32;
        *(byte *)((uVar27 - 0x10 & param_1[3]) + (ulonglong)((uint)param_1[3] & 0xf) + 1 + *param_1)
             = bVar32;
        lVar4 = param_1[1];
        *(undefined8 *)(lVar4 + uVar27 * 0x50) = *(undefined8 *)(puVar30 + -0x42);
        *(undefined8 *)(lVar4 + 8 + uVar27 * 0x50) = *(undefined8 *)(puVar30 + -0x3a);
        *(undefined8 *)(lVar4 + 0x10 + uVar27 * 0x50) = *(undefined8 *)(puVar30 + -0x32);
        uVar5 = *(undefined8 *)(puVar30 + -0x2a);
        *(undefined1 **)(puVar30 + -0x2a) = &DAT_140a6d430;
        *(undefined8 *)(lVar4 + 0x18 + uVar27 * 0x50) = uVar5;
        uVar5 = *(undefined8 *)(puVar30 + -0x22);
        *(undefined8 *)(puVar30 + -0x22) = 0;
        *(undefined8 *)(lVar4 + 0x20 + uVar27 * 0x50) = uVar5;
        uVar5 = *(undefined8 *)(puVar30 + -0x1a);
        *(undefined8 *)(puVar30 + -0x1a) = 0;
        *(undefined8 *)(lVar4 + 0x28 + uVar27 * 0x50) = uVar5;
        uVar5 = *(undefined8 *)(puVar30 + -0x12);
        *(undefined8 *)(puVar30 + -0x12) = 0;
        *(undefined8 *)(lVar4 + 0x30 + uVar27 * 0x50) = uVar5;
        *(undefined1 *)(lVar4 + 0x38 + uVar27 * 0x50) = local_res8;
        *(undefined1 *)(lVar4 + 0x40 + uVar27 * 0x50) = puVar30[-2];
        *(undefined1 *)(lVar4 + 0x41 + uVar27 * 0x50) = puVar30[-1];
        *(undefined1 *)(lVar4 + 0x42 + uVar27 * 0x50) = *puVar30;
        *(undefined8 *)(lVar4 + 0x48 + uVar27 * 0x50) = *(undefined8 *)(puVar30 + 6);
        *(undefined8 *)(puVar30 + 6) = 0;
        lVar4 = *(longlong *)(puVar30 + -0x12);
        if (lVar4 != 0) {
          pvVar25 = *(void **)(puVar30 + -0x2a);
          _Memory = pvVar25;
          if ((0xfff < (lVar4 + 0x18U & 0xfffffffffffffff8) + lVar4 * 0x20) &&
             (_Memory = *(void **)((longlong)pvVar25 + -8),
             0x1f < (ulonglong)((longlong)pvVar25 + (-8 - (longlong)_Memory)))) goto LAB_1404769a2;
          free(_Memory);
          *(undefined1 **)(puVar30 + -0x2a) = &DAT_140a6d430;
          *(undefined8 *)(puVar30 + -0x22) = 0;
          *(undefined8 *)(puVar30 + -0x1a) = 0;
          *(undefined8 *)(puVar30 + -0x12) = 0;
          *(undefined8 *)(puVar30 + 6) = 0;
        }
      }
      lVar29 = lVar29 + 1;
      puVar30 = puVar30 + 0x50;
    } while (lVar29 != lVar3);
    pvVar25 = pvVar2;
    if ((0xfff < (lVar3 + 0x18U & 0xfffffffffffffff8) + lVar3 * 0x50) &&
       (pvVar25 = *(void **)((longlong)pvVar2 + -8),
       0x1f < (ulonglong)((longlong)pvVar2 + (-8 - (longlong)pvVar25)))) {
LAB_1404769a2:
                    /* WARNING: Subroutine does not return */
      _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    free(pvVar25);
  }
  return;
}

