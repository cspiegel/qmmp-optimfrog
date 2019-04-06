HEADERS += decoderfactory.h decoder.h metadatamodel.h frogwrap.h
SOURCES += decoder.cpp decoderfactory.cpp metadatamodel.cpp frogwrap.cpp

CONFIG += warn_on plugin c++11

TEMPLATE = lib

QMAKE_CLEAN += lib$${TARGET}.so

unix {
  CONFIG += link_pkgconfig
  PKGCONFIG += qmmp optimfrog

  QMMP_PREFIX = $$system(pkg-config qmmp --var=prefix)
  PLUGIN_DIR = $$system(pkg-config qmmp --var=plugindir)/Input
  INCLUDEPATH += $${QMMP_PREFIX}/include

  plugin.path = $${PLUGIN_DIR}
  plugin.files = libcas-ofr.so
  INSTALLS += plugin
}
