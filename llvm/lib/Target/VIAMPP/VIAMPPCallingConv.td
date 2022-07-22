//===- VIAMPPCallingConv.td - Calling Conventions for VIAMPP - tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// This describes the calling conventions for VIAMPP architecture.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// VIAMPP Return Value Calling Convention
//===----------------------------------------------------------------------===//
def RetCC_VIAMPP : CallingConv<[
  // Promote i8/i16/i32 arguments to i64.
  CCIfType<[i8, i16, i32], CCPromoteToType<i64>>,

  // Promote f16/f32 arguments to f64.
  CCIfType<[f16, f32], CCPromoteToType<f64>>,

// -=-=-=-=- Check later if we can remove the ReturnReg and pass the return value also via the Stack
  CCIfType<[i64, f64], CCAssignToReg<[ReturnReg]>>,	// ReturnReg

  // Integer and Float values get stored in stack slots that are 8 bytes in size
  // and 8-byte aligned.
  CCIfType<[i64, f64], CCAssignToStack<8, 8>>
]>;

//===----------------------------------------------------------------------===//
// VIAMPP Argument Calling Conventions
//===----------------------------------------------------------------------===//
def CC_VIAMPP : CallingConv<[
  // Promote i8/i16/i32 arguments to i64.
//  CCIfType<[i8, i16, i32], CCPromoteToType<i64>>,
  
  // values get passed in registers
  CCIfType<[i64, f64], CCAssignToReg<[V5,V6,V7,V8]>>,	// ******* comment or uncomment

  // Integer values get stored in stack slots that are 8 bytes in size
  // and 8-byte aligned.
  CCIfType<[i64, f64], CCAssignToStack<8, 8>>
]>;

def CSR : CalleeSavedRegs<(add StackPtr, FramePtr, LinkReg, BaseReg)>;
def CSR_FP : CalleeSavedRegs<(add StackPtr, LinkReg, BaseReg)>;
