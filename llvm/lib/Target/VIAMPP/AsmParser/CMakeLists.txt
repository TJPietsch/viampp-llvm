add_llvm_component_library(LLVMVIAMPPAsmParser
  VIAMPPAsmParser.cpp

  LINK_COMPONENTS
  MC
  MCPARSER
  Support
  VIAMPPDesc
  VIAMPPInfo

  ADD_TO_COMPONENT
  VIAMPP
  )
