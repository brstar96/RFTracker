#include "stdafx.h"

int main()
{
	pcl::visualization::PCLVisualizer *viewer = new pcl::visualization::PCLVisualizer("Viewer");
	viewer->setSize(1600, 800);
	viewer->initCameraParameters();
	viewer->loadCameraParameters("Initial_Cam_Param.cam");
	viewer->addCoordinateSystem(0.3);

	pcl::PointCloud<pcl::PointXYZ>::Ptr Cube(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr Doll(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::io::loadPCDFile("Doll_Scaled_10.pcd", *Doll);
	pcl::io::loadPCDFile("Textured_Cube.pcd", *Cube);


	viewer->addPointCloud(Doll, "Doll");
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 0, 255, 255, "Doll");
	viewer->addPointCloud(Cube, "Cube");
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 255, 0, 255, "Cube");

	viewer->spin();
}