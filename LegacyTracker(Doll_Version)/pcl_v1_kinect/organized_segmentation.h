#ifndef PCL_ORGANIZED_SEGMENTATION_DEMO_H_
#define PCL_ORGANIZED_SEGMENTATION_DEMO_H_

// Boost
#include <boost/thread/thread.hpp>
// PCL
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/features/integral_image_normal.h>
#include <pcl/segmentation/organized_multi_plane_segmentation.h>z
#include <pcl/common/transforms.h>
#include <pcl/segmentation/euclidean_cluster_comparator.h>
#include <pcl/segmentation/organized_connected_component_segmentation.h>

typedef pcl::PointXYZ PointK;

class OrganizedSegmentationDemo
{
public:
	typedef pcl::PointCloud<PointK> Cloud;
	typedef Cloud::Ptr CloudPtr;
	typedef Cloud::ConstPtr CloudConstPtr;


	OrganizedSegmentationDemo();

	~OrganizedSegmentationDemo()
	{
	}

	void cloud_cb(pcl::PointCloud<PointK>::Ptr);

protected:
	boost::shared_ptr<pcl::visualization::PCLVisualizer> vis_;

	pcl::PointCloud<PointK> prev_cloud_;
	pcl::PointCloud<pcl::Normal> prev_normals_;
	std::vector<pcl::PlanarRegion<PointK>, Eigen::aligned_allocator<pcl::PlanarRegion<PointK> > > prev_regions_;
	float* prev_distance_map_;

	pcl::PointCloud<PointK>::CloudVectorType prev_clusters_;

	pcl::NormalEstimationOMP <PointK, pcl::Normal> ne;

	pcl::OrganizedMultiPlaneSegmentation<PointK, pcl::Normal, pcl::Label> mps;

	bool capture_;
	bool data_modified_;
	size_t previous_data_size_;
	size_t previous_clusters_size_;

	bool display_normals_;
	bool display_curvature_;
	bool display_distance_map_;

	bool use_planar_refinement_;
	bool use_clustering_;

	pcl::EuclideanClusterComparator<PointK, pcl::Normal, pcl::Label>::Ptr euclidean_cluster_comparator_;

public:


	void toggleCapturePressed()
	{
		capture_ = !capture_;
	}

	void usePlaneComparatorPressed();
	void useEuclideanComparatorPressed();
	void useRGBComparatorPressed();
	void useEdgeAwareComparatorPressed();

	void disableRefinementPressed()
	{
		use_planar_refinement_ = false;
	}

	void usePlanarRefinementPressed()
	{
		use_planar_refinement_ = true;
	}

	void disableClusteringPressed()
	{
		use_clustering_ = false;
	}

	void useEuclideanClusteringPressed()
	{
		use_clustering_ = true;
	}
};

#endif    // PCL_ORGANIZED_SEGMENTATION_DEMO_H_