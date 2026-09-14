// Candidate VA 140310850; RVA 0x310850
// Ghidra inferred prototype: undefined FUN_140310850()

void FUN_140310850(longlong param_1,longlong *param_2)

{
  undefined4 uVar1;
  int iVar2;
  ulonglong local_res8;
  
  if (0 < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x18))(param_2,param_1,8,0);
  }
  iVar2 = 3;
  if ((int)param_2[1] - 1U < 0x1d) {
    local_res8 = 0;
    (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,0);
    iVar2 = (int)local_res8;
  }
  if (0x1d < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x20,1,1);
  }
  uVar1 = 9;
  if ((int)param_2[1] - 1U < 0x1d) {
    local_res8 = 0;
    (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,0);
    uVar1 = (int)local_res8;
  }
  if (0x1d < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x24,4,0);
  }
  if (0x2c < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x28,4,0);
  }
  if (0x79 < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x2c,1,0);
  }
  if ((0 < (int)param_2[1]) &&
     ((**(code **)(*param_2 + 0x18))(param_2,param_1 + 8,8,0), 0 < (int)param_2[1])) {
    (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x10,8,0);
  }
  if (0xc < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x18,8,0);
  }
  if (0x47 < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x20))(param_2,param_1 + 0x84);
  }
  if (0 < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))(param_2,"linalg::aliases::double2");
    (**(code **)(*param_2 + 0x28))(param_2,param_1 + 0x30);
    (**(code **)(*param_2 + 0x28))(param_2,param_1 + 0x38);
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  local_res8 = local_res8 & 0xffffffff00000000;
  if ((int)param_2[1] - 0x66U < 4) {
    (**(code **)(*param_2 + 0x20))(param_2,&local_res8);
  }
  if (0x65 < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x20))(param_2,param_1 + 0x78);
  }
  if (0x8c < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x20))(param_2,param_1 + 0x7c);
  }
  if (0 < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0xd0,8,0);
  }
  if (0x6b < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0xa0,4,0);
  }
  if ((0x3e < (int)param_2[1]) && (FUN_1402f2290(param_1 + 0xa8,param_2), 0x3e < (int)param_2[1])) {
    (**(code **)(*param_2 + 0x18))(param_2,param_1 + 200,1,1);
  }
  local_res8 = local_res8 & 0xffffffff00000000;
  if ((int)param_2[1] - 0xaaU < 0xc) {
    (**(code **)(*param_2 + 0x20))(param_2,&local_res8);
  }
  local_res8 = local_res8 & 0xffffffffffffff00;
  if ((int)param_2[1] - 0xfU < 0x4d) {
    (**(code **)(*param_2 + 0x18))(param_2,&local_res8,1,1);
  }
  if (0x3d < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0xd8,1,1);
  }
  if (0x8e < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0xda,1,1);
  }
  if (0x8f < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0xdb,1,1);
  }
  if (0x1b < (int)param_2[1]) {
    FUN_140327bb0(param_1 + 0xe0,param_2);
  }
  if (0x1e < (int)param_2[1]) {
    FUN_140327bb0(param_1 + 0xf8,param_2);
  }
  if ((0x1b < (int)param_2[1]) && (FUN_140327bb0(param_1 + 0x110,param_2), 0x1b < (int)param_2[1]))
  {
    FUN_140327bb0(param_1 + 0x128,param_2);
  }
  if ((int)param_2[1] - 0x20U < 0xa6) {
    FUN_14031b0d0(param_1 + 0x178,param_2);
  }
  if (0xc5 < (int)param_2[1]) {
    FUN_140327cb0(param_1 + 0x140,param_2);
  }
  if ((0 < (int)param_2[1]) &&
     ((**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x3f0,8,0), 0 < (int)param_2[1])) {
    (**(code **)(*param_2 + 0x28))(param_2,param_1 + 0x3f8);
  }
  if (0x1d < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x400,4,0);
  }
  if (0 < (int)param_2[1]) {
    FUN_140327cb0(param_1 + 0x408,param_2);
  }
  if (0x3d < (int)param_2[1]) {
    FUN_140327cb0(param_1 + 0x440,param_2);
  }
  if ((0x20 < (int)param_2[1]) &&
     ((**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x478,8,0), 0x20 < (int)param_2[1])) {
    local_res8 = 0;
    (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,0);
    *(int *)(param_1 + 0x480) = (int)local_res8;
    if ((0x20 < (int)param_2[1]) &&
       (((**(code **)(*param_2 + 0x20))(param_2,param_1 + 0x484), 0x20 < (int)param_2[1] &&
        ((**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x488,4,0), 0x20 < (int)param_2[1])))) {
      (**(code **)(*param_2 + 0x20))(param_2,param_1 + 0x48c);
    }
  }
  if (0x3b < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x20))(param_2,param_1 + 0x490);
  }
  if (0x20 < (int)param_2[1]) {
    FUN_140327dc0(param_1 + 0x498,param_2);
  }
  if (0xbf < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x20))(param_2,param_1 + 0x4e0);
  }
  if (iVar2 == 1) {
    *(undefined1 *)(param_1 + 0x20) = 1;
  }
  else if (iVar2 == 0) {
    *(undefined1 *)(param_1 + 0x20) = 0;
  }
  switch(uVar1) {
  case 0:
    *(undefined4 *)(param_1 + 0x24) = 1;
    break;
  case 1:
  case 2:
    *(undefined4 *)(param_1 + 0x24) = 2;
    break;
  case 3:
    *(undefined4 *)(param_1 + 0x24) = 3;
    break;
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
    *(undefined4 *)(param_1 + 0x24) = 4;
  }
  if ((int)param_2[1] < 0x2d) {
    iVar2 = *(int *)(param_1 + 0x24);
    if (iVar2 == 1) {
      *(undefined4 *)(param_1 + 0x28) = 2;
    }
    else if ((iVar2 == 2) || (iVar2 == 3)) {
      *(undefined4 *)(param_1 + 0x28) = 0;
    }
    else if (iVar2 == 4) {
      *(undefined4 *)(param_1 + 0x28) = 0xffffff9c;
    }
  }
  if ((int)param_2[1] < 0x2e) {
    *(uint *)(param_1 + 0x24) = (*(int *)(param_1 + 0x24) == 3) + 1;
  }
  if (((int)param_2[1] < 0x3e) && (*(longlong *)(param_1 + 0xd0) != 0)) {
    *(undefined1 *)(param_1 + 0xd8) = 1;
  }
  if ((int)param_2[1] < 0x66) {
    *(undefined4 *)(param_1 + 0x78) = 0x3f000000;
  }
  return;
}


// Incoming references
// 0xc17cd8 DATA caller none
// 0x3234e6 UNCONDITIONAL_CALL caller 140323440
// 0x32e674 UNCONDITIONAL_CALL caller 14032e5c0
