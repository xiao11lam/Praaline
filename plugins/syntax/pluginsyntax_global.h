#ifndef PLUGINSYNTAX_GLOBAL_H
#define PLUGINSYNTAX_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PLUGIN_SYNTAX_LIBRARY)
#  define PLUGIN_SYNTAX_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define PLUGIN_SYNTAX_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // PLUGINSYNTAX_GLOBAL_H
