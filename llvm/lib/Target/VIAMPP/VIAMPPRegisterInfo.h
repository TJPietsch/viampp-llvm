//===-- VIAMPPRegisterInfo.h - VIAMPP Register Information Impl ----------------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------------------------------------------------------===//
//
// This file contains the VIAMPP implementation of the MRegisterInfo class.
//
//===-------------------------------------------------------------------------------------------------------------------------===//

#ifndef VIAMPPREGISTERINFO_H
#define VIAMPPREGISTERINFO_H

#include "llvm/CodeGen/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "VIAMPPGenRegisterInfo.inc"

namespace llvm {

class TargetInstrInfo;
class Type;

struct VIAMPPRegisterInfo : public VIAMPPGenRegisterInfo {

  const TargetInstrInfo &TII;

  VIAMPPRegisterInfo(const TargetInstrInfo &tii);

  /// Code Generation virtual methods...
  const MCPhysReg *getCalleeSavedRegs(const MachineFunction *MF) const override;

  BitVector getReservedRegs(const MachineFunction &MF) const override;

  bool requiresRegisterScavenging(const MachineFunction &MF) const override;

  void eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj, unsigned FIOperandNum, RegScavenger *RS = nullptr) 
                          const override;

  bool hasBasePointer(const MachineFunction &MF) const;

  // Debug information queries.
  Register getFrameRegister(const MachineFunction &MF) const override;
  Register getBaseRegister() const;
};

} // end namespace llvm

#endif
