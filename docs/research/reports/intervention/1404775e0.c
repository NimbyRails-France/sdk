
int FUN_1404775e0(longlong *param_1)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  longlong lVar4;
  longlong lVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  int iVar9;
  uint uVar10;
  undefined1 local_res8 [3];
  byte local_resb;
  int local_res10 [2];
  int local_38;
  int local_34;
  int local_30;

  lVar4 = *param_1 * 100 + param_1[1];
  lVar5 = lVar4 * 10000;
  iVar6 = (int)(lVar5 / 86400000000);
  if (((longlong)iVar6 * 86400000000 + lVar4 * -10000 != 0) &&
     (lVar5 <= (longlong)iVar6 * 86400000000)) {
    iVar6 = iVar6 + -1;
  }
  iVar7 = iVar6 + 0xafa6c;
  iVar2 = (int)((iVar7 >> 0x1f & 0xfffdc550U) + 3 + iVar7 * 4) / 0x23ab1;
  iVar7 = iVar7 - (iVar2 * 0x23ab1 >> 2);
  uVar8 = (iVar7 + 1) * 0x166db;
  iVar7 = iVar7 - ((uVar8 >> 0x19) * 0x5b5 >> 2);
  uVar10 = iVar7 * 0x217 + 0x14dU >> 0xe;
  iVar9 = -9;
  if (uVar10 < 10) {
    iVar9 = 3;
  }
  uVar1 = iVar9 + uVar10;
  iVar9 = (int)(short)((ushort)(uVar1 < 3) + (short)iVar2 * 100 + (ushort)(uVar8 >> 0x19)) -
          (uint)((byte)uVar1 < 3);
  iVar2 = iVar9;
  if (iVar9 < 0) {
    iVar2 = iVar9 + -99;
  }
  iVar3 = 9;
  if (2 < (byte)uVar1) {
    iVar3 = -3;
  }
  local_res10[0] =
       (((iVar2 / 100 >> 2) +
        ((iVar7 - (uVar10 * 0x3d3 + 0x13 >> 5)) + 1 & 0xff) +
        ((iVar3 + (uVar1 & 0xff)) * 0x3d3 + 0x13 >> 5) + (iVar9 * 0x5b5 >> 2)) - iVar2 / 100) +
       -0xafa6d;
  FUN_140477000(local_res8,local_res10);
  FUN_140483380(&local_38,lVar5 + (longlong)iVar6 * -86400000000);
  uVar8 = local_resb + 6;
  return ((local_38 + (uVar8 + ((uVar8 - uVar8 / 7 >> 1) + uVar8 / 7 >> 2) * -7) * 0x18) * 0x3c +
         local_34) * 0x3c + local_30;
}


// ref 140c25064 caller none
// ref 14034800f caller 140347e20
// ref 140477dbd caller 140477ce0
// ref 14046e4b8 caller 14046e3b0
// ref 14046e69b caller 14046e5e0
// ref 14063dc86 caller 14063da50
// ref 1406aaa23 caller 1406aa960
// ref 1406aaa33 caller 1406aa960
// ref 1406aaa83 caller 1406aa960
