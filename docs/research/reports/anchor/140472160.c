
longlong * FUN_140472160(longlong *param_1,longlong *param_2)

{
  char *pcVar1;
  ulonglong *puVar2;
  ulonglong uVar3;
  longlong lVar4;
  uint uVar5;
  longlong lVar6;
  undefined1 auVar7 [16];
  ushort uVar8;
  uint uVar9;
  byte bVar10;
  ulonglong uVar11;
  longlong lVar12;
  ulonglong uVar13;
  undefined1 auVar14 [16];
  undefined1 auVar15 [16];
  undefined1 auVar16 [16];
  char cVar17;
  char cVar18;
  char cVar19;
  char cVar20;
  char cVar21;
  char cVar22;
  char cVar23;
  char cVar24;
  char cVar25;
  char cVar26;
  char cVar27;
  char cVar28;
  char cVar29;
  char cVar30;
  char cVar31;
  char cVar32;
  char *local_38;
  ulonglong *local_30;
  
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
      bVar10 = 0;
    }
    else {
      bVar10 = 0x3f - (char)lVar6;
    }
    if (0xffffffffffffffffU >> (bVar10 & 0x3f) != 0) {
      FUN_140474800(param_1);
    }
  }
  local_30 = (ulonglong *)param_2[1];
  local_38 = (char *)*param_2;
  cVar17 = *local_38;
  while (cVar17 < -1) {
    auVar14[0] = -(*local_38 < DAT_140aad760);
    auVar14[1] = -(local_38[1] < UNK_140aad761);
    auVar14[2] = -(local_38[2] < UNK_140aad762);
    auVar14[3] = -(local_38[3] < UNK_140aad763);
    auVar14[4] = -(local_38[4] < UNK_140aad764);
    auVar14[5] = -(local_38[5] < UNK_140aad765);
    auVar14[6] = -(local_38[6] < UNK_140aad766);
    auVar14[7] = -(local_38[7] < UNK_140aad767);
    auVar14[8] = -(local_38[8] < UNK_140aad768);
    auVar14[9] = -(local_38[9] < UNK_140aad769);
    auVar14[10] = -(local_38[10] < UNK_140aad76a);
    auVar14[0xb] = -(local_38[0xb] < UNK_140aad76b);
    auVar14[0xc] = -(local_38[0xc] < UNK_140aad76c);
    auVar14[0xd] = -(local_38[0xd] < UNK_140aad76d);
    auVar14[0xe] = -(local_38[0xe] < UNK_140aad76e);
    auVar14[0xf] = -(local_38[0xf] < UNK_140aad76f);
    uVar9 = (ushort)((ushort)(SUB161(auVar14 >> 7,0) & 1) |
                     (ushort)(SUB161(auVar14 >> 0xf,0) & 1) << 1 |
                     (ushort)(SUB161(auVar14 >> 0x17,0) & 1) << 2 |
                     (ushort)(SUB161(auVar14 >> 0x1f,0) & 1) << 3 |
                     (ushort)(SUB161(auVar14 >> 0x27,0) & 1) << 4 |
                     (ushort)(SUB161(auVar14 >> 0x2f,0) & 1) << 5 |
                     (ushort)(SUB161(auVar14 >> 0x37,0) & 1) << 6 |
                     (ushort)(SUB161(auVar14 >> 0x3f,0) & 1) << 7 |
                     (ushort)(SUB161(auVar14 >> 0x47,0) & 1) << 8 |
                     (ushort)(SUB161(auVar14 >> 0x4f,0) & 1) << 9 |
                     (ushort)(SUB161(auVar14 >> 0x57,0) & 1) << 10 |
                     (ushort)(SUB161(auVar14 >> 0x5f,0) & 1) << 0xb |
                     (ushort)(SUB161(auVar14 >> 0x67,0) & 1) << 0xc |
                     (ushort)(SUB161(auVar14 >> 0x6f,0) & 1) << 0xd |
                     (ushort)(SUB161(auVar14 >> 0x77,0) & 1) << 0xe |
                    (ushort)(auVar14[0xf] >> 7) << 0xf) + 1;
    uVar5 = 0;
    if (uVar9 != 0) {
      for (; (uVar9 >> uVar5 & 1) == 0; uVar5 = uVar5 + 1) {
      }
    }
    local_38 = local_38 + uVar5;
    local_30 = local_30 + (ulonglong)uVar5 * 4;
    cVar17 = *local_38;
  }
  lVar6 = *param_2;
  lVar4 = param_2[3];
  cVar17 = DAT_140aad760;
  cVar18 = UNK_140aad761;
  cVar19 = UNK_140aad762;
  cVar20 = UNK_140aad763;
  cVar21 = UNK_140aad764;
  cVar22 = UNK_140aad765;
  cVar23 = UNK_140aad766;
  cVar24 = UNK_140aad767;
  cVar25 = UNK_140aad768;
  cVar26 = UNK_140aad769;
  cVar27 = UNK_140aad76a;
  cVar28 = UNK_140aad76b;
  cVar29 = UNK_140aad76c;
  cVar30 = UNK_140aad76d;
  cVar31 = UNK_140aad76e;
  cVar32 = UNK_140aad76f;
  while (local_38 != (char *)(lVar6 + lVar4)) {
    auVar7._8_8_ = 0;
    auVar7._0_8_ = ((local_30[1] * -0x395b586ca42e166b >> 0x2f ^ local_30[1] * -0x395b586ca42e166b)
                    * -0x395b586ca42e166b ^ *local_30) * -0x395b586ca42e166b + 0xe6546b64;
    uVar3 = SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar7,8) +
            SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar7,0);
    uVar13 = param_1[3];
    uVar11 = uVar3 >> 7 & uVar13;
    pcVar1 = (char *)(*param_1 + uVar11);
    auVar15[0] = -(*pcVar1 < cVar17);
    auVar15[1] = -(pcVar1[1] < cVar18);
    auVar15[2] = -(pcVar1[2] < cVar19);
    auVar15[3] = -(pcVar1[3] < cVar20);
    auVar15[4] = -(pcVar1[4] < cVar21);
    auVar15[5] = -(pcVar1[5] < cVar22);
    auVar15[6] = -(pcVar1[6] < cVar23);
    auVar15[7] = -(pcVar1[7] < cVar24);
    auVar15[8] = -(pcVar1[8] < cVar25);
    auVar15[9] = -(pcVar1[9] < cVar26);
    auVar15[10] = -(pcVar1[10] < cVar27);
    auVar15[0xb] = -(pcVar1[0xb] < cVar28);
    auVar15[0xc] = -(pcVar1[0xc] < cVar29);
    auVar15[0xd] = -(pcVar1[0xd] < cVar30);
    auVar15[0xe] = -(pcVar1[0xe] < cVar31);
    auVar15[0xf] = -(pcVar1[0xf] < cVar32);
    uVar8 = (ushort)(SUB161(auVar15 >> 7,0) & 1) | (ushort)(SUB161(auVar15 >> 0xf,0) & 1) << 1 |
            (ushort)(SUB161(auVar15 >> 0x17,0) & 1) << 2 |
            (ushort)(SUB161(auVar15 >> 0x1f,0) & 1) << 3 |
            (ushort)(SUB161(auVar15 >> 0x27,0) & 1) << 4 |
            (ushort)(SUB161(auVar15 >> 0x2f,0) & 1) << 5 |
            (ushort)(SUB161(auVar15 >> 0x37,0) & 1) << 6 |
            (ushort)(SUB161(auVar15 >> 0x3f,0) & 1) << 7 |
            (ushort)(SUB161(auVar15 >> 0x47,0) & 1) << 8 |
            (ushort)(SUB161(auVar15 >> 0x4f,0) & 1) << 9 |
            (ushort)(SUB161(auVar15 >> 0x57,0) & 1) << 10 |
            (ushort)(SUB161(auVar15 >> 0x5f,0) & 1) << 0xb |
            (ushort)(SUB161(auVar15 >> 0x67,0) & 1) << 0xc |
            (ushort)(SUB161(auVar15 >> 0x6f,0) & 1) << 0xd |
            (ushort)(SUB161(auVar15 >> 0x77,0) & 1) << 0xe | (ushort)(auVar15[0xf] >> 7) << 0xf;
    lVar12 = 0;
    while (uVar8 == 0) {
      lVar12 = lVar12 + 0x10;
      uVar11 = uVar11 + lVar12 & uVar13;
      pcVar1 = (char *)(*param_1 + uVar11);
      auVar16[0] = -(*pcVar1 < cVar17);
      auVar16[1] = -(pcVar1[1] < cVar18);
      auVar16[2] = -(pcVar1[2] < cVar19);
      auVar16[3] = -(pcVar1[3] < cVar20);
      auVar16[4] = -(pcVar1[4] < cVar21);
      auVar16[5] = -(pcVar1[5] < cVar22);
      auVar16[6] = -(pcVar1[6] < cVar23);
      auVar16[7] = -(pcVar1[7] < cVar24);
      auVar16[8] = -(pcVar1[8] < cVar25);
      auVar16[9] = -(pcVar1[9] < cVar26);
      auVar16[10] = -(pcVar1[10] < cVar27);
      auVar16[0xb] = -(pcVar1[0xb] < cVar28);
      auVar16[0xc] = -(pcVar1[0xc] < cVar29);
      auVar16[0xd] = -(pcVar1[0xd] < cVar30);
      auVar16[0xe] = -(pcVar1[0xe] < cVar31);
      auVar16[0xf] = -(pcVar1[0xf] < cVar32);
      uVar8 = (ushort)(SUB161(auVar16 >> 7,0) & 1) | (ushort)(SUB161(auVar16 >> 0xf,0) & 1) << 1 |
              (ushort)(SUB161(auVar16 >> 0x17,0) & 1) << 2 |
              (ushort)(SUB161(auVar16 >> 0x1f,0) & 1) << 3 |
              (ushort)(SUB161(auVar16 >> 0x27,0) & 1) << 4 |
              (ushort)(SUB161(auVar16 >> 0x2f,0) & 1) << 5 |
              (ushort)(SUB161(auVar16 >> 0x37,0) & 1) << 6 |
              (ushort)(SUB161(auVar16 >> 0x3f,0) & 1) << 7 |
              (ushort)(SUB161(auVar16 >> 0x47,0) & 1) << 8 |
              (ushort)(SUB161(auVar16 >> 0x4f,0) & 1) << 9 |
              (ushort)(SUB161(auVar16 >> 0x57,0) & 1) << 10 |
              (ushort)(SUB161(auVar16 >> 0x5f,0) & 1) << 0xb |
              (ushort)(SUB161(auVar16 >> 0x67,0) & 1) << 0xc |
              (ushort)(SUB161(auVar16 >> 0x6f,0) & 1) << 0xd |
              (ushort)(SUB161(auVar16 >> 0x77,0) & 1) << 0xe | (ushort)(auVar16[0xf] >> 7) << 0xf;
    }
    uVar5 = 0;
    if (uVar8 != 0) {
      for (; (uVar8 >> uVar5 & 1) == 0; uVar5 = uVar5 + 1) {
      }
    }
    uVar13 = uVar5 + uVar11 & uVar13;
    bVar10 = (byte)uVar3 & 0x7f;
    *(byte *)(uVar13 + *param_1) = bVar10;
    *(byte *)((uVar13 - 0x10 & param_1[3]) + (ulonglong)((uint)param_1[3] & 0xf) + 1 + *param_1) =
         bVar10;
    lVar12 = param_1[1];
    uVar3 = local_30[1];
    puVar2 = (ulonglong *)(uVar13 * 0x20 + lVar12);
    *puVar2 = *local_30;
    puVar2[1] = uVar3;
    uVar3 = local_30[3];
    puVar2 = (ulonglong *)(uVar13 * 0x20 + 0x10 + lVar12);
    *puVar2 = local_30[2];
    puVar2[1] = uVar3;
    FUN_140472100(&local_38);
  }
  param_1[2] = param_2[2];
  param_1[6] = param_1[6] - param_2[2];
  return param_1;
}

