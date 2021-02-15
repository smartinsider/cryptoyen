// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2019 The PIVX developers
// Copyright (c) 2019 SmartIsider dev
// Copyright (c) 2020 The CryptoYen developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

// clang-format off
#include "main.h"
#include "activemasternode.h"
#include "masternode-sync.h"
#include "masternode-payments.h"
#include "masternode.h"
#include "masternodeman.h"
#include "spork.h"
#include "util.h"
#include "addrman.h"
// clang-format on

class CMasternodeSync;
CMasternodeSync masternodeSync;

CMasternodeSync::CMasternodeSync()
{
    Reset();
}

bool CMasternodeSync::IsSynced()
{
    return RequestedMasternodeAssets == MASTERNODE_SYNC_FINISHED;
}

bool CMasternodeSync::IsBlockchainSynced()
{
    static bool fBlockchainSynced = false;
    static int64_t lastProcess = GetTime();

    // TOFIXED
    static int64_t SleapModeTime = 6; // 1 is optimal
    
    //  if the last call to this function was more than 1h ago (client was in sleep mode) reset the sync process
	if (GetTime() - lastProcess > 60 * 60 * SleapModeTime) {
        Reset();
        fBlockchainSynced = false;
    }
    lastProcess = GetTime();

    if (fBlockchainSynced) return true;

    if (fImporting || fReindex) return false;

    TRY_LOCK(cs_main, lockMain);
    if (!lockMain) return false;

    CBlockIndex* pindex = chainActive.Tip();
    if (pindex == NULL) return false;

    if (pindex->nTime + 60 * 60 * SleapModeTime < GetTime())
        return false;

    fBlockchainSynced = true;

    return true;
}

void CMasternodeSync::Reset()
{
    lastMasternodeList = 0;
    lastMasternodeWinner = 0;
    mapSeenSyncMNB.clear();
    mapSeenSyncMNW.clear();
    lastFailure = 0;
    nCountFailures = 0;
    sumMasternodeList = 0;
    sumMasternodeWinner = 0;
    countMasternodeList = 0;
    countMasternodeWinner = 0;
    RequestedMasternodeAssets = MASTERNODE_SYNC_INITIAL;
    RequestedMasternodeAttempt = 0;
    nAssetSyncStarted = GetTime();
}

void CMasternodeSync::AddedMasternodeList(uint256 hash)
{
    if (mnodeman.mapSeenMasternodeBroadcast.count(hash)) {
        if (mapSeenSyncMNB[hash] < MASTERNODE_SYNC_THRESHOLD) {
            lastMasternodeList = GetTime();
            mapSeenSyncMNB[hash]++;
        }
    } else {
        lastMasternodeList = GetTime();
        mapSeenSyncMNB.insert(make_pair(hash, 1));
    }
}

void CMasternodeSync::AddedMasternodeWinner(uint256 hash)
{
    if (masternodePayments.mapMasternodePayeeVotes.count(hash)) {
        if (mapSeenSyncMNW[hash] < MASTERNODE_SYNC_THRESHOLD) {
            lastMasternodeWinner = GetTime();
            mapSeenSyncMNW[hash]++;
        }
    } else {
        lastMasternodeWinner = GetTime();
        mapSeenSyncMNW.insert(make_pair(hash, 1));
    }
    
}

void CMasternodeSync::GetNextAsset()
{

    switch (RequestedMasternodeAssets) {
    case (MASTERNODE_SYNC_INITIAL):
    case (MASTERNODE_SYNC_FAILED): // should never be used here actually, use Reset() instead
        ClearFulfilledRequest();
        RequestedMasternodeAssets = MASTERNODE_SYNC_LIST;
        break;
    case (MASTERNODE_SYNC_LIST):
        RequestedMasternodeAssets = MASTERNODE_SYNC_MNW;
        break;
    case (MASTERNODE_SYNC_MNW):        
        LogPrintf("CMasternodeSync::GetNextAsset - Sync has finished\n");
        RequestedMasternodeAssets = MASTERNODE_SYNC_FINISHED;
        break;
    }
    RequestedMasternodeAttempt = 0;
    nAssetSyncStarted = GetTime();

}

std::string CMasternodeSync::GetSyncStatus()
{
    switch (masternodeSync.RequestedMasternodeAssets) {
    case MASTERNODE_SYNC_INITIAL:
        return _("Synchronization pending...");
    case MASTERNODE_SYNC_LIST:
        return _("Synchronizing masternodes...");
    case MASTERNODE_SYNC_MNW:
        return _("Synchronizing masternode winners...");
    case MASTERNODE_SYNC_FAILED:
        return _("Synchronization failed");
    case MASTERNODE_SYNC_FINISHED:
        return _("Synchronization finished");
    }
    return "";
}

void CMasternodeSync::ProcessMessage(CNode* pfrom, std::string& strCommand, CDataStream& vRecv)
{
    if (strCommand == "ssc") { //Sync status count
        int nItemID;
        int nCount;
        vRecv >> nItemID >> nCount;

        if (RequestedMasternodeAssets >= MASTERNODE_SYNC_FINISHED) return;

        //this means we will receive no further communication
        switch (nItemID) {
        case (MASTERNODE_SYNC_LIST):
            if (nItemID != RequestedMasternodeAssets) return;
            sumMasternodeList += nCount;
            countMasternodeList++;
            break;
        case (MASTERNODE_SYNC_MNW):
            if (nItemID != RequestedMasternodeAssets) return;
            sumMasternodeWinner += nCount;
            countMasternodeWinner++;
            break;				
        }

        LogPrint("masternode", "CMasternodeSync:ProcessMessage - ssc - got inventory count %d %d\n", nItemID, nCount);
    }
}

void CMasternodeSync::ClearFulfilledRequest()
{
    TRY_LOCK(cs_vNodes, lockRecv);
    if (!lockRecv) return;

    BOOST_FOREACH (CNode* pnode, vNodes) {
        pnode->ClearFulfilledRequest("mnsync");
        pnode->ClearFulfilledRequest("mnwsync");
        pnode->ClearFulfilledRequest("busync");
    }
}

void CMasternodeSync::Process()
{
    static int tick = 0;
    bool syncLog = true;

    if (tick++ % MASTERNODE_SYNC_TIMEOUT != 0) return;

    if (IsSynced()) {
        /* 
            Resync if we lose all masternodes from sleep/wake or failure to sync originally
        */
		
		if (mnodeman.CountEnabled() == 0) {
            Reset();
        } else
            return;
    }

    //try syncing again
    if (RequestedMasternodeAssets == MASTERNODE_SYNC_FAILED && lastFailure + (1 * 60) < GetTime()) {
        Reset();
    } else if (RequestedMasternodeAssets == MASTERNODE_SYNC_FAILED) {
        return;
    }

    LogPrint("masternode", "CMasternodeSync::Process() - tick %d RequestedMasternodeAssets %d\n", tick, RequestedMasternodeAssets);

    if (RequestedMasternodeAssets == MASTERNODE_SYNC_INITIAL) GetNextAsset();
	
    TRY_LOCK(cs_vNodes, lockRecv);
    if (!lockRecv) return;

    BOOST_FOREACH (CNode* pnode, vNodes) {

        if (Params().NetworkID() == CBaseChainParams::REGTEST) {

		    if (RequestedMasternodeAttempt > 0) {
            	mnodeman.DsegUpdate(pnode);
            	int nMnCount = mnodeman.CountEnabled();
            	pnode->PushMessage("mnget", nMnCount); //sync payees
            } else {
                RequestedMasternodeAssets = MASTERNODE_SYNC_FINISHED;
            }
            RequestedMasternodeAttempt++;
            return;
        }
		


        // TOFIXED
        if (false) {
            GetNextAsset();
            pnode->FulfilledRequest("busync");
            return;
        } else {

            if (pnode->nVersion >= masternodePayments.GetMinMasternodePaymentsProto()) { 

                if (RequestedMasternodeAssets == MASTERNODE_SYNC_LIST) {
                    LogPrint("masternode", "CMasternodeSync::Process() - lastMasternodeList %lld (GetTime() - MASTERNODE_SYNC_TIMEOUT) %lld\n", lastMasternodeList, GetTime() - MASTERNODE_SYNC_TIMEOUT);
                    if (lastMasternodeList > 0 && lastMasternodeList < GetTime() - MASTERNODE_SYNC_TIMEOUT * 2 && RequestedMasternodeAttempt >= MASTERNODE_SYNC_THRESHOLD) { //hasn't received a new item in the last five seconds, so we'll move to the
                        GetNextAsset();
                        return;
                    }

                    //if (pnode->HasFulfilledRequest("mnsync")) continue;
    				pnode->HasFulfilledRequest("mnsync");
                    pnode->FulfilledRequest("mnsync");

                    
    				
    				if (syncLog) LogPrintf("TRY TO SYNC = STEP = 1\n");
    					

                    if (RequestedMasternodeAttempt >= MASTERNODE_SYNC_THRESHOLD * 3) return;
    				
    				if (syncLog) LogPrintf("TRY TO SYNC = STEP = 2\n");
    				
    				mnodeman.DsegUpdate(pnode);
                    //if(!mnodeman.DsegUpdate(pnode))
                    //    continue;
    				
    				if (syncLog) LogPrintf("TRY TO SYNC = STEP = 3\n");

                    ++RequestedMasternodeAttempt;

                    return;
                }
    			
    			if (syncLog) LogPrintf("TRY TO SYNC = STEP = 4\n");

                if (RequestedMasternodeAssets == MASTERNODE_SYNC_MNW) {
                    if (lastMasternodeWinner > 0 && lastMasternodeWinner < GetTime() - MASTERNODE_SYNC_TIMEOUT * 2 && RequestedMasternodeAttempt >= MASTERNODE_SYNC_THRESHOLD) { //hasn't received a new item in the last five seconds, so we'll move to the
                        GetNextAsset();
                        // Try to activate our masternode if possible
                        activeMasternode.ManageStatus();
                        return;
                    }

                    if (pnode->HasFulfilledRequest("mnwsync")) continue;
    				
                    pnode->FulfilledRequest("mnwsync");
    				if (syncLog) LogPrintf("TRY TO SYNC = STEP 5\n");

                    // timeout
                    if (lastMasternodeWinner == 0 &&
                        (RequestedMasternodeAttempt >= MASTERNODE_SYNC_THRESHOLD * 3 || GetTime() - nAssetSyncStarted > MASTERNODE_SYNC_TIMEOUT * 5)) {
                        
    					//SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT IS ACTIVE FOREVER					
    					if (syncLog) LogPrintf("TRY TO SYNC = STEP 6\n");
                        LogPrintf("CMasternodeSync::Process - ERROR - Sync has failed, will retry later\n");
                        RequestedMasternodeAssets = MASTERNODE_SYNC_FAILED;
                        RequestedMasternodeAttempt = 0;
                        lastFailure = GetTime();
                        nCountFailures++;
    				    return;
    				}
    				
    				if (syncLog) LogPrintf("TRY TO SYNC = STEP 7\n");
                    if (RequestedMasternodeAttempt >= MASTERNODE_SYNC_THRESHOLD) { 
                         GetNextAsset();
                         // Try to activate our masternode if possible
                         activeMasternode.ManageStatus();
                        return;
                    }

    				if (syncLog) LogPrintf("TRY TO SYNC = STEP 8\n");
                    if (RequestedMasternodeAttempt >= MASTERNODE_SYNC_THRESHOLD * 3){
    					GetNextAsset();
                     	activeMasternode.ManageStatus();
                        return;
    				}

    				
    				if (syncLog) LogPrintf("TRY TO SYNC = STEP 9\n");
                    mnodeman.WinnersUpdate(pnode);
                    
                    if (syncLog) LogPrintf("TRY TO SYNC = STEP 10\n");
                    if (pnode->HasFulfilledRequest("busync")) 
    					continue;
    									
                    pnode->FulfilledRequest("busync");
    				
    				if (syncLog) LogPrintf("TRY TO SYNC = STEP 11\n");
                    ++RequestedMasternodeAttempt;

                    return;
    				
                }
            }
            
        }


    }
}
