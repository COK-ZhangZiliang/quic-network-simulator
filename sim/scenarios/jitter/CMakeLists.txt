set(NS3_LIBS
  libns3-dev-network
  libns3-dev-internet
  libns3-dev-core
  libns3-dev-point-to-point
)

build_lib_example(
  NAME jitter
  SOURCE_FILES jitter.cc jitter-channel.cc jitter-point-to-point-helper.cc
  LIBRARIES_TO_LINK ${NS3_LIBS}
) 