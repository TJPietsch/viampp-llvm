//===--VIAMPPOptMachine.cpp - An optimization pass for VIAMPPAMPP ------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------------------------------------------------------===//
//
// This file defines an optimzer for machine instruction for the VIAMPP target. It runs just before asm output.
//
//===-------------------------------------------------------------------------------------------------------------------------===//

#define DEBUG_TYPE "mi-opt"

#include "VIAMPP.h"
#include "VIAMPPInstrInfo.h"
#include "llvm/Pass.h"
#include "llvm/MC/MCInst.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/Support/raw_ostream.h"
#include "VIAMPPMachineFunctionInfo.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

namespace {
static cl::opt<bool>MoveOpt("moveopt-mode", cl::desc("Replace all MOVoff instructions with 0 offset with MOV counterparts."));

class VIAMPPOptMachine : public MachineFunctionPass {
public:
	static char ID;

	VIAMPPOptMachine() : MachineFunctionPass(ID) {}

	StringRef getPassName() const override { return "VIAMPP Late Optimization Pass"; }
	
	bool runOnMachineFunction(MachineFunction &MF) override;
	unsigned long long getMaxReg() { return maxReg; }

private:
	unsigned long long maxReg = 0;
};
}

bool VIAMPPOptMachine::runOnMachineFunction(MachineFunction &MF) {
//	outs().changeColor(raw_ostream::YELLOW,0);
//	outs() << "---MOVoffAnalysisPass::runOnMachineFunction\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();
	VIAMPPMachineFunctionInfo *VIAMPPMFI = MF.getInfo<VIAMPPMachineFunctionInfo>();

	for (MachineFunction::iterator MBB = MF.begin(), E = MF.end(); MBB != E; ++MBB) {
		MachineBasicBlock::iterator MBBI = MBB->begin();
		while (MBBI != MBB->end()) {
			MachineInstr &MI = *MBBI++;

			// look for highest register number
			for (unsigned i = 0, e = MI.getNumOperands(); i != e; ++i) {
			    if (MI.getOperand(i).isReg()) {
					if (MI.getOperand(i).getReg() > maxReg) maxReg = MI.getOperand(i).getReg();
			    }
			}

			// do the MOVoff optimization
			if (MoveOpt) {
				switch (MI.getOpcode()) {
				case VIAMPP::MOVoffaao:		// -> MOVaxr
					if (MI.getOperand(2).getImm() == 0) {
						BuildMI(*MBB, MI, MI.getDebugLoc(), TII->get(VIAMPP::MOVaxr)).addReg(MI.getOperand(0).getReg()).addReg(MI.getOperand(1).getReg());
			    		MI.eraseFromParent();
					}
					break;

				case VIAMPP::MOVoffaoa:		// -> MOVrxa
					if (MI.getOperand(1).getImm() == 0) {
						BuildMI(*MBB, MI, MI.getDebugLoc(), TII->get(VIAMPP::MOVrxa)).addReg(MI.getOperand(0).getReg()).addReg(MI.getOperand(2).getReg());
			    		MI.eraseFromParent();
					}
					break;

				case VIAMPP::MOVoffaor:		// -> MOVrxr
					if (MI.getOperand(1).getImm() == 0) {
						BuildMI(*MBB, MI, MI.getDebugLoc(), TII->get(VIAMPP::MOVrxr)).addReg(MI.getOperand(0).getReg()).addReg(MI.getOperand(2).getReg());
			    		MI.eraseFromParent();
					}
					break;

				case VIAMPP::MOVoffrao:		// -> MOVrxr
					if (MI.getOperand(2).getImm() == 0) {
						BuildMI(*MBB, MI, MI.getDebugLoc(), TII->get(VIAMPP::MOVrxr)).addReg(MI.getOperand(0).getReg()).addReg(MI.getOperand(1).getReg());
			    		MI.eraseFromParent();
					}
					break;

				case VIAMPP::MOVoffsao:		// -> MOVsxr
					if (MI.getOperand(2).getImm() == 0) {
						BuildMI(*MBB, MI, MI.getDebugLoc(), TII->get(VIAMPP::MOVsxr)).addReg(MI.getOperand(0).getReg()).addReg(MI.getOperand(1).getReg());
			    		MI.eraseFromParent();
					}
					break;

				case VIAMPP::MOVoffaoi:		// -> MOVrxi
					if (MI.getOperand(1).getImm() == 0) {
						BuildMI(*MBB, MI, MI.getDebugLoc(), TII->get(VIAMPP::MOVrxi)).addReg(MI.getOperand(0).getReg()).addReg(MI.getOperand(2).getReg());
			    		MI.eraseFromParent();
					}
					break;

				}
			}	// end MoveOpt
		}
	}

//	outs() << "Pass done!!!\n";
	maxReg -= 21;
//	outs() << "Max register number: " << maxReg << "\n";
	VIAMPPMFI->setMaxUsedRegs(maxReg);

	return false;
}

namespace llvm {
void initializeVIAMPPOptMachinePass(PassRegistry &Registry);
FunctionPass *createVIAMPPOptMachinePass() { return new VIAMPPOptMachine(); }
}

char VIAMPPOptMachine::ID = 0;
static RegisterPass<VIAMPPOptMachine> X("mi-opt", "MOVoff optimization Pass");

