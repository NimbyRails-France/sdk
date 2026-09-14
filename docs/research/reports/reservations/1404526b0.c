// Candidate VA 1404526b0; RVA 0x4526b0
// Ghidra inferred prototype: undefined FUN_1404526b0()

longlong FUN_1404526b0(undefined8 param_1,longlong *param_2,undefined8 *param_3,longlong param_4)

{
  longlong lVar1;
  longlong lVar2;
  double *pdVar3;
  longlong *plVar4;
  double *pdVar5;
  double dVar6;
  double dVar7;
  double local_68 [3];
  double local_50 [3];
  undefined1 local_38 [48];
  
  if (((param_2 != (longlong *)0x0) && (param_3 != (undefined8 *)0x0)) &&
     ((param_2[0x1b] != 0 || (param_2[0x1a] != 0)))) {
    lVar1 = *param_2;
    lVar2 = FUN_14032c420(lVar1,*param_3);
    if ((lVar2 != 0) && (0.0 < *(double *)(lVar2 + 0x88))) {
      local_68[0] = 0.0;
      local_50[1] = DAT_140aabd08 / *(double *)(lVar2 + 0x88);
      local_68[2] = DAT_140aabd08;
      if (DAT_140aabbf8 <= local_50[1]) {
        local_50[1] = DAT_140aabbf8;
      }
      local_50[2] = DAT_140aabd08;
      local_50[0] = 0.0;
      local_68[1] = (double)param_3[1] - local_50[1];
      local_50[1] = (double)param_3[1] + local_50[1];
      pdVar5 = local_68;
      if (0.0 <= local_68[1]) {
        pdVar5 = local_68 + 1;
      }
      pdVar3 = local_68 + 2;
      if (local_68[1] <= DAT_140aabd08) {
        pdVar3 = pdVar5;
      }
      dVar7 = *pdVar3;
      pdVar5 = local_50;
      if (0.0 <= local_50[1]) {
        pdVar5 = local_50 + 1;
      }
      pdVar3 = local_50 + 2;
      if (local_50[1] <= DAT_140aabd08) {
        pdVar3 = pdVar5;
      }
      dVar6 = *pdVar3;
      FUN_140459ea0(param_2[0xd],local_38,*param_3);
      plVar4 = (longlong *)FUN_140459ae0(local_38);
      while (plVar4 != (longlong *)0x0) {
        if (((*plVar4 != 0) && (*plVar4 != param_4)) &&
           (lVar2 = FUN_14033f780(lVar1 + 0x200), lVar2 != 0)) {
          pdVar5 = (double *)(plVar4 + 1);
          if ((double)plVar4[1] < (double)plVar4[2]) {
            pdVar5 = (double *)(plVar4 + 2);
          }
          if (dVar7 < *pdVar5 || dVar7 == *pdVar5) {
            pdVar5 = (double *)(plVar4 + 1);
            if ((double)plVar4[2] < (double)plVar4[1]) {
              pdVar5 = (double *)(plVar4 + 2);
            }
            if (*pdVar5 <= dVar6) {
              return lVar2;
            }
          }
        }
        plVar4 = (longlong *)FUN_140459ae0(local_38);
      }
    }
  }
  return 0;
}


// Incoming references
// 0xc23924 DATA caller none
// 0x45287c UNCONDITIONAL_CALL caller 140452860
// 0x454e55 DATA caller 140452f40
