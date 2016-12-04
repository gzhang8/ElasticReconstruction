#include "stdio.h"
#include "IntegrateApp.h"
#include <iostream>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>


CIntegrateApp::CIntegrateApp( pcl::Grabber & source, bool use_device )
	: cols_( 640 ), rows_( 480 )
	, volume_( cols_, rows_ )
	, capture_( source )
	, use_device_( use_device )
	, exit_( false )
	, registration_( false )
	, time_ms_( 0 )
	, frame_id_( 0 )
	, traj_filename_( "" )
	, pose_filename_( "" )
	, seg_filename_( "" )
	, camera_filename_( "" )
	, ctr_filename_( "" )
	, pcd_filename_( "world.pcd" )
	, ctr_num_( 0 )
	, ctr_resolution_( 8 )
	, ctr_interval_( 50 )
	, ctr_length_( 3.0 )
	, start_from_( -1 )
	, end_at_( 100000000 )
	, saved_frame_count_(0)
	, snapshot_rate_(45)
{
	registration_ = capture_.providesCallback< pcl::ONIGrabber::sig_cb_openni_image_depth_image > ();
	cout << "Registration mode: " << ( registration_ ? "On" : "Off (not supported by source)" ) << endl;

	depth_.resize( cols_ * rows_ );
	scaled_depth_.resize( cols_ * rows_ );

#ifdef IMAGE_VIEWER
	viewer_depth_.setWindowTitle( "Depth stream" );
	viewer_depth_.setPosition( 0, 0 );
#endif
}

CIntegrateApp::~CIntegrateApp(void)
{
}

void CIntegrateApp::Init()
{
	if ( boost::filesystem::exists( camera_filename_ ) ) {
		volume_.camera_.LoadFromFile( camera_filename_ );
	}

	// screenshot
	screenshot_manager_.setCameraIntrinsics(); //(volume_.camera_.fx_);


	if ( ctr_num_ > 0 && boost::filesystem::exists( ctr_filename_ ) && boost::filesystem::exists( seg_filename_ ) ) {
		grids_.resize( ctr_num_ );
		FILE * f = fopen( ctr_filename_.c_str(), "r" );
		for ( int i = 0; i < ctr_num_; i++ ) {
			grids_[ i ].Load( f, ctr_resolution_, ctr_length_ );
		}
		fclose( f );
	} else {
		ctr_num_ = 0;
	}

	if ( boost::filesystem::exists( traj_filename_ ) ) {
		traj_.LoadFromFile( traj_filename_ );
	}

	if ( boost::filesystem::exists( seg_filename_ ) ) {
		seg_traj_.LoadFromFile( seg_filename_ );

		if ( boost::filesystem::exists( pose_filename_ ) ) {
			pose_traj_.LoadFromFile( pose_filename_ );
			traj_.data_.clear();
			for ( int i = 0; i < ( int )pose_traj_.data_.size(); i++ ) {
				for ( int j = 0; j < ctr_interval_; j++ ) {
					int idx = i * ctr_interval_ + j;
					traj_.data_.push_back( FramedTransformation( idx, idx, idx + 1, pose_traj_.data_[ i ].transformation_ * seg_traj_.data_[ idx ].transformation_ ) );
 				}
			}
			printf( "Trajectory created from pose and segment trajectories.\n" );
		}
	}
}

void CIntegrateApp::StartMainLoop( bool triggered_capture )
{
	using namespace openni_wrapper;
	typedef boost::shared_ptr< DepthImage > DepthImagePtr;
	typedef boost::shared_ptr< Image > ImagePtr;
        
	boost::function<void (const ImagePtr&, const DepthImagePtr&, float constant)> call_back_func;
	
	if ( !triggered_capture ) {
		call_back_func = boost::bind( &CIntegrateApp::source_cb2, this, _1, _2, _3 );
	} else {
		call_back_func = boost::bind( &CIntegrateApp::source_cb2_trigger, this, _1, _2, _3 );
	}
	boost::signals2::connection c = capture_.registerCallback ( call_back_func );

	{
		boost::unique_lock< boost::mutex > lock( data_ready_mutex_ );
	
		capture_.start (); // Start stream
    
		int ten_has_data_fail_then_we_call_it_a_day = 0;
		while ( !exit_ ) {
			bool has_data;
			if ( triggered_capture ) {
				( ( pcl::ONIGrabber * ) &capture_ )->start();//trigger(); // Triggers new frame
			}
			has_data = data_ready_cond_.timed_wait( lock, boost::posix_time::millisec( 300 ) );

			if ( has_data ) {
				ten_has_data_fail_then_we_call_it_a_day = 0;
			} else {
				ten_has_data_fail_then_we_call_it_a_day++;
			}
			try {
				this->Execute( has_data );

#ifdef IMAGE_VIEWER
				viewer_depth_.showShortImage( &depth_[ 0 ], cols_, rows_, 0, 3000, true );
				viewer_depth_.spinOnce( 3 );
#endif
			}
			catch (const std::bad_alloc& /*e*/) { cout << "Bad alloc" << endl; break; }
			catch (const std::exception& /*e*/) { cout << "Exception" << endl; break; }

			if ( ten_has_data_fail_then_we_call_it_a_day >= 10 ) {
				exit_ = true;
			}
		}
      
		if ( !triggered_capture ) {
			capture_.stop (); // Stop stream
		}

		volume_.SaveWorld( pcd_filename_ );

		cout << "Total " << frame_id_ << " frames processed." << endl;

		//volume_.SaveWorld( std::string( "world.pcd" ) );
	}
	c.disconnect();
}

//////////////////////////////////////////////
// Capture functions
//////////////////////////////////////////////
void CIntegrateApp::source_cb2( const boost::shared_ptr< openni_wrapper::Image >& image_wrapper, const boost::shared_ptr< openni_wrapper::DepthImage >& depth_wrapper, float )
{
	{
		boost::mutex::scoped_try_lock lock( data_ready_mutex_ );
		if ( exit_ || !lock )
			return;

		if ( depth_wrapper->getWidth() != image_wrapper->getWidth() || depth_wrapper->getHeight() != image_wrapper->getHeight() ) {
			PCL_ERROR( "Resolution of depth stream and image stream must match." );
			return;
		}

		if ( depth_wrapper->getWidth() != cols_ || depth_wrapper->getHeight() != rows_ ) {
			cols_ = depth_wrapper->getWidth();
			rows_ = depth_wrapper->getHeight();
			depth_.resize( cols_ * rows_ );
			scaled_depth_.resize( cols_ * rows_ );
		}
                  
		depth_wrapper->fillDepthImageRaw( cols_, rows_, &depth_[ 0 ] );
	}
	data_ready_cond_.notify_one();
}

void CIntegrateApp::source_cb2_trigger( const boost::shared_ptr< openni_wrapper::Image >& image_wrapper, const boost::shared_ptr< openni_wrapper::DepthImage >& depth_wrapper, float )
{
	{
		boost::mutex::scoped_lock lock( data_ready_mutex_ );
		if ( exit_ )
			return;

		if ( depth_wrapper->getWidth() != cols_ || depth_wrapper->getHeight() != rows_ ) {
			cols_ = depth_wrapper->getWidth();
			rows_ = depth_wrapper->getHeight();
			depth_.resize( cols_ * rows_ );
			scaled_depth_.resize( cols_ * rows_ );
		}
                  
		depth_wrapper->fillDepthImageRaw( cols_, rows_, &depth_[ 0 ] );
		frame_id_ = depth_wrapper->getDepthMetaData().FrameID();

		rgb24_.cols = image_wrapper->getWidth();
        rgb24_.rows = image_wrapper->getHeight();
        rgb24_.step = rgb24_.cols * rgb24_.elemSize(); 
  
        source_image_data_.resize(rgb24_.cols * rgb24_.rows);
        image_wrapper->fillRGB(rgb24_.cols, rgb24_.rows, (unsigned char*)&source_image_data_[0]);
        rgb24_.data = &source_image_data_[0];    


	}
	data_ready_cond_.notify_one();
}


bool UVD2XYZ( int u, int v, unsigned short d, float & x, float & y, float & z ) {
		if ( d > 0 ) {
			z = d / 1000.0f;
			x = ( u - 328.94272f ) * z / 529.21508f;
			y = ( v - 267.4806817f ) * z / 525.5639363f;
			return true;
		} else {
			return false;
		}
}

void CIntegrateApp::Execute( bool has_data )
{
	std::cout << frame_id_<< std::endl;
	if ( !has_data ) {
		return;
	}

	if ( traj_.data_[ frame_id_ - 1 ].frame_ == -1 ) {
		return;
	}

	if ( frame_id_ >= traj_.data_.size() ) {
		exit_ = true;
		return;
	}

	if ( frame_id_ % 100 == 0 ) {
		int tmp = traj_.data_.size();
		printf( "Frames processed : %d / %d\n", frame_id_, tmp );
	}

	if ( frame_id_ < start_from_ || frame_id_ > end_at_ ) {
		if ( frame_id_ > end_at_ ) {
			printf( "Reaching the specified end point.\n" );
			exit_ = true;
		}
		return;
	}

	if ( ctr_num_ > 0 ) {
		Reproject();
		if ( exit_ ) {
			return;
		}
	}

	//save pcd

	pcl::PointCloud<pcl::PointXYZRGB> cloud;

  	// Fill in the cloud data
  	cloud.width    = cols_;
  	cloud.height   = rows_;
  	cloud.is_dense = false;
  	cloud.points.resize (cloud.width * cloud.height);

  	
	float x =0 , y =0 , z =0;
	for ( int v = 0; v < rows_; v += 1 ) {
		for ( int u = 0; u < cols_; u += 1 ) {
			int i = v * cols_ + u;
			unsigned short d = depth_[ v * cols_ + u ];
			UVD2XYZ( u, v, d, x, y, z);
			cloud.points[i].x = x;
			cloud.points[i].y = y;
			cloud.points[i].z = z;
			cloud.points[i].r = rgb24_.data[i].r;
			cloud.points[i].g = rgb24_.data[i].g;
			cloud.points[i].b = rgb24_.data[i].b;
		}
	}
	std::string frame_name = std::to_string(frame_id_);
	pcl::io::savePCDFileASCII (frame_name + ".pcd", cloud);

	//save pose

	std::ofstream file(frame_name + ".txt");
    if (file.is_open())
    {
      
      file << traj_.data_[ frame_id_ - 1 ].transformation_;
	}
	file.close();


	// volume_.ScaleDepth( depth_, scaled_depth_ );
	// volume_.Integrate( depth_, scaled_depth_, traj_.data_[ frame_id_ - 1 ].transformation_ );
	// if(saved_frame_count_ % snapshot_rate_ == 0)
	// {
	// 	Eigen::Matrix4f fourfRT = traj_.data_[ frame_id_ - 1 ].transformation_.cast<float>();
	// 	Eigen::Affine3f F ;
	// 	F.matrix() = fourfRT;
	// 	screenshot_manager_.saveImage (F, rgb24_);
	// }
	// saved_frame_count_++;
}



void CIntegrateApp::Reproject()
{
	if ( frame_id_ > ctr_interval_ * ctr_num_ ) {
		exit_ = true;
		return;
	}

	depth_buffer_.resize( depth_.size() );
	for ( int i = 0; i < cols_ * rows_; i++ ) {
		depth_buffer_[ i ] = depth_[ i ];
		depth_[ i ] = 0;
	}

	int chunk = ( frame_id_ - 1 ) / ctr_interval_;
	Eigen::Matrix4d TiT0Ai_adj = traj_.data_[ frame_id_ - 1 ].transformation_.inverse() * traj_.data_[ 0 ].transformation_ * seg_traj_.data_[ 0 ].transformation_.inverse();

	int uu, vv;
	unsigned short dd;
	double x, y, z;
	for ( int v = 0; v < rows_; v += 1 ) {
		for ( int u = 0; u < cols_; u += 1 ) {
			unsigned short d = depth_buffer_[ v * cols_ + u ];
			if ( volume_.UVD2XYZ( u, v, d, x, y, z ) ) {
				Eigen::Vector4d dummy = seg_traj_.data_[ frame_id_ - 1 ].transformation_ * Eigen::Vector4d( x, y, z, 1 );
				Coordinate coo;
				Eigen::Vector3f pos;

				if ( grids_[ chunk ].GetCoordinate( Eigen::Vector3f( dummy( 0 ), dummy( 1 ), dummy( 2 ) ), coo ) ) {		// in the box, thus has the right coo
					grids_[ chunk ].GetPosition( coo, pos );
					Eigen::Vector4d reproj_pos = TiT0Ai_adj * Eigen::Vector4d( pos( 0 ), pos( 1 ), pos( 2 ), 1.0 );

					if ( volume_.XYZ2UVD( reproj_pos( 0 ), reproj_pos( 1 ), reproj_pos( 2 ), uu, vv, dd ) ) {
						unsigned short ddd = depth_[ vv * cols_ + uu ];
						if ( ddd == 0 || ddd > dd ) {
							depth_[ vv * cols_ + uu ] = dd;
						}
					}
				}
			}
		}
	}
}