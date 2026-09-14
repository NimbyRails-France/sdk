
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 FUN_1407272d0(longlong param_1,undefined8 param_2)

{
  undefined8 *******pppppppuVar1;
  char cVar2;
  ushort uVar3;
  code *pcVar4;
  longlong lVar5;
  undefined2 *_Memory;
  undefined2 *puVar6;
  float fVar7;
  undefined1 uVar8;
  ushort uVar9;
  int iVar10;
  undefined4 uVar11;
  longlong *plVar12;
  undefined8 uVar13;
  undefined8 *puVar14;
  int *******pppppppiVar15;
  int *piVar16;
  longlong lVar17;
  longlong *plVar18;
  float *pfVar19;
  undefined8 *******_Buf1;
  longlong lVar20;
  longlong *plVar21;
  undefined8 *******pppppppuVar22;
  ulonglong uVar23;
  ushort uVar24;
  size_t sVar25;
  uint uVar26;
  longlong *plVar27;
  longlong lVar28;
  undefined2 *puVar29;
  void *pvVar30;
  ushort uVar31;
  ulonglong uVar32;
  ushort uVar33;
  undefined2 uVar34;
  ushort uVar35;
  ulonglong uVar36;
  double dVar37;
  LARGE_INTEGER local_res18;
  LARGE_INTEGER local_res20;
  undefined8 local_c58;
  undefined8 uStack_c50;
  undefined8 local_c48;
  ulonglong uStack_c40;
  ushort local_c38 [2];
  uint local_c34;
  char local_c28;
  float local_c20;
  void *local_c08;
  void *local_c00;
  undefined8 local_bc8;
  undefined8 ******local_bb8 [2];
  size_t local_ba8;
  ulonglong local_ba0;
  int ******local_b98 [2];
  longlong local_b88;
  ulonglong local_b80;
  char local_b78;
  undefined1 local_b70 [16];
  undefined8 local_b60;
  undefined8 uStack_b58;
  undefined4 local_b50;
  undefined4 local_b4c;
  undefined4 local_b48;
  float local_b44;
  undefined1 local_b40 [16];
  undefined8 *local_b30;
  undefined1 local_b08 [48];
  void *local_ad8;
  void *local_ad0;
  longlong local_a88;
  LARGE_INTEGER local_a80;
  undefined1 local_a78;
  undefined1 local_a77;
  undefined1 local_a70 [128];
  undefined1 local_9f0 [128];
  undefined1 local_970 [208];
  undefined8 local_8a0;
  undefined8 uStack_898;
  undefined8 local_890;
  undefined1 auStack_888 [16];
  undefined8 uStack_878;
  undefined8 local_870;
  undefined8 uStack_868;
  undefined8 local_860;
  undefined8 uStack_858;
  undefined8 local_850;
  undefined8 uStack_848;
  undefined8 local_840;
  undefined8 uStack_838;
  undefined8 local_830;
  undefined8 uStack_828;
  undefined8 local_820;
  undefined8 uStack_818;
  undefined1 local_810 [132];
  undefined4 local_78c;
  undefined1 local_788 [128];
  undefined1 local_708 [16];
  undefined1 local_6f8 [24];
  undefined1 local_6e0 [352];
  undefined1 local_580 [24];
  undefined1 local_568 [32];
  void *local_548 [3];
  void *local_530;
  void *local_518;
  void *local_500;
  undefined1 local_4e8 [40];
  undefined1 local_4c0 [352];
  undefined1 local_360 [56];
  undefined1 local_328 [96];
  undefined1 local_2c8 [360];
  undefined1 local_160 [56];
  undefined1 local_128 [136];
  undefined1 local_a0 [40];
  longlong local_78;
  longlong local_70;
  longlong local_68;
  
  uVar36 = 0;
  local_res20.QuadPart = 0;
  QueryPerformanceCounter(&local_res20);
  FUN_1402f07b0(local_b98);
  if (local_b78 != '\0') {
    plVar12 = (longlong *)FUN_1402d0680();
    pcVar4 = *(code **)(*plVar12 + 0x18);
    pppppppiVar15 = local_b98;
    if (7 < local_b80) {
      pppppppiVar15 = (int *******)local_b98[0];
    }
    local_c58._0_1_ = '\0';
    local_c58._1_1_ = '\0';
    local_c58._2_1_ = '\0';
    local_c58._3_1_ = '\0';
    local_c58._4_1_ = '\0';
    local_c58._5_1_ = '\0';
    local_c58._6_1_ = '\0';
    local_c58._7_1_ = '\0';
    uStack_c50 = (undefined2 *)0x0;
    local_c48 = 0;
    uStack_c40 = 0;
    FUN_140025200(&local_c58,pppppppiVar15,local_b88);
    (*pcVar4)(plVar12,&local_c58);
  }
  if (local_b78 != '\0') {
    FUN_140025470(local_b98);
  }
  FUN_1407255f0();
  if (DAT_140b819d0 == '\0') {
                    /* WARNING: Subroutine does not return */
    FUN_1402f2020();
  }
  uVar13 = FUN_1402ed700(&DAT_140b819a0);
  FUN_1405f5f00(local_b40,uVar13);
  if (*(char *)(param_1 + 0x154c) != '\0') {
    puVar14 = (undefined8 *)SteamInternal_ContextInit(&PTR_FUN_140b5eb50);
    (**(code **)(*(longlong *)*puVar14 + 0x150))();
    uVar13 = 0;
    goto joined_r0x00014072896f;
  }
  FUN_1402d3a10(local_c38,param_2);
  uStack_c50 = (undefined2 *)0x0;
  local_c48 = _DAT_140aac980;
  uStack_c40 = _UNK_140aac988;
  local_c58 = (char *)(ulonglong)
                      CONCAT16(s_scaling_140a89ff0[6],
                               CONCAT24(s_scaling_140a89ff0._4_2_,s_scaling_140a89ff0._0_4_));
  uVar13 = FUN_14029abe0(local_b40,&local_c58);
  FUN_140019ca0(local_b98,uVar13);
  FUN_140002d30(&local_c58);
  pppppppiVar15 = local_b98;
  if (0xf < local_b80) {
    pppppppiVar15 = (int *******)local_b98[0];
  }
  if ((local_b88 == 4) && (*(int *)pppppppiVar15 == 0x6f747561)) {
    local_b70 = _DAT_140aad3b0;
    local_b60 = _DAT_140aad2e0;
    uStack_b58 = _UNK_140aad2e8;
    local_b50 = 0x3f400000;
    local_b4c = 0x3f1eb852;
    local_b48 = 0x3f000000;
    pfVar19 = (float *)local_b70;
    do {
      fVar7 = local_c20;
      if ((*pfVar19 <= local_c20) &&
         (fVar7 = *pfVar19, DAT_140aac4dc <= (float)local_c34 / local_c20)) break;
      local_c20 = fVar7;
      pfVar19 = pfVar19 + 1;
    } while (pfVar19 != &local_b44);
  }
  else {
    pppppppiVar15 = local_b98;
    if (0xf < local_b80) {
      pppppppiVar15 = (int *******)local_b98[0];
    }
    local_res18.QuadPart = local_res18.QuadPart & 0xffffffff00000000;
    FUN_1402ab9c0(&local_c58,pppppppiVar15,local_b88 + (longlong)pppppppiVar15,&local_res18);
    if (local_res18.s.LowPart - 0x32 < 0xfb) {
      local_c20 = (float)(int)local_res18.s.LowPart * DAT_140aab6f0;
    }
  }
  if (local_c20 != *(float *)(param_1 + 0x1550)) {
    *(float *)(param_1 + 0x1550) = local_c20;
    FUN_14072e380(param_1);
  }
  local_c58 = (char *)0x0;
  uStack_c50 = (undefined2 *)0x0;
  local_c58 = (char *)FUN_140003270(0x20);
  uVar13 = s_language_code_iso_639_2_140a89798._8_8_;
  local_c48 = _DAT_140aaca90;
  uStack_c40 = _UNK_140aaca98;
  *(undefined8 *)local_c58 = s_language_code_iso_639_2_140a89798._0_8_;
  *(undefined8 *)(local_c58 + 8) = uVar13;
  *(undefined4 *)(local_c58 + 0x10) = s_language_code_iso_639_2_140a89798._16_4_;
  *(undefined2 *)(local_c58 + 0x14) = s_language_code_iso_639_2_140a89798._20_2_;
  local_c58[0x16] = s_language_code_iso_639_2_140a89798[0x16];
  local_c58[0x17] = '\0';
  uVar13 = FUN_14029abe0(local_b40,&local_c58);
  FUN_140019ca0(local_bb8,uVar13);
  if (0xf < uStack_c40) {
    FUN_140003040(&local_c58,local_c58);
  }
  pppppppuVar1 = (undefined8 *******)(param_1 + 0x1558);
  pppppppuVar22 = local_bb8;
  if (0xf < local_ba0) {
    pppppppuVar22 = (undefined8 *******)local_bb8[0];
  }
  sVar25 = *(size_t *)(param_1 + 0x1568);
  _Buf1 = pppppppuVar1;
  if (0xf < *(ulonglong *)(param_1 + 0x1570)) {
    _Buf1 = (undefined8 *******)*pppppppuVar1;
  }
  if (((sVar25 != local_ba8) ||
      ((sVar25 != 0 && (iVar10 = memcmp(_Buf1,pppppppuVar22,sVar25), iVar10 != 0)))) &&
     (FUN_1402d88b0(local_bb8), pppppppuVar1 != local_bb8)) {
    pppppppuVar22 = local_bb8;
    if (0xf < local_ba0) {
      pppppppuVar22 = (undefined8 *******)local_bb8[0];
    }
    FUN_140030630(pppppppuVar1,pppppppuVar22,local_ba8);
  }
  local_c48 = _DAT_140aac9a0;
  uStack_c40 = _UNK_140aac9a8;
  local_c58 = (char *)s_map_names_140a897c0._0_8_;
  uStack_c50 = (undefined2 *)(ulonglong)(byte)s_map_names_140a897c0[8];
  piVar16 = (int *)FUN_14029abe0(local_b40,&local_c58);
  plVar12 = (longlong *)(piVar16 + 4);
  if (0xf < *(ulonglong *)(piVar16 + 6)) {
    piVar16 = *(int **)piVar16;
  }
  if (*plVar12 == 6) {
    iVar10 = *piVar16 + -0x6974616e;
    if (iVar10 == 0) {
      iVar10 = *(ushort *)(piVar16 + 1) - 0x6576;
    }
    DAT_140b770a0 = iVar10 == 0;
  }
  else {
    DAT_140b770a0 = false;
  }
  if (0xf < uStack_c40) {
    FUN_140003040(&local_c58,local_c58);
  }
  FUN_140002d30(local_bb8);
  FUN_140002d30(local_b98);
  FUN_1402d3d20(local_c38);
  lVar17 = DAT_140b81a58;
  uVar11 = *(undefined4 *)(param_1 + 0x14c0);
  *(undefined2 *)(DAT_140b81a58 + 0x3340d4e) = 7;
  *(char *)(lVar17 + 0x3340d40) = (char)((uint)uVar11 >> 0x18);
  *(char *)(lVar17 + 0x3340d41) = (char)((uint)uVar11 >> 0x10);
  *(char *)(lVar17 + 0x3340d42) = (char)((uint)uVar11 >> 8);
  *(char *)(lVar17 + 0x3340d43) = (char)uVar11;
  *(undefined4 *)(lVar17 + 0x3340d48) = 0x3f800000;
  *(undefined1 *)(lVar17 + 0x3340d4c) = 0;
  *(undefined4 *)(lVar17 + 0x3340d50) = 0;
  uVar9 = 1;
  if (1 < local_c38[0]) {
    uVar9 = local_c38[0];
  }
  *(ushort *)(lVar17 + 0x3340d54) = uVar9;
  uVar9 = 1;
  if (1 < (ushort)local_c34) {
    uVar9 = (ushort)local_c34;
  }
  *(ushort *)(lVar17 + 0x3340d56) = uVar9;
  FUN_140833ff0(0);
  lVar17 = DAT_140b81a58;
  *(undefined4 *)(DAT_140b81a58 + 0x3340e10) = 0;
  uVar9 = 1;
  if (1 < local_c38[0]) {
    uVar9 = local_c38[0];
  }
  *(ushort *)(lVar17 + 0x3340e14) = uVar9;
  uVar24 = 1;
  uVar9 = uVar24;
  if (1 < (ushort)local_c34) {
    uVar9 = (ushort)local_c34;
  }
  *(ushort *)(lVar17 + 0x3340e16) = uVar9;
  *(undefined4 *)(lVar17 + 0x3341050) = 0;
  uVar9 = uVar24;
  if (1 < local_c38[0]) {
    uVar9 = local_c38[0];
  }
  *(ushort *)(lVar17 + 0x3341054) = uVar9;
  uVar9 = uVar24;
  if (1 < (ushort)local_c34) {
    uVar9 = (ushort)local_c34;
  }
  *(ushort *)(lVar17 + 0x3341056) = uVar9;
  *(undefined4 *)(lVar17 + 0x3341110) = 0;
  if (1 < local_c38[0]) {
    uVar24 = local_c38[0];
  }
  *(ushort *)(lVar17 + 0x3341114) = uVar24;
  uVar24 = 1;
  uVar9 = uVar24;
  if (1 < (ushort)local_c34) {
    uVar9 = (ushort)local_c34;
  }
  *(ushort *)(lVar17 + 0x3341116) = uVar9;
  *(undefined4 *)(lVar17 + 0x3340ed0) = 0;
  uVar9 = uVar24;
  if (1 < local_c38[0]) {
    uVar9 = local_c38[0];
  }
  *(ushort *)(lVar17 + 0x3340ed4) = uVar9;
  uVar9 = uVar24;
  if (1 < (ushort)local_c34) {
    uVar9 = (ushort)local_c34;
  }
  *(ushort *)(lVar17 + 0x3340ed6) = uVar9;
  *(undefined4 *)(lVar17 + 0x3340f90) = 0;
  uVar9 = uVar24;
  if (1 < local_c38[0]) {
    uVar9 = local_c38[0];
  }
  *(ushort *)(lVar17 + 0x3340f94) = uVar9;
  uVar9 = uVar24;
  if (1 < (ushort)local_c34) {
    uVar9 = (ushort)local_c34;
  }
  *(ushort *)(lVar17 + 0x3340f96) = uVar9;
  *(undefined4 *)(lVar17 + 0x33411d0) = 0;
  uVar9 = uVar24;
  if (1 < local_c38[0]) {
    uVar9 = local_c38[0];
  }
  *(ushort *)(lVar17 + 0x33411d4) = uVar9;
  uVar9 = uVar24;
  if (1 < (ushort)local_c34) {
    uVar9 = (ushort)local_c34;
  }
  *(ushort *)(lVar17 + 0x33411d6) = uVar9;
  *(undefined4 *)(lVar17 + 0x3341350) = 0;
  if (1 < local_c38[0]) {
    uVar24 = local_c38[0];
  }
  *(ushort *)(lVar17 + 0x3341354) = uVar24;
  uVar24 = 1;
  uVar9 = uVar24;
  if (1 < (ushort)local_c34) {
    uVar9 = (ushort)local_c34;
  }
  *(ushort *)(lVar17 + 0x3341356) = uVar9;
  *(undefined4 *)(lVar17 + 0x33414d0) = 0;
  uVar9 = uVar24;
  if (1 < local_c38[0]) {
    uVar9 = local_c38[0];
  }
  *(ushort *)(lVar17 + 0x33414d4) = uVar9;
  uVar9 = uVar24;
  if (1 < (ushort)local_c34) {
    uVar9 = (ushort)local_c34;
  }
  *(ushort *)(lVar17 + 0x33414d6) = uVar9;
  *(undefined4 *)(lVar17 + 0x3341410) = 0;
  uVar9 = uVar24;
  if (1 < local_c38[0]) {
    uVar9 = local_c38[0];
  }
  *(ushort *)(lVar17 + 0x3341414) = uVar9;
  uVar9 = uVar24;
  if (1 < (ushort)local_c34) {
    uVar9 = (ushort)local_c34;
  }
  *(ushort *)(lVar17 + 0x3341416) = uVar9;
  *(undefined4 *)(lVar17 + 0x3341590) = 0;
  uVar9 = uVar24;
  if (1 < local_c38[0]) {
    uVar9 = local_c38[0];
  }
  *(ushort *)(lVar17 + 0x3341594) = uVar9;
  uVar9 = uVar24;
  if (1 < (ushort)local_c34) {
    uVar9 = (ushort)local_c34;
  }
  *(ushort *)(lVar17 + 0x3341596) = uVar9;
  *(undefined4 *)(lVar17 + 0x3341650) = 0;
  uVar9 = uVar24;
  if (1 < local_c38[0]) {
    uVar9 = local_c38[0];
  }
  *(ushort *)(lVar17 + 0x3341654) = uVar9;
  uVar9 = uVar24;
  if (1 < (ushort)local_c34) {
    uVar9 = (ushort)local_c34;
  }
  *(ushort *)(lVar17 + 0x3341656) = uVar9;
  *(undefined4 *)(lVar17 + 0x3341710) = 0;
  uVar9 = uVar24;
  if (1 < local_c38[0]) {
    uVar9 = local_c38[0];
  }
  *(ushort *)(lVar17 + 0x3341714) = uVar9;
  uVar9 = uVar24;
  if (1 < (ushort)local_c34) {
    uVar9 = (ushort)local_c34;
  }
  *(ushort *)(lVar17 + 0x3341716) = uVar9;
  *(undefined4 *)(lVar17 + 0x3341dd0) = 0;
  uVar9 = uVar24;
  if (1 < local_c38[0]) {
    uVar9 = local_c38[0];
  }
  *(ushort *)(lVar17 + 0x3341dd4) = uVar9;
  uVar9 = uVar24;
  if (1 < (ushort)local_c34) {
    uVar9 = (ushort)local_c34;
  }
  *(ushort *)(lVar17 + 0x3341dd6) = uVar9;
  *(undefined4 *)(lVar17 + 0x3341d10) = 0;
  uVar9 = uVar24;
  if (1 < local_c38[0]) {
    uVar9 = local_c38[0];
  }
  *(ushort *)(lVar17 + 0x3341d14) = uVar9;
  uVar9 = uVar24;
  if (1 < (ushort)local_c34) {
    uVar9 = (ushort)local_c34;
  }
  *(ushort *)(lVar17 + 0x3341d16) = uVar9;
  *(undefined4 *)(lVar17 + 0x3341e90) = 0;
  if (1 < local_c38[0]) {
    uVar24 = local_c38[0];
  }
  *(ushort *)(lVar17 + 0x3341e94) = uVar24;
  uVar24 = 1;
  uVar9 = uVar24;
  if (1 < (ushort)local_c34) {
    uVar9 = (ushort)local_c34;
  }
  *(ushort *)(lVar17 + 0x3341e96) = uVar9;
  *(undefined4 *)(lVar17 + 0x3342310) = 0;
  uVar9 = uVar24;
  if (1 < local_c38[0]) {
    uVar9 = local_c38[0];
  }
  *(ushort *)(lVar17 + 0x3342314) = uVar9;
  uVar9 = uVar24;
  if (1 < (ushort)local_c34) {
    uVar9 = (ushort)local_c34;
  }
  *(ushort *)(lVar17 + 0x3342316) = uVar9;
  *(undefined4 *)(lVar17 + 0x33423d0) = 0;
  uVar9 = uVar24;
  if (1 < local_c38[0]) {
    uVar9 = local_c38[0];
  }
  *(ushort *)(lVar17 + 0x33423d4) = uVar9;
  uVar9 = uVar24;
  if (1 < (ushort)local_c34) {
    uVar9 = (ushort)local_c34;
  }
  *(ushort *)(lVar17 + 0x33423d6) = uVar9;
  *(undefined4 *)(lVar17 + 0x3342b50) = 0;
  uVar9 = uVar24;
  if (1 < local_c38[0]) {
    uVar9 = local_c38[0];
  }
  *(ushort *)(lVar17 + 0x3342b54) = uVar9;
  uVar9 = uVar24;
  if (1 < (ushort)local_c34) {
    uVar9 = (ushort)local_c34;
  }
  *(ushort *)(lVar17 + 0x3342b56) = uVar9;
  *(undefined4 *)(lVar17 + 0x334c8d0) = 0;
  uVar9 = uVar24;
  if (1 < local_c38[0]) {
    uVar9 = local_c38[0];
  }
  *(ushort *)(lVar17 + 0x334c8d4) = uVar9;
  uVar9 = uVar24;
  if (1 < (ushort)local_c34) {
    uVar9 = (ushort)local_c34;
  }
  *(ushort *)(lVar17 + 0x334c8d6) = uVar9;
  *(undefined4 *)(lVar17 + 0x334c990) = 0;
  uVar9 = uVar24;
  if (1 < local_c38[0]) {
    uVar9 = local_c38[0];
  }
  *(ushort *)(lVar17 + 0x334c994) = uVar9;
  if (1 < (ushort)local_c34) {
    uVar24 = (ushort)local_c34;
  }
  *(ushort *)(lVar17 + 0x334c996) = uVar24;
  *(undefined1 *)(lVar17 + 0x3340f62) = 2;
  *(undefined1 *)(lVar17 + 0x3341262) = 2;
  if ((longlong *)*DAT_140b81998 == (longlong *)0x0) {
    uVar8 = 0;
  }
  else {
    uVar8 = (**(code **)(*(longlong *)*DAT_140b81998 + 0x18))();
  }
  FUN_140822070(param_1,local_c38,uVar8);
  FUN_140822b00(param_1);
  FUN_1402d3a10(local_b08,local_c38);
  if (*(char *)(param_1 + 0x5fc0) != '\0') {
    uVar13 = FUN_1402d3970(local_b08,local_788);
    FUN_1407289e0(local_b08,uVar13);
    FUN_1402d3c30(local_788);
  }
  if (*(char *)(param_1 + 0x5fc1) != '\0') {
    uVar13 = FUN_1402d3c80(local_b08,local_788);
    FUN_1407289e0(local_b08,uVar13);
    FUN_1402d3c30(local_788);
  }
  lVar17 = param_1 + 0x5ff8;
  FUN_14034d6e0(lVar17,*(undefined8 *)(param_1 + 0x6008));
  *(longlong *)lVar17 = lVar17;
  *(longlong *)(param_1 + 0x6000) = lVar17;
  *(undefined8 *)(param_1 + 0x6008) = 0;
  *(undefined1 *)(param_1 + 0x6010) = 0;
  *(undefined8 *)(param_1 + 0x6018) = 0;
  FUN_140728b40(local_708);
  plVar12 = DAT_140b81998;
  if (*DAT_140b81998 != 0) {
    plVar27 = DAT_140b81998 + 0x148;
    iVar10 = _Mtx_lock(plVar27);
    while( true ) {
      if (iVar10 != 0) {
        std::_Throw_Cpp_error(5);
        pcVar4 = (code *)swi(3);
        uVar13 = (*pcVar4)();
        return uVar13;
      }
      if (*(int *)((longlong)plVar27 + 0x4c) == 0x7fffffff) {
        *(undefined4 *)((longlong)plVar27 + 0x4c) = 0x7ffffffe;
        std::_Throw_Cpp_error(6);
        pcVar4 = (code *)swi(3);
        uVar13 = (*pcVar4)();
        return uVar13;
      }
      lVar17 = plVar12[0xf1];
      lVar28 = plVar12[0xf0];
      _Mtx_unlock(plVar27);
      if (lVar28 == lVar17) break;
      FUN_14026c9e0(&local_c58);
      if ((longlong)local_c58 < 0x7ffffffffff0bdbf) {
        local_res18.QuadPart = (longlong)local_c58 + 1000000;
      }
      else {
        local_res18.QuadPart = 0x7fffffffffffffff;
      }
      FUN_14026ee80(&local_res18);
      plVar12 = DAT_140b81998;
      plVar27 = DAT_140b81998 + 0x148;
      iVar10 = _Mtx_lock(plVar27);
    }
    FUN_1407401f0(DAT_140b81998,local_bc8);
    FUN_140740420(DAT_140b81998,local_2c8);
    local_a88 = param_1;
    if (*(longlong *)(local_78 + 0x28) == 0) {
      local_a80.QuadPart = 0;
    }
    else {
      puVar14 = (undefined8 *)SteamInternal_ContextInit(&PTR_FUN_140b5eb68);
      (**(code **)(*(longlong *)*puVar14 + 0x10))((longlong *)*puVar14,&local_res18);
      local_a80 = local_res18;
    }
    local_a78 = *(longlong *)(local_78 + 0x28) != 0;
    if ((*(longlong *)(local_78 + 0x28) == 0) ||
       (local_a77 = 1, *(longlong *)(local_78 + 0x20) == 0)) {
      local_a77 = 0;
    }
    FUN_1402d3a10(local_a70,local_c38);
    FUN_1402d3a10(local_9f0);
    memset(local_970,0,0xd0);
    FUN_140641930(local_970);
    uStack_878 = 0;
    local_870 = 0;
    uStack_868 = 0;
    local_860 = 0;
    uStack_858 = 0;
    local_850 = 0;
    uStack_848 = 0;
    local_840 = 0;
    local_830 = 0;
    uStack_828 = 0;
    local_820 = 0;
    local_8a0 = 0;
    uStack_898 = 0;
    local_890 = 0;
    auStack_888 = ZEXT816(0);
    uStack_838 = 0;
    uStack_818 = 0;
    FUN_14062a5f0(local_810,0,0);
    local_78c = *(undefined4 *)(param_1 + 0x1548);
    FUN_14072f4a0(*DAT_140b81998,&local_a88,local_2c8);
    (*(code *)**(undefined8 **)*DAT_140b81998)((undefined8 *)*DAT_140b81998,&local_a88,local_2c8);
    FUN_140728b40(local_4e8);
    FUN_140728dc0(local_4e8,local_a0);
    FUN_140348270(local_4c0,local_2c8);
    FUN_140728e40(local_360,local_160);
    FUN_140728f30(local_328,local_128);
    FUN_140728dc0(local_708,local_4e8);
    FUN_140348270(local_6e0,local_4c0);
    FUN_140728e40(local_580,local_360);
    FUN_140728f30(local_548,local_328);
    FUN_140728c00(local_4e8);
    FUN_1405ef270(&local_a88);
    FUN_140728d00(local_2c8);
  }
  if (*(longlong *)(param_1 + 0x5fe8) != 0) {
    puVar14 = (undefined8 *)(*(longlong *)(param_1 + 0x5fd0) + 0x20);
    if ((*(byte *)(*(longlong *)(param_1 + 0x5fd0) + 0x37) & 0x80) != 0) {
      puVar14 = (undefined8 *)*puVar14;
    }
    lVar17 = FUN_1405088a0(param_1 + 0x16f8,puVar14);
    if (lVar17 != 0) {
      FUN_140508900(param_1 + 0x16f8,puVar14);
    }
  }
  plVar27 = *(longlong **)(param_1 + 0x6060);
  lVar17 = *plVar27;
  plVar12 = plVar27;
  if (lVar17 == 0) {
    plVar12 = plVar27 + 1;
    lVar17 = *plVar12;
    while (lVar17 == 0) {
      plVar12 = plVar12 + 1;
      lVar17 = *plVar12;
    }
  }
  if (lVar17 != plVar27[*(longlong *)(param_1 + 0x6068)]) {
    do {
      lVar28 = *(longlong *)(lVar17 + 0x38);
      plVar27 = plVar12;
      if (*(longlong *)(lVar17 + 8) == *(longlong *)(param_1 + 0x6088)) {
        while (lVar28 == 0) {
          lVar28 = plVar27[1];
          plVar27 = plVar27 + 1;
        }
      }
      else {
        while (lVar28 == 0) {
          lVar28 = plVar27[1];
          plVar27 = plVar27 + 1;
        }
        lVar20 = *plVar12;
        lVar5 = *(longlong *)(lVar20 + 0x38);
        if (lVar20 == lVar17) {
          *plVar12 = lVar5;
        }
        else {
          for (; lVar5 != lVar17; lVar5 = *(longlong *)(lVar5 + 0x38)) {
            lVar20 = lVar5;
          }
          *(undefined8 *)(lVar20 + 0x38) = *(undefined8 *)(lVar5 + 0x38);
        }
        FUN_14073dd20();
        *(longlong *)(param_1 + 0x6070) = *(longlong *)(param_1 + 0x6070) + -1;
      }
      lVar17 = lVar28;
      plVar12 = plVar27;
    } while (lVar28 != *(longlong *)
                        (*(longlong *)(param_1 + 0x6060) + *(longlong *)(param_1 + 0x6068) * 8));
  }
  FUN_140822b00(param_1);
  FUN_140833920("Shell::render_nuklear",0x7fffffff);
  FUN_140518170(param_1 + 0x16f0);
  lVar17 = DAT_140b7b650;
  lVar28 = DAT_140b7b648;
  if (DAT_140b7b648 != DAT_140b7b650) {
    do {
      (**(code **)(lVar28 + 0x10))(lVar28);
      lVar28 = lVar28 + 0x30;
    } while (lVar28 != lVar17);
    if (DAT_140b7b648 != DAT_140b7b650) {
      DAT_140b7b650 = DAT_140b7b648;
    }
  }
  FUN_140731d80(param_1,local_c38);
  FUN_1401bbf20(*(undefined8 *)(param_1 + 0x1580));
  if (*(longlong *)(DAT_140b81a58 + 0x418) == 0) {
    FUN_14082c080("C:\\development\\nimby\\thirdparty\\bgfx\\src\\bgfx.cpp",0x14b7,0,
                  "bgfx is configured to allow only encoder API. See: `BGFX_CONFIG_ENCODER_API_ONLY`."
                 );
  }
  FUN_140832220(*(undefined8 *)(DAT_140b81a58 + 0x418),"vg::frame",0x7fffffff);
  lVar17 = *(longlong *)(param_1 + 0x1580);
  *(undefined4 *)(lVar17 + 0x80) = 0;
  uVar26 = *(uint *)(lVar17 + 0x298);
  if (uVar26 != 0) {
    uVar9 = *(ushort *)(lVar17 + 0x290 + (ulonglong)uVar26 * 2);
    uVar32 = (ulonglong)uVar9;
    if (uVar9 != 0xffff) {
      lVar28 = *(longlong *)(lVar17 + 0xe0);
      if (*(short *)(lVar28 + 8 + uVar32 * 0xc) == -1) {
        uVar24 = 0xffff;
        uVar35 = 0xffff;
      }
      else {
        uVar24 = *(ushort *)(lVar28 + uVar32 * 0xc);
        uVar35 = *(ushort *)(lVar28 + 2 + uVar32 * 0xc);
      }
      uVar32 = 0;
      if (uVar26 != 0) {
        do {
          uVar3 = *(ushort *)(lVar17 + 0x290 + uVar32 * 2);
          uVar23 = (ulonglong)uVar3;
          if (uVar3 != 0xffff) {
            lVar28 = *(longlong *)(lVar17 + 0xe0);
            if (*(short *)(lVar28 + 8 + uVar23 * 0xc) == -1) {
              uVar33 = 0xffff;
              uVar31 = 0xffff;
            }
            else {
              uVar31 = *(ushort *)(lVar28 + uVar23 * 0xc);
              uVar33 = *(ushort *)(lVar28 + 2 + uVar23 * 0xc);
            }
            if ((uVar31 < uVar24) || (uVar33 < uVar35)) {
              FUN_1401b42c0(lVar17);
            }
            else {
              *(ushort *)(lVar17 + 0x290 + uVar36 * 2) = uVar3;
              uVar36 = (ulonglong)((int)uVar36 + 1);
            }
          }
          uVar26 = (int)uVar32 + 1;
          uVar32 = (ulonglong)uVar26;
        } while (uVar26 < *(uint *)(lVar17 + 0x298));
      }
      uVar34 = 0xffff;
      *(undefined2 *)(lVar17 + 0x290 + uVar36 * 2) = *(undefined2 *)(lVar17 + 0x290);
      *(ushort *)(lVar17 + 0x290) = uVar9;
      *(undefined4 *)(lVar17 + 0x298) = 0;
      FUN_1401ba2e0(lVar17);
      lVar28 = (longlong)((int)uVar36 + 1);
      if (lVar28 < 4) {
        puVar29 = (undefined2 *)(lVar17 + 0x290 + lVar28 * 2);
        for (lVar20 = 4 - lVar28; lVar20 != 0; lVar20 = lVar20 + -1) {
          *puVar29 = uVar34;
          puVar29 = puVar29 + 1;
        }
      }
    }
  }
  FUN_140740c40(DAT_140b81998);
  if (DAT_140b81998[2] != 0) {
    FUN_14024fd80();
  }
  plVar12 = (longlong *)DAT_140b81998[1];
  DAT_140b81998[1] = 0;
  if (plVar12 != (longlong *)0x0) {
    (**(code **)(*plVar12 + 0x20))(plVar12,1);
  }
  FUN_1405577e0(param_1 + 0x70);
  FUN_1405577e0(param_1 + 0x2b0);
  FUN_1405577e0(param_1 + 0x4f0);
  FUN_1405577e0(param_1 + 0x730);
  FUN_1405577e0(param_1 + 0x970);
  FUN_1405577e0(param_1 + 0xbb0);
  FUN_1405577e0(param_1 + 0xdf0);
  FUN_1405577e0(param_1 + 0x1030);
  FUN_1405577e0(param_1 + 0x1270);
  plVar12 = (longlong *)FUN_1402e2ea0();
  (**(code **)(*plVar12 + 8))(plVar12,&local_c58);
  if (uStack_c50 != (undefined2 *)0x0) {
    FUN_140740420(DAT_140b81998,local_2c8);
    uVar13 = 0;
    if ((local_70 != 0) && (local_68 != 0)) {
      uVar13 = *(undefined8 *)(local_70 + 0x408);
    }
    FUN_140731510(uVar13,&local_c58,param_1 + 0x70);
    *(longlong *)(param_1 + 0x2a8) = *(longlong *)(param_1 + 0x2a8) + 1;
    FUN_140731510(uVar13,&local_c58,param_1 + 0x2b0);
    *(longlong *)(param_1 + 0x4e8) = *(longlong *)(param_1 + 0x4e8) + 1;
    FUN_140731510(uVar13,&local_c58,param_1 + 0x4f0);
    *(longlong *)(param_1 + 0x728) = *(longlong *)(param_1 + 0x728) + 1;
    FUN_140731510(uVar13,&local_c58,param_1 + 0x730);
    *(longlong *)(param_1 + 0x968) = *(longlong *)(param_1 + 0x968) + 1;
    FUN_140731510(uVar13,&local_c58,param_1 + 0x970);
    *(longlong *)(param_1 + 0xba8) = *(longlong *)(param_1 + 0xba8) + 1;
    FUN_140731510(uVar13,&local_c58,param_1 + 0xbb0);
    *(longlong *)(param_1 + 0xde8) = *(longlong *)(param_1 + 0xde8) + 1;
    FUN_140731510(uVar13,&local_c58,param_1 + 0xdf0);
    *(longlong *)(param_1 + 0x1028) = *(longlong *)(param_1 + 0x1028) + 1;
    FUN_140731510(uVar13,&local_c58,param_1 + 0x1030);
    *(longlong *)(param_1 + 0x1268) = *(longlong *)(param_1 + 0x1268) + 1;
    FUN_140731510(uVar13,&local_c58,param_1 + 0x1270);
    *(longlong *)(param_1 + 0x14a8) = *(longlong *)(param_1 + 0x14a8) + 1;
    if ((DAT_140b81998 != (longlong *)0x0) && ((undefined8 *)DAT_140b81998[2] != (undefined8 *)0x0))
    {
      FUN_14024d8e0(*(undefined8 *)DAT_140b81998[2]);
    }
    FUN_140728d00(local_2c8);
  }
  cVar2 = *(char *)((longlong)*(longlong **)((longlong)local_c58 + 8) + 0x19);
  plVar12 = *(longlong **)((longlong)local_c58 + 8);
  while (cVar2 == '\0') {
    FUN_1402a4160(&local_c58,&local_c58,plVar12[2]);
    plVar27 = (longlong *)*plVar12;
    FUN_140002d30(plVar12 + 5);
    free(plVar12);
    plVar12 = plVar27;
    cVar2 = *(char *)((longlong)plVar27 + 0x19);
  }
  free(local_c58);
  local_c58 = (char *)0x0;
  uStack_c50 = (undefined2 *)0x0;
  local_c48 = 0;
  FUN_14073acc0(param_1 + 0x14c8,&local_c58);
  puVar6 = uStack_c50;
  _Memory = (undefined2 *)local_c58;
  for (puVar29 = (undefined2 *)local_c58; puVar29 != puVar6; puVar29 = puVar29 + 1) {
    FUN_1401b4410(*(undefined8 *)(param_1 + 0x1580),*puVar29);
  }
  if (_Memory != (undefined2 *)0x0) {
    free(_Memory);
  }
  plVar12 = DAT_140b81998;
  uVar13 = FUN_1407290d0(local_4e8,local_708);
  FUN_1407408b0(plVar12,uVar13);
  uVar11 = FUN_14082fe70(DAT_140b81a58,0);
  *(undefined4 *)(param_1 + 0x1548) = uVar11;
  FUN_1407255f0();
  if (DAT_140b819d0 == '\0') {
                    /* WARNING: Subroutine does not return */
    FUN_1402f2020();
  }
  plVar18 = (longlong *)FUN_1402ed700(&DAT_140b819a0);
  uStack_c40 = 0xf;
  plVar12 = (longlong *)plVar18[2];
  local_c48 = 0xb;
  plVar27 = plVar18;
  if (local_c28 == '\0') {
    local_c58._0_1_ = s_perf_bg_fps_140a89a98[0];
    local_c58._1_1_ = s_perf_bg_fps_140a89a98[1];
    local_c58._2_1_ = s_perf_bg_fps_140a89a98[2];
    local_c58._3_1_ = s_perf_bg_fps_140a89a98[3];
    local_c58._4_1_ = s_perf_bg_fps_140a89a98[4];
    local_c58._5_1_ = s_perf_bg_fps_140a89a98[5];
    local_c58._6_1_ = s_perf_bg_fps_140a89a98[6];
    local_c58._7_1_ = s_perf_bg_fps_140a89a98[7];
    uStack_c50._0_3_ = CONCAT12(s_perf_bg_fps_140a89a98[10],s_perf_bg_fps_140a89a98._8_2_);
    uStack_c50 = (undefined2 *)(ulonglong)(uint3)uStack_c50;
    if (plVar12 != (longlong *)0x0) {
      do {
        plVar21 = plVar12 + 4;
        uVar36 = plVar12[6];
        if (0xf < (ulonglong)plVar12[7]) {
          plVar21 = (longlong *)*plVar21;
        }
        uVar32 = uVar36;
        if (0xb < uVar36) {
          uVar32 = 0xb;
        }
        iVar10 = memcmp(plVar21,&local_c58,uVar32);
        plVar21 = plVar12;
        if (iVar10 == 0) {
          if (10 < uVar36) {
LAB_140728743:
            plVar21 = plVar12 + 1;
            plVar27 = plVar12;
          }
        }
        else if (-1 < iVar10) goto LAB_140728743;
        plVar12 = (longlong *)*plVar21;
      } while (plVar12 != (longlong *)0x0);
      goto LAB_140728752;
    }
  }
  else {
    local_c58._0_1_ = s_perf_fg_fps_140a898d0[0];
    local_c58._1_1_ = s_perf_fg_fps_140a898d0[1];
    local_c58._2_1_ = s_perf_fg_fps_140a898d0[2];
    local_c58._3_1_ = s_perf_fg_fps_140a898d0[3];
    local_c58._4_1_ = s_perf_fg_fps_140a898d0[4];
    local_c58._5_1_ = s_perf_fg_fps_140a898d0[5];
    local_c58._6_1_ = s_perf_fg_fps_140a898d0[6];
    local_c58._7_1_ = s_perf_fg_fps_140a898d0[7];
    uStack_c50._0_3_ = CONCAT12(s_perf_fg_fps_140a898d0[10],s_perf_fg_fps_140a898d0._8_2_);
    uStack_c50 = (undefined2 *)(ulonglong)(uint3)uStack_c50;
    if (plVar12 != (longlong *)0x0) {
      do {
        plVar21 = plVar12 + 4;
        uVar36 = plVar12[6];
        if (0xf < (ulonglong)plVar12[7]) {
          plVar21 = (longlong *)*plVar21;
        }
        uVar32 = uVar36;
        if (0xb < uVar36) {
          uVar32 = 0xb;
        }
        iVar10 = memcmp(plVar21,&local_c58,uVar32);
        plVar21 = plVar12;
        if (iVar10 == 0) {
          if (10 < uVar36) {
LAB_1407286c3:
            plVar21 = plVar12 + 1;
            plVar27 = plVar12;
          }
        }
        else if (-1 < iVar10) goto LAB_1407286c3;
        plVar12 = (longlong *)*plVar21;
      } while (plVar12 != (longlong *)0x0);
LAB_140728752:
      if (plVar27 != plVar18) {
        plVar12 = plVar27 + 4;
        uVar36 = plVar27[6];
        if (0xf < (ulonglong)plVar27[7]) {
          plVar12 = (longlong *)*plVar12;
        }
        sVar25 = 0xb;
        if (uVar36 < 0xb) {
          sVar25 = uVar36;
        }
        iVar10 = memcmp(&local_c58,plVar12,sVar25);
        if (iVar10 == 0) {
          if (uVar36 < 0xc) {
LAB_140728797:
            if (plVar27[10] != 0) {
              plVar12 = plVar27 + 8;
              if (0xf < (ulonglong)plVar27[0xb]) {
                plVar12 = (longlong *)*plVar12;
              }
              local_res18.QuadPart = local_res18.QuadPart & 0xffffffff00000000;
              FUN_1402ab9c0(&local_c58,plVar12,plVar27[10] + (longlong)plVar12,&local_res18);
              if (0 < (int)local_res18.s.LowPart) {
                dVar37 = DAT_140aabd08 / (double)(int)local_res18.s.LowPart;
                local_res18.QuadPart = 0;
                QueryPerformanceCounter(&local_res18);
                lVar17 = local_res18.QuadPart - (longlong)local_res20;
                if (DAT_140b5ebf0.QuadPart == 0xffffffffffffffff) {
                  QueryPerformanceFrequency(&local_res18);
                  DAT_140b5ebf0.s = local_res18.s;
                }
                if (0.0 < dVar37 - (double)lVar17 / (double)DAT_140b5ebf0.QuadPart) {
                  local_res18.QuadPart = llround();
                  FUN_14034b570(&local_res18);
                }
              }
            }
          }
        }
        else if (-1 < iVar10) goto LAB_140728797;
      }
    }
  }
  uVar13 = 1;
  if (local_500 != (void *)0x0) {
    free(local_500);
  }
  if (local_518 != (void *)0x0) {
    free(local_518);
  }
  if (local_530 != (void *)0x0) {
    free(local_530);
  }
  if (local_548[0] != (void *)0x0) {
    free(local_548[0]);
  }
  FUN_14073a8b0(local_568);
  FUN_14073a830(local_580);
  FUN_140347150(local_6e0);
  FUN_14034bc60(local_6f8);
  for (pvVar30 = local_ad8; pvVar30 != local_ad0; pvVar30 = (void *)((longlong)pvVar30 + 0x70)) {
    FUN_140002d30((longlong)pvVar30 + 0x40);
  }
  pvVar30 = local_c08;
  if (local_ad8 != (void *)0x0) {
    free(local_ad8);
    pvVar30 = local_c08;
  }
  for (; pvVar30 != local_c00; pvVar30 = (void *)((longlong)pvVar30 + 0x70)) {
    FUN_140002d30((longlong)pvVar30 + 0x40);
  }
  if (local_c08 != (void *)0x0) {
    free(local_c08);
  }
joined_r0x00014072896f:
  while (local_b30 != (undefined8 *)0x0) {
    FUN_14029c4e0(local_b40,*local_b30);
    puVar14 = (undefined8 *)local_b30[1];
    FUN_140002d30(local_b30 + 8);
    FUN_140002d30(local_b30 + 4);
    free(local_b30);
    local_b30 = puVar14;
  }
  s_language_code_iso_639_2_140a89798[0] = (char)s_language_code_iso_639_2_140a89798._0_8_;
  s_language_code_iso_639_2_140a89798[1] = SUB81(s_language_code_iso_639_2_140a89798._0_8_,1);
  s_language_code_iso_639_2_140a89798[2] = SUB81(s_language_code_iso_639_2_140a89798._0_8_,2);
  s_language_code_iso_639_2_140a89798[3] = SUB81(s_language_code_iso_639_2_140a89798._0_8_,3);
  s_language_code_iso_639_2_140a89798[4] = SUB81(s_language_code_iso_639_2_140a89798._0_8_,4);
  s_language_code_iso_639_2_140a89798[5] = SUB81(s_language_code_iso_639_2_140a89798._0_8_,5);
  s_language_code_iso_639_2_140a89798[6] = SUB81(s_language_code_iso_639_2_140a89798._0_8_,6);
  s_language_code_iso_639_2_140a89798[7] = SUB81(s_language_code_iso_639_2_140a89798._0_8_,7);
  s_language_code_iso_639_2_140a89798[8] = (char)s_language_code_iso_639_2_140a89798._8_8_;
  s_language_code_iso_639_2_140a89798[9] = SUB81(s_language_code_iso_639_2_140a89798._8_8_,1);
  s_language_code_iso_639_2_140a89798[10] = SUB81(s_language_code_iso_639_2_140a89798._8_8_,2);
  s_language_code_iso_639_2_140a89798[0xb] = SUB81(s_language_code_iso_639_2_140a89798._8_8_,3);
  s_language_code_iso_639_2_140a89798[0xc] = SUB81(s_language_code_iso_639_2_140a89798._8_8_,4);
  s_language_code_iso_639_2_140a89798[0xd] = SUB81(s_language_code_iso_639_2_140a89798._8_8_,5);
  s_language_code_iso_639_2_140a89798[0xe] = SUB81(s_language_code_iso_639_2_140a89798._8_8_,6);
  s_language_code_iso_639_2_140a89798[0xf] = SUB81(s_language_code_iso_639_2_140a89798._8_8_,7);
  s_language_code_iso_639_2_140a89798[0x10] = (char)s_language_code_iso_639_2_140a89798._16_4_;
  s_language_code_iso_639_2_140a89798[0x11] = SUB41(s_language_code_iso_639_2_140a89798._16_4_,1);
  s_language_code_iso_639_2_140a89798[0x12] = SUB41(s_language_code_iso_639_2_140a89798._16_4_,2);
  s_language_code_iso_639_2_140a89798[0x13] = SUB41(s_language_code_iso_639_2_140a89798._16_4_,3);
  s_language_code_iso_639_2_140a89798[0x14] = (char)s_language_code_iso_639_2_140a89798._20_2_;
  s_language_code_iso_639_2_140a89798[0x15] = SUB21(s_language_code_iso_639_2_140a89798._20_2_,1);
  s_map_names_140a897c0[0] = (char)s_map_names_140a897c0._0_8_;
  s_map_names_140a897c0[1] = SUB81(s_map_names_140a897c0._0_8_,1);
  s_map_names_140a897c0[2] = SUB81(s_map_names_140a897c0._0_8_,2);
  s_map_names_140a897c0[3] = SUB81(s_map_names_140a897c0._0_8_,3);
  s_map_names_140a897c0[4] = SUB81(s_map_names_140a897c0._0_8_,4);
  s_map_names_140a897c0[5] = SUB81(s_map_names_140a897c0._0_8_,5);
  s_map_names_140a897c0[6] = SUB81(s_map_names_140a897c0._0_8_,6);
  s_map_names_140a897c0[7] = SUB81(s_map_names_140a897c0._0_8_,7);
  s_perf_fg_fps_140a898d0[0] = (char)s_perf_fg_fps_140a898d0._0_8_;
  s_perf_fg_fps_140a898d0[1] = SUB81(s_perf_fg_fps_140a898d0._0_8_,1);
  s_perf_fg_fps_140a898d0[2] = SUB81(s_perf_fg_fps_140a898d0._0_8_,2);
  s_perf_fg_fps_140a898d0[3] = SUB81(s_perf_fg_fps_140a898d0._0_8_,3);
  s_perf_fg_fps_140a898d0[4] = SUB81(s_perf_fg_fps_140a898d0._0_8_,4);
  s_perf_fg_fps_140a898d0[5] = SUB81(s_perf_fg_fps_140a898d0._0_8_,5);
  s_perf_fg_fps_140a898d0[6] = SUB81(s_perf_fg_fps_140a898d0._0_8_,6);
  s_perf_fg_fps_140a898d0[7] = SUB81(s_perf_fg_fps_140a898d0._0_8_,7);
  s_perf_fg_fps_140a898d0[8] = (char)s_perf_fg_fps_140a898d0._8_2_;
  s_perf_fg_fps_140a898d0[9] = SUB21(s_perf_fg_fps_140a898d0._8_2_,1);
  s_perf_bg_fps_140a89a98[0] = (char)s_perf_bg_fps_140a89a98._0_8_;
  s_perf_bg_fps_140a89a98[1] = SUB81(s_perf_bg_fps_140a89a98._0_8_,1);
  s_perf_bg_fps_140a89a98[2] = SUB81(s_perf_bg_fps_140a89a98._0_8_,2);
  s_perf_bg_fps_140a89a98[3] = SUB81(s_perf_bg_fps_140a89a98._0_8_,3);
  s_perf_bg_fps_140a89a98[4] = SUB81(s_perf_bg_fps_140a89a98._0_8_,4);
  s_perf_bg_fps_140a89a98[5] = SUB81(s_perf_bg_fps_140a89a98._0_8_,5);
  s_perf_bg_fps_140a89a98[6] = SUB81(s_perf_bg_fps_140a89a98._0_8_,6);
  s_perf_bg_fps_140a89a98[7] = SUB81(s_perf_bg_fps_140a89a98._0_8_,7);
  s_perf_bg_fps_140a89a98[8] = (char)s_perf_bg_fps_140a89a98._8_2_;
  s_perf_bg_fps_140a89a98[9] = SUB21(s_perf_bg_fps_140a89a98._8_2_,1);
  s_scaling_140a89ff0[0] = (char)s_scaling_140a89ff0._0_4_;
  s_scaling_140a89ff0[1] = SUB41(s_scaling_140a89ff0._0_4_,1);
  s_scaling_140a89ff0[2] = SUB41(s_scaling_140a89ff0._0_4_,2);
  s_scaling_140a89ff0[3] = SUB41(s_scaling_140a89ff0._0_4_,3);
  s_scaling_140a89ff0[4] = (char)s_scaling_140a89ff0._4_2_;
  s_scaling_140a89ff0[5] = SUB21(s_scaling_140a89ff0._4_2_,1);
  return uVar13;
}

