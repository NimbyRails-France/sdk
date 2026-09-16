// Candidate VA 1405e1a00; RVA 0x5e1a00
// Ghidra inferred prototype: undefined FUN_1405e1a00()

void FUN_1405e1a00(longlong *param_1,longlong *param_2)

{
  code *pcVar1;
  char cVar2;
  uint uVar3;
  undefined8 uVar4;
  longlong lVar5;
  void *pvVar6;
  char *pcVar7;
  void *pvVar8;
  undefined1 *_Memory;
  char *pcVar9;
  longlong lVar10;
  longlong lVar11;
  bool bVar12;
  void *local_2c8;
  void *pvStack_2c0;
  void *local_2b8;
  void *local_2a8;
  undefined8 uStack_2a0;
  undefined8 local_298;
  ulonglong local_290;
  undefined8 local_288;
  undefined8 uStack_280;
  undefined8 local_278;
  undefined8 local_270;
  void *local_268;
  void *pvStack_260;
  undefined8 local_258;
  void **ppvStack_250;
  undefined **local_248;
  undefined1 *local_240;
  undefined8 local_238;
  ulonglong local_230;
  undefined1 local_228 [512];

  *(undefined4 *)((longlong)param_2 + 0x2c) = 0x43c80000;
  *(undefined1 *)(param_2 + 5) = 1;
  FUN_14055b8e0(param_2,&DAT_140b8e660);
  (**(code **)(*param_2 + 8))(param_2);
  FUN_14055b8e0(param_2,&DAT_140b8dce0);
  pcVar1 = *(code **)(*param_2 + 0xa8);
  uVar4 = FUN_1402d82e0("host_game","Host game");
  (*pcVar1)(param_2,uVar4,0x12);
  FUN_14055b8e0(param_2,&DAT_140b8dfe0);
  (**(code **)(*param_2 + 8))(param_2);
  FUN_14055b8e0(param_2,&DAT_140b8e060);
  pcVar1 = *(code **)(*param_2 + 0xa8);
  uVar4 = FUN_1402d82e0("host_server_mode","Connection:");
  (*pcVar1)(param_2,uVar4,0x14);
  lVar5 = FUN_1402d82e0("host_server_mode_relay","Steam Relay Network");
  local_2a8 = (void *)0x0;
  uStack_2a0 = 0;
  local_298 = 0;
  local_290 = 0;
  lVar10 = -1;
  lVar11 = -1;
  do {
    lVar11 = lVar11 + 1;
  } while (*(char *)(lVar5 + lVar11) != '\0');
  FUN_140002c00(&local_2a8,lVar5);
  lVar5 = FUN_1402d82e0("host_server_mode_direct","Direct");
  local_288 = 0;
  uStack_280 = 0;
  local_278 = 0;
  local_270 = 0;
  lVar11 = -1;
  do {
    lVar11 = lVar11 + 1;
  } while (*(char *)(lVar5 + lVar11) != '\0');
  FUN_140002c00(&local_288,lVar5);
  local_2c8 = (void *)0x0;
  pvStack_2c0 = (void *)0x0;
  local_2b8 = (void *)0x0;
  local_2c8 = (void *)thunk_FUN_140983da8(0x40);
  pvStack_2c0 = (void *)((longlong)local_2c8 + 0x40);
  local_2b8 = pvStack_2c0;
  FUN_14041eae0(&local_2a8,&local_268,local_2c8);
  _eh_vector_destructor_iterator_(&local_2a8,0x20,2,thunk_FUN_140002d30);
  bVar12 = *(int *)(*param_1 + 0x3d0) != 0;
  FUN_14055b8e0(param_2,&DAT_140b8e0e0);
  uVar3 = (**(code **)(*param_2 + 0x120))(param_2,&local_2c8,bVar12);
  pvVar8 = pvStack_2c0;
  pvVar6 = local_2c8;
  if (uVar3 != bVar12) {
    *(uint *)(*param_1 + 0x3d0) = uVar3;
  }
  for (; pvVar6 != pvVar8; pvVar6 = (void *)((longlong)pvVar6 + 0x20)) {
    FUN_140002d30(pvVar6);
  }
  if (local_2c8 != (void *)0x0) {
    free(local_2c8);
  }
  (**(code **)(*param_2 + 0x18))(param_2);
  pcVar1 = *(code **)(*param_2 + 0xc0);
  if (*(int *)param_1[1] == 0) {
    pcVar9 =
    "Clients connect to your PC via the Steam Relay Network.\nThis is a safe, optimized CDN which hides your IP but has built-in bandwidth cap."
    ;
    pcVar7 = "host_server_mode_relay_desc";
  }
  else {
    pcVar9 =
    "Clients will attempt to directly connect to your PC with your connection public IP address.\nEnables hosting of much larger builds if your connection has enough upload bandwidth."
    ;
    pcVar7 = "host_server_mode_direct_desc";
  }
  uVar4 = FUN_1402d82e0(pcVar7,pcVar9);
  (*pcVar1)(param_2,uVar4);
  if (*(int *)param_1[1] == 1) {
    lVar5 = *param_1;
    if (*(char *)(lVar5 + 0x3c8) == '\0') {
      FUN_14055b8e0(param_2,&DAT_140b8dfe0);
      pcVar9 = "Detecting public IP...";
      pcVar7 = "host_server_wait_ip";
    }
    else {
      if ((7 < *(ulonglong *)(lVar5 + 0x398)) || (2 < *(ulonglong *)(lVar5 + 0x3b8))) {
        if (*(ulonglong *)(lVar5 + 0x398) != 0) {
          FUN_14055b8e0(param_2,&DAT_140b8dfe0);
          FUN_14055b8e0(param_2,&DAT_140b8dee0);
          (**(code **)(*param_2 + 8))(param_2);
          FUN_14055b8e0(param_2,&DAT_140b8e060);
          pcVar1 = *(code **)(*param_2 + 0xa8);
          uVar4 = FUN_1402d82e0("host_server_ipv4","Detected public IPv4:");
          (*pcVar1)(param_2,uVar4,0x14);
          FUN_14055b8e0(param_2,&DAT_140b8e0e0);
          (**(code **)(*param_2 + 0xa0))(param_2,*param_1 + 0x388,0x11);
          (**(code **)(*param_2 + 0x18))(param_2);
        }
        if (*(longlong *)(*param_1 + 0x3b8) != 0) {
          FUN_14055b8e0(param_2,&DAT_140b8dfe0);
          FUN_14055b8e0(param_2,&DAT_140b8dee0);
          (**(code **)(*param_2 + 8))(param_2);
          FUN_14055b8e0(param_2,&DAT_140b8e060);
          pcVar1 = *(code **)(*param_2 + 0xa8);
          uVar4 = FUN_1402d82e0("host_server_ipv6","Detected public IPv6:");
          (*pcVar1)(param_2,uVar4,0x14);
          FUN_14055b8e0(param_2,&DAT_140b8e0e0);
          (**(code **)(*param_2 + 0xa0))(param_2,*param_1 + 0x3a8,0x11);
          (**(code **)(*param_2 + 0x18))(param_2);
        }
        FUN_14055b8e0(param_2,&DAT_140b8dfe0);
        FUN_14055b8e0(param_2,&DAT_140b8dee0);
        (**(code **)(*param_2 + 8))(param_2);
        FUN_14055b8e0(param_2,&DAT_140b8e060);
        pcVar1 = *(code **)(*param_2 + 0xa8);
        uVar4 = FUN_1402d82e0("host_server_port","UDP Port:");
        (*pcVar1)(param_2,uVar4,0x14);
        FUN_14055b8e0(param_2,&DAT_140b8e0e0);
        pcVar1 = *(code **)(*param_2 + 0xa0);
        uVar4 = FUN_14028fde0(&local_2a8,0x9967);
        (*pcVar1)(param_2,uVar4);
        if (0xf < local_290) {
          pvVar6 = local_2a8;
          if ((0xfff < local_290 + 1) &&
             (pvVar6 = *(void **)((longlong)local_2a8 + -8),
             0x1f < (ulonglong)((longlong)local_2a8 + (-8 - (longlong)pvVar6)))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pvVar6);
        }
        (**(code **)(*param_2 + 0x18))(param_2);
        *(undefined4 *)((longlong)param_2 + 0x2c) = 0;
        *(undefined4 *)((longlong)param_2 + 0x34) = 0;
        *(undefined1 *)(param_2 + 5) = 1;
        *(undefined1 *)(param_2 + 6) = 1;
        pcVar1 = *(code **)(*param_2 + 0xb0);
        pvVar6 = (void *)FUN_1402d82e0("host_server_direct_warn",
                                       "Your PC or router must have a direct connection to the internet with a public IP. You must forward UDP port {} in your router to your PC and set up your firewall to allow connections. VPNs, proxies and CG-NAT will defeat this feature. Weird and Wry cannot offer technical support if this feature does not work for your connection."
                                      );
        pvVar8 = (void *)0xffffffffffffffff;
        do {
          pvVar8 = (void *)((longlong)pvVar8 + 1);
        } while (*(char *)((longlong)pvVar6 + (longlong)pvVar8) != '\0');
        local_268 = (void *)CONCAT44(local_268._4_4_,0x9967);
        local_2c8 = local_268;
        pvStack_2c0 = pvStack_260;
        ppvStack_250 = &local_2c8;
        local_238 = 0;
        local_248 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
        local_240 = local_228;
        local_230 = 500;
        local_258 = 1;
        local_268 = pvVar6;
        pvStack_260 = pvVar8;
        FUN_140022830(&local_248,&local_268,&local_258);
        local_2a8 = (void *)0x0;
        uStack_2a0 = 0;
        local_298 = 0;
        local_290 = 0;
        FUN_140002c00(&local_2a8,local_240);
        local_248 = fmt::v10::basic_memory_buffer<char,500,std::allocator<char>_>::vftable;
        if (local_240 != local_228) {
          _Memory = local_240;
          if ((0xfff < local_230) &&
             (_Memory = *(undefined1 **)(local_240 + -8),
             (undefined1 *)0x1f < local_240 + (-8 - (longlong)_Memory))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(_Memory);
        }
        (*pcVar1)(param_2,DAT_140aac494);
        if (0xf < local_290) {
          pvVar6 = local_2a8;
          if ((0xfff < local_290 + 1) &&
             (pvVar6 = *(void **)((longlong)local_2a8 + -8),
             0x1f < (ulonglong)((longlong)local_2a8 + (-8 - (longlong)pvVar6)))) {
                    /* WARNING: Subroutine does not return */
            _invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
          }
          free(pvVar6);
        }
        goto LAB_1405e20b3;
      }
      FUN_14055b8e0(param_2,&DAT_140b8dfe0);
      pcVar9 = "Cannot detect public IP, use Steam Relay";
      pcVar7 = "host_server_bad_ip";
    }
    pcVar1 = *(code **)(*param_2 + 0xa8);
    uVar4 = FUN_1402d82e0(pcVar7,pcVar9);
    (*pcVar1)(param_2,uVar4,0x11);
  }
LAB_1405e20b3:
  FUN_14055b8e0(param_2,&DAT_140b8dfe0);
  (**(code **)(*param_2 + 8))(param_2);
  FUN_14055b8e0(param_2,&DAT_140b8e060);
  pcVar1 = *(code **)(*param_2 + 0xa8);
  uVar4 = FUN_1402d82e0("host_sim_mode","Clock and bank:");
  (*pcVar1)(param_2,uVar4,0x14);
  lVar5 = FUN_1402d82e0("host_sim_mode_shared","Shared between all players");
  local_2a8 = (void *)0x0;
  uStack_2a0 = 0;
  local_298 = 0;
  local_290 = 0;
  lVar11 = -1;
  do {
    lVar11 = lVar11 + 1;
  } while (*(char *)(lVar5 + lVar11) != '\0');
  FUN_140002c00(&local_2a8,lVar5);
  lVar5 = FUN_1402d82e0("host_sim_mode_private","Private for each player");
  local_288 = 0;
  uStack_280 = 0;
  local_278 = 0;
  local_270 = 0;
  do {
    lVar10 = lVar10 + 1;
  } while (*(char *)(lVar5 + lVar10) != '\0');
  FUN_140002c00(&local_288,lVar5,lVar10);
  local_2c8 = (void *)0x0;
  pvStack_2c0 = (void *)0x0;
  local_2b8 = (void *)0x0;
  local_2c8 = (void *)thunk_FUN_140983da8(0x40);
  pvStack_2c0 = (void *)((longlong)local_2c8 + 0x40);
  local_2b8 = pvStack_2c0;
  FUN_14041eae0(&local_2a8,&local_268,local_2c8);
  _eh_vector_destructor_iterator_(&local_2a8,0x20,2,thunk_FUN_140002d30);
  bVar12 = *(char *)(*param_1 + 0x35c) != '\0';
  FUN_14055b8e0(param_2,&DAT_140b8e0e0);
  uVar3 = (**(code **)(*param_2 + 0x120))(param_2,&local_2c8,bVar12);
  pvVar8 = pvStack_2c0;
  pvVar6 = local_2c8;
  if (uVar3 != bVar12) {
    *(bool *)(*param_1 + 0x35c) = uVar3 == 1;
  }
  for (; pvVar6 != pvVar8; pvVar6 = (void *)((longlong)pvVar6 + 0x20)) {
    FUN_140002d30(pvVar6);
  }
  if (local_2c8 != (void *)0x0) {
    free(local_2c8);
  }
  (**(code **)(*param_2 + 0x18))(param_2);
  pcVar1 = *(code **)(*param_2 + 0xc0);
  if (*(char *)(param_1[2] + 4) == '\0') {
    pcVar9 =
    "Time, train motion and pax simulation is shared and synchronized between all clients.\nTracks, lines and trains are edited cooperatively."
    ;
    pcVar7 = "host_sim_mode_shared_desc";
  }
  else {
    pcVar9 =
    "Time, train motion and pax simulation is independent for each client.\nTracks, lines and trains are edited cooperatively.\nScales to larger build sizes than shared simulation.\nGames are always set to unlimited money."
    ;
    pcVar7 = "host_sim_mode_private_desc";
  }
  uVar4 = FUN_1402d82e0(pcVar7,pcVar9);
  (*pcVar1)(param_2,uVar4);
  pcVar1 = *(code **)(*param_2 + 0x90);
  cVar2 = *(char *)param_1[3];
  uVar4 = FUN_1402d82e0("new_game","New game");
  cVar2 = (*pcVar1)(param_2,uVar4,cVar2 == '\0');
  if (cVar2 != '\0') {
    FUN_1402e0680();
    FUN_1402e0960();
    FUN_1402d8e30();
    *(undefined4 *)(*param_1 + 8) = 0xb;
  }
  pcVar1 = *(code **)(*param_2 + 0x90);
  cVar2 = *(char *)param_1[3];
  uVar4 = FUN_1402d82e0("load_game","Load game");
  cVar2 = (*pcVar1)(param_2,uVar4,cVar2 == '\0');
  if (cVar2 != '\0') {
    *(undefined4 *)(*param_1 + 8) = 4;
    FUN_1405de980(*param_1);
  }
  *(undefined4 *)((longlong)param_2 + 0x34) = 0x41200000;
  *(undefined1 *)(param_2 + 6) = 1;
  (**(code **)(*param_2 + 200))(param_2);
  pcVar1 = *(code **)(*param_2 + 0x90);
  uVar4 = FUN_1402d82e0(&DAT_140a8b00c,&DAT_140a8af90);
  cVar2 = (*pcVar1)(param_2,uVar4,0);
  if (cVar2 != '\0') {
    *(undefined4 *)(*param_1 + 8) = 0;
  }
                    /* WARNING: Could not recover jumptable at 0x0001405e23ae. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*param_2 + 0x18))(param_2);
  return;
}


// Incoming references
// 0xc2ff54 DATA caller none
// 0x5faa52 UNCONDITIONAL_CALL caller 1405fa720
// 0x5fb037 UNCONDITIONAL_CALL caller 1405fa720
