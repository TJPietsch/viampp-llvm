//===-- VIAMPP.h - Top-level interface for VIAMPP representation ------------------------------------------------*- -- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM VIAMPP back-end.
//
//===-------------------------------------------------------------------------------------------------------------------------===//

#ifndef TARGET_VIAMPP_H
#define TARGET_VIAMPP_H

#include "MCTargetDesc/VIAMPPMCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class TargetMachine;
class VIAMPPTargetMachine;
class MCInst;
class MachineInstr;

// createVIAMPPISelDag - This pass converts a legalized DAG into a VIAMPP-specific DAG, ready for instruction scheduling.
FunctionPass *createVIAMPPISelDag(VIAMPPTargetMachine &TM, CodeGenOpt::Level OptLevel);

// createVIAMPPOptMachine - This pass replaces MOVoff instructions with appropriate MOV instructions if the offset is zero.
FunctionPass *createVIAMPPOptMachinePass();
} // end namespace llvm;

#endif
