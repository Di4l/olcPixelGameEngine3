#pragma once

#include "config.h"
#include "pixel.h"
#include "vector2d.h"
#include "vector4d.h"
#include "matrix3d.h"
#include "matrix4d.h"
#include "transform2d.h"
#include "window.h"
#include "core.h"

#include "image.h"
#include "imload_iface.h"
#if OLC_HOST == OLC_HOST_WINDOWS
#include "imload_wingdi.h"
#endif

#if OLC_HOST == OLC_HOST_MACOS
#include "imload_macos.h"
#endif

#include "gputask.h"
#include "gpu_iface.h"

#if OLC_GPU == OLC_GPU_NONE
#include "gpu_none.h"
#endif

#if OLC_GPU == OLC_GPU_OPENGL33
#include "gpu_opengl33.h"
#endif




