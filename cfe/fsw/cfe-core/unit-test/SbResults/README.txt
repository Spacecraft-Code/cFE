The Software Bus unit test results are expected to have the
following results for each of the SB services source files:

ut_cfe_sb PASSED 146 tests.
ut_cfe_sb FAILED 0 tests.

gcov: '/mnt/hgfs/Projects/Repo_cFE/fsw/cfe-core/src/sb/ccsds.c' 100.00%  21
gcov: '/mnt/hgfs/Projects/Repo_cFE/fsw/cfe-core/src/sb/cfe_sb_task.c' 99.46%  370
gcov: '/mnt/hgfs/Projects/Repo_cFE/fsw/cfe-core/src/sb/cfe_sb_init.c' 100.00%  45
gcov: '/mnt/hgfs/Projects/Repo_cFE/fsw/cfe-core/src/sb/cfe_sb_api.c' 98.72%  549
gcov: '/mnt/hgfs/Projects/Repo_cFE/fsw/cfe-core/src/sb/cfe_sb_buf.c' 100.00%  51
gcov: '/mnt/hgfs/Projects/Repo_cFE/fsw/cfe-core/src/sb/cfe_sb_priv.c' 98.58%  141
gcov: '/mnt/hgfs/Projects/Repo_cFE/fsw/cfe-core/src/sb/cfe_sb_util.c' 100.00%  86


The Software Bus unit test results are expected to have the
following results for each of the SB source files:

==========================================================================

cfe_sb_task.c - 99.44% coverage (2 Lines of Code NOT executed)

The first line of code that is not executed in Unit Testing
is in the CFE_SB_TaskMain function.  The code snippet is
as follows:

    -:  137:    /* Main loop */
    5:  138:    while (Status == CFE_SUCCESS)
    -:  139:    {
    -:  140:        /* Increment the Main task Execution Counter */
    1:  141:        CFE_ES_IncrementTaskCounter();
    -:  142:
    1:  143:        CFE_ES_PerfLogExit(CFE_SB_MAIN_PERF_ID);
    -:  144:
    -:  145:        /* Pend on receipt of packet */
    1:  146:        Status = CFE_SB_RcvMsg(&CFE_SB.CmdPipePktPtr,
    1:  147:                                CFE_SB.CmdPipe,
    -:  148:                                CFE_SB_PEND_FOREVER);
    -:  149:
    1:  150:        CFE_ES_PerfLogEntry(CFE_SB_MAIN_PERF_ID);
    -:  151:
    1:  152:        if(Status == CFE_SUCCESS)
    -:  153:        {
    -:  154:            /* Process cmd pipe msg */
#####:  155:            CFE_SB_ProcessCmdPipePkt();
    -:  156:        }else{
    1:  157:            CFE_ES_WriteToSysLog("SB:Error reading cmd pipe,RC=0x%08X\n",Status);
    -:  158:        }/* end if */
    -:  159:
    -:  160:    }/* end while */
    -:  161:
    -:  162:    /* while loop exits only if CFE_SB_RcvMsg returns error */
    2:  163:    CFE_ES_ExitApp(CFE_ES_CORE_APP_RUNTIME_ERROR);
    -:  164:
    2:  165:}/* end CFE_SB_TaskMain */

Have not found a way to execute this line while allowing the unit test
terminate normally.


The next line of code cannot be executed without altering EVS or SB cfg parameters.


       12:  243:    if(CFE_SB_FILTERED_EVENT8 != 0){
        -:  244:      CFE_SB.EventFilters[CfgFileEventsToFilter].EventID = CFE_SB_FILTERED_EVENT8;
        -:  245:      CFE_SB.EventFilters[CfgFileEventsToFilter].Mask    = CFE_SB_FILTER_MASK8;
        -:  246:      CfgFileEventsToFilter++;
        -:  247:    }/* end if */
        -:  248:
        -:  249:    /* Be sure the number of events to register for filtering
        -:  250:    ** does not exceed CFE_EVS_MAX_EVENT_FILTERS */
       12:  251:    if(CFE_EVS_MAX_EVENT_FILTERS < CfgFileEventsToFilter){
    #####:  252:      CfgFileEventsToFilter = CFE_EVS_MAX_EVENT_FILTERS;
        -:  253:    }/* end if */
        -:  254:

==========================================================================

cfe_sb_api.c - 98.72% coverage -

A DCR was entered to get full coverage of this file in the next release.

       27: 1117:int32  CFE_SB_SendMsgFull(CFE_SB_Msg_t    *MsgPtr,
        -: 1118:                          uint32           TlmCntIncrements,
        -: 1119:                          uint32           CopyMode)
        -: 1120:{
        -: 1121:    CFE_SB_MsgId_t          MsgId;
        -: 1122:    int32                   Status;
       27: 1123:    CFE_SB_DestinationD_t   *DestPtr = NULL;
        -: 1124:    CFE_SB_PipeD_t          *PipeDscPtr;
        -: 1125:    CFE_SB_RouteEntry_t     *RtgTblPtr;
        -: 1126:    CFE_SB_BufferD_t        *BufDscPtr;
        -: 1127:    uint16                  TotalMsgSize;
        -: 1128:    uint16                  RtgTblIdx;
       27: 1129:    uint32                  TskId = 0;
        -: 1130:    uint16                  i;
        -: 1131:    char                    FullName[(OS_MAX_API_NAME * 2)];
        -: 1132:    CFE_SB_EventBuf_t       SBSndErr;
        -: 1133:
       27: 1134:    SBSndErr.EvtsToSnd = 0;
        -: 1135:
        -: 1136:    /* get task id for events and Sender Info*/
       27: 1137:    TskId = OS_TaskGetId();
        -: 1138:
        -: 1139:    /* check input parameter */
       27: 1140:    if(MsgPtr == NULL){
        1: 1141:        CFE_SB_LockSharedData(__func__,__LINE__);
        1: 1142:        CFE_SB.HKTlmMsg.MsgSendErrCnt++;
        1: 1143:        CFE_SB_UnlockSharedData(__func__,__LINE__);
        1: 1144:        CFE_EVS_SendEventWithAppID(CFE_SB_SEND_BAD_ARG_EID,CFE_EVS_ERROR,CFE_SB.AppId,
        -: 1145:            "Send Err:Bad input argument,Arg 0x%x,App %s",
        -: 1146:            (uint32)MsgPtr,CFE_SB_GetAppTskName(TskId,FullName));
        1: 1147:        return CFE_SB_BAD_ARGUMENT;
        -: 1148:    }/* end if */
        -: 1149:
       26: 1150:    MsgId = CFE_SB_GetMsgId(MsgPtr);
        -: 1151:
        -: 1152:    /* validate the msgid in the message */
       26: 1153:    if(CFE_SB_ValidateMsgId(MsgId) != CFE_SUCCESS){
        1: 1154:        CFE_SB_LockSharedData(__func__,__LINE__);
        1: 1155:        CFE_SB.HKTlmMsg.MsgSendErrCnt++;
        1: 1156:        if (CopyMode == CFE_SB_SEND_ZEROCOPY)
        -: 1157:        {
    #####: 1158:            BufDscPtr = CFE_SB_GetBufferFromCaller(MsgId, MsgPtr);
    #####: 1159:            CFE_SB_DecrBufUseCnt(BufDscPtr);
        -: 1160:        }
        1: 1161:        CFE_SB_UnlockSharedData(__func__,__LINE__);
        1: 1162:        CFE_EVS_SendEventWithAppID(CFE_SB_SEND_INV_MSGID_EID,CFE_EVS_ERROR,CFE_SB.AppId,
        -: 1163:            "Send Err:Invalid MsgId(0x%x)in msg,App %s",
        -: 1164:            MsgId,CFE_SB_GetAppTskName(TskId,FullName));
        1: 1165:        return CFE_SB_BAD_ARGUMENT;
        -: 1166:    }/* end if */
        -: 1167:
       25: 1168:    TotalMsgSize = CFE_SB_GetTotalMsgLength(MsgPtr);
        -: 1169:
        -: 1170:    /* Verify the size of the pkt is < or = the mission defined max */
       25: 1171:    if(TotalMsgSize > CFE_SB_MAX_SB_MSG_SIZE){
        1: 1172:        CFE_SB_LockSharedData(__func__,__LINE__);
        1: 1173:        CFE_SB.HKTlmMsg.MsgSendErrCnt++;
        1: 1174:        if (CopyMode == CFE_SB_SEND_ZEROCOPY)
        -: 1175:        {
    #####: 1176:            BufDscPtr = CFE_SB_GetBufferFromCaller(MsgId, MsgPtr);
    #####: 1177:            CFE_SB_DecrBufUseCnt(BufDscPtr);
        -: 1178:        }
        1: 1179:        CFE_SB_UnlockSharedData(__func__,__LINE__);
        1: 1180:        CFE_EVS_SendEventWithAppID(CFE_SB_MSG_TOO_BIG_EID,CFE_EVS_ERROR,CFE_SB.AppId,
        -: 1181:            "Send Err:Msg Too Big MsgId=0x%x,app=%s,size=%d,MaxSz=%d",
        -: 1182:            MsgId,CFE_SB_GetAppTskName(TskId,FullName),TotalMsgSize,CFE_SB_MAX_SB_MSG_SIZE);
        1: 1183:        return CFE_SB_MSG_TOO_BIG;
        -: 1184:    }/* end if */
        -: 1185:
        -: 1186:    /* take semaphore to prevent a task switch during this call */
       24: 1187:    CFE_SB_LockSharedData(__func__,__LINE__);
        -: 1188:
       24: 1189:    RtgTblIdx = CFE_SB_GetRoutingTblIdx(MsgId);
        -: 1190:
        -: 1191:    /* if there have been no subscriptions for this pkt, */
        -: 1192:    /* increment the dropped pkt cnt, send event and return success */
       24: 1193:    if(RtgTblIdx == CFE_SB_AVAILABLE){
        -: 1194:
        9: 1195:        CFE_SB.HKTlmMsg.NoSubscribersCnt++;
        -: 1196:
        9: 1197:        if (CopyMode == CFE_SB_SEND_ZEROCOPY){
    #####: 1198:            BufDscPtr = CFE_SB_GetBufferFromCaller(MsgId, MsgPtr);
    #####: 1199:            CFE_SB_DecrBufUseCnt(BufDscPtr);
        -: 1200:        }
        -: 1201:
        9: 1202:        CFE_SB_UnlockSharedData(__func__,__LINE__);
        -: 1203:
        -: 1204:        /* Determine if event can be sent without causing recursive event problem */
        9: 1205:        if(CFE_SB_RequestToSendEvent(TskId,CFE_SB_SEND_NO_SUBS_EID_BIT) == CFE_SB_GRANTED){
        -: 1206:
        9: 1207:           CFE_EVS_SendEventWithAppID(CFE_SB_SEND_NO_SUBS_EID,CFE_EVS_INFORMATION,CFE_SB.AppId,
        -: 1208:              "No subscribers for MsgId 0x%x,sender %s",
        -: 1209:              MsgId,CFE_SB_GetAppTskName(TskId,FullName));
        -: 1210:
        -: 1211:           /* clear the bit so the task may send this event again */
        9: 1212:           CFE_CLR(CFE_SB.StopRecurseFlags[TskId],CFE_SB_SEND_NO_SUBS_EID_BIT);
        -: 1213:        }/* end if */
        -: 1214:
        9: 1215:        return CFE_SUCCESS;
        -: 1216:    }/* end if */
        -: 1217:
        -: 1218:    /* Allocate a new buffer. */
       15: 1219:    if (CopyMode == CFE_SB_SEND_ZEROCOPY){
        3: 1220:        BufDscPtr = CFE_SB_GetBufferFromCaller(MsgId, MsgPtr);
        -: 1221:    }
        -: 1222:    else{
       12: 1223:        BufDscPtr = CFE_SB_GetBufferFromPool(MsgId, TotalMsgSize);
        -: 1224:    }
       15: 1225:    if (BufDscPtr == NULL){
        1: 1226:        CFE_SB.HKTlmMsg.MsgSendErrCnt++;
        1: 1227:        CFE_SB_UnlockSharedData(__func__,__LINE__);
        -: 1228:
        -: 1229:        /* Determine if event can be sent without causing recursive event problem */
        1: 1230:        if(CFE_SB_RequestToSendEvent(TskId,CFE_SB_GET_BUF_ERR_EID_BIT) == CFE_SB_GRANTED){
        -: 1231:
        1: 1232:            CFE_EVS_SendEventWithAppID(CFE_SB_GET_BUF_ERR_EID,CFE_EVS_ERROR,CFE_SB.AppId,
        -: 1233:              "Send Err:Request for Buffer Failed. MsgId 0x%x,app %s,size %d",
        -: 1234:              MsgId,CFE_SB_GetAppTskName(TskId,FullName),TotalMsgSize);
        -: 1235:
        -: 1236:            /* clear the bit so the task may send this event again */
        1: 1237:            CFE_CLR(CFE_SB.StopRecurseFlags[TskId],CFE_SB_GET_BUF_ERR_EID_BIT);
        -: 1238:        }/* end if */
        -: 1239:
        1: 1240:        return CFE_SB_BUF_ALOC_ERR;
        -: 1241:    }/* end if */
        -: 1242:
        -: 1243:    /* Copy the packet into the SB memory space */
       14: 1244:    if (CopyMode != CFE_SB_SEND_ZEROCOPY){
        -: 1245:        /* Copy the packet into the SB memory space */
       11: 1246:        CFE_PSP_MemCpy( BufDscPtr->Buffer, MsgPtr, (uint16)TotalMsgSize );
        -: 1247:    }
        -: 1248:
        -: 1249:    /* For Tlm packets, increment the seq count if requested */
       14: 1250:    if((CFE_SB_GetPktType(MsgId)==CFE_SB_TLM) &&
        -: 1251:       (TlmCntIncrements==CFE_SB_INCREMENT_TLM)){
       11: 1252:        CFE_SB.RoutingTbl[RtgTblIdx].SeqCnt++;
       11: 1253:        CFE_SB_SetMsgSeqCnt((CFE_SB_Msg_t *)BufDscPtr->Buffer,
        -: 1254:                              CFE_SB.RoutingTbl[RtgTblIdx].SeqCnt);
        -: 1255:    }/* end if */
        -: 1256:
        -: 1257:    /* store the sender information */
       14: 1258:    if(CFE_SB.SenderReporting != 0)
        -: 1259:    {
       14: 1260:       BufDscPtr->Sender.ProcessorId = CFE_PSP_GetProcessorId();
       14: 1261:       strncpy(&BufDscPtr->Sender.AppName[0],CFE_SB_GetAppTskName(TskId,FullName),OS_MAX_API_NAME);
        -: 1262:    }
        -: 1263:
       14: 1264:    RtgTblPtr = &CFE_SB.RoutingTbl[RtgTblIdx];
        -: 1265:
        -: 1266:    /* At this point there must be at least one destination for pkt */
        -: 1267:
       14: 1268:    DestPtr = RtgTblPtr -> ListHeadPtr;
        -: 1269:
        -: 1270:    /* Send the packet to all destinations  */
       28: 1271:    for (i=0; i < RtgTblPtr -> Destinations; i++) {
        -: 1272:
        -: 1273:        /* The DestPtr should never be NULL in this loop, this is just extra
        -: 1274:           protection in case of the unforseen */
       14: 1275:        if(DestPtr == NULL){
    #####: 1276:          break;
        -: 1277:        }
        -: 1278:
       14: 1279:        if (DestPtr->Active == CFE_SB_INACTIVE)    /* destination is inactive */
        -: 1280:        {
        1: 1281:            DestPtr = DestPtr -> Next;
        1: 1282:            continue;
        -: 1283:        }
        -: 1284:

==========================================================================

cfe_sb_buf.c - 100% coverage

==========================================================================

cfe_sb_init.c - 100% coverage

==========================================================================

cfe_sb_priv.c - 98.58% coverage

     1215:  606:char *CFE_SB_GetAppTskName(uint32 TaskId,char *FullName){
        -:  607:
        -:  608:    CFE_ES_TaskInfo_t  TaskInfo;
     1215:  609:    CFE_ES_TaskInfo_t  *ptr = &TaskInfo;
        -:  610:    char               AppName[OS_MAX_API_NAME];
        -:  611:    char               TskName[OS_MAX_API_NAME];
        -:  612:
     1215:  613:    if(CFE_ES_GetTaskInfo(ptr, TaskId) != CFE_SUCCESS){
        -:  614:
        -:  615:      /* unlikely, but possible if TaskId is bogus */
    #####:  616:      strncpy(FullName,"Unknown",OS_MAX_API_NAME-1);
    #####:  617:      FullName[OS_MAX_API_NAME-1] = '\0';
        -:  618:
     1215:  619:    }else if(strncmp((char *)ptr->AppName,(char *)ptr->TaskName,OS_MAX_API_NAME-1) == 0){
        -:  620:

==========================================================================

cfe_sb_util.c - 100% coverage

==========================================================================

