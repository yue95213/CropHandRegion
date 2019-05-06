#pragma once

#define MAX_DEPTH  650 // assume hand region depth values are smaller than this value
#define MIN_DEPTH  50 // assume hand region depth values are larger than this value
#define MIN_DIFF   20  // the difference threhold between hand depth image and background depth image
#define TH_OUTLIER   400  // threshold to remove outliers 
#define HALF_LENGTH   130  // fixed-size cube in 3D (camera coordinates)

Rect ROI(120,80,1050,550);
float background_center_depth;
Rect crop_ROI(ROI);