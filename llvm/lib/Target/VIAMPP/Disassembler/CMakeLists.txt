add_llvm_COMPONENT_library(LLVMVIAMPPDisassembler
  VIAMPPDisassembler.cpp

  LINK_COMPONENTS
  MCDisassembler
  Support
  VIAMPPInfo

  ADD_TO_COMPONENT
  VIAMPP
  )
