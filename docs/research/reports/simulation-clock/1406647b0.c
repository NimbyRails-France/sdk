// Candidate VA 1406647b0; RVA 0x6647b0
// Ghidra inferred prototype: undefined FUN_1406647b0()

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_1406647b0(longlong param_1,longlong *param_2,longlong param_3,undefined8 param_4,
                  longlong *param_5)

{
  DWORD DVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  longlong lVar4;
  code *pcVar5;
  undefined1 auVar6 [12];
  undefined1 auVar7 [12];
  undefined1 auVar8 [12];
  undefined1 auVar9 [12];
  undefined1 auVar10 [12];
  undefined1 auVar11 [12];
  undefined1 auVar12 [12];
  undefined1 auVar13 [12];
  undefined1 auVar14 [12];
  undefined1 auVar15 [12];
  undefined1 auVar16 [12];
  undefined1 auVar17 [12];
  undefined1 auVar18 [16];
  undefined1 auVar19 [16];
  undefined1 auVar20 [16];
  undefined1 auVar21 [16];
  undefined1 auVar22 [16];
  undefined1 auVar23 [16];
  int *piVar24;
  LARGE_INTEGER LVar25;
  char cVar26;
  undefined1 uVar27;
  uint uVar28;
  undefined4 uVar29;
  int iVar30;
  undefined8 uVar31;
  longlong lVar32;
  longlong lVar33;
  LARGE_INTEGER *pLVar34;
  ulonglong *puVar35;
  LARGE_INTEGER *pLVar36;
  undefined8 uVar37;
  undefined8 *puVar38;
  LONG *pLVar39;
  char *pcVar40;
  undefined1 *puVar41;
  ulonglong uVar42;
  LONG *pLVar43;
  char *pcVar44;
  void *pvVar45;
  int *piVar46;
  void *pvVar47;
  uint uVar48;
  longlong lVar49;
  longlong lVar50;
  ulonglong uVar51;
  int iVar52;
  double dVar53;
  undefined1 auVar54 [16];
  LARGE_INTEGER LVar70;
  undefined1 auVar55 [16];
  undefined1 auVar56 [16];
  undefined1 auVar57 [16];
  undefined1 auVar58 [16];
  undefined1 auVar59 [16];
  undefined1 auVar60 [16];
  undefined1 auVar61 [16];
  undefined1 auVar62 [16];
  undefined1 auVar63 [16];
  undefined1 auVar64 [16];
  undefined1 auVar65 [16];
  undefined1 auVar66 [16];
  undefined1 auVar67 [16];
  undefined1 auVar68 [16];
  undefined1 auVar69 [16];
  undefined4 uVar71;
  undefined4 uVar72;
  longlong local_res18;
  undefined8 local_res20;
  undefined1 *in_stack_fffffffffffffae8;
  undefined4 uVar73;
  undefined1 local_4c8 [12];
  undefined4 uStack_4bc;
  undefined8 *local_4b8;
  undefined1 local_4a8 [4];
  undefined4 uStack_4a4;
  LARGE_INTEGER LStack_4a0;
  undefined1 local_498 [16];
  void *local_488;
  undefined8 uStack_480;
  undefined1 local_478 [16];
  LARGE_INTEGER local_468;
  LARGE_INTEGER LStack_460;
  LARGE_INTEGER local_458;
  LARGE_INTEGER LStack_450;
  LARGE_INTEGER local_448;
  ulonglong local_440;
  ulonglong uStack_438;
  undefined1 local_430 [16];
  void *local_420 [3];
  ulonglong local_408;
  undefined1 local_400;
  undefined1 local_3ff [15];
  LARGE_INTEGER local_3f0;
  int local_3e8;
  int local_3e4;
  undefined8 local_3e0;
  undefined8 uStack_3d8;
  undefined8 local_3d0;
  ulonglong uStack_3c8;
  void *local_3c0;
  undefined8 uStack_3b8;
  undefined8 local_3b0;
  ulonglong uStack_3a8;
  undefined1 local_3a0 [32];
  void *local_380 [3];
  ulonglong local_368;
  undefined8 local_358;
  undefined8 uStack_350;
  undefined8 local_348;
  undefined8 uStack_340;
  undefined8 local_338;
  undefined8 uStack_330;
  undefined8 local_328;
  undefined8 uStack_320;
  undefined8 local_318;
  undefined8 uStack_310;
  undefined8 local_308;
  undefined8 uStack_300;
  undefined8 local_2f8;
  undefined8 uStack_2f0;
  undefined8 local_2e8;
  undefined8 uStack_2e0;
  undefined8 local_2d8;
  undefined8 uStack_2d0;
  undefined4 local_2c8;
  undefined4 local_2c4;
  undefined4 local_2c0;
  undefined1 local_2bc [4];
  undefined **local_2b8;
  undefined1 *local_2b0;
  undefined8 local_2a8;
  undefined8 local_2a0;
  undefined1 local_298 [600];

  local_res18 = param_3;
  local_res20 = param_4;
  if (*(longlong *)(param_3 + 600) == 0) {
                    /* WARNING: Subroutine does not return */
    abort();
  }
  lVar4 = *(longlong *)(*(longlong *)(param_3 + 600) + 0x410);
  if ((*(int *)(*(longlong *)ThreadLocalStoragePointer + 8) < DAT_140be3334) &&
     (FUN_140983d2c(&DAT_140be3334), DAT_140be3334 == -1)) {
    local_358 = _DAT_140aacec0;
    uStack_350 = _UNK_140aacec8;
    local_348 = _DAT_140aacf00;
    uStack_340 = _UNK_140aacf08;
    local_338 = _DAT_140aacf90;
    uStack_330 = _UNK_140aacf98;
    local_328 = _DAT_140aacfc0;
    uStack_320 = _UNK_140aacfc8;
    local_318 = _DAT_140aacfd0;
    uStack_310 = _UNK_140aacfd8;
    local_308 = _DAT_140aacfe0;
    uStack_300 = _UNK_140aacfe8;
    local_2f8 = _DAT_140aacff0;
    uStack_2f0 = _UNK_140aacff8;
    local_2e8 = _DAT_140aad020;
    uStack_2e0 = _UNK_140aad028;
    local_2d8 = _DAT_140aad050;
    uStack_2d0 = _UNK_140aad058;
    local_2c8 = 7000;
    local_2c4 = 8000;
    local_2c0 = 9000;
    local_468.QuadPart = (LONGLONG)&local_358;
    LStack_460.QuadPart = (LONGLONG)local_2bc;
    FUN_1404823e0(&DAT_140be3318,&local_468);
    atexit(FUN_1409a9ca0);
    _Init_thread_footer(&DAT_140be3334);
  }
  FUN_14055b8e0(param_5,&DAT_140b8dfe0);
  (**(code **)(*param_5 + 8))(param_5);
  FUN_14055b8e0(param_5,&DAT_140b8e060);
  pcVar5 = *(code **)(*param_5 + 0xa8);
  uVar31 = FUN_1402d82e0("option_company_time_zones","Map time zones:");
  (*pcVar5)(param_5,uVar31,0x14);
  _local_4c8 = ZEXT816(0);
  local_4b8 = (undefined8 *)0x0;
  lVar32 = FUN_1402d82e0("option_company_time_zones_zero","Always 0");
  _local_4a8 = ZEXT816(0);
  local_498 = ZEXT816(0);
  lVar49 = -1;
  do {
    lVar49 = lVar49 + 1;
  } while (*(char *)(lVar32 + lVar49) != '\0');
  FUN_140002c00(local_4a8,lVar32);
  puVar38 = stack0xfffffffffffffb40;
  if (stack0xfffffffffffffb40 < local_4b8) {
    stack0xfffffffffffffb40 = stack0xfffffffffffffb40 + 4;
    *puVar38 = _local_4a8;
    puVar38[1] = LStack_4a0;
    puVar38[2] = local_498._0_8_;
    puVar38[3] = local_498._8_8_;
    auVar65[0xf] = 0;
    auVar65._0_15_ = stack0xfffffffffffffb59;
    _local_4a8 = auVar65 << 8;
    uVar51 = 0xf;
  }
  else {
    FUN_1404ba220(local_4c8,local_4a8);
    uVar51 = local_498._8_8_;
  }
  if (0xf < uVar51) {
    FUN_140003040(local_4a8,_local_4a8);
  }
  lVar32 = FUN_1402d82e0("option_company_time_zones_simplified","Simplified");
  _local_4a8 = ZEXT816(0);
  local_498 = ZEXT816(0);
  lVar49 = -1;
  do {
    lVar49 = lVar49 + 1;
  } while (*(char *)(lVar32 + lVar49) != '\0');
  FUN_140002c00(local_4a8,lVar32);
  puVar38 = stack0xfffffffffffffb40;
  if (stack0xfffffffffffffb40 < local_4b8) {
    stack0xfffffffffffffb40 = stack0xfffffffffffffb40 + 4;
    *puVar38 = _local_4a8;
    puVar38[1] = LStack_4a0;
    puVar38[2] = local_498._0_8_;
    puVar38[3] = local_498._8_8_;
    auVar18[0xf] = 0;
    auVar18._0_15_ = stack0xfffffffffffffb59;
    _local_4a8 = auVar18 << 8;
    uVar51 = 0xf;
  }
  else {
    FUN_1404ba220(local_4c8,local_4a8);
    uVar51 = local_498._8_8_;
  }
  if (0xf < uVar51) {
    FUN_140003040(local_4a8,_local_4a8);
  }
  lVar32 = FUN_1402d82e0("option_company_time_zones_approximate","Approximate");
  _local_4a8 = ZEXT816(0);
  LVar70.QuadPart = 0;
  local_498 = ZEXT816(0);
  lVar49 = -1;
  do {
    lVar49 = lVar49 + 1;
  } while (*(char *)(lVar32 + lVar49) != '\0');
  FUN_140002c00(local_4a8,lVar32);
  puVar38 = stack0xfffffffffffffb40;
  if (stack0xfffffffffffffb40 < local_4b8) {
    stack0xfffffffffffffb40 = stack0xfffffffffffffb40 + 4;
    *puVar38 = _local_4a8;
    puVar38[1] = LStack_4a0;
    puVar38[2] = local_498._0_8_;
    ((LARGE_INTEGER *)(puVar38 + 3))->QuadPart = local_498._8_8_;
    auVar19[0xf] = 0;
    auVar19._0_15_ = stack0xfffffffffffffb59;
    _local_4a8 = auVar19 << 8;
    uVar51 = 0xf;
    LVar70 = (LARGE_INTEGER)local_498._8_8_;
  }
  else {
    FUN_1404ba220(local_4c8,local_4a8);
    uVar51 = local_498._8_8_;
  }
  if (0xf < uVar51) {
    FUN_140003040(local_4a8,_local_4a8);
  }
  if (*(longlong *)(*(longlong *)(param_3 + 0x250) + 0x28) == 0) {
    FUN_14055b8e0(param_5,&DAT_140b8e0e0);
    iVar52 = (**(code **)(*param_5 + 0x120))(param_5,local_4c8,*(undefined4 *)(lVar4 + 0x68));
    if (iVar52 != *(int *)(lVar4 + 0x68)) {
      local_res20._0_5_ = CONCAT14(1,iVar52);
      *(undefined8 *)(param_1 + 0x81c) = local_res20;
    }
  }
  else {
    FUN_14055b8e0(param_5,&DAT_140b8e0e0);
    (**(code **)(*param_5 + 0xa0))
              (param_5,(longlong)*(int *)(lVar4 + 0x68) * 0x20 + local_4c8._0_8_,0x11);
  }
  pvVar47 = stack0xfffffffffffffb40;
  pvVar45 = (void *)local_4c8._0_8_;
  if ((undefined8 *)local_4c8._0_8_ != stack0xfffffffffffffb40) {
    do {
      FUN_140002d30(pvVar45);
      pvVar45 = (void *)((longlong)pvVar45 + 0x20);
    } while (pvVar45 != pvVar47);
  }
  if ((void *)local_4c8._0_8_ != (void *)0x0) {
    free((void *)local_4c8._0_8_);
  }
  (**(code **)(*param_5 + 0x18))(param_5);
  FUN_14055b8e0(param_5,&DAT_140b8dfe0);
  (**(code **)(*param_5 + 8))(param_5);
  FUN_14055b8e0(param_5,&DAT_140b8e060);
  pcVar5 = *(code **)(*param_5 + 0xa8);
  uVar31 = FUN_1402d82e0("option_company_acc_utc0_offset_s","Accounting time shift:");
  (*pcVar5)(param_5,uVar31,0x14);
  uVar73 = (undefined4)((ulonglong)in_stack_fffffffffffffae8 >> 0x20);
  if (*(longlong *)(*(longlong *)(local_res18 + 0x250) + 0x28) == 0) {
    pcVar5 = *(code **)(*param_5 + 0xc0);
    uVar31 = FUN_1402d82e0("option_company_acc_utc0_offset_s_desc",
                           "Format: (-)hh:mm:ss\nAccounting new day will be considered to start at this shifted time relative to UTC0"
                          );
    (*pcVar5)(param_5,uVar31);
    FUN_14058d3f0(param_1 + 0x7c8,1,*(undefined4 *)(lVar4 + 0x6c));
    if ((*(int *)(param_1 + 0x7d4) < -0xa8c0) || (0xa8c0 < *(int *)(param_1 + 0x7d4))) {
      uVar29 = *(undefined4 *)(*param_2 + 0x612c);
      *(undefined1 *)((longlong)param_5 + 0x54) = 1;
      *(undefined4 *)((longlong)param_5 + 0x55) = uVar29;
    }
    FUN_14055b8e0(param_5,&DAT_140b8e0e0);
    in_stack_fffffffffffffae8 = (undefined1 *)CONCAT44(uVar73,0x264);
    uVar28 = (**(code **)(*param_5 + 0xe0))
                       (param_5,param_1 + 0x7f8,0xff,0x40,in_stack_fffffffffffffae8);
    FUN_14058d4b0(param_1 + 0x7c8);
    uVar48 = *(int *)(param_1 + 0x7d4) + 0xa8c0;
    *(undefined4 *)((longlong)param_5 + 0x2c) = 0x428c0000;
    *(undefined1 *)(param_5 + 5) = 1;
    *(undefined1 *)(param_5 + 7) = 1;
    *(undefined8 *)((longlong)param_5 + 0x3c) = 0x40800000;
    *(undefined8 *)((longlong)param_5 + 0x44) = 0;
    pcVar5 = *(code **)(*param_5 + 0x90);
    uVar31 = FUN_1402d82e0("apply","Apply");
    cVar26 = (*pcVar5)(param_5,uVar31,0x15180 < uVar48);
    if ((uVar48 < 0x15181) && (((uVar28 & 0x10) != 0 || (cVar26 != '\0')))) {
      local_res20._0_5_ = CONCAT14(1,*(undefined4 *)(param_1 + 0x7d4));
      *(undefined8 *)(param_1 + 0x824) = local_res20;
    }
  }
  else {
    FUN_14055b8e0(param_5,&DAT_140b8e0e0);
    pcVar5 = *(code **)(*param_5 + 0xa0);
    uVar31 = FUN_14058b630(local_420,*(undefined4 *)(lVar4 + 0x6c),3);
    (*pcVar5)(param_5,uVar31,0x11);
    FUN_140002d30(local_420);
  }
  lVar32 = -1;
  (**(code **)(*param_5 + 0x18))(param_5);
  FUN_14055b8e0(param_5,&DAT_140b8dfe0);
  (**(code **)(*param_5 + 8))(param_5);
  LVar25 = LStack_460;
  if (*(longlong *)(param_1 + 0x5c0) == 0) {
    LStack_460.s.LowPart._0_1_ = 1;
    LStack_460._4_4_ = LVar25.s.HighPart;
    auVar6._4_8_ = LVar70.QuadPart;
    auVar6._0_4_ = local_468.s.HighPart;
    auVar54._0_8_ = auVar6._0_8_ << 0x20;
    auVar54._8_4_ = LStack_460._0_4_;
    auVar54._12_4_ = LStack_460._4_4_;
    LVar70.QuadPart = auVar54._8_8_;
    LStack_4a0.s = LVar70.s;
    _local_4a8 = (void *)(double)*(float *)(lVar4 + 0x5c);
    local_468 = (LARGE_INTEGER)(double)*(float *)(lVar4 + 0x5c);
    LStack_460.QuadPart = LVar70.QuadPart;
    FUN_140647d20(param_1 + 0x570,1,local_4a8);
    *(undefined1 *)(param_1 + 0x5d0) = 1;
    *(undefined1 *)(param_1 + 0x618) = 0;
  }
  FUN_14055b8e0(param_5,&DAT_140b8e060);
  pcVar5 = *(code **)(*param_5 + 0xa8);
  uVar31 = FUN_1402d82e0("option_company_parallel_offset","New track parallel off.:");
  (*pcVar5)(param_5,uVar31,0x14);
  auVar65 = _local_4a8;
  lVar49 = local_res18;
  if (*(longlong *)(*(longlong *)(local_res18 + 0x250) + 0x28) == 0) {
    LStack_4a0.s.LowPart._0_1_ = 1;
    auVar18 = _local_4a8;
    uStack_4a4 = auVar65._4_4_;
    LStack_4a0._0_4_ = auVar18._8_4_;
    LStack_4a0._4_4_ = auVar65._12_4_;
    auVar8._4_8_ = LVar70.QuadPart;
    auVar8._0_4_ = uStack_4a4;
    auVar57._0_8_ = auVar8._0_8_ << 0x20;
    auVar57._8_4_ = LStack_4a0._0_4_;
    auVar57._12_4_ = LStack_4a0._4_4_;
    auVar56._8_8_ = auVar57._8_8_;
    auVar56._0_8_ = (double)*(float *)(lVar4 + 0x5c);
    in_stack_fffffffffffffae8 = local_4c8;
    _local_4c8 = auVar56;
    _local_4a8 = auVar56;
    cVar26 = FUN_140647fb0(param_1 + 0x570,param_2,param_5,1,in_stack_fffffffffffffae8);
    if (cVar26 != '\0') {
      auVar56 = ZEXT416(*(uint *)(lVar4 + 0x60));
      in_stack_fffffffffffffae8 =
           (undefined1 *)
           CONCAT44((int)((ulonglong)in_stack_fffffffffffffae8 >> 0x20),
                    *(undefined4 *)(lVar4 + 0x50));
      FUN_14073f9b0(lVar49,*(undefined4 *)(lVar4 + 0x44),*(undefined4 *)(lVar4 + 0x48),
                    *(undefined4 *)(lVar4 + 0x4c),in_stack_fffffffffffffae8,
                    *(undefined4 *)(lVar4 + 0x54),*(undefined1 *)(lVar4 + 0x58),
                    (float)*(double *)(param_1 + 0x5c8),*(undefined4 *)(lVar4 + 100),
                    *(undefined4 *)(lVar4 + 0x68),*(undefined4 *)(lVar4 + 0x6c),
                    *(uint *)(lVar4 + 0x60),*(undefined8 *)(lVar4 + 0xd0));
    }
  }
  else {
    FUN_14055b8e0(param_5,&DAT_140b8e0e0);
    pcVar5 = *(code **)(*param_5 + 0xa0);
    auVar7._4_8_ = LVar70.QuadPart;
    auVar7._0_4_ = uStack_4a4;
    auVar55._0_8_ = auVar7._0_8_ << 0x20;
    auVar55._8_4_ = LStack_4a0._0_4_;
    auVar55._12_4_ = LStack_4a0._4_4_;
    auVar56._4_12_ = auVar55._4_12_;
    auVar56._0_4_ = *(undefined4 *)(lVar4 + 0x5c);
    local_468.QuadPart = (LONGLONG)&DAT_00000009;
    LStack_460.QuadPart = (LONGLONG)local_4c8;
    LStack_4a0 = (LARGE_INTEGER)0x7;
    _local_4a8 = "{:.2f}m";
    _local_4c8 = auVar56;
    FUN_140021e90(&local_440,local_4a8,&local_468);
    (*pcVar5)(param_5,&local_440,0x11);
    FUN_140002d30(&local_440);
  }
  (**(code **)(*param_5 + 0x18))(param_5);
  FUN_14055b8e0(param_5,&DAT_140b8dfe0);
  (**(code **)(*param_5 + 8))(param_5);
  auVar65 = _local_4a8;
  if (*(longlong *)(param_1 + 0x688) == 0) {
    LStack_4a0.s.LowPart._0_1_ = 1;
    auVar18 = _local_4a8;
    uStack_4a4 = auVar65._4_4_;
    LStack_4a0._0_4_ = auVar18._8_4_;
    LStack_4a0._4_4_ = auVar65._12_4_;
    auVar9._4_8_ = auVar56._8_8_;
    auVar9._0_4_ = uStack_4a4;
    auVar58._0_8_ = auVar9._0_8_ << 0x20;
    auVar58._8_4_ = LStack_4a0._0_4_;
    auVar58._12_4_ = LStack_4a0._4_4_;
    auVar56._8_8_ = auVar58._8_8_;
    auVar56._0_8_ = (double)*(float *)(lVar4 + 0x60);
    _local_4c8 = auVar56;
    _local_4a8 = auVar56;
    FUN_140647d20(param_1 + 0x638,1,local_4c8);
    *(undefined1 *)(param_1 + 0x698) = 1;
    *(undefined1 *)(param_1 + 0x6e0) = 0;
  }
  FUN_14055b8e0(param_5,&DAT_140b8e060);
  pcVar5 = *(code **)(*param_5 + 0xa8);
  uVar31 = FUN_1402d82e0("option_company_track_overlap_distance","New track overlap dist.:");
  (*pcVar5)(param_5,uVar31,0x14);
  auVar65 = _local_4a8;
  lVar49 = local_res18;
  if (*(longlong *)(*(longlong *)(local_res18 + 0x250) + 0x28) == 0) {
    LStack_4a0.s.LowPart._0_1_ = 1;
    auVar18 = _local_4a8;
    uStack_4a4 = auVar65._4_4_;
    LStack_4a0._0_4_ = auVar18._8_4_;
    LStack_4a0._4_4_ = auVar65._12_4_;
    auVar11._4_8_ = auVar56._8_8_;
    auVar11._0_4_ = uStack_4a4;
    auVar61._0_8_ = auVar11._0_8_ << 0x20;
    auVar61._8_4_ = LStack_4a0._0_4_;
    auVar61._12_4_ = LStack_4a0._4_4_;
    auVar60._8_8_ = auVar61._8_8_;
    auVar60._0_8_ = (double)*(float *)(lVar4 + 0x60);
    in_stack_fffffffffffffae8 = local_4c8;
    _local_4c8 = auVar60;
    _local_4a8 = auVar60;
    cVar26 = FUN_140647fb0(param_1 + 0x638,param_2,param_5,1,in_stack_fffffffffffffae8);
    if (cVar26 != '\0') {
      auVar60 = ZEXT416(*(uint *)(lVar4 + 0x5c));
      in_stack_fffffffffffffae8 =
           (undefined1 *)
           CONCAT44((int)((ulonglong)in_stack_fffffffffffffae8 >> 0x20),
                    *(undefined4 *)(lVar4 + 0x50));
      FUN_14073f9b0(lVar49,*(undefined4 *)(lVar4 + 0x44),*(undefined4 *)(lVar4 + 0x48),
                    *(undefined4 *)(lVar4 + 0x4c),in_stack_fffffffffffffae8,
                    *(undefined4 *)(lVar4 + 0x54),*(undefined1 *)(lVar4 + 0x58),
                    *(uint *)(lVar4 + 0x5c),*(undefined4 *)(lVar4 + 100),
                    *(undefined4 *)(lVar4 + 0x68),*(undefined4 *)(lVar4 + 0x6c),
                    (float)*(double *)(param_1 + 0x690),*(undefined8 *)(lVar4 + 0xd0));
    }
  }
  else {
    FUN_14055b8e0(param_5,&DAT_140b8e0e0);
    pcVar5 = *(code **)(*param_5 + 0xa0);
    auVar10._4_8_ = auVar56._8_8_;
    auVar10._0_4_ = local_4c8._4_4_;
    auVar59._0_8_ = auVar10._0_8_ << 0x20;
    auVar59._8_4_ = local_4c8._8_4_;
    auVar59._12_4_ = uStack_4bc;
    auVar60._4_12_ = auVar59._4_12_;
    auVar60._0_4_ = *(undefined4 *)(lVar4 + 0x60);
    LStack_4a0 = (LARGE_INTEGER)local_4c8;
    _local_4a8 = (void *)0x9;
    local_468.QuadPart = 0x140a8fcc0;
    LStack_460.QuadPart = 7;
    _local_4c8 = auVar60;
    FUN_140021e90(&local_440,&local_468,local_4a8);
    (*pcVar5)(param_5,&local_440,0x11);
    FUN_140002d30(&local_440);
  }
  (**(code **)(*param_5 + 0x18))(param_5);
  FUN_14055b8e0(param_5,&DAT_140b8dfe0);
  (**(code **)(*param_5 + 8))(param_5);
  auVar65 = _local_4a8;
  if (*(longlong *)(param_1 + 0x750) == 0) {
    LStack_4a0.s.LowPart._0_1_ = 1;
    auVar18 = _local_4a8;
    uStack_4a4 = auVar65._4_4_;
    LStack_4a0._0_4_ = auVar18._8_4_;
    LStack_4a0._4_4_ = auVar65._12_4_;
    auVar12._4_8_ = auVar60._8_8_;
    auVar12._0_4_ = uStack_4a4;
    auVar62._0_8_ = auVar12._0_8_ << 0x20;
    auVar62._8_4_ = LStack_4a0._0_4_;
    auVar62._12_4_ = LStack_4a0._4_4_;
    auVar60._8_8_ = auVar62._8_8_;
    auVar60._0_8_ = *(undefined8 *)(lVar4 + 0xd0);
    _local_4c8 = auVar60;
    _local_4a8 = auVar60;
    FUN_140647d20(param_1 + 0x700,1,local_4c8);
    *(undefined1 *)(param_1 + 0x760) = 1;
    *(undefined1 *)(param_1 + 0x7a8) = 0;
  }
  FUN_14055b8e0(param_5,&DAT_140b8e060);
  pcVar5 = *(code **)(*param_5 + 0xa8);
  uVar31 = FUN_1402d82e0("option_company_train_braking_factor","Train braking factor:");
  (*pcVar5)(param_5,uVar31,0x14);
  auVar65 = _local_4a8;
  if (*(longlong *)(*(longlong *)(local_res18 + 0x250) + 0x28) == 0) {
    LStack_4a0.s.LowPart._0_1_ = 1;
    auVar18 = _local_4a8;
    uStack_4a4 = auVar65._4_4_;
    LStack_4a0._0_4_ = auVar18._8_4_;
    LStack_4a0._4_4_ = auVar65._12_4_;
    auVar14._4_8_ = auVar60._8_8_;
    auVar14._0_4_ = uStack_4a4;
    auVar64._0_8_ = auVar14._0_8_ << 0x20;
    auVar64._8_4_ = LStack_4a0._0_4_;
    auVar64._12_4_ = LStack_4a0._4_4_;
    LStack_4a0 = (LARGE_INTEGER)auVar64._8_8_;
    _local_4a8 = (void *)*(undefined8 *)(lVar4 + 0xd0);
    stack0xfffffffffffffb40 = (undefined8 *)auVar64._8_8_;
    local_4c8._0_8_ = *(undefined8 *)(lVar4 + 0xd0);
    in_stack_fffffffffffffae8 = local_4c8;
    cVar26 = FUN_140647fb0(param_1 + 0x700,param_2,param_5,1,in_stack_fffffffffffffae8);
    lVar49 = local_res18;
    if (cVar26 != '\0') {
      in_stack_fffffffffffffae8 =
           (undefined1 *)
           CONCAT44((int)((ulonglong)in_stack_fffffffffffffae8 >> 0x20),
                    *(undefined4 *)(lVar4 + 0x50));
      FUN_14073f9b0(local_res18,*(undefined4 *)(lVar4 + 0x44),*(undefined4 *)(lVar4 + 0x48),
                    *(undefined4 *)(lVar4 + 0x4c),in_stack_fffffffffffffae8,
                    *(undefined4 *)(lVar4 + 0x54),*(undefined1 *)(lVar4 + 0x58),
                    *(undefined4 *)(lVar4 + 0x5c),*(undefined4 *)(lVar4 + 100),
                    *(undefined4 *)(lVar4 + 0x68),*(undefined4 *)(lVar4 + 0x6c),
                    *(undefined4 *)(lVar4 + 0x60),*(undefined8 *)(param_1 + 0x758));
    }
  }
  else {
    FUN_14055b8e0(param_5,&DAT_140b8e0e0);
    pcVar5 = *(code **)(*param_5 + 0xa0);
    auVar13._4_8_ = auVar60._8_8_;
    auVar13._0_4_ = local_4c8._4_4_;
    auVar63._0_8_ = auVar13._0_8_ << 0x20;
    auVar63._8_4_ = local_4c8._8_4_;
    auVar63._12_4_ = uStack_4bc;
    stack0xfffffffffffffb40 = (undefined8 *)auVar63._8_8_;
    local_4c8._0_8_ = *(undefined8 *)(lVar4 + 0xd0);
    LStack_4a0 = (LARGE_INTEGER)local_4c8;
    _local_4a8 = (void *)0xa;
    local_468.QuadPart = 0x140a8fcc0;
    LStack_460.QuadPart = 7;
    FUN_140021e90(&local_440,&local_468,local_4a8);
    (*pcVar5)(param_5,&local_440,0x11);
    FUN_140002d30(&local_440);
    lVar49 = local_res18;
  }
  (**(code **)(*param_5 + 0x18))(param_5);
  FUN_14055b8e0(param_5,&DAT_140b8dfe0);
  (**(code **)(*param_5 + 8))(param_5);
  FUN_14055b8e0(param_5,&DAT_140b8e060);
  pcVar5 = *(code **)(*param_5 + 0xa8);
  uVar31 = FUN_1402d82e0("option_company_pax_spawn_station_pick","Pax spawn station:");
  (*pcVar5)(param_5,uVar31,0x14);
  _local_4c8 = ZEXT816(0);
  local_4b8 = (undefined8 *)0x0;
  lVar33 = FUN_1402d82e0("option_company_pax_spawn_station_random","Random");
  _local_4a8 = ZEXT816(0);
  local_498 = ZEXT816(0);
  lVar50 = -1;
  do {
    lVar50 = lVar50 + 1;
  } while (*(char *)(lVar33 + lVar50) != '\0');
  FUN_140002c00(local_4a8,lVar33);
  puVar38 = stack0xfffffffffffffb40;
  if (stack0xfffffffffffffb40 < local_4b8) {
    stack0xfffffffffffffb40 = stack0xfffffffffffffb40 + 4;
    *puVar38 = _local_4a8;
    puVar38[1] = LStack_4a0;
    puVar38[2] = local_498._0_8_;
    puVar38[3] = local_498._8_8_;
    auVar20[0xf] = 0;
    auVar20._0_15_ = stack0xfffffffffffffb59;
    _local_4a8 = auVar20 << 8;
    uVar51 = 0xf;
  }
  else {
    FUN_1404ba220(local_4c8,local_4a8);
    uVar51 = local_498._8_8_;
  }
  if (0xf < uVar51) {
    FUN_140003040(local_4a8,_local_4a8);
  }
  lVar33 = FUN_1402d82e0("option_company_pax_spawn_station_shortest_walk","Shortest walk");
  _local_4a8 = ZEXT816(0);
  local_498 = ZEXT816(0);
  lVar50 = -1;
  do {
    lVar50 = lVar50 + 1;
  } while (*(char *)(lVar33 + lVar50) != '\0');
  FUN_140002c00(local_4a8,lVar33);
  puVar38 = stack0xfffffffffffffb40;
  if (stack0xfffffffffffffb40 < local_4b8) {
    stack0xfffffffffffffb40 = stack0xfffffffffffffb40 + 4;
    *puVar38 = _local_4a8;
    puVar38[1] = LStack_4a0;
    puVar38[2] = local_498._0_8_;
    puVar38[3] = local_498._8_8_;
    auVar21[0xf] = 0;
    auVar21._0_15_ = stack0xfffffffffffffb59;
    _local_4a8 = auVar21 << 8;
    uVar51 = 0xf;
  }
  else {
    FUN_1404ba220(local_4c8,local_4a8);
    uVar51 = local_498._8_8_;
  }
  if (0xf < uVar51) {
    FUN_140003040(local_4a8,_local_4a8);
  }
  lVar33 = FUN_1402d82e0("option_company_pax_spawn_station_shortest_walk_best_path",
                         "Shortest walk, best path");
  _local_4a8 = ZEXT816(0);
  local_498 = ZEXT816(0);
  lVar50 = -1;
  do {
    lVar50 = lVar50 + 1;
  } while (*(char *)(lVar33 + lVar50) != '\0');
  FUN_140002c00(local_4a8,lVar33);
  puVar38 = stack0xfffffffffffffb40;
  if (stack0xfffffffffffffb40 < local_4b8) {
    stack0xfffffffffffffb40 = stack0xfffffffffffffb40 + 4;
    *puVar38 = _local_4a8;
    puVar38[1] = LStack_4a0;
    *(undefined1 (*) [16])(puVar38 + 2) = local_498;
    auVar22[0xf] = 0;
    auVar22._0_15_ = stack0xfffffffffffffb59;
    _local_4a8 = auVar22 << 8;
    uVar51 = 0xf;
  }
  else {
    FUN_1404ba220(local_4c8,local_4a8);
    uVar51 = local_498._8_8_;
  }
  if (0xf < uVar51) {
    FUN_140003040(local_4a8,_local_4a8);
  }
  if (*(longlong *)(*(longlong *)(lVar49 + 0x250) + 0x28) == 0) {
    FUN_14055b8e0(param_5,&DAT_140b8e0e0);
    iVar52 = (**(code **)(*param_5 + 0x120))(param_5,local_4c8,*(undefined4 *)(lVar4 + 0xd8));
    if (iVar52 != *(int *)(lVar4 + 0xd8)) {
      lVar49 = FUN_140748820(lVar49 + 0x228);
      local_res20._0_5_ = CONCAT14(1,iVar52);
      *(undefined8 *)(lVar49 + 0xbc8) = local_res20;
    }
  }
  else {
    FUN_14055b8e0(param_5,&DAT_140b8e0e0);
    (**(code **)(*param_5 + 0xa0))
              (param_5,(longlong)*(int *)(lVar4 + 0xd8) * 0x20 + local_4c8._0_8_,0x11);
  }
  pvVar47 = stack0xfffffffffffffb40;
  pvVar45 = (void *)local_4c8._0_8_;
  if ((undefined8 *)local_4c8._0_8_ != stack0xfffffffffffffb40) {
    do {
      FUN_140002d30(pvVar45);
      pvVar45 = (void *)((longlong)pvVar45 + 0x20);
    } while (pvVar45 != pvVar47);
  }
  if ((void *)local_4c8._0_8_ != (void *)0x0) {
    free((void *)local_4c8._0_8_);
  }
  (**(code **)(*param_5 + 0x18))(param_5);
  FUN_14055b8e0(param_5,&DAT_140b8dfe0);
  (**(code **)(*param_5 + 8))(param_5);
  FUN_14055b8e0(param_5,&DAT_140b8e060);
  pcVar5 = *(code **)(*param_5 + 0xa8);
  uVar31 = FUN_1402d82e0("option_company_demand","Global demand factor:");
  (*pcVar5)(param_5,uVar31,0x14);
  piVar24 = DAT_140be3320;
  uVar51 = _UNK_140aac918;
  uVar31 = _DAT_140aac910;
  if (*(longlong *)(*(longlong *)(local_res18 + 0x250) + 0x28) == 0) {
    _local_4a8 = ZEXT816(0);
    auVar23._8_8_ = 0;
    auVar23._0_8_ = local_498._8_8_;
    local_498 = auVar23 << 0x40;
    for (piVar46 = DAT_140be3318; piVar46 != piVar24; piVar46 = piVar46 + 1) {
      uVar28 = *piVar46 / 10;
      uVar48 = uVar28;
      if ((int)uVar28 < 0) {
        uVar48 = -uVar28;
      }
      local_468.QuadPart = local_468.QuadPart & 0xffffffff00000000;
      iVar52 = 0x1f;
      if ((uVar48 | 1) != 0) {
        for (; (uVar48 | 1) >> iVar52 == 0; iVar52 = iVar52 + -1) {
        }
      }
      puVar41 = &local_400;
      if ((int)uVar28 < 0) {
        local_400 = 0x2d;
        puVar41 = local_3ff;
      }
      lVar49 = FUN_140023c60(local_4c8,puVar41,(ulonglong)uVar48,
                             *(longlong *)(&DAT_140a49580 + (longlong)iVar52 * 8) +
                             (ulonglong)uVar48 >> 0x20);
      local_3e0 = 0;
      uStack_3d8 = 0;
      local_3d0 = 0;
      uStack_3c8 = 0;
      if (&local_400 == *(undefined1 **)(lVar49 + 8)) {
        local_3d0 = uVar31;
        uStack_3c8 = uVar51;
        local_3e0 = 0;
      }
      else {
        FUN_140002c00(&local_3e0,&local_400,
                      (longlong)*(undefined1 **)(lVar49 + 8) - (longlong)&local_400);
      }
      puVar35 = (ulonglong *)FUN_1400254f0(&local_3e0,&DAT_140a49d24,1);
      local_440 = *puVar35;
      uStack_438 = puVar35[1];
      local_430 = *(undefined1 (*) [16])(puVar35 + 2);
      *(undefined1 *)puVar35 = 0;
      puVar35[2] = 0;
      puVar35[3] = 0xf;
      LVar70 = LStack_4a0;
      if ((ulonglong)LStack_4a0 < (ulonglong)local_498._0_8_) {
        LStack_4a0 = (LARGE_INTEGER)((longlong)LStack_4a0 + 0x20);
        *(ulonglong *)LVar70 = local_440;
        *(ulonglong *)((longlong)LVar70 + 8) = uStack_438;
        *(undefined1 (*) [16])((longlong)LVar70 + 0x10) = local_430;
        local_430._8_8_ = uVar51;
        local_430._0_8_ = uVar31;
        local_440 = local_440 & 0xffffffffffffff00;
      }
      else {
        FUN_1404ba220(local_4a8,&local_440);
      }
      FUN_140002d30(&local_440);
      if (0xf < uStack_3c8) {
        FUN_140003040(&local_3e0,local_3e0);
      }
    }
    local_448.s.LowPart = 0;
    iVar52 = (int)((longlong)DAT_140be3320 - (longlong)DAT_140be3318 >> 2);
    if (0 < iVar52) {
      do {
        if (DAT_140be3318[(int)local_448.s.LowPart] == *(int *)(lVar4 + 0x44)) break;
        local_448.s.LowPart = local_448.s.LowPart + 1;
      } while ((int)local_448.s.LowPart < iVar52);
    }
    local_468.s.LowPart = iVar52 + -1;
    local_3f0.s.LowPart = 0;
    pLVar34 = &local_3f0;
    if (-1 < (int)local_448.s.LowPart) {
      pLVar34 = &local_448;
    }
    pLVar36 = &local_468;
    if ((int)local_448.s.LowPart <= iVar52 + -1) {
      pLVar36 = pLVar34;
    }
    DVar1 = (pLVar36->s).LowPart;
    FUN_14055b8e0(param_5,&DAT_140b8e0e0);
    iVar52 = (**(code **)(*param_5 + 0x120))(param_5,local_4a8,DVar1);
    if (((-1 < iVar52) && (iVar52 < (int)((longlong)DAT_140be3320 - (longlong)DAT_140be3318 >> 2)))
       && (DAT_140be3318[iVar52] != *(int *)(lVar4 + 0x44))) {
      in_stack_fffffffffffffae8 =
           (undefined1 *)
           CONCAT44((int)((ulonglong)in_stack_fffffffffffffae8 >> 0x20),
                    *(undefined4 *)(lVar4 + 0x50));
      FUN_14073f9b0(local_res18,DAT_140be3318[iVar52],*(undefined4 *)(lVar4 + 0x48),
                    *(undefined4 *)(lVar4 + 0x4c),in_stack_fffffffffffffae8,
                    *(undefined4 *)(lVar4 + 0x54),*(undefined1 *)(lVar4 + 0x58),
                    *(undefined4 *)(lVar4 + 0x5c),*(undefined4 *)(lVar4 + 100),
                    *(undefined4 *)(lVar4 + 0x68),*(undefined4 *)(lVar4 + 0x6c),
                    *(undefined4 *)(lVar4 + 0x60),*(undefined8 *)(lVar4 + 0xd0));
    }
    LVar70 = LStack_4a0;
    pvVar47 = _local_4a8;
    if ((LARGE_INTEGER)_local_4a8 != LStack_4a0) {
      do {
        FUN_140002d30(pvVar47);
        pvVar47 = (void *)((longlong)pvVar47 + 0x20);
      } while ((LARGE_INTEGER)pvVar47 != LVar70);
    }
    if (_local_4a8 != (void *)0x0) {
      free(_local_4a8);
    }
  }
  else {
    FUN_14055b8e0(param_5,&DAT_140b8e0e0);
    pcVar5 = *(code **)(*param_5 + 0xa0);
    uVar31 = FUN_14028fde0(local_3a0,*(int *)(lVar4 + 0x44) / 10);
    pLVar34 = (LARGE_INTEGER *)FUN_1400254f0(uVar31,&DAT_140a49d24,1);
    local_468 = (LARGE_INTEGER)pLVar34->QuadPart;
    LStack_460 = pLVar34[1];
    local_458 = pLVar34[2];
    LStack_450 = pLVar34[3];
    *(undefined1 *)pLVar34 = 0;
    pLVar34[2].QuadPart = 0;
    pLVar34[3].QuadPart = 0xf;
    (*pcVar5)(param_5,&local_468,0x11);
    if (0xf < (ulonglong)LStack_450) {
      FUN_140003040(&local_468,local_468.QuadPart);
    }
    uVar51 = _UNK_140aac918;
    uVar31 = _DAT_140aac910;
    local_458 = (LARGE_INTEGER)_DAT_140aac910;
    LStack_450 = (LARGE_INTEGER)_UNK_140aac918;
    local_468.QuadPart = local_468.QuadPart & 0xffffffffffffff00;
    FUN_140002d30(local_3a0);
  }
  (**(code **)(*param_5 + 0x18))(param_5);
  FUN_14055b8e0(param_5,&DAT_140b8dfe0);
  (**(code **)(*param_5 + 8))(param_5);
  FUN_14055b8e0(param_5,&DAT_140b8e060);
  (**(code **)(*param_5 + 200))(param_5);
  FUN_14055b8e0(param_5,&DAT_140b8e0e0);
  pcVar5 = *(code **)(*param_5 + 0x90);
  cVar26 = *(char *)(param_1 + 0xc0);
  uVar37 = FUN_1402d82e0("option_company_demand_editor","Demand curve editor");
  cVar26 = (*pcVar5)(param_5,uVar37,-(cVar26 != '\0') & 2);
  if (cVar26 != '\0') {
    *(bool *)(param_1 + 0xc0) = *(char *)(param_1 + 0xc0) == '\0';
  }
  (**(code **)(*param_5 + 0x18))(param_5);
  FUN_14055b8e0(param_5,&DAT_140b8dfe0);
  (**(code **)(*param_5 + 8))(param_5);
  FUN_14055b8e0(param_5,&DAT_140b8e060);
  (**(code **)(*param_5 + 200))(param_5);
  FUN_14055b8e0(param_5,&DAT_140b8e0e0);
  pcVar5 = *(code **)(*param_5 + 0x90);
  cVar26 = *(char *)(param_1 + 0x4c0);
  uVar37 = FUN_1402d82e0("option_company_mod_poi_layer_editor","Mod POI layer options");
  cVar26 = (*pcVar5)(param_5,uVar37,-(cVar26 != '\0') & 2);
  if (cVar26 != '\0') {
    *(bool *)(param_1 + 0x4c0) = *(char *)(param_1 + 0x4c0) == '\0';
  }
  (**(code **)(*param_5 + 0x18))(param_5);
  FUN_14055b8e0(param_5,&DAT_140b8dfe0);
  (**(code **)(*param_5 + 8))(param_5);
  FUN_14055b8e0(param_5,&DAT_140b8e060);
  pcVar5 = *(code **)(*param_5 + 0xa8);
  uVar37 = FUN_1402d82e0("option_company_pax_pf_bias","Pax pathfind bias:");
  (*pcVar5)(param_5,uVar37,0x14);
  lVar49 = local_res18;
  uVar73 = DAT_140aabae4;
  uVar29 = (undefined4)((ulonglong)in_stack_fffffffffffffae8 >> 0x20);
  if (*(longlong *)(*(longlong *)(local_res18 + 0x250) + 0x28) == 0) {
    FUN_14055b8e0(param_5,&DAT_140b8e0e0);
    *(undefined1 *)(param_5 + 4) = 1;
    *(undefined4 *)((longlong)param_5 + 0x24) = 2;
    *(undefined1 *)(param_5 + 3) = 1;
    *(undefined4 *)((longlong)param_5 + 0x1c) = 0x1e0;
    (**(code **)(*param_5 + 8))(param_5);
    uVar71 = *(undefined4 *)(lVar4 + 100);
    if (*(char *)(param_1 + 0x83c) != '\0') {
      uVar71 = *(undefined4 *)(param_1 + 0x838);
    }
    local_res20 = CONCAT44(local_res20._4_4_,uVar71);
    *(undefined1 *)(param_5 + 3) = 1;
    *(undefined4 *)((longlong)param_5 + 0x1c) = 0x1e0;
    in_stack_fffffffffffffae8 = (undefined1 *)CONCAT44(uVar29,DAT_140aab6f0);
    (**(code **)(*param_5 + 0xf8))(param_5);
    cVar26 = (**(code **)(*param_5 + 0x170))(param_5);
    if (cVar26 == '\0') {
LAB_140665edd:
    }
    else if ((float)local_res20 != *(float *)(lVar4 + 100)) {
      local_468.s.HighPart._0_1_ = 1;
      local_468.s.LowPart = (DWORD)(float)local_res20;
      *(LARGE_INTEGER *)(param_1 + 0x838) = local_468;
      dVar53 = (double)FUN_140249580(param_1 + 0x830);
      if ((DAT_140aabd08 <= dVar53) && ((char)param_2[0xe] == '\0')) {
        in_stack_fffffffffffffae8 =
             (undefined1 *)
             CONCAT44((int)((ulonglong)in_stack_fffffffffffffae8 >> 0x20),
                      *(undefined4 *)(lVar4 + 0x50));
        FUN_14073f9b0(local_res18,*(undefined4 *)(lVar4 + 0x44),*(undefined4 *)(lVar4 + 0x48),
                      *(undefined4 *)(lVar4 + 0x4c),in_stack_fffffffffffffae8,
                      *(undefined4 *)(lVar4 + 0x54),*(undefined1 *)(lVar4 + 0x58),
                      *(undefined4 *)(lVar4 + 0x5c),(float)local_res20,*(undefined4 *)(lVar4 + 0x68)
                      ,*(undefined4 *)(lVar4 + 0x6c),*(undefined4 *)(lVar4 + 0x60),
                      *(undefined8 *)(lVar4 + 0xd0));
        local_468.QuadPart = 0;
        QueryPerformanceCounter(&local_468);
        ((LARGE_INTEGER *)(param_1 + 0x830))->QuadPart = (LONGLONG)local_468;
        *(undefined1 *)(param_1 + 0x83c) = 0;
      }
      goto LAB_140665edd;
    }
    *(undefined4 *)((longlong)param_5 + 0x2c) = 0x420c0000;
    *(undefined1 *)(param_5 + 5) = 1;
    pcVar5 = *(code **)(*param_5 + 0xa0);
    uVar29 = lroundf((float)local_res20 * DAT_140aac3c0);
    uVar37 = FUN_14028fde0(local_420,uVar29);
    puVar38 = (undefined8 *)FUN_1400254f0(uVar37,&DAT_140a49d24,1);
    local_488 = (void *)*puVar38;
    uStack_480 = puVar38[1];
    local_478._0_8_ = puVar38[2];
    local_478._8_8_ = puVar38[3];
    *(undefined1 *)puVar38 = 0;
    puVar38[2] = 0;
    puVar38[3] = 0xf;
    (*pcVar5)(param_5,&local_488);
    if (0xf < (ulonglong)local_478._8_8_) {
      pvVar47 = local_488;
      if ((0xfff < local_478._8_8_ + 1) &&
         (pvVar47 = *(void **)((longlong)local_488 + -8),
         0x1f < (ulonglong)((longlong)local_488 + (-8 - (longlong)pvVar47)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pvVar47);
    }
    local_478._8_8_ = uVar51;
    local_478._0_8_ = uVar31;
    local_488 = (void *)((ulonglong)local_488 & 0xffffffffffffff00);
    if (0xf < local_408) {
      pvVar47 = local_420[0];
      if ((0xfff < local_408 + 1) &&
         (pvVar47 = *(void **)((longlong)local_420[0] + -8),
         0x1f < (ulonglong)((longlong)local_420[0] + (-8 - (longlong)pvVar47)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pvVar47);
    }
    (**(code **)(*param_5 + 0x18))(param_5);
    lVar49 = local_res18;
  }
  else {
    FUN_14055b8e0(param_5,&DAT_140b8e0e0);
    pcVar5 = *(code **)(*param_5 + 0xa0);
    uVar29 = lroundf(*(float *)(lVar4 + 100) * DAT_140aac3c0);
    uVar37 = FUN_14028fde0(local_380,uVar29);
    puVar38 = (undefined8 *)FUN_1400254f0(uVar37,&DAT_140a49d24,1);
    local_3c0 = (void *)*puVar38;
    uStack_3b8 = puVar38[1];
    local_3b0 = puVar38[2];
    uStack_3a8 = puVar38[3];
    *(undefined1 *)puVar38 = 0;
    puVar38[2] = 0;
    puVar38[3] = 0xf;
    (*pcVar5)(param_5,&local_3c0);
    if (0xf < uStack_3a8) {
      pvVar47 = local_3c0;
      if ((0xfff < uStack_3a8 + 1) &&
         (pvVar47 = *(void **)((longlong)local_3c0 + -8),
         0x1f < (ulonglong)((longlong)local_3c0 + (-8 - (longlong)pvVar47)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pvVar47);
    }
    local_3c0 = (void *)((ulonglong)local_3c0 & 0xffffffffffffff00);
    local_3b0 = uVar31;
    uStack_3a8 = uVar51;
    if (0xf < local_368) {
      pvVar47 = local_380[0];
      if ((0xfff < local_368 + 1) &&
         (pvVar47 = *(void **)((longlong)local_380[0] + -8),
         0x1f < (ulonglong)((longlong)local_380[0] + (-8 - (longlong)pvVar47)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pvVar47);
    }
  }
  (**(code **)(*param_5 + 0x18))(param_5);
  if (*(longlong *)(*(longlong *)(lVar49 + 0x250) + 0x28) == 0) {
    FUN_14055b8e0(param_5,&DAT_140b8dfe0);
    *(undefined1 *)(param_5 + 7) = 1;
    *(undefined4 *)(param_5 + 8) = 0;
    (**(code **)(*param_5 + 8))(param_5);
    FUN_14055b8e0(param_5,&DAT_140b8e060);
    (**(code **)(*param_5 + 200))(param_5);
    FUN_14055b8e0(param_5,&DAT_140b8e0e0);
    *(undefined1 *)(param_5 + 4) = 1;
    *(undefined4 *)((longlong)param_5 + 0x24) = 2;
    *(undefined1 *)(param_5 + 3) = 1;
    *(undefined4 *)((longlong)param_5 + 0x1c) = 0x1e0;
    (**(code **)(*param_5 + 8))(param_5);
    *(undefined1 *)(param_5 + 3) = 1;
    *(undefined4 *)((longlong)param_5 + 0x1c) = 0x1e0;
    pcVar5 = *(code **)(*param_5 + 0xa8);
    uVar37 = FUN_1402d82e0("option_company_pax_pf_bias_less","Better paths");
    (*pcVar5)(param_5,uVar37,0x11);
    *(undefined1 *)(param_5 + 3) = 1;
    *(undefined4 *)((longlong)param_5 + 0x1c) = 0x1e0;
    pcVar5 = *(code **)(*param_5 + 0xa8);
    uVar37 = FUN_1402d82e0("option_company_pax_pf_bias_more","Faster sim");
    (*pcVar5)(param_5,uVar37,0x14);
    *(undefined4 *)((longlong)param_5 + 0x2c) = 0x420c0000;
    *(undefined1 *)(param_5 + 5) = 1;
    (**(code **)(*param_5 + 200))(param_5);
    (**(code **)(*param_5 + 0x18))(param_5);
    (**(code **)(*param_5 + 0x18))(param_5);
  }
  lVar33 = FUN_1402d82e0("new_station_defaults","New station defaults");
  local_440 = 0;
  uStack_438 = 0;
  local_430 = ZEXT816(0);
  lVar50 = -1;
  do {
    lVar50 = lVar50 + 1;
  } while (*(char *)(lVar33 + lVar50) != '\0');
  FUN_140002c00(&local_440,lVar33);
  uVar27 = FUN_1405cc370(param_2,param_5,&local_440);
  *(undefined1 *)(param_1 + 0x819) = uVar27;
  if (*(char *)(param_1 + 0x818) != '\0') {
    FUN_14055b8e0(param_5,&DAT_140b8dfe0);
    (**(code **)(*param_5 + 8))(param_5);
    FUN_14055b8e0(param_5,&DAT_140b8e060);
    pcVar5 = *(code **)(*param_5 + 0xa8);
    uVar37 = FUN_1402d82e0("option_company_station_label","Station label:");
    (*pcVar5)(param_5,uVar37,0x14);
    _local_4c8 = ZEXT816(0);
    local_4b8 = (undefined8 *)0x0;
    lVar33 = FUN_1402d82e0("station_label_mode_full",&DAT_140a900e0);
    local_488 = (void *)0x0;
    uStack_480 = 0;
    local_478 = ZEXT816(0);
    lVar50 = -1;
    do {
      lVar50 = lVar50 + 1;
    } while (*(char *)(lVar33 + lVar50) != '\0');
    FUN_140002c00(&local_488,lVar33);
    puVar38 = stack0xfffffffffffffb40;
    if (stack0xfffffffffffffb40 < local_4b8) {
      stack0xfffffffffffffb40 = stack0xfffffffffffffb40 + 4;
      *puVar38 = local_488;
      puVar38[1] = uStack_480;
      puVar38[2] = local_478._0_8_;
      puVar38[3] = local_478._8_8_;
      local_488 = (void *)((ulonglong)local_488 & 0xffffffffffffff00);
      uVar42 = 0xf;
    }
    else {
      FUN_1404ba220(local_4c8,&local_488);
      uVar42 = local_478._8_8_;
    }
    if (0xf < uVar42) {
      pvVar47 = local_488;
      if ((0xfff < uVar42 + 1) &&
         (pvVar47 = *(void **)((longlong)local_488 + -8),
         0x1f < (ulonglong)((longlong)local_488 + (-8 - (longlong)pvVar47)))) goto LAB_140666373;
      free(pvVar47);
    }
    lVar33 = FUN_1402d82e0("station_label_mode_name_pop","Name and pax");
    local_488 = (void *)0x0;
    uStack_480 = 0;
    local_478 = ZEXT816(0);
    lVar50 = -1;
    do {
      lVar50 = lVar50 + 1;
    } while (*(char *)(lVar33 + lVar50) != '\0');
    FUN_140002c00(&local_488,lVar33);
    puVar38 = stack0xfffffffffffffb40;
    if (stack0xfffffffffffffb40 < local_4b8) {
      stack0xfffffffffffffb40 = stack0xfffffffffffffb40 + 4;
      *puVar38 = local_488;
      puVar38[1] = uStack_480;
      puVar38[2] = local_478._0_8_;
      puVar38[3] = local_478._8_8_;
      local_488 = (void *)((ulonglong)local_488 & 0xffffffffffffff00);
      uVar42 = 0xf;
    }
    else {
      FUN_1404ba220(local_4c8,&local_488);
      uVar42 = local_478._8_8_;
    }
    if (0xf < uVar42) {
      pvVar47 = local_488;
      if ((0xfff < uVar42 + 1) &&
         (pvVar47 = *(void **)((longlong)local_488 + -8),
         0x1f < (ulonglong)((longlong)local_488 + (-8 - (longlong)pvVar47)))) {
LAB_140666373:
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pvVar47);
    }
    lVar33 = FUN_1402d82e0("station_label_mode_name_only","Name only");
    local_488 = (void *)0x0;
    uStack_480 = 0;
    local_478 = ZEXT816(0);
    lVar50 = -1;
    do {
      lVar50 = lVar50 + 1;
    } while (*(char *)(lVar33 + lVar50) != '\0');
    FUN_140002c00(&local_488,lVar33);
    puVar38 = stack0xfffffffffffffb40;
    if (stack0xfffffffffffffb40 < local_4b8) {
      stack0xfffffffffffffb40 = stack0xfffffffffffffb40 + 4;
      *puVar38 = local_488;
      puVar38[1] = uStack_480;
      puVar38[2] = local_478._0_8_;
      puVar38[3] = local_478._8_8_;
      local_488 = (void *)((ulonglong)local_488 & 0xffffffffffffff00);
      uVar42 = 0xf;
    }
    else {
      FUN_1404ba220(local_4c8,&local_488);
      uVar42 = local_478._8_8_;
    }
    if (0xf < uVar42) {
      pvVar47 = local_488;
      if ((0xfff < uVar42 + 1) &&
         (pvVar47 = *(void **)((longlong)local_488 + -8),
         0x1f < (ulonglong)((longlong)local_488 + (-8 - (longlong)pvVar47)))) goto LAB_140666596;
      free(pvVar47);
    }
    lVar33 = FUN_1402d82e0("station_label_mode_icon_only","Icon only");
    local_488 = (void *)0x0;
    uStack_480 = 0;
    local_478 = ZEXT816(0);
    lVar50 = -1;
    do {
      lVar50 = lVar50 + 1;
    } while (*(char *)(lVar33 + lVar50) != '\0');
    FUN_140002c00(&local_488,lVar33);
    puVar38 = stack0xfffffffffffffb40;
    if (stack0xfffffffffffffb40 < local_4b8) {
      stack0xfffffffffffffb40 = stack0xfffffffffffffb40 + 4;
      *puVar38 = local_488;
      puVar38[1] = uStack_480;
      puVar38[2] = local_478._0_8_;
      puVar38[3] = local_478._8_8_;
      local_488 = (void *)((ulonglong)local_488 & 0xffffffffffffff00);
      uVar42 = 0xf;
    }
    else {
      FUN_1404ba220(local_4c8,&local_488);
      uVar42 = local_478._8_8_;
    }
    if (0xf < uVar42) {
      pvVar47 = local_488;
      if ((0xfff < uVar42 + 1) &&
         (pvVar47 = *(void **)((longlong)local_488 + -8),
         0x1f < (ulonglong)((longlong)local_488 + (-8 - (longlong)pvVar47)))) goto LAB_140666596;
      free(pvVar47);
    }
    lVar33 = FUN_1402d82e0("station_label_mode_dot_only","Dot only");
    local_488 = (void *)0x0;
    uStack_480 = 0;
    auVar65 = ZEXT816(0);
    do {
      lVar32 = lVar32 + 1;
    } while (*(char *)(lVar33 + lVar32) != '\0');
    local_478 = auVar65;
    FUN_140002c00(&local_488,lVar33);
    puVar38 = stack0xfffffffffffffb40;
    if (stack0xfffffffffffffb40 < local_4b8) {
      stack0xfffffffffffffb40 = stack0xfffffffffffffb40 + 4;
      *puVar38 = local_488;
      puVar38[1] = uStack_480;
      *(undefined1 (*) [16])(puVar38 + 2) = local_478;
      local_488 = (void *)((ulonglong)local_488 & 0xffffffffffffff00);
      uVar42 = 0xf;
      auVar65 = local_478;
    }
    else {
      FUN_1404ba220(local_4c8,&local_488);
      uVar42 = local_478._8_8_;
    }
    if (0xf < uVar42) {
      pvVar47 = local_488;
      if ((0xfff < uVar42 + 1) &&
         (pvVar47 = *(void **)((longlong)local_488 + -8),
         0x1f < (ulonglong)((longlong)local_488 + (-8 - (longlong)pvVar47)))) {
LAB_140666596:
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pvVar47);
    }
    if (*(longlong *)(*(longlong *)(lVar49 + 0x250) + 0x28) == 0) {
      FUN_14055b8e0(param_5,&DAT_140b8e0e0);
      iVar52 = (**(code **)(*param_5 + 0x120))(param_5,local_4c8,*(undefined4 *)(lVar4 + 0x48));
      if (iVar52 != *(int *)(lVar4 + 0x48)) {
        auVar65 = ZEXT416(*(uint *)(lVar4 + 0x5c));
        in_stack_fffffffffffffae8 =
             (undefined1 *)
             CONCAT44((int)((ulonglong)in_stack_fffffffffffffae8 >> 0x20),
                      *(undefined4 *)(lVar4 + 0x50));
        FUN_14073f9b0(lVar49,*(undefined4 *)(lVar4 + 0x44),iVar52,*(undefined4 *)(lVar4 + 0x4c),
                      in_stack_fffffffffffffae8,*(undefined4 *)(lVar4 + 0x54),
                      *(undefined1 *)(lVar4 + 0x58),*(uint *)(lVar4 + 0x5c),
                      *(undefined4 *)(lVar4 + 100),*(undefined4 *)(lVar4 + 0x68),
                      *(undefined4 *)(lVar4 + 0x6c),*(undefined4 *)(lVar4 + 0x60),
                      *(undefined8 *)(lVar4 + 0xd0));
      }
    }
    else {
      FUN_14055b8e0(param_5,&DAT_140b8e0e0);
      (**(code **)(*param_5 + 0xa0))
                (param_5,(longlong)*(int *)(lVar4 + 0x48) * 0x20 + local_4c8._0_8_,0x11);
    }
    uVar29 = (undefined4)((ulonglong)in_stack_fffffffffffffae8 >> 0x20);
    pvVar47 = stack0xfffffffffffffb40;
    pvVar45 = (void *)local_4c8._0_8_;
    if ((undefined8 *)local_4c8._0_8_ != stack0xfffffffffffffb40) {
      do {
        FUN_140002d30(pvVar45);
        uVar29 = (undefined4)((ulonglong)in_stack_fffffffffffffae8 >> 0x20);
        pvVar45 = (void *)((longlong)pvVar45 + 0x20);
      } while (pvVar45 != pvVar47);
    }
    if ((void *)local_4c8._0_8_ != (void *)0x0) {
      free((void *)local_4c8._0_8_);
    }
    (**(code **)(*param_5 + 0x18))(param_5);
    FUN_14055b8e0(param_5,&DAT_140b8dfe0);
    (**(code **)(*param_5 + 8))(param_5);
    FUN_14055b8e0(param_5,&DAT_140b8e060);
    pcVar5 = *(code **)(*param_5 + 0xa8);
    uVar37 = FUN_1402d82e0("station_pop_area","Population radius:");
    (*pcVar5)(param_5,uVar37,0x14);
    if (*(longlong *)(*(longlong *)(local_res18 + 0x250) + 0x28) == 0) {
      local_res20 = CONCAT44(local_res20._4_4_,*(undefined4 *)(lVar4 + 0x4c));
      FUN_14055b8e0(param_5,&DAT_140b8e0e0);
      uVar37 = 0;
      (**(code **)(*param_5 + 0xf8))(param_5);
      *(undefined4 *)((longlong)param_5 + 0x2c) = 0x42380000;
      *(undefined1 *)(param_5 + 5) = 1;
      pcVar5 = *(code **)(*param_5 + 0xa0);
      uVar71 = SUB84(DAT_140aabd98,0);
      uVar72 = (undefined4)((ulonglong)DAT_140aabd98 >> 0x20);
      auVar16._4_8_ = uVar37;
      auVar16._0_4_ = local_4c8._4_4_;
      auVar68._0_8_ = auVar16._0_8_ << 0x20;
      auVar68._8_4_ = local_4c8._8_4_;
      auVar68._12_4_ = uStack_4bc;
      auVar67._8_8_ = auVar68._8_8_;
      auVar67._0_8_ = (double)*(float *)(lVar4 + 0x4c) * DAT_140aabd98;
      LStack_4a0 = (LARGE_INTEGER)local_4c8;
      _local_4a8 = (void *)0xa;
      local_468.QuadPart = (LONGLONG)&DAT_140a8f018;
      LStack_460 = (LARGE_INTEGER)0x9;
      _local_4c8 = auVar67;
      FUN_140021e90(local_420,&local_468,local_4a8);
      (*pcVar5)(param_5,local_420,0x11);
      FUN_140002d30(local_420);
      if ((float)local_res20 != *(float *)(lVar4 + 0x4c)) {
        auVar67 = ZEXT416(*(uint *)(lVar4 + 0x5c));
        uVar37 = CONCAT44(uVar29,*(undefined4 *)(lVar4 + 0x50));
        FUN_14073f9b0(local_res18,*(undefined4 *)(lVar4 + 0x44),*(undefined4 *)(lVar4 + 0x48),
                      (float)local_res20,uVar37,*(undefined4 *)(lVar4 + 0x54),
                      *(undefined1 *)(lVar4 + 0x58),*(uint *)(lVar4 + 0x5c),
                      *(undefined4 *)(lVar4 + 100),*(undefined4 *)(lVar4 + 0x68),
                      *(undefined4 *)(lVar4 + 0x6c),*(undefined4 *)(lVar4 + 0x60),
                      *(undefined8 *)(lVar4 + 0xd0));
        uVar29 = (undefined4)((ulonglong)uVar37 >> 0x20);
      }
    }
    else {
      FUN_14055b8e0(param_5,&DAT_140b8e0e0);
      pcVar5 = *(code **)(*param_5 + 0xa0);
      uVar71 = SUB84(DAT_140aabd98,0);
      uVar72 = (undefined4)((ulonglong)DAT_140aabd98 >> 0x20);
      auVar15._4_8_ = auVar65._8_8_;
      auVar15._0_4_ = local_4c8._4_4_;
      auVar66._0_8_ = auVar15._0_8_ << 0x20;
      auVar66._8_4_ = local_4c8._8_4_;
      auVar66._12_4_ = uStack_4bc;
      auVar67._8_8_ = auVar66._8_8_;
      auVar67._0_8_ = (double)*(float *)(lVar4 + 0x4c) * DAT_140aabd98;
      LStack_4a0 = (LARGE_INTEGER)local_4c8;
      _local_4a8 = (void *)0xa;
      local_468.QuadPart = (LONGLONG)&DAT_140a8f018;
      LStack_460 = (LARGE_INTEGER)0x9;
      _local_4c8 = auVar67;
      FUN_140021e90(local_420,&local_468,local_4a8);
      (*pcVar5)(param_5,local_420,0x11);
      FUN_140002d30(local_420);
    }
    (**(code **)(*param_5 + 0x18))(param_5);
    FUN_14055b8e0(param_5,&DAT_140b8dfe0);
    (**(code **)(*param_5 + 8))(param_5);
    FUN_14055b8e0(param_5,&DAT_140b8e060);
    pcVar5 = *(code **)(*param_5 + 0xa8);
    uVar37 = FUN_1402d82e0("station_max_walk","Auto walk transfer:");
    (*pcVar5)(param_5,uVar37,0x14);
    if (*(longlong *)(*(longlong *)(local_res18 + 0x250) + 0x28) == 0) {
      local_res20 = CONCAT44(local_res20._4_4_,*(undefined4 *)(lVar4 + 0x50));
      FUN_14055b8e0(param_5,&DAT_140b8e0e0);
      (**(code **)(*param_5 + 0xf8))(param_5);
      *(undefined4 *)((longlong)param_5 + 0x2c) = 0x42380000;
      *(undefined1 *)(param_5 + 5) = 1;
      pcVar5 = *(code **)(*param_5 + 0xa0);
      local_4c8._0_8_ = (double)*(float *)(lVar4 + 0x50) * (double)CONCAT44(uVar72,uVar71);
      LStack_4a0 = (LARGE_INTEGER)local_4c8;
      _local_4a8 = (void *)0xa;
      local_468.QuadPart = (LONGLONG)&DAT_140a8f018;
      LStack_460 = (LARGE_INTEGER)0x9;
      FUN_140021e90(local_420,&local_468,local_4a8);
      (*pcVar5)(param_5,local_420,0x11);
      FUN_140002d30(local_420);
      lVar32 = local_res18;
      if ((float)local_res20 != *(float *)(lVar4 + 0x50)) {
        uVar37 = CONCAT44(uVar29,(float)local_res20);
        FUN_14073f9b0(local_res18,*(undefined4 *)(lVar4 + 0x44),*(undefined4 *)(lVar4 + 0x48),
                      *(undefined4 *)(lVar4 + 0x4c),uVar37,*(undefined4 *)(lVar4 + 0x54),
                      *(undefined1 *)(lVar4 + 0x58),*(undefined4 *)(lVar4 + 0x5c),
                      *(undefined4 *)(lVar4 + 100),*(undefined4 *)(lVar4 + 0x68),
                      *(undefined4 *)(lVar4 + 0x6c),*(undefined4 *)(lVar4 + 0x60),
                      *(undefined8 *)(lVar4 + 0xd0));
        uVar29 = (undefined4)((ulonglong)uVar37 >> 0x20);
      }
    }
    else {
      FUN_14055b8e0(param_5,&DAT_140b8e0e0);
      pcVar5 = *(code **)(*param_5 + 0xa0);
      auVar17._4_8_ = auVar67._8_8_;
      auVar17._0_4_ = local_4c8._4_4_;
      auVar69._0_8_ = auVar17._0_8_ << 0x20;
      auVar69._8_4_ = local_4c8._8_4_;
      auVar69._12_4_ = uStack_4bc;
      stack0xfffffffffffffb40 = (undefined8 *)auVar69._8_8_;
      local_4c8._0_8_ = (double)*(float *)(lVar4 + 0x50) * (double)CONCAT44(uVar72,uVar71);
      LStack_4a0 = (LARGE_INTEGER)local_4c8;
      _local_4a8 = (void *)0xa;
      local_468.QuadPart = (LONGLONG)&DAT_140a8f018;
      LStack_460 = (LARGE_INTEGER)0x9;
      FUN_140021e90(local_420,&local_468,local_4a8);
      (*pcVar5)(param_5,local_420,0x11);
      FUN_140002d30(local_420);
      lVar32 = local_res18;
    }
    (**(code **)(*param_5 + 0x18))(param_5);
    FUN_14055b8e0(param_5,&DAT_140b8dfe0);
    (**(code **)(*param_5 + 8))(param_5);
    FUN_14055b8e0(param_5,&DAT_140b8e060);
    pcVar5 = *(code **)(*param_5 + 0xa8);
    uVar37 = FUN_1402d82e0("station_max_plat_pax","Platforms pax capacity:");
    (*pcVar5)(param_5,uVar37,0x14);
    if (*(longlong *)(*(longlong *)(lVar32 + 0x250) + 0x28) == 0) {
      iVar52 = (int)(DAT_140b77ee0 - DAT_140b77ed8 >> 2) + -1;
      local_3f0.s.HighPart = 0;
      lVar49 = DAT_140b77ed8;
      local_3e4 = iVar52;
      lVar33 = FUN_14046f4a0(DAT_140b77ed8,DAT_140b77ee0,lVar4 + 0x54);
      local_3e8 = (int)(lVar33 - lVar49 >> 2);
      pLVar43 = &local_3f0.s.HighPart;
      if (-1 < local_3e8) {
        pLVar43 = &local_3e8;
      }
      pLVar39 = &local_3e4;
      if (local_3e8 <= iVar52) {
        pLVar39 = pLVar43;
      }
      iVar52 = *pLVar39;
      local_res18 = CONCAT44(local_res18._4_4_,(float)iVar52);
      FUN_14055b8e0(param_5,&DAT_140b8e0e0);
      uVar37 = CONCAT44(uVar29,uVar73);
      (**(code **)(*param_5 + 0xf8))(param_5);
      *(undefined4 *)((longlong)param_5 + 0x2c) = 0x428c0000;
      *(undefined1 *)(param_5 + 5) = 1;
      pcVar5 = *(code **)(*param_5 + 0xa0);
      puVar38 = (undefined8 *)
                FUN_140572550(local_3a0,*(undefined4 *)(DAT_140b77ed8 + (longlong)iVar52 * 4));
      local_4c8._0_8_ = puVar38;
      if (0xf < (ulonglong)puVar38[3]) {
        local_4c8._0_8_ = *puVar38;
      }
      stack0xfffffffffffffb40 = (undefined8 *)puVar38[2];
      local_2a8 = 0;
      local_2b8 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
      local_2b0 = local_298;
      local_2a0 = 500;
      LStack_4a0 = (LARGE_INTEGER)local_4c8;
      _local_4a8 = (void *)0xd;
      local_468.QuadPart = 0x140a8f280;
      LStack_460 = (LARGE_INTEGER)0x6;
      FUN_140022830(&local_2b8,&local_468,local_4a8,0,uVar37);
      uVar29 = (undefined4)((ulonglong)uVar37 >> 0x20);
      local_488 = (void *)0x0;
      uStack_480 = 0;
      local_478 = ZEXT816(0);
      FUN_140002c00(&local_488,local_2b0,local_2a8);
      local_2b8 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
      if (local_2b0 != local_298) {
        FUN_140003210(local_298,local_2b0,local_2a0);
      }
      (*pcVar5)(param_5,&local_488,0x11);
      if (0xf < (ulonglong)local_478._8_8_) {
        FUN_140003040(&local_488,local_488);
      }
      local_478._8_8_ = uVar51;
      local_478._0_8_ = uVar31;
      local_488 = (void *)((ulonglong)local_488 & 0xffffffffffffff00);
      FUN_140002d30(local_3a0);
      iVar30 = lroundf();
      if (iVar52 != iVar30) {
        uVar31 = *(undefined8 *)(lVar4 + 0xd0);
        uVar73 = *(undefined4 *)(lVar4 + 0x60);
        uVar2 = *(undefined4 *)(lVar4 + 0x6c);
        uVar3 = *(undefined4 *)(lVar4 + 0x68);
        uVar71 = *(undefined4 *)(lVar4 + 100);
        uVar72 = *(undefined4 *)(lVar4 + 0x5c);
        uVar27 = *(undefined1 *)(lVar4 + 0x58);
        iVar52 = lroundf();
        uVar37 = CONCAT44(uVar29,*(undefined4 *)(lVar4 + 0x50));
        FUN_14073f9b0(lVar32,*(undefined4 *)(lVar4 + 0x44),*(undefined4 *)(lVar4 + 0x48),
                      *(undefined4 *)(lVar4 + 0x4c),uVar37,
                      *(undefined4 *)(DAT_140b77ed8 + (longlong)iVar52 * 4),uVar27,uVar72,uVar71,
                      uVar3,uVar2,uVar73,uVar31);
        uVar29 = (undefined4)((ulonglong)uVar37 >> 0x20);
      }
    }
    else {
      FUN_14055b8e0(param_5,&DAT_140b8e0e0);
      pcVar5 = *(code **)(*param_5 + 0xa0);
      puVar38 = (undefined8 *)FUN_140572550(local_3a0,*(undefined4 *)(lVar4 + 0x54));
      local_4c8._0_8_ = puVar38;
      if (0xf < (ulonglong)puVar38[3]) {
        local_4c8._0_8_ = *puVar38;
      }
      stack0xfffffffffffffb40 = (undefined8 *)puVar38[2];
      LStack_4a0 = (LARGE_INTEGER)local_4c8;
      _local_4a8 = (void *)0xd;
      local_468.QuadPart = 0x140a8f280;
      LStack_460 = (LARGE_INTEGER)0x6;
      FUN_140021e90(local_420,&local_468,local_4a8);
      (*pcVar5)(param_5,local_420,0x11);
      FUN_140002d30(local_420);
      FUN_140002d30(local_3a0);
    }
    (**(code **)(*param_5 + 0x18))(param_5);
    FUN_14055b8e0(param_5,&DAT_140b8dfe0);
    (**(code **)(*param_5 + 8))(param_5);
    if (*(longlong *)(*(longlong *)(lVar32 + 0x250) + 0x28) == 0) {
      FUN_14055b8e0(param_5,&DAT_140b8e060);
      (**(code **)(*param_5 + 200))(param_5);
      local_res18 = CONCAT44(local_res18._4_4_,(uint)*(byte *)(lVar4 + 0x58));
      FUN_14055b8e0(param_5,&DAT_140b8e0e0);
      pcVar5 = *(code **)(*param_5 + 0xf0);
      uVar31 = FUN_1402d82e0("station_transfer_overflow_into_hall","Transfers overflow into hall");
      (*pcVar5)(param_5,uVar31,&local_res18);
      if ((uint)local_res18 != *(byte *)(lVar4 + 0x58)) {
        FUN_14073f9b0(lVar32,*(undefined4 *)(lVar4 + 0x44),*(undefined4 *)(lVar4 + 0x48),
                      *(undefined4 *)(lVar4 + 0x4c),CONCAT44(uVar29,*(undefined4 *)(lVar4 + 0x50)),
                      *(undefined4 *)(lVar4 + 0x54),(uint)local_res18 != 0,
                      *(undefined4 *)(lVar4 + 0x5c),*(undefined4 *)(lVar4 + 100),
                      *(undefined4 *)(lVar4 + 0x68),*(undefined4 *)(lVar4 + 0x6c),
                      *(undefined4 *)(lVar4 + 0x60),*(undefined8 *)(lVar4 + 0xd0));
      }
    }
    else {
      FUN_14055b8e0(param_5,&DAT_140b8e060);
      pcVar5 = *(code **)(*param_5 + 0xa8);
      uVar31 = FUN_1402d82e0("station_transfer_overflow_into_hall","Transfers overflow into hall");
      (*pcVar5)(param_5,uVar31,0x11);
      FUN_14055b8e0(param_5,&DAT_140b8e0e0);
      pcVar5 = *(code **)(*param_5 + 0xa8);
      if (*(char *)(lVar4 + 0x58) == '\0') {
        pcVar44 = "Disabled";
        pcVar40 = "disabled";
      }
      else {
        pcVar44 = "Enabled";
        pcVar40 = "enabled";
      }
      uVar31 = FUN_1402d82e0(pcVar40,pcVar44);
      (*pcVar5)(param_5,uVar31,0x11);
    }
    (**(code **)(*param_5 + 0x18))(param_5);
  }
  return;
}


// Incoming references
// 0xc32750 DATA caller none
// 0x667869 UNCONDITIONAL_CALL caller 140667240
