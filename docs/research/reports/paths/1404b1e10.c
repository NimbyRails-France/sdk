// Candidate VA 1404b1e10; RVA 0x4b1e10
// Ghidra inferred prototype: undefined FUN_1404b1e10()

void FUN_1404b1e10(longlong param_1,longlong *param_2)

{
  longlong lVar1;
  int *piVar2;
  longlong lVar3;
  int *piVar4;
  longlong local_res8;
  longlong local_res10;
  
  if (0xbc < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x18))(param_2,param_1,8,0);
  }
  if (0xd8 < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))(param_2,"nimby::model::Train::Dynamics");
    FUN_1403157f0(param_1 + 8,param_2);
    (**(code **)(*param_2 + 0x10))(param_2);
    if ((0xd8 < (int)param_2[1]) &&
       ((**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x78,8,1), 0xd8 < (int)param_2[1])) {
      (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x80,4,0);
    }
  }
  if (0xd8 < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))
              (param_2,
               "??$visit@UCachedHitcher@Motion@model@nimby@@@serde@@YAXPEBV?$vector@UCachedHitcher@Motion@model@nimby@@Vallocator@eastl@@@eastl@@PEAUSerializer@0@@Z"
              );
    local_res8 = *(longlong *)(param_1 + 0x90) - *(longlong *)(param_1 + 0x88) >> 5;
    (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,1);
    lVar1 = *(longlong *)(param_1 + 0x90);
    for (lVar3 = *(longlong *)(param_1 + 0x88); lVar3 != lVar1; lVar3 = lVar3 + 0x20) {
      (**(code **)(*param_2 + 8))(param_2,"nimby::model::Motion::CachedHitcher");
      if ((0xd8 < (int)param_2[1]) &&
         ((**(code **)(*param_2 + 0x18))(param_2,lVar3,8,0), 0xd8 < (int)param_2[1])) {
        (**(code **)(*param_2 + 0x18))(param_2,lVar3 + 8,8);
      }
      if (0xdc < (int)param_2[1]) {
        (**(code **)(*param_2 + 0x18))(param_2,lVar3 + 0x10,8);
      }
      (**(code **)(*param_2 + 0x10))(param_2);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
    if (0xd8 < (int)param_2[1]) {
      (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0xa0,8,1);
    }
  }
  if (0xbc < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))
              (param_2,
               "??$visit@UFatal@Motion@model@nimby@@@serde@@YAXPEBV?$optional@UFatal@Motion@model@nimby@@@std@@PEAUSerializer@0@@Z"
              );
    local_res8 = CONCAT71(local_res8._1_7_,*(undefined1 *)(param_1 + 0xb0));
    (**(code **)(*param_2 + 0x18))(param_2,&local_res8,1,1);
    if ((char)local_res8 != '\0') {
      if (*(char *)(param_1 + 0xb0) == '\0') {
                    /* WARNING: Subroutine does not return */
        FUN_1402f2020();
      }
      (**(code **)(*param_2 + 8))(param_2,"nimby::model::Motion::Fatal");
      if (0xbc < (int)param_2[1]) {
        local_res10 = (longlong)*(int *)(param_1 + 0xa8);
        (**(code **)(*param_2 + 0x18))(param_2,&local_res10,8,0);
      }
      if (0xc4 < (int)param_2[1]) {
        (**(code **)(*param_2 + 0x20))(param_2,param_1 + 0xac);
      }
      (**(code **)(*param_2 + 0x10))(param_2);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
    if (0xbc < (int)param_2[1]) {
      (**(code **)(*param_2 + 8))
                (param_2,
                 "??$visit@UPresence@Motion@model@nimby@@@serde@@YAXPEBV?$optional@UPresence@Motion@model@nimby@@@std@@PEAUSerializer@0@@Z"
                );
      local_res8 = CONCAT71(local_res8._1_7_,*(undefined1 *)(param_1 + 0x1d0));
      (**(code **)(*param_2 + 0x18))(param_2,&local_res8,1,1);
      if ((char)local_res8 != '\0') {
        if (*(char *)(param_1 + 0x1d0) == '\0') {
                    /* WARNING: Subroutine does not return */
          FUN_1402f2020();
        }
        FUN_14049b620(param_1 + 0xb8,param_2);
      }
      (**(code **)(*param_2 + 0x10))(param_2);
    }
  }
  if (0xd8 < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))
              (param_2,
               "??$visit@UHitch@Motion@model@nimby@@@serde@@YAXPEBV?$optional@UHitch@Motion@model@nimby@@@std@@PEAUSerializer@0@@Z"
              );
    local_res8 = CONCAT71(local_res8._1_7_,*(undefined1 *)(param_1 + 0x1f0));
    (**(code **)(*param_2 + 0x18))(param_2,&local_res8,1,1);
    if ((char)local_res8 != '\0') {
      if (*(char *)(param_1 + 0x1f0) == '\0') {
                    /* WARNING: Subroutine does not return */
        FUN_1402f2020();
      }
      (**(code **)(*param_2 + 8))(param_2,"nimby::model::Motion::Hitch");
      if (0xd8 < (int)param_2[1]) {
        (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x1d8,8,0);
      }
      if (0xdc < (int)param_2[1]) {
        (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x1e0,8,0);
      }
      if (0xdd < (int)param_2[1]) {
        (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x1e8,1,1);
      }
      (**(code **)(*param_2 + 0x10))(param_2);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  if (0xbc < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))
              (param_2,
               "??$visit@UBlackhole@Motion@model@nimby@@@serde@@YAXPEBV?$optional@UBlackhole@Motion@model@nimby@@@std@@PEAUSerializer@0@@Z"
              );
    local_res8 = CONCAT71(local_res8._1_7_,*(undefined1 *)(param_1 + 0x218));
    (**(code **)(*param_2 + 0x18))(param_2,&local_res8,1,1);
    if ((char)local_res8 != '\0') {
      if (*(char *)(param_1 + 0x218) == '\0') {
                    /* WARNING: Subroutine does not return */
        FUN_1402f2020();
      }
      (**(code **)(*param_2 + 8))(param_2,"nimby::model::Motion::Blackhole");
      if ((0xbc < (int)param_2[1]) &&
         (FUN_1402f2ee0(param_1 + 0x1f8,param_2), 0xbc < (int)param_2[1])) {
        (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x210,1,1);
      }
      (**(code **)(*param_2 + 0x10))(param_2);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
    if ((0xbc < (int)param_2[1]) && (FUN_1404bde40(param_1 + 0x220,param_2), 0xbc < (int)param_2[1])
       ) {
      (**(code **)(*param_2 + 8))
                (param_2,
                 "??$visit@UDrive@Motion@model@nimby@@@serde@@YAXPEBV?$optional@UDrive@Motion@model@nimby@@@std@@PEAUSerializer@0@@Z"
                );
      local_res8 = CONCAT71(local_res8._1_7_,*(undefined1 *)(param_1 + 0x4b0));
      (**(code **)(*param_2 + 0x18))(param_2,&local_res8,1,1);
      if ((char)local_res8 != '\0') {
        if (*(char *)(param_1 + 0x4b0) == '\0') {
                    /* WARNING: Subroutine does not return */
          FUN_1402f2020();
        }
        (**(code **)(*param_2 + 8))(param_2,"nimby::model::Motion::Drive");
        FUN_1404b1660(param_1 + 0x290,param_2);
        (**(code **)(*param_2 + 0x10))(param_2);
      }
      (**(code **)(*param_2 + 0x10))(param_2);
      if (0xbc < (int)param_2[1]) {
        (**(code **)(*param_2 + 8))
                  (param_2,
                   "??$visit@UTimedStop@Motion@model@nimby@@@serde@@YAXPEBV?$optional@UTimedStop@Motion@model@nimby@@@std@@PEAUSerializer@0@@Z"
                  );
        local_res8 = CONCAT71(local_res8._1_7_,*(undefined1 *)(param_1 + 0x4d0));
        (**(code **)(*param_2 + 0x18))(param_2,&local_res8,1,1);
        if ((char)local_res8 != '\0') {
          if (*(char *)(param_1 + 0x4d0) == '\0') {
                    /* WARNING: Subroutine does not return */
            FUN_1402f2020();
          }
          (**(code **)(*param_2 + 8))(param_2,"nimby::model::Motion::TimedStop");
          if (((0xbc < (int)param_2[1]) &&
              ((**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x4b8,8,0), 0xbc < (int)param_2[1]))
             && ((**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x4c0,8,0), 0xbc < (int)param_2[1]
                )) {
            (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x4c8,4,0);
          }
          (**(code **)(*param_2 + 0x10))(param_2);
        }
        (**(code **)(*param_2 + 0x10))(param_2);
        if (0xbc < (int)param_2[1]) {
          FUN_1404bdff0(param_1 + 0x4d8,param_2);
        }
      }
    }
  }
  if (0xd9 < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))
              (param_2,
               "??$visit@URunStop@Motion@model@nimby@@@serde@@YAXPEBV?$optional@URunStop@Motion@model@nimby@@@std@@PEAUSerializer@0@@Z"
              );
    local_res8 = CONCAT71(local_res8._1_7_,*(undefined1 *)(param_1 + 0x510));
    (**(code **)(*param_2 + 0x18))(param_2,&local_res8,1,1);
    if ((char)local_res8 != '\0') {
      if (*(char *)(param_1 + 0x510) == '\0') {
                    /* WARNING: Subroutine does not return */
        FUN_1402f2020();
      }
      (**(code **)(*param_2 + 8))(param_2,"nimby::model::Motion::RunStop");
      if (((0xd9 < (int)param_2[1]) &&
          ((**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x4f8,8,0), 0xd9 < (int)param_2[1])) &&
         (((**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x500,8,0), 0xd9 < (int)param_2[1] &&
          ((**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x508,4,0), 0xd9 < (int)param_2[1])))) {
        (**(code **)(*param_2 + 0x20))(param_2,param_1 + 0x50c);
      }
      (**(code **)(*param_2 + 0x10))(param_2);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  if (0xbc < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))
              (param_2,
               "??$visit@UStationStop@Motion@model@nimby@@@serde@@YAXPEBV?$optional@UStationStop@Motion@model@nimby@@@std@@PEAUSerializer@0@@Z"
              );
    local_res8 = CONCAT71(local_res8._1_7_,*(undefined1 *)(param_1 + 0x558));
    (**(code **)(*param_2 + 0x18))(param_2,&local_res8,1,1);
    if ((char)local_res8 != '\0') {
      if (*(char *)(param_1 + 0x558) == '\0') {
                    /* WARNING: Subroutine does not return */
        FUN_1402f2020();
      }
      (**(code **)(*param_2 + 8))(param_2,"nimby::model::Motion::StationStop");
      FUN_1404b1c40(param_1 + 0x518,param_2);
      (**(code **)(*param_2 + 0x10))(param_2);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
    if (0xbc < (int)param_2[1]) {
      (**(code **)(*param_2 + 8))
                (param_2,
                 "??$visit@UDispatchCooldown@Motion@model@nimby@@@serde@@YAXPEBV?$optional@UDispatchCooldown@Motion@model@nimby@@@std@@PEAUSerializer@0@@Z"
                );
      local_res8 = CONCAT71(local_res8._1_7_,*(undefined1 *)(param_1 + 0x598));
      (**(code **)(*param_2 + 0x18))(param_2,&local_res8,1,1);
      if ((char)local_res8 != '\0') {
        if (*(char *)(param_1 + 0x598) == '\0') {
                    /* WARNING: Subroutine does not return */
          FUN_1402f2020();
        }
        (**(code **)(*param_2 + 8))(param_2,"nimby::model::Motion::DispatchCooldown");
        if (0xbc < (int)param_2[1]) {
          (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x560,8,0);
        }
        (**(code **)(*param_2 + 0x10))(param_2);
      }
      (**(code **)(*param_2 + 0x10))(param_2);
    }
  }
  if (0xd9 < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))
              (param_2,
               "??$visit@URunDispatch@Motion@model@nimby@@@serde@@YAXPEBV?$optional@URunDispatch@Motion@model@nimby@@@std@@PEAUSerializer@0@@Z"
              );
    local_res8 = CONCAT71(local_res8._1_7_,*(undefined1 *)(param_1 + 0x5d0));
    (**(code **)(*param_2 + 0x18))(param_2,&local_res8,1,1);
    if ((char)local_res8 != '\0') {
      if (*(char *)(param_1 + 0x5d0) == '\0') {
                    /* WARNING: Subroutine does not return */
        FUN_1402f2020();
      }
      (**(code **)(*param_2 + 8))(param_2,"nimby::model::Motion::RunDispatch");
      if (((0xd9 < (int)param_2[1]) &&
          (FUN_14049b510(param_1 + 0x5a0,param_2), 0xd9 < (int)param_2[1])) &&
         ((**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x5c0,8,0), 0xd9 < (int)param_2[1])) {
        (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x5c8,4,0);
      }
      (**(code **)(*param_2 + 0x10))(param_2);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  if ((0xbc < (int)param_2[1]) && (FUN_1404be180(param_1 + 0x5d8,param_2), 0xbc < (int)param_2[1]))
  {
    (**(code **)(*param_2 + 8))
              (param_2,
               "??$visit@UEndedDrive@Motion@model@nimby@@@serde@@YAXPEBV?$optional@UEndedDrive@Motion@model@nimby@@@std@@PEAUSerializer@0@@Z"
              );
    local_res8 = CONCAT71(local_res8._1_7_,*(undefined1 *)(param_1 + 0x608));
    (**(code **)(*param_2 + 0x18))(param_2,&local_res8,1,1);
    if ((char)local_res8 != '\0') {
      if (*(char *)(param_1 + 0x608) == '\0') {
                    /* WARNING: Subroutine does not return */
        FUN_1402f2020();
      }
      (**(code **)(*param_2 + 8))(param_2,"nimby::model::Motion::EndedDrive");
      if (0xbc < (int)param_2[1]) {
        (**(code **)(*param_2 + 0x20))(param_2,param_1 + 0x5f8);
      }
      local_res10 = 0;
      if ((int)param_2[1] - 0xbdU < 0x25) {
        (**(code **)(*param_2 + 0x18))(param_2,&local_res10,8,0);
      }
      if (0xe1 < (int)param_2[1]) {
        (**(code **)(*param_2 + 0x18))(param_2,param_1 + 0x600,8,0);
      }
      (**(code **)(*param_2 + 0x10))(param_2);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
    if (0xbc < (int)param_2[1]) {
      (**(code **)(*param_2 + 8))
                (param_2,
                 "??$visit@UEndedTimedStop@Motion@model@nimby@@@serde@@YAXPEBV?$optional@UEndedTimedStop@Motion@model@nimby@@@std@@PEAUSerializer@0@@Z"
                );
      local_res8 = CONCAT71(local_res8._1_7_,*(undefined1 *)(param_1 + 0x611));
      (**(code **)(*param_2 + 0x18))(param_2,&local_res8,1,1);
      if ((char)local_res8 != '\0') {
        if (*(char *)(param_1 + 0x611) == '\0') {
                    /* WARNING: Subroutine does not return */
          FUN_1402f2020();
        }
        (**(code **)(*param_2 + 8))(param_2,"nimby::model::Motion::EndedTimedStop");
        (**(code **)(*param_2 + 0x10))(param_2);
      }
      (**(code **)(*param_2 + 0x10))(param_2);
    }
  }
  if (0xdb < (int)param_2[1]) {
    (**(code **)(*param_2 + 8))
              (param_2,
               "??$visit@UTask@Motion@model@nimby@@@serde@@YAXPEBV?$vector@UTask@Motion@model@nimby@@Vallocator@eastl@@@eastl@@PEAUSerializer@0@@Z"
              );
    local_res8 = *(longlong *)(param_1 + 0x620) - *(longlong *)(param_1 + 0x618) >> 5;
    (**(code **)(*param_2 + 0x18))(param_2,&local_res8,8,1);
    piVar2 = *(int **)(param_1 + 0x620);
    for (piVar4 = *(int **)(param_1 + 0x618); piVar4 != piVar2; piVar4 = piVar4 + 8) {
      (**(code **)(*param_2 + 8))(param_2,"nimby::model::Motion::Task");
      if (0xdb < (int)param_2[1]) {
        local_res10 = (longlong)*piVar4;
        (**(code **)(*param_2 + 0x18))(param_2,&local_res10,8,0);
        if (0xdb < (int)param_2[1]) {
          (**(code **)(*param_2 + 8))(param_2,"nimby::model::ScriptPrivStructInstance");
          if (0xdb < (int)param_2[1]) {
            FUN_1404bacd0(piVar4 + 2,param_2);
          }
          (**(code **)(*param_2 + 0x10))(param_2);
        }
      }
      (**(code **)(*param_2 + 0x10))(param_2);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
  }
  return;
}


// Incoming references
// 0xc26c24 DATA caller none
// 0x4c2b16 UNCONDITIONAL_CALL caller 1404c2a90
