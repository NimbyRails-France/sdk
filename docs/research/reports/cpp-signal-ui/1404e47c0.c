// Candidate VA 1404e47c0; RVA 0x4e47c0
// Ghidra inferred prototype: undefined FUN_1404e47c0()

/* WARNING: Function: __chkstk replaced with injection: alloca_probe */
/* WARNING: Removing unreachable block (ram,0x0001404e4881) */
/* WARNING: Removing unreachable block (ram,0x0001404e4895) */
/* WARNING: Removing unreachable block (ram,0x0001404e48ae) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_1404e47c0(longlong param_1,undefined8 param_2,undefined8 param_3,undefined4 *param_4)

{
  void *pvVar1;
  undefined8 uVar2;
  ulonglong uVar3;
  longlong lVar4;
  undefined8 uVar5;
  void *pvVar6;
  longlong *plVar7;
  void **local_res8;
  void *local_10e8;
  longlong lStack_10e0;
  longlong local_10d8;
  int local_10d0;
  int iStack_10cc;
  undefined4 local_10c8;
  undefined1 local_10c4;
  undefined1 local_10c3;
  void *local_10c0;
  undefined8 uStack_10b8;
  undefined8 local_10b0;
  ulonglong uStack_10a8;
  void *local_10a0;
  undefined8 uStack_1098;
  undefined8 local_1090;
  ulonglong uStack_1088;
  undefined1 local_1080;
  undefined7 uStack_107f;
  undefined8 uStack_1078;
  undefined8 local_1070;
  ulonglong uStack_1068;
  uint local_1060;
  undefined4 uStack_105c;
  undefined4 uStack_1058;
  undefined4 uStack_1054;
  undefined8 local_1050;
  undefined8 uStack_1048;
  char local_1040;
  undefined8 local_1038;
  uint *local_1030;
  char *local_1028;
  undefined8 local_1020;
  longlong local_1018;
  void *local_1010;
  undefined8 uStack_1008;
  undefined8 local_1000;
  ulonglong uStack_ff8;
  void *local_ff0;
  undefined8 uStack_fe8;
  undefined8 local_fe0;
  ulonglong uStack_fd8;
  void *local_fd0;
  undefined8 uStack_fc8;
  undefined8 local_fc0;
  ulonglong uStack_fb8;
  uint local_fb0;
  undefined4 uStack_fac;
  undefined4 uStack_fa8;
  undefined4 uStack_fa4;
  undefined4 local_fa0;
  undefined4 uStack_f9c;
  undefined4 uStack_f98;
  undefined4 uStack_f94;
  undefined4 local_f90;
  int iStack_f8c;
  longlong local_f88;
  undefined **local_f80;
  basic_ostream<char,std::char_traits<char>_> local_f78 [120];
  basic_ios<char,std::char_traits<char>_> local_f00 [104];
  undefined1 local_e98 [848];
  undefined4 local_b48;
  undefined4 uStack_b44;
  undefined4 uStack_b40;
  undefined4 uStack_b3c;
  undefined4 local_b38;
  undefined4 uStack_b34;
  undefined4 uStack_b30;
  undefined4 uStack_b2c;
  undefined1 local_b28 [2816];

  if (*(char *)(param_1 + 0x58) != '\0') {
    if ((*(longlong *)(param_1 + 0x1d0) != 0) &&
       ((byte)(*(char *)(*(longlong *)(param_1 + 0x1d0) + 8) - 3U) < 2)) {
      FUN_1403638c0(param_1 + 0x1d0);
    }
    lVar4 = DAT_140b77e78;
    pvVar6 = DAT_140b77e70;
    uVar3 = _UNK_140aac918;
    uVar2 = _DAT_140aac910;
    local_10e8 = (void *)0x0;
    lStack_10e0 = 0;
    local_10d8 = 0;
    local_10c3 = 0;
    if (DAT_140b77e95 != '\0') {
      local_10c3 = 0;
      local_10c4 = DAT_140b77e94;
      local_10c8 = DAT_140b77e90;
      iStack_10cc = DAT_140b77e8c;
      local_10d0 = DAT_140b77e88;
      local_10e8 = DAT_140b77e70;
      lStack_10e0 = DAT_140b77e78;
      local_10d8 = DAT_140b77e80;
      DAT_140b77e70 = (void *)0x0;
      DAT_140b77e78 = 0;
      DAT_140b77e80 = 0;
      LOCK();
      DAT_140b77e95 = '\0';
      UNLOCK();
      uStack_1098 = 0;
      local_1090 = _DAT_140aac910;
      uStack_1088 = _UNK_140aac918;
      local_10a0 = (void *)0x0;
      uStack_10b8 = 0;
      local_10b0 = _DAT_140aac910;
      uStack_10a8 = _UNK_140aac918;
      local_10c0 = (void *)0x0;
      if ((DAT_140b77e88 == 0) ||
         ((ulonglong)(lVar4 - (longlong)pvVar6) <
          (ulonglong)(uint)(DAT_140b77e88 * DAT_140b77e8c * 4))) {
        uVar5 = __acrt_iob_func(1);
        local_1038 = 0;
        local_1030 = &local_1060;
        local_1028 = "invalid screenshot!\n";
        local_1020 = 0x14;
        FUN_140021f90(uVar5,&local_1028,&local_1038);
      }
      else {
        local_res8 = &local_10e8;
        FUN_1404e4e80(&local_res8,0x4b0,600,&local_10a0);
        FUN_1404e4e80(&local_res8,400,200);
      }
      FUN_1404b7cb0(&local_f88);
      FUN_1404aafd0(local_e98,param_2,param_3);
      local_b48 = *param_4;
      uStack_b44 = param_4[1];
      uStack_b40 = param_4[2];
      uStack_b3c = param_4[3];
      local_b38 = param_4[4];
      uStack_b34 = param_4[5];
      uStack_b30 = param_4[6];
      uStack_b2c = param_4[7];
      plVar7 = (longlong *)(param_4 + 8);
      if ((longlong *)local_b28 != plVar7) {
        if (0xf < *(ulonglong *)(param_4 + 0xe)) {
          plVar7 = (longlong *)*plVar7;
        }
        FUN_140030630(local_b28,plVar7,*(undefined8 *)(param_4 + 0xc));
      }
      FUN_14049f050(&local_1060,&local_f88,local_e98);
      FUN_1404aa510(local_e98);
      if (local_1040 != '\0') {
        FUN_140002d30(&local_1060);
      }
      FUN_1404e7a00(&local_1080,param_2);
      if (*(char *)(param_1 + 0x50) != '\0') {
        FUN_140002d30(param_1 + 0x30);
        *(undefined1 *)(param_1 + 0x50) = 0;
      }
      FUN_140279c10(&local_f80,&local_1060);
      local_1010 = (void *)CONCAT71(uStack_107f,local_1080);
      uStack_1008 = uStack_1078;
      local_1000 = local_1070;
      uStack_ff8 = uStack_1068;
      local_1070 = uVar2;
      uStack_1068 = uVar3;
      local_1080 = 0;
      local_ff0 = local_10a0;
      uStack_fe8 = uStack_1098;
      local_fe0 = local_1090;
      uStack_fd8 = uStack_1088;
      local_1090 = uVar2;
      uStack_1088 = uVar3;
      local_10a0 = (void *)((ulonglong)local_10a0 & 0xffffffffffffff00);
      local_fd0 = local_10c0;
      uStack_fc8 = uStack_10b8;
      local_fc0 = local_10b0;
      uStack_fb8 = uStack_10a8;
      local_10b0 = uVar2;
      uStack_10a8 = uVar3;
      local_10c0 = (void *)((ulonglong)local_10c0 & 0xffffffffffffff00);
      local_fb0 = local_1060;
      uStack_fac = uStack_105c;
      uStack_fa8 = uStack_1058;
      uStack_fa4 = uStack_1054;
      local_fa0 = (undefined4)local_1050;
      uStack_f9c = local_1050._4_4_;
      uStack_f98 = (undefined4)uStack_1048;
      uStack_f94 = uStack_1048._4_4_;
      local_1050 = uVar2;
      uStack_1048 = uVar3;
      local_1060 = local_1060 & 0xffffff00;
      local_f90 = *(undefined4 *)(param_1 + 0x1c);
      local_1018 = param_1;
      uVar5 = FUN_1402cbf90();
      uVar5 = FUN_1404ea710(&local_res8,uVar5);
      FUN_140470c70(param_1 + 0x1d0,uVar5);
      if (local_res8 != (void **)0x0) {
        LOCK();
        pvVar6 = *local_res8;
        *local_res8 = (void *)((longlong)*local_res8 + -1);
        UNLOCK();
        if (pvVar6 == (void *)0x1) {
          (**(code **)local_res8[3])();
        }
      }
      if (0xf < CONCAT44(uStack_f94,uStack_f98)) {
        pvVar1 = (void *)CONCAT44(uStack_fac,local_fb0);
        pvVar6 = pvVar1;
        if ((0xfff < CONCAT44(uStack_f94,uStack_f98) + 1) &&
           (pvVar6 = *(void **)((longlong)pvVar1 + -8),
           0x1f < (ulonglong)((longlong)pvVar1 + (-8 - (longlong)pvVar6)))) goto LAB_1404e4c21;
        free(pvVar6);
      }
      if (0xf < uStack_fb8) {
        pvVar6 = local_fd0;
        if ((0xfff < uStack_fb8 + 1) &&
           (pvVar6 = *(void **)((longlong)local_fd0 + -8),
           0x1f < (ulonglong)((longlong)local_fd0 + (-8 - (longlong)pvVar6)))) goto LAB_1404e4c21;
        free(pvVar6);
      }
      if (0xf < uStack_fd8) {
        pvVar6 = local_ff0;
        if ((0xfff < uStack_fd8 + 1) &&
           (pvVar6 = *(void **)((longlong)local_ff0 + -8),
           0x1f < (ulonglong)((longlong)local_ff0 + (-8 - (longlong)pvVar6)))) goto LAB_1404e4c21;
        free(pvVar6);
      }
      if (0xf < uStack_ff8) {
        pvVar6 = local_1010;
        if ((0xfff < uStack_ff8 + 1) &&
           (pvVar6 = *(void **)((longlong)local_1010 + -8),
           0x1f < (ulonglong)((longlong)local_1010 + (-8 - (longlong)pvVar6)))) {
LAB_1404e4c21:
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pvVar6);
      }
      if (0xf < uStack_1048) {
        pvVar1 = (void *)CONCAT44(uStack_105c,local_1060);
        pvVar6 = pvVar1;
        if ((0xfff < uStack_1048 + 1) &&
           (pvVar6 = *(void **)((longlong)pvVar1 + -8),
           0x1f < (ulonglong)((longlong)pvVar1 + (-8 - (longlong)pvVar6)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pvVar6);
      }
      if (0xf < uStack_1068) {
        pvVar1 = (void *)CONCAT71(uStack_107f,local_1080);
        pvVar6 = pvVar1;
        if ((0xfff < uStack_1068 + 1) &&
           (pvVar6 = *(void **)((longlong)pvVar1 + -8),
           0x1f < (ulonglong)((longlong)pvVar1 + (-8 - (longlong)pvVar6)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pvVar6);
      }
      local_1070 = uVar2;
      uStack_1068 = uVar3;
      local_1080 = 0;
      *(undefined ***)((longlong)&local_f88 + (longlong)*(int *)(local_f88 + 4)) =
           std::basic_ostringstream<char,std::char_traits<char>,std::allocator<char>_>::vftable;
      *(int *)((longlong)&iStack_f8c + (longlong)*(int *)(local_f88 + 4)) =
           *(int *)(local_f88 + 4) + -0x88;
      local_f80 = std::basic_stringbuf<char,std::char_traits<char>,std::allocator<char>_>::vftable;
      FUN_140279b20(&local_f80);
      std::basic_streambuf<char,std::char_traits<char>_>::
      ~basic_streambuf<char,std::char_traits<char>_>
                ((basic_streambuf<char,std::char_traits<char>_> *)&local_f80);
      std::basic_ostream<char,std::char_traits<char>_>::~basic_ostream<char,std::char_traits<char>_>
                (local_f78);
      std::basic_ios<char,std::char_traits<char>_>::~basic_ios<char,std::char_traits<char>_>
                (local_f00);
      if (0xf < uStack_10a8) {
        pvVar6 = local_10c0;
        if ((0xfff < uStack_10a8 + 1) &&
           (pvVar6 = *(void **)((longlong)local_10c0 + -8),
           0x1f < (ulonglong)((longlong)local_10c0 + (-8 - (longlong)pvVar6)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pvVar6);
      }
      local_10b0 = uVar2;
      uStack_10a8 = uVar3;
      local_10c0 = (void *)((ulonglong)local_10c0 & 0xffffffffffffff00);
      if (0xf < uStack_1088) {
        pvVar6 = local_10a0;
        if ((0xfff < uStack_1088 + 1) &&
           (pvVar6 = *(void **)((longlong)local_10a0 + -8),
           0x1f < (ulonglong)((longlong)local_10a0 + (-8 - (longlong)pvVar6)))) {
                    /* WARNING: Subroutine does not return */
          _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        free(pvVar6);
      }
      local_1090 = uVar2;
      uStack_1088 = uVar3;
      local_10a0 = (void *)((ulonglong)local_10a0 & 0xffffffffffffff00);
    }
    if (local_10e8 != (void *)0x0) {
      pvVar6 = local_10e8;
      if ((0xfff < (ulonglong)(local_10d8 - (longlong)local_10e8)) &&
         (pvVar6 = *(void **)((longlong)local_10e8 + -8),
         0x1f < (ulonglong)((longlong)local_10e8 + (-8 - (longlong)pvVar6)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(pvVar6);
    }
  }
  return;
}


// Incoming references
// 0xc28d60 DATA caller none
// 0x740d04 UNCONDITIONAL_CALL caller 140740c40
