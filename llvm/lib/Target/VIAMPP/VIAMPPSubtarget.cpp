//===-- VIAMPPSubtarget.cpp - VIAMPP Subtarget Information -------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------------------------------------------------------===//
//
// This file implements the VIAMPP specific subclass of TargetSubtargetInfo.
//
//===-------------------------------------------------------------------------------------------------------------------------===//

#include "VIAMPP.h"
#include "VIAMPPFrameLowering.h"
#include "VIAMPPSubtarget.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "VIAMPP-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "VIAMPPGenSubtargetInfo.inc"

void VIAMPPSubtarget::anchor() {}

VIAMPPSubtarget::VIAMPPSubtarget(const Triple &TT, const StringRef CPU, const StringRef FS, const VIAMPPTargetMachine &TM)
    : VIAMPPGenSubtargetInfo(TT, CPU, /* Tune CPU */ CPU, FS),
	  InstrInfo(),
	  FrameLowering((*this)),
          TLInfo(TM, *this),
	  TSInfo() {

    // Determine default and user specified characteristics
    StringRef CPUName = CPU;
    // CPUName is empty when invoked from tools like llc
    if (CPUName.empty())
    	CPUName = "viampp";
}
