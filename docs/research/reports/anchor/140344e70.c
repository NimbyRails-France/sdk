
longlong * FUN_140344e70(longlong *param_1,longlong *param_2)

{
  char *pcVar1;
  char cVar2;
  ulonglong uVar3;
  longlong lVar4;
  uint uVar5;
  longlong lVar6;
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
  uint uVar25;
  byte bVar26;
  char *pcVar27;
  longlong lVar28;
  ulonglong uVar29;
  ulonglong uVar30;
  ulonglong *puVar31;
  undefined1 auVar32 [16];
  undefined1 auVar33 [16];
  undefined1 auVar34 [16];
  undefined1 auVar35 [16];
  
  *param_1 = (longlong)&DAT_140a6d430;
  param_1[1] = 0;
  param_1[2] = 0;
  param_1[3] = 0;
  param_1[6] = 0;
  uVar3 = param_2[3];
  if (uVar3 != 0) {
    lVar6 = 0x3f;
    if (uVar3 != 0) {
      for (; uVar3 >> lVar6 == 0; lVar6 = lVar6 + -1) {
      }
    }
    if (uVar3 == 0) {
      bVar26 = 0;
    }
    else {
      bVar26 = 0x3f - (char)lVar6;
    }
    if (0xffffffffffffffffU >> (bVar26 & 0x3f) != 0) {
      FUN_140345730(param_1);
    }
  }
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
  puVar31 = (ulonglong *)param_2[1];
  pcVar27 = (char *)*param_2;
  cVar2 = *pcVar27;
  while (cVar2 < -1) {
    auVar32[0] = -(*pcVar27 < DAT_140aad760);
    auVar32[1] = -(pcVar27[1] < UNK_140aad761);
    auVar32[2] = -(pcVar27[2] < UNK_140aad762);
    auVar32[3] = -(pcVar27[3] < UNK_140aad763);
    auVar32[4] = -(pcVar27[4] < UNK_140aad764);
    auVar32[5] = -(pcVar27[5] < UNK_140aad765);
    auVar32[6] = -(pcVar27[6] < UNK_140aad766);
    auVar32[7] = -(pcVar27[7] < UNK_140aad767);
    auVar32[8] = -(pcVar27[8] < UNK_140aad768);
    auVar32[9] = -(pcVar27[9] < UNK_140aad769);
    auVar32[10] = -(pcVar27[10] < UNK_140aad76a);
    auVar32[0xb] = -(pcVar27[0xb] < UNK_140aad76b);
    auVar32[0xc] = -(pcVar27[0xc] < UNK_140aad76c);
    auVar32[0xd] = -(pcVar27[0xd] < UNK_140aad76d);
    auVar32[0xe] = -(pcVar27[0xe] < UNK_140aad76e);
    auVar32[0xf] = -(pcVar27[0xf] < UNK_140aad76f);
    uVar25 = (ushort)((ushort)(SUB161(auVar32 >> 7,0) & 1) |
                      (ushort)(SUB161(auVar32 >> 0xf,0) & 1) << 1 |
                      (ushort)(SUB161(auVar32 >> 0x17,0) & 1) << 2 |
                      (ushort)(SUB161(auVar32 >> 0x1f,0) & 1) << 3 |
                      (ushort)(SUB161(auVar32 >> 0x27,0) & 1) << 4 |
                      (ushort)(SUB161(auVar32 >> 0x2f,0) & 1) << 5 |
                      (ushort)(SUB161(auVar32 >> 0x37,0) & 1) << 6 |
                      (ushort)(SUB161(auVar32 >> 0x3f,0) & 1) << 7 |
                      (ushort)(SUB161(auVar32 >> 0x47,0) & 1) << 8 |
                      (ushort)(SUB161(auVar32 >> 0x4f,0) & 1) << 9 |
                      (ushort)(SUB161(auVar32 >> 0x57,0) & 1) << 10 |
                      (ushort)(SUB161(auVar32 >> 0x5f,0) & 1) << 0xb |
                      (ushort)(SUB161(auVar32 >> 0x67,0) & 1) << 0xc |
                      (ushort)(SUB161(auVar32 >> 0x6f,0) & 1) << 0xd |
                      (ushort)(SUB161(auVar32 >> 0x77,0) & 1) << 0xe |
                     (ushort)(auVar32[0xf] >> 7) << 0xf) + 1;
    uVar5 = 0;
    if (uVar25 != 0) {
      for (; (uVar25 >> uVar5 & 1) == 0; uVar5 = uVar5 + 1) {
      }
    }
    pcVar27 = pcVar27 + uVar5;
    puVar31 = puVar31 + uVar5;
    cVar2 = *pcVar27;
  }
  lVar6 = param_2[3];
  lVar4 = *param_2;
  while (pcVar27 != (char *)(lVar6 + lVar4)) {
    auVar7._8_8_ = 0;
    auVar7._0_8_ = *puVar31;
    uVar3 = SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar7,8) +
            SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar7,0);
    uVar29 = param_1[3];
    uVar30 = uVar3 >> 7 & uVar29;
    pcVar1 = (char *)(*param_1 + uVar30);
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
    lVar28 = 0;
    while (uVar8 == 0) {
      lVar28 = lVar28 + 0x10;
      uVar30 = uVar30 + lVar28 & uVar29;
      pcVar1 = (char *)(uVar30 + *param_1);
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
      uVar8 = (ushort)(SUB161(auVar34 >> 7,0) & 1) | (ushort)(SUB161(auVar34 >> 0xf,0) & 1) << 1 |
              (ushort)(SUB161(auVar34 >> 0x17,0) & 1) << 2 |
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
              (ushort)(SUB161(auVar34 >> 0x77,0) & 1) << 0xe | (ushort)(auVar34[0xf] >> 7) << 0xf;
    }
    uVar5 = 0;
    if (uVar8 != 0) {
      for (; (uVar8 >> uVar5 & 1) == 0; uVar5 = uVar5 + 1) {
      }
    }
    uVar29 = uVar5 + uVar30 & uVar29;
    bVar26 = (byte)uVar3 & 0x7f;
    *(byte *)(uVar29 + *param_1) = bVar26;
    *(byte *)((uVar29 - 0x10 & param_1[3]) + (ulonglong)((uint)param_1[3] & 0xf) + 1 + *param_1) =
         bVar26;
    *(ulonglong *)(param_1[1] + uVar29 * 8) = *puVar31;
    pcVar27 = pcVar27 + 1;
    puVar31 = puVar31 + 1;
    cVar2 = *pcVar27;
    while (cVar2 < -1) {
      auVar35[0] = -(*pcVar27 < cVar9);
      auVar35[1] = -(pcVar27[1] < cVar10);
      auVar35[2] = -(pcVar27[2] < cVar11);
      auVar35[3] = -(pcVar27[3] < cVar12);
      auVar35[4] = -(pcVar27[4] < cVar13);
      auVar35[5] = -(pcVar27[5] < cVar14);
      auVar35[6] = -(pcVar27[6] < cVar15);
      auVar35[7] = -(pcVar27[7] < cVar16);
      auVar35[8] = -(pcVar27[8] < cVar17);
      auVar35[9] = -(pcVar27[9] < cVar18);
      auVar35[10] = -(pcVar27[10] < cVar19);
      auVar35[0xb] = -(pcVar27[0xb] < cVar20);
      auVar35[0xc] = -(pcVar27[0xc] < cVar21);
      auVar35[0xd] = -(pcVar27[0xd] < cVar22);
      auVar35[0xe] = -(pcVar27[0xe] < cVar23);
      auVar35[0xf] = -(pcVar27[0xf] < cVar24);
      uVar25 = (ushort)((ushort)(SUB161(auVar35 >> 7,0) & 1) |
                        (ushort)(SUB161(auVar35 >> 0xf,0) & 1) << 1 |
                        (ushort)(SUB161(auVar35 >> 0x17,0) & 1) << 2 |
                        (ushort)(SUB161(auVar35 >> 0x1f,0) & 1) << 3 |
                        (ushort)(SUB161(auVar35 >> 0x27,0) & 1) << 4 |
                        (ushort)(SUB161(auVar35 >> 0x2f,0) & 1) << 5 |
                        (ushort)(SUB161(auVar35 >> 0x37,0) & 1) << 6 |
                        (ushort)(SUB161(auVar35 >> 0x3f,0) & 1) << 7 |
                        (ushort)(SUB161(auVar35 >> 0x47,0) & 1) << 8 |
                        (ushort)(SUB161(auVar35 >> 0x4f,0) & 1) << 9 |
                        (ushort)(SUB161(auVar35 >> 0x57,0) & 1) << 10 |
                        (ushort)(SUB161(auVar35 >> 0x5f,0) & 1) << 0xb |
                        (ushort)(SUB161(auVar35 >> 0x67,0) & 1) << 0xc |
                        (ushort)(SUB161(auVar35 >> 0x6f,0) & 1) << 0xd |
                        (ushort)(SUB161(auVar35 >> 0x77,0) & 1) << 0xe |
                       (ushort)(auVar35[0xf] >> 7) << 0xf) + 1;
      uVar5 = 0;
      if (uVar25 != 0) {
        for (; (uVar25 >> uVar5 & 1) == 0; uVar5 = uVar5 + 1) {
        }
      }
      pcVar27 = pcVar27 + uVar5;
      puVar31 = puVar31 + uVar5;
      cVar2 = *pcVar27;
    }
  }
  param_1[2] = param_2[2];
  param_1[6] = param_1[6] - param_2[2];
  return param_1;
}

