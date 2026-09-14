// Candidate VA 1404b1660; RVA 0x4b1660
// Ghidra inferred prototype: undefined FUN_1404b1660()

void FUN_1404b1660(longlong param_1,longlong *param_2)

{
  undefined8 local_res8;
  
  if (0xbc < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))(param_2,"nimby::model::Path");
    FUN_1404afe30(param_1,param_2);
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  if (0xbc < (int)param_2[1]) {
    FUN_14049b420(param_1 + 0x110,param_2);
  }
  if (0xcd < (int)param_2[1]) {
    FUN_1402f2470(param_1 + 0x158,param_2);
  }
  if (0xbc < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x1c8,8,0);
    if (0xbc < (int)param_2[1]) {
      (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x1d0,8,0);
      if (0xbc < (int)param_2[1]) {
        (**(code **)(*param_2 + 0x20))(param_2,param_1 + 0x1d8);
        if (0xbc < (int)param_2[1]) {
          (**(code **)(*param_2 + 0x20))(param_2,param_1 + 0x1dc);
          if (0xbc < (int)param_2[1]) {
            (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x1e0,4,0);
            if (0xbc < (int)param_2[1]) {
              (**(code **)(*param_2 + 0x20))(param_2,param_1 + 0x1e4);
              if (0xbc < (int)param_2[1]) {
                (**(code **)(*param_2 + 0x20))(param_2,param_1 + 0x1e8);
                if (0xbc < (int)param_2[1]) {
                  (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x1f0,8,0);
                  if (0xbc < (int)param_2[1]) {
                    (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x1f8,8,0);
                    if (0xbc < (int)param_2[1]) {
                      (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x200,1,1);
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  local_res8 = 0;
  if ((int)param_2[1] - 0xbdU < 6) {
    (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,0);
  }
  if (200 < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))
              (param_2,
               "??$visit@UBrake@Drive@Motion@model@nimby@@@serde@@YAXPEBV?$optional@UBrake@Drive@Motion@model@nimby@@@std@@PEAUSerializer@0@@Z"
              );
    local_res8 = CONCAT71(local_res8._1_7_,*(undefined1 *)(param_1 + 0x210));
    (**(code **)(*param_2 + 0x18))(param_2,&local_res8,1,1);
    if ((char)local_res8 != '\0') {
      if (*(char *)(param_1 + 0x210) == '\0') {
                    /* WARNING: Subroutine does not return */
        FUN_1402f2020();
      }
      (**(code **)(*param_2 + 8))(param_2,"nimby::model::Motion::Drive::Brake");
      if (200 < (int)param_2[1]) {
        (**(code **)(*param_2 + 0x20))(param_2,param_1 + 0x204);
        if (200 < (int)param_2[1]) {
          (**(code **)(*param_2 + 0x20))(param_2,param_1 + 0x208);
        }
      }
      if (0xd6 < (int)param_2[1]) {
        (**(code **)(*param_2 + 0x20))(param_2,param_1 + 0x20c);
      }
      (**(code **)(*param_2 + 0x10))(param_2);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
    if (200 < (int)param_2[1]) {
      (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x218,8,0);
    }
  }
  return;
}


// Incoming references
// 0xc26c00 DATA caller none
// 0x4b22f0 UNCONDITIONAL_CALL caller 1404b1e10
