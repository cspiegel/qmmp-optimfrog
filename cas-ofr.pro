HEADERS += decoderfactory.h decoder.h metadatamodel.h frogwrap.h
SOURCES += decoder.cpp decoderfactory.cpp metadatamodel.cpp frogwrap.cpp

CONFIG += warn_on plugin c++11

TEMPLATE = lib

QMAKE_CLEAN += lib$${TARGET}.so

unix {
  CONFIG += link_pkgconfig
  PKGCONFIG += qmmp

  QMMP_PREFIX = $$system(pkg-config qmmp --var=prefix)
  INCLUDEPATH += $${QMMP_PREFIX}/include
  INCLUDEPATH += $${OPTIMFROG_PATH}
  LIBS += $${OPTIMFROG_PATH}/Library/libOptimFROG.so
  QMAKE_RPATHDIR += $${OPTIMFROG_PATH}/Library

  plugin.path = $${QMMP_PREFIX}/lib/qmmp/Input
  plugin.files = libcas-ofr.so
  INSTALLS += plugin
}
