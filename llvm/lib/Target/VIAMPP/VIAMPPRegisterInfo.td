//===-- VIAMPPRegisterInfo.td - VIAMPP Register defs --------- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the VIAMPP register file 
//
// Changes here also need to be reflected in the decoder table and function in 
// Disassembler/VIAMPPDisassembler.
//
//===----------------------------------------------------------------------===//

// Virtual register definition
class VIAMPPReg<bits<16> Enc, string n, list<string> alt = []> : Register<n> {
  let Namespace = "VIAMPP";
  let HWEncoding = Enc;
  let AltNames = alt;
}

// Special purpose registers for C/C++
def StackPtr    : VIAMPPReg<0, "StackPtr",  ["v0"]>; // stack pointer register
def FramePtr    : VIAMPPReg<1, "FramePtr",  ["v1"]>; // frame pointer register
def ReturnReg   : VIAMPPReg<2, "ReturnReg", ["v2"]>; // return value register
def LinkReg     : VIAMPPReg<3, "LinkReg",   ["v3"]>; // return address register
def BaseReg     : VIAMPPReg<4, "BaseReg",   ["v4"]>; // base register

// General virtual registers
foreach i = 5-255 in {	// derzeit nur mit 256, dann aber 65535
  def V#i : VIAMPPReg < i, "v"#i>;
}

// General virtual registers class
def VRegs : RegisterClass<"VIAMPP", [i64, f64], 64, (add StackPtr, FramePtr, 
            ReturnReg, LinkReg, BaseReg, (sequence "V%u", 5, 255))>; // derzeit nur mit 256, dann 65535


// System register definition
class VIAMPPSysReg<bits<16> Enc, string n, list<string> alt = []> : Register<n> {
  let Namespace = "VIAMPP";
  let HWEncoding = Enc;
  let AltNames = alt;
}

// Special registers
def PC 	     : VIAMPPSysReg<0, "PC",       ["PC"]>;	  // program counter
def BaseAddr : VIAMPPSysReg<1, "BaseAddr", ["baseaddr"]>; // base address register
def MinAddr  : VIAMPPSysReg<2, "MinAddr",  ["minaddr"]>;  // min address register
def MaxAddr  : VIAMPPSysReg<3, "MaxAddr",  ["maxaddr"]>;  // max address register
def SigVal   : VIAMPPSysReg<4, "SigVal",   ["sigval"]>;   // signal value register
def Alice    : VIAMPPSysReg<5, "Alice",    ["alice"]>;    // timer alice value register
def Bob      : VIAMPPSysReg<6, "Bob",      ["bob"]>;      // timer bob value register
def Counter  : VIAMPPSysReg<7, "Counter",  ["counter"]>;  // free running counter register
def InOut    : VIAMPPSysReg<8, "InOut",    ["inout"]>;    // input/output port register
def TrapMask : VIAMPPSysReg<9, "TrapMask", ["trapmask"]>; // trap mask register
def PrevPC   : VIAMPPSysReg<10,"PrevPC",   ["prevPC"]>;   // previous PC
def Config   : VIAMPPSysReg<11,"Config",   ["config"]>;   // CPU configuration register

// Trap vectors
def SigVec   : VIAMPPSysReg<16, "SigVec",   ["sigvec"]>;   // signal trap vector (maskable)
def InpVec   : VIAMPPSysReg<17, "InpVec",   ["inpvec"]>;   // input port trap vector (maskable)
def AliceVec : VIAMPPSysReg<18, "AliceVec", ["alicevec"]>; // timer alice trap vector (maskable)
def BobVec   : VIAMPPSysReg<19, "BobVec",   ["bobvec"]>;   // timer alice trap vector (maskable)
def RangeVec : VIAMPPSysReg<20, "RangeVec", ["rangevec"]>; // address out of range vector
def BAddrVec : VIAMPPSysReg<21, "BAddrVec", ["baddrvec"]>; // bad address trap vector
def IllOpVec : VIAMPPSysReg<22, "IllOpVec", ["illopvec"]>; // illegal opcode trap vector
def CBVec    : VIAMPPSysReg<23, "CBVec",    ["cbvec"]>;	   // carry/borrow trap vector
def OvflVec  : VIAMPPSysReg<24, "OvflVec",  ["ovflvec"]>;  // overflow on multiplication trap vector
def DivVec  : VIAMPPSysReg<25, "DivVec",    ["divvec"]>;   // division by zero trap vector

// System register class
def SRegs : RegisterClass<"VIAMPP", [i64], 64, (add PC, BaseAddr,MinAddr, 
            MaxAddr, SigVal, Alice, Bob, Counter, InOut, TrapMask, PrevPC,
            
            SigVec, InpVec, AliceVec, BobVec, RangeVec, BAddrVec, IllOpVec,
	    CBVec, OvflVec, DivVec)>;
