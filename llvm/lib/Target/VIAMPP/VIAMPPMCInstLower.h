//===-- VIAMPPMCInstLower.h - Lower MachineInstr to MCInst --------------------------------------------------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------------------------------------------------------===//

#ifndef VIAMPPMCINSTLOWER_H
#define VIAMPPMCINSTLOWER_H

#include "llvm/Support/Compiler.h"
#include "llvm/CodeGen/MachineOperand.h"

namespace llvm {
  class AsmPrinter;
  class MCContext;
  class MCInst;
  class MCOperand;
  class MCSymbol;
  class MachineInstr;
  class MachineOperand;
  class MachineFunction;
  class Mangler;


  // VIAMPPMCInstLower - This class is used to lower an MachineInstr into an MCInst.
class LLVM_LIBRARY_VISIBILITY VIAMPPMCInstLower {
  MCContext &Ctx;
  AsmPrinter &Printer;
//  Mangler *Mang;
  
public:
  VIAMPPMCInstLower(MCContext &ctx, AsmPrinter &printer)
    : Ctx(ctx), Printer(printer) {}
  
  void Initialize(MCContext *C);
  void Lower(const MachineInstr *MI, MCInst &OutMI) const;

  MCOperand LowerSymbolOperand(const MachineOperand &MO, MCSymbol *Sym) const;

  MCSymbol *GetGlobalAddressSymbol(const MachineOperand &MO) const;
  MCSymbol *GetExternalSymbolSymbol(const MachineOperand &MO) const;
  MCSymbol *GetJumpTableSymbol(const MachineOperand &MO) const;
  MCSymbol *GetConstantPoolIndexSymbol(const MachineOperand &MO) const;
  MCSymbol *GetBlockAddressSymbol(const MachineOperand &MO) const;
};

}

#endif
