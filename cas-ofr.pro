HEADERS += decoderfactory.h decoder.h metadatamodel.h frogwrap.h
SOURCES += decoder.cpp decoderfactory.cpp metadatamodel.cpp frogwrap.cpp

CONFIG += warn_on plugin c++11

TEMPLATE = lib

QMAKE_CLEAN += lib$${TARGET}.so

unix {
  CONFIG += link_pkgconfig
  PKGCONFIG += qmmp optimfrog

  QMMP_PREFIX = $$system(pkg-config qmmp --variable=prefix)
  PLUGIN_DIR = $$system(pkg-config qmmp --variable=plugindir)/Input
  LOCAL_INCLUDES = $${QMMP_PREFIX}/include
  LOCAL_INCLUDES -= $$QMAKE_DEFAULT_INCDIRS
  INCLUDEPATH += $$LOCAL_INCLUDES

  plugin.path = $${PLUGIN_DIR}
  plugin.files = libcas-ofr.so
  INSTALLS += plugin
}
