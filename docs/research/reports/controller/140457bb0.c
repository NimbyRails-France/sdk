
undefined8 FUN_140457bb0(longlong param_1,ulonglong *param_2)

{
  longlong lVar1;
  char *pcVar2;
  char *pcVar3;
  code *pcVar4;
  undefined1 auVar5 [16];
  ushort uVar6;
  ushort uVar7;
  char cVar8;
  int iVar9;
  int iVar10;
  undefined8 uVar11;
  byte bVar12;
  undefined1 auVar13 [16];
  undefined1 auVar14 [16];
  longlong local_res8;

  auVar5._8_8_ = 0;
  auVar5._0_8_ = *param_2;
  lVar1 = SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar5,8) +
          SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar5,0);
  param_1 = (ulonglong)
            (((uint)((ulonglong)lVar1 >> 0x18) ^ (uint)((ulonglong)lVar1 >> 0x10) ^
             (uint)((ulonglong)lVar1 >> 8)) & 0xf) * 0x88 + param_1;
  iVar9 = _Mtx_lock(param_1);
  if (iVar9 != 0) {
    std::_Throw_Cpp_error(5);
    pcVar4 = (code *)swi(3);
    uVar11 = (*pcVar4)();
    return uVar11;
  }
  if (*(int *)(param_1 + 0x4c) == 0x7fffffff) {
    *(undefined4 *)(param_1 + 0x4c) = 0x7ffffffe;
    std::_Throw_Cpp_error(6);
    pcVar4 = (code *)swi(3);
    uVar11 = (*pcVar4)();
    return uVar11;
  }
  cVar8 = FUN_14045f780(param_1 + 0x50,param_2,lVar1,&local_res8);
  if (cVar8 != '\0') {
    pcVar2 = (char *)(*(longlong *)(param_1 + 0x50) + local_res8);
    if (local_res8 != *(longlong *)(param_1 + 0x68)) {
      FUN_1402ec870(local_res8 * 0x60 + 8 + *(longlong *)(param_1 + 0x58));
      *(longlong *)(param_1 + 0x60) = *(longlong *)(param_1 + 0x60) + -1;
      lVar1 = *(longlong *)(param_1 + 0x50);
      auVar14[0] = -(DAT_140aad620 == *pcVar2);
      auVar14[1] = -(UNK_140aad621 == pcVar2[1]);
      auVar14[2] = -(UNK_140aad622 == pcVar2[2]);
      auVar14[3] = -(UNK_140aad623 == pcVar2[3]);
      auVar14[4] = -(UNK_140aad624 == pcVar2[4]);
      auVar14[5] = -(UNK_140aad625 == pcVar2[5]);
      auVar14[6] = -(UNK_140aad626 == pcVar2[6]);
      auVar14[7] = -(UNK_140aad627 == pcVar2[7]);
      auVar14[8] = -(UNK_140aad628 == pcVar2[8]);
      auVar14[9] = -(UNK_140aad629 == pcVar2[9]);
      auVar14[10] = -(UNK_140aad62a == pcVar2[10]);
      auVar14[0xb] = -(UNK_140aad62b == pcVar2[0xb]);
      auVar14[0xc] = -(UNK_140aad62c == pcVar2[0xc]);
      auVar14[0xd] = -(UNK_140aad62d == pcVar2[0xd]);
      auVar14[0xe] = -(UNK_140aad62e == pcVar2[0xe]);
      auVar14[0xf] = -(UNK_140aad62f == pcVar2[0xf]);
      uVar6 = (ushort)(SUB161(auVar14 >> 7,0) & 1) | (ushort)(SUB161(auVar14 >> 0xf,0) & 1) << 1 |
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
              (ushort)(SUB161(auVar14 >> 0x77,0) & 1) << 0xe | (ushort)(auVar14[0xf] >> 7) << 0xf;
      pcVar3 = (char *)(((ulonglong)(pcVar2 + (-0x10 - lVar1)) & *(ulonglong *)(param_1 + 0x68)) +
                       lVar1);
      auVar13[0] = -(*pcVar3 == DAT_140aad620);
      auVar13[1] = -(pcVar3[1] == UNK_140aad621);
      auVar13[2] = -(pcVar3[2] == UNK_140aad622);
      auVar13[3] = -(pcVar3[3] == UNK_140aad623);
      auVar13[4] = -(pcVar3[4] == UNK_140aad624);
      auVar13[5] = -(pcVar3[5] == UNK_140aad625);
      auVar13[6] = -(pcVar3[6] == UNK_140aad626);
      auVar13[7] = -(pcVar3[7] == UNK_140aad627);
      auVar13[8] = -(pcVar3[8] == UNK_140aad628);
      auVar13[9] = -(pcVar3[9] == UNK_140aad629);
      auVar13[10] = -(pcVar3[10] == UNK_140aad62a);
      auVar13[0xb] = -(pcVar3[0xb] == UNK_140aad62b);
      auVar13[0xc] = -(pcVar3[0xc] == UNK_140aad62c);
      auVar13[0xd] = -(pcVar3[0xd] == UNK_140aad62d);
      auVar13[0xe] = -(pcVar3[0xe] == UNK_140aad62e);
      auVar13[0xf] = -(pcVar3[0xf] == UNK_140aad62f);
      uVar7 = (ushort)(SUB161(auVar13 >> 7,0) & 1) | (ushort)(SUB161(auVar13 >> 0xf,0) & 1) << 1 |
              (ushort)(SUB161(auVar13 >> 0x17,0) & 1) << 2 |
              (ushort)(SUB161(auVar13 >> 0x1f,0) & 1) << 3 |
              (ushort)(SUB161(auVar13 >> 0x27,0) & 1) << 4 |
              (ushort)(SUB161(auVar13 >> 0x2f,0) & 1) << 5 |
              (ushort)(SUB161(auVar13 >> 0x37,0) & 1) << 6 |
              (ushort)(SUB161(auVar13 >> 0x3f,0) & 1) << 7 |
              (ushort)(SUB161(auVar13 >> 0x47,0) & 1) << 8 |
              (ushort)(SUB161(auVar13 >> 0x4f,0) & 1) << 9 |
              (ushort)(SUB161(auVar13 >> 0x57,0) & 1) << 10 |
              (ushort)(SUB161(auVar13 >> 0x5f,0) & 1) << 0xb |
              (ushort)(SUB161(auVar13 >> 0x67,0) & 1) << 0xc |
              (ushort)(SUB161(auVar13 >> 0x6f,0) & 1) << 0xd |
              (ushort)(SUB161(auVar13 >> 0x77,0) & 1) << 0xe | (ushort)(auVar13[0xf] >> 7) << 0xf;
      if ((uVar7 == 0) || (uVar6 == 0)) {
LAB_140457ce0:
        bVar12 = 0;
      }
      else {
        iVar9 = 0;
        if (uVar6 != 0) {
          for (; (uVar6 >> iVar9 & 1) == 0; iVar9 = iVar9 + 1) {
          }
        }
        iVar10 = 0x1f;
        if (uVar7 != 0) {
          for (; ((uint)uVar7 << 0x10) >> iVar10 == 0; iVar10 = iVar10 + -1) {
          }
        }
        if (uVar7 == 0) {
          iVar10 = 0x20;
        }
        else {
          iVar10 = 0x1f - iVar10;
        }
        if (0xf < (uint)(iVar10 + iVar9)) goto LAB_140457ce0;
        bVar12 = 1;
      }
      uVar11 = 1;
      cVar8 = -2;
      if (bVar12 != 0) {
        cVar8 = -0x80;
      }
      *pcVar2 = cVar8;
      *(char *)((*(ulonglong *)(param_1 + 0x68) & (ulonglong)(pcVar2 + (-0x10 - lVar1))) +
                (ulonglong)((uint)*(ulonglong *)(param_1 + 0x68) & 0xf) + 1 +
               *(longlong *)(param_1 + 0x50)) = cVar8;
      *(longlong *)(param_1 + 0x80) = *(longlong *)(param_1 + 0x80) + (ulonglong)bVar12;
      goto LAB_140457d22;
    }
  }
  uVar11 = 0;
LAB_140457d22:
  _Mtx_unlock(param_1);
  return uVar11;
}
