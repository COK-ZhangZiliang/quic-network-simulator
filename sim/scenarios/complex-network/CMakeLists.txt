set(NS3_LIBS
  libns3-dev-network
  libns3-dev-internet
  libns3-dev-applications
  libns3-dev-core
  libns3-dev-point-to-point
)

build_lib_example(
  NAME complex-network
  SOURCE_FILES complex-network.cc complex-helper.cc complex-channel.cc complex-error-model.cc
  LIBRARIES_TO_LINK ${NS3_LIBS}
) 