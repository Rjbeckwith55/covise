# ADD_DEFINITIONS()
# INCLUDE_DIRECTORIES()

SET(HEADERS )

SET(SOURCES
  RadialScale.cpp
)

SET(EXTRASOURCES  )

ADD_COVISE_MODULE(Converter RadialScale ${EXTRASOURCES} )
TARGET_LINK_LIBRARIES(RadialScale  coApi coAppl coCore )

COVISE_INSTALL_TARGET(RadialScale)
