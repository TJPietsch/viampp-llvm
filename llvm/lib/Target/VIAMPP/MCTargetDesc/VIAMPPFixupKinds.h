//===-- VIAMPPFixupKinds.h - VIAMPP Specific Fixup Entries ---------------------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===-------------------------------------------------------------------------------------------------------------------------===//

#ifndef VIAMPPFIXUPKINDS_H
#define VIAMPPFIXUPKINDS_H

#include "llvm/MC/MCFixup.h"

namespace llvm {
namespace VIAMPP {

  // This table *must* be in the same order of MCFixupKindInfo Infos[VIAMPP::NumTargetFixupKinds] in VIAMPPAsmBackend.cpp.
  enum Fixups {
	fixup_VIAMPP_NONE = FirstTargetFixupKind,
	
	fixup_VIAMPP_16_pcrel,		// 16-bit PC relative fixup
	fixup_VIAMPP_32_pcrel,		// 32-bit PC relative fixup
	fixup_VIAMPP_64_pcrel,		// 64-bit PC relative fixup
	fixup_VIAMPP_64_abs,		// 64-bit PC absolute fixup
	fixup_VIAMPP_HI,		// upper 32-bits of a symbolic relocation
	fixup_VIAMPP_LO,		// lower 32-bits of a symbolic relocation

    // Marker
    LastTargetFixupKind,
    NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
  };
} // namespace VIAMPP
} // namespace llvm


#endif
