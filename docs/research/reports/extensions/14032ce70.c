// Candidate VA 14032ce70; RVA 0x32ce70
// Ghidra inferred prototype: undefined FUN_14032ce70()

void FUN_14032ce70(undefined8 *param_1,longlong *param_2)

{
  undefined8 *puVar1;
  undefined8 local_res10;
  longlong local_res18;
  undefined8 *local_res20;

  (**(code **)(*param_2 + 8))
            (param_2,
             "??$visit@_JN_NUEnumRepr@ScriptStructInstance@model@nimby@@U?$IDRef@UTrack@model@nimby@@@34@U?$IDRef@UStationGroup@model@nimby@@@34@U?$IDRef@UBuilding@model@nimby@@@34@U?$IDRef@ULine@model@nimby@@@34@U?$IDRef@UTrain@model@nimby@@@34@U?$IDRef@USchedule@model@nimby@@@34@U?$IDRef@UScript@model@nimby@@@34@U?$IDRef@USignal@model@nimby@@@34@U?$IDRef@UTagKind@model@nimby@@@34@U?$HeapBox@UTags@model@nimby@@@34@U?$HeapBox@V?$vector@U?$IDRef@ULine@model@nimby@@@model@nimby@@Vallocator@eastl@@@eastl@@@34@U?$HeapBox@V?$vector@U?$IDRef@UTrain@model@nimby@@@model@nimby@@Vallocator@eastl@@@eastl@@@34@U?$HeapBox@V?$vector@U?$IDRef@USchedule@model@nimby@@@model@nimby@@Vallocator@eastl@@@eastl@@@34@U?$HeapBox@V?$vector@U?$IDRef@USignal@model@nimby@@@model@nimby@@Vallocator@eastl@@@eastl@@@34@@serde@@YAXPEAV?$variant@_JN_NUEnumRepr@ScriptStructInstance@model@nimby@@U?$IDRef@UTrack@model@nimby@@@34@U?$IDRef@UStationGroup@model@nimby@@@34@U?$IDRef@UBuilding@model@nimby@@@34@U?$IDRef@ULine@model@nimby@@@34@U?$IDRef@UTrain@model@nimby@@@34@U?$IDRef@USchedule@model@nimby@@@34@U?$IDRef@UScript@model@nimby@@@34@U?$IDRef@USignal@model@nimby@@@34@U?$IDRef@UTagKind@model@nimby@@@34@U?$HeapBox@UTags@model@nimby@@@34@U?$HeapBox@V?$vector@U?$IDRef@ULine@model@nimby@@@model@nimby@@Vallocator@eastl@@@eastl@@@34@U?$HeapBox@V?$vector@U?$IDRef@UTrain@model@nimby@@@model@nimby@@Vallocator@eastl@@@eastl@@@34@U?$HeapBox@V?$vector@U?$IDRef@USchedule@model@nimby@@@model@nimby@@Vallocator@eastl@@@eastl@@@34@U?$HeapBox@V?$vector@U?$IDRef@USignal@model@nimby@@@model@nimby@@Vallocator@eastl@@@eastl@@@34@@std@@PEAUDeserializer@0@@Z"
            );
  local_res18 = 0;
  (**(code **)(*param_2 + 0x18))(param_2,&local_res18,8,1);
  if (local_res18 == 0) {
    (**(code **)(*param_2 + 0x18))(param_2,&local_res10,8,0);
    if (*(char *)(param_1 + 1) != '\0') {
      FUN_1403385b0(param_1);
      *(undefined1 *)(param_1 + 1) = 0;
    }
    *param_1 = local_res10;
  }
  if (local_res18 == 1) {
    (**(code **)(*param_2 + 0x28))(param_2,&local_res10);
    if (*(char *)(param_1 + 1) == '\x01') {
      *param_1 = local_res10;
    }
    else {
      FUN_1403385b0(param_1);
      *param_1 = local_res10;
      *(undefined1 *)(param_1 + 1) = 1;
    }
  }
  if (local_res18 == 2) {
    (**(code **)(*param_2 + 0x18))(param_2,&local_res10,1,1);
    if (*(char *)(param_1 + 1) != '\x02') {
      FUN_1403385b0(param_1);
      *(undefined1 *)(param_1 + 1) = 2;
    }
    *(undefined1 *)param_1 = (undefined1)local_res10;
  }
  if (local_res18 == 3) {
    (**(code **)(*param_2 + 8))(param_2,"nimby::model::ScriptStructInstance::EnumRepr");
    if (0xcb < (int)param_2[1]) {
      (**(code **)(*param_2 + 0x18))(param_2,&local_res10,8,1);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
    if (*(char *)(param_1 + 1) != '\x03') {
      FUN_1403385b0(param_1);
      *(undefined1 *)(param_1 + 1) = 3;
    }
    *param_1 = local_res10;
  }
  if (local_res18 == 4) {
    local_res10 = 0;
    (**(code **)(*param_2 + 8))(param_2,"nimby::model::IDRef<nimby::model::Track>");
    if (0xcb < (int)param_2[1]) {
      (**(code **)(*param_2 + 0x18))(param_2,&local_res10,8,0);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
    if (*(char *)(param_1 + 1) != '\x04') {
      FUN_1403385b0(param_1);
      *(undefined1 *)(param_1 + 1) = 4;
    }
    *param_1 = local_res10;
  }
  if (local_res18 == 5) {
    local_res10 = 0;
    (**(code **)(*param_2 + 8))(param_2,"nimby::model::IDRef<nimby::model::StationGroup>");
    if (0xcb < (int)param_2[1]) {
      (**(code **)(*param_2 + 0x18))(param_2,&local_res10,8,0);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
    switch(*(undefined1 *)(param_1 + 1)) {
    case 0xd:
    case 0xe:
    case 0xf:
    case 0x10:
    case 0x11:
      puVar1 = (undefined8 *)*param_1;
      if (puVar1 != (undefined8 *)0x0) {
        if ((void *)*puVar1 != (void *)0x0) {
          free((void *)*puVar1);
        }
        free(puVar1);
      }
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 0xb:
    case 0xc:
      *param_1 = local_res10;
      *(undefined1 *)(param_1 + 1) = 5;
      break;
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1a:
    case 0x1b:
    case 0x1c:
    case 0x1d:
    case 0x1e:
    case 0x1f:
      goto switchD_14032d372_caseD_7;
    case 0x20:
    case 0x21:
    case 0x22:
    case 0x23:
    case 0x24:
      goto switchD_14032d372_caseD_d;
    case 0x25:
    case 0x26:
    case 0x27:
    case 0x28:
    case 0x29:
    case 0x2a:
    case 0x2b:
    case 0x2c:
    case 0x2d:
    case 0x2e:
    case 0x2f:
    case 0x30:
    case 0x31:
    case 0x32:
      goto switchD_14032d3b1_caseD_8;
    case 0x33:
    case 0x34:
    case 0x35:
      goto switchD_14032d3b1_caseD_d;
    case 0x36:
      goto switchD_14032d3b1_caseD_10;
    case 0x37:
      goto switchD_14032d3b1_caseD_11;
    case 0x38:
    case 0x39:
    case 0x3a:
    case 0x3b:
    case 0x3c:
    case 0x3d:
    case 0x3e:
    case 0x3f:
    case 0x40:
    case 0x41:
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x45:
      goto switchD_14032d420_caseD_9;
    case 0x46:
      goto switchD_14032d420_caseD_d;
    case 0x47:
      goto switchD_14032d420_caseD_e;
    case 0x48:
      goto switchD_14032d420_caseD_f;
    case 0x49:
      goto switchD_14032d420_caseD_10;
    case 0x4a:
      goto switchD_14032d420_caseD_11;
    case 0x4b:
    case 0x4c:
    case 0x4d:
    case 0x4e:
    case 0x4f:
    case 0x50:
    case 0x51:
    case 0x52:
    case 0x53:
    case 0x54:
    case 0x55:
    case 0x56:
    case 0x57:
    case 0x58:
      goto switchD_14032d4a5_caseD_a;
    case 0x59:
      goto switchD_14032d4a5_caseD_d;
    case 0x5a:
      goto switchD_14032d4a5_caseD_e;
    case 0x5b:
      goto switchD_14032d4a5_caseD_f;
    case 0x5c:
      goto switchD_14032d4a5_caseD_10;
    case 0x5d:
      goto switchD_14032d4a5_caseD_11;
    case 0x5e:
    case 0x5f:
    case 0x60:
    case 0x61:
    case 0x62:
    case 99:
    case 100:
    case 0x65:
    case 0x66:
    case 0x67:
    case 0x68:
    case 0x69:
    case 0x6a:
    case 0x6b:
      goto switchD_14032d52a_caseD_b;
    case 0x6c:
      goto switchD_14032d52a_caseD_d;
    case 0x6d:
      goto switchD_14032d52a_caseD_e;
    case 0x6e:
      goto switchD_14032d52a_caseD_f;
    case 0x6f:
      goto switchD_14032d52a_caseD_10;
    case 0x70:
      goto switchD_14032d52a_caseD_11;
    case 0x71:
    case 0x72:
    case 0x73:
    case 0x74:
    case 0x75:
    case 0x76:
    case 0x77:
    case 0x78:
    case 0x79:
    case 0x7a:
    case 0x7b:
    case 0x7c:
    case 0x7d:
    case 0x7e:
      goto switchD_14032d5af_caseD_c;
    case 0x7f:
      goto switchD_14032d5af_caseD_d;
    default:
      *param_1 = local_res10;
    }
  }
  if (local_res18 == 6) {
    local_res10 = 0;
    (**(code **)(*param_2 + 8))(param_2,"nimby::model::IDRef<nimby::model::Building>");
    if (0xcb < (int)param_2[1]) {
      (**(code **)(*param_2 + 0x18))(param_2,&local_res10,8,0);
    }
    (**(code **)(*param_2 + 0x10))(param_2);
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
    switch(*(undefined1 *)(param_1 + 1)) {
    case 0xd:
    case 0xe:
    case 0xf:
    case 0x10:
    case 0x11:
switchD_14032d372_caseD_d:
      puVar1 = (undefined8 *)*param_1;
      if (puVar1 != (undefined8 *)0x0) {
        if ((void *)*puVar1 != (void *)0x0) {
          free((void *)*puVar1);
        }
        free(puVar1);
      }
    case 7:
    case 8:
    case 9:
    case 10:
    case 0xb:
    case 0xc:
switchD_14032d372_caseD_7:
      *param_1 = local_res10;
      *(undefined1 *)(param_1 + 1) = 6;
      break;
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1a:
    case 0x1b:
    case 0x1c:
    case 0x1d:
    case 0x1e:
    case 0x1f:
      goto switchD_14032d3b1_caseD_8;
    case 0x20:
    case 0x21:
    case 0x22:
      goto switchD_14032d3b1_caseD_d;
    case 0x23:
      goto switchD_14032d3b1_caseD_10;
    case 0x24:
      goto switchD_14032d3b1_caseD_11;
    case 0x25:
    case 0x26:
    case 0x27:
    case 0x28:
    case 0x29:
    case 0x2a:
    case 0x2b:
    case 0x2c:
    case 0x2d:
    case 0x2e:
    case 0x2f:
    case 0x30:
    case 0x31:
    case 0x32:
      goto switchD_14032d420_caseD_9;
    case 0x33:
      goto switchD_14032d420_caseD_d;
    case 0x34:
      goto switchD_14032d420_caseD_e;
    case 0x35:
      goto switchD_14032d420_caseD_f;
    case 0x36:
      goto switchD_14032d420_caseD_10;
    case 0x37:
      goto switchD_14032d420_caseD_11;
    case 0x38:
    case 0x39:
    case 0x3a:
    case 0x3b:
    case 0x3c:
    case 0x3d:
    case 0x3e:
    case 0x3f:
    case 0x40:
    case 0x41:
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x45:
      goto switchD_14032d4a5_caseD_a;
    case 0x46:
      goto switchD_14032d4a5_caseD_d;
    case 0x47:
      goto switchD_14032d4a5_caseD_e;
    case 0x48:
      goto switchD_14032d4a5_caseD_f;
    case 0x49:
      goto switchD_14032d4a5_caseD_10;
    case 0x4a:
      goto switchD_14032d4a5_caseD_11;
    case 0x4b:
    case 0x4c:
    case 0x4d:
    case 0x4e:
    case 0x4f:
    case 0x50:
    case 0x51:
    case 0x52:
    case 0x53:
    case 0x54:
    case 0x55:
    case 0x56:
    case 0x57:
    case 0x58:
      goto switchD_14032d52a_caseD_b;
    case 0x59:
      goto switchD_14032d52a_caseD_d;
    case 0x5a:
      goto switchD_14032d52a_caseD_e;
    case 0x5b:
      goto switchD_14032d52a_caseD_f;
    case 0x5c:
      goto switchD_14032d52a_caseD_10;
    case 0x5d:
      goto switchD_14032d52a_caseD_11;
    case 0x5e:
    case 0x5f:
    case 0x60:
    case 0x61:
    case 0x62:
    case 99:
    case 100:
    case 0x65:
    case 0x66:
    case 0x67:
    case 0x68:
    case 0x69:
    case 0x6a:
    case 0x6b:
      goto switchD_14032d5af_caseD_c;
    case 0x6c:
      goto switchD_14032d5af_caseD_d;
    case 0x6d:
      goto switchD_14032d5af_caseD_e;
    case 0x6e:
      goto switchD_14032d5af_caseD_f;
    case 0x6f:
      goto switchD_14032d5af_caseD_10;
    case 0x70:
      goto switchD_14032d5af_caseD_11;
    case 0x71:
    case 0x72:
    case 0x73:
    case 0x74:
    case 0x75:
    case 0x76:
    case 0x77:
    case 0x78:
    case 0x79:
    case 0x7a:
    case 0x7b:
    case 0x7c:
    case 0x7d:
    case 0x7e:
      goto switchD_14032d5af_caseD_12;
    case 0x7f:
      goto switchD_14032d634_caseD_d;
    default:
      *param_1 = local_res10;
    }
  }
  if (local_res18 != 7) goto LAB_14032d11a;
  local_res10 = 0;
  (**(code **)(*param_2 + 8))(param_2,"nimby::model::IDRef<nimby::model::Line>");
  if (0xcb < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x18))(param_2,&local_res10,8,0);
  }
  (**(code **)(*param_2 + 0x10))(param_2);
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
  switch(*(undefined1 *)(param_1 + 1)) {
  case 0xd:
  case 0xe:
  case 0xf:
switchD_14032d3b1_caseD_d:
    puVar1 = (undefined8 *)*param_1;
    if (puVar1 == (undefined8 *)0x0) goto switchD_14032d3b1_caseD_8;
    if ((void *)*puVar1 != (void *)0x0) {
      free((void *)*puVar1);
    }
    free(puVar1);
    *param_1 = local_res10;
    *(undefined1 *)(param_1 + 1) = 7;
    break;
  case 0x10:
switchD_14032d3b1_caseD_10:
    FUN_14033ba80(param_1);
    *param_1 = local_res10;
    *(undefined1 *)(param_1 + 1) = 7;
    break;
  case 0x11:
switchD_14032d3b1_caseD_11:
    FUN_14033ba80(param_1);
  case 8:
  case 9:
  case 10:
  case 0xb:
  case 0xc:
switchD_14032d3b1_caseD_8:
    *param_1 = local_res10;
    *(undefined1 *)(param_1 + 1) = 7;
    break;
  case 0x12:
  case 0x13:
  case 0x14:
  case 0x15:
  case 0x16:
  case 0x17:
  case 0x18:
  case 0x19:
  case 0x1a:
  case 0x1b:
  case 0x1c:
  case 0x1d:
  case 0x1e:
  case 0x1f:
    goto switchD_14032d420_caseD_9;
  case 0x20:
    goto switchD_14032d420_caseD_d;
  case 0x21:
    goto switchD_14032d420_caseD_e;
  case 0x22:
    goto switchD_14032d420_caseD_f;
  case 0x23:
    goto switchD_14032d420_caseD_10;
  case 0x24:
    goto switchD_14032d420_caseD_11;
  case 0x25:
  case 0x26:
  case 0x27:
  case 0x28:
  case 0x29:
  case 0x2a:
  case 0x2b:
  case 0x2c:
  case 0x2d:
  case 0x2e:
  case 0x2f:
  case 0x30:
  case 0x31:
  case 0x32:
    goto switchD_14032d4a5_caseD_a;
  case 0x33:
    goto switchD_14032d4a5_caseD_d;
  case 0x34:
    goto switchD_14032d4a5_caseD_e;
  case 0x35:
    goto switchD_14032d4a5_caseD_f;
  case 0x36:
    goto switchD_14032d4a5_caseD_10;
  case 0x37:
    goto switchD_14032d4a5_caseD_11;
  case 0x38:
  case 0x39:
  case 0x3a:
  case 0x3b:
  case 0x3c:
  case 0x3d:
  case 0x3e:
  case 0x3f:
  case 0x40:
  case 0x41:
  case 0x42:
  case 0x43:
  case 0x44:
  case 0x45:
    goto switchD_14032d52a_caseD_b;
  case 0x46:
    goto switchD_14032d52a_caseD_d;
  case 0x47:
    goto switchD_14032d52a_caseD_e;
  case 0x48:
    goto switchD_14032d52a_caseD_f;
  case 0x49:
    goto switchD_14032d52a_caseD_10;
  case 0x4a:
    goto switchD_14032d52a_caseD_11;
  case 0x4b:
  case 0x4c:
  case 0x4d:
  case 0x4e:
  case 0x4f:
  case 0x50:
  case 0x51:
  case 0x52:
  case 0x53:
  case 0x54:
  case 0x55:
  case 0x56:
  case 0x57:
  case 0x58:
    goto switchD_14032d5af_caseD_c;
  case 0x59:
    goto switchD_14032d5af_caseD_d;
  case 0x5a:
    goto switchD_14032d5af_caseD_e;
  case 0x5b:
    goto switchD_14032d5af_caseD_f;
  case 0x5c:
    goto switchD_14032d5af_caseD_10;
  case 0x5d:
    goto switchD_14032d5af_caseD_11;
  case 0x5e:
  case 0x5f:
  case 0x60:
  case 0x61:
  case 0x62:
  case 99:
  case 100:
  case 0x65:
  case 0x66:
  case 0x67:
  case 0x68:
  case 0x69:
  case 0x6a:
  case 0x6b:
    goto switchD_14032d5af_caseD_12;
  case 0x6c:
    goto switchD_14032d634_caseD_d;
  case 0x6d:
    goto switchD_14032d634_caseD_e;
  case 0x6e:
    goto switchD_14032d634_caseD_f;
  case 0x6f:
    goto switchD_14032d634_caseD_10;
  case 0x70:
    goto switchD_14032d634_caseD_11;
  case 0x71:
  case 0x72:
  case 0x73:
  case 0x74:
  case 0x75:
  case 0x76:
  case 0x77:
  case 0x78:
  case 0x79:
  case 0x7a:
  case 0x7b:
  case 0x7c:
  case 0x7d:
  case 0x7e:
    goto switchD_14032d634_caseD_12;
  case 0x7f:
    goto switchD_14032d634_caseD_20;
  default:
    *param_1 = local_res10;
  }
LAB_14032d11a:
  if (local_res18 != 8) goto LAB_14032d171;
  local_res10 = 0;
  (**(code **)(*param_2 + 8))(param_2,"nimby::model::IDRef<nimby::model::Train>");
  if (0xcb < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x18))(param_2,&local_res10,8,0);
  }
  (**(code **)(*param_2 + 0x10))(param_2);
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
  switch(*(undefined1 *)(param_1 + 1)) {
  case 0xd:
switchD_14032d420_caseD_d:
    FUN_14033ba80(param_1);
    *param_1 = local_res10;
    *(undefined1 *)(param_1 + 1) = 8;
    break;
  case 0xe:
switchD_14032d420_caseD_e:
    FUN_14033ba80(param_1);
    *param_1 = local_res10;
    *(undefined1 *)(param_1 + 1) = 8;
    break;
  case 0xf:
switchD_14032d420_caseD_f:
    FUN_14033ba80(param_1);
    *param_1 = local_res10;
    *(undefined1 *)(param_1 + 1) = 8;
    break;
  case 0x10:
switchD_14032d420_caseD_10:
    FUN_14033ba80(param_1);
    *param_1 = local_res10;
    *(undefined1 *)(param_1 + 1) = 8;
    break;
  case 0x11:
switchD_14032d420_caseD_11:
    FUN_14033ba80(param_1);
  case 9:
  case 10:
  case 0xb:
  case 0xc:
switchD_14032d420_caseD_9:
    *param_1 = local_res10;
    *(undefined1 *)(param_1 + 1) = 8;
    break;
  case 0x12:
  case 0x13:
  case 0x14:
  case 0x15:
  case 0x16:
  case 0x17:
  case 0x18:
  case 0x19:
  case 0x1a:
  case 0x1b:
  case 0x1c:
  case 0x1d:
  case 0x1e:
  case 0x1f:
    goto switchD_14032d4a5_caseD_a;
  case 0x20:
    goto switchD_14032d4a5_caseD_d;
  case 0x21:
    goto switchD_14032d4a5_caseD_e;
  case 0x22:
    goto switchD_14032d4a5_caseD_f;
  case 0x23:
    goto switchD_14032d4a5_caseD_10;
  case 0x24:
    goto switchD_14032d4a5_caseD_11;
  case 0x25:
  case 0x26:
  case 0x27:
  case 0x28:
  case 0x29:
  case 0x2a:
  case 0x2b:
  case 0x2c:
  case 0x2d:
  case 0x2e:
  case 0x2f:
  case 0x30:
  case 0x31:
  case 0x32:
    goto switchD_14032d52a_caseD_b;
  case 0x33:
    goto switchD_14032d52a_caseD_d;
  case 0x34:
    goto switchD_14032d52a_caseD_e;
  case 0x35:
    goto switchD_14032d52a_caseD_f;
  case 0x36:
    goto switchD_14032d52a_caseD_10;
  case 0x37:
    goto switchD_14032d52a_caseD_11;
  case 0x38:
  case 0x39:
  case 0x3a:
  case 0x3b:
  case 0x3c:
  case 0x3d:
  case 0x3e:
  case 0x3f:
  case 0x40:
  case 0x41:
  case 0x42:
  case 0x43:
  case 0x44:
  case 0x45:
    goto switchD_14032d5af_caseD_c;
  case 0x46:
    goto switchD_14032d5af_caseD_d;
  case 0x47:
    goto switchD_14032d5af_caseD_e;
  case 0x48:
    goto switchD_14032d5af_caseD_f;
  case 0x49:
    goto switchD_14032d5af_caseD_10;
  case 0x4a:
    goto switchD_14032d5af_caseD_11;
  case 0x4b:
  case 0x4c:
  case 0x4d:
  case 0x4e:
  case 0x4f:
  case 0x50:
  case 0x51:
  case 0x52:
  case 0x53:
  case 0x54:
  case 0x55:
  case 0x56:
  case 0x57:
  case 0x58:
    goto switchD_14032d5af_caseD_12;
  case 0x59:
    goto switchD_14032d634_caseD_d;
  case 0x5a:
    goto switchD_14032d634_caseD_e;
  case 0x5b:
    goto switchD_14032d634_caseD_f;
  case 0x5c:
    goto switchD_14032d634_caseD_10;
  case 0x5d:
    goto switchD_14032d634_caseD_11;
  case 0x5e:
  case 0x5f:
  case 0x60:
  case 0x61:
  case 0x62:
  case 99:
  case 100:
  case 0x65:
  case 0x66:
  case 0x67:
  case 0x68:
  case 0x69:
  case 0x6a:
  case 0x6b:
    goto switchD_14032d634_caseD_12;
  case 0x6c:
    goto switchD_14032d634_caseD_20;
  case 0x6d:
    goto switchD_14032d6b9_caseD_e;
  case 0x6e:
    goto switchD_14032d6b9_caseD_f;
  case 0x6f:
    goto switchD_14032d6b9_caseD_10;
  case 0x70:
    goto switchD_14032d6b9_caseD_11;
  default:
    *param_1 = local_res10;
  }
LAB_14032d171:
  if (local_res18 != 9) goto LAB_14032d1c8;
  local_res10 = 0;
  (**(code **)(*param_2 + 8))(param_2,"nimby::model::IDRef<nimby::model::Schedule>");
  if (0xcb < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x18))(param_2,&local_res10,8,0);
  }
  (**(code **)(*param_2 + 0x10))(param_2);
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
  switch(*(undefined1 *)(param_1 + 1)) {
  case 0xd:
switchD_14032d4a5_caseD_d:
    FUN_14033ba80(param_1);
    *param_1 = local_res10;
    *(undefined1 *)(param_1 + 1) = 9;
    break;
  case 0xe:
switchD_14032d4a5_caseD_e:
    FUN_14033ba80(param_1);
    *param_1 = local_res10;
    *(undefined1 *)(param_1 + 1) = 9;
    break;
  case 0xf:
switchD_14032d4a5_caseD_f:
    FUN_14033ba80(param_1);
    *param_1 = local_res10;
    *(undefined1 *)(param_1 + 1) = 9;
    break;
  case 0x10:
switchD_14032d4a5_caseD_10:
    FUN_14033ba80(param_1);
    *param_1 = local_res10;
    *(undefined1 *)(param_1 + 1) = 9;
    break;
  case 0x11:
switchD_14032d4a5_caseD_11:
    FUN_14033ba80(param_1);
  case 10:
  case 0xb:
  case 0xc:
switchD_14032d4a5_caseD_a:
    *param_1 = local_res10;
    *(undefined1 *)(param_1 + 1) = 9;
    break;
  case 0x12:
  case 0x13:
  case 0x14:
  case 0x15:
  case 0x16:
  case 0x17:
  case 0x18:
  case 0x19:
  case 0x1a:
  case 0x1b:
  case 0x1c:
  case 0x1d:
  case 0x1e:
  case 0x1f:
    goto switchD_14032d52a_caseD_b;
  case 0x20:
    goto switchD_14032d52a_caseD_d;
  case 0x21:
    goto switchD_14032d52a_caseD_e;
  case 0x22:
    goto switchD_14032d52a_caseD_f;
  case 0x23:
    goto switchD_14032d52a_caseD_10;
  case 0x24:
    goto switchD_14032d52a_caseD_11;
  case 0x25:
  case 0x26:
  case 0x27:
  case 0x28:
  case 0x29:
  case 0x2a:
  case 0x2b:
  case 0x2c:
  case 0x2d:
  case 0x2e:
  case 0x2f:
  case 0x30:
  case 0x31:
  case 0x32:
    goto switchD_14032d5af_caseD_c;
  case 0x33:
    goto switchD_14032d5af_caseD_d;
  case 0x34:
    goto switchD_14032d5af_caseD_e;
  case 0x35:
    goto switchD_14032d5af_caseD_f;
  case 0x36:
    goto switchD_14032d5af_caseD_10;
  case 0x37:
    goto switchD_14032d5af_caseD_11;
  case 0x38:
  case 0x39:
  case 0x3a:
  case 0x3b:
  case 0x3c:
  case 0x3d:
  case 0x3e:
  case 0x3f:
  case 0x40:
  case 0x41:
  case 0x42:
  case 0x43:
  case 0x44:
  case 0x45:
    goto switchD_14032d5af_caseD_12;
  case 0x46:
    goto switchD_14032d634_caseD_d;
  case 0x47:
    goto switchD_14032d634_caseD_e;
  case 0x48:
    goto switchD_14032d634_caseD_f;
  case 0x49:
    goto switchD_14032d634_caseD_10;
  case 0x4a:
    goto switchD_14032d634_caseD_11;
  case 0x4b:
  case 0x4c:
  case 0x4d:
  case 0x4e:
  case 0x4f:
  case 0x50:
  case 0x51:
  case 0x52:
  case 0x53:
  case 0x54:
  case 0x55:
  case 0x56:
  case 0x57:
  case 0x58:
    goto switchD_14032d634_caseD_12;
  case 0x59:
    goto switchD_14032d634_caseD_20;
  case 0x5a:
    goto switchD_14032d6b9_caseD_e;
  case 0x5b:
    goto switchD_14032d6b9_caseD_f;
  case 0x5c:
    goto switchD_14032d6b9_caseD_10;
  case 0x5d:
    goto switchD_14032d6b9_caseD_11;
  default:
    *param_1 = local_res10;
  }
LAB_14032d1c8:
  if (local_res18 != 10) goto LAB_14032d21f;
  local_res10 = 0;
  (**(code **)(*param_2 + 8))(param_2,"nimby::model::IDRef<nimby::model::Script>");
  if (0xcb < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x18))(param_2,&local_res10,8,0);
  }
  (**(code **)(*param_2 + 0x10))(param_2);
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
  switch(*(undefined1 *)(param_1 + 1)) {
  case 0xd:
switchD_14032d52a_caseD_d:
    FUN_14033ba80(param_1);
    *param_1 = local_res10;
    *(undefined1 *)(param_1 + 1) = 10;
    break;
  case 0xe:
switchD_14032d52a_caseD_e:
    FUN_14033ba80(param_1);
    *param_1 = local_res10;
    *(undefined1 *)(param_1 + 1) = 10;
    break;
  case 0xf:
switchD_14032d52a_caseD_f:
    FUN_14033ba80(param_1);
    *param_1 = local_res10;
    *(undefined1 *)(param_1 + 1) = 10;
    break;
  case 0x10:
switchD_14032d52a_caseD_10:
    FUN_14033ba80(param_1);
    *param_1 = local_res10;
    *(undefined1 *)(param_1 + 1) = 10;
    break;
  case 0x11:
switchD_14032d52a_caseD_11:
    FUN_14033ba80(param_1);
  case 0xb:
  case 0xc:
switchD_14032d52a_caseD_b:
    *param_1 = local_res10;
    *(undefined1 *)(param_1 + 1) = 10;
    break;
  case 0x12:
  case 0x13:
  case 0x14:
  case 0x15:
  case 0x16:
  case 0x17:
  case 0x18:
  case 0x19:
  case 0x1a:
  case 0x1b:
  case 0x1c:
  case 0x1d:
  case 0x1e:
  case 0x1f:
    goto switchD_14032d5af_caseD_c;
  case 0x20:
    goto switchD_14032d5af_caseD_d;
  case 0x21:
    goto switchD_14032d5af_caseD_e;
  case 0x22:
    goto switchD_14032d5af_caseD_f;
  case 0x23:
    goto switchD_14032d5af_caseD_10;
  case 0x24:
    goto switchD_14032d5af_caseD_11;
  case 0x25:
  case 0x26:
  case 0x27:
  case 0x28:
  case 0x29:
  case 0x2a:
  case 0x2b:
  case 0x2c:
  case 0x2d:
  case 0x2e:
  case 0x2f:
  case 0x30:
  case 0x31:
  case 0x32:
    goto switchD_14032d5af_caseD_12;
  case 0x33:
    goto switchD_14032d634_caseD_d;
  case 0x34:
    goto switchD_14032d634_caseD_e;
  case 0x35:
    goto switchD_14032d634_caseD_f;
  case 0x36:
    goto switchD_14032d634_caseD_10;
  case 0x37:
    goto switchD_14032d634_caseD_11;
  case 0x38:
  case 0x39:
  case 0x3a:
  case 0x3b:
  case 0x3c:
  case 0x3d:
  case 0x3e:
  case 0x3f:
  case 0x40:
  case 0x41:
  case 0x42:
  case 0x43:
  case 0x44:
  case 0x45:
    goto switchD_14032d634_caseD_12;
  case 0x46:
    goto switchD_14032d634_caseD_20;
  case 0x47:
    goto switchD_14032d6b9_caseD_e;
  case 0x48:
    goto switchD_14032d6b9_caseD_f;
  case 0x49:
    goto switchD_14032d6b9_caseD_10;
  case 0x4a:
    goto switchD_14032d6b9_caseD_11;
  default:
    *param_1 = local_res10;
  }
LAB_14032d21f:
  if (local_res18 != 0xb) goto LAB_14032d24a;
  local_res10 = 0;
  FUN_1402f2950(&local_res10,param_2);
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
  switch(*(undefined1 *)(param_1 + 1)) {
  case 0xd:
switchD_14032d5af_caseD_d:
    FUN_14033ba80(param_1);
    *param_1 = local_res10;
    *(undefined1 *)(param_1 + 1) = 0xb;
    break;
  case 0xe:
switchD_14032d5af_caseD_e:
    FUN_14033ba80(param_1);
    *param_1 = local_res10;
    *(undefined1 *)(param_1 + 1) = 0xb;
    break;
  case 0xf:
switchD_14032d5af_caseD_f:
    FUN_14033ba80(param_1);
    *param_1 = local_res10;
    *(undefined1 *)(param_1 + 1) = 0xb;
    break;
  case 0x10:
switchD_14032d5af_caseD_10:
    FUN_14033ba80(param_1);
    *param_1 = local_res10;
    *(undefined1 *)(param_1 + 1) = 0xb;
    break;
  case 0x11:
switchD_14032d5af_caseD_11:
    FUN_14033ba80(param_1);
  case 0xc:
switchD_14032d5af_caseD_c:
    *param_1 = local_res10;
    *(undefined1 *)(param_1 + 1) = 0xb;
    break;
  case 0x12:
  case 0x13:
  case 0x14:
  case 0x15:
  case 0x16:
  case 0x17:
  case 0x18:
  case 0x19:
  case 0x1a:
  case 0x1b:
  case 0x1c:
  case 0x1d:
  case 0x1e:
  case 0x1f:
    goto switchD_14032d5af_caseD_12;
  case 0x20:
    goto switchD_14032d634_caseD_d;
  case 0x21:
    goto switchD_14032d634_caseD_e;
  case 0x22:
    goto switchD_14032d634_caseD_f;
  case 0x23:
    goto switchD_14032d634_caseD_10;
  case 0x24:
    goto switchD_14032d634_caseD_11;
  case 0x25:
  case 0x26:
  case 0x27:
  case 0x28:
  case 0x29:
  case 0x2a:
  case 0x2b:
  case 0x2c:
  case 0x2d:
  case 0x2e:
  case 0x2f:
  case 0x30:
  case 0x31:
  case 0x32:
    goto switchD_14032d634_caseD_12;
  case 0x33:
    goto switchD_14032d634_caseD_20;
  case 0x34:
    goto switchD_14032d6b9_caseD_e;
  case 0x35:
    goto switchD_14032d6b9_caseD_f;
  case 0x36:
    goto switchD_14032d6b9_caseD_10;
  case 0x37:
    goto switchD_14032d6b9_caseD_11;
  default:
    *param_1 = local_res10;
  }
LAB_14032d24a:
  if (local_res18 != 0xc) goto LAB_14032d2a1;
  local_res10 = 0;
  (**(code **)(*param_2 + 8))(param_2,"nimby::model::IDRef<nimby::model::TagKind>");
  if (0xcb < (int)param_2[1]) {
    (**(code **)(*param_2 + 0x18))(param_2,&local_res10,8,0);
  }
  (**(code **)(*param_2 + 0x10))(param_2);
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
  switch(*(undefined1 *)(param_1 + 1)) {
  case 0xd:
switchD_14032d634_caseD_d:
    FUN_14033ba80(param_1);
    *param_1 = local_res10;
    *(undefined1 *)(param_1 + 1) = 0xc;
    break;
  case 0xe:
switchD_14032d634_caseD_e:
    FUN_14033ba80(param_1);
    *param_1 = local_res10;
    *(undefined1 *)(param_1 + 1) = 0xc;
    break;
  case 0xf:
switchD_14032d634_caseD_f:
    FUN_14033ba80(param_1);
    *param_1 = local_res10;
    *(undefined1 *)(param_1 + 1) = 0xc;
    break;
  case 0x10:
switchD_14032d634_caseD_10:
    FUN_14033ba80(param_1);
    *param_1 = local_res10;
    *(undefined1 *)(param_1 + 1) = 0xc;
    break;
  case 0x11:
switchD_14032d634_caseD_11:
    FUN_14033ba80(param_1);
switchD_14032d5af_caseD_12:
    *param_1 = local_res10;
    *(undefined1 *)(param_1 + 1) = 0xc;
    break;
  case 0x12:
  case 0x13:
  case 0x14:
  case 0x15:
  case 0x16:
  case 0x17:
  case 0x18:
  case 0x19:
  case 0x1a:
  case 0x1b:
  case 0x1c:
  case 0x1d:
  case 0x1e:
  case 0x1f:
    goto switchD_14032d634_caseD_12;
  case 0x20:
switchD_14032d634_caseD_20:
    FUN_14033ba80(param_1);
    goto switchD_14032d634_caseD_12;
  case 0x21:
    goto switchD_14032d6b9_caseD_e;
  case 0x22:
    goto switchD_14032d6b9_caseD_f;
  case 0x23:
    goto switchD_14032d6b9_caseD_10;
  case 0x24:
    goto switchD_14032d6b9_caseD_11;
  default:
    *param_1 = local_res10;
  }
LAB_14032d2a1:
  local_res10 = 0xe;
  if (local_res18 != 0xd) goto LAB_14032d705;
  puVar1 = (undefined8 *)FUN_140983da8(0x18);
  *puVar1 = 0;
  puVar1[1] = 0;
  *puVar1 = 0;
  puVar1[1] = 0;
  puVar1[2] = 0;
  local_res20 = puVar1;
  (**(code **)(*param_2 + 8))(param_2,"nimby::model::HeapBox<nimby::model::Tags>");
  if (0xcb < (int)param_2[1]) {
    FUN_1402f2900(puVar1,param_2);
  }
  (**(code **)(*param_2 + 0x10))(param_2);
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
  switch(*(undefined1 *)(param_1 + 1)) {
  case 0xe:
switchD_14032d6b9_caseD_e:
    FUN_14033ba80(param_1);
    break;
  case 0xf:
switchD_14032d6b9_caseD_f:
    FUN_14033ba80(param_1);
    break;
  case 0x10:
switchD_14032d6b9_caseD_10:
    FUN_14033ba80(param_1);
    break;
  case 0x11:
switchD_14032d6b9_caseD_11:
    FUN_14033ba80(param_1);
    break;
  default:
    FUN_14033f940(param_1,&local_res20);
    goto LAB_14032d6fc;
  }
switchD_14032d634_caseD_12:
  *(undefined1 *)(param_1 + 1) = 0xff;
  FUN_14033cf80(param_1,&local_res20);
LAB_14032d6fc:
  FUN_14033ba80(&local_res20);
LAB_14032d705:
  FUN_140335540(param_1,param_2,&local_res10,local_res18);
  FUN_140335710(param_1,param_2,&local_res10,local_res18);
  FUN_1403358e0(param_1,param_2,&local_res10,local_res18);
  FUN_140335ab0(param_1,param_2,&local_res10,local_res18);
  (**(code **)(*param_2 + 0x10))(param_2);
  return;
}


// Incoming references
// 0xc19814 DATA caller none
// 0x30f986 UNCONDITIONAL_CALL caller 14030f8b0
