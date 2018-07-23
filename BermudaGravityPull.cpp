/******************************************************************************************************************************************************************
********************DIFFICULTY FORMULA - WRITTEN BY BRIAN OTIENO **************************************************************************************************
******************************************************************************************************************************************************************/
unsigned int static BermudaGravityPull(const CBlockIndex* pindexLast, const Consensus::Params& params) {
    const CBlockIndex *BlockLastSolved = pindexLast;
    const CBlockIndex *BlockReading = pindexLast;
    uint64_t PastBlocksMass = 0;
    int64_t PastRateActualSeconds = 0;
    int64_t PastRateTargetSeconds = 0;
    double PastRateAdjustmentRatio = double(0.99);
    int64_t MaxTime;
    arith_uint256 bnNew;
  
    uint64_t PastBlocksMin = (uint64_t (params.nPowTargetTimespan * 25)) / params.nPowTargetSpacing; 
    uint64_t PastBlocksMax = (uint64_t (params.nPowTargetTimespan * 70)) / params.nPowTargetSpacing; 

    if (BlockLastSolved == NULL || BlockLastSolved->nHeight == 0 || (uint64_t)BlockLastSolved->nHeight < PastBlocksMin) { return UintToArith256(params.powLimit).GetCompact(); }
		
		pindexInitial = pindexLast->pprev;
		pindexRecent = pindexLast;
		
		/* previous_max=timestamp[height - N] - where N is most recent block */
		PastRateActualSeconds = pindexRecent->GetBlockTime() - pindexInitial->GetBlockTime(); 
		
		/* In (i = height-N+1; i < height+1; i++), applying timestamp gives this; timestamp(height -N)+1 which is just (pindexInitial - pindexRecent)+1 */
    for (unsigned int i = pindexRecent - pindexInitial + 1; i < pindexRecent; i++) { 
        if (PastBlocksMax > 0 && i > PastBlocksMax) { assert("Check if value is within the WWHM limit", (PastBlocksMax > 0 && i > PastBlocksMax)); break; }
        	MaxTime = max(pindexInitial[i]->GetBlockTime(), PastRateActualSeconds); /*max_timestamp=max(timestamp[i], previous_max)*/
		/* PastRateTargetSeconds = max_timestamp - previous_max*/
		PastRateTargetSeconds = MaxTime - PastRateActualSeconds; 
		/* PastRateTargetSeconds=1 if PastRateTargetSeconds < 1 and PastRateTargetSeconds = 10*T if PastRateTargetSeconds > 10*T */
		PastRateTargetSeconds = (PastRateTargetSeconds <1)? 1 : (PastRateTargetSeconds > (10*PastRateTargetSeconds))? (10*PastRateTargetSeconds): PastRateTargetSeconds;
		/* Re-assigning PastRateActualSeconds to new and thus next PastRateActualSeconds for the next iteration: previous_max=max_timestamp */
		PastRateActualSeconds = MaxTime;
		PastBlocksMass++; 		 
    } //Iteration for the next i;
	
/* Enough restrictions, no limits need to be set. Straight to retargeting. */

   bnNew.SetCompact(pindexLast->nBits);
   
    /*next_D = previous_D*limit if next_D > previous_D*limit */ 	
	if (PastRateActualSeconds > (PastRateTargetSeconds * UintToArith256(params.powLimit))) {
        bnNew *= PastRateActualSeconds; 
    }
	/*next_D = previous_D/limit if next_D > previous_D/limit*/
	if (PastRateActualSeconds > (PastRateTargetSeconds * UintToArith256(params.powLimit))) {
        bnNew /= PastRateTargetSeconds;
    }	  
    return bnNew.GetCompact();
}