//===-- VIAMPPInstrInfo.h - VIAMPP Instruction Information ---------------------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------------------------------------------------------===//
//
// This file contains the VIAMPP implementation of the TargetInstrInfo class.
//
//===-------------------------------------------------------------------------------------------------------------------------===//

#ifndef VIAMPPINSTRUCTIONINFO_H
#define VIAMPPINSTRUCTIONINFO_H

#include "VIAMPPRegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "VIAMPPGenInstrInfo.inc"

namespace llvm {

class VIAMPPInstrInfo : public VIAMPPGenInstrInfo {
  const VIAMPPRegisterInfo RI;
  virtual void anchor();

public:
	VIAMPPInstrInfo();

	// getRegisterInfo - TargetInstrInfo is a superset of MRegister info.  As such, whenever a client has an instance of
	// instruction info, it should always be able to get register info as well (through this method).
	const VIAMPPRegisterInfo &getRegisterInfo() const { return RI; }

	void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator I, const DebugLoc &DL, MCRegister DestReg,
					 MCRegister SrcReg, bool KillSrc) const override;

	virtual void storeRegToStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator MI, Register SrcReg, bool isKill,
									 int FrameIndex, const TargetRegisterClass *RC, const TargetRegisterInfo *TRI) const override;

	virtual void loadRegFromStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator MI, Register DestReg, int FrameIndex,
									  const TargetRegisterClass *RC, const TargetRegisterInfo *TRI) const override;

	bool analyzeBranch(MachineBasicBlock &MBB, MachineBasicBlock *&TBB, MachineBasicBlock *&FBB,
					   SmallVectorImpl<MachineOperand> &Cond, bool AllowModify) const override;

	MachineBasicBlock *getBranchDestBlock(const MachineInstr &MI) const override;


	unsigned removeBranch(MachineBasicBlock &MBB, int *BytesRemoved = nullptr) const override;

	unsigned insertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB, MachineBasicBlock *FBB, ArrayRef<MachineOperand> Cond,
						  const DebugLoc &DL, int *BytesAdded = nullptr) const override;

	bool expandPostRAPseudo(MachineInstr &MI) const override;
};
}

#endif
