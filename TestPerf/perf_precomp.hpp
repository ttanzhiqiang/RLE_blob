#ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wmissing-declarations"
#  if defined __clang__ || defined __APPLE__
#    pragma GCC diagnostic ignored "-Wmissing-prototypes"
#    pragma GCC diagnostic ignored "-Wextra"
#  endif
#endif

#ifndef __OPENCV_PERF_PRECOMP_HPP__
#define __OPENCV_PERF_PRECOMP_HPP__

#include "opencv2/ts/ts.hpp"
//#include "opencv2/contrib/contrib.hpp"
#include "blob.h"
#include "morph.h"
#include "HalconCpp.h"
#include "opencv_halcon.hpp"

using namespace std;
using namespace cv;
using namespace perf;
using namespace blob;
using namespace cvt;

#ifdef GTEST_CREATE_SHARED_LIBRARY
#error no modules except ts should have GTEST_CREATE_SHARED_LIBRARY defined
#endif

#endif
