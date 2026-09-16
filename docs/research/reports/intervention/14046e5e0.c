
void FUN_14046e5e0(longlong *param_1,undefined8 *param_2,longlong param_3,longlong param_4,
                  char param_5)

{
  longlong *plVar1;
  int *piVar2;
  longlong lVar3;
  longlong lVar4;
  double dVar5;
  char cVar6;
  undefined8 uVar7;
  longlong *plVar8;
  int iVar9;
  int iVar10;
  double dVar11;
  int local_res8;
  undefined4 uStackX_c;
  undefined8 *local_res10;
  undefined8 local_res18;
  longlong *local_res20;
  undefined1 local_a8 [8];
  longlong *local_a0;
  undefined8 local_98;
  longlong *local_90;
  longlong local_88;
  longlong *local_80;
  longlong local_78;
  longlong *local_70;
  int local_68;
  int local_64;

  local_res18 = *(undefined8 *)(param_3 + 0x410);
  local_res8 = *(int *)(param_4 + 0x221c);
  if (DAT_140b77e58 >> 1 < 2) {
    local_res8 = 1;
  }
  else if (local_res8 == 0) {
    local_res8 = (DAT_140b77e58 >> 1) - 1;
  }
  local_res10 = param_2;
  FUN_14046e3b0(param_1,*param_2);
  if ((*param_1 == 0) || (param_1[4] == 0)) {
    plVar8 = (longlong *)param_2[1];
    if (plVar8 != (longlong *)0x0) {
      LOCK();
      plVar1 = plVar8 + 1;
      lVar4 = *plVar1;
      *(int *)plVar1 = (int)*plVar1 + -1;
      UNLOCK();
      if ((int)lVar4 == 1) {
        (**(code **)*plVar8)(plVar8);
        LOCK();
        piVar2 = (int *)((longlong)plVar8 + 0xc);
        iVar9 = *piVar2;
        *piVar2 = *piVar2 + -1;
        UNLOCK();
        if (iVar9 == 1) {
          (**(code **)(*plVar8 + 8))(plVar8);
        }
      }
    }
  }
  else {
    iVar9 = *(int *)(param_4 + 0x20f8) % 0x93a80;
    if (iVar9 < 0) {
      iVar9 = iVar9 + 0x93a80;
    }
    if (iVar9 == 0) {
      iVar9 = FUN_1404775e0(param_4 + 0x20);
    }
    if ((param_1[2] != 0) && (*(char *)(param_1[2] + 0x2bd) != '\0')) {
      if (*param_1 != 0) {
        LOCK();
        *(undefined1 *)(*param_1 + 700) = 1;
        UNLOCK();
      }
      FUN_1402322b0(param_1,param_1 + 2);
      FUN_140470bd0(param_1 + 2);
    }
    dVar5 = DAT_140aabd80;
    iVar10 = local_res8;
    if ((((param_5 == '\0') &&
         (cVar6 = FUN_1403570e0(*param_1,local_res18,param_3 + 0x428,param_3 + 0x780,param_3 + 0x890
                               ), iVar10 = local_res8, cVar6 != '\0')) &&
        ((param_1[2] == 0 || (cVar6 = FUN_1403570e0(), iVar10 = local_res8, cVar6 != '\0')))) &&
       (dVar11 = (double)FUN_140249580(param_1 + 0x16), iVar10 = local_res8, dVar5 < dVar11)) {
      if (param_1[2] != 0) {
        LOCK();
        *(undefined1 *)(param_1[2] + 700) = 1;
        UNLOCK();
      }
      uVar7 = FUN_1404701b0(local_a8);
      FUN_140232230(param_1 + 2,uVar7);
      plVar8 = local_a0;
      if (local_a0 != (longlong *)0x0) {
        LOCK();
        plVar1 = local_a0 + 1;
        lVar4 = *plVar1;
        *(int *)plVar1 = (int)*plVar1 + -1;
        UNLOCK();
        if ((int)lVar4 == 1) {
          (**(code **)*local_a0)(local_a0);
          LOCK();
          piVar2 = (int *)((longlong)plVar8 + 0xc);
          iVar10 = *piVar2;
          *piVar2 = *piVar2 + -1;
          UNLOCK();
          if (iVar10 == 1) {
            (**(code **)(*plVar8 + 8))(plVar8);
          }
        }
      }
      FUN_1403559c0(param_1[2],param_3,local_res18,param_3 + 0x428,param_3 + 0x780,param_3 + 0x890);
      iVar10 = local_res8;
      if (param_2[1] != 0) {
        LOCK();
        piVar2 = (int *)(param_2[1] + 8);
        *piVar2 = *piVar2 + 1;
        UNLOCK();
      }
      local_98 = *param_2;
      local_90 = (longlong *)param_2[1];
      if (param_1[1] != 0) {
        LOCK();
        piVar2 = (int *)(param_1[1] + 8);
        *piVar2 = *piVar2 + 1;
        UNLOCK();
      }
      local_88 = *param_1;
      local_80 = (longlong *)param_1[1];
      if (param_1[3] != 0) {
        LOCK();
        piVar2 = (int *)(param_1[3] + 8);
        *piVar2 = *piVar2 + 1;
        UNLOCK();
      }
      local_78 = param_1[2];
      local_70 = (longlong *)param_1[3];
      local_64 = local_res8;
      local_68 = iVar9;
      uVar7 = FUN_1402cbb70();
      FUN_140470ce0(&local_res20,uVar7);
      FUN_140470c70(param_1 + 10,&local_res20);
      if (local_res20 != (longlong *)0x0) {
        LOCK();
        lVar4 = *local_res20;
        *local_res20 = *local_res20 + -1;
        UNLOCK();
        if (lVar4 == 1) {
          (**(code **)local_res20[3])();
        }
      }
      plVar8 = local_70;
      if (local_70 != (longlong *)0x0) {
        LOCK();
        plVar1 = local_70 + 1;
        lVar4 = *plVar1;
        *(int *)plVar1 = (int)*plVar1 + -1;
        UNLOCK();
        if ((int)lVar4 == 1) {
          (**(code **)*local_70)(local_70);
          LOCK();
          piVar2 = (int *)((longlong)plVar8 + 0xc);
          iVar9 = *piVar2;
          *piVar2 = *piVar2 + -1;
          UNLOCK();
          if (iVar9 == 1) {
            (**(code **)(*plVar8 + 8))(plVar8);
          }
        }
      }
      plVar8 = local_80;
      if (local_80 != (longlong *)0x0) {
        LOCK();
        plVar1 = local_80 + 1;
        lVar4 = *plVar1;
        *(int *)plVar1 = (int)*plVar1 + -1;
        UNLOCK();
        if ((int)lVar4 == 1) {
          (**(code **)*local_80)(local_80);
          LOCK();
          piVar2 = (int *)((longlong)plVar8 + 0xc);
          iVar9 = *piVar2;
          *piVar2 = *piVar2 + -1;
          UNLOCK();
          if (iVar9 == 1) {
            (**(code **)(*plVar8 + 8))(plVar8);
          }
        }
      }
      plVar8 = local_90;
      if (local_90 != (longlong *)0x0) {
        LOCK();
        plVar1 = local_90 + 1;
        lVar4 = *plVar1;
        *(int *)plVar1 = (int)*plVar1 + -1;
        UNLOCK();
        if ((int)lVar4 == 1) {
          (**(code **)*local_90)(local_90);
          LOCK();
          piVar2 = (int *)((longlong)plVar8 + 0xc);
          iVar9 = *piVar2;
          *piVar2 = *piVar2 + -1;
          UNLOCK();
          if (iVar9 == 1) {
            (**(code **)(*plVar8 + 8))(plVar8);
          }
        }
      }
      FUN_140241680(param_1 + 0x16);
    }
    if ((param_1[6] != 0) && (*(char *)(param_1[6] + 0x149) != '\0')) {
      if (param_1[4] != 0) {
        LOCK();
        *(undefined1 *)(param_1[4] + 0x148) = 1;
        UNLOCK();
      }
      FUN_1402322b0(param_1 + 4,param_1 + 6);
      param_1[6] = 0;
      plVar8 = (longlong *)param_1[7];
      param_1[7] = 0;
      if (plVar8 != (longlong *)0x0) {
        LOCK();
        plVar1 = plVar8 + 1;
        lVar4 = *plVar1;
        *(int *)plVar1 = (int)*plVar1 + -1;
        UNLOCK();
        if ((int)lVar4 == 1) {
          (**(code **)*plVar8)(plVar8);
          LOCK();
          piVar2 = (int *)((longlong)plVar8 + 0xc);
          iVar9 = *piVar2;
          *piVar2 = *piVar2 + -1;
          UNLOCK();
          if (iVar9 == 1) {
            (**(code **)(*plVar8 + 8))(plVar8);
          }
        }
      }
    }
    if (param_5 == '\0') {
      if ((((*(longlong *)(param_1[4] + 0x138) != *(longlong *)(param_3 + 0x740)) ||
           (*(longlong *)(param_1[4] + 0x140) != *(longlong *)(param_3 + 0x1590))) &&
          ((lVar4 = param_1[6], lVar4 == 0 ||
           ((*(longlong *)(lVar4 + 0x138) != *(longlong *)(param_3 + 0x740) ||
            (*(longlong *)(lVar4 + 0x140) != *(longlong *)(param_3 + 0x1590))))))) &&
         (dVar11 = (double)FUN_140249580(param_1 + 0x17), dVar5 < dVar11)) {
        if (param_1[6] != 0) {
          LOCK();
          *(undefined1 *)(param_1[6] + 0x148) = 1;
          UNLOCK();
        }
        plVar8 = (longlong *)FUN_140470230(local_a8);
        lVar4 = *plVar8;
        lVar3 = plVar8[1];
        *plVar8 = 0;
        plVar8[1] = 0;
        param_1[6] = lVar4;
        plVar8 = (longlong *)param_1[7];
        param_1[7] = lVar3;
        if (plVar8 != (longlong *)0x0) {
          LOCK();
          plVar1 = plVar8 + 1;
          lVar4 = *plVar1;
          *(int *)plVar1 = (int)*plVar1 + -1;
          UNLOCK();
          if ((int)lVar4 == 1) {
            (**(code **)*plVar8)(plVar8);
            LOCK();
            piVar2 = (int *)((longlong)plVar8 + 0xc);
            iVar9 = *piVar2;
            *piVar2 = *piVar2 + -1;
            UNLOCK();
            if (iVar9 == 1) {
              (**(code **)(*plVar8 + 8))(plVar8);
            }
          }
        }
        if (local_a0 != (longlong *)0x0) {
          LOCK();
          plVar8 = local_a0 + 1;
          lVar4 = *plVar8;
          *(int *)plVar8 = (int)*plVar8 + -1;
          UNLOCK();
          if ((int)lVar4 == 1) {
            (**(code **)*local_a0)(local_a0);
            LOCK();
            piVar2 = (int *)((longlong)local_a0 + 0xc);
            iVar9 = *piVar2;
            *piVar2 = *piVar2 + -1;
            UNLOCK();
            if (iVar9 == 1) {
              (**(code **)(*local_a0 + 8))(local_a0);
            }
          }
        }
        FUN_140356d20(param_1[6],param_3,local_res18,*(undefined8 *)(param_3 + 0x1590),
                      param_3 + 0x428);
        if (param_2[1] != 0) {
          LOCK();
          piVar2 = (int *)(param_2[1] + 8);
          *piVar2 = *piVar2 + 1;
          UNLOCK();
        }
        local_98 = *param_2;
        local_90 = (longlong *)param_2[1];
        if (param_1[5] != 0) {
          LOCK();
          piVar2 = (int *)(param_1[5] + 8);
          *piVar2 = *piVar2 + 1;
          UNLOCK();
        }
        local_88 = param_1[4];
        local_80 = (longlong *)param_1[5];
        if (param_1[7] != 0) {
          LOCK();
          piVar2 = (int *)(param_1[7] + 8);
          *piVar2 = *piVar2 + 1;
          UNLOCK();
        }
        local_78 = param_1[6];
        local_70 = (longlong *)param_1[7];
        local_68 = iVar10;
        uVar7 = FUN_1402cbb70();
        FUN_140470e30(&local_res8,uVar7,&local_98);
        FUN_140470c70(param_1 + 0xc,&local_res8);
        plVar8 = (longlong *)CONCAT44(uStackX_c,local_res8);
        if (plVar8 != (longlong *)0x0) {
          LOCK();
          lVar4 = *plVar8;
          *plVar8 = *plVar8 + -1;
          UNLOCK();
          if (lVar4 == 1) {
            (**(code **)plVar8[3])();
          }
        }
        plVar8 = local_70;
        if (local_70 != (longlong *)0x0) {
          LOCK();
          plVar1 = local_70 + 1;
          lVar4 = *plVar1;
          *(int *)plVar1 = (int)*plVar1 + -1;
          UNLOCK();
          if ((int)lVar4 == 1) {
            (**(code **)*local_70)(local_70);
            LOCK();
            piVar2 = (int *)((longlong)plVar8 + 0xc);
            iVar9 = *piVar2;
            *piVar2 = *piVar2 + -1;
            UNLOCK();
            if (iVar9 == 1) {
              (**(code **)(*plVar8 + 8))(plVar8);
            }
          }
        }
        plVar8 = local_80;
        if (local_80 != (longlong *)0x0) {
          LOCK();
          plVar1 = local_80 + 1;
          lVar4 = *plVar1;
          *(int *)plVar1 = (int)*plVar1 + -1;
          UNLOCK();
          if ((int)lVar4 == 1) {
            (**(code **)*local_80)(local_80);
            LOCK();
            piVar2 = (int *)((longlong)plVar8 + 0xc);
            iVar9 = *piVar2;
            *piVar2 = *piVar2 + -1;
            UNLOCK();
            if (iVar9 == 1) {
              (**(code **)(*plVar8 + 8))(plVar8);
            }
          }
        }
        plVar8 = local_90;
        if (local_90 != (longlong *)0x0) {
          LOCK();
          plVar1 = local_90 + 1;
          lVar4 = *plVar1;
          *(int *)plVar1 = (int)*plVar1 + -1;
          UNLOCK();
          if ((int)lVar4 == 1) {
            (**(code **)*local_90)(local_90);
            LOCK();
            piVar2 = (int *)((longlong)plVar8 + 0xc);
            iVar9 = *piVar2;
            *piVar2 = *piVar2 + -1;
            UNLOCK();
            if (iVar9 == 1) {
              (**(code **)(*plVar8 + 8))(plVar8);
            }
          }
        }
        FUN_140241680(param_1 + 0x17);
      }
    }
    FUN_140230e20(param_2);
  }
  return;
}


// ref 140c247f4 caller none
// ref 140478f95 caller 140478f40
