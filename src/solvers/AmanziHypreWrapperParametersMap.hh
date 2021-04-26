const std::map<std::string, int_func> FunctionParameter::hypreMapIntFunc_ = {
  {"HYPRE_GMRESSetStopCrit", &HYPRE_GMRESSetStopCrit},
  {"HYPRE_ParCSRGMRESSetPrintLevel", &HYPRE_ParCSRGMRESSetPrintLevel},
  {"HYPRE_BoomerAMGSetSmoothNumLevels", &HYPRE_BoomerAMGSetSmoothNumLevels},
  {"HYPRE_BiCGSTABSetMinIter", &HYPRE_BiCGSTABSetMinIter},
  {"HYPRE_COGMRESSetPrintLevel", &HYPRE_COGMRESSetPrintLevel},
  {"HYPRE_BoomerAMGSetCGCIts", &HYPRE_BoomerAMGSetCGCIts},
  {"HYPRE_ParCSRBiCGSTABSetPrintLevel", &HYPRE_ParCSRBiCGSTABSetPrintLevel},
  {"HYPRE_BoomerAMGSetOverlap", &HYPRE_BoomerAMGSetOverlap},
  {"HYPRE_BiCGSTABSetLogging", &HYPRE_BiCGSTABSetLogging},
  {"HYPRE_ParCSRHybridSetTwoNorm", &HYPRE_ParCSRHybridSetTwoNorm},
  {"HYPRE_LGMRESSetAugDim", &HYPRE_LGMRESSetAugDim},
  {"HYPRE_BoomerAMGSetCoarsenCutFactor", &HYPRE_BoomerAMGSetCoarsenCutFactor},
  {"HYPRE_ParCSRHybridSetMaxCoarseSize", &HYPRE_ParCSRHybridSetMaxCoarseSize},
  {"HYPRE_MGRSetInterpType", &HYPRE_MGRSetInterpType},
  {"HYPRE_ParaSailsSetSym", &HYPRE_ParaSailsSetSym},
  {"HYPRE_ParCSRCOGMRESSetUnroll", &HYPRE_ParCSRCOGMRESSetUnroll},
  {"HYPRE_BoomerAMGSetPrintLevel", &HYPRE_BoomerAMGSetPrintLevel},
  {"HYPRE_BoomerAMGSetChebyOrder", &HYPRE_BoomerAMGSetChebyOrder},
  {"HYPRE_AMSSetMaxIter", &HYPRE_AMSSetMaxIter},
  {"HYPRE_BoomerAMGSetMaxIter", &HYPRE_BoomerAMGSetMaxIter},
  {"HYPRE_LGMRESSetLogging", &HYPRE_LGMRESSetLogging},
  {"HYPRE_BoomerAMGSetInterpVecVariant", &HYPRE_BoomerAMGSetInterpVecVariant},
  {"HYPRE_PCGSetPrintLevel", &HYPRE_PCGSetPrintLevel},
  {"HYPRE_BoomerAMGDDSetStartLevel", &HYPRE_BoomerAMGDDSetStartLevel},
  {"HYPRE_ParCSRGMRESSetStopCrit", &HYPRE_ParCSRGMRESSetStopCrit},
  {"HYPRE_ParCSRCGNRSetLogging", &HYPRE_ParCSRCGNRSetLogging},
  {"HYPRE_ParCSRHybridSetDSCGMaxIter", &HYPRE_ParCSRHybridSetDSCGMaxIter},
  {"HYPRE_BoomerAMGSetSmoothType", &HYPRE_BoomerAMGSetSmoothType},
  {"HYPRE_ILUSetPrintLevel", &HYPRE_ILUSetPrintLevel},
  {"HYPRE_BoomerAMGSetSym", &HYPRE_BoomerAMGSetSym},
  {"HYPRE_ParCSRHybridSetSolverType", &HYPRE_ParCSRHybridSetSolverType},
  {"HYPRE_BoomerAMGSetInterpVecQMax", &HYPRE_BoomerAMGSetInterpVecQMax},
  {"HYPRE_ParCSRGMRESSetMinIter", &HYPRE_ParCSRGMRESSetMinIter},
  {"HYPRE_AMSSetPrintLevel", &HYPRE_AMSSetPrintLevel},
  {"HYPRE_BoomerAMGSetSchwarzUseNonSymm", &HYPRE_BoomerAMGSetSchwarzUseNonSymm},
  {"HYPRE_BoomerAMGDDSetFACNumRelax", &HYPRE_BoomerAMGDDSetFACNumRelax},
  {"HYPRE_BoomerAMGSetChebyVariant", &HYPRE_BoomerAMGSetChebyVariant},
  {"HYPRE_MGRSetMaxIter", &HYPRE_MGRSetMaxIter},
  {"HYPRE_PCGSetLogging", &HYPRE_PCGSetLogging},
  {"HYPRE_ParCSRLGMRESSetMaxIter", &HYPRE_ParCSRLGMRESSetMaxIter},
  {"HYPRE_ParCSRHybridSetSetupType", &HYPRE_ParCSRHybridSetSetupType},
  {"HYPRE_BoomerAMGSetPostInterpType", &HYPRE_BoomerAMGSetPostInterpType},
  {"HYPRE_MGRSetGlobalsmoothType", &HYPRE_MGRSetGlobalsmoothType},
  {"HYPRE_ParCSRCOGMRESSetMaxIter", &HYPRE_ParCSRCOGMRESSetMaxIter},
  {"HYPRE_ParCSRCGNRSetStopCrit", &HYPRE_ParCSRCGNRSetStopCrit},
  {"HYPRE_BoomerAMGSetCycleType", &HYPRE_BoomerAMGSetCycleType},
  {"HYPRE_ParCSRHybridSetRelaxOrder", &HYPRE_ParCSRHybridSetRelaxOrder},
  {"HYPRE_ParCSRGMRESSetLogging", &HYPRE_ParCSRGMRESSetLogging},
  {"HYPRE_BoomerAMGSetRelaxOrder", &HYPRE_BoomerAMGSetRelaxOrder},
  {"HYPRE_GMRESSetRelChange", &HYPRE_GMRESSetRelChange},
  {"HYPRE_ParCSRHybridSetMeasureType", &HYPRE_ParCSRHybridSetMeasureType},
  {"HYPRE_FlexGMRESSetLogging", &HYPRE_FlexGMRESSetLogging},
  {"HYPRE_BoomerAMGSetCRUseCG", &HYPRE_BoomerAMGSetCRUseCG},
  {"HYPRE_BoomerAMGSetSeqThreshold", &HYPRE_BoomerAMGSetSeqThreshold},
  {"HYPRE_MGRSetNumRelaxSweeps", &HYPRE_MGRSetNumRelaxSweeps},
  {"HYPRE_BoomerAMGSetKeepTranspose", &HYPRE_BoomerAMGSetKeepTranspose},
  {"HYPRE_BoomerAMGSetMaxCoarseSize", &HYPRE_BoomerAMGSetMaxCoarseSize},
  {"HYPRE_EuclidSetMem", &HYPRE_EuclidSetMem},
  {"HYPRE_MGRSetNonCpointsToFpoints", &HYPRE_MGRSetNonCpointsToFpoints},
  {"HYPRE_ParCSRBiCGSTABSetStopCrit", &HYPRE_ParCSRBiCGSTABSetStopCrit},
  {"HYPRE_BoomerAMGSetMinCoarseSize", &HYPRE_BoomerAMGSetMinCoarseSize},
  {"HYPRE_BoomerAMGSetMaxNzPerRow", &HYPRE_BoomerAMGSetMaxNzPerRow},
  {"HYPRE_BoomerAMGSetChebyScale", &HYPRE_BoomerAMGSetChebyScale},
  {"HYPRE_BoomerAMGSetVariant", &HYPRE_BoomerAMGSetVariant},
  {"HYPRE_ParCSRParaSailsSetReuse", &HYPRE_ParCSRParaSailsSetReuse},
  {"HYPRE_ParCSRCOGMRESSetMinIter", &HYPRE_ParCSRCOGMRESSetMinIter},
  {"HYPRE_ILUSetMaxIter", &HYPRE_ILUSetMaxIter},
  {"HYPRE_ILUSetLogging", &HYPRE_ILUSetLogging},
  {"HYPRE_ParCSRHybridSetKeepTranspose", &HYPRE_ParCSRHybridSetKeepTranspose},
  {"HYPRE_BoomerAMGSetAddLastLvl", &HYPRE_BoomerAMGSetAddLastLvl},
  {"HYPRE_ParCSRFlexGMRESSetKDim", &HYPRE_ParCSRFlexGMRESSetKDim},
  {"HYPRE_EuclidSetRowScale", &HYPRE_EuclidSetRowScale},
  {"HYPRE_ADSSetPrintLevel", &HYPRE_ADSSetPrintLevel},
  {"HYPRE_SchwarzSetVariant", &HYPRE_SchwarzSetVariant},
  {"HYPRE_BoomerAMGSetFCycle", &HYPRE_BoomerAMGSetFCycle},
  {"HYPRE_BoomerAMGSetISType", &HYPRE_BoomerAMGSetISType},
  {"HYPRE_ParCSRHybridSetRecomputeResidualP", &HYPRE_ParCSRHybridSetRecomputeResidualP},
  {"HYPRE_BoomerAMGSetMultAdditive", &HYPRE_BoomerAMGSetMultAdditive},
  {"HYPRE_GMRESSetSkipRealResidualCheck", &HYPRE_GMRESSetSkipRealResidualCheck},
  {"HYPRE_COGMRESSetKDim", &HYPRE_COGMRESSetKDim},
  {"HYPRE_BoomerAMGSetEuLevel", &HYPRE_BoomerAMGSetEuLevel},
  {"HYPRE_BoomerAMGSetMultAddPMaxElmts", &HYPRE_BoomerAMGSetMultAddPMaxElmts},
  {"HYPRE_FlexGMRESSetPrintLevel", &HYPRE_FlexGMRESSetPrintLevel},
  {"HYPRE_ParCSRPCGSetStopCrit", &HYPRE_ParCSRPCGSetStopCrit},
  {"HYPRE_BoomerAMGSetIsTriangular", &HYPRE_BoomerAMGSetIsTriangular},
  {"HYPRE_ParCSRCGNRSetMaxIter", &HYPRE_ParCSRCGNRSetMaxIter},
  {"HYPRE_BoomerAMGSetADropType", &HYPRE_BoomerAMGSetADropType},
  {"HYPRE_MGRSetRelaxType", &HYPRE_MGRSetRelaxType},
  {"HYPRE_ILUSetLevelOfFill", &HYPRE_ILUSetLevelOfFill},
  {"HYPRE_MGRSetPrintLevel", &HYPRE_MGRSetPrintLevel},
  {"HYPRE_BoomerAMGDDSetFACCycleType", &HYPRE_BoomerAMGDDSetFACCycleType},
  {"HYPRE_AMSSetCycleType", &HYPRE_AMSSetCycleType},
  {"HYPRE_ILUSetLocalReordering", &HYPRE_ILUSetLocalReordering},
  {"HYPRE_ParCSRPilutSetMaxIter", &HYPRE_ParCSRPilutSetMaxIter},
  {"HYPRE_PCGSetMaxIter", &HYPRE_PCGSetMaxIter},
  {"HYPRE_ParCSRPCGSetTwoNorm", &HYPRE_ParCSRPCGSetTwoNorm},
  {"HYPRE_BoomerAMGSetRedundant", &HYPRE_BoomerAMGSetRedundant},
  {"HYPRE_FlexGMRESSetKDim", &HYPRE_FlexGMRESSetKDim},
  {"HYPRE_BoomerAMGSetSmoothNumSweeps", &HYPRE_BoomerAMGSetSmoothNumSweeps},
  {"HYPRE_ParCSRPCGSetRelChange", &HYPRE_ParCSRPCGSetRelChange},
  {"HYPRE_BoomerAMGSetGMRESSwitchR", &HYPRE_BoomerAMGSetGMRESSwitchR},
  {"HYPRE_LGMRESSetPrintLevel", &HYPRE_LGMRESSetPrintLevel},
  {"HYPRE_ParCSRFlexGMRESSetLogging", &HYPRE_ParCSRFlexGMRESSetLogging},
  {"HYPRE_BoomerAMGSetAggPMaxElmts", &HYPRE_BoomerAMGSetAggPMaxElmts},
  {"HYPRE_ParCSRHybridSetCoarsenType", &HYPRE_ParCSRHybridSetCoarsenType},
  {"HYPRE_ParCSRCGNRSetMinIter", &HYPRE_ParCSRCGNRSetMinIter},
  {"HYPRE_ParCSRPCGSetPrintLevel", &HYPRE_ParCSRPCGSetPrintLevel},
  {"HYPRE_ParCSRPCGSetLogging", &HYPRE_ParCSRPCGSetLogging},
  {"HYPRE_ParCSRGMRESSetMaxIter", &HYPRE_ParCSRGMRESSetMaxIter},
  {"HYPRE_ParCSRHybridSetMaxLevels", &HYPRE_ParCSRHybridSetMaxLevels},
  {"HYPRE_AMSSetBetaAMGCoarseRelaxType", &HYPRE_AMSSetBetaAMGCoarseRelaxType},
  {"HYPRE_BoomerAMGSetRestriction", &HYPRE_BoomerAMGSetRestriction},
  {"HYPRE_ILUSetMaxNnzPerRow", &HYPRE_ILUSetMaxNnzPerRow},
  {"HYPRE_ParCSRParaSailsSetSym", &HYPRE_ParCSRParaSailsSetSym},
  {"HYPRE_PCGSetRecomputeResidualP", &HYPRE_PCGSetRecomputeResidualP},
  {"HYPRE_BoomerAMGSetAddRelaxType", &HYPRE_BoomerAMGSetAddRelaxType},
  {"HYPRE_ParCSRParaSailsSetLogging", &HYPRE_ParCSRParaSailsSetLogging},
  {"HYPRE_CGNRSetStopCrit", &HYPRE_CGNRSetStopCrit},
  {"HYPRE_CGNRSetLogging", &HYPRE_CGNRSetLogging},
  {"HYPRE_ParCSRPCGSetMaxIter", &HYPRE_ParCSRPCGSetMaxIter},
  {"HYPRE_BoomerAMGSetDebugFlag", &HYPRE_BoomerAMGSetDebugFlag},
  {"HYPRE_ParCSRHybridSetLogging", &HYPRE_ParCSRHybridSetLogging},
  {"HYPRE_EuclidSetBJ", &HYPRE_EuclidSetBJ},
  {"HYPRE_SchwarzSetOverlap", &HYPRE_SchwarzSetOverlap},
  {"HYPRE_BoomerAMGSetSepWeight", &HYPRE_BoomerAMGSetSepWeight},
  {"HYPRE_ParCSRHybridSetRelaxType", &HYPRE_ParCSRHybridSetRelaxType},
  {"HYPRE_BoomerAMGSetMaxLevels", &HYPRE_BoomerAMGSetMaxLevels},
  {"HYPRE_BoomerAMGSetAdditive", &HYPRE_BoomerAMGSetAdditive},
  {"HYPRE_ADSSetCycleType", &HYPRE_ADSSetCycleType},
  {"HYPRE_BoomerAMGSetNodal", &HYPRE_BoomerAMGSetNodal},
  {"HYPRE_ParCSRHybridSetMinCoarseSize", &HYPRE_ParCSRHybridSetMinCoarseSize},
  {"HYPRE_COGMRESSetLogging", &HYPRE_COGMRESSetLogging},
  {"HYPRE_AMSSetProjectionFrequency", &HYPRE_AMSSetProjectionFrequency},
  {"HYPRE_MGRSetNumInterpSweeps", &HYPRE_MGRSetNumInterpSweeps},
  {"HYPRE_MGRSetFRelaxMethod", &HYPRE_MGRSetFRelaxMethod},
  {"HYPRE_CGNRSetMaxIter", &HYPRE_CGNRSetMaxIter},
  {"HYPRE_BoomerAMGSetDomainType", &HYPRE_BoomerAMGSetDomainType},
  {"HYPRE_ILUSetType", &HYPRE_ILUSetType},
  {"HYPRE_ParCSRHybridSetKDim", &HYPRE_ParCSRHybridSetKDim},
  {"HYPRE_BoomerAMGDDSetNumGhostLayers", &HYPRE_BoomerAMGDDSetNumGhostLayers},
  {"HYPRE_ParCSRPilutSetFactorRowSize", &HYPRE_ParCSRPilutSetFactorRowSize},
  {"HYPRE_ParCSRFlexGMRESSetMinIter", &HYPRE_ParCSRFlexGMRESSetMinIter},
  {"HYPRE_PCGSetTwoNorm", &HYPRE_PCGSetTwoNorm},
  {"HYPRE_ParCSRHybridSetInterpType", &HYPRE_ParCSRHybridSetInterpType},
  {"HYPRE_AMSSetDimension", &HYPRE_AMSSetDimension},
  {"HYPRE_ParCSRFlexGMRESSetMaxIter", &HYPRE_ParCSRFlexGMRESSetMaxIter},
  {"HYPRE_ParCSRCOGMRESSetPrintLevel", &HYPRE_ParCSRCOGMRESSetPrintLevel},
  {"HYPRE_BoomerAMGSetRelaxType", &HYPRE_BoomerAMGSetRelaxType},
  {"HYPRE_BoomerAMGSetPlotGrids", &HYPRE_BoomerAMGSetPlotGrids},
  {"HYPRE_MGRSetMaxCoarseLevels", &HYPRE_MGRSetMaxCoarseLevels},
  {"HYPRE_ParCSRCOGMRESSetKDim", &HYPRE_ParCSRCOGMRESSetKDim},
  {"HYPRE_PCGSetRecomputeResidual", &HYPRE_PCGSetRecomputeResidual},
  {"HYPRE_ParCSRHybridSetNodal", &HYPRE_ParCSRHybridSetNodal},
  {"HYPRE_ParCSRHybridSetSeqThreshold", &HYPRE_ParCSRHybridSetSeqThreshold},
  {"HYPRE_BoomerAMGSetLevel", &HYPRE_BoomerAMGSetLevel},
  {"HYPRE_PCGSetRelChange", &HYPRE_PCGSetRelChange},
  {"HYPRE_BoomerAMGSetEuBJ", &HYPRE_BoomerAMGSetEuBJ},
  {"HYPRE_BoomerAMGSetSabs", &HYPRE_BoomerAMGSetSabs},
  {"HYPRE_COGMRESSetMinIter", &HYPRE_COGMRESSetMinIter},
  {"HYPRE_FlexGMRESSetMaxIter", &HYPRE_FlexGMRESSetMaxIter},
  {"HYPRE_BoomerAMGSetNumFunctions", &HYPRE_BoomerAMGSetNumFunctions},
  {"HYPRE_ParCSRLGMRESSetKDim", &HYPRE_ParCSRLGMRESSetKDim},
  {"HYPRE_MGRSetReservedCpointsLevelToKeep", &HYPRE_MGRSetReservedCpointsLevelToKeep},
  {"HYPRE_ParCSRLGMRESSetLogging", &HYPRE_ParCSRLGMRESSetLogging},
  {"HYPRE_PCGSetStopCrit", &HYPRE_PCGSetStopCrit},
  {"HYPRE_SchwarzSetNumFunctions", &HYPRE_SchwarzSetNumFunctions},
  {"HYPRE_ParCSRHybridSetAggNumLevels", &HYPRE_ParCSRHybridSetAggNumLevels},
  {"HYPRE_BoomerAMGDDSetFACRelaxType", &HYPRE_BoomerAMGDDSetFACRelaxType},
  {"HYPRE_MGRSetRestrictType", &HYPRE_MGRSetRestrictType},
  {"HYPRE_MGRSetPMaxElmts", &HYPRE_MGRSetPMaxElmts},
  {"HYPRE_EuclidSetStats", &HYPRE_EuclidSetStats},
  {"HYPRE_FlexGMRESSetMinIter", &HYPRE_FlexGMRESSetMinIter},
  {"HYPRE_BoomerAMGSetMeasureType", &HYPRE_BoomerAMGSetMeasureType},
  {"HYPRE_ParCSRHybridSetNumSweeps", &HYPRE_ParCSRHybridSetNumSweeps},
  {"HYPRE_ADSSetMaxIter", &HYPRE_ADSSetMaxIter},
  {"HYPRE_LGMRESSetMinIter", &HYPRE_LGMRESSetMinIter},
  {"HYPRE_SchwarzSetNonSymm", &HYPRE_SchwarzSetNonSymm},
  {"HYPRE_COGMRESSetMaxIter", &HYPRE_COGMRESSetMaxIter},
  {"HYPRE_GMRESSetLogging", &HYPRE_GMRESSetLogging},
  {"HYPRE_BoomerAMGSetConvergeType", &HYPRE_BoomerAMGSetConvergeType},
  {"HYPRE_BoomerAMGDDSetPadding", &HYPRE_BoomerAMGDDSetPadding},
  {"HYPRE_ParCSRCOGMRESSetLogging", &HYPRE_ParCSRCOGMRESSetLogging},
  {"HYPRE_BoomerAMGSetNumCRRelaxSteps", &HYPRE_BoomerAMGSetNumCRRelaxSteps},
  {"HYPRE_GMRESSetMinIter", &HYPRE_GMRESSetMinIter},
  {"HYPRE_ILUSetSchurMaxIter", &HYPRE_ILUSetSchurMaxIter},
  {"HYPRE_ParCSRBiCGSTABSetMaxIter", &HYPRE_ParCSRBiCGSTABSetMaxIter},
  {"HYPRE_ParCSRGMRESSetKDim", &HYPRE_ParCSRGMRESSetKDim},
  {"HYPRE_BoomerAMGSetNodalDiag", &HYPRE_BoomerAMGSetNodalDiag},
  {"HYPRE_BoomerAMGDDSetFACNumCycles", &HYPRE_BoomerAMGDDSetFACNumCycles},
  {"HYPRE_MGRSetBlockSize", &HYPRE_MGRSetBlockSize},
  {"HYPRE_BoomerAMGSetInterpType", &HYPRE_BoomerAMGSetInterpType},
  {"HYPRE_BoomerAMGSetMinIter", &HYPRE_BoomerAMGSetMinIter},
  {"HYPRE_ParCSRFlexGMRESSetPrintLevel", &HYPRE_ParCSRFlexGMRESSetPrintLevel},
  {"HYPRE_ParCSRLGMRESSetMinIter", &HYPRE_ParCSRLGMRESSetMinIter},
  {"HYPRE_BoomerAMGSetNumSamples", &HYPRE_BoomerAMGSetNumSamples},
  {"HYPRE_SchwarzSetDomainType", &HYPRE_SchwarzSetDomainType},
  {"HYPRE_BoomerAMGSetChebyEigEst", &HYPRE_BoomerAMGSetChebyEigEst},
  {"HYPRE_COGMRESSetUnroll", &HYPRE_COGMRESSetUnroll},
  {"HYPRE_ParCSRHybridSetRecomputeResidual", &HYPRE_ParCSRHybridSetRecomputeResidual},
  {"HYPRE_ParCSRCOGMRESSetCGS", &HYPRE_ParCSRCOGMRESSetCGS},
  {"HYPRE_ParCSRHybridSetPrintLevel", &HYPRE_ParCSRHybridSetPrintLevel},
  {"HYPRE_ParaSailsSetReuse", &HYPRE_ParaSailsSetReuse},
  {"HYPRE_ParCSRHybridSetPCGMaxIter", &HYPRE_ParCSRHybridSetPCGMaxIter},
  {"HYPRE_ParCSRBiCGSTABSetMinIter", &HYPRE_ParCSRBiCGSTABSetMinIter},
  {"HYPRE_ParCSRHybridSetCycleType", &HYPRE_ParCSRHybridSetCycleType},
  {"HYPRE_ParCSRBiCGSTABSetLogging", &HYPRE_ParCSRBiCGSTABSetLogging},
  {"HYPRE_BoomerAMGSetAggNumLevels", &HYPRE_BoomerAMGSetAggNumLevels},
  {"HYPRE_ParCSRHybridSetNumPaths", &HYPRE_ParCSRHybridSetNumPaths},
  {"HYPRE_BoomerAMGSetPMaxElmts", &HYPRE_BoomerAMGSetPMaxElmts},
  {"HYPRE_ParCSRLGMRESSetPrintLevel", &HYPRE_ParCSRLGMRESSetPrintLevel},
  {"HYPRE_MGRSetLogging", &HYPRE_MGRSetLogging},
  {"HYPRE_GMRESSetMaxIter", &HYPRE_GMRESSetMaxIter},
  {"HYPRE_ParCSRHybridSetPMaxElmts", &HYPRE_ParCSRHybridSetPMaxElmts},
  {"HYPRE_CGNRSetMinIter", &HYPRE_CGNRSetMinIter},
  {"HYPRE_MGRSetNumRestrictSweeps", &HYPRE_MGRSetNumRestrictSweeps},
  {"HYPRE_ParaSailsSetLogging", &HYPRE_ParaSailsSetLogging},
  {"HYPRE_BiCGSTABSetPrintLevel", &HYPRE_BiCGSTABSetPrintLevel},
  {"HYPRE_BoomerAMGSetGSMG", &HYPRE_BoomerAMGSetGSMG},
  {"HYPRE_ParCSRHybridSetNumFunctions", &HYPRE_ParCSRHybridSetNumFunctions},
  {"HYPRE_BiCGSTABSetStopCrit", &HYPRE_BiCGSTABSetStopCrit},
  {"HYPRE_ParCSRLGMRESSetAugDim", &HYPRE_ParCSRLGMRESSetAugDim},
  {"HYPRE_BoomerAMGSetLogging", &HYPRE_BoomerAMGSetLogging},
  {"HYPRE_BoomerAMGSetNumSweeps", &HYPRE_BoomerAMGSetNumSweeps},
  {"HYPRE_ParCSRHybridSetRelChange", &HYPRE_ParCSRHybridSetRelChange},
  {"HYPRE_BoomerAMGSetAggInterpType", &HYPRE_BoomerAMGSetAggInterpType},
  {"HYPRE_BoomerAMGSetCoordDim", &HYPRE_BoomerAMGSetCoordDim},
  {"HYPRE_BiCGSTABSetMaxIter", &HYPRE_BiCGSTABSetMaxIter},
  {"HYPRE_LGMRESSetKDim", &HYPRE_LGMRESSetKDim},
  {"HYPRE_ParCSRHybridSetStopCrit", &HYPRE_ParCSRHybridSetStopCrit},
  {"HYPRE_GMRESSetKDim", &HYPRE_GMRESSetKDim},
  {"HYPRE_BoomerAMGSetNumPaths", &HYPRE_BoomerAMGSetNumPaths},
  {"HYPRE_EuclidSetLevel", &HYPRE_EuclidSetLevel},
  {"HYPRE_BoomerAMGSetCoarsenType", &HYPRE_BoomerAMGSetCoarsenType},
  {"HYPRE_COGMRESSetCGS", &HYPRE_COGMRESSetCGS},
  {"HYPRE_GMRESSetPrintLevel", &HYPRE_GMRESSetPrintLevel},
  {"HYPRE_BoomerAMGSetSimple", &HYPRE_BoomerAMGSetSimple},
  {"HYPRE_MGRSetMaxGlobalsmoothIters", &HYPRE_MGRSetMaxGlobalsmoothIters},
  {"HYPRE_LGMRESSetMaxIter", &HYPRE_LGMRESSetMaxIter},
  {"HYPRE_AMSSetAlphaAMGCoarseRelaxType", &HYPRE_AMSSetAlphaAMGCoarseRelaxType}};

const std::map<std::string, double_func> FunctionParameter::hypreMapDoubleFunc_ = {
  {"HYPRE_BoomerAMGSetTol", &HYPRE_BoomerAMGSetTol},
  {"HYPRE_BoomerAMGSetStrongThresholdR", &HYPRE_BoomerAMGSetStrongThresholdR},
  {"HYPRE_BoomerAMGSetMaxRowSum", &HYPRE_BoomerAMGSetMaxRowSum},
  {"HYPRE_AMSSetTol", &HYPRE_AMSSetTol},
  {"HYPRE_BoomerAMGSetStrongThreshold", &HYPRE_BoomerAMGSetStrongThreshold},
  {"HYPRE_ParCSRCGNRSetTol", &HYPRE_ParCSRCGNRSetTol},
  {"HYPRE_ParCSRFlexGMRESSetAbsoluteTol", &HYPRE_ParCSRFlexGMRESSetAbsoluteTol},
  {"HYPRE_ParCSRLGMRESSetTol", &HYPRE_ParCSRLGMRESSetTol},
  {"HYPRE_GMRESSetAbsoluteTol", &HYPRE_GMRESSetAbsoluteTol},
  {"HYPRE_ParCSRPCGSetAbsoluteTol", &HYPRE_ParCSRPCGSetAbsoluteTol},
  {"HYPRE_BiCGSTABSetTol", &HYPRE_BiCGSTABSetTol},
  {"HYPRE_BoomerAMGSetCRStrongTh", &HYPRE_BoomerAMGSetCRStrongTh},
  {"HYPRE_BoomerAMGSetNonGalerkinTol", &HYPRE_BoomerAMGSetNonGalerkinTol},
  {"HYPRE_BoomerAMGDDSetFACRelaxWeight", &HYPRE_BoomerAMGDDSetFACRelaxWeight},
  {"HYPRE_BoomerAMGSetCRRate", &HYPRE_BoomerAMGSetCRRate},
  {"HYPRE_ParaSailsSetLoadbal", &HYPRE_ParaSailsSetLoadbal},
  {"HYPRE_BoomerAMGSetJacobiTruncThreshold", &HYPRE_BoomerAMGSetJacobiTruncThreshold},
  {"HYPRE_BoomerAMGSetFilterThresholdR", &HYPRE_BoomerAMGSetFilterThresholdR},
  {"HYPRE_PCGSetResidualTol", &HYPRE_PCGSetResidualTol},
  {"HYPRE_BiCGSTABSetConvergenceFactorTol", &HYPRE_BiCGSTABSetConvergenceFactorTol},
  {"HYPRE_PCGSetConvergenceFactorTol", &HYPRE_PCGSetConvergenceFactorTol},
  {"HYPRE_LGMRESSetAbsoluteTol", &HYPRE_LGMRESSetAbsoluteTol},
  {"HYPRE_BiCGSTABSetAbsoluteTol", &HYPRE_BiCGSTABSetAbsoluteTol},
  {"HYPRE_ParCSRParaSailsSetLoadbal", &HYPRE_ParCSRParaSailsSetLoadbal},
  {"HYPRE_PCGSetAbsoluteTolFactor", &HYPRE_PCGSetAbsoluteTolFactor},
  {"HYPRE_COGMRESSetTol", &HYPRE_COGMRESSetTol},
  {"HYPRE_GMRESSetTol", &HYPRE_GMRESSetTol},
  {"HYPRE_BoomerAMGSetAddRelaxWt", &HYPRE_BoomerAMGSetAddRelaxWt},
  {"HYPRE_LGMRESSetTol", &HYPRE_LGMRESSetTol},
  {"HYPRE_BoomerAMGSetAggTruncFactor", &HYPRE_BoomerAMGSetAggTruncFactor},
  {"HYPRE_ADSSetTol", &HYPRE_ADSSetTol},
  {"HYPRE_ParCSRCOGMRESSetAbsoluteTol", &HYPRE_ParCSRCOGMRESSetAbsoluteTol},
  {"HYPRE_ILUSetTol", &HYPRE_ILUSetTol},
  {"HYPRE_ParCSRCOGMRESSetTol", &HYPRE_ParCSRCOGMRESSetTol},
  {"HYPRE_ParCSRHybridSetTruncFactor", &HYPRE_ParCSRHybridSetTruncFactor},
  {"HYPRE_MGRSetTol", &HYPRE_MGRSetTol},
  {"HYPRE_ParCSRGMRESSetTol", &HYPRE_ParCSRGMRESSetTol},
  {"HYPRE_BoomerAMGSetDropTol", &HYPRE_BoomerAMGSetDropTol},
  {"HYPRE_BoomerAMGSetSchwarzRlxWeight", &HYPRE_BoomerAMGSetSchwarzRlxWeight},
  {"HYPRE_ParCSRHybridSetMaxRowSum", &HYPRE_ParCSRHybridSetMaxRowSum},
  {"HYPRE_ParCSRHybridSetAbsoluteTol", &HYPRE_ParCSRHybridSetAbsoluteTol},
  {"HYPRE_BoomerAMGSetRelaxWt", &HYPRE_BoomerAMGSetRelaxWt},
  {"HYPRE_BoomerAMGSetSCommPkgSwitch", &HYPRE_BoomerAMGSetSCommPkgSwitch},
  {"HYPRE_PCGSetAbsoluteTol", &HYPRE_PCGSetAbsoluteTol},
  {"HYPRE_ParCSRHybridSetRelaxWt", &HYPRE_ParCSRHybridSetRelaxWt},
  {"HYPRE_ParCSRLGMRESSetAbsoluteTol", &HYPRE_ParCSRLGMRESSetAbsoluteTol},
  {"HYPRE_EuclidSetSparseA", &HYPRE_EuclidSetSparseA},
  {"HYPRE_ParCSRHybridSetConvergenceTol", &HYPRE_ParCSRHybridSetConvergenceTol},
  {"HYPRE_MGRSetTruncateCoarseGridThreshold", &HYPRE_MGRSetTruncateCoarseGridThreshold},
  {"HYPRE_GMRESSetConvergenceFactorTol", &HYPRE_GMRESSetConvergenceFactorTol},
  {"HYPRE_BoomerAMGSetTruncFactor", &HYPRE_BoomerAMGSetTruncFactor},
  {"HYPRE_ParCSRHybridSetStrongThreshold", &HYPRE_ParCSRHybridSetStrongThreshold},
  {"HYPRE_BoomerAMGSetOuterWt", &HYPRE_BoomerAMGSetOuterWt},
  {"HYPRE_BoomerAMGSetFilter", &HYPRE_BoomerAMGSetFilter},
  {"HYPRE_BoomerAMGSetInterpVecAbsQTrunc", &HYPRE_BoomerAMGSetInterpVecAbsQTrunc},
  {"HYPRE_BoomerAMGSetChebyFraction", &HYPRE_BoomerAMGSetChebyFraction},
  {"HYPRE_BoomerAMGSetADropTol", &HYPRE_BoomerAMGSetADropTol},
  {"HYPRE_SchwarzSetRelaxWeight", &HYPRE_SchwarzSetRelaxWeight},
  {"HYPRE_EuclidSetILUT", &HYPRE_EuclidSetILUT},
  {"HYPRE_PCGSetTol", &HYPRE_PCGSetTol},
  {"HYPRE_ILUSetDropThreshold", &HYPRE_ILUSetDropThreshold},
  {"HYPRE_ParaSailsSetFilter", &HYPRE_ParaSailsSetFilter},
  {"HYPRE_CGNRSetTol", &HYPRE_CGNRSetTol},
  {"HYPRE_LGMRESSetConvergenceFactorTol", &HYPRE_LGMRESSetConvergenceFactorTol},
  {"HYPRE_ParCSRHybridSetOuterWt", &HYPRE_ParCSRHybridSetOuterWt},
  {"HYPRE_FlexGMRESSetTol", &HYPRE_FlexGMRESSetTol},
  {"HYPRE_BoomerAMGSetThreshold", &HYPRE_BoomerAMGSetThreshold},
  {"HYPRE_ILUSetNSHDropThreshold", &HYPRE_ILUSetNSHDropThreshold},
  {"HYPRE_ParCSRBiCGSTABSetTol", &HYPRE_ParCSRBiCGSTABSetTol},
  {"HYPRE_ParCSRBiCGSTABSetAbsoluteTol", &HYPRE_ParCSRBiCGSTABSetAbsoluteTol},
  {"HYPRE_BoomerAMGSetEuSparseA", &HYPRE_BoomerAMGSetEuSparseA},
  {"HYPRE_FlexGMRESSetAbsoluteTol", &HYPRE_FlexGMRESSetAbsoluteTol},
  {"HYPRE_ParCSRFlexGMRESSetTol", &HYPRE_ParCSRFlexGMRESSetTol},
  {"HYPRE_ParCSRParaSailsSetFilter", &HYPRE_ParCSRParaSailsSetFilter},
  {"HYPRE_BoomerAMGSetMultAddTruncFactor", &HYPRE_BoomerAMGSetMultAddTruncFactor},
  {"HYPRE_ParCSRPCGSetTol", &HYPRE_ParCSRPCGSetTol},
  {"HYPRE_ParCSRGMRESSetAbsoluteTol", &HYPRE_ParCSRGMRESSetAbsoluteTol},
  {"HYPRE_ParCSRHybridSetTol", &HYPRE_ParCSRHybridSetTol},
  {"HYPRE_COGMRESSetConvergenceFactorTol", &HYPRE_COGMRESSetConvergenceFactorTol},
  {"HYPRE_FlexGMRESSetConvergenceFactorTol", &HYPRE_FlexGMRESSetConvergenceFactorTol},
  {"HYPRE_COGMRESSetAbsoluteTol", &HYPRE_COGMRESSetAbsoluteTol},
  {"HYPRE_ParCSRPilutSetDropTolerance", &HYPRE_ParCSRPilutSetDropTolerance}};

const std::map<std::string, double_int_func> FunctionParameter::hypreMapDoubleIntFunc_ = {
};

const std::map<std::string, int_int_func> FunctionParameter::hypreMapIntIntFunc_ = {
  {"HYPRE_ParCSRHybridSetCycleNumSweeps", &HYPRE_ParCSRHybridSetCycleNumSweeps},
  {"HYPRE_BoomerAMGSetCycleNumSweeps", &HYPRE_BoomerAMGSetCycleNumSweeps},
  {"HYPRE_ParCSRHybridSetCycleRelaxType", &HYPRE_ParCSRHybridSetCycleRelaxType},
  {"HYPRE_BoomerAMGSetCycleRelaxType", &HYPRE_BoomerAMGSetCycleRelaxType},
  {"HYPRE_ADSSetChebySmoothingOptions", &HYPRE_ADSSetChebySmoothingOptions}};

const std::map<std::string, int_star_func> FunctionParameter::hypreMapIntStarFunc_ = {
  {"HYPRE_ParCSRHybridSetNumGridSweeps", &HYPRE_ParCSRHybridSetNumGridSweeps},
  {"HYPRE_MGRSetLevelInterpType", &HYPRE_MGRSetLevelInterpType},
  {"HYPRE_MGRSetCoarseGridMethod", &HYPRE_MGRSetCoarseGridMethod},
  {"HYPRE_ParCSRHybridSetDofFunc", &HYPRE_ParCSRHybridSetDofFunc},
  {"HYPRE_BoomerAMGSetDofFunc", &HYPRE_BoomerAMGSetDofFunc},
  {"HYPRE_SchwarzSetDofFunc", &HYPRE_SchwarzSetDofFunc},
  {"HYPRE_BoomerAMGSetNumGridSweeps", &HYPRE_BoomerAMGSetNumGridSweeps},
  {"HYPRE_ParCSRHybridSetGridRelaxType", &HYPRE_ParCSRHybridSetGridRelaxType},
  {"HYPRE_MGRSetLevelRestrictType", &HYPRE_MGRSetLevelRestrictType},
  {"HYPRE_MGRSetLevelFRelaxMethod", &HYPRE_MGRSetLevelFRelaxMethod},
  {"HYPRE_BoomerAMGSetGridRelaxType", &HYPRE_BoomerAMGSetGridRelaxType},
  {"HYPRE_MGRSetLevelFRelaxNumFunctions", &HYPRE_MGRSetLevelFRelaxNumFunctions}};

const std::map<std::string, double_star_func> FunctionParameter::hypreMapDoubleStarFunc_ = {
  {"HYPRE_BoomerAMGSetRelaxWeight", &HYPRE_BoomerAMGSetRelaxWeight},
  {"HYPRE_ParCSRHybridSetOmega", &HYPRE_ParCSRHybridSetOmega},
  {"HYPRE_ILUSetDropThresholdArray", &HYPRE_ILUSetDropThresholdArray},
  {"HYPRE_ParCSRHybridSetRelaxWeight", &HYPRE_ParCSRHybridSetRelaxWeight},
  {"HYPRE_BoomerAMGSetOmega", &HYPRE_BoomerAMGSetOmega},
  {"HYPRE_ILUSetNSHDropThresholdArray", &HYPRE_ILUSetNSHDropThresholdArray},
  {"HYPRE_ParCSRHybridGetSetupSolveTime", &HYPRE_ParCSRHybridGetSetupSolveTime}};

const std::map<std::string, int_int_double_double_func> FunctionParameter::hypreMapIntIntDoubleDoubleFunc_ = {
  {"HYPRE_AMSSetSmoothingOptions", &HYPRE_AMSSetSmoothingOptions},
  {"HYPRE_ADSSetSmoothingOptions", &HYPRE_ADSSetSmoothingOptions}};

const std::map<std::string, int_int_int_double_int_int_func> FunctionParameter::hypreMapIntIntIntDoubleIntIntFunc_ = {
  {"HYPRE_AMSSetAlphaAMGOptions", &HYPRE_AMSSetAlphaAMGOptions},
  {"HYPRE_AMSSetBetaAMGOptions", &HYPRE_AMSSetBetaAMGOptions},
  {"HYPRE_ADSSetAMGOptions", &HYPRE_ADSSetAMGOptions}};

const std::map<std::string, int_star_star_func> FunctionParameter::hypreMapIntStarStarFunc_ = {
  {"HYPRE_BoomerAMGSetGridRelaxPoints", &HYPRE_BoomerAMGSetGridRelaxPoints},
  {"HYPRE_ParCSRHybridSetGridRelaxPoints", &HYPRE_ParCSRHybridSetGridRelaxPoints}};

const std::map<std::string, char_star_func> FunctionParameter::hypreMapCharStarFunc_ = {
  {"HYPRE_BoomerAMGSetEuclidFile", &HYPRE_BoomerAMGSetEuclidFile},
  {"HYPRE_EuclidSetParamsFromFile", &HYPRE_EuclidSetParamsFromFile}};
