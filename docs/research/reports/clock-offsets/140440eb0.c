
void FUN_140440eb0(longlong param_1,longlong param_2)

{
  longlong lVar1;
  longlong *plVar2;
  longlong lVar3;
  int iVar4;
  longlong *plVar5;
  longlong local_res8 [4];

  local_res8[1] = *(longlong *)(param_1 + 0x10);
  if (-1 < local_res8[1]) {
    local_res8[2] = 0x15180;
    plVar5 = local_res8;
    if (0 < local_res8[1]) {
      plVar5 = local_res8 + 1;
    }
    local_res8[0] = 1;
    plVar2 = local_res8 + 2;
    if (local_res8[1] < 0x15181) {
      plVar2 = plVar5;
    }
    lVar1 = *plVar2;
    lVar3 = FUN_14045f300(*(longlong *)(param_2 + 8) + 0xa0,*(undefined8 *)(param_1 + 8));
    if ((lVar3 != 0) && (*(char *)(lVar3 + 0x4d0) != '\0')) {
      iVar4 = (int)lVar1;
      *(int *)(lVar3 + 0x4c8) = *(int *)(lVar3 + 0x4c8) + iVar4;
      *(longlong *)(lVar3 + 0x4b8) = *(longlong *)(lVar3 + 0x4b8) + (longlong)iVar4;
      *(longlong *)(lVar3 + 0x4c0) = *(longlong *)(lVar3 + 0x4c0) + (longlong)iVar4 * 1000000;
    }
  }
  return;
}
