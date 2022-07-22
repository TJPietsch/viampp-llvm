//===-- VIAMPPInstrFormats.td - VIAMPP Instruction Formats -*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Instruction format superclass
//===----------------------------------------------------------------------===//
class VIAMPPInst<dag outs, dag ins, string asmstr, list<dag> pattern>
    : Instruction {
  field bits<64> Inst;
  // SoftFail is a field the disassembler can use to provide a way for
  // instructions to not match without killing the whole decode process. It is
  // mainly used for ARM, but Tablegen expects this field to exist or it fails
  // to build the decode table.
  field bits<64> SoftFail = 0;
  

  let Namespace = "VIAMPP";
  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString = asmstr;
  let Pattern = pattern;
  let Size = 8;
 

  // define Opcode and Address Modes in base class because all instructions
  // have the same bit-size and bit-location
  bits<7> Opcode = 0;
  bits<3> DestMode = 0; // destination addressing mode
  bits<3> Src1Mode = 0; // source 1 addressing mode
  bits<3> Src2Mode = 0; // source 2 addressing mode

  let Inst{63-57} = Opcode;   // set upper 7 bits to opcode
  let Inst{56-54} = DestMode;
  let Inst{53-51} = Src1Mode;
  let Inst{50-48} = Src2Mode;
}

//===----------------------------------------------------------------------===//
// Base Instruction Formats (Format A, B, Offset-1, Offset-2 & Pseudo)
//===----------------------------------------------------------------------===//

// VIAMPP Format A Instructions (operand, operand, operand)
class VIAMPPFormAInst<dag outs, dag ins, string asmstr, list<dag> pattern>
	: VIAMPPInst<outs, ins, asmstr, pattern> {
	
  bits<16> Dest = 0; 	// destination
  bits<16> Src1 = 0; 	// source 1
  bits<16> Src2 = 0; 	// source 2

  let Inst{47-32} = Dest;
  let Inst{31-16} = Src1;	  // source 1 can also be immediate
  let Inst{15-0}  = Src2;     // source 2 can also be immediate
}

// VIAMPP Format B Instructions (operand, immedite32)
class VIAMPPFormBInst<dag outs, dag ins, string asmstr, list<dag> pattern>
	: VIAMPPInst<outs, ins, asmstr, pattern> {
	
  bits<16> Dest = 0; 	// destination
  bits<32> Imm32 = 0; 	// double immediate

//  let DestMode = 0b000;
//  let Src1Mode = 0b100;
//  let Src2Mode = 0b100;

  let Inst{47-32} = Dest;
  let Inst{31-0} = Imm32;
}

// VIAMPP Format Offset-1 Instructions (dest, efa)
class VIAMPPFormOff1Inst<dag outs, dag ins, string asmstr, list<dag> pattern>
	: VIAMPPInst<outs, ins, asmstr, pattern> {
	
  bits<16> Dest = 0; 	// destination
  bits<32> Efa = 0; 	// effective address

//  let DestMode = 0b000;
//  let Src1Mode = 0b100;
//  let Src2Mode = 0b100;

  let Inst{47-32} = Dest;
  let Inst{31-0} = Efa;
}

// VIAMPP Format Offset-2 Instructions (efa, src)
class VIAMPPFormOff2Inst<dag outs, dag ins, string asmstr, list<dag> pattern>
	: VIAMPPInst<outs, ins, asmstr, pattern> {
	
  bits<16> Src = 0; 	// source
  bits<32> Efa = 0; 	// effective address

//  let DestMode = 0b000;
//  let Src1Mode = 0b100;
//  let Src2Mode = 0b100;

  let Inst{47-16} = Efa;
  let Inst{15-0} = Src;
}


// VIAMPP pseudo instructions format
class Pseudo<dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPInst<outs, ins, asmstr, pattern> {
  let isPseudo = 1;
  let isCodeGenOnly = 1;
}

//===----------------------------------------------------------------------===//
// Format A Instruction Formats
//===----------------------------------------------------------------------===//
// AAA Instructions (Absolute Absolute Absolute)
class AAA_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b001;
    let Src1Mode = 0b001;
    let Src2Mode = 0b001;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = src2;
}

// AAR Instructions (Absolute Absolute Reference)
class AAR_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b001;
    let Src1Mode = 0b001;
    let Src2Mode = 0b010;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = src2;
}

// ARA Instructions (Absolute Reference Absolute)
class ARA_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b001;
    let Src1Mode = 0b010;
    let Src2Mode = 0b001;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = src2;
}

// ARR Instructions (Absolute Reference Reference)
class ARR_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b001;
    let Src1Mode = 0b010;
    let Src2Mode = 0b010;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = src2;
}

// RAA Instructions (Reference Absolute Absolute)
class RAA_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b010;
    let Src1Mode = 0b001;
    let Src2Mode = 0b001;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = src2;
}

// RAR Instructions (Reference Absolute Reference)
class RAR_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b010;
    let Src1Mode = 0b001;
    let Src2Mode = 0b010;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = src2;
}

// RRA Instructions (Reference Reference Absolute)
class RRA_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b010;
    let Src1Mode = 0b010;
    let Src2Mode = 0b001;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = src2;
}

// RRR Instructions (Reference Reference Reference)
class RRR_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b010;
    let Src1Mode = 0b010;
    let Src2Mode = 0b010;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = src2;
}

// SAA Instructions (System Absolute Absolute)
class SAA_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b011;
    let Src1Mode = 0b001;
    let Src2Mode = 0b001;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = src2;
}

// SAR Instructions (System Absolute Reference)
class SAR_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b011;
    let Src1Mode = 0b001;
    let Src2Mode = 0b010;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = src2;
}

// SRA Instructions (System Reference Absolute)
class SRA_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b011;
    let Src1Mode = 0b010;
    let Src2Mode = 0b001;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = src2;
}

// SRR Instructions (System Reference Reference)
class SRR_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b011;
    let Src1Mode = 0b010;
    let Src2Mode = 0b010;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = src2;
}

// AAI Instructions (Absolute Absolute Immediate)
class AAI_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> imm16;

    let Opcode = opcode;
    let DestMode = 0b001;
    let Src1Mode = 0b001;
    let Src2Mode = 0b011;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = imm16;
}

// AIA Instructions (Absolute Immediate Absolute)
class AIA_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> imm16;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b001;
    let Src1Mode = 0b011;
    let Src2Mode = 0b001;
    let Dest = dest;
    let Src1 = imm16;
    let Src2 = src2;
}

// ARI Instructions (Absolute Reference Immediate)
class ARI_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> imm16;

    let Opcode = opcode;
    let DestMode = 0b001;
    let Src1Mode = 0b010;
    let Src2Mode = 0b011;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = imm16;
}

// AIR Instructions (Absolute Immediate Reference)
class AIR_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> imm16;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b001;
    let Src1Mode = 0b011;
    let Src2Mode = 0b010;
    let Dest = dest;
    let Src1 = imm16;
    let Src2 = src2;
}

// RAI Instructions (Reference Absolute Immediate)
class RAI_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> imm16;

    let Opcode = opcode;
    let DestMode = 0b010;
    let Src1Mode = 0b001;
    let Src2Mode = 0b011;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = imm16;
}

// RIA Instructions (Reference Immediate Absolute)
class RIA_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> imm16;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b010;
    let Src1Mode = 0b011;
    let Src2Mode = 0b001;
    let Dest = dest;
    let Src1 = imm16;
    let Src2 = src2;
}

// RRI Instructions (Reference Reference Immediate)
class RRI_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> imm16;

    let Opcode = opcode;
    let DestMode = 0b010;
    let Src1Mode = 0b010;
    let Src2Mode = 0b011;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = imm16;
}

// RIR Instructions (Reference Immediate Reference)
class RIR_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> imm16;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b010;
    let Src1Mode = 0b011;
    let Src2Mode = 0b010;
    let Dest = dest;
    let Src1 = imm16;
    let Src2 = src2;
}

// AII Instructions (Absolute Immediate Immediate)
class AII_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> imm16;
    bits<16> imm16br;

    let Opcode = opcode;
    let DestMode = 0b001;
    let Src1Mode = 0b011;
    let Src2Mode = 0b011;
    let Dest = dest;
    let Src1 = imm16;
    let Src2 = imm16br;
}

// RII Instructions (Reference Immediate Immediate)
class RII_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> imm16;
    bits<16> imm16br;

    let Opcode = opcode;
    let DestMode = 0b010;
    let Src1Mode = 0b011;
    let Src2Mode = 0b011;
    let Dest = dest;
    let Src1 = imm16;
    let Src2 = imm16br;
}

// SII Instructions (System Immediate Immediate)
class SII_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> imm16;
    bits<16> imm16br;

    let Opcode = opcode;
    let DestMode = 0b011;
    let Src1Mode = 0b011;
    let Src2Mode = 0b011;
    let Dest = dest;
    let Src1 = imm16;
    let Src2 = imm16br;
}

// SAI Instructions (System Absolute Immediate)
class SAI_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> imm16;

    let Opcode = opcode;
    let DestMode = 0b011;
    let Src1Mode = 0b001;
    let Src2Mode = 0b011;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = imm16;
}

// SRI Instructions (System Reference Immediate)
class SRI_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> imm16;

    let Opcode = opcode;
    let DestMode = 0b011;
    let Src1Mode = 0b010;
    let Src2Mode = 0b011;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = imm16;
}

// SIA Instructions (System Immediate Absolute)
class SIA_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> imm16;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b011;
    let Src1Mode = 0b011;
    let Src2Mode = 0b001;
    let Dest = dest;
    let Src1 = imm16;
    let Src2 = src2;
}

// SIR Instructions (System Immediate Reference)
class SIR_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> imm16;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b011;
    let Src1Mode = 0b011;
    let Src2Mode = 0b010;
    let Dest = dest;
    let Src1 = imm16;
    let Src2 = src2;
}

// ASA Instructions (Absolute System Absolute)
class ASA_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b001;
    let Src1Mode = 0b111;
    let Src2Mode = 0b001;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = src2;
}

// ASR Instructions (Absolute System Reference)
class ASR_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b001;
    let Src1Mode = 0b111;
    let Src2Mode = 0b010;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = src2;
}

// ASI Instructions (Absolute System Immediate)
class ASI_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> imm16;

    let Opcode = opcode;
    let DestMode = 0b001;
    let Src1Mode = 0b111;
    let Src2Mode = 0b011;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = imm16;
}

// RSA Instructions (Reference System Absolute)
class RSA_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b010;
    let Src1Mode = 0b111;
    let Src2Mode = 0b001;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = src2;
}

// RSR Instructions (Reference System Reference)
class RSR_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b010;
    let Src1Mode = 0b111;
    let Src2Mode = 0b010;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = src2;
}

// RSI Instructions (Reference System Immediate)
class RSI_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> imm16;

    let Opcode = opcode;
    let DestMode = 0b010;
    let Src1Mode = 0b111;
    let Src2Mode = 0b011;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = imm16;
}

// SSA Instructions (System System Absolute)
class SSA_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b011;
    let Src1Mode = 0b111;
    let Src2Mode = 0b001;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = src2;
}

// SSR Instructions (System System Reference)
class SSR_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b011;
    let Src1Mode = 0b111;
    let Src2Mode = 0b010;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = src2;
}

// SSI Instructions (Reference System Immediate)
class SSI_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> imm16;

    let Opcode = opcode;
    let DestMode = 0b011;
    let Src1Mode = 0b111;
    let Src2Mode = 0b011;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = imm16;
}

// AAS Instructions (Absolute Absolute System)
class AAS_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b001;
    let Src1Mode = 0b001;
    let Src2Mode = 0b111;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = src2;
}

// ARS Instructions (Absolute Reference System)
class ARS_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b001;
    let Src1Mode = 0b010;
    let Src2Mode = 0b111;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = src2;
}

// RAS Instructions (Reference Absolute System)
class RAS_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b010;
    let Src1Mode = 0b001;
    let Src2Mode = 0b111;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = src2;
}

// RRS Instructions (Reference Reference System)
class RRS_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b010;
    let Src1Mode = 0b010;
    let Src2Mode = 0b111;
    let Dest = dest;
    let Src1 = src1;
    let Src2 = src2;
}

// AxA Instructions (Absolute x Absolute)
class AxA_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b001;
    let Src1Mode = 0b000;
    let Src2Mode = 0b001;
    let Dest = dest;
    let Src1 = 0b0000000000000000;
    let Src2 = src2;
}

// AxR Instructions (Absolute x Reference)
class AxR_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b001;
    let Src1Mode = 0b000;
    let Src2Mode = 0b010;
    let Dest = dest;
    let Src1 = 0b0000000000000000;
    let Src2 = src2;
}

// RxA Instructions (Reference x Absolute)
class RxA_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b010;
    let Src1Mode = 0b000;
    let Src2Mode = 0b001;
    let Dest = dest;
    let Src1 = 0b0000000000000000;
    let Src2 = src2;
}

// RxR Instructions (Reference x Reference)
class RxR_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b010;
    let Src1Mode = 0b000;
    let Src2Mode = 0b010;
    let Dest = dest;
    let Src1 = 0b0000000000000000;
    let Src2 = src2;
}

// SxA Instructions (System x Absolute)
class SxA_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b011;
    let Src1Mode = 0b000;
    let Src2Mode = 0b001;
    let Dest = dest;
    let Src1 = 0b0000000000000000;
    let Src2 = src2;
}

// SxR Instructions (System x Reference)
class SxR_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b011;
    let Src1Mode = 0b000;
    let Src2Mode = 0b010;
    let Dest = dest;
    let Src1 = 0b0000000000000000;
    let Src2 = src2;
}

// AxS Instructions (Absolute x System)
class AxS_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b001;
    let Src1Mode = 0b000;
    let Src2Mode = 0b111;
    let Dest = dest;
    let Src1 = 0b0000000000000000;
    let Src2 = src2;
}

// RxS Instructions (Reference x System)
class RxS_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b010;
    let Src1Mode = 0b000;
    let Src2Mode = 0b111;
    let Dest = dest;
    let Src1 = 0b0000000000000000;
    let Src2 = src2;
}

// SxS Instructions (Reference x System)
class SxS_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b011;
    let Src1Mode = 0b000;
    let Src2Mode = 0b111;
    let Dest = dest;
    let Src1 = 0b0000000000000000;
    let Src2 = src2;
}

// AxI Instructions (Absolute x Immediate)
class AxI_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> imm16;

    let Opcode = opcode;
    let DestMode = 0b001;
    let Src1Mode = 0b000;
    let Src2Mode = 0b011;
    let Dest = dest;
    let Src1 = 0b0000000000000000;
    let Src2 = imm16;
}

// RxI Instructions (Reference x Immediate)
class RxI_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> imm16;

    let Opcode = opcode;
    let DestMode = 0b010;
    let Src1Mode = 0b000;
    let Src2Mode = 0b011;
    let Dest = dest;
    let Src1 = 0b0000000000000000;
    let Src2 = imm16;
}

// SxI Instructions (System x Immediate)
class SxI_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> imm16;

    let Opcode = opcode;
    let DestMode = 0b011;
    let Src1Mode = 0b000;
    let Src2Mode = 0b011;
    let Dest = dest;
    let Src1 = 0b0000000000000000;
    let Src2 = imm16;
}

// xxx Instructions (x x x)
class xxx_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormAInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<16> src1;
    bits<16> src2;

    let Opcode = opcode;
    let DestMode = 0b000;
    let Src1Mode = 0b000;
    let Src2Mode = 0b000;
    let Dest = 0b0000000000000000;
    let Src1 = 0b0000000000000000;
    let Src2 = 0b0000000000000000;
}

//===----------------------------------------------------------------------===//
// Format B Instruction Formats
//===----------------------------------------------------------------------===//
// A2I Instructions (Absolute Long-Immediate)
class A2I_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormBInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<32> imm32;

    let Opcode = opcode;
    let DestMode = 0b001;
    let Src1Mode = 0b100;
    let Src2Mode = 0b100;    
    let Dest = dest;
    let Imm32 = imm32;
}

// R2I Instructions (Reference Long-Immediate)
class R2I_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormBInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<32> imm32;

    let Opcode = opcode;
    let DestMode = 0b010;
    let Src1Mode = 0b100;
    let Src2Mode = 0b100;   
    let Dest = dest;
    let Imm32 = imm32;
}

// S2I Instructions (Reference Long-Immediate)
class S2I_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormBInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<32> imm32;

    let Opcode = opcode;
    let DestMode = 0b011;
    let Src1Mode = 0b100;
    let Src2Mode = 0b100;   
    let Dest = dest;
    let Imm32 = imm32;
}


// Ad2I Instructions (Reference Long-Immediate)
class Ad2I_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormBInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<32> imm32;

    let Opcode = opcode;
    let DestMode = 0b100;
    let Src1Mode = 0b100;
    let Src2Mode = 0b100;   
    let Dest = dest;
    let Imm32 = imm32;
}

// Sd2I Instructions (Reference Long-Immediate)
class Sd2I_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormBInst<outs, ins, asmstr, pattern> {

    bits<16> dest;
    bits<32> imm32;

    let Opcode = opcode;
    let DestMode = 0b101;
    let Src1Mode = 0b100;
    let Src2Mode = 0b100;   
    let Dest = dest;
    let Imm32 = imm32;
}

//===----------------------------------------------------------------------===//
// Format Offset-1, Offset-2 Instruction Formats
//===----------------------------------------------------------------------===//
// Offset-1 type
class AAO_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormOff1Inst<outs, ins, asmstr, pattern> {
    
    bits<16> dest;
    bits<32> efa;

    let Opcode = opcode;
    let DestMode = 0b001;
    let Src1Mode = 0b001;
    let Src2Mode = 0b110;   
    let Dest = dest;
    let Efa = efa;
}
class ARO_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormOff1Inst<outs, ins, asmstr, pattern> {
    
    bits<16> dest;
    bits<32> efa;

    let Opcode = opcode;
    let DestMode = 0b001;
    let Src1Mode = 0b011;
    let Src2Mode = 0b110;   
    let Dest = dest;
    let Efa = efa;
}
 
class RAO_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormOff1Inst<outs, ins, asmstr, pattern> {
    
    bits<16> dest;
    bits<32> efa;

    let Opcode = opcode;
    let DestMode = 0b010;
    let Src1Mode = 0b001;
    let Src2Mode = 0b110;   
    let Dest = dest;
    let Efa = efa;
}

class RRO_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormOff1Inst<outs, ins, asmstr, pattern> {
    
    bits<16> dest;
    bits<32> efa;

    let Opcode = opcode;
    let DestMode = 0b010;
    let Src1Mode = 0b010;
    let Src2Mode = 0b110;   
    let Dest = dest;
    let Efa = efa;
}

class SAO_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormOff1Inst<outs, ins, asmstr, pattern> {
    
    bits<16> dest;
    bits<32> efa;

    let Opcode = opcode;
    let DestMode = 0b011;
    let Src1Mode = 0b001;
    let Src2Mode = 0b110;   
    let Dest = dest;
    let Efa = efa;
}

class SRO_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormOff1Inst<outs, ins, asmstr, pattern> {
    
    bits<16> dest;
    bits<32> efa;

    let Opcode = opcode;
    let DestMode = 0b011;
    let Src1Mode = 0b010;
    let Src2Mode = 0b110;   
    let Dest = dest;
    let Efa = efa;
}
    
// Offset-2 type
// AOA Instructions (Absolute Offset Absolute)
class AOA_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormOff2Inst<outs, ins, asmstr, pattern> {

    bits<32> efa;
    bits<16> src;

    let Opcode = opcode;
    let DestMode = 0b001;
    let Src1Mode = 0b110;
    let Src2Mode = 0b001;
    let Efa = efa;
    let Src = src;
}

// AOR Instructions (Absolute Offset Relative)
class AOR_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormOff2Inst<outs, ins, asmstr, pattern> {

    bits<32> efa;
    bits<16> src;

    let Opcode = opcode;
    let DestMode = 0b001;
    let Src1Mode = 0b110;
    let Src2Mode = 0b010;
    let Efa = efa;
    let Src = src;
}

// AOS Instructions (Absolute Offset Absolute)
class AOS_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormOff2Inst<outs, ins, asmstr, pattern> {

    bits<32> efa;
    bits<16> src;

    let Opcode = opcode;
    let DestMode = 0b001;
    let Src1Mode = 0b110;
    let Src2Mode = 0b111;
    let Efa = efa;
    let Src = src;
}

// ROA Instructions (Relative Offset Absolute)
class ROA_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormOff2Inst<outs, ins, asmstr, pattern> {

    bits<32> efa;
    bits<16> src;

    let Opcode = opcode;
    let DestMode = 0b010;
    let Src1Mode = 0b110;
    let Src2Mode = 0b001;
    let Efa = efa;
    let Src = src;
}

// ROS Instructions (Relative Offset Absolute)
class ROS_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormOff2Inst<outs, ins, asmstr, pattern> {

    bits<32> efa;
    bits<16> src;

    let Opcode = opcode;
    let DestMode = 0b010;
    let Src1Mode = 0b110;
    let Src2Mode = 0b111;
    let Efa = efa;
    let Src = src;
}

// ROR Instructions (Relative Offset Relative)
class ROR_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormOff2Inst<outs, ins, asmstr, pattern> {

    bits<32> efa;
    bits<16> src;

    let Opcode = opcode;
    let DestMode = 0b010;
    let Src1Mode = 0b110;
    let Src2Mode = 0b010;
    let Efa = efa;
    let Src = src;
}

// AOI Instructions (Absolute Offset Immediate)
class AOI_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormOff2Inst<outs, ins, asmstr, pattern> {

    bits<32> efa;
    bits<16> imm16;

    let Opcode = opcode;
    let DestMode = 0b001;
    let Src1Mode = 0b110;
    let Src2Mode = 0b011;
    let Efa = efa;
    let Src = imm16;
}

// ROI Instructions (Relative Offset Immediate)
class ROI_Inst<bits<7> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : VIAMPPFormOff2Inst<outs, ins, asmstr, pattern> {

    bits<32> efa;
    bits<16> imm16;

    let Opcode = opcode;
    let DestMode = 0b010;
    let Src1Mode = 0b110;
    let Src2Mode = 0b011;
    let Efa = efa;
    let Src = imm16;
}
    