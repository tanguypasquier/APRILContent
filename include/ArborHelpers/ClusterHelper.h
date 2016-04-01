  /// \file ClusterHelper.h
/*
 *
 * ClusterHelper.h header template automatically generated by a class generator
 * Creation date : ven. avr. 10 2015
 *
 * This file is part of ArborContent libraries.
 * 
 * ArborContent is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 * 
 * ArborContent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with ArborContent.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */


#ifndef CLUSTERHELPER_H
#define CLUSTERHELPER_H

#include "Pandora/PandoraInternal.h"
#include "Pandora/PandoraInputTypes.h"

namespace arbor_content
{

/** 
 *  @brief  ClusterHelper class
 */ 
class ClusterHelper 
{
public:
	/**
	 *  @brief  Get the cluster centroid
	 *
	 *  @param  pCluster the cluster to evaluate the centroid
	 *  @param  centroid the centroid to receive
	 */
	static pandora::StatusCode GetCentroid(const pandora::Cluster *const pCluster, pandora::CartesianVector &centroid);

	/**
	 *  @brief  Get the closest distance approach between all the hits of the cluster and the given position vector
	 *
	 *  @param  pCluster the cluster to evaluate the closest distance approach
	 *  @param  point the position vector
	 *  @param  closestDistance the closest distance approach between the point and the cluster, unit mm
	 */
	static pandora::StatusCode GetClosestDistanceApproach(const pandora::Cluster *const pCluster, const pandora::CartesianVector &point,
			float &closestDistance);

	/**
	 *  @brief  Get the distance between the cluster centroid and the given position vector
	 *
	 *  @param  pCluster the cluster to evaluate the closest distance approach
	 *  @param  point the position vector
	 *  @param  centroidDistance the distance between the point and the cluster centroid, unit mm
	 */
	static pandora::StatusCode GetCentroidDistance(const pandora::Cluster *const pCluster, const pandora::CartesianVector &point,
			float &centroidDistance);

	/**
	 *  @brief  Get the closest distance approach between all the hits of the clusters
	 *
	 *  @param  pCluster the first cluster
	 *  @param  pCluster the second cluster
	 *  @param  closestDistance the closest distance approach between the two clusters, unit mm
	 */
	static pandora::StatusCode GetClosestDistanceApproach(const pandora::Cluster *const pCluster1, const pandora::Cluster *const pCluster2,
			float &closestDistance);

	/**
	 *  @brief  Get the number of calo hit seeds in the cluster
	 *
	 *  @param  pCluster the cluster to get the number of calo hit seeds
	 *  @param  nSeeds the number of seeds to receive
	 */
	static pandora::StatusCode GetNCaloHitSeeds(const pandora::Cluster *const pCluster, unsigned int &nSeeds);
};



} 

#endif  //  CLUSTERHELPER_H
