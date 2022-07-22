//===-- VIAMPP.td - Describe the VIAMPP Target Machine -----*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This is the top level entry point for the VIAMPP target.
// (See llvm/Target/Target.td for information)
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Target-independent interfaces which we are implementing
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// Register File, Calling Convention and Instruction Descriptions
//===----------------------------------------------------------------------===//

include "VIAMPPRegisterInfo.td"
include "VIAMPPCallingConv.td"
include "VIAMPPInstrInfo.td"

def VIAMPPInstrInfo : InstrInfo;

//===----------------------------------------------------------------------===//
// VIAMPP processors supported.
//===----------------------------------------------------------------------===//

class Proc<string Name, list<SubtargetFeature> Features>
    : Processor<Name, NoItineraries, Features>;

def : Proc<"generic", []>;
def : Proc<"generic", []>;

//===---------------------------------------------------------------------===//
// Assembly Printers
//===---------------------------------------------------------------------===//

def VIAMPPAsmWriter : AsmWriter {
  string AsmWriterClassName = "InstPrinter";
  bit isMCAsmWriter = 1;
}

//===---------------------------------------------------------------------===//
// Assembly Parsers
//===---------------------------------------------------------------------===//

def VIAMPPAsmParser : AsmParser {
  let ShouldEmitMatchRegisterAltName = 1;
}

//===----------------------------------------------------------------------===//
// Declare the target which we are implementing
//===----------------------------------------------------------------------===//

def VIAMPP : Target {
  // Pull in Instruction Info
  let InstructionSet = VIAMPPInstrInfo;
//  let AssemblyWriter = [VIAMPPAsmWriter];
  let AssemblyParsers = [VIAMPPAsmParser];
}
