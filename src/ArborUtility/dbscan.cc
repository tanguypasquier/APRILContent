#include "ArborUtility/dbscan.h"

#include <mlpack/methods/dbscan/dbscan.hpp>

namespace arbor_content
{
	ArborDBSCAN::ArborDBSCAN()
	{
	}


	void ArborDBSCAN::DoClustering()
	{
	     arma::mat dataset = {
	          {1.0000,   1.0000,   1.0000, 100., 100},
	          {2.0000,   3.0000,   2.0000, 0.,   1.},
	          {2.0000,   2.0000,   3.0000, 0.,   2.}
	     };

	     dataset.print();

	     mlpack::dbscan::DBSCAN<> dbscan(20., 2);

         // the assignments of points
         arma::Row< size_t > points;

         dbscan.Cluster(dataset, points);

         for (size_t i = 0; i < points.size(); ++i)
         {
             unsigned int iCluster = points.at(i);
               
             if(iCluster>=0)
                std::cout << "elment " << i << " is in cluster: " << iCluster << std::endl;
             else
                std::cout << "elment " << i << " is not clustered" << std::endl;
         } 
	}
} 
