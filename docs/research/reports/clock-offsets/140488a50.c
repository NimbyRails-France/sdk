
ulonglong FUN_140488a50(longlong param_1,ulonglong param_2)

{
  uint uVar1;
  longlong *plVar2;
  ulonglong uVar3;
  undefined1 *_Memory;
  void *_Memory_00;
  ulonglong *puVar4;
  longlong lVar5;
  undefined8 local_848;
  undefined8 ***pppuStack_840;
  void *local_838;
  longlong lStack_830;
  undefined8 local_828;
  ulonglong uStack_820;
  undefined8 ***local_818;
  undefined8 uStack_810;
  undefined **local_7f8;
  undefined1 *local_7f0;
  undefined8 local_7e8;
  ulonglong local_7e0;
  undefined1 local_7d8 [512];
  ulonglong local_5d8;
  undefined1 local_5d0;
  undefined1 local_5c0 [32];
  char local_5a0;
  undefined8 ***local_598 [2];
  undefined8 local_588;
  ulonglong local_580;
  undefined1 local_38 [32];

  uVar3 = 0;
  lVar5 = *(longlong *)(param_1 + 0x1170);
  for (puVar4 = *(ulonglong **)(lVar5 + (param_2 % (ulonglong)*(uint *)(param_1 + 0x1178)) * 8);
      puVar4 != (ulonglong *)0x0; puVar4 = (ulonglong *)puVar4[0xb8]) {
    if (param_2 == *puVar4) goto LAB_140488ab2;
  }
  puVar4 = *(ulonglong **)(lVar5 + *(longlong *)(param_1 + 0x1178) * 8);
LAB_140488ab2:
  if (puVar4 != *(ulonglong **)(lVar5 + *(longlong *)(param_1 + 0x1178) * 8)) {
    FUN_14030d0f0(&local_5d8,puVar4 + 1);
    do {
      lVar5 = 8;
      do {
        uVar1 = std::_Random_device();
        uVar3 = (ulonglong)(uVar1 & 0xff) | uVar3 << 8;
        lVar5 = lVar5 + -1;
      } while (lVar5 != 0);
    } while (uVar3 < 0x100);
    local_5d8 = uVar3;
    local_838 = (void *)FUN_1402d82e0("fmt_copy_of","Copy of {}");
    lStack_830 = -1;
    do {
      lStack_830 = lStack_830 + 1;
    } while (*(char *)((longlong)local_838 + lStack_830) != '\0');
    local_818 = local_598;
    if (0xf < local_580) {
      local_818 = local_598[0];
    }
    uStack_810 = local_588;
    pppuStack_840 = &local_818;
    local_7e8 = 0;
    local_7f8 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
    local_7f0 = local_7d8;
    local_7e0 = 500;
    local_848 = 0xd;
    FUN_140022830(&local_7f8,&local_838,&local_848);
    local_838 = (void *)0x0;
    lStack_830 = 0;
    local_828 = 0;
    uStack_820 = 0;
    FUN_140002c00(&local_838,local_7f0);
    local_7f8 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
    if (local_7f0 != local_7d8) {
      _Memory = local_7f0;
      if ((0xfff < local_7e0) &&
         (_Memory = *(undefined1 **)(local_7f0 + -8),
         (undefined1 *)0x1f < local_7f0 + (-8 - (longlong)_Memory))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(_Memory);
    }
    FUN_140025630(local_598,&local_838);
    if (0xf < uStack_820) {
      _Memory_00 = local_838;
      if ((0xfff < uStack_820 + 1) &&
         (_Memory_00 = *(void **)((longlong)local_838 + -8),
         0x1f < (ulonglong)((longlong)local_838 + (-8 - (longlong)_Memory_00)))) {
                    /* WARNING: Subroutine does not return */
        _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      free(_Memory_00);
    }
    local_5d0 = 0;
    plVar2 = (longlong *)FUN_140494160(param_1 + 0x1168,&local_818);
    FUN_14032a030(*plVar2 + 8,&local_5d8);
    FUN_14031f9e0(local_38);
    FUN_140002d30(local_598);
    param_2 = uVar3;
    if (local_5a0 != '\0') {
      FUN_140002d30(local_5c0);
    }
  }
  return param_2;
}
