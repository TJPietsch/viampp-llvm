//===-- VIAMPPFrameLowering.cpp - VIAMPP Frame Information -------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------------------------------------------------------===//
//
// This file contains the VIAMPP implementation of TargetFrameLowering class.
// see "llvm/include/CodeGen/TargetFrameLowering.h" for information
//
//===-------------------------------------------------------------------------------------------------------------------------===//

#include "VIAMPPFrameLowering.h"
//#include "VIAMPP.h"
#include "VIAMPPInstrInfo.h"
//#include "VIAMPPSubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
//#include "llvm/CodeGen/RegisterScavenging.h"
//#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

//===-------------------------------------------------------------------------------------------------------------------------===//
// VIAMPPFrameLowering
//===-------------------------------------------------------------------------------------------------------------------------===//

// Return true if the specified function should have a dedicated frame pointer register. For most targets this is true only if the
// function has variable sized allocas or if frame pointer elimination is disabled.
bool VIAMPPFrameLowering::hasFP(const MachineFunction &MF) const {
//	outs().changeColor(raw_ostream::MAGENTA,0);
//	outs() << "VIAMPPFrameLowering::hasFP\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	const MachineFrameInfo &MFI = MF.getFrameInfo();
//	const TargetRegisterInfo *TRI = MF.getSubtarget().getRegisterInfo();

	return (MF.getTarget().Options.DisableFramePointerElim(MF) || MFI.hasVarSizedObjects()); // || MFI.isFrameAddressTaken() || TRI->needsStackRealignment(MF));
}

// determineFrameLayout - Determine the size of the frame and maximum call frame size.
void VIAMPPFrameLowering::determineFrameLayout(MachineFunction &MF) const {
//	outs().changeColor(raw_ostream::MAGENTA,0);
//	outs() << "VIAMPPFrameLowering::determineFrameLayout\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	MachineFrameInfo &MFI = MF.getFrameInfo();
	const TargetRegisterInfo *TRI = MF.getSubtarget().getRegisterInfo();

	// Get the number of bytes to allocate from the FrameInfo.
	unsigned FrameSize = MFI.getStackSize();

	// Get the alignment.
	unsigned StackAlign = TRI->needsStackRealignment(MF) ? MFI.getMaxAlign().value() :
		                                               MF.getSubtarget().getFrameLowering()->getStackAlignment();

	// Get the maximum call frame size of all the calls.
	unsigned maxCallFrameSize = MFI.getMaxCallFrameSize();

	// If we have dynamic alloca then maxCallFrameSize needs to be aligned so that allocations will be aligned.
	if (MFI.hasVarSizedObjects())
		maxCallFrameSize = alignTo(maxCallFrameSize, StackAlign);

	// Update maximum call frame size.
	MFI.setMaxCallFrameSize(maxCallFrameSize);

   // Include call frame size in total.
	if (!(hasReservedCallFrame(MF) && MFI.adjustsStack()))
		FrameSize += maxCallFrameSize;

	// Make sure the frame is aligned.
	FrameSize = alignTo(FrameSize, StackAlign);

	// Update frame info.
	MFI.setStackSize(FrameSize);
}

// Iterates through each basic block in a machine function and replaces ADJDYNALLOC pseudo instructions with a VIAMPP::ADDaai with 
// the maximum call frame size as the immediate.
void VIAMPPFrameLowering::replaceAdjDynAllocPseudo(MachineFunction &MF) const {
//	outs().changeColor(raw_ostream::MAGENTA,0);
//	outs() << "VIAMPPFrameLowering::replaceAdjDynAllocPseudo\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();
	unsigned MaxCallFrameSize = MF.getFrameInfo().getMaxCallFrameSize();

	for (MachineFunction::iterator MBB = MF.begin(), E = MF.end(); MBB != E; ++MBB) {
		MachineBasicBlock::iterator MBBI = MBB->begin();
		while (MBBI != MBB->end()) {
			MachineInstr &MI = *MBBI++;
			if (MI.getOpcode() == VIAMPP::ADJDYNALLOC) {
				DebugLoc DL = MI.getDebugLoc();
				unsigned Dst = MI.getOperand(0).getReg();
				unsigned Src = MI.getOperand(1).getReg();

				BuildMI(*MBB, MI, DL, TII->get(VIAMPP::ADDaai), Dst).addReg(Src).addImm(MaxCallFrameSize);
				MI.eraseFromParent();
			}
		}
	}
}

// Inserts prologue code into the function
void VIAMPPFrameLowering::emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const {
//	outs().changeColor(raw_ostream::MAGENTA,0);
//	outs() << "VIAMPPFrameLowering::emitProlog\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	assert(&MF.front() == &MBB && "Shrink-wrapping not yet supported");
	const MachineFrameInfo &MFI = MF.getFrameInfo();
	const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();
	const VIAMPPRegisterInfo *TRI = static_cast<const VIAMPPRegisterInfo*>(MF.getSubtarget().getRegisterInfo());
	MachineBasicBlock::iterator MBBI = MBB.begin();

	DebugLoc DL = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();
	bool HasRA = MFI.adjustsStack();    //RA = Return Address

	// Determine the correct frame layout
	determineFrameLayout(MF);

	// Get the number of bytes to allocate from the FrameInfo.
	/* LLVM counts stack offsets in bytes. VIAMPP is only 64 bit oriented. Therefore we need to divide the StackSize by 8. */
	unsigned StackSize = MFI.getStackSize()/8;
//	outs().changeColor(raw_ostream::MAGENTA,0);
//	outs() << "--VIAMPPFrameLowering::emitProlog: StackSize: " << StackSize << "\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	// No need to allocate space on the stack.
	if (StackSize == 0 && !HasRA)
		return;

	int Offset = -1;

	if (HasRA) {
//		outs().changeColor(raw_ostream::MAGENTA,0);
//		outs() << "--VIAMPPFrameLowering::emitProlog: if (HasRA): Offset: " << Offset << "\n";
//		outs().changeColor(raw_ostream::MAGENTA,0);
		// Save return address onto stack
		BuildMI(MBB, MBBI, DL, TII->get(VIAMPP::MOVoffaoa)).addReg(VIAMPP::StackPtr).addImm(Offset).addReg(VIAMPP::LinkReg);		// StackPtr, LinkReg

		Offset -= 1;
	}

	if (hasFP(MF)) {
		outs().changeColor(raw_ostream::MAGENTA,0);
		outs() << "--VIAMPPFrameLowering::emitProlog: if (hasFP(MF)): Offset: " << Offset << "\n";
		outs().changeColor(raw_ostream::MAGENTA,0);
		// Save frame pointer onto stack
		BuildMI(MBB, MBBI, DL, TII->get(VIAMPP::MOVoffaoa)).addReg(VIAMPP::FramePtr).addImm(Offset).addReg(VIAMPP::StackPtr);	// FramePtr, StackPtr

		Offset -= 1;

		// In case of a base pointer, it need to be saved here before we start modifying it below.
		if (TRI->hasBasePointer(MF)) {
			BuildMI(MBB, MBBI, DL, TII->get(VIAMPP::MOVoffaoa)).addReg(TRI->getBaseRegister()).addImm(Offset).addReg(VIAMPP::StackPtr);	//StackPtr
		}

		// Set frame pointer to stack pointer
		BuildMI(MBB, MBBI, DL, TII->get(VIAMPP::MOVaxa), VIAMPP::FramePtr).addReg(VIAMPP::StackPtr);		// FramePtr, StackPtr
	} // end if(hasFP)

//	outs().changeColor(raw_ostream::MAGENTA,0);
//	outs() << "--VIAMPPFrameLowering::emitProlog: adjust Stack: StackSize: " << StackSize << "\n";
//	outs().changeColor(raw_ostream::WHITE,0);
	if (isInt<32>(StackSize)) {
		if (StackSize) {
        // Adjust stack
			BuildMI(MBB, MBBI, DL, TII->get(VIAMPP::SUBad2i), VIAMPP::StackPtr).addReg(VIAMPP::StackPtr).addImm(StackSize); // StackPtr
		}
	} else {
		outs().changeColor(raw_ostream::MAGENTA,0);
		outs() << "--VIAMPPFrameLowering::emitProlog: adjust Stack: Stack Size > isInt<32>\n";
		outs().changeColor(raw_ostream::WHITE,0);
		// TODO: Stack Size > int<32> --------------------------*******************************************************************
	}

	// If a base pointer is needed, set it up here. Any variable sized objects will be located after this, so local objects can be
	// adressed with the base pointer.
	if (TRI->hasBasePointer(MF)) {
//		outs().changeColor(raw_ostream::MAGENTA,0);
//		outs() << "--VIAMPPFrameLowering::emitProlog: TRI->hasBasePointer\n";
//		outs().changeColor(raw_ostream::WHITE,0);
		BuildMI(MBB, MBBI, DL, TII->get(VIAMPP::MOVaxa), TRI->getBaseRegister()).addReg(VIAMPP::StackPtr);		// StackPtr
	}
	if (MFI.hasVarSizedObjects()) {
//		outs().changeColor(raw_ostream::MAGENTA,0);
//		outs() << "--VIAMPPFrameLowering::emitProlog: VarSizedObjects\n";
//		outs().changeColor(raw_ostream::WHITE,0);
		replaceAdjDynAllocPseudo(MF);
	}
}

// Inserts epilogue code into the function
void VIAMPPFrameLowering::emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const {
//	outs().changeColor(raw_ostream::MAGENTA,0);
//	outs() << "VIAMPPFrameLowering::emitEpiloug\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
	const MachineFrameInfo &MFI = MF.getFrameInfo();
	const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();
	const VIAMPPRegisterInfo *TRI = static_cast<const VIAMPPRegisterInfo*>(MF.getSubtarget().getRegisterInfo());
	bool HasRA = MFI.adjustsStack();      // RA = Return Address
	DebugLoc DL = MBBI->getDebugLoc();

	int FPOffset = HasRA ? -2 : -1;
	int BPOffset = FPOffset - 1;
	int RAOffset = -1;

	// Get the number of bytes from FrameInfo
	/* LLVM counts stack offsets in bytes. VIAMPP is only 64 bit oriented. Therefore we need to divide the offset by 8. */
	int StackSize = (int) MFI.getStackSize()/8;

	if (hasFP(MF)) {
//		outs().changeColor(raw_ostream::MAGENTA,0);
//		outs() <<"VIAMPPFrameLowering::emitEpilogue: has Frame Pointer:" << hasFP(MF) << "\n";
//		outs().changeColor(raw_ostream::WHITE,0);
		// Set stack pointer to frame pointer
 		BuildMI(MBB, MBBI, DL, TII->get(VIAMPP::MOVaxa), VIAMPP::StackPtr).addReg(VIAMPP::FramePtr);
 		// Load previous frame pointer from stack
		BuildMI(MBB, MBBI, DL, TII->get(VIAMPP::MOVoffaao), VIAMPP::FramePtr).addReg(VIAMPP::StackPtr).addImm(FPOffset);
	} else {
//		outs().changeColor(raw_ostream::MAGENTA,0);
//		outs() <<"VIAMPPFrameLowering::emitEpilogue: has no Frame Pointer\n";
//		outs().changeColor(raw_ostream::WHITE,0);
		if (isInt<32>(StackSize)) {
			if (StackSize) {
				BuildMI(MBB, MBBI, DL, TII->get(VIAMPP::ADDad2i), VIAMPP::StackPtr).addReg(VIAMPP::StackPtr).addImm(StackSize);
			}
		} else {
			// TODO: Stack Size > int<32> --------------------------***********************************************************
		} // end if int<16>)
	} // end if hasFP

   // l.lwz basereg, stack_loc(r1)
	if (TRI->hasBasePointer(MF)) {
		outs().changeColor(raw_ostream::MAGENTA,0);
		outs() <<"VIAMPPFrameLowering::emitEpilogue: TRI->hasBasePointer(MF):" << TRI->hasBasePointer(MF) << " BPOffset: " << BPOffset << "\n";
		outs().changeColor(raw_ostream::WHITE,0);
		BuildMI(MBB, MBBI, DL, TII->get(VIAMPP::MOVoffaao), TRI->getBaseRegister()).addReg(VIAMPP::StackPtr).addImm(BPOffset);
	}

   // l.lwz r9, stack_loc(r1)
	if (HasRA) {
//		outs().changeColor(raw_ostream::MAGENTA,0);
//		outs() <<"VIAMPPFrameLowering::emitEpilogue: HasRA\n";
//		outs().changeColor(raw_ostream::WHITE,0);
		BuildMI(MBB, MBBI, DL, TII->get(VIAMPP::MOVoffaao), VIAMPP::LinkReg).addReg(VIAMPP::StackPtr).addImm(RAOffset);
	}
}

// This method is called during prolog/epilog code insertion to eliminate call frame setup and destroy pseudo instructions (but
// only if the Target is using them).  It is responsible for eliminating these instructions, replacing them with concrete
// instructions.  This method need only be implemented if using call frame setup/destroy pseudo instructions. Returns an iterator
// pointing to the instruction after the replaced one. We eliminate the ADJCALLSTACKDOWN, ADJCALLSTACKUP pseudo instructions.
MachineBasicBlock::iterator VIAMPPFrameLowering::eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
		                                                                    MachineBasicBlock::iterator MI) const {
//	outs().changeColor(raw_ostream::MAGENTA,0);
//	outs() << "VIAMPPFrameLowering::eliminateCallFramePseudoInstr\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	return MBB.erase(MI);
}

// This method determines which of the registers reported by TargetRegisterInfo::getCalleeSavedRegs() should actually get saved.
// The default implementation checks populates the SavedRegs bitset with all registers which are modified in the function,
// targets may override this function to save additional registers. This method also sets up the register scavenger ensuring there
// is a free register or a frameindex available. This method should not be called by any passes outside of PEI, because it may
// change state passed in by MF and RS. The preferred interface outside PEI is getCalleeSaves.
void VIAMPPFrameLowering::determineCalleeSaves(MachineFunction &MF, BitVector &SavedRegs, RegScavenger *RS) const {
//	outs().changeColor(raw_ostream::MAGENTA,0);
//	outs() << "VIAMPPFrameLowering::determineCalleeSaves\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	TargetFrameLowering::determineCalleeSaves(MF, SavedRegs, RS);
	MachineFrameInfo &MFI = MF.getFrameInfo();
	const VIAMPPRegisterInfo *TRI = static_cast<const VIAMPPRegisterInfo*>(MF.getSubtarget().getRegisterInfo());

	int Offset = -8;

	if (MFI.adjustsStack()) {
		// Reserve 8 bytes for LinkReg
		MFI.CreateFixedObject(8, Offset, true);
		// Mark unused since we will save it manually in the prologue
		Offset -= 8;
	}

	if (hasFP(MF)) {
		MFI.CreateFixedObject(8, Offset, true);
		Offset -= 8;
	}

	if (TRI->hasBasePointer(MF)) {
		MFI.CreateFixedObject(8, Offset, true);
		SavedRegs.set(TRI->getBaseRegister());
	}
}
