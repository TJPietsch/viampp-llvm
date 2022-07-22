//===-- VIAMPPFrameLowering.h - Frame info for VIAMPP Target -------------------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------------------------------------------------------===//
//
// This class implements VIAMPP-specific bits of the TargetFrameLowering class.
//
//===-------------------------------------------------------------------------------------------------------------------------===//

#ifndef VIAMPPFRAMEINFO_H
#define VIAMPPFRAMEINFO_H

#include "VIAMPP.h"
#include "llvm/CodeGen/TargetFrameLowering.h"

namespace llvm {
class VIAMPPSubtarget;

class VIAMPPFrameLowering : public TargetFrameLowering {
public:
  explicit VIAMPPFrameLowering(const VIAMPPSubtarget &STI)
//      : TargetFrameLowering(TargetFrameLowering::StackGrowsDown,(llvm::Align)8, 0, (llvm::Align)8) {}
      : TargetFrameLowering(TargetFrameLowering::StackGrowsDown,Align(8), 0) {}

  // emitProlog/emitEpilog - These methods insert prolog and epilog code into the function.
  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  MachineBasicBlock::iterator
  eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB, MachineBasicBlock::iterator MI) const override;

  bool hasFP(const MachineFunction &MF) const override;

  void determineCalleeSaves(MachineFunction &MF, BitVector &SavedRegs, RegScavenger *RS = nullptr) const override;

  static int stackSlotSize() { return 8; }

private:
  void determineFrameLayout(MachineFunction &MF) const;
  void replaceAdjDynAllocPseudo(MachineFunction &MF) const;
};
}

#endif // VIAMPPFRAMEINFO_H
