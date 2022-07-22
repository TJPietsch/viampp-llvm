//==- VIAMPPMachineFunctionInfo.h - VIAMPP Machine Function Info ----------------------------------------------------*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------------------------------------------------------===//
//
// This file declares  VIAMPP specific per-machine-function information.
//
//===-------------------------------------------------------------------------------------------------------------------------===//
#ifndef VIAMPPMACHINEFUNCTIONINFO_H
#define VIAMPPMACHINEFUNCTIONINFO_H

#include "VIAMPPRegisterInfo.h"
#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {
class MachineFunction;

// VIAMPPMachineFunctionInfo - This class is derived from MachineFunction and contains private VIAMPP target-specific information for
// each MachineFunction.

class VIAMPPMachineFunctionInfo : public MachineFunctionInfo {

public:
  VIAMPPMachineFunctionInfo(MachineFunction &MF) : SRetReturnReg(0), VarArgsFrameIndex(0), MaxUsedRegs(0) {}

  unsigned getSRetReturnReg() const { return SRetReturnReg; }
  void setSRetReturnReg(unsigned Reg) { SRetReturnReg = Reg; }

  int getVarArgsFrameIndex() const { return VarArgsFrameIndex; }
  void setVarArgsFrameIndex(int Index) { VarArgsFrameIndex = Index; }

  unsigned long long getMaxUsedRegs() const { return MaxUsedRegs; }
  void setMaxUsedRegs(unsigned long long numRegs) {
	  if (numRegs > MaxUsedRegs) MaxUsedRegs = numRegs;
//	  outs() << "VIAMPPMachineFunction: MaxUsedRegs: " << MaxUsedRegs << "\n";
  }

private:
  virtual void anchor();

  // SRetReturnReg - VIAMPP ABI require that sret lowering includes returning the value of the returned struct in a register. This
  // field holds the virtual register into which the sret argument is passed.
  unsigned SRetReturnReg;

  // VarArgsFrameIndex - FrameIndex for start of varargs area.
  int VarArgsFrameIndex;

  unsigned long long MaxUsedRegs;
};
}

#endif

