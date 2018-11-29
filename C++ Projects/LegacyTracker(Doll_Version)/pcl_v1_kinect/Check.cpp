#include "stdafx.h"
pcl::PointCloud<pcl::PointXYZ>::Ptr Full_Cloud(new pcl::PointCloud<pcl::PointXYZ>);

int main()
{
	float Grid_min[3] = { -0.6, -0.01, 1.6 };
	float Grid_max[3] = { 0.0, 0.6, 1.9 };
	pcl::PassThrough<pcl::PointXYZ> Axis_Filter;
	Axis_Filter.setKeepOrganized(false);

	boost::shared_ptr<pcl::Grabber> grabber = boost::make_shared<pcl::Kinect2Grabber>();
	pcl::PointCloud<pcl::PointXYZ>::ConstPtr Kinect_cloud(new pcl::PointCloud<pcl::PointXYZ>);
	boost::function<void(const pcl::PointCloud<pcl::PointXYZ>::ConstPtr&)> function =
		[&Kinect_cloud](const pcl::PointCloud<pcl::PointXYZ>::ConstPtr &ptr){
		Kinect_cloud = ptr;
		pcl::copyPointCloud(*Kinect_cloud, *Full_Cloud);
	};
	boost::signals2::connection connection = grabber->registerCallback(function);
	pcl::visualization::PCLVisualizer *viewer = new pcl::visualization::PCLVisualizer("Viewer");

	int RunTime_Window, Info_Window, Surface_Window;
	viewer->createViewPort(0.25, 0.0, 1.0, 1.0, RunTime_Window);
	string Axis_Filter_Field[3] = { "x", "y", "z" };
	viewer->initCameraParameters();
	viewer->loadCameraParameters("Initial_Cam_Param.cam");
	grabber->start();

	while (!viewer->wasStopped())
	{
		Axis_Filter.setInputCloud(Full_Cloud);
		for (int i = 0; i < 3; i++)
		{
			Axis_Filter.setFilterFieldName(Axis_Filter_Field[i]);
			Axis_Filter.setFilterLimits(Grid_min[i], Grid_max[i]);
			Axis_Filter.filter(*Full_Cloud);
			Axis_Filter.setInputCloud(Full_Cloud);
		}

		viewer->addPointCloud(Full_Cloud, "Fu1ll", RunTime_Window);
		viewer->spinOnce();
		viewer->removeAllPointClouds();
	}

}