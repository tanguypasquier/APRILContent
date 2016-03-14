  /// \file PointingClusterAssociationAlgorithm.cc
/*
 *
 * PointingClusterAssociationAlgorithm.cc source template automatically generated by a class generator
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

#include "Pandora/AlgorithmHeaders.h"

#include "ArborTopologicalAssociation/PointingClusterAssociationAlgorithm.h"

#include "ArborHelpers/SortingHelper.h"
#include "ArborHelpers/GeometryHelper.h"
#include "ArborHelpers/ReclusterHelper.h"
#include "ArborHelpers/ClusterHelper.h"

#include <algorithm>

namespace arbor_content
{

pandora::StatusCode PointingClusterAssociationAlgorithm::Run()
{
	const pandora::ClusterList *pClusterList = NULL;
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

	if(pClusterList->empty())
		return pandora::STATUS_CODE_SUCCESS;

	// get candidate clusters for association
	pandora::ClusterVector clusterVector;

	for(pandora::ClusterList::const_iterator clusterIter = pClusterList->begin(), clusterEndIter = pClusterList->end() ;
			clusterEndIter != clusterIter ; ++clusterIter)
	{
		const pandora::Cluster *const pCluster = *clusterIter;

		if(!this->CanMergeCluster(pCluster))
			continue;

		clusterVector.push_back(pCluster);
	}

	// sort them by inner layer
	std::sort(clusterVector.begin(), clusterVector.end(), SortingHelper::SortClustersByInnerLayer);

	for(pandora::ClusterVector::reverse_iterator iter = clusterVector.rbegin(), endIter = clusterVector.rend() ;
			endIter != iter ; ++iter)
	{
		const pandora::Cluster *const pDaughterCluster = *iter;

		if(NULL == pDaughterCluster)
			continue;

		const pandora::Cluster *pBestParentCluster = NULL;
		PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->FindBestParentCluster(pDaughterCluster, clusterVector, pBestParentCluster));

		if(NULL == pBestParentCluster)
			continue;

		// if neutral cluster
		if(pBestParentCluster->GetAssociatedTrackList().empty())
		{
			PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBestParentCluster, pDaughterCluster));
			(*iter) = NULL;
		}
		// charged cluster case
		else
		{
			const pandora::TrackList &trackList(pBestParentCluster->GetAssociatedTrackList());
			float trackEnergySum(0.f);

		    for (pandora::TrackList::const_iterator trackIter = trackList.begin(), trackIterEnd = trackList.end(); trackIter != trackIterEnd; ++trackIter)
		        trackEnergySum += (*trackIter)->GetEnergyAtDca();

		    const float bestParentClusterEnergy = pBestParentCluster->GetTrackComparisonEnergy(this->GetPandora());
		    const float clusterEnergySum = bestParentClusterEnergy + pDaughterCluster->GetTrackComparisonEnergy(this->GetPandora());

		    const float oldChi = ReclusterHelper::GetTrackClusterCompatibility(this->GetPandora(), bestParentClusterEnergy, trackEnergySum);
		    const float newChi = ReclusterHelper::GetTrackClusterCompatibility(this->GetPandora(), clusterEnergySum, trackEnergySum);

		    // if we improve on chi2 or if chi2 still valid, associate
		    if(newChi*newChi < oldChi*oldChi || newChi*newChi < m_chi2AssociationCut)
		    {
				PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBestParentCluster, pDaughterCluster));
				(*iter) = NULL;
		    }
		}
	}

	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PointingClusterAssociationAlgorithm::CanMergeCluster(const pandora::Cluster *const pCluster) const
{
	if(NULL == pCluster)
		return false;

	if(!PandoraContentApi::IsAvailable(*this, pCluster))
		return false;

	if(pCluster->GetNCaloHits() < m_minNCaloHits || pCluster->GetNCaloHits() > m_maxNCaloHits)
		return false;

	const unsigned int firstPseudoLayer = pCluster->GetInnerPseudoLayer();
	const unsigned int lastPseudoLayer = pCluster->GetOuterPseudoLayer();

	unsigned int nPseudoLayers = lastPseudoLayer - firstPseudoLayer + 1;

	if(nPseudoLayers < m_minNPseudoLayers || nPseudoLayers >= m_maxNPseudoLayers)
		return false;

	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PointingClusterAssociationAlgorithm::FindBestParentCluster(const pandora::Cluster *const pDaughterCluster, const pandora::ClusterVector &clusterVector,
		const pandora::Cluster *&pBestParentCluster) const
{
	pBestParentCluster = NULL;

	if(NULL == pDaughterCluster)
		return pandora::STATUS_CODE_SUCCESS;

	const pandora::ClusterFitResult &daughterClusterFitResult(pDaughterCluster->GetFitToAllHitsResult());

	if( ! daughterClusterFitResult.IsFitSuccessful() )
	{
		std::cout << "Daughter cluster fit failed !" << std::endl;
		return pandora::STATUS_CODE_SUCCESS;
	}

	pandora::CartesianVector daughterClusterCentroid(0.f, 0.f, 0.f);
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ClusterHelper::GetCentroid(pDaughterCluster, daughterClusterCentroid));

	const pandora::Cluster *pBestBarycentreCluster = NULL;
	const pandora::Cluster *pBestInterceptCluster = NULL;
	float bestBarycentreImpactParameter(std::numeric_limits<float>::max());
	float bestInterceptImpactParameter(std::numeric_limits<float>::max());

	// loop over clusters and look for two parent clusters with two different computations
	for(pandora::ClusterVector::const_reverse_iterator iter = clusterVector.rbegin(), endIter = clusterVector.rend() ;
			endIter != iter ; ++iter)
	{
		const pandora::Cluster *const pParentCluster = *iter;

		if(NULL == pParentCluster)
			continue;

		if(pParentCluster == pDaughterCluster)
			continue;

		if(pParentCluster->GetInnerPseudoLayer() >= pDaughterCluster->GetInnerPseudoLayer())
			continue;

		const pandora::ClusterFitResult &parentClusterFitResult(pParentCluster->GetFitToAllHitsResult());

		if( ! parentClusterFitResult.IsFitSuccessful() )
		{
			std::cout << "Parent cluster fit failed !" << std::endl;
			continue;
		}

		pandora::CartesianVector parentClusterCentroid(0.f, 0.f, 0.f);
		PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ClusterHelper::GetCentroid(pParentCluster, parentClusterCentroid));

		PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->PerformBarycentreClusterComputation(daughterClusterFitResult, daughterClusterCentroid,
				parentClusterFitResult, parentClusterCentroid, pParentCluster, pBestBarycentreCluster, bestBarycentreImpactParameter));

		PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->PerformInterceptClusterComputation(daughterClusterFitResult, daughterClusterCentroid,
				parentClusterFitResult, parentClusterCentroid, pParentCluster, pBestInterceptCluster, bestInterceptImpactParameter));
	}

	// choose the best parent cluster to merge-in
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->ChooseBestParentCluster(pBestBarycentreCluster, pBestInterceptCluster, pDaughterCluster,
			pBestParentCluster));

	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PointingClusterAssociationAlgorithm::PerformBarycentreClusterComputation(const pandora::ClusterFitResult &daughterClusterFitResult, const pandora::CartesianVector &daughterClusterCentroid,
		const pandora::ClusterFitResult &parentClusterFitResult, const pandora::CartesianVector &parentClusterCentroid,
		const pandora::Cluster *const pParentCluster, const pandora::Cluster *&pBestBarycentreCluster, float &bestImpactParameter) const
{
	const pandora::CartesianVector centroidDifference(daughterClusterCentroid - parentClusterCentroid);
	const float clustersAngle = parentClusterFitResult.GetDirection().GetOpeningAngle(centroidDifference);

	float impactParameter = 0.f;

	if(pandora::STATUS_CODE_SUCCESS != GeometryHelper::GetClosestDistanceToLine(daughterClusterCentroid, daughterClusterFitResult.GetDirection(),
			parentClusterCentroid, impactParameter))
		return pandora::STATUS_CODE_SUCCESS;

	if(clustersAngle > m_clustersAngleCut
	|| impactParameter > m_barycentreImpactParameterCut
	|| impactParameter > bestImpactParameter)
		return pandora::STATUS_CODE_SUCCESS;

	bestImpactParameter = impactParameter;
	pBestBarycentreCluster = pParentCluster;

	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PointingClusterAssociationAlgorithm::PerformInterceptClusterComputation(const pandora::ClusterFitResult &daughterClusterFitResult, const pandora::CartesianVector &daughterClusterCentroid,
		const pandora::ClusterFitResult &parentClusterFitResult, const pandora::CartesianVector &parentClusterCentroid,
		const pandora::Cluster *const pParentCluster, const pandora::Cluster *&pBestInterceptCluster, float &bestImpactParameter) const
{
	float impactParameter = 0.f;

	if(pandora::STATUS_CODE_SUCCESS != GeometryHelper::GetClosestDistanceBetweenLines(daughterClusterCentroid, daughterClusterFitResult.GetDirection(),
			parentClusterCentroid, parentClusterFitResult.GetDirection(), impactParameter))
		return pandora::STATUS_CODE_SUCCESS;

	if(impactParameter > m_interceptImpactParameterCut || impactParameter > bestImpactParameter)
		return pandora::STATUS_CODE_SUCCESS;

	pandora::CartesianVector projectionOnParentClusterAxis(0.f, 0.f, 0.f);
	pandora::CartesianVector projectionOnDaughterClusterAxis(0.f, 0.f, 0.f);

	if(pandora::STATUS_CODE_SUCCESS != GeometryHelper::GetCrossingPointsBetweenLines(daughterClusterCentroid, daughterClusterFitResult.GetDirection(),
			parentClusterCentroid, parentClusterFitResult.GetDirection(), projectionOnDaughterClusterAxis, projectionOnParentClusterAxis))
		return pandora::STATUS_CODE_SUCCESS;

	float closestDistanceApproach = 0.f;

	if(pandora::STATUS_CODE_SUCCESS != ClusterHelper::GetClosestDistanceApproach(pParentCluster, projectionOnDaughterClusterAxis,
			closestDistanceApproach))
		return pandora::STATUS_CODE_SUCCESS;

	if(m_interceptClosestDistanceApproachCut < closestDistanceApproach)
		return pandora::STATUS_CODE_SUCCESS;

	pBestInterceptCluster = pParentCluster;
	bestImpactParameter = impactParameter;

	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PointingClusterAssociationAlgorithm::ChooseBestParentCluster(const pandora::Cluster *const pBarycentreParentCluster, const pandora::Cluster *const pInterceptParentCluster,
		const pandora::Cluster *const pDaughterCluster, const pandora::Cluster *&pBestParentCluster) const
{
	if(NULL == pBarycentreParentCluster && NULL == pInterceptParentCluster)
	{
		pBestParentCluster = NULL;
		return pandora::STATUS_CODE_SUCCESS;
	}

	if(pBarycentreParentCluster == pInterceptParentCluster)
	{
		pBestParentCluster = pBarycentreParentCluster;
		return pandora::STATUS_CODE_SUCCESS;
	}

	if(NULL != pBarycentreParentCluster && NULL == pInterceptParentCluster)
	{
		pBestParentCluster = pBarycentreParentCluster;
		return pandora::STATUS_CODE_SUCCESS;
	}

	if(NULL == pBarycentreParentCluster && NULL != pInterceptParentCluster)
	{
		pBestParentCluster = pInterceptParentCluster;
		return pandora::STATUS_CODE_SUCCESS;
	}

	// here, two different parent clusters have been found.
	// we need to decide which one is the best parent
	pandora::CartesianVector barycentreClusterCentroid(0.f, 0.f, 0.f);
	pandora::CartesianVector interceptClusterCentroid(0.f, 0.f, 0.f);
	pandora::CartesianVector daughterClusterCentroid(0.f, 0.f, 0.f);
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ClusterHelper::GetCentroid(pBarycentreParentCluster, barycentreClusterCentroid));
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ClusterHelper::GetCentroid(pInterceptParentCluster, interceptClusterCentroid));
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ClusterHelper::GetCentroid(pDaughterCluster, daughterClusterCentroid));

	const float barycentreCentroidDifference = (barycentreClusterCentroid - daughterClusterCentroid).GetMagnitude();
	const float interceptCentroidDifference = (interceptClusterCentroid - daughterClusterCentroid).GetMagnitude();

	pBestParentCluster = barycentreCentroidDifference < interceptCentroidDifference ?
			pBarycentreParentCluster : pInterceptParentCluster;

	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PointingClusterAssociationAlgorithm::ReadSettings(const pandora::TiXmlHandle xmlHandle)
{
	m_minNCaloHits = 10;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
	     "MinNCaloHits", m_minNCaloHits));

	m_maxNCaloHits = std::numeric_limits<unsigned int>::max();
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
	     "MaxNCaloHits", m_maxNCaloHits));

	m_minNPseudoLayers = 4;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
	     "MinNPseudoLayers", m_minNPseudoLayers));

	m_maxNPseudoLayers = std::numeric_limits<unsigned int>::max();
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
	     "MaxNPseudoLayers", m_maxNPseudoLayers));

	m_chi2AssociationCut = 1.f;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
		 "Chi2AssociationCut", m_chi2AssociationCut));

	m_clustersAngleCut = M_PI/6.f;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
		 "ClustersAngleCut", m_clustersAngleCut));

	m_barycentreImpactParameterCut = 20.f;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
		 "BarycentreImpactParameterCut", m_barycentreImpactParameterCut));

	m_interceptImpactParameterCut = 20.f;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
		 "InterceptImpactParameterCut", m_interceptImpactParameterCut));

	m_interceptClosestDistanceApproachCut = 20.f;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
		 "InterceptClosestDistanceApproachCut", m_interceptClosestDistanceApproachCut));

	return pandora::STATUS_CODE_SUCCESS;
}


} 

