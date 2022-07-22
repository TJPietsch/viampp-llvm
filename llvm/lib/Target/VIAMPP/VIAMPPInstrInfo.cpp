//===-- VIAMPPInstrInfo.cpp - VIAMPP Instruction Information -------------------------------------------------------*- C++ -*-===//
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

#include "VIAMPPInstrInfo.h"
#include "VIAMPP.h"
#include "VIAMPPMachineFunctionInfo.h"
#include "VIAMPPSubtarget.h"
#include "VIAMPPTargetMachine.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_CTOR_DTOR
#include "VIAMPPGenInstrInfo.inc"

using namespace llvm;

// Pin the vtable to this file.
void VIAMPPInstrInfo::anchor() {}

VIAMPPInstrInfo::VIAMPPInstrInfo() : VIAMPPGenInstrInfo(VIAMPP::ADJCALLSTACKDOWN, VIAMPP::ADJCALLSTACKUP), RI(*this) {
//	outs().changeColor(raw_ostream::BLUE,0);
//	outs() << "VIAMPPInstrInfo::VIAMPPInstrInfo\n";
//	outs().changeColor(raw_ostream::WHITE,0);
}

// Emit instructions to copy a pair of physical registers.
//
// This function should support copies within any legal register class as well as any cross-class copies created during instruction
// selection.
//
// The source and destination registers may overlap, which may require a careful implementation when multiple copy instructions are
// required for large registers. See for example the ARM target.
void VIAMPPInstrInfo::copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator MI, const DebugLoc &DL, MCRegister DestReg,
                               MCRegister SrcReg, bool KillSrc) const {
//	outs().changeColor(raw_ostream::BLUE,0);
//	outs() << "VIAMPPInstrInfo::copyPhysReg\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	if (VIAMPP::VRegsRegClass.contains(DestReg, SrcReg))
		BuildMI(MBB, MI, DL, get(VIAMPP::MOVaxa), DestReg).addReg(SrcReg, getKillRegState(KillSrc));
	else
		llvm_unreachable("Impossible reg-to-reg copy");
}

// Store the specified register of the given register class to the specified stack frame index. The store instruction is to be
// added to the given machine basic block before the specified machine instruction. If isKill is true, the register operand is the
// last use and must be marked kill.
void VIAMPPInstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator MI, Register SrcReg, bool isKill,
                                       int FrameIdx, const TargetRegisterClass *RC, const TargetRegisterInfo *TRI) const {
//	outs().changeColor(raw_ostream::BLUE,0);
//	outs() << "VIAMPPInstrInfo::storeRegToStackSlot\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	DebugLoc DL;
	if (MI != MBB.end())
		DL = MI->getDebugLoc();

	if (RC == &VIAMPP::VRegsRegClass)
		BuildMI(MBB, MI, DL, get(VIAMPP::MOVaxa)).addReg(SrcReg, getKillRegState(isKill)).addFrameIndex(FrameIdx);
	else
		llvm_unreachable("Can't store this register to stack slot");
}

// Load the specified register of the given register class from the specified stack frame index. The load instruction is to be
// added to the given machine basic block before the specified machine instruction.
void VIAMPPInstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator MI, Register DestReg, int FrameIdx,
                                        const TargetRegisterClass *RC, const TargetRegisterInfo *TRI) const{
//	outs().changeColor(raw_ostream::BLUE,0);
//	outs() << "VIAMPPInstrInfo::LoadRegFromStackSlot\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	DebugLoc DL;
	if (MI != MBB.end())
		DL = MI->getDebugLoc();

	if (RC == &VIAMPP::VRegsRegClass)
		BuildMI(MBB, MI, DL, get(VIAMPP::MOVaxa), DestReg).addFrameIndex(FrameIdx);
	else
		llvm_unreachable("Can't store this register to stack slot");
}

MachineBasicBlock *VIAMPPInstrInfo::getBranchDestBlock(const MachineInstr &MI) const {
//	outs().changeColor(raw_ostream::BLUE,0);
//	outs() << "VIAMPPInstrInfo::getBranchDestBlock\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	assert(MI.getDesc().isBranch() && "Unexpected opcode!");
	// The branch target is always the last operand.
	int NumOp = MI.getNumExplicitOperands();
	return MI.getOperand(NumOp - 1).getMBB();
}

// Analyze the branching code at the end of MBB, returning true if it cannot be understood (e.g. it's a switch dispatch or isn't
// implemented for a target).  Upon success, this returns false and returns with the following information in various cases:
//
// 1. If this block ends with no branches (it just falls through to its succ) just return false, leaving TBB/FBB null.
// 2. If this block ends with only an unconditional branch, it sets TBB to be the destination block.
// 3. If this block ends with a conditional branch and it falls through to a successor block, it sets TBB to be the branch
//    destination block and a list of operands that evaluate the condition. These operands can be passed to other TargetInstrInfo
//    methods to create new branches.
// 4. If this block ends with a conditional branch followed by an unconditional branch, it returns the 'true' destination in TBB,
//    the 'false' destination in FBB, and a list of operands that evaluate the condition.  These operands can be passed to other
//    TargetInstrInfo methods to create new branches.
//
// Note that removeBranch and insertBranch must be implemented to support cases where this method returns success.
//
// If AllowModify is true, then this routine is allowed to modify the basic block (e.g. delete instructions after the unconditional
// branch).
//
// The CFG information in MBB.Predecessors and MBB.Successors must be valid before calling this function.
bool VIAMPPInstrInfo::analyzeBranch(MachineBasicBlock &MBB, MachineBasicBlock *&TBB, MachineBasicBlock *&FBB,
		                            SmallVectorImpl<MachineOperand> &Cond, bool AllowModify) const {
//	outs().changeColor(raw_ostream::BLUE,0);
//	outs() << "VIAMPPInstrInfo::analyzeBranch\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	// Start from the bottom of the block and work up, examining the terminator instructions.
	MachineBasicBlock::iterator I = MBB.end();
	while (I != MBB.begin()) {
		--I;

		// Skip debug values
		if (I->isDebugValue())
			continue;

		// Working from the bottom, when we see a non-terminator instruction, we're done.
		if (!isUnpredicatedTerminator(*I))
			break;

		// A terminator that isn't a branch can't easily be handled by this analysis.
		if (!I->isBranch())
			return true;

		// Handle unconditional branches.
		if (I->getOpcode() == VIAMPP::JD) {
//			outs().changeColor(raw_ostream::BLUE,0);
//			outs() << "----VIAMPPInstrInfo::analyzeBranch: in unconditional branches\n";
//			outs().changeColor(raw_ostream::WHITE,0);
			if (!AllowModify) {
				TBB = I->getOperand(0).getMBB();
				continue;
			}

			// If the block has any instructions after a JD, delete them.
			while (std::next(I) != MBB.end())
				std::next(I)->eraseFromParent();
			Cond.clear();
			FBB = nullptr;

			// Delete the JI if it's equivalent to a fall-through.
			if (MBB.isLayoutSuccessor(I->getOperand(0).getMBB())) {
				TBB = nullptr;
				I->eraseFromParent();
				I = MBB.end();
				continue;
			}

			// TBB is used to indicate the unconditional destination.
			TBB = I->getOperand(0).getMBB();
			continue;
		}

		// We don't handle conditional branches
		return true;
	} // while end

	// Return false indicating branch successfully analyzed.
	return false;
}

// Insert branch code into the end of the specified MachineBasicBlock. The operands to this method are the same as those returned
// by analyzeBranch. This is only invoked in cases where analyzeBranch returns success. It returns the number of instructions
// inserted. If BytesAdded is non-null, report the change in code size from the added instructions.
//
// It is also invoked by tail merging to add unconditional branches in cases where analyzeBranch doesn't apply because there was no
// original branch to analyze. At least this much must be implemented, else tail merging needs to be disabled.
//
// The CFG information in MBB.Predecessors and MBB.Successors must be valid before calling this function.
unsigned VIAMPPInstrInfo::insertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB, MachineBasicBlock *FBB,
									ArrayRef<MachineOperand> Cond, const DebugLoc &DL, int *BytesAdded) const {
//	outs().changeColor(raw_ostream::BLUE,0);
//	outs() << "VIAMPPInstrInfo::insertBranch\n";
//	outs().changeColor(raw_ostream::WHITE,0);


	assert(!BytesAdded && "code size not handled");

	// Shouldn't be a fall through.
	assert(TBB && "insertBranch must not be told to insert a fallthrough");

	// Unconditional branch
	if (Cond.empty()) {
		assert(!FBB && "Unconditional branch with multiple successors!");
		BuildMI(&MBB, DL, get(VIAMPP::JD)).addMBB(TBB);
		return 1;
	}

	llvm_unreachable("Unexpected conditional branch");
	return 0;
}

// Remove the branching code at the end of the specific MBB. This is only invoked in cases where analyzeBranch returns success.
// It returns the number of instructions that were removed. If BytesRemoved is non-null, report the change in code size from the
// removed instructions.
unsigned VIAMPPInstrInfo::removeBranch(MachineBasicBlock &MBB, int *BytesRemoved) const {
//	outs().changeColor(raw_ostream::BLUE,0);
//	outs() << "VIAMPPInstrInfo::removeBranch\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	assert(!BytesRemoved && "code size not handled");

	MachineBasicBlock::iterator I = MBB.end();
	unsigned Count = 0;

	while (I != MBB.begin()) {
		--I;
		if (I->isDebugInstr())
			continue;
		if (I->getOpcode() != VIAMPP::JD)
			break;
		// Remove the branch.
		I->eraseFromParent();
		I = MBB.end();
		++Count;
	}

	return Count;
}

bool VIAMPPInstrInfo::expandPostRAPseudo(MachineInstr &MI) const {
//	outs().changeColor(raw_ostream::BLUE,1);
//	outs() << "VIAMPPInstrInfo::expandPostRAPseudo\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	switch (MI.getOpcode()) {
	default:
		return false;

	//   Expand CallReg
	case VIAMPP::CallReg: {
		outs().changeColor(raw_ostream::BLUE,1);
		outs() << "----VIAMPPInstrInfo::emitCallReg\n";
		outs().changeColor(raw_ostream::WHITE,0);

		DebugLoc dl = MI.getDebugLoc();
		MachineBasicBlock &MBB = *MI.getParent();
	    const MachineOperand &MO = MI.getOperand(0);
		const GlobalValue *CallDest = MO.getGlobal();

		// add LinkReg, PC, #2
		// add PC, PC, "src2"
		BuildMI(MBB, MI, dl, get(VIAMPP::ADDasi)).addReg(VIAMPP::LinkReg).addReg(VIAMPP::PC).addImm(2);
		BuildMI(MBB, MI, dl, get(VIAMPP::ADDssa)).addReg(VIAMPP::PC).addReg(VIAMPP::PC)
				                                 .addGlobalAddress(CallDest, 0, MO.getTargetFlags());

		MBB.erase(MI);
		return true;
	}

	//   Expand CallIndReg
	case VIAMPP::CallIndReg: {
		outs().changeColor(raw_ostream::BLUE,1);
		outs() << "----VIAMPPInstrInfo::emitCallIndReg\n";
		outs().changeColor(raw_ostream::WHITE,0);

		DebugLoc dl = MI.getDebugLoc();
		MachineBasicBlock &MBB = *MI.getParent();
		const MachineOperand &MO = MI.getOperand(0);
		const GlobalValue *CallDest = MO.getGlobal();

		// add LinkReg, PC, #2
		// add PC, PC, "(src2)"
		BuildMI(MBB, MI, dl, get(VIAMPP::ADDasi)).addReg(VIAMPP::LinkReg).addReg(VIAMPP::PC).addImm(2);
		BuildMI(MBB, MI, dl, get(VIAMPP::ADDssr)).addReg(VIAMPP::PC).addReg(VIAMPP::PC)
												 .addGlobalAddress(CallDest, 0, MO.getTargetFlags());

		MBB.erase(MI);
		return true;
	}

	//   Expand Call
	case VIAMPP::Call: {
//		outs().changeColor(raw_ostream::BLUE,1);
//		outs() << "----VIAMPPInstrInfo::emitCall\n";
//		outs().changeColor(raw_ostream::WHITE,0);

		// add LinkReg, PC, #2
		// add PC, PC, "imm32"

		DebugLoc dl = MI.getDebugLoc();
		MachineBasicBlock &MBB = *MI.getParent();
		const MachineOperand &MO = MI.getOperand(0);
		switch (MO.getType()) {
		default:
//			outs() << "------Unhandled MachineOperand\n";
			break;
		case MachineOperand::MO_Immediate:
			outs() << "------MO_Immediate\n";
//			BuildMI(MBB, MI, dl, get(VIAMPP::ADDasi)).addReg(VIAMPP::LinkReg).addReg(VIAMPP::PC).addImm(2);		// LinkReg
//			Build (...)
			break;
		case MachineOperand::MO_GlobalAddress: {
//			outs() << "------MO_GlobalAddress\n";
			const GlobalValue *GV = MO.getGlobal();
			unsigned TF = MO.getTargetFlags();
//			outs() << "        MO.getOffset: " << MO.getOffset() << "\n";
//			outs() << "        MO.getTargetFlags: " << TF << "\n";
			BuildMI(MBB, MI, dl, get(VIAMPP::ADDasi)).addReg(VIAMPP::LinkReg).addReg(VIAMPP::PC).addImm(2);		// update LinkReg with return address
			BuildMI(MBB, MI, dl, get(VIAMPP::ADDsd2i), VIAMPP::PC).addReg(VIAMPP::PC).addGlobalAddress(GV, MO.getOffset(), TF);
			break;
		}
		case MachineOperand::MO_ExternalSymbol:
			outs() << "------MO_ExternalSymbol\n";
//			BuildMI(MBB, MI, dl, get(VIAMPP::ADDasi)).addReg(VIAMPP::LinkReg).addReg(VIAMPP::PC).addImm(2);		// LinkReg
			//			Build (...)
			break;
		}
		MBB.erase(MI);
//		outs().changeColor(raw_ostream::WHITE,0);
		return true;

	}
/*
	// Expand MOV64
	case VIAMPP::MOV64: {
		outs().changeColor(raw_ostream::BLUE,1);
		outs() << "----VIAMPPInstrInfo::MOV64\n";
		outs().changeColor(raw_ostream::WHITE,0);

		DebugLoc dl = MI.getDebugLoc();
		MachineBasicBlock &MBB = *MI.getParent();
		const MachineOperand &MO = MI.getOperand(0);

//		BuildMI(MBB, MI, dl, get(VIAMPP::MOVa2i)).addReg(MO.getReg()).addImm(MO.getImm() & 0xFFFFFFFFUL);
//		BuildMI(MBB, MI, dl, get(VIAMPP::MOVHIa2i)).addReg(MO.getReg()).addImm(MO.getImm() >> 32);

		BuildMI(MBB, MI, dl, get(VIAMPP::SLEEP));		// Test

		MBB.erase(MI);
		outs().changeColor(raw_ostream::WHITE,0);
		return true;
	}
*/
	}
}

