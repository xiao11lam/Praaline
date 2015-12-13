#ifndef PLUGIN_FLORALPFC_GLOBAL_H
#define PLUGIN_FLORALPFC_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PLUGIN_FLORALPFC_LIBRARY)
#  define PLUGIN_FLORALPFC_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define PLUGIN_FLORALPFC_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // PLUGIN_FLORALPFC_GLOBAL_H
