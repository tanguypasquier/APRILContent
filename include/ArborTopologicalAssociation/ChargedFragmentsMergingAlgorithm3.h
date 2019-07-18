  /// \file ChargedFragmentsMergingAlgorithm3.h
/*
 *
 * ChargedFragmentsMergingAlgorithm3.h header template automatically generated by a class generator
 * Creation date : jeu. avr. 9 2015
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


#ifndef CHARGEDFRAGMENTSMERGINGALGORITHM3_H
#define CHARGEDFRAGMENTSMERGINGALGORITHM3_H 

#include "Pandora/Algorithm.h"
#include "Pandora/PandoraInputTypes.h"
#include "ArborApi/ArborInputTypes.h"

#include "ArborHelpers/CaloHitRangeSearchHelper.h"
#include "ArborHelpers/CaloHitNeighborSearchHelper.h"

namespace pandora { class ClusterFitResult; }

namespace arbor_content
{

/** 
 * @brief ChargedFragmentsMergingAlgorithm3 class
 */ 
class ChargedFragmentsMergingAlgorithm3 : public pandora::Algorithm
{
public:
	/**
	 *  @brief  Factory class for instantiating algorithm
	 */
	class Factory : public pandora::AlgorithmFactory
	{
	public:
		pandora::Algorithm *CreateAlgorithm() const;
	};

private:
	pandora::StatusCode Run();
	pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);
  
    arbor_content::ArborCluster* GetMainCluster(const pandora::CaloHitVector& caloHitVector);
  
    void GetNearbyClusters(pandora::Cluster* cluster, const std::vector<arbor_content::ArborCluster*>& clusterVector, 
			std::vector<arbor_content::ArborCluster*>& clustersInRange);

    bool CheckNearbyClusterWithCharge(const arbor_content::ArborCluster* pCluster, std::vector<arbor_content::ArborCluster*>& nearbyClusters, int charge);

    void SearchProperClusters(ArborCluster* startingCluster, 
		    std::vector<arbor_content::ArborCluster*>& allClusters,
			std::vector<arbor_content::ArborCluster*>& properClusters,
			std::vector<float>& distances);
  
	void FillClusters(std::map<const pandora::Cluster*, pandora::ClusterList>& clustersMergingMap, 
		  pandora::Cluster* cluster, pandora::Cluster* clusterToMerge);
  
	void MCClusterMerging(const pandora::ClusterList& clustersForMerging);
  
	void MergeClusters(std::map<const pandora::Cluster*, pandora::ClusterList>& clustersMergingMap);
  
	void MakeMergingMap(std::vector<ArborCluster*>& clustersToMerge, std::vector<ArborCluster*>& clusterVector, 
			std::map<const pandora::Cluster*, pandora::ClusterList>& clustersMergingMap);

    pandora::StatusCode CleanClusterForMerging(std::vector<ArborCluster*>& clusterVector);

    bool CheckStartingPoints(const pandora::Cluster* chargedCluster, const pandora::Cluster* fragment);

	/**
	 *  @brief  Get the eligible clusters for parent-daughter association
	 *
	 *  @param  clusterVector the cluster vector to receive
	 */
	pandora::StatusCode GetEligibleClusters(std::vector<ArborCluster*>& clusterVector) const;

	/**
	 *  @brief  Algorithm workhorse, find associations between daughter and parent cluster
	 *
	 *  @param  clusterVector the input cluster vector
	 *  @param  clusterToClusterMap the map of daughter to parent cluster to receive
	 */
	pandora::StatusCode FindClustersToMerge(const pandora::ClusterVector &clusterVector, ClusterToClusterMap &clusterToClusterMap) const;

	/**
	 *  @brief  Whether the cluster is eligible for association
	 *
	 *  @param  pCluster address of a candidate cluster for association
	 *
	 *  @return  boolean
	 */
	bool CanMergeCluster(const pandora::Cluster *const pCluster) const;

	/**
	 *  @brief  Find the best parent cluster to merge a daughter one
	 *
	 *  @param  pDaughterCluster address of the daughter cluster
	 *  @param  clusterVector a cluster vector
	 *  @param  pBestParentCluster address of the best parent cluster to receive
	 */
	pandora::StatusCode FindBestParentCluster(const pandora::Cluster *const pDaughterCluster, const pandora::ClusterVector &clusterVector,
			const pandora::Cluster *&pBestParentCluster) const;

private:
	float                            m_maxStartingClusterDistance;
	float                            m_maxClosestClusterDistance;

	bool                             m_debugOutput;
	bool                             m_debugOutput2;
	bool                             m_onlyUseConnectedHits;
	bool                             m_useMCPForPID;
	bool                             m_makeRecord;

	arma::mat m_clusterCentroidsMatrix;
	std::vector<ArborCluster*> m_clustersToMerge;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ChargedFragmentsMergingAlgorithm3::Factory::CreateAlgorithm() const
{
    return new ChargedFragmentsMergingAlgorithm3();
}

} 

#endif
