#pragma once
#include <QtCore/qglobal.h>

#if defined(RABBIT_MQ_LIBRARY)
    #define RABBIT_MQ_EXPORT Q_DECL_EXPORT
#else
    #define RABBIT_MQ_EXPORT Q_DECL_IMPORT
#endif
