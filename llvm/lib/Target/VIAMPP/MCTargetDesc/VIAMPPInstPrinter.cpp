//===-- VIAMPPInstPrinter.cpp - Convert VIAMPP MCInst to asm syntax ----------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------------------------------------------------------===//
//
// This class prints an VIAMPP MCInst to a .s file.
//
//===-------------------------------------------------------------------------------------------------------------------------===//

#include "VIAMPP.h"
#include "VIAMPPInstPrinter.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"

using namespace llvm;

#define DEBUG_TYPE "asm-printer"

// Include the auto-generated portion of the assembly writer.
//#define PRINT_ALIAS_INSTR
#include "VIAMPPGenAsmWriter.inc"

void VIAMPPInstPrinter::printInst(const MCInst *MI, uint64_t Address, StringRef Annot, const MCSubtargetInfo &STI, raw_ostream &O) {
//	outs().changeColor(raw_ostream::GREEN,0);
//	outs() << "VIAMPPInstPrinter::printInst\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	printInstruction(MI, Address, O);
	printAnnotation(O, Annot);
}

void VIAMPPInstPrinter::printRegName(raw_ostream &O, unsigned RegNo) const {
//	outs().changeColor(raw_ostream::GREEN,0);
//	outs() << "VIAMPPInstPrinter::printRegName: " << StringRef(getRegisterName(RegNo)) << "\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	O << StringRef(getRegisterName(RegNo));
}

static void printExpr(const MCExpr *Expr, raw_ostream &O, const MCAsmInfo &MAI) {
	const MCSymbolRefExpr *SRE;

	const MCBinaryExpr *BE = dyn_cast<MCBinaryExpr>(Expr);
	if (BE)
		SRE = dyn_cast<MCSymbolRefExpr>(BE->getLHS());
	else
	SRE = dyn_cast<MCSymbolRefExpr>(Expr);
	assert(SRE && "Unexpected MCExpr type.");

	MCSymbolRefExpr::VariantKind Kind = SRE->getKind();

	if(Kind != MCSymbolRefExpr::VK_None)
		O << MCSymbolRefExpr::getVariantKindName(Kind);
	else
		Expr->print(O, &MAI);
}

void VIAMPPInstPrinter::printOperand(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
//	outs().changeColor(raw_ostream::GREEN,0);
//	outs() << "VIAMPPInstPrinter::printOperand: OpNo: " << OpNo << "\n";
//	MI->dump();
//	outs().changeColor(raw_ostream::WHITE,0);

	const MCOperand &MOp = MI->getOperand(OpNo);

	if (MOp.isReg()) {
//outs() << "----VIAMPPInstPrinter::printOperand: isReg\n";
		printRegName(O, MOp.getReg());
		return;
	}

	if (MOp.isImm()) {
//outs() << "----VIAMPPInstPrinter::printOperand: isImm\n";
		O << "#" << (int)MOp.getImm();		// check "formatted print" in "llvm/Support/FormattedStream" (see BPF, RISCV)
		return;
	}

	if (MOp.isExpr()) {
outs() << "----VIAMPPInstPrinter::printOperand: isExpr\n";
//		MOp.getExpr()->print(O, &MAI);
printExpr(MOp.getExpr(), O, MAI);
		return;
	}

	llvm_unreachable("Unknown operand kind in printOperand");
}

// Print a memory operand (defined in VIAMPPInstrInfo.td)
void VIAMPPInstPrinter::printIndirect(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
//	outs().changeColor(raw_ostream::GREEN,0);
//	outs() << "VIAMPPInstPrinter::printIndirect\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	const MCOperand &BaseAddr = MI->getOperand(OpNo);

	if (BaseAddr.isReg()) {
//		outs() << "VIAMPPInstPrinter::printIndirect: isReg\n";
		O << "(" << getRegisterName(BaseAddr.getReg()) << ")";
		return;
	}

	if (BaseAddr.isExpr()) {
		outs() << "VIAMPPInstPrinter::printIndirect: isExpr\n";
		O << "(";
//		BaseAddr.getExpr()->print(O, &MAI);
		printExpr(BaseAddr.getExpr(), O, MAI);
		O << ")";
		return;
	}
}

void VIAMPPInstPrinter::printIndOff(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
//	outs().changeColor(raw_ostream::GREEN,0);
//	outs() << "VIAMPPInstPrinter::printIndOff\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	const MCOperand &BaseAddr = MI->getOperand(OpNo);
	const MCOperand &Offset = MI->getOperand(OpNo+1);
	assert(Offset.isImm() && "Expected immediate in displacement field");
	O << "[" << Offset.getImm() << "](" <<	getRegisterName(BaseAddr.getReg()) << ")";
}

void VIAMPPInstPrinter::printRegOff(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
//	outs().changeColor(raw_ostream::GREEN,0);
//	outs() << "VIAMPPInstPrinter::printRegOff\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	const MCOperand &BaseAddr = MI->getOperand(OpNo);
	const MCOperand &Offset = MI->getOperand(OpNo+1);
	assert(Offset.isImm() && "Expected immediate in displacement field");
	O << "[" << Offset.getImm() << "]" << getRegisterName(BaseAddr.getReg());
//	O << "[" /*<< Offset.getImm()*/ << "]" ;//<< getRegisterName(BaseAddr.getReg());

}


