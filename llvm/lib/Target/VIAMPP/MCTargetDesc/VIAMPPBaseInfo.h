//===--VIAMPPBaseInfo.h - Top Level definition for VIAMPP MC -----------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------------------------------------------------------===//
//
// This file contains small standalone helper functions and enum definitions for the VIAMPP target useful for the compiler
// back-end and the MC libraries.
//
//===-------------------------------------------------------------------------------------------------------------------------===//

#ifndef VIAMPPBASEINFO_H
#define VIAMPPBASEINFO_H

#include "VIAMPPMCTargetDesc.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/Support/DataTypes.h"
#include "llvm/Support/ErrorHandling.h"

namespace llvm {

// VIAMPPII - This namespace holds all of the target specific flags that instruction info tracks.
namespace VIAMPPII {
// Target Operand Flag enum.
	enum TOF {
		//===------------------------------------------------------------------===//
		// VIAMPP Specific MachineOperand flags.
		MO_NO_FLAG,

		// MO_ABS_HI/LO - Represents the hi or low part of an absolute symbol address.
		MO_ABS_HI,
		MO_ABS_LO,
	};
} // namespace VIAMPPII

static inline unsigned getVIAMPPRegisterNumbering(unsigned Reg) {
	//	outs().changeColor(raw_ostream::YELLOW,0);
	//	outs() << "---MOVoffAnalysisPass::runOnMachineFunction\n";
	//	outs().changeColor(raw_ostream::WHITE,0);
	switch(Reg) {
        default: llvm_unreachable("Unknown register number!");
  }
}
} // namespace llvm

#endif
