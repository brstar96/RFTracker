#pragma warning(disable: 4996)

#include "Socket.h"

#include <boost/make_shared.hpp>
#include <iostream>
#include <conio.h>
#include <math.h>
#include <queue>
#include <time.h>

#include <limits>
#include <vector>
#include <Eigen/Core>

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/point_representation.h>
#include <pcl/PCLPointCloud2.h>

#include <pcl/io/pcd_io.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl\common\time.h>

#include <pcl/features/fpfh_omp.h>
#include <pcl/features/normal_3d_omp.h>
#include <pcl/features/integral_image_normal.h>
#include <pcl/registration/sample_consensus_prerejective.h>
#include <pcl\registration\ia_ransac.h>

#include <pcl/filters/filter.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/filters/passthrough.h>
#include <pcl\filters\statistical_outlier_removal.h>
#include <pcl/filters/extract_indices.h>

#include <pcl/registration/icp.h>
#include <pcl/registration/transforms.h>

#include <pcl/segmentation/sac_segmentation.h>

#include <pcl\kdtree\kdtree_flann.h>
#include <pcl\octree\octree.h>

#include "Kinect_grabber.h"
#include "organized_segmentation.h"



using namespace std;
using pcl::visualization::PointCloudColorHandlerCustom;
using pcl::visualization::PointCloudColorHandlerRGBField;
using namespace pcl;
using namespace pcl::io;
using namespace pcl::console;

typedef pcl::PointXYZ PointT;
typedef pcl::PointNormal PointNormalT;
typedef pcl::PointCloud<PointNormalT> PointCloudWithNormals;