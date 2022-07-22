//===-- VIAMPPInstrInfo.td - Target Description for VIAMPP -------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the VIAMPP instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Instruction Formats
//===----------------------------------------------------------------------===//
include "VIAMPPInstrFormats.td"

//===----------------------------------------------------------------------===//
// Operand Definitions
//===----------------------------------------------------------------------===//
//include "VIAMPPOperands.td"
//===----------------------------------------------------------------------===//
// Type Profiles.
//===----------------------------------------------------------------------===//
def SDT_VIAMPPSelectCC     : SDTypeProfile<1, 5, [SDTCisSameAs<1, 2>,
                                                  SDTCisSameAs<0, 4>, 
                                                  SDTCisSameAs<4, 5>]>;
def SDT_VIAMPPWrapper      : SDTypeProfile<1, 1, [SDTCisSameAs<0, 1>,
                                                  SDTCisPtrTy<0>]>;
def SDT_VIAMPPCall         : SDTypeProfile<0, -1, [SDTCisVT<0, i64>]>;
def SDT_VIAMPPCallSeqStart : SDCallSeqStart<[SDTCisVT<0, i64>, SDTCisVT<1, i64>]>;
def SDT_VIAMPPCallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, i64>, SDTCisVT<1, i64>]>;
def SDT_VIAMPPAdjDynAlloc  : SDTypeProfile<1, 1, [SDTCisVT<0, i64>,
                                               SDTCisVT<1, i64>]>;

//===----------------------------------------------------------------------===//
// VIAMPP Specific Node Definitions (define also in VIAMPPISelLowering.cpp/h)
//===----------------------------------------------------------------------===//
def VIAMPPretFlag 	    : SDNode<"VIAMPPISD::RET_FLAG", SDTNone, [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;
def VIAMPPSelectCC      : SDNode<"VIAMPPISD::SELECT_CC", SDT_VIAMPPSelectCC>; //, [SDNPInGlue]>;
def VIAMPPWrapper       : SDNode<"VIAMPPISD::Wrapper", SDT_VIAMPPWrapper>;
def VIAMPPcall          : SDNode<"VIAMPPISD::CALL", SDT_VIAMPPCall, [SDNPHasChain, SDNPOutGlue, SDNPOptInGlue, SDNPVariadic]>;
def VIAMPPcallseq_start : SDNode<"ISD::CALLSEQ_START", SDT_VIAMPPCallSeqStart, [SDNPHasChain, SDNPOutGlue]>;
def VIAMPPcallseq_end   : SDNode<"ISD::CALLSEQ_END", SDT_VIAMPPCallSeqEnd, [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;
def VIAMPPadjdynalloc   : SDNode<"VIAMPPISD::ADJDYNALLOC", SDT_VIAMPPAdjDynAlloc>;
def VIAMPPHi : SDNode<"VIAMPPISD::HI", SDTIntUnaryOp>;
def VIAMPPLo : SDNode<"VIAMPPISD::LO", SDTIntUnaryOp>;

//===----------------------------------------------------------------------===//
// VIAMPP Immediate Extract Definitions
//===----------------------------------------------------------------------===//
// Extract bits 0-15 (low-end) of an immeditae value.
def LO16 : SDNodeXForm<imm,  [{
  return CurDAG->getTargetConstant((uint64_t)N->getZExtValue() & 0xffff, SDLoc(N), MVT::i64);
}]>;

// Extract bits 0-31 (low-end) of an immeditae value.
def LO32 : SDNodeXForm<imm,  [{
  return CurDAG->getTargetConstant((uint64_t)N->getZExtValue() & 0xffffffff, SDLoc(N), MVT::i64);
}]>;

// Extract bits 63-32 (low-end) of an immeditae value.
def HI32 : SDNodeXForm<imm,  [{
  return CurDAG->getTargetConstant((uint64_t)N->getZExtValue() >> 32, SDLoc(N), MVT::i64);
}]>;


//===----------------------------------------------------------------------===//
// VIAMPP Immediate Definitions
//===----------------------------------------------------------------------===//
def VIAMPPimm16 : PatLeaf<(i64 imm), [{return isInt<16>(N->getZExtValue()); }], LO16>;

def VIAMPPimm32 : PatLeaf<(i64 imm), [{return isInt<32>(N->getZExtValue());}], LO32>;

//===----------------------------------------------------------------------===//
// VIAMPP Immediate Predicate Definitions
//===----------------------------------------------------------------------===//
def i64lo16s : PatLeaf<(i64 imm), [{
  //i64lo16 predicate - true if the 64-bit immediate has only rightmost 16 bits set
  return (N->getZExtValue() & 0xFFFFULL) == N->getSExtValue());
}], LO16>;

def i64lo32s : PatLeaf<(i64 imm), [{
  //i64lo32 predicate - true if the 64-bit immediate has only rightmost 32 bits set
  return (N->getZExtValue() & 0xFFFFFFFFULL) == N->getSExtValue());
}], LO32>;

def i64lo32 : PatLeaf<(i64 imm), [{
  //i64lo32 predicate - true if the 64-bit immediate has only rightmost 32 bits set
  return (N->getZExtValue() & 0xFFFFFFFFULL) == N->getZExtValue());
}], LO32>;

def i64hi32 : PatLeaf<(i64 imm), [{
  //i64hi32 predicate - true if the 64-bit immediate has only leftmost 32 bits set
  return (N->getZExtValue() & 0xFFFFFFFF00000000ULL) == N->getZExtValue());
}], HI32>;

//===----------------------------------------------------------------------===//
// Address operand modes ((Vregs), [offset]Vregs and [offset](Vregs) addressing)
//===----------------------------------------------------------------------===//

def IndRegAsmOperand : AsmOperandClass {
  let Name = "Relative";
  let RenderMethod = "addRegOperands";
}
  
// Address operand for indirect addressing mode (R addressing)
def VIAMPPindirect : Operand<iPTR> {
  let PrintMethod = "printIndirect";
//  let MIOperandInfo = (ops (i64 VRegs));
  let MIOperandInfo = (ops VRegs);
  let ParserMatchClass = IndRegAsmOperand;
  let EncoderMethod = "getMemoryValue";
  let DecoderMethod = "DecodeMemoryValue";
}

def MemAsmOperand : AsmOperandClass {
  let Name = "Mem";
}

// Address operand for register + offset addressing mode (R+off addressing)
def VIAMPPregoff : Operand<iPTR> {
  let PrintMethod = "printRegOff";
  let MIOperandInfo = (ops VRegs, i64imm);	// ganz alt
//  let MIOperandInfo = (ops (i64 VRegs), i16imm);
//  let MIOperandInfo = (ops (i64 VRegs), i16imm);
  let ParserMatchClass = MemAsmOperand;
  let EncoderMethod = "getMemoryOffsetValue";
  let DecoderMethod = "DecodeMemoryOffsetValue";
}

def MemIndAsmOperand : AsmOperandClass {
  let Name = "MemInd";
}

// Address operand for indirect + offset addressing mode ((R)+off addressing)
def VIAMPPindoff : Operand<iPTR> {
  let PrintMethod = "printIndOff";
//  let MIOperandInfo = (ops (i64 VRegs), i16imm);
  let MIOperandInfo = (ops VRegs, i16imm);
//  let MIOperandInfo = (ops VRegs, i64imm);	// ganz alt
  let ParserMatchClass = MemIndAsmOperand;
  let EncoderMethod = "getMemoryOffsetValue";
  let DecoderMethod = "DecodeMemoryOffsetValue";
}

//===----------------------------------------------------------------------===//
// VIAMPP Complex Pattern Definitions.
//===----------------------------------------------------------------------===//

def addr : ComplexPattern<iPTR, 1, "SelectAddr", [], []>;
def addrOff : ComplexPattern<iPTR, 2, "SelectAddrOff", [frameindex], []>;

//===----------------------------------------------------------------------===//
// Pseudo Instructions
//===----------------------------------------------------------------------===//
//
// ADJCALLSTACKDOWN/UP implicitly use/def R1 because they may be expanded into
// a stack adjustment and the codegen must know that they may modify the stack
// pointer before prolog-epilog rewriting occurs.
// Pessimistically assume ADJCALLSTACKDOWN / ADJCALLSTACKUP will become
// sub / add which can clobber the stack pointer.
let Defs = [StackPtr], Uses = [StackPtr] in {
def ADJCALLSTACKDOWN : Pseudo<(outs), (ins i64imm:$amt1, i64imm:$amt2),
                                     "#ADJCALLSTACKDOWN",
                                     [(VIAMPPcallseq_start timm:$amt1, timm:$amt2)]>;
def ADJCALLSTACKUP : Pseudo<(outs), (ins i64imm:$amt1, i64imm:$amt2),
                                   "#ADJCALLSTACKUP",
                                   [(VIAMPPcallseq_end timm:$amt1, timm:$amt2)]>;
}

let isCodeGenOnly = 1, Uses = [StackPtr] in {
def ADDframe : Pseudo<(outs VRegs:$dst), (ins VIAMPPindirect:$base, i64imm:$offset),
                      "#ADDframe PSEUDO", []>;
}

// Dynamic stack allocation
let Defs = [StackPtr], Uses = [StackPtr] in {
def ADJDYNALLOC : Pseudo<(outs VRegs:$dst), (ins VRegs:$src),
                  "#ADJDYNALLOC $dst $src",
                  [(set VRegs:$dst, (VIAMPPadjdynalloc VRegs:$src))]>;
}
 
//===----------------------------------------------------------------------===//
// ALU Instructions
//===----------------------------------------------------------------------===//
include "VIAMPPALUInstrinfo.td"

//===----------------------------------------------------------------------===//
// Control Flow Instructions
//===----------------------------------------------------------------------===//
include "VIAMPPCntrFlowInstrinfo.td"

//===----------------------------------------------------------------------===//
// Data Transfer Instructions
//===----------------------------------------------------------------------===//
include "VIAMPPTransferInstrinfo.td"

//===----------------------------------------------------------------------===//
// System Instructions
//===----------------------------------------------------------------------===//
include "VIAMPPSystemInstrinfo.td"

//===----------------------------------------------------------------------===//
// Non-Instruction Patterns
//===----------------------------------------------------------------------===//
// signed 16-bit immediate
def : Pat<(i64 VIAMPPimm16:$imm), (MOVaxi imm:$imm)>;

// arbitrary immediate
def : Pat<(i64 imm:$imm), (ORad2i (MOVHIa2i (HI32 imm:$imm)), (LO32 imm:$imm))>;

/*  for later comparisons with R and (R) as branch offset
//let usesCustomInserter = 1 in
//def Select : VIAMPPPseudoInst<(outs GPRegs:$dst),
//                    (ins GPRegs:$src, GPRegs:$src2, i32imm:$condition),
//                    "# Select PSEUDO",
//                    [(set GPRegs:$dst,
//                     (VIAMPPSelectCC GPRegs:$src, GPRegs:$src2, imm:$condition))]>;
//
*/

// GlobalAddress, ExternalSymbol, JumpTable, ConstantPool
def : Pat<(VIAMPPHi tglobaladdr:$dst), (MOVHIa2i tglobaladdr:$dst)>;
def : Pat<(VIAMPPLo tglobaladdr:$dst), (MOVa2i tglobaladdr:$dst)>;
//def : Pat<(VIAMPPLo tglobaladdr:$dst), (ORad2i VRegs:$dest, tglobaladdr:$dst)>;

def : Pat<(VIAMPPHi texternalsym:$dst), (MOVHIa2i texternalsym:$dst)>;
def : Pat<(VIAMPPLo texternalsym:$dst), (MOVa2i texternalsym:$dst)>;
//def : Pat<(VIAMPPLo texternalsym:$dst), (ORad2i texternalsym:$dst)>;

def : Pat<(VIAMPPHi tblockaddress:$dst), (MOVHIa2i tblockaddress:$dst)>;
def : Pat<(VIAMPPLo tblockaddress:$dst), (MOVa2i tblockaddress:$dst)>;
//def : Pat<(VIAMPPLo tblockaddress:$dst), (ORad2i tblockaddress:$dst)>;

def : Pat<(VIAMPPHi tjumptable:$dst), (MOVHIa2i tjumptable:$dst)>;
def : Pat<(VIAMPPLo tjumptable:$dst), (MOVa2i tjumptable:$dst)>;
//def : Pat<(VIAMPPLo tjumptable:$dst), (ORad2i tjumptable:$dst)>;

def : Pat<(VIAMPPHi tconstpool:$dst), (MOVHIa2i tconstpool:$dst)>;
def : Pat<(VIAMPPLo tconstpool:$dst), (MOVa2i tconstpool:$dst)>;
//def : Pat<(VIAMPPLo tconstpool:$dst), (ORad2i tconstpool:$dst)>;

def : Pat<(or VRegs:$hi, (VIAMPPLo tglobaladdr:$lo)),
          (ORad2i VRegs:$hi, tglobaladdr:$lo)>;
def : Pat<(or VRegs:$hi, (VIAMPPLo texternalsym:$lo)),
          (ORad2i VRegs:$hi, texternalsym:$lo)>;
def : Pat<(or VRegs:$hi, (VIAMPPLo tblockaddress:$lo)),
          (ORad2i VRegs:$hi, tblockaddress:$lo)>;
def : Pat<(or VRegs:$hi, (VIAMPPLo tjumptable:$lo)),
          (ORad2i VRegs:$hi, tjumptable:$lo)>;
def : Pat<(or VRegs:$hi, (VIAMPPLo tconstpool:$lo)),
          (ORad2i VRegs:$hi, tconstpool:$lo)>;

/*
// Calls -- used and defined in VIAMPPCntrFlow
//def : Pat<(VIAMPPcall (i64 tglobaladdr:$addr)), (CALLImm tglobaladdr:$addr)>;
//def : Pat<(VIAMPPcall (i64 texternalsym:$addr)), (CALLImm texternalsym:$addr)>;
*/

// Loads
def : Pat<(sextloadi16  addr:$src), (i64 (MOVaxi addr:$src))>;
def : Pat<(sextloadi32  addr:$src), (i64 (MOVa2i addr:$src))>;

//===----------------------------------------------------------------------===//
// Floating Point Instructions
//===----------------------------------------------------------------------===//
include "VIAMPPFPInstrinfo.td"

