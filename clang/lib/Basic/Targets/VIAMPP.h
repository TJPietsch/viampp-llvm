//===--- VIAMPP.h - Declare ARC target feature support -------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares VIAMPP TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_BASIC_TARGETS_VIAMPP_H
#define LLVM_CLANG_LIB_BASIC_TARGETS_VIAMPP_H

#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/Compiler.h"

namespace clang {
namespace targets {

class LLVM_LIBRARY_VISIBILITY VIAMPPTargetInfo : public TargetInfo {
public:
  VIAMPPTargetInfo(const llvm::Triple &Triple, const TargetOptions &)
      : TargetInfo(Triple) {
	PointerWidth = PointerAlign = 64;
	BoolWidth = BoolAlign = 64;
	CharWidth = 8;			// for Viampp
        CharAlign = 64;			// for Viampp
	ShortWidth = ShortAlign = 64;	// for Viampp
	IntWidth = IntAlign = 64;
	HalfWidth = HalfAlign = 64;
	BFloat16Width = BFloat16Align = 64;
	FloatWidth = FloatAlign = 64;
	DoubleWidth = DoubleAlign = 64;
	LongDoubleWidth = LongDoubleAlign = 64;
	Float128Align = 64;
	LargeArrayMinWidth = LargeArrayAlign = 64;
	LongWidth = LongAlign = 64;
	LongLongWidth = LongLongAlign = 64;

	BigEndian = true;
	TLSSupported = false;
	NoAsmVariants = true;
	
	SizeType = UnsignedLongLong;
	PtrDiffType = SignedLongLong;
	IntPtrType = SignedLongLong;
	IntMaxType = SignedLongLong;
	Int64Type = SignedLongLong;
	WIntType = SignedLongLong;	// for Viampp
        Char16Type = UnsignedLongLong;	// for Viampp
	Char32Type = UnsignedLongLong;	// for Viampp

//    resetDataLayout("E-m:e-p:64:64-S64-i1:64:64-i8:64:64-i16:64:64-"
//                    "i32:64:64-i64:64:64-f16:64:64-f32:64:64-f64:64:64-a:0:64-n64");
    resetDataLayout("E-m:e-p:64:64-i1:64:64-i8:64:64-i16:64:64-"
                    "i32:64:64-i64:64:64-f16:64:64-f32:64:64-f64:64:64-a:0:64-n64");
  }

  void getTargetDefines(const LangOptions &Opts,
                        MacroBuilder &Builder) const override;

  ArrayRef<Builtin::Info> getTargetBuiltins() const override {
    return None;
  }

  BuiltinVaListKind getBuiltinVaListKind() const override {
    return TargetInfo::VoidPtrBuiltinVaList;
  }

  const char *getClobbers() const override {
	return "";
  }

  ArrayRef<const char *> getGCCRegNames() const override {
    return None;
  }

  ArrayRef<TargetInfo::GCCRegAlias> getGCCRegAliases() const override {
    return None;
  }

  bool validateAsmConstraint(const char *&Name,
                             TargetInfo::ConstraintInfo &Info) const override {
    return false;
  }

  bool hasExtIntType() const override { return true; }
};

} // namespace targets
} // namespace clang

#endif // LLVM_CLANG_LIB_BASIC_TARGETS_VIAMPP_H

