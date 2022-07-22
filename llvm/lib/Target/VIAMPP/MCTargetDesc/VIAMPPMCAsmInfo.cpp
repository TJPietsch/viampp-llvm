//===-- VIAMPPMCAsmInfo.cpp - TJP asm properties -----------------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------------------------------------------------------===//

#include "VIAMPPMCAsmInfo.h"
//#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Triple.h"

using namespace llvm;

void VIAMPPMCAsmInfo::anchor() {}

VIAMPPMCAsmInfo::VIAMPPMCAsmInfo(const Triple &TT) {
  CodePointerSize = 8;
  IsLittleEndian = false;
  CommentString = "//";
  SupportsDebugInformation = true;
  PrivateLabelPrefix = "";
}
