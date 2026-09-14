// Candidate VA 14046fb80; RVA 0x46fb80
// Ghidra inferred prototype: undefined FUN_14046fb80()

void FUN_14046fb80(ulonglong param_1,ulonglong param_2,undefined8 param_3)

{
  longlong lVar1;
  longlong **pplVar2;
  char cVar3;
  longlong ****pppplVar4;
  undefined8 uVar5;
  longlong ***ppplVar6;
  longlong ****pppplVar7;
  ulonglong uVar8;
  longlong *local_res10;
  longlong **local_res20;
  undefined8 local_88;
  longlong **local_80;
  longlong **local_78;
  longlong ***local_70;
  longlong ***ppplStack_68;
  longlong local_60;
  undefined8 local_58;
  undefined8 uStack_50;
  undefined8 local_48;
  undefined8 uStack_40;
  
  uVar8 = 0;
  local_88 = 0;
  if (param_1 == 0) {
    param_1 = (ulonglong)DAT_140b77e58;
  }
  if (param_2 < param_1) {
    param_1 = param_2;
  }
  local_60 = 0;
  local_70 = (longlong ***)&local_70;
  ppplStack_68 = (longlong ***)&local_70;
  if (param_1 != 0) {
    do {
      local_58 = &local_88;
      uStack_50 = param_3;
      local_48 = uVar8;
      uStack_40 = param_2;
      uVar5 = FUN_1402cbb70();
      ppplVar6 = _aligned_malloc(0x80,0x40);
      if (ppplVar6 == (longlong ***)0x0) {
                    /* WARNING: Subroutine does not return */
        abort();
      }
      *ppplVar6 = (longlong **)0x1;
      *(undefined1 *)(ppplVar6 + 1) = 0;
      ppplVar6[2] = (longlong **)0x0;
      ppplVar6[2] = (longlong **)0x0;
      ppplVar6[3] = (longlong **)&PTR_FUN_140a02e20;
      *(undefined4 *)(ppplVar6 + 8) = (undefined4)local_58;
      *(undefined4 *)((longlong)ppplVar6 + 0x44) = local_58._4_4_;
      *(undefined4 *)(ppplVar6 + 9) = (undefined4)uStack_50;
      *(undefined4 *)((longlong)ppplVar6 + 0x4c) = uStack_50._4_4_;
      *(undefined4 *)(ppplVar6 + 10) = (undefined4)local_48;
      *(undefined4 *)((longlong)ppplVar6 + 0x54) = local_48._4_4_;
      *(undefined4 *)(ppplVar6 + 0xb) = (undefined4)uStack_40;
      *(undefined4 *)((longlong)ppplVar6 + 0x5c) = uStack_40._4_4_;
      *ppplVar6 = (longlong **)((longlong)*ppplVar6 + 1);
      local_78 = &local_res10;
      local_res10 = (longlong *)0x0;
      local_res20 = (longlong **)ppplVar6;
      local_80 = (longlong **)ppplVar6;
      FUN_140004680(uVar5,&local_res20);
      if (local_res10 != (longlong *)0x0) {
        LOCK();
        lVar1 = *local_res10;
        *local_res10 = *local_res10 + -1;
        UNLOCK();
        if (lVar1 == 1) {
          (**(code **)local_res10[3])();
        }
      }
      pppplVar7 = (longlong ****)thunk_FUN_140983da8(0x18);
      pppplVar7[2] = ppplVar6;
      *pppplVar7 = (longlong ***)&local_70;
      pppplVar7[1] = ppplStack_68;
      *ppplStack_68 = (longlong **)pppplVar7;
      ppplStack_68 = (longlong ***)pppplVar7;
      local_60 = local_60 + 1;
      uVar8 = uVar8 + 1;
    } while (uVar8 < param_1);
  }
  pppplVar7 = (longlong ****)local_70;
  if ((longlong ****)local_70 != &local_70) {
    do {
      ppplVar6 = pppplVar7[2];
      cVar3 = *(char *)(ppplVar6 + 1);
      if (1 < (byte)(cVar3 - 3U)) {
        (**(code **)(*(longlong *)ThreadLocalStoragePointer + 0x20))(ppplVar6);
        cVar3 = *(char *)(ppplVar6 + 1);
      }
      if (cVar3 == '\x04') {
                    /* WARNING: Subroutine does not return */
        terminate();
      }
      ppplVar6 = pppplVar7[2];
      pppplVar7[2] = (longlong ***)0x0;
      if (ppplVar6 != (longlong ***)0x0) {
        LOCK();
        pplVar2 = *ppplVar6;
        *ppplVar6 = (longlong **)((longlong)*ppplVar6 + -1);
        UNLOCK();
        if (pplVar2 == (longlong **)0x1) {
          (*(code *)*ppplVar6[3])();
        }
      }
      pppplVar7 = (longlong ****)*pppplVar7;
    } while (pppplVar7 != &local_70);
  }
  pppplVar7 = (longlong ****)local_70;
  if ((longlong ****)local_70 != &local_70) {
    do {
      pppplVar4 = (longlong ****)*pppplVar7;
      ppplVar6 = pppplVar7[2];
      if (ppplVar6 != (longlong ***)0x0) {
        LOCK();
        pplVar2 = *ppplVar6;
        *ppplVar6 = (longlong **)((longlong)*ppplVar6 + -1);
        UNLOCK();
        if (pplVar2 == (longlong **)0x1) {
          (*(code *)*ppplVar6[3])();
        }
      }
      free(pppplVar7);
      pppplVar7 = pppplVar4;
    } while (pppplVar4 != &local_70);
  }
  return;
}


// Incoming references
// 0xc24920 DATA caller none
// 0x46d136 UNCONDITIONAL_CALL caller 14046cbd0
