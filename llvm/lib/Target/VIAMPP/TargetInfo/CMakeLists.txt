add_llvm_component_library(LLVMVIAMPPInfo
  VIAMPPTargetInfo.cpp

  LINK_COMPONENTS
  Support

  ADD_TO_COMPONENT
  VIAMPP
  )

