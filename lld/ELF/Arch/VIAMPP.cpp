//===- VIAMPP.cpp ---------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//

#include "InputFiles.h"
#include "Symbols.h"
#include "Target.h"
#include "lld/Common/ErrorHandler.h"
#include "llvm/Object/ELF.h"
#include "llvm/Support/Endian.h"

#include "llvm/Support/ErrorHandling.h"

using namespace llvm;
using namespace llvm::object;
using namespace llvm::support::endian;
using namespace llvm::ELF;
using namespace lld;
using namespace lld::elf;

namespace {
class VIAMPP final : public TargetInfo {
public:
  VIAMPP();
	RelExpr getRelExpr(RelType type, const Symbol &s, const uint8_t *loc) const override;
	void relocate(uint8_t *loc, const Relocation &rel, uint64_t val) const override;
};
} // namespace

VIAMPP::VIAMPP() {
	noneRel = R_VIAMPP_NONE;

	defaultImageBase = 0x0;
}

RelExpr VIAMPP::getRelExpr(RelType type, const Symbol &s, const uint8_t *loc) const {
	switch (type) {
	case R_VIAMPP_NONE:
		return R_NONE;
	case R_VIAMPP_16_pcrel:
	case R_VIAMPP_32_pcrel:
	case R_VIAMPP_64_pcrel:
		return R_PC;
	case R_VIAMPP_HI:
	case R_VIAMPP_LO:
//    return R_PC;
return R_ABS;
	case R_VIAMPP_64_abs:
	default:
		return R_ABS;
  }
}

void VIAMPP::relocate(uint8_t *loc, const Relocation &rel, uint64_t val) const {
	switch (rel.type) {
	case R_VIAMPP_16_pcrel:
		write16be(loc, (read64(loc) | ((val / 8) & 0x000000000000FFFF)));
		break;
	case R_VIAMPP_32_pcrel:
		write64be(loc, (read64(loc) | ((val / 8) & 0x00000000FFFFFFFF)));
		break;
	case R_VIAMPP_64_pcrel:
		write64be(loc, val);
		break;
	case R_VIAMPP_HI:		// base on .viampp section (amount of used registers)
		llvm::outs() << "R_VIAMPP_HI: val: " << val << "\n";
		write64be(loc, (read64(loc) | (((val / 8) >> 32) & 0x00000000FFFFFFFF)));
		break;
	case R_VIAMPP_LO:		 // base on .viampp section (amount of used registers)
		llvm::outs() << "R_VIAMPP_LO: val: " << val << "\n";
		write64be(loc, (read64(loc) | ((val / 8) & 0x00000000FFFFFFFF)));
		break;
	case R_VIAMPP_64_abs:
		write64be(loc, val);
		break;

	default:
		error(getErrorLocation(loc) + "unrecognized relocation " + toString(rel.type));
  }
}

TargetInfo *elf::getVIAMPPTargetInfo() {
	static VIAMPP target;
	return &target;
}
