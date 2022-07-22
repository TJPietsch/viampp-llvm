//===-- VIAMPPAsmPrinter.cpp - VIAMPP LLVM assembly writer -------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to the VIAMPP assembly language.
//
//===-------------------------------------------------------------------------------------------------------------------------===//

#define DEBUG_TYPE "VIAMPP-asm-printer"

#include "VIAMPP.h"
#include "VIAMPPInstrInfo.h"
#include "VIAMPPMCInstLower.h"
#include "VIAMPPSubtarget.h"
#include "VIAMPPTargetMachine.h"
#include "VIAMPPMachineFunctionInfo.h"
#include "MCTargetDesc/VIAMPPInstPrinter.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineJumpTableInfo.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Mangler.h"
#include "llvm/IR/Module.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include "llvm/Target/TargetMachine.h"

using namespace llvm;

namespace {

class VIAMPPAsmPrinter final : public AsmPrinter {
	VIAMPPMCInstLower MCInstLowering;

public:
	explicit VIAMPPAsmPrinter(TargetMachine &TM, std::unique_ptr<MCStreamer> Streamer)
                          : AsmPrinter(TM, std::move(Streamer)), MCInstLowering(OutContext, *this) {}

	StringRef getPassName() const override { return "VIAMPP Assembly Printer"; }

	// we add elf section here	// Thomas --------------- experimental ----------------
	bool doFinalization(Module &M) override;

	// Targets should implement this to emit instructions.
	void emitInstruction(const MachineInstr *MI) override;

	// Targets can override this to emit stuff before the first basic block in the function.
	void emitFunctionBodyStart() override;

	// Print to the current output stream assembly representations of the constants in the constant pool MCP. This is used to print
	// out constants which have been "spilled to memory" by the code generator.
	void emitConstantPool() override;

	// Print operand for inline assembly
	//
	// Print the specified operand of MI, an INLINEASM instruction, using the specified assembler variant.  Targets should override
	// this to format as appropriate.  This method can return true if the operand is erroneous.
	bool PrintAsmOperand(const MachineInstr *MI, unsigned OpNo, const char *ExtraCode, raw_ostream &O) override;

	// Print the specified operand of MI, an INLINEASM instruction, using the specified assembler variant as an address. Targets
	// should override this to format as appropriate.  This method can return true if the operand is erroneous.
	bool PrintAsmMemoryOperand(const MachineInstr *MI, unsigned OpNum, const char *ExtraCode, raw_ostream &O) override;

//  void EmitFunctionEntryLabel();
};
} // end of anonymous namespace


void VIAMPPAsmPrinter::emitInstruction(const MachineInstr *MI) {
	outs() << "VIAMPPAsmPrinter::EmitInstruction\n";

	MachineBasicBlock::const_instr_iterator I = MI->getIterator();
	MachineBasicBlock::const_instr_iterator E = MI->getParent()->instr_end();

	do {
		MCInst TmpInst;
		MCInstLowering.Lower(MI, TmpInst);
//TmpInst.dump();
		EmitToStreamer(*OutStreamer, TmpInst);
	} while ((I != E) && I->isInsideBundle());
}

void VIAMPPAsmPrinter::emitFunctionBodyStart() {
	outs() << "VIAMPPAsmPrinter::EmitFunctionBodyStart\n";
	MCInstLowering.Initialize(&MF->getContext());
}

void VIAMPPAsmPrinter::emitConstantPool() {
	outs() << "VIAMPPAsmPrinter::EmitConstantPool\n";

	const MachineConstantPool *MCP = MF->getConstantPool();
	const std::vector<MachineConstantPoolEntry> &CP = MCP->getConstants();

	if (CP.empty())
		return;

	AsmPrinter::emitConstantPool();
}

// Print operand for inline assembly
bool VIAMPPAsmPrinter::PrintAsmOperand(const MachineInstr *MI, unsigned OpNo, const char *ExtraCode, raw_ostream &O) {
	outs() << "VIAMPPAsmPrinter::PrintAsmOperand\n";

	// Does this asm operand have a single letter operand modifier?
	if (ExtraCode && ExtraCode[0])
		return AsmPrinter::PrintAsmOperand(MI, OpNo, ExtraCode, O);

	const MachineOperand &MO = MI->getOperand(OpNo);

	  switch (MO.getType()) {
	  case MachineOperand::MO_Register:
	    O << VIAMPPInstPrinter::getRegisterName(MO.getReg());
		return false;
	    break;

	  case MachineOperand::MO_Immediate:
	    O << MO.getImm();
		return false;
	    break;

	  case MachineOperand::MO_MachineBasicBlock:
	    O << *MO.getMBB()->getSymbol();
		return false;
	    break;

	  case MachineOperand::MO_GlobalAddress:
	    O << *getSymbol(MO.getGlobal());
		return false;
	    break;

	  case MachineOperand::MO_BlockAddress: {
	    MCSymbol *BA = GetBlockAddressSymbol(MO.getBlockAddress());
	    O << BA->getName();
		return false;
	    break;
	  }

	  case MachineOperand::MO_ExternalSymbol:
	    O << *GetExternalSymbolSymbol(MO.getSymbolName());
		return false;
	    break;

	  case MachineOperand::MO_JumpTableIndex:
	    O << MAI->getPrivateGlobalPrefix() << "JTI" << getFunctionNumber() << '_' << MO.getIndex();
		return false;
	    break;

	  case MachineOperand::MO_ConstantPoolIndex:
	    O << MAI->getPrivateGlobalPrefix() << "CPI" << getFunctionNumber() << '_' << MO.getIndex();
		return false;
//	    return;

	  default:
	    llvm_unreachable("<unknown operand type>");
	  }

	  return true;		//maybe false
}

bool VIAMPPAsmPrinter::PrintAsmMemoryOperand(const MachineInstr *MI, unsigned OpNum, const char *ExtraCode, raw_ostream &O) {
	outs() << "VIAMPPAsmPrinter::PrintAsmMemoryOperand";

	const MachineOperand &MO = MI->getOperand(OpNum);
	assert(MO.isReg() && "unexpected inline asm memory operand\n");
	O << "(" << VIAMPPInstPrinter::getRegisterName(MO.getReg()) << ")";

	return false;
}

bool VIAMPPAsmPrinter::doFinalization(Module &M) {
	VIAMPPMachineFunctionInfo *VIAMPPMFI = MF->getInfo<VIAMPPMachineFunctionInfo>();

    OutStreamer->SwitchSection(OutContext.getObjectFileInfo()->getVIAMPPRegSection());
    OutStreamer->emitInt64(VIAMPPMFI->getMaxUsedRegs());

	return AsmPrinter::doFinalization(M);
}


//void VIAMPPAsmPrinter::EmitFunctionEntryLabel() {
//  OutStreamer->emitLabel(CurrentFnSym);
//}


// Force static initialization.
extern "C" void LLVMInitializeVIAMPPAsmPrinter() {
	RegisterAsmPrinter<VIAMPPAsmPrinter> X(TheVIAMPPTarget);
}
