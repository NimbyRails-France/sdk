// Candidate VA 14045f860; RVA 0x45f860
// Ghidra inferred prototype: undefined FUN_14045f860()

longlong *
FUN_14045f860(longlong param_1,longlong *param_2,longlong *param_3,ulonglong param_4,
             longlong *param_5)

{
  char *pcVar1;
  longlong lVar2;
  ulonglong uVar3;
  uint uVar4;
  code *pcVar5;
  ushort uVar6;
  undefined2 uVar7;
  int iVar8;
  uint uVar9;
  longlong *plVar10;
  ulonglong uVar11;
  longlong lVar12;
  ulonglong uVar13;
  longlong lVar14;
  undefined1 auVar15 [16];
  undefined1 auVar16 [16];
  char cVar17;
  char cVar19;
  char cVar20;
  uint uVar18;
  char cVar21;
  longlong lStack_30;
  
  lVar12 = (ulonglong)
           (((uint)(param_4 >> 0x18) ^ (uint)(param_4 >> 0x10) ^ (uint)(param_4 >> 8)) & 0xf) * 0x88
           + param_1;
  iVar8 = _Mtx_lock(lVar12);
  if (iVar8 != 0) {
    std::_Throw_Cpp_error(5);
    pcVar5 = (code *)swi(3);
    plVar10 = (longlong *)(*pcVar5)();
    return plVar10;
  }
  if (*(int *)(lVar12 + 0x4c) == 0x7fffffff) {
    *(undefined4 *)(lVar12 + 0x4c) = 0x7ffffffe;
    std::_Throw_Cpp_error(6);
    pcVar5 = (code *)swi(3);
    plVar10 = (longlong *)(*pcVar5)();
    return plVar10;
  }
  lVar14 = param_5[1];
  lVar2 = *param_5;
  *param_5 = lVar12;
  *(undefined1 *)(param_5 + 1) = 1;
  if ((char)lVar14 != '\0') {
    _Mtx_unlock(lVar2);
  }
  uVar3 = *(ulonglong *)(lVar12 + 0x68);
  lVar2 = *(longlong *)(lVar12 + 0x50);
  lVar14 = 0;
  uVar13 = param_4 >> 7;
  uVar7 = CONCAT11((char)param_4,(char)param_4);
  uVar18 = CONCAT22(uVar7,uVar7) & 0x7f7f7f7f;
  do {
    uVar13 = uVar13 & uVar3;
    pcVar1 = (char *)(uVar13 + lVar2);
    cVar17 = (char)uVar18;
    auVar15[0] = -(*pcVar1 == cVar17);
    cVar19 = (char)(uVar18 >> 8);
    auVar15[1] = -(pcVar1[1] == cVar19);
    cVar20 = (char)(uVar18 >> 0x10);
    auVar15[2] = -(pcVar1[2] == cVar20);
    cVar21 = (char)(uVar18 >> 0x18);
    auVar15[3] = -(pcVar1[3] == cVar21);
    auVar15[4] = -(pcVar1[4] == cVar17);
    auVar15[5] = -(pcVar1[5] == cVar19);
    auVar15[6] = -(pcVar1[6] == cVar20);
    auVar15[7] = -(pcVar1[7] == cVar21);
    auVar15[8] = -(pcVar1[8] == cVar17);
    auVar15[9] = -(pcVar1[9] == cVar19);
    auVar15[10] = -(pcVar1[10] == cVar20);
    auVar15[0xb] = -(pcVar1[0xb] == cVar21);
    auVar15[0xc] = -(pcVar1[0xc] == cVar17);
    auVar15[0xd] = -(pcVar1[0xd] == cVar19);
    auVar15[0xe] = -(pcVar1[0xe] == cVar20);
    auVar15[0xf] = -(pcVar1[0xf] == cVar21);
    uVar6 = (ushort)(SUB161(auVar15 >> 7,0) & 1) | (ushort)(SUB161(auVar15 >> 0xf,0) & 1) << 1 |
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
    uVar9 = (uint)uVar6;
    if (uVar6 != 0) {
      do {
        uVar4 = 0;
        if (uVar9 != 0) {
          for (; (uVar9 >> uVar4 & 1) == 0; uVar4 = uVar4 + 1) {
          }
        }
        uVar11 = uVar4 + uVar13 & uVar3;
        if (*(longlong *)(uVar11 * 0x20 + *(longlong *)(lVar12 + 0x58)) == *param_3) {
          lStack_30 = uVar11 * 0x20 + *(longlong *)(lVar12 + 0x58);
          goto LAB_14045f9af;
        }
        uVar9 = uVar9 & uVar9 - 1;
      } while (uVar9 != 0);
    }
    auVar16[0] = -(DAT_140aad620 == *pcVar1);
    auVar16[1] = -(UNK_140aad621 == pcVar1[1]);
    auVar16[2] = -(UNK_140aad622 == pcVar1[2]);
    auVar16[3] = -(UNK_140aad623 == pcVar1[3]);
    auVar16[4] = -(UNK_140aad624 == pcVar1[4]);
    auVar16[5] = -(UNK_140aad625 == pcVar1[5]);
    auVar16[6] = -(UNK_140aad626 == pcVar1[6]);
    auVar16[7] = -(UNK_140aad627 == pcVar1[7]);
    auVar16[8] = -(UNK_140aad628 == pcVar1[8]);
    auVar16[9] = -(UNK_140aad629 == pcVar1[9]);
    auVar16[10] = -(UNK_140aad62a == pcVar1[10]);
    auVar16[0xb] = -(UNK_140aad62b == pcVar1[0xb]);
    auVar16[0xc] = -(UNK_140aad62c == pcVar1[0xc]);
    auVar16[0xd] = -(UNK_140aad62d == pcVar1[0xd]);
    auVar16[0xe] = -(UNK_140aad62e == pcVar1[0xe]);
    auVar16[0xf] = -(UNK_140aad62f == pcVar1[0xf]);
    uVar11 = uVar3;
    if ((((((((((((((((SUB161(auVar16 >> 7,0) & 1) != 0 || (SUB161(auVar16 >> 0xf,0) & 1) != 0) ||
                    (SUB161(auVar16 >> 0x17,0) & 1) != 0) || (SUB161(auVar16 >> 0x1f,0) & 1) != 0)
                  || (SUB161(auVar16 >> 0x27,0) & 1) != 0) || (SUB161(auVar16 >> 0x2f,0) & 1) != 0)
                || (SUB161(auVar16 >> 0x37,0) & 1) != 0) || (SUB161(auVar16 >> 0x3f,0) & 1) != 0) ||
              (SUB161(auVar16 >> 0x47,0) & 1) != 0) || (SUB161(auVar16 >> 0x4f,0) & 1) != 0) ||
            (SUB161(auVar16 >> 0x57,0) & 1) != 0) || (SUB161(auVar16 >> 0x5f,0) & 1) != 0) ||
          (SUB161(auVar16 >> 0x67,0) & 1) != 0) || (SUB161(auVar16 >> 0x6f,0) & 1) != 0) ||
        (SUB161(auVar16 >> 0x77,0) & 1) != 0) || auVar16[0xf] < '\0') {
LAB_14045f9af:
      if (uVar11 + lVar2 == lVar2 + uVar3) {
        *param_2 = 0;
        param_2[1] = 0;
        param_2[2] = 0;
        param_2[4] = 0;
      }
      else {
        *param_2 = lVar12;
        param_2[1] = param_1 + 0x880;
        param_2[2] = uVar11 + lVar2;
        param_2[3] = lStack_30;
        param_2[4] = 0;
        if (lVar12 != 0) {
          param_2[4] = *(longlong *)(lVar12 + 0x68) + *(longlong *)(lVar12 + 0x50);
          param_2[5] = lStack_30;
        }
      }
      return param_2;
    }
    lVar14 = lVar14 + 0x10;
    uVar13 = uVar13 + lVar14;
  } while( true );
}


// Incoming references
// 0xc24104 DATA caller none
// 0xb09548 DATA caller none
// 0xb0955c DATA caller none
// 0x442fed UNCONDITIONAL_CALL caller 140442f40
// 0x45163a UNCONDITIONAL_CALL caller 1404515c0
// 0x4584b6 UNCONDITIONAL_CALL caller 140458460
// 0x47bd5b UNCONDITIONAL_CALL caller 14047a5f0
// 0x457f5e UNCONDITIONAL_CALL caller 140457f00
// 0x452646 UNCONDITIONAL_CALL caller 140452480
// 0x452933 UNCONDITIONAL_CALL caller 140452860
// 0x452d19 UNCONDITIONAL_CALL caller 140452c30
