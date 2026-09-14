
/* WARNING: Removing unreachable block (ram,0x00014046ff26) */

void FUN_14046fe00(undefined8 *param_1)

{
  longlong *plVar1;
  longlong lVar2;
  longlong lVar3;
  ulonglong uVar4;
  undefined8 *puVar5;
  longlong lVar6;
  undefined1 auVar7 [16];
  longlong lVar8;
  ulonglong *puVar9;
  longlong lVar10;
  byte bVar11;
  int iVar12;
  ulonglong *puVar13;
  ulonglong *puVar14;
  ulonglong uVar15;
  ulonglong *local_res10;
  ulonglong *local_70;
  char local_68;
  char local_60 [8];
  longlong local_58;
  longlong local_50;
  
  puVar14 = (ulonglong *)*param_1;
  LOCK();
  uVar15 = *puVar14;
  *puVar14 = *puVar14 + 1;
  UNLOCK();
  if (uVar15 < (ulonglong)param_1[3]) {
    do {
      plVar1 = (longlong *)param_1[1];
      lVar2 = param_1[2];
      puVar14 = (ulonglong *)(*(longlong *)(*(longlong *)(*plVar1 + 0x20) + 0xd8) + uVar15 * 0x90);
      lVar3 = plVar1[1];
      uVar4 = *puVar14;
      if (uVar4 != 0) {
        for (puVar9 = *(ulonglong **)
                       (*(longlong *)(lVar3 + 0x38) +
                       (uVar4 % (ulonglong)*(uint *)(lVar3 + 0x40)) * 8); puVar9 != (ulonglong *)0x0
            ; puVar9 = (ulonglong *)puVar9[10]) {
          if (uVar4 == *puVar9) {
            if ((puVar9 != (ulonglong *)0x0) &&
               (puVar13 = puVar9 + 1,
               puVar9 != *(ulonglong **)
                          (*(longlong *)(lVar3 + 0x38) + *(longlong *)(lVar3 + 0x40) * 8)))
            goto LAB_14046feb6;
            break;
          }
        }
      }
      puVar13 = (ulonglong *)0x0;
LAB_14046feb6:
      local_res10 = (ulonglong *)0x0;
      puVar9 = puVar13;
      if (puVar13 == (ulonglong *)0x0) {
        puVar9 = (ulonglong *)FUN_140983da8(0x48);
        *puVar9 = 0;
        puVar9[1] = 0;
        puVar9[2] = 0;
        puVar9[3] = 0;
        puVar9[4] = 0;
        puVar9[5] = 0;
        puVar9[6] = 0;
        puVar9[7] = 0;
        *puVar9 = 0;
        puVar9[1] = 0;
        puVar9[2] = (ulonglong)&DAT_140a6d430;
        puVar9[3] = 0;
        puVar9[4] = 0;
        puVar9[5] = 0;
        puVar9[8] = 0;
        *puVar9 = *puVar14;
        local_res10 = puVar9;
      }
      puVar5 = (undefined8 *)*plVar1;
      lVar3 = plVar1[3];
      iVar12 = *(int *)(lVar3 + 0x20f8) % 0x93a80;
      if (iVar12 < 0) {
        iVar12 = iVar12 + 0x93a80;
      }
      uVar15 = FUN_140466690(puVar5[4],*puVar5,plVar1[2],uVar15 & 0xffffffff,iVar12,
                             *(undefined8 *)(lVar3 + 0x2108),*(undefined8 *)(lVar3 + 0x2100),
                             puVar9[1],(double)*(float *)plVar1[4],*(undefined4 *)(lVar3 + 0x2128),
                             puVar9 + 2,puVar5 + 0x19,lVar2 * 0x100 + puVar5[0xd],plVar1[5]);
      puVar9[1] = uVar15;
      if (puVar13 == (ulonglong *)0x0) {
        auVar7._8_8_ = 0;
        auVar7._0_8_ = *puVar14;
        lVar2 = SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar7,8) +
                SUB168(ZEXT816(0xde5fb9d2630458e9) * auVar7,0);
        local_68 = '\0';
        local_70 = puVar13;
        FUN_1404710b0(*plVar1 + 0x220,local_60,lVar2,puVar14,&local_70);
        lVar8 = local_50;
        lVar3 = local_58;
        if (local_60[0] == '\0') {
          FUN_14046e260(*(longlong *)(local_50 + 0x58) + 8 + local_58 * 0x50);
        }
        else {
          lVar10 = local_58 * 0x50;
          lVar6 = *(longlong *)(local_50 + 0x58);
          *(ulonglong *)(lVar6 + lVar10) = *puVar14;
          *(ulonglong *)(lVar6 + 8 + lVar10) = *local_res10;
          *(ulonglong *)(lVar6 + 0x10 + lVar10) = local_res10[1];
          FUN_140472160(lVar6 + 0x18 + lVar10);
          bVar11 = (byte)lVar2 & 0x7f;
          *(byte *)(lVar3 + *(longlong *)(lVar8 + 0x50)) = bVar11;
          *(byte *)((lVar3 - 0x10U & *(ulonglong *)(lVar8 + 0x68)) +
                    (ulonglong)((uint)*(ulonglong *)(lVar8 + 0x68) & 0xf) + 1 +
                   *(longlong *)(lVar8 + 0x50)) = bVar11;
        }
        if (local_68 != '\0') {
          _Mtx_unlock(local_70);
        }
      }
      if (local_res10 != (ulonglong *)0x0) {
        FUN_140353b70(local_res10 + 2);
        free(local_res10);
      }
      puVar14 = (ulonglong *)*param_1;
      LOCK();
      uVar15 = *puVar14;
      *puVar14 = *puVar14 + 1;
      UNLOCK();
    } while (uVar15 < (ulonglong)param_1[3]);
  }
  return;
}

