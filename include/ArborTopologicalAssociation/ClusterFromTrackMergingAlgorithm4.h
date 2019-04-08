  /// \file ClusterFromTrackMergingAlgorithm4.h
/*
 *
 * ClusterFromTrackMergingAlgorithm4.h header template automatically generated by a class generator
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


#ifndef CLUSTERFROMTRACKMERGINGALGORITHM4_H 
#define CLUSTERFROMTRACKMERGINGALGORITHM4_H

#include "Pandora/Algorithm.h"
#include "Pandora/PandoraInputTypes.h"
#include "ArborApi/ArborInputTypes.h"

#include "ArborHelpers/CaloHitRangeSearchHelper.h"
#include "ArborHelpers/CaloHitNeighborSearchHelper.h"

namespace pandora { class ClusterFitResult; }

namespace arbor_content
{

/** 
 * @brief ClusterFromTrackMergingAlgorithm4 class
 */ 
class ClusterFromTrackMergingAlgorithm4 : public pandora::Algorithm
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

    void SearchProperClusters(ArborCluster* startingCluster, 
		  std::vector<arbor_content::ArborCluster*>& properClusters);

    void SearchProperClusters(const pandora::Track* pTrack, ArborCluster* startingCluster, 
		  std::vector<arbor_content::ArborCluster*>& properClusters);

    pandora::StatusCode CleanClusterForMerging(std::vector<ArborCluster*>& clusterVector);

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

	/**
	 *  @brief  Get the cluster backward direction and the inner cluster position using a cluster fit of the n first layers
	 *
	 *  @param  pCluster the input cluster address
	 *  @param  backwardDirection the backward direction cartesian vector to receive
	 *  @param  innerPosition the inner cluster position to receive
	 */
	pandora::StatusCode GetClusterBackwardDirection(const pandora::Cluster *const pCluster, pandora::CartesianVector &backwardDirection, pandora::CartesianVector &innerPosition) const;

private:
	bool                             m_discriminatePhotonPid;            ///< Whether to discriminate photons
	bool                             m_allowNeutralParentMerging;
	unsigned int                     m_minNCaloHits;
	unsigned int                     m_maxNCaloHits;
	unsigned int                     m_minNPseudoLayers;
	unsigned int                     m_maxNPseudoLayers;
	float                            m_chi2AssociationCut;
	unsigned int                     m_nBackwardLayersFit;
	float                            m_maxBackwardAngle;
	float                            m_maxBackwardDistanceFine;
	float                            m_maxBackwardDistanceCoarse;
	unsigned int                     m_maxBackwardPseudoLayer;
	unsigned int                     m_minParentClusterBackwardNHits;
	float                            m_maxClusterFitDca;
	float                            m_minClusterCosOpeningAngle;
	float                            m_minClusterFitCosOpeningAngle;
	float                            m_minClusterFitCosOpeningAngle2;

	float                            m_maxStartingClusterDistance;
	float                            m_maxClusterDistanceToMerge;
	float                            m_maxClosestPhotonDistance;

	bool                             m_mergePhotonClusters;
	bool                             m_mergeChargedClusters;

	arma::mat m_clusterCentroidsMatrix;
	std::vector<ArborCluster*> m_clustersToMerge;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ClusterFromTrackMergingAlgorithm4::Factory::CreateAlgorithm() const
{
    return new ClusterFromTrackMergingAlgorithm4();
}

} 

#endif
