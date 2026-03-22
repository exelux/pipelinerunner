#pragma once

#include <QtCore/qglobal.h>

#if defined(TOOLKIT_LIBRARY)
    #define TOOLKIT_EXPORT Q_DECL_EXPORT
#else
    #define TOOLKIT_EXPORT Q_DECL_IMPORT
#endif
