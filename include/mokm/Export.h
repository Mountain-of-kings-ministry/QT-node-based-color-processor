#pragma once

#include <QtCore/qglobal.h>

#if defined(MOKM_LIBRARY)
#  define MOKM_EXPORT Q_DECL_EXPORT
#else
#  define MOKM_EXPORT Q_DECL_IMPORT
#endif
