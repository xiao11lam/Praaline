#ifndef PLUGINPROSOGRAM_GLOBAL_H
#define PLUGINPROSOGRAM_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PLUGIN_PROSOGRAM_LIBRARY)
#  define PLUGIN_PROSOGRAM_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define PLUGIN_PROSOGRAM_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // PLUGINPROSOGRAM_GLOBAL_H
