set(NS3_LIBS
  libns3-dev-network
  libns3-dev-internet
  libns3-dev-applications
  libns3-dev-core
  libns3-dev-point-to-point
  libns3-dev-traffic-control
)

build_lib_example(
  NAME bandwidth-variation
  SOURCE_FILES bandwidth-variation.cc
  LIBRARIES_TO_LINK ${NS3_LIBS}
) 