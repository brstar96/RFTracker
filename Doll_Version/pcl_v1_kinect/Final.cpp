#include "stdafx.h"

#define Frame_Rate		1
#define IPC				1

#pragma region  
unsigned char red[6] = { 255, 0, 0, 255, 255, 0 };
unsigned char grn[6] = { 0, 255, 0, 255, 0, 255 };
unsigned char blu[6] = { 0, 0, 255, 0, 255, 255 };
char text[1024];
char id[1024];

void keyboard_callback(const::visualization::KeyboardEvent& event, void*);
void gotoxy(int x, int y);

pcl::PointCloud<pcl::PointXYZ>::Ptr Full_Cloud(new pcl::PointCloud<pcl::PointXYZ>);
pcl::PointCloud<pcl::PointXYZ>::Ptr Grid_Pivot(new pcl::PointCloud<pcl::PointXYZ>);
pcl::PointCloud<pcl::PointXYZ>::Ptr Result(new pcl::PointCloud<pcl::PointXYZ>);
pcl::PointCloud<pcl::PointXYZRGB>::Ptr Raw_XYZRGB_Cloud(new pcl::PointCloud<pcl::PointXYZRGB>);
pcl::PointCloud<pcl::PointXYZRGB>::Ptr Gray_Scaled_Cloud(new pcl::PointCloud<pcl::PointXYZRGB>);
Eigen::Matrix4f Euclidean, Translation, Rough_Translation, Matrix;
vector<Eigen::Matrix4f> Euclidean_Candidates;

double start = 0.0;
double end_ = 0.0;
double FPS_Start;
double FPS_Total = 0.0;
queue<double>* FPS_Time = new queue<double>();
clock_t Time_S, Time_E;
bool save_ = false;
int save_idx = 0;
double Sx, Sy, Sz;
#pragma endregion

#pragma region Visualization Command
#define Aligned_Virtual							0
#define Detectrion_Evaluation_Visualization		1
#define UI_2D_Visualization						1
#define UI_3D_Visualization						0		// not recommend
bool Enable_visualize_full_pointcloud =			true;
#pragma endregion

#pragma region Virtual Pre-Processing Params

#define Pivot_Rad				0.03
double Virtual_avg[3];

#pragma endregion 

#pragma region First Segmentation Params

bool Detecting = true;
bool Ready = false;

#define A_Cluster_Min_Size				400
#define A_Neighbor_Point_Angle_Th		3.0
#define A_Neighbor_Point_Distance_th	0.02
#define B_Cluster_Min_Size				500
#define B_Neighbor_Cluster_Angle_Th		2.0
#define B_Connected_Point_Distance_Th	0.01
#define C_Cluster_Min_Size				200
#define C_Cluster_Max_Size				1500
#define Normal_Smoothing_Th				10

#pragma endregion 

#pragma region Object Detection

#define Fitness_Th		1000.0
#define Outlier_Lv_1	0.003
#define Outlier_Lv_2		0.006
#define Outlier_Lv_3	0.015
#define Weight_Lv_1		3.0
#define Weight_Lv_2		7.0
#define Weight_Lv_3		15.0
#define N_F_Param_1_1	15
#define N_F_Param_1_2	2.5
#define N_F_Param_2_1	35
#define N_F_Param_2_2	0.2

#define Radian 3.1415 / 180.0

void Transformation_Matrix();
double Rotation_Angle[3] = { 0.0, 0.0, 0.0 };


#pragma endregion

#pragma region Detection Evaluation

#define Front_Pivot_Rad			0.005
#define Front_Pivot_Neighbor	5.0
#define Evaluation_Mothod		2		//( 1 : Total Normal gap compare  2 : Each pivot's Normal gap compare )


#pragma endregion

#pragma region Second Segmentation Params

#define Axis_Filtering_Rad		0.08

#pragma endregion 

#pragma region Rough Translation Params

#define Rough_Translation_Th 0.01

#pragma endregion

#pragma region Translate Estimation Param

#define Radius_Search_Th			128.0f
#define Normal_Calculate_Th			0.02
#define Feature_Calculate_Th		Normal_Calculate_Th + 0.005
#define Base_Rad_Prev				0.03
#define Base_Rad_Cur				0.05
#define Pivot_Rad_Prev				Base_Rad_Prev - Normal_Calculate_Th
#define OpenMP_Th					8
#define Correspondence_Search_Th	3
#define ICP_Activate_T_Th			0.01
#define ICP_Activate_R_Th			0.012
#define ICP_Activate_T_Delay			3
#define ICP_Activate_R_Delay			6//8

#pragma endregion 

#pragma region Translate Estimation Param
#define Surface_Search_Rad			0.03
#pragma endregion 

#pragma region ICP Tracking Params

#define Correspondence_Max_Dist			0.08
#define Refining_RANSAC_Dist_th			0.08
#define Tracking_Iteration				10000
#define Refining_RANSAC_Iteration		5000

#pragma endregion 

int main()
{
#pragma region 
	boost::shared_ptr<pcl::Grabber> grabber = boost::make_shared<pcl::Kinect2Grabber>();
	pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr Kinect_cloud(new pcl::PointCloud<pcl::PointXYZRGB>);
	boost::function<void(const pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr&)> function =
		[&Kinect_cloud](const pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr &ptr){
		Kinect_cloud = ptr;
		pcl::copyPointCloud(*Kinect_cloud, *Raw_XYZRGB_Cloud);
	};
	boost::signals2::connection connection = grabber->registerCallback(function);
	pcl::visualization::PCLVisualizer *viewer = new pcl::visualization::PCLVisualizer("Viewer");
	viewer->setSize(1600, 800);
	viewer->registerKeyboardCallback(&keyboard_callback, "keyboard");
	viewer->initCameraParameters();
	viewer->loadCameraParameters("Initial_Cam_Param.cam");

	int RunTime_Window, Info_Window, Surface_Window;
	viewer->createViewPort(0.25, 0.0, 1.0, 1.0, RunTime_Window);
	viewer->createViewPort(0.0, 0.0, 0.25, 1.0, Info_Window);

	Grid_Pivot->resize(8);

	pcl::KdTreeFLANN<pcl::PointXYZ> Search;
	pcl::PointIndices::Ptr Index(new pcl::PointIndices);
	vector<float> D;
	vector<int> idx;

	pcl::PointCloud<pcl::PointXYZ>::Ptr Text_Pos(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr Cloud(new pcl::PointCloud<pcl::PointXYZ>);
	Text_Pos->resize(1);

	float Grid_min[3];
	float Grid_max[3];
#pragma endregion 

#pragma region Virtual_Pre_Processing Elements

#pragma region Cloud
	pcl::PointCloud<pcl::PointXYZ>::Ptr Virtual_Copy(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr Virtual_Full(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr Virtual_Center(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr Virtual_Axis(new pcl::PointCloud<pcl::PointXYZ>);
#pragma endregion
	pcl::io::loadPCDFile("Doll_Scaled_10.pcd", *Virtual_Full);

	Virtual_Center->resize(1);

	Virtual_Center->points[0].x = 0.0;
	Virtual_Center->points[0].y = 0.0;
	Virtual_Center->points[0].z = 0.0;

	for (int i = 0; i < Virtual_Full->size(); i++)
	{
		Virtual_Center->points[0].x += Virtual_Full->points[i].x;
		Virtual_Center->points[0].y += Virtual_Full->points[i].y;
		Virtual_Center->points[0].z += Virtual_Full->points[i].z;
	}

	Virtual_Center->points[0].x /= Virtual_Full->size();
	Virtual_Center->points[0].y /= Virtual_Full->size();
	Virtual_Center->points[0].z /= Virtual_Full->size();

	Virtual_avg[0] = Virtual_Center->points[0].x;
	Virtual_avg[1] = Virtual_Center->points[0].y + 0.005;
	Virtual_avg[2] = Virtual_Center->points[0].z - 0.01;

	Virtual_Axis->resize(4);
	Virtual_Axis->points[0].x = 0.0;
	Virtual_Axis->points[0].y = 0.0;
	Virtual_Axis->points[0].z = 0.0;

	Virtual_Axis->points[1].x = 0.1;
	Virtual_Axis->points[1].y = 0.0;
	Virtual_Axis->points[1].z = 0.0;

	Virtual_Axis->points[2].x = 0.0;
	Virtual_Axis->points[2].y = 0.1;
	Virtual_Axis->points[2].z = 0.0;

	Virtual_Axis->points[3].x = 0.0;
	Virtual_Axis->points[3].y = 0.0;
	Virtual_Axis->points[3].z = -0.1;
#pragma endregion

#pragma region First Segmentation Elements

#pragma region Method
	pcl::IntegralImageNormalEstimation<pcl::PointXYZ, pcl::Normal> Fast_Normal_Estimator;
	pcl::OrganizedMultiPlaneSegmentation<pcl::PointXYZ, pcl::Normal, pcl::Label> A_Point_to_Cluster;
	pcl::EuclideanClusterComparator<pcl::PointXYZ, pcl::Normal, pcl::Label>::Ptr B_Cluster_to_Component(new pcl::EuclideanClusterComparator<pcl::PointXYZ, pcl::Normal, pcl::Label>());
	pcl::OrganizedConnectedComponentSegmentation<pcl::PointXYZ, pcl::Label> C_Component_to_ConnectedComponent(B_Cluster_to_Component);

	Fast_Normal_Estimator.setNormalEstimationMethod(Fast_Normal_Estimator.AVERAGE_3D_GRADIENT);
	Fast_Normal_Estimator.setNormalSmoothingSize(Normal_Smoothing_Th);

	A_Point_to_Cluster.setMinInliers(A_Cluster_Min_Size);
	A_Point_to_Cluster.setAngularThreshold(pcl::deg2rad(A_Neighbor_Point_Angle_Th));
	A_Point_to_Cluster.setDistanceThreshold(A_Neighbor_Point_Distance_th);

	B_Cluster_to_Component->setAngularThreshold(pcl::deg2rad(B_Neighbor_Cluster_Angle_Th));
	B_Cluster_to_Component->setDistanceThreshold(B_Connected_Point_Distance_Th, true);
#pragma endregion 

#pragma region Vars
	vector<pcl::PlanarRegion<pcl::PointXYZ>, Eigen::aligned_allocator<pcl::PlanarRegion<pcl::PointXYZ> > > A_Regions;
	vector<pcl::ModelCoefficients> A_Model_coefficients;
	vector<pcl::PointIndices> A_Inliers;
	vector<pcl::PointIndices> A_Label_indices;
	vector<pcl::PointIndices> A_Boundary_indices;
	vector<bool> B_Plane_labels;
	vector<pcl::PointIndices> C_Euclidean_label_indices;
#pragma endregion 

#pragma region Cloud
	pcl::PointCloud<pcl::PointXYZ>::Ptr A_Full_Cloud(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::Normal>::Ptr A_Full_Cloud_Normal(new pcl::PointCloud<pcl::Normal>);
	pcl::PointCloud<pcl::Label>::Ptr A_Labels(new pcl::PointCloud<pcl::Label>);
	pcl::PointCloud<pcl::PointXYZ>::CloudVectorType C_Clusters;
	pcl::PointCloud<pcl::Label> C_Euclidean_labels;
#pragma endregion 

#pragma endregion

#pragma region Object Detection Elements
#pragma region Cloud
	pcl::PointCloud<pcl::PointXYZ>::CloudVectorType Virtual_Initial_Full;

	pcl::PointCloud<pcl::PointXYZ>::Ptr Virtual_Pivot(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::CloudVectorType Virtual_Pivot_Cloud;
	pcl::PointCloud<pcl::PointXYZ>::CloudVectorType Virtual_Initial_Pivot;
	pcl::PointCloud<pcl::PointXYZ>::CloudVectorType Virtual_Initial_Pivot_Cloud[20];

	pcl::PointCloud<pcl::PointXYZ>::Ptr Virtual_Surface_Pivot(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::CloudVectorType Virtual_Initial_Surface_Pivot;

	pcl::PointCloud<pcl::PointXYZ>::Ptr Virtual_Inner_Pivot(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::CloudVectorType Virtual_Initial_Inner_Pivot;

	pcl::PointCloud<pcl::PointXYZ>::CloudVectorType Virtual_Initial_Axis;

	pcl::PointCloud<pcl::PointXYZ>::Ptr Virtual_Front_Pivot(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::CloudVectorType Virtual_Front_Pivot_Cloud;
	pcl::PointCloud<pcl::PointXYZ>::CloudVectorType Virtual_Candidate_Full;
	pcl::PointCloud<pcl::PointXYZ>::CloudVectorType Virtual_Candidate_Center;
	pcl::PointCloud<pcl::PointXYZ>::CloudVectorType Virtual_Candidate_Pivot;
	vector<pcl::PointCloud<pcl::PointXYZ>::CloudVectorType> Virtual_Candidate_Pivot_Cloud;
	pcl::PointCloud<pcl::PointXYZ>::CloudVectorType Virtual_Candidate_Surface_Pivot;
	pcl::PointCloud<pcl::PointXYZ>::CloudVectorType Virtual_Candidate_Inner_Pivot;

	pcl::PointCloud<pcl::PointXYZ>::CloudVectorType Virtual_Candidate_Axis;

	pcl::PointCloud<pcl::Normal>::Ptr Virtual_Normal(new pcl::PointCloud<pcl::Normal>);


	pcl::PointCloud<pcl::PointXYZ>::Ptr Physical_Full(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr Filtered_Physical(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::CloudVectorType Physical_Candidate_Full;
	pcl::PointCloud<pcl::PointXYZ>::CloudVectorType Physical_Candidate_Grid;
	pcl::PointCloud<pcl::Normal>::Ptr Physical_Normal(new pcl::PointCloud<pcl::Normal>);
#pragma endregion

#pragma region Method
	pcl::StatisticalOutlierRemoval<pcl::PointXYZ> Noise_Filter;
	Noise_Filter.setMeanK(N_F_Param_1_1);
	Noise_Filter.setStddevMulThresh(N_F_Param_1_2);
#pragma endregion

#pragma region Vars
	std::vector<pcl::PointIndices> Outlier_Index;
	pcl::PointCloud<pcl::PointXYZ>::CloudVectorType Outlier_Cloud;
	Outlier_Index.resize(3);
	Outlier_Cloud.resize(3);

	pcl::PointCloud<pcl::Normal>::Ptr Physical_Normal_Avg(new pcl::PointCloud<pcl::Normal>);
	pcl::PointCloud<pcl::Normal>::Ptr Virtual_Normal_Avg(new pcl::PointCloud<pcl::Normal>);

	Physical_Normal_Avg->resize(1);
	Virtual_Normal_Avg->resize(1);
	double Physical_Center[3] = { 0.0, 0.0, 0.0 };

	double Min_Fitness;
	double Fitness;
	int Matched_Idx;

	double Final_Physical_Idx = 0;
	vector<double> Norm_Gap_Avg;
	vector<int> Front_Pivot_Cnt;
#pragma endregion	

#pragma region Pre Processing
	pcl::io::loadPCDFile("Surface_Pivot_Recognizing_Scaled_10.pcd", *Virtual_Pivot);
	pcl::io::loadPCDFile("Surface_Pivot_Scaled_10.pcd", *Virtual_Surface_Pivot);
	pcl::io::loadPCDFile("Inner_Pivot_Scaled_10.pcd", *Virtual_Inner_Pivot);

	Search.setInputCloud(Virtual_Full);
	for (int i = 0; i < Virtual_Pivot->size(); i++)
	{
		Search.radiusSearch(Virtual_Pivot->points[i], 0.02, Index->indices, D);
		pcl::copyPointCloud(*Virtual_Full, Index->indices, *Virtual_Copy);
		Virtual_Pivot_Cloud.push_back(*Virtual_Copy);
	}
	/*for (int i = 0; i < Virtual_Surface_Pivot->size(); i++)
	{
		Search.radiusSearch(Virtual_Surface_Pivot->points[i], 0.02, Index->indices, D);
		pcl::copyPointCloud(*Virtual_Full, Index->indices, *Virtual_Copy);
		Virtual_Surface_Pivot_Cloud.push_back(*Virtual_Copy);
	}*/

	for (int i = 0; i < 3; i++)
	{
		Rotation_Angle[0] = Rotation_Angle[1] = Rotation_Angle[2] = 0.0;

		if (i != 1)
			Rotation_Angle[i] = 90.0;

		for (int a = 0; a < 4; a++)
		{
			if (i == 2)
				Rotation_Angle[0] = 90.0 * a;
			else
				Rotation_Angle[1] = 90.0 * a;

			Transformation_Matrix();

			pcl::transformPointCloud(*Virtual_Full, *Virtual_Copy, Matrix);
			Virtual_Initial_Full.push_back(*Virtual_Copy);

			pcl::transformPointCloud(*Virtual_Pivot, *Virtual_Copy, Matrix);
			Virtual_Initial_Pivot.push_back(*Virtual_Copy);

			for (int j = 0; j < Virtual_Pivot_Cloud.size(); j++)
			{
				pcl::transformPointCloud(Virtual_Pivot_Cloud[j], *Virtual_Copy, Matrix);
				Virtual_Initial_Pivot_Cloud[Virtual_Initial_Full.size() - 1].push_back(*Virtual_Copy);
			}

			pcl::transformPointCloud(*Virtual_Surface_Pivot, *Virtual_Copy, Matrix);
			Virtual_Initial_Surface_Pivot.push_back(*Virtual_Copy);

			pcl::transformPointCloud(*Virtual_Inner_Pivot, *Virtual_Copy, Matrix);
			Virtual_Initial_Inner_Pivot.push_back(*Virtual_Copy);

			pcl::transformPointCloud(*Virtual_Axis, *Virtual_Copy, Matrix);
			Virtual_Initial_Axis.push_back(*Virtual_Copy);
		}

		if (i != 1)
		{
			Rotation_Angle[i] = 90.0 * -1;
			for (int a = 0; a < 4; a++)
			{
				if (i == 2)
					Rotation_Angle[0] = 90.0 * a;
				else
					Rotation_Angle[1] = 90.0 * a;

				Transformation_Matrix();

				pcl::transformPointCloud(*Virtual_Full, *Virtual_Copy, Matrix);
				Virtual_Initial_Full.push_back(*Virtual_Copy);

				pcl::transformPointCloud(*Virtual_Pivot, *Virtual_Copy, Matrix);
				Virtual_Initial_Pivot.push_back(*Virtual_Copy);

				for (int j = 0; j < Virtual_Pivot_Cloud.size(); j++)
				{
					pcl::transformPointCloud(Virtual_Pivot_Cloud[j], *Virtual_Copy, Matrix);
					Virtual_Initial_Pivot_Cloud[Virtual_Initial_Full.size() - 1].push_back(*Virtual_Copy);
				}

				pcl::transformPointCloud(*Virtual_Surface_Pivot, *Virtual_Copy, Matrix);
				Virtual_Initial_Surface_Pivot.push_back(*Virtual_Copy);

				pcl::transformPointCloud(*Virtual_Inner_Pivot, *Virtual_Copy, Matrix);
				Virtual_Initial_Inner_Pivot.push_back(*Virtual_Copy);

				pcl::transformPointCloud(*Virtual_Axis, *Virtual_Copy, Matrix);
				Virtual_Initial_Axis.push_back(*Virtual_Copy);
			}
		}
	}
#pragma endregion
#pragma endregion

#pragma region Second Segmentation Elements 

#pragma region Method
	pcl::PassThrough<pcl::PointXYZ> Axis_Filter;
	Axis_Filter.setKeepOrganized(false);
#pragma endregion

#pragma region Var
	string Axis_Filter_Field[3] = { "x", "y", "z" };

	int Grid_Line_Idx_Start[12] = { 1, 3, 4, 2, 5, 8, 7, 6, 3, 4, 2, 1 };
	int Grid_Line_Idx_End[12] = { 3, 4, 2, 1, 8, 7, 6, 5, 5, 8, 7, 6 };
#pragma endregion

#pragma region Cloud
	pcl::PointCloud<pcl::PointXYZ>::Ptr B_Filtered_Cloud(new pcl::PointCloud<pcl::PointXYZ>);
#pragma endregion

#pragma endregion

#pragma region Rough Translation Elements
	double Center_P_Cur[3], Center_P_Prev[3];
	bool Rough_Translated = false;
	bool Rough_T_Activation = false;
#pragma endregion

#pragma region Translate Estimation Elements

#pragma region Method
	pcl::NormalEstimationOMP<pcl::PointXYZ, pcl::Normal> Calculate_Normal;
	pcl::FPFHEstimationOMP<pcl::PointXYZ, pcl::Normal, pcl::FPFHSignature33> Calculate_Feature;
	pcl::KdTreeFLANN<pcl::FPFHSignature33> Calculate_Correspondence;

	pcl::search::KdTree<pcl::PointXYZ>::Ptr Calculate_Normal_Method(new pcl::search::KdTree<pcl::PointXYZ>);
	pcl::search::KdTree<pcl::PointXYZ>::Ptr Calculate_Feature_Method(new pcl::search::KdTree<pcl::PointXYZ>);

	Calculate_Normal.setSearchMethod(Calculate_Normal_Method);
	Calculate_Normal.setRadiusSearch(Normal_Calculate_Th);
	Calculate_Normal.setNumberOfThreads(OpenMP_Th);

	Calculate_Feature.setSearchMethod(Calculate_Feature_Method);
	Calculate_Feature.setRadiusSearch(Feature_Calculate_Th);
	Calculate_Feature.setNumberOfThreads(OpenMP_Th);
#pragma endregion 

#pragma region Var
	vector<float> Dist[8];
	std::vector<pcl::PointIndices> Inliers;
	Inliers.resize(Virtual_Surface_Pivot->size());

	int Robust_Pivot_A = 0;
	int Robust_Pivot_B = 0;

	int Correspondence_Idx_Cur = 0;
	int Correspondence_Idx_Prev = 0;

	double Correspondence_Dist_Avg = 0;
	double Correspondence_Dist_Min = 0;
#pragma endregion 

#pragma region Cloud
#pragma region Prev
	pcl::PointCloud<pcl::PointXYZ>::Ptr C_Full_Cloud_Prev(new pcl::PointCloud<pcl::PointXYZ>);

	pcl::PointCloud<pcl::PointXYZ>::Ptr Base_Cloud_Prev(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointIndices::Ptr Base_Indices_Prev(new pcl::PointIndices);

	pcl::PointCloud<pcl::PointXYZ>::Ptr Pivot_Cloud_Prev(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointIndices::Ptr Pivot_Indices_Prev(new pcl::PointIndices);

	pcl::PointCloud<pcl::Normal>::Ptr Normal_Prev(new pcl::PointCloud<pcl::Normal>);
	pcl::PointCloud<pcl::FPFHSignature33>::Ptr Feature_Prev(new pcl::PointCloud<pcl::FPFHSignature33>);
#pragma endregion 

#pragma region Cur
	pcl::PointCloud<pcl::PointXYZ>::Ptr C_Full_Cloud_Cur(new pcl::PointCloud<pcl::PointXYZ>);

	pcl::PointCloud<pcl::PointXYZ>::Ptr Base_Cloud_Cur(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointIndices::Ptr Base_Indices_Cur(new pcl::PointIndices);

	pcl::PointIndices::Ptr Pivot_Indices_Cur(new pcl::PointIndices);
	pcl::PointCloud<pcl::PointXYZ>::Ptr Pivot_Cloud_Cur(new pcl::PointCloud<pcl::PointXYZ>);

	pcl::PointCloud<pcl::Normal>::Ptr Normal_Cur(new pcl::PointCloud<pcl::Normal>);
	pcl::PointCloud<pcl::FPFHSignature33>::Ptr Feature_Cur(new pcl::PointCloud<pcl::FPFHSignature33>);

	pcl::PointCloud<pcl::PointXYZ>::Ptr Fitness_Cloud(new pcl::PointCloud<pcl::PointXYZ>);
#pragma endregion 
#pragma endregion 

#pragma endregion

#pragma region ICP Activation Elements
#pragma region Var
	bool ICP_Activate = true;
	double ICP_Activate_T_Dist;
	double ICP_Activate_R_Dist;

	queue<pcl::PointCloud<pcl::PointXYZ>>* Q_T;
	Q_T = new queue<pcl::PointCloud<pcl::PointXYZ>>();

	queue<pcl::PointCloud<pcl::PointXYZ>>* Q_R;
	Q_R = new queue<pcl::PointCloud<pcl::PointXYZ>>();
#pragma endregion 

#pragma region Cloud
	pcl::PointCloud<pcl::PointXYZ>::Ptr Rotation_Physical_Cur(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr Rotation_Physical_Prev(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointIndices::Ptr Rotation_Pivot_Indices(new pcl::PointIndices);
#pragma endregion
#pragma endregion

#pragma region ICP Elements
	pcl::IterativeClosestPoint<pcl::PointXYZ, pcl::PointXYZ> icp;
	icp.setEuclideanFitnessEpsilon(1e-12);
	icp.setTransformationEpsilon(1e-8);
	icp.setUseReciprocalCorrespondences(true);
	icp.setMaxCorrespondenceDistance(Correspondence_Max_Dist);
	icp.setRANSACOutlierRejectionThreshold(Refining_RANSAC_Dist_th);
	icp.setMaximumIterations(100);
	icp.setRANSACIterations(50);
#pragma endregion 

#pragma region Surface Extraction Elements

#pragma region Method
	pcl::ExtractIndices<pcl::PointXYZ> Indices_Extracter;
	pcl::octree::OctreePointCloudSearch<pcl::PointXYZ> Surface_Extractor(Radius_Search_Th);
	Indices_Extracter.setNegative(true);
#pragma endregion 

#pragma region Cloud
	pcl::PointCloud<pcl::PointXYZ>::Ptr Surface_Cloud(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointIndices::Ptr Surface_Indices(new pcl::PointIndices);
#pragma endregion 

#pragma endregion 

#pragma region IPC Elements
	char Packet[72];
	Socket* Transmiter;

	if (IPC)
		Transmiter = new Socket();
#pragma endregion 

	grabber->start();

	while (!viewer->wasStopped())
	{
		if (Raw_XYZRGB_Cloud->size() > 0)
		{
			viewer->removeAllPointClouds();
			viewer->removeAllShapes();

			pcl::copyPointCloud(*Raw_XYZRGB_Cloud, *Gray_Scaled_Cloud);

			if (Enable_visualize_full_pointcloud && Detecting)
			{
				for (int i = 0; i < Gray_Scaled_Cloud->size(); i++)
				{
					int data = 0;
					data += Gray_Scaled_Cloud->points[i].r;
					data += Gray_Scaled_Cloud->points[i].g;
					data += Gray_Scaled_Cloud->points[i].b;

					data /= 3;

					Gray_Scaled_Cloud->points[i].r = data;
					Gray_Scaled_Cloud->points[i].g = data;
					Gray_Scaled_Cloud->points[i].b = data;
				}

				viewer->addPointCloud(Gray_Scaled_Cloud, "Full_Cloud", RunTime_Window);
			}

			pcl::copyPointCloud(*Gray_Scaled_Cloud, *Full_Cloud);
			pcl::copyPointCloud(*Full_Cloud, *A_Full_Cloud);

#pragma region Detecting
			
			if (Detecting)
			{
				/*if (!Ready)
				{
					pcl::copyPointCloud(*Full_Cloud, *A_Full_Cloud);
					pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> color(A_Full_Cloud, 100, 100, 100);
					viewer->addPointCloud(A_Full_Cloud, color, "Full_Cloud", RunTime_Window);
				}*/
				Euclidean = Eigen::Matrix4f::Identity();

				A_Regions.clear();
				A_Model_coefficients.clear();
				A_Labels->clear();
				A_Inliers.clear();
				A_Label_indices.clear();
				A_Boundary_indices.clear();
				C_Clusters.clear();
				B_Plane_labels.clear();
				C_Euclidean_labels.clear();
				C_Euclidean_label_indices.clear();

				Fast_Normal_Estimator.setInputCloud(A_Full_Cloud);
				Fast_Normal_Estimator.compute(*A_Full_Cloud_Normal);
				A_Point_to_Cluster.setInputCloud(A_Full_Cloud);
				A_Point_to_Cluster.setInputNormals(A_Full_Cloud_Normal);
				A_Point_to_Cluster.segmentAndRefine(A_Regions, A_Model_coefficients, A_Inliers, A_Labels, A_Label_indices, A_Boundary_indices);

				if (A_Regions.size() > 0)
				{
					B_Plane_labels.resize(A_Label_indices.size(), false);
					for (int i = 0; i < A_Label_indices.size(); i++)
						if (A_Label_indices[i].indices.size() > B_Cluster_Min_Size) B_Plane_labels[i] = true;

					B_Cluster_to_Component->setInputCloud(A_Full_Cloud);
					B_Cluster_to_Component->setLabels(A_Labels);
					B_Cluster_to_Component->setExcludeLabels(B_Plane_labels);

					C_Component_to_ConnectedComponent.setInputCloud(A_Full_Cloud);
					C_Component_to_ConnectedComponent.segment(C_Euclidean_labels, C_Euclidean_label_indices);

					for (int i = 0; i < C_Euclidean_label_indices.size(); i++)
						if (C_Cluster_Min_Size < C_Euclidean_label_indices[i].indices.size() && C_Euclidean_label_indices[i].indices.size() < C_Cluster_Max_Size)
						{
							pcl::PointCloud<pcl::PointXYZ> cluster;
							pcl::copyPointCloud(*A_Full_Cloud, C_Euclidean_label_indices[i].indices, cluster);
							C_Clusters.push_back(cluster);
						}
				}

				if (!Ready)
					for (int i = 0; i < C_Clusters.size(); i++)
					{
						sprintf(text, "cluster_%d", i);

						pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> color0(boost::make_shared<pcl::PointCloud<pcl::PointXYZ> >(C_Clusters[i]), red[i % 6], grn[i % 6], blu[i % 6]);
						viewer->addPointCloud(boost::make_shared<pcl::PointCloud<pcl::PointXYZ> >(C_Clusters[i]), color0, text, RunTime_Window);
						viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 5, text);
						viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_OPACITY, 0.3, text);
					}

#pragma region UI
				if (UI_2D_Visualization)
				{
					viewer->addText("1. Clustering", 10, 500, 30, 1, 1, 0, "Title", Info_Window);

					sprintf(text, "Cluster Number\n\n            Size");
					viewer->addText(text, 10, 450, 13, 1, 1, 0, text, Info_Window);

					sprintf(text, "Cluster Min\n            Max\n\nNeighbor Point Angle\n                            Dist\n\nNeighbor Cluster Angle\n\nNormal Smoothing");
					viewer->addText(text, 10, 170, 13, 1, 1, 0, text, Info_Window);

					sprintf(text, "|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|");
					viewer->addText(text, 170, 30, 13, 1, 1, 1, text, Info_Window);

					sprintf(text, "  %d\n\n ", C_Clusters.size());
					viewer->addText(text, 180, 450, 13, 1, 1, 1, text, Info_Window);

					for (int i = 0; i < C_Clusters.size(); i++)
					{
						sprintf(text, "  %d", C_Clusters[i].size());
						viewer->addText(text, 180, 450 - 11 * i, 13, 1, 1, 1, text, Info_Window);
					}

					sprintf(text, "  %d\n  %d\n\n  Norm_Gap_Avg%.2f\n  %.2f\n\n  %.2f\n\n  %.2f", C_Cluster_Min_Size, C_Cluster_Max_Size, A_Neighbor_Point_Angle_Th, A_Neighbor_Point_Distance_th, B_Neighbor_Cluster_Angle_Th, Normal_Smoothing_Th);
					viewer->addText(text, 180, 170, 13, 1, 1, 1, text, Info_Window);
				}
#pragma endregion
				if (Ready)
				{
					//viewer->removeAllPointClouds();
					viewer->removeAllShapes();

#pragma region UI

					if (UI_2D_Visualization)
					{
						viewer->addText("2. Searching", 10, 500, 30, 1, 1, 0, text, Info_Window);

						sprintf(text, "Cluster Number\n\n            Size");
						viewer->addText(text, 10, 450, 13, 1, 1, 0, text, Info_Window);

						sprintf(text, "Fitness Threshold\n\nFilter     Param 1\n            Param 2\n\nOutlier Range (mm)  Lv 1\n                                 Lv 2\n                                 Lv 3\n\nWeight    Level 1\n               Level 2\n               Level 3");
						viewer->addText(text, 10, 170, 13, 1, 1, 0, text, Info_Window);

						sprintf(text, "|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|");
						viewer->addText(text, 170, 30, 13, 1, 1, 1, text, Info_Window);


						sprintf(text, "  %d\n\n ", C_Clusters.size());
						viewer->addText(text, 180, 450, 13, 1, 1, 1, text, Info_Window);

						for (int i = 0; i < C_Clusters.size(); i++)
						{
							sprintf(text, "  %d", C_Clusters[i].size());
							viewer->addText(text, 180, 450 - 11 * i, 13, 1, 1, 1, text, Info_Window);
						}

						sprintf(text, "  %.2f\n\n  %d\n  %.2f\n\n  %.3f\n  %.2f\n  %.2f\n\n  * %.2f\n  * %.2f\n  * %.2f", Fitness_Th, N_F_Param_1_1, N_F_Param_1_2, Outlier_Lv_1, Outlier_Lv_2, Outlier_Lv_3, Weight_Lv_1, Weight_Lv_2, Weight_Lv_3);
						viewer->addText(text, 180, 170, 13, 1, 1, 1, text, Info_Window);
					}
#pragma endregion 

					pcl::copyPointCloud(*Full_Cloud, *A_Full_Cloud);

					for (int i = 0; i < C_Clusters.size(); i++)
					{
						sprintf(text, "cluster_%d", i);

						pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> color(boost::make_shared<pcl::PointCloud<pcl::PointXYZ> >(C_Clusters[i]), 0, 0, 200);
						viewer->addPointCloud(boost::make_shared<pcl::PointCloud<pcl::PointXYZ> >(C_Clusters[i]), color, text, RunTime_Window);
						viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, text);
					}

					for (int i = 0; i < C_Clusters.size(); i++)
					{
						Physical_Center[0] = Physical_Center[1] = Physical_Center[2] = 0.0;

						Translation = Eigen::Matrix4f::Identity();

						Noise_Filter.setInputCloud(boost::make_shared<pcl::PointCloud<pcl::PointXYZ> >(C_Clusters[i]));
						Noise_Filter.filter(*Physical_Full);
						Min_Fitness = 9999;
						double x_min = 999;
						double y_min = 999;
						double z_min = 999;

						double x_max = -999;
						double y_max = -999;
						double z_max = -999;

						for (int j = 0; j < C_Clusters[i].size(); j++)
						{
							Physical_Center[0] += C_Clusters[i].points[j].x;
							Physical_Center[1] += C_Clusters[i].points[j].y;
							Physical_Center[2] += C_Clusters[i].points[j].z;

							if (C_Clusters[i].points[j].x < x_min)
								x_min = C_Clusters[i].points[j].x;
							else if (x_max < C_Clusters[i].points[j].x)
								x_max = C_Clusters[i].points[j].x;

							if (C_Clusters[i].points[j].y < y_min)
								y_min = C_Clusters[i].points[j].y;
							else if (y_max < C_Clusters[i].points[j].y)

								y_max = C_Clusters[i].points[j].y;

							if (C_Clusters[i].points[j].z < z_min)
								z_min = C_Clusters[i].points[j].z;
							else if (z_max < C_Clusters[i].points[j].z)
								z_max = C_Clusters[i].points[j].z;
						}

						Physical_Center[0] /= (double)C_Clusters[i].size();
						Physical_Center[1] /= (double)C_Clusters[i].size();
						Physical_Center[2] /= (double)C_Clusters[i].size();

						Translation(0, 3) = Physical_Center[0] - Virtual_avg[0];
						Translation(1, 3) = Physical_Center[1] - Virtual_avg[1];
						Translation(2, 3) = Physical_Center[2] - Virtual_avg[2];

						Grid_min[0] = Physical_Center[0] - abs(x_max - x_min) / 2.0;
						Grid_max[0] = Physical_Center[0] + abs(x_max - x_min) / 2.0;
						Grid_min[1] = Physical_Center[1] - abs(y_max - y_min) / 2.0;
						Grid_max[1] = Physical_Center[1] + abs(y_max - y_min) / 2.0;
						Grid_min[2] = Physical_Center[2] - abs(z_max - z_min) / 2.0;
						Grid_max[2] = Physical_Center[2] + abs(z_max - z_min) / 2.0;

						Grid_Pivot->points[0].x = Grid_min[0];
						Grid_Pivot->points[0].y = Grid_min[1];
						Grid_Pivot->points[0].z = Grid_min[2];

						Grid_Pivot->points[1].x = Grid_max[0];
						Grid_Pivot->points[1].y = Grid_min[1];
						Grid_Pivot->points[1].z = Grid_min[2];

						Grid_Pivot->points[2].x = Grid_min[0];
						Grid_Pivot->points[2].y = Grid_max[1];
						Grid_Pivot->points[2].z = Grid_min[2];

						Grid_Pivot->points[3].x = Grid_max[0];
						Grid_Pivot->points[3].y = Grid_max[1];
						Grid_Pivot->points[3].z = Grid_min[2];

						Grid_Pivot->points[4].x = Grid_min[0];
						Grid_Pivot->points[4].y = Grid_max[1];
						Grid_Pivot->points[4].z = Grid_max[2];

						Grid_Pivot->points[5].x = Grid_min[0];
						Grid_Pivot->points[5].y = Grid_min[1];
						Grid_Pivot->points[5].z = Grid_max[2];

						Grid_Pivot->points[6].x = Grid_max[0];
						Grid_Pivot->points[6].y = Grid_min[1];
						Grid_Pivot->points[6].z = Grid_max[2];

						Grid_Pivot->points[7].x = Grid_max[0];
						Grid_Pivot->points[7].y = Grid_max[1];
						Grid_Pivot->points[7].z = Grid_max[2];

						for (int j = 0; j < 12; j++)
						{
							sprintf(text, "Line_%d_%d", i, j);
							viewer->addLine(Grid_Pivot->points[Grid_Line_Idx_Start[j] - 1], Grid_Pivot->points[Grid_Line_Idx_End[j] - 1], text, RunTime_Window);
							viewer->setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 255, 255, 0, text);
							viewer->setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_LINE_WIDTH, 1, text);
						}

						for (int j = 0; j < Virtual_Initial_Full.size(); j++)
						{
							Index->indices.clear();
							Outlier_Index[0].indices.clear();
							Outlier_Index[1].indices.clear();
							Outlier_Index[2].indices.clear();

							pcl::transformPointCloud(Virtual_Initial_Full[j], *Virtual_Copy, Translation);

							icp.setInputSource(Physical_Full);
							icp.setInputTarget(Virtual_Copy);
							icp.align(*Cloud);

							Matrix = icp.getFinalTransformation().inverse();
							pcl::transformPointCloud(*Virtual_Copy, *Virtual_Copy, Matrix);

							viewer->addPointCloud(Virtual_Copy, "Virtual_Copy", RunTime_Window);
							viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 255, 0, 0, "Virtual_Copy", RunTime_Window);
							viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_OPACITY, 0.5, "Virtual_Copy", RunTime_Window);
							viewer->spinOnce();
							viewer->removePointCloud("Virtual_Copy");

							Search.setInputCloud(Virtual_Copy);

							for (int l = 0; l < Physical_Full->size(); l++)
							{
								Search.nearestKSearch(Physical_Full->points[l], 1, idx, D);

								if (Outlier_Lv_1 < D[0] * 100 && D[0] * 100 <= Outlier_Lv_2)
									Outlier_Index[0].indices.push_back(l);
								else if (Outlier_Lv_2 < D[0] * 100 && D[0] * 100 <= Outlier_Lv_3)
									Outlier_Index[1].indices.push_back(l);
								else if (Outlier_Lv_3 < D[0] * 100)
									Outlier_Index[2].indices.push_back(l);
							}

							Fitness = (double)Outlier_Index[0].indices.size() * Weight_Lv_1 +
								(double)Outlier_Index[1].indices.size() * Weight_Lv_2 +
								(double)Outlier_Index[2].indices.size() * Weight_Lv_3;

							if (Fitness < Min_Fitness)
							{
								Min_Fitness = Fitness;
								Matched_Idx = j;

								for (int j = 0; j < 3; j++)
								{
									Index->indices = Outlier_Index[j].indices;
									pcl::copyPointCloud(*Physical_Full, Index->indices, Outlier_Cloud[j]);
								}
								Euclidean = Matrix * Translation;
							}
						}

						for (int j = 0; j < 3; j++)
						{
							sprintf(text, "Outlier_%d", j + (i * 5));
							pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> color0(boost::make_shared<pcl::PointCloud<pcl::PointXYZ> >(Outlier_Cloud[j]), 255, 127.0 * (2 - j), 0);
							viewer->addPointCloud(boost::make_shared<pcl::PointCloud<pcl::PointXYZ>>(Outlier_Cloud[j]), color0, text, RunTime_Window);
							viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, text);
						}

#pragma region UI
						if (UI_3D_Visualization)
						{
							sprintf(text, "[ Physical_Full Num | %d ]\n\n[ Outlier | (*1.0|R)%d / (*3.0|G)%d / (*15.0|B)%d ]\n[ Matched Model | %d ]", i, Outlier_Cloud[0].size(), Outlier_Cloud[1].size(), Outlier_Cloud[2].size(), Matched_Idx);
							Text_Pos->points[0].x = Grid_min[0] - 0.01;
							Text_Pos->points[0].y = Grid_max[1] + 0.01;
							Text_Pos->points[0].z = Grid_min[2];
							sprintf(id, "Text3D%d", i);
							viewer->addText3D(text, Text_Pos->points[0], 0.005, 1.0, 1.0, 0.0, id);

							sprintf(text, "\n[ Fitness Score | (*10.0) %.2f ]", Min_Fitness);
							Text_Pos->points[0].x = Grid_min[0] - 0.01;
							Text_Pos->points[0].y = Grid_max[1] + 0.01;
							Text_Pos->points[0].z = Grid_min[2];
							sprintf(id, "Text3D_%d", i);
							viewer->addText3D(text, Text_Pos->points[0], 0.005, 1.0, 0.0, 0.0, id);
						}
#pragma endregion

						if (Min_Fitness < Fitness_Th)
						{
							Euclidean_Candidates.push_back(Euclidean);
							
							pcl::transformPointCloud(Virtual_Initial_Full[Matched_Idx], *Cloud, Euclidean);
							Virtual_Candidate_Full.push_back(*Cloud);

							if (Aligned_Virtual)
							{
								sprintf(text, "Cloud_%d", i);
								viewer->addPointCloud(Cloud, text, RunTime_Window);
								viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 0, 0, 255, text);
								viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 0.5, text);
							}

							pcl::transformPointCloud(Virtual_Initial_Pivot[Matched_Idx], *Cloud, Euclidean);
							Virtual_Candidate_Pivot.push_back(*Cloud);

							pcl::PointCloud<pcl::PointXYZ>::CloudVectorType Pivot_Cloud;
							for (int j = 0; j < Virtual_Initial_Pivot_Cloud[Matched_Idx].size(); j++)
							{
								pcl::transformPointCloud(Virtual_Initial_Pivot_Cloud[Matched_Idx][j], *Cloud, Euclidean);
								Pivot_Cloud.push_back(*Cloud);
							}

							Virtual_Candidate_Pivot_Cloud.push_back(Pivot_Cloud);

							Pivot_Cloud.clear();

							pcl::transformPointCloud(Virtual_Initial_Surface_Pivot[Matched_Idx], *Cloud, Euclidean);
							Virtual_Candidate_Surface_Pivot.push_back(*Cloud);

							pcl::transformPointCloud(Virtual_Initial_Inner_Pivot[Matched_Idx], *Cloud, Euclidean);
							Virtual_Candidate_Inner_Pivot.push_back(*Cloud);

							pcl::transformPointCloud(*Virtual_Center, *Cloud, Euclidean);
							Virtual_Candidate_Center.push_back(*Cloud);

							Physical_Candidate_Full.push_back(*Physical_Full);

							Physical_Candidate_Grid.push_back(*Grid_Pivot);

							pcl::transformPointCloud(Virtual_Initial_Axis[Matched_Idx], *Cloud, Euclidean);
							Virtual_Candidate_Axis.push_back(*Cloud);

							for (int j = 0; j < 12; j++)
							{
								sprintf(text, "Line_%d_%d_", i, j);
								viewer->addLine(Grid_Pivot->points[Grid_Line_Idx_Start[j] - 1], Grid_Pivot->points[Grid_Line_Idx_End[j] - 1], text, RunTime_Window);
								viewer->setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 255, 0, 0, text);
								viewer->setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_LINE_WIDTH, 1, text);
							}
						}
						viewer->spinOnce();
					}


#pragma region UI
					sprintf(text, "Found Candidate");
					viewer->addText(text, 10, 339, 13, 1, 0, 0, text, Info_Window);

					sprintf(text, "  %d", Virtual_Candidate_Full.size());
					viewer->addText(text, 180, 339, 13, 1, 0, 0, text, Info_Window);
#pragma endregion					

					for (int i = 0; i < C_Clusters.size(); i++)
						for (int j = 0; j < 12; j++)
						{
							sprintf(text, "Line_%d_%d", i, j);
							viewer->removeShape(text);
						}

					Time_S = clock();

					while (1)
					{
						viewer->spinOnce();
						Time_E = clock();

						if ((Time_E - Time_S) / (double)CLOCKS_PER_SEC > 1.0)
							break;
					}

#pragma region Detection Evaluation

					viewer->removeAllShapes();

					if (Virtual_Candidate_Full.size() > 1)
					{
						Noise_Filter.setMeanK(N_F_Param_2_1);
						Noise_Filter.setStddevMulThresh(N_F_Param_2_2);

						double Min_Norm_Dist = 9999;
						for (int i = 0; i < Virtual_Candidate_Full.size(); i++)
						{
							for (int j = 0; j < 12; j++)
							{
								sprintf(text, "Line_%d_%d", i, j);
								viewer->addLine(Physical_Candidate_Grid[i].points[Grid_Line_Idx_Start[j] - 1], Physical_Candidate_Grid[i].points[Grid_Line_Idx_End[j] - 1], text, RunTime_Window);
								viewer->setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 255, 255, 0, text);
								viewer->setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_LINE_WIDTH, 1, text);
							}

							Virtual_Front_Pivot_Cloud.clear();
							Index->indices.clear();

							Noise_Filter.setInputCloud(boost::make_shared<pcl::PointCloud<pcl::PointXYZ>>(Physical_Candidate_Full[i]));
							Noise_Filter.filter(*Filtered_Physical);

							Search.setInputCloud(Filtered_Physical);

							for (int j = 0; j < Virtual_Candidate_Pivot[i].size(); j++)
							{
								Search.nearestKSearch(Virtual_Candidate_Pivot[i].points[j], Front_Pivot_Neighbor, idx, D);

								double D_Avg = 0.0;

								for (int a = 0; a < Front_Pivot_Neighbor; a++)
									D_Avg += D[a];

								D_Avg /= (double)Front_Pivot_Neighbor;

								if (D_Avg * 100 < Front_Pivot_Rad)
									Index->indices.push_back(j);
							}
							Front_Pivot_Cnt.push_back(Index->indices.size());

							if (Index->indices.size() > 2)
							{
								pcl::copyPointCloud(Virtual_Candidate_Pivot[i], Index->indices, *Virtual_Front_Pivot);

								for (int j = 0; j < Index->indices.size(); j++)
								{
									pcl::copyPointCloud(Virtual_Candidate_Pivot_Cloud[i][Index->indices[j]], *Cloud);
									Virtual_Front_Pivot_Cloud.push_back(*Cloud);
								}
								if (Detectrion_Evaluation_Visualization)
								{
									sprintf(text, "Virtual_Front_Pivot_%d", i);
									viewer->addPointCloud(Virtual_Front_Pivot, text, RunTime_Window);
									viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 255, 0, 0, text, RunTime_Window);
									viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 5, text, RunTime_Window);
								}

								int Physical_Size = 0;
								int Virtual_Size = 0;
								int Cnt = 0;
								Physical_Normal_Avg->points[0].normal_x = Physical_Normal_Avg->points[0].normal_y = Physical_Normal_Avg->points[0].normal_z =
									Virtual_Normal_Avg->points[0].normal_x = Virtual_Normal_Avg->points[0].normal_y = Virtual_Normal_Avg->points[0].normal_z = 0.0;

								if (Evaluation_Mothod == 1)
								{
									for (int j = 0; j < Virtual_Front_Pivot->size(); j++)
									{
										Search.radiusSearch(Virtual_Front_Pivot->points[j], 0.02, Index->indices, D);

										if (Index->indices.size() >= 20)
										{
											pcl::copyPointCloud(*Filtered_Physical, Index->indices, *Cloud);

											*Calculate_Normal_Method = new pcl::search::KdTree <pcl::PointXYZ>;
											Calculate_Normal.setInputCloud(Cloud);
											Calculate_Normal.compute(*Physical_Normal);

											*Calculate_Normal_Method = new pcl::search::KdTree <pcl::PointXYZ>;
											Calculate_Normal.setInputCloud(boost::make_shared<pcl::PointCloud<pcl::PointXYZ>>(Virtual_Front_Pivot_Cloud[j]));
											Calculate_Normal.compute(*Virtual_Normal);

											for (int a = 0; a < Physical_Normal->size(); a++)
												if (!isnan(Physical_Normal->points[a].normal_x))
												{
													Physical_Size++;
													Physical_Normal_Avg->points[0].normal_x += Physical_Normal->points[a].normal_x;
													Physical_Normal_Avg->points[0].normal_y += Physical_Normal->points[a].normal_y;
													Physical_Normal_Avg->points[0].normal_z += Physical_Normal->points[a].normal_z;
												}

											for (int a = 0; a < Virtual_Normal->size(); a++)
											{
												Virtual_Size++;
												Virtual_Normal_Avg->points[0].normal_x += Virtual_Normal->points[a].normal_x;
												Virtual_Normal_Avg->points[0].normal_y += Virtual_Normal->points[a].normal_y;
												Virtual_Normal_Avg->points[0].normal_z += Virtual_Normal->points[a].normal_z;
											}

											if (Detectrion_Evaluation_Visualization)
											{
												pcl::PointCloud<pcl::PointXYZ>::Ptr Physical_Center(new pcl::PointCloud<pcl::PointXYZ>);
												pcl::PointCloud<pcl::PointXYZ>::Ptr Point(new pcl::PointCloud<pcl::PointXYZ>);

												Physical_Center->resize(1);
												Point->resize(1);

												Physical_Center->points[0].x = Physical_Center->points[0].y = Physical_Center->points[0].z = 0.0;
												Point->points[0].x = Virtual_Front_Pivot->points[j].x;
												Point->points[0].y = Virtual_Front_Pivot->points[j].y;
												Point->points[0].z = Virtual_Front_Pivot->points[j].z;

												for (int a = 0; a < Cloud->size(); a++)
												{
													Physical_Center->points[0].x += Cloud->points[a].x;
													Physical_Center->points[0].y += Cloud->points[a].y;
													Physical_Center->points[0].z += Cloud->points[a].z;
												}

												Physical_Center->points[0].x /= (double)Cloud->size();
												Physical_Center->points[0].y /= (double)Cloud->size();
												Physical_Center->points[0].z /= (double)Cloud->size();

												sprintf(text, "%d_%d_Cloud", i, j);
												viewer->addPointCloud(Cloud, text, RunTime_Window);
												viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 0, 0, 255, text);
												viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, text);

												sprintf(text, "%d_%d_Cloud_Normal", i, j);
												viewer->addPointCloudNormals<pcl::PointXYZ, pcl::Normal>(Physical_Center, Physical_Normal_Avg, 1, 0.05, text, RunTime_Window);
												viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 255, 0, 255, text);

												sprintf(text, "%d_%d_Virtual_Normal", i, j);
												viewer->addPointCloudNormals<pcl::PointXYZ, pcl::Normal>(Point, Virtual_Normal_Avg, 1, 0.05, text, RunTime_Window);
												viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 255, 0, 0, text);
											}

										}
									}

									Physical_Normal_Avg->points[0].normal_x /= (double)Physical_Size;
									Physical_Normal_Avg->points[0].normal_y /= (double)Physical_Size;
									Physical_Normal_Avg->points[0].normal_z /= (double)Physical_Size;
									Virtual_Normal_Avg->points[0].normal_x /= (double)Virtual_Size;
									Virtual_Normal_Avg->points[0].normal_y /= (double)Virtual_Size;
									Virtual_Normal_Avg->points[0].normal_z /= (double)Virtual_Size;


									double Norm_Dist = sqrt(
										pow(Physical_Normal_Avg->points[0].normal_x - Virtual_Normal_Avg->points[0].normal_x, 2) +
										pow(Physical_Normal_Avg->points[0].normal_y - Virtual_Normal_Avg->points[0].normal_y, 2) +
										pow(Physical_Normal_Avg->points[0].normal_z - Virtual_Normal_Avg->points[0].normal_z, 2)
										);

									Norm_Gap_Avg.push_back(Norm_Dist);

									if (Norm_Gap_Avg.back() < Min_Norm_Dist)
									{
										Final_Physical_Idx = i;
										Min_Norm_Dist = Norm_Gap_Avg.back();
									}
								}

								if (Evaluation_Mothod == 2)
								{
									Norm_Gap_Avg.push_back(0.0);
									for (int j = 0; j < Virtual_Front_Pivot->size(); j++)
									{
										Search.radiusSearch(Virtual_Front_Pivot->points[j], 0.02, Index->indices, D);

										if (Index->indices.size() >= 20)
										{
											Cnt++;

											pcl::copyPointCloud(*Filtered_Physical, Index->indices, *Cloud);

											Physical_Size = 0;
											Virtual_Size = 0;
											Physical_Normal_Avg->points[0].normal_x = Physical_Normal_Avg->points[0].normal_y = Physical_Normal_Avg->points[0].normal_z =
												Virtual_Normal_Avg->points[0].normal_x = Virtual_Normal_Avg->points[0].normal_y = Virtual_Normal_Avg->points[0].normal_z = 0.0;
											*Calculate_Normal_Method = new pcl::search::KdTree <pcl::PointXYZ>;
											Calculate_Normal.setInputCloud(Cloud);
											Calculate_Normal.compute(*Physical_Normal);
											*Calculate_Normal_Method = new pcl::search::KdTree <pcl::PointXYZ>;
											Calculate_Normal.setInputCloud(boost::make_shared<pcl::PointCloud<pcl::PointXYZ>>(Virtual_Front_Pivot_Cloud[j]));
											Calculate_Normal.compute(*Virtual_Normal);

											for (int a = 0; a < Physical_Normal->size(); a++)
												if (!isnan(Physical_Normal->points[a].normal_x))
												{
													Physical_Size++;
													Physical_Normal_Avg->points[0].normal_x += Physical_Normal->points[a].normal_x;
													Physical_Normal_Avg->points[0].normal_y += Physical_Normal->points[a].normal_y;
													Physical_Normal_Avg->points[0].normal_z += Physical_Normal->points[a].normal_z;
												}

											for (int a = 0; a < Virtual_Normal->size(); a++)
											{
												Virtual_Size++;
												Virtual_Normal_Avg->points[0].normal_x += Virtual_Normal->points[a].normal_x;
												Virtual_Normal_Avg->points[0].normal_y += Virtual_Normal->points[a].normal_y;
												Virtual_Normal_Avg->points[0].normal_z += Virtual_Normal->points[a].normal_z;
											}

											Physical_Normal_Avg->points[0].normal_x /= (double)Physical_Size;
											Physical_Normal_Avg->points[0].normal_y /= (double)Physical_Size;
											Physical_Normal_Avg->points[0].normal_z /= (double)Physical_Size;
											Virtual_Normal_Avg->points[0].normal_x /= (double)Virtual_Size;
											Virtual_Normal_Avg->points[0].normal_y /= (double)Virtual_Size;
											Virtual_Normal_Avg->points[0].normal_z /= (double)Virtual_Size;

											double Norm_Dist = sqrt(
												pow(Physical_Normal_Avg->points[0].normal_x - Virtual_Normal_Avg->points[0].normal_x, 2) +
												pow(Physical_Normal_Avg->points[0].normal_y - Virtual_Normal_Avg->points[0].normal_y, 2) +
												pow(Physical_Normal_Avg->points[0].normal_z - Virtual_Normal_Avg->points[0].normal_z, 2)
												);

											Norm_Gap_Avg.back() += Norm_Dist;

											if (Detectrion_Evaluation_Visualization)
											{
												pcl::PointCloud<pcl::PointXYZ>::Ptr Physical_Center(new pcl::PointCloud<pcl::PointXYZ>);
												pcl::PointCloud<pcl::PointXYZ>::Ptr Point(new pcl::PointCloud<pcl::PointXYZ>);

												Physical_Center->resize(1);
												Point->resize(1);

												Physical_Center->points[0].x = Physical_Center->points[0].y = Physical_Center->points[0].z = 0.0;
												Point->points[0].x = Virtual_Front_Pivot->points[j].x;
												Point->points[0].y = Virtual_Front_Pivot->points[j].y;
												Point->points[0].z = Virtual_Front_Pivot->points[j].z;

												for (int a = 0; a < Cloud->size(); a++)
												{
													Physical_Center->points[0].x += Cloud->points[a].x;
													Physical_Center->points[0].y += Cloud->points[a].y;
													Physical_Center->points[0].z += Cloud->points[a].z;
												}

												Physical_Center->points[0].x /= (double)Cloud->size();
												Physical_Center->points[0].y /= (double)Cloud->size();
												Physical_Center->points[0].z /= (double)Cloud->size();

												int color = j % 6;

												sprintf(text, "%d_%d_Cloud", i, j);
												viewer->addPointCloud(Cloud, text, RunTime_Window);
												viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, red[color], grn[color], blu[color], text);
												viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, text);

												sprintf(text, "%d_%d_Cloud_Normal", i, j);
												viewer->addPointCloudNormals<pcl::PointXYZ, pcl::Normal>(Physical_Center, Physical_Normal_Avg, 1, 0.05, text, RunTime_Window);
												viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, red[color], grn[color], blu[color], text);

												sprintf(text, "%d_%d_Virtual_Normal", i, j);
												viewer->addPointCloudNormals<pcl::PointXYZ, pcl::Normal>(Point, Virtual_Normal_Avg, 1, 0.05, text, RunTime_Window);
												viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 255, 0, 0, text);
											}
										}
									}

									Norm_Gap_Avg.back() /= (double)Cnt;

									if (Norm_Gap_Avg.back() < Min_Norm_Dist)
									{
										Final_Physical_Idx = i;
										Min_Norm_Dist = Norm_Gap_Avg.back();
									}
								}

#pragma region UI
								if (UI_3D_Visualization)
								{
									sprintf(text, "[ Candidate | %d ]\n[ Front Pivot | %d / %d ]", i, Virtual_Front_Pivot->size(), Virtual_Candidate_Pivot[0].size());
									Text_Pos->points[0].x = Physical_Candidate_Grid[i].points[2].x - 0.01;
									Text_Pos->points[0].y = Physical_Candidate_Grid[i].points[2].y + 0.01;
									Text_Pos->points[0].z = Physical_Candidate_Grid[i].points[2].z;
									sprintf(id, "Text3D%d", i);
									viewer->addText3D(text, Text_Pos->points[0], 0.005, 1.0, 1.0, 0.0, id);
									sprintf(text, "\n\n[ Normal Fitness | %.2f ]", Norm_Gap_Avg.back());
									sprintf(id, "Text3D_%d", i);
									viewer->addText3D(text, Text_Pos->points[0], 0.005, 1.0, 0.0, 0.0, id);
								}
#pragma endregion
							}
							viewer->spinOnce();
						}
					}

					else
						Final_Physical_Idx = 0;

					for (int i = 0; i < 12; i++)
					{
						sprintf(text, "Line_%d", i);
						viewer->addLine(Physical_Candidate_Grid[Final_Physical_Idx].points[Grid_Line_Idx_Start[i] - 1], Physical_Candidate_Grid[Final_Physical_Idx].points[Grid_Line_Idx_End[i] - 1], text, RunTime_Window);
						viewer->setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 255, 0, 0, text);
						viewer->setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_LINE_WIDTH, 1, text);
					}

#pragma region UI
					if (UI_2D_Visualization)
					{
						viewer->addText("3. Select", 11, 500, 30, 1, 1, 0, "3. Select", Info_Window);

						sprintf(text, "Candidate Number\n\nNormal Fitness");
						viewer->addText(text, 10, 437, 13, 1, 1, 0, text, Info_Window);

						sprintf(text, "Normal Radius (mm)\n             Method\n\nFilter     Param 1\n            Param 2\n\nFront Pivot Range (mm) \n                 Neighbor\n                 Method");
						viewer->addText(text, 10, 170, 13, 1, 1, 0, text, Info_Window);

						sprintf(text, "  %d", Virtual_Candidate_Full.size());
						viewer->addText(text, 180, 463, 13, 1, 1, 1, "Candidate Number", Info_Window);

						if (Virtual_Candidate_Full.size() > 1)
							for (int i = 0; i < Norm_Gap_Avg.size(); i++)
							{
								sprintf(text, "  %.3f", Norm_Gap_Avg[i]);
								viewer->addText(text, 180, 437 - 11 * i, 13, 1, 1, 1, text, Info_Window);
							}

						else
						{
							sprintf(text, "  -");
							viewer->addText(text, 180, 437, 13, 1, 1, 1, text, Info_Window);
						}
						sprintf(text, "Front Pivot");
						viewer->addText(text, 10, 437 - 11 * (Virtual_Candidate_Full.size() + 2), 13, 1, 1, 0, text, Info_Window);

						if (Virtual_Candidate_Full.size() > 1)
							for (int i = 0; i < Virtual_Candidate_Full.size(); i++)
							{
								sprintf(text, "  %d", Front_Pivot_Cnt[i]);
								viewer->addText(text, 180, 437 - 11 * (Virtual_Candidate_Full.size() + 2) - 11 * i, 13, 1, 1, 1, text, Info_Window);
							}
						else
						{
							sprintf(text, "  -");
							viewer->addText(text, 180, 437 + 2, 13, 1, 1, 1, text, Info_Window);
						}

						sprintf(text, "  %.2f\n  %s\n\n  %d\n  %.2f\n\n  %.3f\n  %.f\n  %s", Normal_Calculate_Th, "K-D Tree", N_F_Param_2_1, N_F_Param_2_2, Front_Pivot_Rad, Front_Pivot_Neighbor, "Nearest K Search");
						viewer->addText(text, 180, 170, 13, 1, 1, 1, text, Info_Window);

						sprintf(text, "|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|");
						viewer->addText(text, 170, 30, 13, 1, 1, 1, text, Info_Window);
					}
#pragma endregion

					Time_S = clock();

					while (1)
					{
						viewer->spinOnce();
						Time_E = clock();

						if ((Time_E - Time_S) / (double)CLOCKS_PER_SEC > 1.0)
							break;
					}

					viewer->removeAllPointClouds();
					viewer->removeAllShapes();
					system("cls");
#pragma endregion 

#pragma region Copy Clouds for Tracking

					Grid_min[0] = Virtual_Candidate_Center[Final_Physical_Idx].points[0].x - Axis_Filtering_Rad;
					Grid_max[0] = Virtual_Candidate_Center[Final_Physical_Idx].points[0].x + Axis_Filtering_Rad;
					Grid_min[1] = Virtual_Candidate_Center[Final_Physical_Idx].points[0].y - Axis_Filtering_Rad;
					Grid_max[1] = Virtual_Candidate_Center[Final_Physical_Idx].points[0].y + Axis_Filtering_Rad;
					Grid_min[2] = Virtual_Candidate_Center[Final_Physical_Idx].points[0].z - Axis_Filtering_Rad;
					Grid_max[2] = Virtual_Candidate_Center[Final_Physical_Idx].points[0].z + Axis_Filtering_Rad;

					pcl::copyPointCloud(Physical_Candidate_Full[Final_Physical_Idx], *C_Full_Cloud_Prev);
					pcl::copyPointCloud(Virtual_Candidate_Full[Final_Physical_Idx], *Virtual_Full);
					pcl::copyPointCloud(Virtual_Candidate_Center[Final_Physical_Idx], *Virtual_Center);
					pcl::copyPointCloud(Virtual_Candidate_Surface_Pivot[Final_Physical_Idx], *Virtual_Surface_Pivot);
					pcl::copyPointCloud(Virtual_Candidate_Inner_Pivot[Final_Physical_Idx], *Virtual_Inner_Pivot);
					pcl::copyPointCloud(Virtual_Candidate_Axis[Final_Physical_Idx], *Virtual_Axis);

					Euclidean = Euclidean_Candidates[Final_Physical_Idx];

					Detecting = false;

					icp.setMaximumIterations(Tracking_Iteration);
					icp.setRANSACIterations(Refining_RANSAC_Iteration);

					Q_T->push(*Virtual_Surface_Pivot);
					Q_R->push(*C_Full_Cloud_Prev);

					viewer->createViewPort(0.0, 0.0, 0.25, 0.3, Surface_Window);
					viewer->createViewPortCamera(Surface_Window);
				}
			}
#pragma endregion 
#pragma region Tracking
			else
			{
#pragma region Second Segmentation

				Axis_Filter.setInputCloud(Full_Cloud);
				for (int i = 0; i < 3; i++)
				{
					Axis_Filter.setFilterFieldName(Axis_Filter_Field[i]);
					Axis_Filter.setFilterLimits(Grid_min[i], Grid_max[i]);
					Axis_Filter.filter(*B_Filtered_Cloud);
					Axis_Filter.setInputCloud(B_Filtered_Cloud);
				}

				pcl::copyPointCloud(*B_Filtered_Cloud, *C_Full_Cloud_Cur);

#pragma endregion 

#pragma region Initialization

				*Calculate_Normal_Method = new pcl::search::KdTree <pcl::PointXYZ>;
				*Calculate_Feature_Method = new pcl::search::KdTree <pcl::PointXYZ>;
				Base_Indices_Prev->indices.clear();
				Pivot_Indices_Prev->indices.clear();
				Base_Indices_Cur->indices.clear();
				Pivot_Indices_Cur->indices.clear();
				Rotation_Pivot_Indices->indices.clear();
				ICP_Activate = true;
				Rough_Translated = false;
				Robust_Pivot_A = 0;
				Translation = Euclidean = Rough_Translation = Eigen::Matrix4f::Identity();
				Center_P_Cur[0] = Center_P_Cur[1] = Center_P_Cur[2] = 0.0;

#pragma endregion

#pragma region Rough_Translation(Translate via Center Point)
				for (int i = 0; i < C_Full_Cloud_Cur->size(); i++)
				{
					Center_P_Cur[0] += C_Full_Cloud_Cur->points[i].x;
					Center_P_Cur[1] += C_Full_Cloud_Cur->points[i].y;
					Center_P_Cur[2] += C_Full_Cloud_Cur->points[i].z;
				}

				Center_P_Cur[0] /= C_Full_Cloud_Cur->size();
				Center_P_Cur[1] /= C_Full_Cloud_Cur->size();
				Center_P_Cur[2] /= C_Full_Cloud_Cur->size();
				if (Rough_T_Activation)
				{
					double Dist = sqrt(
						pow(Center_P_Cur[0] - Center_P_Prev[0], 2) +
						pow(Center_P_Cur[1] - Center_P_Prev[1], 2) +
						pow(Center_P_Cur[2] - Center_P_Prev[2], 2)
						);


					if (Dist > Rough_Translation_Th)
					{
						Rough_Translated = true;

						Rough_Translation(0, 3) = Center_P_Cur[0] - Center_P_Prev[0];
						Rough_Translation(1, 3) = Center_P_Cur[1] - Center_P_Prev[1];
						Rough_Translation(2, 3) = Center_P_Cur[2] - Center_P_Prev[2];

						pcl::transformPointCloud(*C_Full_Cloud_Prev, *C_Full_Cloud_Prev, Rough_Translation);
						pcl::transformPointCloud(*Virtual_Full, *Virtual_Full, Rough_Translation);
						pcl::transformPointCloud(*Virtual_Center, *Virtual_Center, Rough_Translation);
						pcl::transformPointCloud(*Virtual_Surface_Pivot, *Virtual_Surface_Pivot, Rough_Translation);
						pcl::transformPointCloud(*Virtual_Inner_Pivot, *Virtual_Inner_Pivot, Rough_Translation);
						pcl::transformPointCloud(*Virtual_Axis, *Virtual_Axis, Rough_Translation);

						for (int i = 0; i < Q_R->size(); i++)
						{
							pcl::PointCloud<pcl::PointXYZ> Cloud;
							Cloud = Q_R->front();
							Q_R->pop();
							pcl::transformPointCloud(Cloud, Cloud, Rough_Translation);
							Q_R->push(Cloud);
						}
					}
				}

				else
					Rough_T_Activation = true;

				Center_P_Prev[0] = Center_P_Cur[0];
				Center_P_Prev[1] = Center_P_Cur[1];
				Center_P_Prev[2] = Center_P_Cur[2];

#pragma endregion

#pragma region Translate Estimation

#pragma region Pivot_Prev_Extract

				Search.setInputCloud(C_Full_Cloud_Prev);
				for (int i = 0; i < Virtual_Surface_Pivot->size(); i++)
				{
					Search.radiusSearch(Virtual_Surface_Pivot->points[i], Base_Rad_Prev, Inliers[i].indices, Dist[i]);
					if (Inliers[Robust_Pivot_A].indices.size() < Inliers[i].indices.size())
						Robust_Pivot_A = i;
				}

				if (Robust_Pivot_B != Robust_Pivot_A)
				{
					if (abs((int)(Inliers[Robust_Pivot_B].indices.size() - Inliers[Robust_Pivot_A].indices.size())) < 10)
						Robust_Pivot_A = Robust_Pivot_B;

					else
						Robust_Pivot_B = Robust_Pivot_A;
				}

				Base_Indices_Prev->indices = Inliers[Robust_Pivot_A].indices;

				pcl::copyPointCloud(*C_Full_Cloud_Prev, Base_Indices_Prev->indices, *Base_Cloud_Prev);

				for (int i = 0; i < Dist[Robust_Pivot_A].size(); i++)
				{
					if (Dist[Robust_Pivot_A][i] * 100 > Pivot_Rad_Prev)
						break;

					Pivot_Indices_Prev->indices.push_back(i);
				}

				pcl::copyPointCloud(*Base_Cloud_Prev, Pivot_Indices_Prev->indices, *Pivot_Cloud_Prev);

				Calculate_Normal.setInputCloud(C_Full_Cloud_Prev);
				Calculate_Normal.setIndices(Base_Indices_Prev);
				Calculate_Normal.compute(*Normal_Prev);

				Calculate_Feature.setInputCloud(Base_Cloud_Prev);
				Calculate_Feature.setInputNormals(Normal_Prev);
				Calculate_Feature.setIndices(Pivot_Indices_Prev);
				Calculate_Feature.compute(*Feature_Prev);
#pragma endregion 

#pragma region Pivot_Cur_Extract

				Inliers[Robust_Pivot_A].indices.clear();
				Dist[Robust_Pivot_A].clear();

				Search.setInputCloud(C_Full_Cloud_Cur);

				double Cur_Base_Dist_Th = Base_Rad_Cur;

				while (1)
				{
					Search.radiusSearch(Virtual_Surface_Pivot->points[Robust_Pivot_A], Cur_Base_Dist_Th, Inliers[Robust_Pivot_A].indices, Dist[Robust_Pivot_A]);

					if (Inliers[Robust_Pivot_A].indices.size() < 100)
					{
						Cur_Base_Dist_Th += 0.015;
						Inliers[Robust_Pivot_A].indices.clear();
					}
					else
						break;
				}
				Base_Indices_Cur->indices = Inliers[Robust_Pivot_A].indices;
				pcl::copyPointCloud(*C_Full_Cloud_Cur, Base_Indices_Cur->indices, *Base_Cloud_Cur);

				double Cur_Pivot_Dist_Th = Cur_Base_Dist_Th - 0.02;

				for (int i = 0; i < Dist[Robust_Pivot_A].size(); i++)
				{
					if (Dist[Robust_Pivot_A][i] * 100 > Cur_Pivot_Dist_Th)
					{
						if (Pivot_Indices_Cur->indices.size() < 30)
							Cur_Pivot_Dist_Th += 0.005;
						else
							break;
					}
					Pivot_Indices_Cur->indices.push_back(i);
				}

				pcl::copyPointCloud(*Base_Cloud_Cur, Pivot_Indices_Cur->indices, *Pivot_Cloud_Cur);

				*Calculate_Normal_Method = new pcl::search::KdTree <pcl::PointXYZ>;
				*Calculate_Feature_Method = new pcl::search::KdTree <pcl::PointXYZ>;

				Calculate_Normal.setInputCloud(C_Full_Cloud_Cur);
				Calculate_Normal.setIndices(Base_Indices_Cur);
				Calculate_Normal.compute(*Normal_Cur);

				Calculate_Feature.setInputCloud(Base_Cloud_Cur);
				Calculate_Feature.setInputNormals(Normal_Cur);
				Calculate_Feature.setIndices(Pivot_Indices_Cur);
				Calculate_Feature.compute(*Feature_Cur);
#pragma endregion 

#pragma region Best Correspondence Search
				idx.clear();
				
				Search.setInputCloud(C_Full_Cloud_Prev);

				for (int i = 0; i < Virtual_Surface_Pivot->size(); i++)
				{
					Search.radiusSearch(Virtual_Surface_Pivot->points[i], 0.02, Index->indices, D);
					if (Index->indices.size() > 5)
						idx.insert(idx.end(), Index->indices.begin(), Index->indices.end());
				}
				
				pcl::copyPointCloud(*C_Full_Cloud_Prev, idx, *Cloud);
				
				vector<int> Nearest_P;
				vector<float> Nearest_Dist;
				vector<int> Cor_Idx;

				Correspondence_Dist_Min = 9999;
				Calculate_Correspondence.setInputCloud(Feature_Cur);
				Search.setInputCloud(C_Full_Cloud_Cur);

				for (int i = 0; i < Feature_Prev->size(); i++)
				{
					Cor_Idx.clear();
					Dist[0].clear();

					Calculate_Correspondence.nearestKSearch(*Feature_Prev, i, Correspondence_Search_Th, Cor_Idx, Dist[0]);

					for (int j = 0; j < Correspondence_Search_Th; j++)
					{
						Correspondence_Dist_Avg = 0;

						Translation(0, 3) = Pivot_Cloud_Cur->points[Cor_Idx[j]].x - Pivot_Cloud_Prev->points[i].x;
						Translation(1, 3) = Pivot_Cloud_Cur->points[Cor_Idx[j]].y - Pivot_Cloud_Prev->points[i].y;
						Translation(2, 3) = Pivot_Cloud_Cur->points[Cor_Idx[j]].z - Pivot_Cloud_Prev->points[i].z;

						pcl::transformPointCloud(*Cloud, *Fitness_Cloud, Translation);

						for (int k = 0; k < Fitness_Cloud->size(); k++)
						{
							Nearest_Dist.clear();
							Search.nearestKSearch(Fitness_Cloud->points[k], 1, Nearest_P, Nearest_Dist);
							Correspondence_Dist_Avg += Nearest_Dist[0];
						}

						Correspondence_Dist_Avg /= (double)Pivot_Cloud_Cur->size();

						if (Correspondence_Dist_Avg < Correspondence_Dist_Min)
						{
							Correspondence_Dist_Min = Correspondence_Dist_Avg;
							Correspondence_Idx_Cur = Cor_Idx[j];
							Correspondence_Idx_Prev = i;
						}

						Translation = Eigen::Matrix4f::Identity();
					}
				}
#pragma endregion 

#pragma region Translate Via Correspondece

				Translation(0, 3) = Pivot_Cloud_Cur->points[Correspondence_Idx_Cur].x - Pivot_Cloud_Prev->points[Correspondence_Idx_Prev].x;
				Translation(1, 3) = Pivot_Cloud_Cur->points[Correspondence_Idx_Cur].y - Pivot_Cloud_Prev->points[Correspondence_Idx_Prev].y;
				Translation(2, 3) = Pivot_Cloud_Cur->points[Correspondence_Idx_Cur].z - Pivot_Cloud_Prev->points[Correspondence_Idx_Prev].z;

				pcl::transformPointCloud(*Virtual_Full, *Virtual_Full, Translation);
				pcl::transformPointCloud(*Virtual_Center, *Virtual_Center, Translation);
				pcl::transformPointCloud(*Virtual_Surface_Pivot, *Virtual_Surface_Pivot, Translation);
				pcl::transformPointCloud(*Virtual_Inner_Pivot, *Virtual_Inner_Pivot, Translation);
				pcl::transformPointCloud(*Virtual_Axis, *Virtual_Axis, Translation);

				for (int i = 0; i < Q_R->size(); i++)
				{
					pcl::PointCloud<pcl::PointXYZ> Cloud;
					Cloud = Q_R->front();
					Q_R->pop();
					pcl::transformPointCloud(Cloud, Cloud, Translation);
					Q_R->push(Cloud);
				}

#pragma endregion 
#pragma endregion

#pragma region Determine ICP Activation

#pragma region Determine via Translation

				if (Q_T->size() == ICP_Activate_T_Delay)
				{
					Q_T->pop();
					*Rotation_Physical_Prev = Q_T->front();
				}

				else
					*Rotation_Physical_Prev = Q_T->front();

				if (Q_R->size() == ICP_Activate_R_Delay)
					Q_R->pop();
				if (!Rough_Translated)
				{
					ICP_Activate_T_Dist = sqrt(
						pow(Virtual_Surface_Pivot->points[Robust_Pivot_A].x - Rotation_Physical_Prev->points[Robust_Pivot_A].x, 2) +
						pow(Virtual_Surface_Pivot->points[Robust_Pivot_A].y - Rotation_Physical_Prev->points[Robust_Pivot_A].y, 2) +
						pow(Virtual_Surface_Pivot->points[Robust_Pivot_A].z - Rotation_Physical_Prev->points[Robust_Pivot_A].z, 2)
						);


#pragma endregion

#pragma region Determine via Rotation

					if (ICP_Activate_T_Dist < ICP_Activate_T_Th)
					{
						Search.setInputCloud(C_Full_Cloud_Cur);
						Search.radiusSearch(Virtual_Surface_Pivot->points[Robust_Pivot_A], 0.07, Rotation_Pivot_Indices->indices, D);

						pcl::copyPointCloud(*C_Full_Cloud_Cur, Rotation_Pivot_Indices->indices, *Rotation_Physical_Cur);

						*Rotation_Physical_Prev = Q_R->front();

						icp.setInputSource(Rotation_Physical_Cur);
						icp.setInputTarget(Rotation_Physical_Prev);
						icp.align(*Result);

						ICP_Activate_R_Dist = -1;

						for (int j = 0; j < Rotation_Physical_Cur->size(); j++)
						{
							double Dist = sqrt(
								pow(Result->points[j].x - Rotation_Physical_Cur->points[j].x, 2) +
								pow(Result->points[j].y - Rotation_Physical_Cur->points[j].y, 2) +
								pow(Result->points[j].z - Rotation_Physical_Cur->points[j].z, 2)
								);

							if (ICP_Activate_R_Dist < Dist)
								ICP_Activate_R_Dist = Dist;
						}

						if (ICP_Activate_R_Dist < ICP_Activate_R_Th && !Rough_Translated)
							ICP_Activate = false;
					}
				}

#pragma endregion

#pragma region ICP Tracking

				if (ICP_Activate)
				{
					icp.setInputSource(Base_Cloud_Cur);
					icp.setInputTarget(Virtual_Full);
					icp.align(*Result);

					Euclidean = icp.getFinalTransformation().inverse();

					pcl::transformPointCloud(*Virtual_Center, *Virtual_Center, Euclidean);
					pcl::transformPointCloud(*Virtual_Full, *Virtual_Full, Euclidean);
					pcl::transformPointCloud(*Virtual_Surface_Pivot, *Virtual_Surface_Pivot, Euclidean);
					pcl::transformPointCloud(*Virtual_Inner_Pivot, *Virtual_Inner_Pivot, Euclidean);
					pcl::transformPointCloud(*Virtual_Axis, *Virtual_Axis, Euclidean);
				}

				Q_T->push(*Virtual_Surface_Pivot);

				Euclidean = Euclidean * Translation * Rough_Translation;
				
#pragma endregion

#pragma region Surface Extraction

				Surface_Cloud->clear();
				pcl::PointCloud<pcl::PointXYZ>::Ptr Copy(new pcl::PointCloud<pcl::PointXYZ>);
				pcl::copyPointCloud(*C_Full_Cloud_Cur, *Copy);

				for (int i = 0; i < Virtual_Inner_Pivot->size(); i++)
				{
					Surface_Indices->indices.clear();
					D.clear();

					Surface_Extractor.deleteTree();
					Surface_Extractor.setInputCloud(Copy);
					Surface_Extractor.addPointsFromInputCloud();

					Surface_Extractor.radiusSearch(Virtual_Inner_Pivot->points[i], Surface_Search_Rad, Surface_Indices->indices, D);

					for (int j = 0; j < Surface_Indices->indices.size(); j++)
						Surface_Cloud->push_back(Copy->points[Surface_Indices->indices[j]]);

					Indices_Extracter.setInputCloud(Copy);
					Indices_Extracter.setIndices(Surface_Indices);

					Indices_Extracter.filter(*Copy);
				}

				pcl::copyPointCloud(*Surface_Cloud, *C_Full_Cloud_Prev);

				Q_R->push(*C_Full_Cloud_Prev);
#pragma endregion 

#pragma region Visualizing 

#pragma region Axis Filtering Area Modification
				Grid_min[0] = Virtual_Center->points[0].x - Axis_Filtering_Rad;
				Grid_max[0] = Virtual_Center->points[0].x + Axis_Filtering_Rad;
				Grid_min[1] = Virtual_Center->points[0].y - Axis_Filtering_Rad;
				Grid_max[1] = Virtual_Center->points[0].y + Axis_Filtering_Rad;
				Grid_min[2] = Virtual_Center->points[0].z - Axis_Filtering_Rad;
				Grid_max[2] = Virtual_Center->points[0].z + Axis_Filtering_Rad;
#pragma endregion 

				Grid_Pivot->points[0].x = Grid_min[0];
				Grid_Pivot->points[0].y = Grid_min[1];
				Grid_Pivot->points[0].z = Grid_min[2];

				Grid_Pivot->points[1].x = Grid_max[0];
				Grid_Pivot->points[1].y = Grid_min[1];
				Grid_Pivot->points[1].z = Grid_min[2];

				Grid_Pivot->points[2].x = Grid_min[0];
				Grid_Pivot->points[2].y = Grid_max[1];
				Grid_Pivot->points[2].z = Grid_min[2];

				Grid_Pivot->points[3].x = Grid_max[0];
				Grid_Pivot->points[3].y = Grid_max[1];
				Grid_Pivot->points[3].z = Grid_min[2];

				Grid_Pivot->points[4].x = Grid_min[0];
				Grid_Pivot->points[4].y = Grid_max[1];
				Grid_Pivot->points[4].z = Grid_max[2];

				Grid_Pivot->points[5].x = Grid_min[0];
				Grid_Pivot->points[5].y = Grid_min[1];
				Grid_Pivot->points[5].z = Grid_max[2];

				Grid_Pivot->points[6].x = Grid_max[0];
				Grid_Pivot->points[6].y = Grid_min[1];
				Grid_Pivot->points[6].z = Grid_max[2];

				Grid_Pivot->points[7].x = Grid_max[0];
				Grid_Pivot->points[7].y = Grid_max[1];
				Grid_Pivot->points[7].z = Grid_max[2];

				viewer->addPointCloud(C_Full_Cloud_Cur, "Physical", RunTime_Window);
				viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 0, 100, 255, "Physical");
				viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 4.0, "Physical");

				viewer->addPointCloud(Virtual_Full, "Virtual", RunTime_Window);
				viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 255, 0, 0, "Virtual");
				viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 0.5, "Virtual");

				viewer->addPointCloud(Virtual_Axis, "Virtual_Axis", RunTime_Window);
				viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 255, 255, 0, "Virtual_Axis");
				viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 6.0, "Virtual_Axis");

				Sx = Sy = Sz = 0.0;
				for (int i = 0; i < C_Full_Cloud_Prev->size(); i++)
				{
					Sx += C_Full_Cloud_Prev->points[i].x;
					Sy += C_Full_Cloud_Prev->points[i].y;
					Sz += C_Full_Cloud_Prev->points[i].z;
				}

				Sx /= (double)C_Full_Cloud_Prev->size();
				Sy /= (double)C_Full_Cloud_Prev->size();
				Sz /= (double)C_Full_Cloud_Prev->size();

				viewer->addPointCloud(C_Full_Cloud_Prev, "C_Full_Cloud_Prev", Surface_Window);
				viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 255, 255, 0, "C_Full_Cloud_Prev");
				viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 0.5, "C_Full_Cloud_Prev");

				for (int i = 0; i < 12; i++)
				{
					sprintf(text, "Line_%d", i);
					viewer->addLine(Grid_Pivot->points[Grid_Line_Idx_Start[i] - 1], Grid_Pivot->points[Grid_Line_Idx_End[i] - 1], text, RunTime_Window);
					viewer->setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 255, 255, 0, text);
					viewer->setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_LINE_WIDTH, 3, text);
				}

				for (int i = 0; i < 3; i++)
				{
					sprintf(text, "Axis_%d", i);
					viewer->addLine(Virtual_Axis->points[0], Virtual_Axis->points[i + 1], text, RunTime_Window);
					viewer->setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, red[i], grn[i], blu[i], text);
					viewer->setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_LINE_WIDTH, 3, text);
				}

				if (UI_2D_Visualization)
				{
					viewer->addText("3. Tracking", 10, 500, 30, 1, 1, 0, "3. Tracking", Info_Window);

					sprintf(text, "State");
					viewer->addText(text, 10, 450, 13, 1, 1, 0, text, Info_Window);

					if (ICP_Activate_T_Dist > ICP_Activate_T_Th)
					{
						sprintf(text, "Rigid");
						viewer->addText(text, 182, 450, 13, 1, 1, 1, text, Info_Window);
					}

					else if (ICP_Activate_T_Dist < ICP_Activate_T_Th && ICP_Activate_R_Dist > ICP_Activate_R_Th)
					{
						sprintf(text, "Rotating");
						viewer->addText(text, 182, 450, 13, 1, 1, 1, text, Info_Window);
					}

					else
					{
						sprintf(text, "Static");
						viewer->addText(text, 182, 450, 13, 1, 1, 1, text, Info_Window);
					}

					sprintf(text, "ICP Activation");
					viewer->addText(text, 10, 424, 13, 1, 1, 0, text, Info_Window);

					if (ICP_Activate)
					{
						sprintf(text, "Acticate");
						viewer->addText(text, 182, 424, 13, 1, 0, 0, text, Info_Window);
					}

					else
					{
						sprintf(text, "Disable");
						viewer->addText(text, 182, 424, 13, 0, 0, 1, text, Info_Window);
					}

					sprintf(text, "Frame Rate(sec)");
					viewer->addText(text, 10, 398, 13, 1, 1, 0, text, Info_Window);

					sprintf(text, "Pos  X");
					viewer->addText(text, 10, 372, 13, 1, 1, 0, text, Info_Window);

					sprintf(text, "%.2f", Virtual_Axis->points[0].x);
					viewer->addText(text, 182, 372, 13, 1, 1, 1, text, Info_Window);

					sprintf(text, "       Y");
					viewer->addText(text, 10, 359, 13, 1, 1, 0, text, Info_Window);

					sprintf(text, "%.2f", Virtual_Axis->points[0].y);
					viewer->addText(text, 182, 359, 13, 1, 1, 1, text, Info_Window);

					sprintf(text, "       Z");
					viewer->addText(text, 10, 346, 13, 1, 1, 0, text, Info_Window);

					sprintf(text, "%.2f", Virtual_Axis->points[0].z);
					viewer->addText(text, 182, 346, 13, 1, 1, 1, text, Info_Window);

					sprintf(text, "Robust Pivot Idx");
					viewer->addText(text, 10, 320, 13, 1, 1, 0, text, Info_Window);

					sprintf(text, "%d", Robust_Pivot_A);
					viewer->addText(text, 182, 320, 13, 1, 1, 1, text, Info_Window);

					sprintf(text, "|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|");
					viewer->addText(text, 170, 30, 13, 1, 1, 1, text, Info_Window);

					sprintf(text, "Surface Extraction  ------------------------------------------------------------");
					viewer->addText(text, 0, 150, 10, 1, 1, 0, text, Surface_Window);

					sprintf(text, "|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|");
					viewer->addText(text, 232, 0, 10, 1, 1, 0, "Bar1", Surface_Window);
					sprintf(text, "|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|\n|");
					viewer->addText(text, 235, 0, 10, 1, 1, 0, "Bar2", Surface_Window);
				}
#pragma endregion
			}

#pragma endregion
		}
#pragma region IPC

		if (IPC && Ready)
		{
			int idx = 0;
			int copy_num;

			for (int y = 0; y < 3; y++)
			{
				for (int x = 0; x < 4; x++)
				{
					if (Euclidean(y, x) < 0)
					{
						Packet[idx++] = '-';
						Euclidean(y, x) *= -1;
					}
					else
						Packet[idx++] = '+';

					for (int i = 0; i < 5; i++)
					{
						Euclidean(y, x) *= 10;
						copy_num = Euclidean(y, x);
						Euclidean(y, x) -= copy_num;

						Packet[idx++] = copy_num;
					}
				}
			}
			Transmiter->communicate_data(Packet, 72);
		}

#pragma endregion 

#pragma region Frame Rate
		end_ = pcl::getTime();

		if (Frame_Rate && ((clock() - FPS_Start) / (double)CLOCKS_PER_SEC > 2.0) && Ready)
		{
			//gotoxy(5, 3);

			if (FPS_Time->size() == 10)
			{
				FPS_Total -= FPS_Time->front();
				FPS_Time->pop();
			}

			FPS_Time->push(end_ - start);
			FPS_Total += FPS_Time->back();
		}

		start = pcl::getTime();

		sprintf(text, "%.2f", 1.0 / (FPS_Total / (double)FPS_Time->size()));
		viewer->addText(text, 182, 398, 30, 1, 1, 1, text, Info_Window);

#pragma endregion 

		viewer->spinOnce();

		if (Sx != 0.0 && Sy != 0.0)
		{
			viewer->setCameraPosition(Sx, Sy - 0.2, Sz + 0.3, 0.0, 1.0, 0.0, Surface_Window);
			viewer->setCameraClipDistances(0.0, 10.0, Surface_Window);
		}

		viewer->removeAllPointClouds();
		viewer->removeAllShapes();
	}
}

void Transformation_Matrix()
{
	Matrix = Eigen::Matrix4f::Identity();

	double temp[3][3] = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };
	double result_M[3][3] = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };

	double x_mat[3][3] = { { 1, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };
	double y_mat[3][3] = { { 0, 0, 0 }, { 0, 1, 0 }, { 0, 0, 0 } };
	double z_mat[3][3] = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 1 } };

	x_mat[1][1] = cos(Rotation_Angle[0] * Radian);
	x_mat[1][2] = sin(Rotation_Angle[0] * Radian)*-1;
	x_mat[2][1] = sin(Rotation_Angle[0] * Radian);
	x_mat[2][2] = cos(Rotation_Angle[0] * Radian);

	y_mat[0][0] = cos(Rotation_Angle[1] * Radian);
	y_mat[0][2] = sin(Rotation_Angle[1] * Radian);
	y_mat[2][0] = sin(Rotation_Angle[1] * Radian)*-1;
	y_mat[2][2] = cos(Rotation_Angle[1] * Radian);

	z_mat[0][0] = cos(Rotation_Angle[2] * Radian);
	z_mat[0][1] = sin(Rotation_Angle[2] * Radian)*-1;
	z_mat[1][0] = sin(Rotation_Angle[2] * Radian);
	z_mat[1][1] = cos(Rotation_Angle[2] * Radian);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			for (int l = 0; l < 3; l++)
				temp[i][j] += x_mat[i][l] * y_mat[l][j];
		}
	}

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			for (int l = 0; l < 3; l++)
				result_M[i][j] += temp[i][l] * z_mat[l][j];
		}
	}

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			Matrix(i, j) = result_M[i][j];
		}
	}
}


void keyboard_callback(const pcl::visualization::KeyboardEvent& event, void*)
{
	if (event.getKeyCode() && event.keyDown()){
		switch (event.getKeyCode())
		{
		case '.':
			break;
		case '/':
			break;

		case ';':
			break;
		case '"':
			break;

		case '[':
			break;
		case ']':
			break;

		case '&':
			break;
		case '*':
			break;
		case '(':
			break;
		case ')':
			break;

		case 'y': Ready = true;
			FPS_Start = clock();
			break;

		case 't':save_ = true;
			break;

		case '`': cout << endl << "=============================================================" << endl << endl;
			break;
		}
	}
}

void gotoxy(int x, int y)
{
	COORD Pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}