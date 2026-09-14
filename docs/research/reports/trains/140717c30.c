// Candidate VA 140717c30; RVA 0x717c30
// Ghidra inferred prototype: undefined FUN_140717c30()

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_140717c30(longlong param_1,longlong *param_2,longlong param_3,float *param_4,float *param_5
                  ,undefined8 param_6,undefined8 param_7,float *param_8,float *param_9,
                  float *param_10,float *param_11)

{
  float fVar1;
  float fVar2;
  undefined8 uVar3;
  float fVar4;
  float fVar5;
  undefined2 local_68 [2];
  float local_64;
  undefined4 local_60;
  undefined4 local_5c;
  undefined8 local_58;
  
  fVar5 = *(float *)(*param_2 + 0x1550);
  fVar1 = *param_5;
  fVar2 = *param_4;
  if (fVar5 < 0.0) {
    fVar5 = DAT_140aabae4;
  }
  local_68[0] = *(undefined2 *)(*param_2 + 0x15ba);
  local_64 = fVar5 * DAT_140aac254;
  local_5c = *(undefined4 *)(param_1 + 0xc);
  fVar4 = DAT_140aaba24 / fVar5;
  local_60 = 0x11;
  local_58 = 0;
  (**(code **)(**(longlong **)(param_3 + 8) + 0xf0))
            (*(longlong **)(param_3 + 8),fVar4 * *param_8,fVar4 * *param_9,fVar4 * *param_10,
             fVar4 * *param_11);
  (**(code **)(**(longlong **)(param_3 + 8) + 0xd8))();
  (**(code **)(**(longlong **)(param_3 + 8) + 0x110))
            (*(longlong **)(param_3 + 8),(*(float *)(param_1 + 8) - DAT_140aac19c) * fVar5 + fVar2,
             (*(float *)(param_1 + 8) - _DAT_140aac278) * fVar5 + fVar1);
  (**(code **)(**(longlong **)(param_3 + 8) + 0x118))(*(longlong **)(param_3 + 8),DAT_140aac650);
  uVar3 = FUN_1402d82e0("schedule_group_trains_table_trains","Trains");
  (**(code **)(**(longlong **)(param_3 + 8) + 0x138))
            (*(longlong **)(param_3 + 8),local_68,0,0,uVar3,0);
  (**(code **)(**(longlong **)(param_3 + 8) + 0xe0))();
  (**(code **)(**(longlong **)(param_3 + 8) + 0xe8))();
  return;
}


// Incoming references
// 0xc36194 DATA caller none
// 0xa95ad0 DATA caller none
