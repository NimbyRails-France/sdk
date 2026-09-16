
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

longlong FUN_140456ab0(longlong *param_1,ulonglong *param_2)

{
  ulonglong uVar1;
  uint uVar2;
  undefined1 auVar3 [16];
  ushort uVar4;
  uint uVar5;
  ulonglong uVar6;
  longlong lVar7;
  byte bVar8;
  ulonglong uVar9;
  ulonglong *puVar10;
  undefined1 auVar11 [16];
  undefined1 auVar12 [16];
  char cVar13;
  char cVar15;
  char cVar16;
  uint uVar14;
  char cVar17;

  lVar7 = 0;
  auVar3._8_8_ = 0;
  auVar3._0_8_ = *param_2;
  uVar1 = SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar3,8) +
          SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar3,0);
  bVar8 = (byte)uVar1;
  uVar9 = uVar1 >> 7;
  uVar14 = CONCAT22(CONCAT11(bVar8,bVar8),CONCAT11(bVar8,bVar8)) & 0x7f7f7f7f;
  while( true ) {
    uVar9 = uVar9 & param_1[3];
    auVar3 = *(undefined1 (*) [16])(uVar9 + *param_1);
    cVar13 = (char)uVar14;
    auVar11[0] = -(auVar3[0] == cVar13);
    cVar15 = (char)(uVar14 >> 8);
    auVar11[1] = -(auVar3[1] == cVar15);
    cVar16 = (char)(uVar14 >> 0x10);
    auVar11[2] = -(auVar3[2] == cVar16);
    cVar17 = (char)(uVar14 >> 0x18);
    auVar11[3] = -(auVar3[3] == cVar17);
    auVar11[4] = -(auVar3[4] == cVar13);
    auVar11[5] = -(auVar3[5] == cVar15);
    auVar11[6] = -(auVar3[6] == cVar16);
    auVar11[7] = -(auVar3[7] == cVar17);
    auVar11[8] = -(auVar3[8] == cVar13);
    auVar11[9] = -(auVar3[9] == cVar15);
    auVar11[10] = -(auVar3[10] == cVar16);
    auVar11[0xb] = -(auVar3[0xb] == cVar17);
    auVar11[0xc] = -(auVar3[0xc] == cVar13);
    auVar11[0xd] = -(auVar3[0xd] == cVar15);
    auVar11[0xe] = -(auVar3[0xe] == cVar16);
    auVar11[0xf] = -(auVar3[0xf] == cVar17);
    uVar4 = (ushort)(SUB161(auVar11 >> 7,0) & 1) | (ushort)(SUB161(auVar11 >> 0xf,0) & 1) << 1 |
            (ushort)(SUB161(auVar11 >> 0x17,0) & 1) << 2 |
            (ushort)(SUB161(auVar11 >> 0x1f,0) & 1) << 3 |
            (ushort)(SUB161(auVar11 >> 0x27,0) & 1) << 4 |
            (ushort)(SUB161(auVar11 >> 0x2f,0) & 1) << 5 |
            (ushort)(SUB161(auVar11 >> 0x37,0) & 1) << 6 |
            (ushort)(SUB161(auVar11 >> 0x3f,0) & 1) << 7 |
            (ushort)(SUB161(auVar11 >> 0x47,0) & 1) << 8 |
            (ushort)(SUB161(auVar11 >> 0x4f,0) & 1) << 9 |
            (ushort)(SUB161(auVar11 >> 0x57,0) & 1) << 10 |
            (ushort)(SUB161(auVar11 >> 0x5f,0) & 1) << 0xb |
            (ushort)(SUB161(auVar11 >> 0x67,0) & 1) << 0xc |
            (ushort)(SUB161(auVar11 >> 0x6f,0) & 1) << 0xd |
            (ushort)(SUB161(auVar11 >> 0x77,0) & 1) << 0xe | (ushort)(auVar11[0xf] >> 7) << 0xf;
    uVar5 = (uint)uVar4;
    if (uVar4 != 0) {
      do {
        uVar2 = 0;
        if (uVar5 != 0) {
          for (; (uVar5 >> uVar2 & 1) == 0; uVar2 = uVar2 + 1) {
          }
        }
        uVar6 = uVar2 + uVar9 & param_1[3];
        if (*(ulonglong *)(param_1[1] + uVar6 * 0x10) == *param_2) goto LAB_140456bbf;
        uVar5 = uVar5 & uVar5 - 1;
      } while (uVar5 != 0);
    }
    auVar12[0] = -(DAT_140aad620 == auVar3[0]);
    auVar12[1] = -(DAT_140aad620._1_1_ == auVar3[1]);
    auVar12[2] = -(DAT_140aad620._2_1_ == auVar3[2]);
    auVar12[3] = -(DAT_140aad620._3_1_ == auVar3[3]);
    auVar12[4] = -(DAT_140aad620._4_1_ == auVar3[4]);
    auVar12[5] = -(DAT_140aad620._5_1_ == auVar3[5]);
    auVar12[6] = -(DAT_140aad620._6_1_ == auVar3[6]);
    auVar12[7] = -(DAT_140aad620._7_1_ == auVar3[7]);
    auVar12[8] = -(DAT_140aad620._8_1_ == auVar3[8]);
    auVar12[9] = -(DAT_140aad620._9_1_ == auVar3[9]);
    auVar12[10] = -(DAT_140aad620._10_1_ == auVar3[10]);
    auVar12[0xb] = -(DAT_140aad620._11_1_ == auVar3[0xb]);
    auVar12[0xc] = -(DAT_140aad620._12_1_ == auVar3[0xc]);
    auVar12[0xd] = -(DAT_140aad620._13_1_ == auVar3[0xd]);
    auVar12[0xe] = -(DAT_140aad620._14_1_ == auVar3[0xe]);
    auVar12[0xf] = -(DAT_140aad620._15_1_ == auVar3[0xf]);
    if ((((((((((((((((SUB161(auVar12 >> 7,0) & 1) != 0 || (SUB161(auVar12 >> 0xf,0) & 1) != 0) ||
                    (SUB161(auVar12 >> 0x17,0) & 1) != 0) || (SUB161(auVar12 >> 0x1f,0) & 1) != 0)
                  || (SUB161(auVar12 >> 0x27,0) & 1) != 0) || (SUB161(auVar12 >> 0x2f,0) & 1) != 0)
                || (SUB161(auVar12 >> 0x37,0) & 1) != 0) || (SUB161(auVar12 >> 0x3f,0) & 1) != 0) ||
              (SUB161(auVar12 >> 0x47,0) & 1) != 0) || (SUB161(auVar12 >> 0x4f,0) & 1) != 0) ||
            (SUB161(auVar12 >> 0x57,0) & 1) != 0) || (SUB161(auVar12 >> 0x5f,0) & 1) != 0) ||
          (SUB161(auVar12 >> 0x67,0) & 1) != 0) || (SUB161(auVar12 >> 0x6f,0) & 1) != 0) ||
        (SUB161(auVar12 >> 0x77,0) & 1) != 0) || auVar12[0xf] < '\0') break;
    lVar7 = lVar7 + 0x10;
    uVar9 = uVar9 + lVar7;
  }
  uVar6 = FUN_1403df900(param_1,uVar1);
  puVar10 = (ulonglong *)(uVar6 * 0x10 + param_1[1]);
  *puVar10 = *param_2;
  puVar10[1] = 0;
  *(byte *)(uVar6 + *param_1) = bVar8 & 0x7f;
  *(byte *)((uVar6 - 0x10 & param_1[3]) + (ulonglong)((uint)param_1[3] & 0xf) + 1 + *param_1) =
       bVar8 & 0x7f;
LAB_140456bbf:
  return param_1[1] + 8 + uVar6 * 0x10;
}
