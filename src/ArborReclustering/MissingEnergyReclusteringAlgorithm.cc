  /// \file MissingEnergyReclusteringAlgorithm.cc
/*
 *
 * MissingEnergyReclusteringAlgorithm.cc source template automatically generated by a class generator
 * Creation date : mer. f�vr. 24 2016
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


#include "ArborReclustering/MissingEnergyReclusteringAlgorithm.h"

#include "Pandora/AlgorithmHeaders.h"

#include "ArborApi/ArborContentApi.h"
#include "ArborHelpers/ReclusterHelper.h"
#include "ArborHelpers/ClusterHelper.h"

namespace arbor_content
{

pandora::StatusCode MissingEnergyReclusteringAlgorithm::Run()
{
	// get current cluster list
	const pandora::ClusterList *pClusterList = NULL;
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

	if(pClusterList->empty())
		return pandora::STATUS_CODE_SUCCESS;

	pandora::ClusterVector clusterVector(pClusterList->begin(), pClusterList->end());
	const unsigned int nClusters(clusterVector.size());

	for(unsigned int i=0 ; i<nClusters ; ++i)
	{
		const pandora::Cluster *const pCluster = clusterVector[i];

		if( NULL == pCluster )
			continue;

		const pandora::TrackList trackList(pCluster->GetAssociatedTrackList());
		unsigned int nTrackAssociations(trackList.size());

		if((nTrackAssociations < m_minTrackAssociations) || (nTrackAssociations > m_maxTrackAssociations))
			continue;

		const float chi = ReclusterHelper::GetTrackClusterCompatibility(this->GetPandora(), pCluster, trackList);

		// check for chi2 and missing energy in charged cluster
		if(chi*chi < m_minChi2ToRunReclustering || chi > 0.f)
			continue;

		// prepare clusters and tracks for reclustering
	    pandora::ClusterList reclusterClusterList;
	    reclusterClusterList.insert(pCluster);
	    pandora::TrackList reclusterTrackList(trackList);

	    UIntVector originalClusterIndices(1, i);

	    // find nearby clusters to potentially merge-in
		pandora::CartesianVector clusterCentroid(0.f, 0.f, 0.f);
		PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ClusterHelper::GetCentroid(pCluster, clusterCentroid));

		for(unsigned int j=0 ; j<nClusters ; ++j)
		{
			const pandora::Cluster *const pOtherCluster = clusterVector[j];

			if((NULL == pOtherCluster) || (pCluster == pOtherCluster))
				continue;

			float clusterHitsDistance = std::numeric_limits<float>::max();
			PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ClusterHelper::GetClosestDistanceApproach(pOtherCluster, pCluster, clusterHitsDistance));

			if(clusterHitsDistance < m_maxClusterHitsDistance)
			{
				reclusterClusterList.insert(pOtherCluster);
				originalClusterIndices.push_back(j);

				const pandora::TrackList associatedTrackList(pOtherCluster->GetAssociatedTrackList());

				if(!associatedTrackList.empty())
					reclusterTrackList.insert(associatedTrackList.begin(), associatedTrackList.end());
			}
		}

		if(1 == reclusterClusterList.size())
			continue;

	    // initialize reclustering
	    std::string originalClusterListName;
	    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ArborContentApi::InitializeReclustering(*this,
	    		reclusterTrackList, reclusterClusterList, originalClusterListName));

	    float bestChi(chi);
	    std::string bestReclusterClusterListName(originalClusterListName);

	    for(pandora::StringVector::const_iterator clusteringAlgIter = m_clusteringAlgorithmList.begin(), endClusteringAlgIter = m_clusteringAlgorithmList.end() ;
	    		endClusteringAlgIter != clusteringAlgIter ; ++clusteringAlgIter)
	    {
	    	const pandora::ClusterList *pReclusterClusterList = NULL;
	    	std::string reclusterClusterListName;
	    	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ArborContentApi::RunReclusteringAlgorithm(*this,
	    			*clusteringAlgIter, pReclusterClusterList, reclusterClusterListName));

	    	if(pReclusterClusterList->empty())
	    		continue;

	    	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_associationAlgorithmName));
	    	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ArborContentApi::PostRunReclusteringAlgorithm(*this, reclusterClusterListName));

	    	ReclusterResult reclusterResult;
	    	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ReclusterHelper::ExtractReclusterResults(this->GetPandora(), *pReclusterClusterList, reclusterResult));

	    	const float newChi(reclusterResult.GetChi());
	    	const float newChi2(reclusterResult.GetChi2());

	    	if(newChi2 < bestChi*bestChi)
	    	{
	    		bestChi = newChi;
	    		bestReclusterClusterListName = reclusterClusterListName;

	    		if(newChi2 < m_maxChi2ToStopReclustering)
	    			break;
	    	}
	    }

        // tidy the cluster vector used for reclustering
        if( originalClusterListName != bestReclusterClusterListName )
        {
        	for(UIntVector::const_iterator iter = originalClusterIndices.begin(), endIter = originalClusterIndices.end() ;
        			endIter != iter ; ++iter)
        		clusterVector[*iter] = NULL;
        }

        // Recreate track-cluster associations for chosen recluster candidates
        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::TemporarilyReplaceCurrentList<pandora::Cluster>(*this, bestReclusterClusterListName));
        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_trackClusterAssociationAlgName));
        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ArborContentApi::EndReclustering(*this, bestReclusterClusterListName));

    	// run monitoring algorithm if provided
    	if(!m_monitoringAlgorithmName.empty())
    	{
	    	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this,
	    			m_monitoringAlgorithmName));
    	}
	}

	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode MissingEnergyReclusteringAlgorithm::ReadSettings(const pandora::TiXmlHandle xmlHandle)
{
	m_minTrackAssociations = 1;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
		"MinTrackAssociations", m_minTrackAssociations));

	m_maxTrackAssociations = 1;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
		"MaxTrackAssociations", m_maxTrackAssociations));

	m_minChi2ToRunReclustering = 2.5f;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
		 "MinChi2ToRunReclustering", m_minChi2ToRunReclustering));

	m_maxChi2ToStopReclustering = 0.8f;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
		 "MaxChi2ToStopReclustering", m_maxChi2ToStopReclustering));

	m_maxClusterCentroidDistance = 300.f;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
		 "MaxClusterCentroidDistance", m_maxClusterCentroidDistance));

	m_maxClusterHitsDistance = 30.f;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
		 "MaxClusterHitsDistance", m_maxClusterHitsDistance));

	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ProcessAlgorithmList(*this, xmlHandle,
		 "clusteringAlgorithms", m_clusteringAlgorithmList));

	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ProcessAlgorithm(*this, xmlHandle,
		 "ClusterAssociation", m_associationAlgorithmName));

	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ProcessAlgorithm(*this, xmlHandle,
		 "TrackClusterAssociation", m_trackClusterAssociationAlgName));

	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ProcessAlgorithm(*this, xmlHandle,
		 "Monitoring", m_monitoringAlgorithmName));

	return pandora::STATUS_CODE_SUCCESS;
}


} 

