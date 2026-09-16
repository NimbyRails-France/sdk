
void FUN_140441130(longlong param_1,longlong *param_2)

{
  longlong lVar1;
  longlong lVar2;
  longlong lVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  undefined4 uVar6;
  longlong lVar7;
  longlong lVar8;
  longlong *plVar9;
  int iVar10;
  longlong *plVar11;
  longlong local_res10 [3];
  undefined4 local_78;
  undefined1 uStack_74;
  undefined2 uStack_73;
  undefined1 uStack_71;
  undefined8 uStack_70;
  undefined8 local_68;
  undefined4 uStack_60;
  undefined4 uStack_5c;
  longlong local_58;
  undefined4 uStack_50;
  uint uStack_4c;

  lVar2 = *param_2;
  lVar3 = *(longlong *)(param_1 + 0x18);
  uVar4 = *(undefined8 *)(param_1 + 8);
  local_res10[1] = 0x93a80;
  local_res10[2] = param_2[1];
  plVar11 = local_res10;
  if (-1 < lVar3) {
    plVar11 = (longlong *)(param_1 + 0x18);
  }
  local_res10[0] = 0;
  lVar1 = local_res10[2] + 0xa0;
  iVar10 = *(int *)(local_res10[2] + 0x20f8) % 0x93a80;
  uVar5 = *(undefined8 *)(param_1 + 0x10);
  lVar7 = FUN_14045f300(lVar1,uVar4);
  lVar8 = FUN_14033f7f0(lVar2 + 0x180,uVar5);
  if ((((lVar7 != 0) && (lVar8 != 0)) &&
      (*(longlong *)(lVar8 + 0x118) != *(longlong *)(lVar8 + 0x120))) &&
     (((*(int *)(lVar8 + 0xfc) != 0 && (*(char *)(lVar7 + 0x5f0) == '\0')) &&
      ((*(char *)(lVar7 + 0x5d0) == '\0' && (*(char *)(lVar7 + 0x1f0) == '\0')))))) {
    uStack_5c = 0xffffffff;
    if (iVar10 < 0) {
      iVar10 = iVar10 + 0x93a80;
    }
    plVar9 = local_res10 + 1;
    if (lVar3 < 0x93a81) {
      plVar9 = plVar11;
    }
    uStack_60 = 0;
    uStack_50 = 0;
    local_58 = *(longlong *)(local_res10[2] + 0x2108);
    *(ulonglong *)(lVar7 + 0x5a0) =
         CONCAT17(uStack_71,CONCAT25(uStack_73,(uint5)(uint)(iVar10 + (int)*plVar9)));
    *(undefined8 *)(lVar7 + 0x5a8) = uVar5;
    *(undefined1 *)(lVar7 + 0x5d0) = 1;
    local_58 = local_58 + lVar3;
    *(undefined8 *)(lVar7 + 0x5b0) = 0;
    *(undefined8 *)(lVar7 + 0x5b8) = 0xffffffff00000000;
    *(longlong *)(lVar7 + 0x5c0) = local_58;
    *(ulonglong *)(lVar7 + 0x5c8) = (ulonglong)uStack_4c << 0x20;
    lVar3 = *(longlong *)(lVar8 + 0x118);
    local_78 = *(undefined4 *)(lVar3 + 0x78);
    uVar6 = *(undefined4 *)(lVar3 + 0x7c);
    uStack_70 = *(undefined8 *)(lVar3 + 0x80);
    local_68 = *(undefined8 *)(lVar3 + 0x88);
    uStack_74 = (undefined1)uVar6;
    uStack_73 = (undefined2)((uint)uVar6 >> 8);
    uStack_71 = (undefined1)((uint)uVar6 >> 0x18);
    if (*(char *)(lVar7 + 0x1d0) == '\0') {
      FUN_14044be30(lVar1,lVar2,0,uVar4,&local_78);
    }
    else {
      FUN_14044c390(lVar1,lVar2,0,uVar4,&local_78,1);
    }
  }
  return;
}
