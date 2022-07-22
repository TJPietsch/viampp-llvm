//== VIAMPPInstPrinter.h - Convert VIAMPP MCInst to assembly syntax -----------------------------------------------------*- C++ -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------------------------------------------------------===//
//
// This file contains the declaration of the VIAMPPInstPrinter class, which is used to print VIAMPP MCInst to a .s file.
//
//===-------------------------------------------------------------------------------------------------------------------------===//

#ifndef VIAMPPINSTPRINTER_H
#define VIAMPPINSTPRINTER_H
#include "llvm/MC/MCInstPrinter.h"

namespace llvm {

class TargetMachine;

class VIAMPPInstPrinter : public MCInstPrinter {
public:
	VIAMPPInstPrinter(const MCAsmInfo &MAI, const MCInstrInfo &MII, const MCRegisterInfo &MRI) : MCInstPrinter(MAI, MII, MRI) {}

	void printInst(const MCInst *MI, uint64_t Address, StringRef Annot, const MCSubtargetInfo &STI, raw_ostream &O) override;

	void printOperand(const MCInst *MI, unsigned OpNo, raw_ostream &O);
	void printRegName(raw_ostream &OS, unsigned RegNo) const override;
  
	void printIndirect(const MCInst *MI, unsigned OpNo, raw_ostream &O);
	void printRegOff(const MCInst *MI, unsigned OpNo, raw_ostream &O);
	void printIndOff(const MCInst *MI, unsigned OpNo, raw_ostream &O);

	// Autogenerated by tblgen.
	std::pair<const char *, uint64_t> getMnemonic(const MCInst *MI) override;
	void printInstruction(const MCInst *MI, uint64_t Address, raw_ostream &O);
	bool printAliasInstr(const MCInst *MI, uint64_t Address, raw_ostream &O);
	static const char *getRegisterName(unsigned RegNo);

};
} // end namespace llvm

#endif