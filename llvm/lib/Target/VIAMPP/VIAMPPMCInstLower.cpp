//===-- VIAMPPMCInstLower.cpp - Convert VIAMPP MachineInstr to an MCInst -------------------------------------------------------=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------------------------------------------------------===//
//
// This file contains code to lower VIAMPP MachineInstrs to their corresponding MCInst records.
//
//===-------------------------------------------------------------------------------------------------------------------------===//

#include "VIAMPP.h"
#include "VIAMPPMCInstLower.h"
#include "MCTargetDesc/VIAMPPBaseInfo.h"
#include "VIAMPPInstrInfo.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Support/Debug.h"


using namespace llvm;

void VIAMPPMCInstLower::Initialize(MCContext *C) {
//  outs() << "VIAMPPMCInstLower::Initialize\n";

//  Ctx = C;
}

MCSymbol *VIAMPPMCInstLower::GetGlobalAddressSymbol(const MachineOperand &MO) const {
	outs() << "VIAMPPMCInstLower::GetGlobalAddressSymbol\n";

	return Printer.getSymbol(MO.getGlobal());
}

MCSymbol *VIAMPPMCInstLower::GetBlockAddressSymbol(const MachineOperand &MO) const {
	outs() << "VIAMPPMCInstLower::GetBlockAddressSymbol\n";

	return Printer.GetBlockAddressSymbol(MO.getBlockAddress());
}

MCSymbol *VIAMPPMCInstLower::GetExternalSymbolSymbol(const MachineOperand &MO) const {
	outs() << "VIAMPPMCInstLower::GetExternalSymbolSymbol\n";

	return Printer.GetExternalSymbolSymbol(MO.getSymbolName());
}

MCSymbol *VIAMPPMCInstLower::GetJumpTableSymbol(const MachineOperand &MO) const {
	outs() << "VIAMPPMCInstLower::GetJumpTableSymbol\n";

	SmallString<256> Name;
	raw_svector_ostream(Name) << Printer.MAI->getPrivateGlobalPrefix() << "JTI" << Printer.getFunctionNumber() << '_'
                            << MO.getIndex();
	// Create a symbol for the name.
	return Ctx.getOrCreateSymbol(Name);
}

MCSymbol *VIAMPPMCInstLower::GetConstantPoolIndexSymbol(const MachineOperand &MO) const {
	outs() << "VIAMPPMCInstLower::GetConstantPoolIndexSymbol\n";

	SmallString<256> Name;
	raw_svector_ostream(Name) << Printer.MAI->getPrivateGlobalPrefix() << "CPI" << Printer.getFunctionNumber() << '_'
                            << MO.getIndex();
	// Create a symbol for the name.
	return Ctx.getOrCreateSymbol(Name);
}

MCOperand VIAMPPMCInstLower::LowerSymbolOperand(const MachineOperand &MO, MCSymbol *Sym) const {
  outs() << "VIAMPPMCInstLower::LowerSymbolOperand\n";
	MCSymbolRefExpr::VariantKind Kind;

  
	switch (MO.getTargetFlags()) {
    default: llvm_unreachable("Unknown target flag on GV operand");
//    case VIAMPPII::MO_NO_FLAG:	Kind = MCSymbolRefExpr::VK_VIAMPP_NONE; outs() << "MO_NO_FLAG\n"; break;
    case VIAMPPII::MO_NO_FLAG:	Kind = MCSymbolRefExpr::VK_None; outs() << "MO_NO_FLAG\n"; break;
    case VIAMPPII::MO_ABS_HI:	Kind = MCSymbolRefExpr::VK_VIAMPP_ABS_HI;  outs() << "MO_ABS_HI\n";break;
    case VIAMPPII::MO_ABS_LO:	Kind = MCSymbolRefExpr::VK_VIAMPP_ABS_LO;  outs() << "MO_ABS_LO\n";break;
//    case 0: break;
    }

  const MCExpr *Expr = MCSymbolRefExpr::create(Sym, Kind, Ctx);
//const MCExpr *Expr = MCSymbolRefExpr::create(Sym, Ctx);

  if (!MO.isJTI() && MO.getOffset())
	Expr = MCBinaryExpr::createAdd(Expr, MCConstantExpr::create(MO.getOffset(), Ctx), Ctx);

  return MCOperand::createExpr(Expr);
}

void VIAMPPMCInstLower::Lower(const MachineInstr *MI, MCInst &OutMI) const {
	outs() << "VIAMPPMCInstLower::Lower\n";

	OutMI.setOpcode(MI->getOpcode());

	for (unsigned i = 0, e = MI->getNumOperands(); i != e; ++i) {
		const MachineOperand &MO = MI->getOperand(i);

    MCOperand MCOp;

    switch (MO.getType()) {
    default:
    	llvm_unreachable("unknown operand type");
    case MachineOperand::MO_Register:
    	// Ignore all implicit register operands.
    	if (MO.isImplicit()) continue;
    	MCOp = MCOperand::createReg(MO.getReg());
    	break;
    case MachineOperand::MO_Immediate:
    	MCOp = MCOperand::createImm(MO.getImm());
    	break;
    case MachineOperand::MO_FPImmediate: {
        // FP immediates may be dealt with like regular immediates from this point on.
        APFloat Val = MO.getFPImm()->getValueAPF();
    	MCOp = MCOperand::createImm(*Val.bitcastToAPInt().getRawData());
    	break;
    }
	case MachineOperand::MO_MachineBasicBlock:
		MCOp = MCOperand::createExpr(MCSymbolRefExpr::create(MO.getMBB()->getSymbol(), Ctx));
		break;
	case MachineOperand::MO_RegisterMask:
		continue;
    case MachineOperand::MO_GlobalAddress:
    	MCOp = LowerSymbolOperand(MO, GetGlobalAddressSymbol(MO));
    	break;
    case MachineOperand::MO_BlockAddress:
    	MCOp = LowerSymbolOperand(MO, GetBlockAddressSymbol(MO));
    	break;
    case MachineOperand::MO_ExternalSymbol:
    	MCOp = LowerSymbolOperand(MO, GetExternalSymbolSymbol(MO));
    	break;
    case MachineOperand::MO_JumpTableIndex:
    	MCOp = LowerSymbolOperand(MO, GetJumpTableSymbol(MO));
    	break;
    case MachineOperand::MO_ConstantPoolIndex:
    	MCOp = LowerSymbolOperand(MO, GetConstantPoolIndexSymbol(MO));
    	break;
    }

    OutMI.addOperand(MCOp);
  }
}
