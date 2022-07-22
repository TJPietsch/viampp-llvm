//===-- VIAMPPRegisterInfo.cpp - VIAMPP Register Information -----------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------------------------------------------------------===//
//
// This file contains the VIAMPP implementation of the RegisterInfo class.
//
//===-------------------------------------------------------------------------------------------------------------------------===//

#include "VIAMPP.h"
#include "VIAMPPRegisterInfo.h"
#include "VIAMPPSubtarget.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/IR/Instruction.h"

#define GET_REGINFO_TARGET_DESC
#include "VIAMPPGenRegisterInfo.inc"

using namespace llvm;

VIAMPPRegisterInfo::VIAMPPRegisterInfo(const TargetInstrInfo &tii) : VIAMPPGenRegisterInfo(VIAMPP::LinkReg), TII(tii) {
//	outs().changeColor(raw_ostream::YELLOW,0);
//	outs() << "VIAMPPRegisterInfo::VIAMPPRegisterInfo\n";
//	outs().changeColor(raw_ostream::WHITE,0);
}

// Return a null-terminated list of all of the callee-saved registers on this target. The register should be in the order of desired
// callee-save stack frame offset. The first register is closest to the incoming stack pointer if stack grows down, and vice versa.
// Notice: This function does not take into account disabled CSRs.
//         In most cases you will want to use instead the function
//         getCalleeSavedRegs that is implemented in MachineRegisterInfo.
const uint16_t *VIAMPPRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
//	outs().changeColor(raw_ostream::YELLOW,0);
//	outs() << "VIAMPPRegisterInfo::getCalleeSavedRegs\n";
//	outs().changeColor(raw_ostream::WHITE,0);

    const TargetFrameLowering *TFI = getFrameLowering(*MF);

    return TFI->hasFP(*MF) ? CSR_FP_SaveList : CSR_SaveList;
}

// Returns a bitset indexed by physical register number indicating if a register is a special register that has particular uses and
// should be considered unavailable at all times, e.g. stack pointer, return address.
// A reserved register:
// - is not allocatable
// - is considered always live
// - is ignored by liveness tracking
// It is often necessary to reserve the super registers of a reserved register as well, to avoid them getting allocated indirectly.
// You may use markSuperRegs() and checkAllSuperRegsMarked() in this case.
BitVector VIAMPPRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
//	outs().changeColor(raw_ostream::YELLOW,0);
//	outs() << "VIAMPPRegisterInfo::getReservedRegs\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	BitVector Reserved(getNumRegs());
	const VIAMPPFrameLowering *TFI = getFrameLowering(MF);

	Reserved.set(VIAMPP::StackPtr);				// Stack pointer Register
	if(TFI->hasFP(MF))
		Reserved.set(VIAMPP::StackPtr);            // Frame pointer Register
	Reserved.set(VIAMPP::LinkReg);					// Link register/Return address pointer
	if (hasBasePointer(MF))
		Reserved.set(getBaseRegister());  		// Base Pointer
	Reserved.set(VIAMPP::ReturnReg);				// Return Register
	return Reserved;
}

// Returns true if the target requires (and can make use of) the register scavenger.
bool VIAMPPRegisterInfo::requiresRegisterScavenging(const MachineFunction &MF) const {
//	outs().changeColor(raw_ostream::YELLOW,0);
//	outs() << "VIAMPPRegisterInfo::requiresRegisterScavenging\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	return true;
}

// This method eliminates abstract frame indices from instructions which may use them. The instruction referenced by the iterator
// contains an MO_FrameIndex operand which must be eliminated by this method. This method may modify or replace the specified
// instruction, as long as it keeps the iterator pointing at the finished product. SPAdj is the SP adjustment due to call frame
// setup instruction. FIOperandNum is the FI operand number.
//
// FrameIndex represent objects inside a abstract stack. We must replace FrameIndex with an stack/frame pointer direct reference.
void VIAMPPRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj, unsigned FIOperandNum, RegScavenger *RS) const {
//	outs().changeColor(raw_ostream::YELLOW,0);
//	outs() << "VIAMPPRegisterInfo::eliminateFrameIndex\n";
//	outs().changeColor(raw_ostream::WHITE,0);

    assert(SPAdj == 0 && "Unexpected");

    MachineInstr &MI = *II;
    MachineBasicBlock &MBB = *MI.getParent();
    MachineFunction &MF = *MBB.getParent();

    const VIAMPPFrameLowering *TFI = getFrameLowering(MF);
    DebugLoc dl = MI.getDebugLoc();

    int FrameIndex = MI.getOperand(FIOperandNum).getIndex();
    long Offset = MF.getFrameInfo().getObjectOffset(FrameIndex);

    unsigned BasePtr = (TFI->hasFP(MF) ? VIAMPP::FramePtr : VIAMPP::StackPtr);

	if (!TFI->hasFP(MF)) {
		Offset += MF.getFrameInfo().getStackSize();
	} else {
		Offset += 8; // Skip the saved FP
	}

	// Fold imm into offset
	Offset += MI.getOperand(FIOperandNum + 1).getImm();
	Offset/= 8;		// devide by 8 since VIAMPP only works on 64-bit stack slots

//	outs().changeColor(raw_ostream::YELLOW,0);
//	outs() << "VIAMPPRegisterInfo::EliminateFrameIndex: Instruction: " << MI.getOpcode() << "\n";
//	outs() << "BasePtr: " << BasePtr << " Offset: " << Offset << " Imm: " << MI.getOperand(FIOperandNum + 1).getImm() << "\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	if (isInt<16>(Offset)) {
//		outs().changeColor(raw_ostream::YELLOW,0);
//		outs() << "---VIAMPPRegisterInfo::eliminateFrameIndex: Offset fits in 16 bit\n";
//		outs().changeColor(raw_ostream::WHITE,0);

		MI.getOperand(FIOperandNum).ChangeToRegister(BasePtr, false);	// isDef=false
		MI.getOperand(FIOperandNum+1).ChangeToImmediate(Offset);

		return;
	} else {	// end if Offset is 16 bit
		if (isInt<32>(Offset)) {
//			outs().changeColor(raw_ostream::YELLOW,0);
//			outs() << "---VIAMPPRegisterInfo::eliminateFrameIndex: Offset fits in 32 bit\n";
//			outs().changeColor(raw_ostream::WHITE,0);

			Register DstReg = MI.getOperand(0).getReg();
			MI.setDesc(TII.get(VIAMPP::MOVaxa));
			MI.getOperand(FIOperandNum).ChangeToRegister(BasePtr, false);

			if (Offset < 0) {
				//				outs() << "--VIAMPPRegisterInfo::eliminateFrameIndex: Offset < 0 -> SUBa2i: " << Offset << "\n";
				BuildMI(MBB, std::next(II), dl, TII.get(VIAMPP::SUBad2i), DstReg).addReg(DstReg).addImm(-Offset);
			} else {
				//				outs() << "--VIAMPPRegisterInfo::eliminateFrameIndex: Offset >= 0 -> ADDa2i: " << Offset << "\n";
				BuildMI(MBB, std::next(II), dl, TII.get(VIAMPP::ADDad2i), DstReg).addReg(DstReg).addImm(Offset);
			}

			MI.getOperand(FIOperandNum).ChangeToRegister(DstReg, false, false, true);	// isDef, isImp, isKill
			MI.getOperand(FIOperandNum+1).ChangeToImmediate(0);

			return;
		} else {
//			outs().changeColor(raw_ostream::YELLOW,0);
//			outs() << "---VIAMPPRegisterInfo::eliminateFrameIndex: Offset > 32 bit\n";
//			outs().changeColor(raw_ostream::WHITE,0);
			llvm_unreachable("frame index offset larger than isInt<32> bit not yet supported!!");
			return;
		}
	}

	// we should never reach here. just left from development. delete when testing finished!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	outs().changeColor(raw_ostream::YELLOW,0);
	outs() << "VIAMPPRegisterInfo::eliminateFrameIndex: fall through should never happen !!!\n";
	outs().changeColor(raw_ostream::WHITE,0);
	MI.getOperand(FIOperandNum).ChangeToRegister(BasePtr, false);
	MI.getOperand(FIOperandNum+1).ChangeToImmediate(Offset);
	return;

//	}
}


bool VIAMPPRegisterInfo::hasBasePointer(const MachineFunction &MF) const {
//	outs().changeColor(raw_ostream::YELLOW,0);
//	outs() << "VIAMPPRegisterInfo::hasBasePointer\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	const MachineFrameInfo &MFI = MF.getFrameInfo();
	// When we need stack realignment and there are dynamic allocas, we can't reference off of the stack pointer, so we reserve a
	// base pointer.
	if (needsStackRealignment(MF) && MFI.hasVarSizedObjects())
		return true;

	return false;
}

Register VIAMPPRegisterInfo::getBaseRegister() const {
	outs().changeColor(raw_ostream::YELLOW,0);
	outs() << "VIAMPPRegisterInfo::getBaseRegister\n";
	outs().changeColor(raw_ostream::WHITE,0);

  return VIAMPP::BaseReg;		// BaseReg
}

// This method should return the register used as a base for values allocated in the current stack frame.
Register VIAMPPRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
	outs().changeColor(raw_ostream::YELLOW,0);
	outs() << "VIAMPPRegisterInfo::getFrameRegister\n";
	outs().changeColor(raw_ostream::WHITE,0);

	const TargetFrameLowering *TFI = getFrameLowering(MF);

	return TFI->hasFP(MF) ? VIAMPP::FramePtr : VIAMPP::StackPtr;		// FramePtr, StackPtr
}
