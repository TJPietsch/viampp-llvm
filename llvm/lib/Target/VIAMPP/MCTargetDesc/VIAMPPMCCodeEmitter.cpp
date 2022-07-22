//===-- VIAMPPFrameLowering.cpp - VIAMPP Frame Information -------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------------------------------------------------------===//
//
// This file contains the VIAMPP implementation of the MCCodeEmitter class.
//
//===-------------------------------------------------------------------------------------------------------------------------===//

#include "VIAMPP.h"
#include "MCTargetDesc/VIAMPPFixupKinds.h"
#include "MCTargetDesc/VIAMPPMCTargetDesc.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/Endian.h"
#include "llvm/Support/EndianStream.h"
#include "llvm/Support/raw_ostream.h"

#define DEBUG_TYPE "mccodeemitter"

STATISTIC(MCNumEmitted, "Number of MC instructions emitted");

namespace llvm {

namespace {

class VIAMPPMCCodeEmitter : public MCCodeEmitter {
    MCContext &Ctx;
    MCInstrInfo const &MCII;
    
    // TableGen'erated function for getting the binary encoding for an instruction.
    uint64_t getBinaryCodeForInstr(const MCInst &MI, SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const;

    // Return the binary encoding of operands. If an operand requires relocation, the relocation is recorded and zero returned
    unsigned getMachineOpValue(const MCInst &MI, const MCOperand &MO, SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const;    
    uint16_t getMemoryValue(const MCInst &MI, unsigned Op, SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const;
    uint32_t getMemoryOffsetValue(const MCInst &MI, unsigned Op, SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const;
    uint16_t getImm16Value(const MCInst &MI, unsigned Op, SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const;
    uint32_t getImm32Value(const MCInst &MI, unsigned Op, SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const;

public:
    VIAMPPMCCodeEmitter(MCContext &ctx, MCInstrInfo const &MCII) : Ctx(ctx), MCII(MCII) {}
    
    // EncodeInstruction - Encode the given Inst to bytes on the output stream OS.
	void encodeInstruction(const MCInst &Inst, raw_ostream &OS, SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const override;
	
};

} // end anonymous namespace

unsigned VIAMPPMCCodeEmitter::getMachineOpValue(const MCInst &MI, const MCOperand &MO, SmallVectorImpl<MCFixup> &Fixups,
		                                     const MCSubtargetInfo &STI) const {
//	outs().changeColor(raw_ostream::RED,0);
//	outs() << "VIAMPPMCCodeEmitter::getMachineOpValue\n";
//	outs().changeColor(raw_ostream::WHITE,0);

	if (MO.isReg()) {
		unsigned Reg =  Ctx.getRegisterInfo()->getEncodingValue(MO.getReg());
		return Reg; //Ctx.getRegisterInfo()->getEncodingValue(MO.getReg());
	}

	if (MO.isImm()) {
		unsigned Imm =  static_cast<unsigned>(MO.getImm());
	    return Imm; //static_cast<unsigned>(MO.getImm());
	}

	// MO must be an expression
	assert(MO.isExpr() && "Expected expr operand");

	// see llvm/MC/MCExpr.h for a complete definition
	const MCExpr *Expr = MO.getExpr();
	MCExpr::ExprKind Kind = Expr->getKind();

	if (Kind == MCExpr::Constant) {
		return cast<MCConstantExpr>(Expr)->getValue();
	}

	if (Kind == MCExpr::Binary) {
		Expr = static_cast<const MCBinaryExpr*>(Expr)->getLHS();
		Kind = Expr->getKind();
	}

	// not a constant or binary expression
	assert (Kind == MCExpr::SymbolRef && "unknown MCExpr kind");

	MCInstrDesc const &Desc = MCII.get(MI.getOpcode());
outs() << "   Opcode: " <<	Desc.getOpcode() << "\n"; // == RISCV::JAL

	VIAMPP::Fixups FixupKind = VIAMPP::Fixups(0);

outs() << "FixupKind: " << cast<MCSymbolRefExpr>(Expr)->getKind() << "=============" << MCSymbolRefExpr::VK_None << "\n";
outs() << "InstrFormat: " << Desc.TSFlags << "\n";
switch(cast<MCSymbolRefExpr>(Expr)->getKind()) {
	default:
		llvm_unreachable("Unknown fixup kind!");
	case MCSymbolRefExpr::VK_None:
		// 32 bit relocation as default(for call instructions)
		FixupKind = VIAMPP::fixup_VIAMPP_32_pcrel;
		break;
	case MCSymbolRefExpr::VK_VIAMPP_ABS_HI:
		FixupKind = VIAMPP::fixup_VIAMPP_HI;
		break;
	case MCSymbolRefExpr::VK_VIAMPP_ABS_LO:
		FixupKind = VIAMPP::fixup_VIAMPP_LO;
		break;
	} // switch

	Fixups.push_back(MCFixup::create(0, MO.getExpr(), MCFixupKind(FixupKind)));

	return 0;
}

uint16_t VIAMPPMCCodeEmitter::getMemoryValue(const MCInst &MI, unsigned Op, SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const {
	outs().changeColor(raw_ostream::RED,0);
	outs() << "VIAMPPMCCodeEmitter::getMemoryValue\n";
	outs().changeColor(raw_ostream::WHITE,0);

	const MCOperand op = MI.getOperand(Op);
	assert(op.isReg() && "Operand is not register.");

	return Ctx.getRegisterInfo()->getEncodingValue(op.getReg());
}

uint32_t VIAMPPMCCodeEmitter::getMemoryOffsetValue(const MCInst &MI, unsigned Op, SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const {
	outs().changeColor(raw_ostream::RED,0);
	outs() << "VIAMPPMCCodeEmitter::getMemoryOffValue\n";
	outs().changeColor(raw_ostream::WHITE,0);
	
	uint32_t encoding;

	const MCOperand op1 = MI.getOperand(Op);
	assert(op1.isReg() && "First operand is not register.");
	encoding = (Ctx.getRegisterInfo()->getEncodingValue(op1.getReg()) << 16);

	// The offset can be an immediate value or a relocation.
	const MCOperand op2 = MI.getOperand(Op + 1);
	assert((op2.isImm() || op2.isExpr()) && "Second operand is not immediate or expression.");
	unsigned Value = getMachineOpValue(MI, op2, Fixups, STI);
	encoding |= (static_cast<int16_t>(Value) & 0xffff);

	return encoding;
	
}

uint16_t VIAMPPMCCodeEmitter::getImm16Value(const MCInst &MI, unsigned Op, SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const {
	outs().changeColor(raw_ostream::RED,0);
	outs() << "VIAMPPMCCodeEmitter::getVIAMPPImm16\n";
	outs().changeColor(raw_ostream::WHITE,0);

	const MCOperand MO = MI.getOperand(Op);
	if(MO.isImm())
		return MO.getImm();

	Fixups.push_back(MCFixup::create(0, MO.getExpr(), static_cast<MCFixupKind>(VIAMPP::fixup_VIAMPP_16_pcrel)));

	return 0;
}

uint32_t VIAMPPMCCodeEmitter::getImm32Value(const MCInst &MI, unsigned Op, SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const {
	outs().changeColor(raw_ostream::RED,0);
	outs() << "VIAMPPMCCodeEmitter::getVIAMPPImm32\n";
	outs().changeColor(raw_ostream::WHITE,0);

	const MCOperand MO = MI.getOperand(Op);

	if(MO.isImm())
		return MO.getImm();

	Fixups.push_back(MCFixup::create(0, MO.getExpr(), static_cast<MCFixupKind>(VIAMPP::fixup_VIAMPP_32_pcrel)));

	return 0;
}


void VIAMPPMCCodeEmitter::encodeInstruction(const MCInst &MI, raw_ostream &OS, SmallVectorImpl<MCFixup> &Fixups, const MCSubtargetInfo &STI) const {
//	outs().changeColor(raw_ostream::RED,0);
//	outs() << "VIAMPPMCCodeEmitter::encodeInstruction\n";
//	outs().changeColor(raw_ostream::WHITE,0);
	
	// Get instruction encoding and emit it
	++MCNumEmitted;       // Keep track of the number of emitted insns.
	uint64_t Value = getBinaryCodeForInstr(MI, Fixups, STI);
  
	// Emit bytes in big-endian
	for (int i = (8 - 1) * 8; i >= 0; i -= 8)
		OS << static_cast<char>((Value >> i) & 0xff);
}

#include "VIAMPPGenMCCodeEmitter.inc"

MCCodeEmitter *createVIAMPPMCCodeEmitter(const MCInstrInfo &MCII, const MCRegisterInfo &MRI, MCContext &Ctx) {
	return new VIAMPPMCCodeEmitter(Ctx, MCII);
}

} // end namespace llvm
