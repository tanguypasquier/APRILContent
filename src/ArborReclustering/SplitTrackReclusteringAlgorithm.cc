/// \file SplitTrackReclusteringAlgorithm.cc
/*
 *
 * SplitTrackReclusteringAlgorithm.cc source template automatically generated by a class generator
 * Creation date : mar. d�c. 8 2015
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


#include "ArborReclustering/SplitTrackReclusteringAlgorithm.h"

#include "Pandora/AlgorithmHeaders.h"
#include "ArborHelpers/ReclusterHelper.h"
#include "ArborHelpers/CaloHitHelper.h"
#include "ArborHelpers/GeometryHelper.h"
#include "ArborApi/ArborContentApi.h"

namespace arbor_content
{

  pandora::StatusCode SplitTrackReclusteringAlgorithm::Run()
  {
	   const pandora::ClusterList* photonList = NULL;
	   const std::string photonListName("PhotonClusters");
	   PandoraContentApi::GetList(*this, photonListName, photonList);
	   std::cout << "photonList: " << photonList->size() << std::endl;

    // Get current cluster list
    const pandora::ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

	std::cout << "pClusterList: " << pClusterList->size() << std::endl;
    if( pClusterList->empty() )
      return pandora::STATUS_CODE_SUCCESS;

    const float bField(PandoraContentApi::GetPlugins(*this)->GetBFieldPlugin()->GetBField(pandora::CartesianVector(0.f, 0.f, 0.f)));

    pandora::ClusterVector clusterVector(pClusterList->begin(), pClusterList->end());

    for(pandora::ClusterVector::iterator iter = clusterVector.begin(), endIter = clusterVector.end() ;
        endIter != iter ; ++iter)
    {
      const pandora::Cluster *const pOriginalCluster(*iter);

      if(NULL == pOriginalCluster)
        continue;

      const pandora::TrackList trackList(pOriginalCluster->GetAssociatedTrackList());
      const unsigned int nTracks(trackList.size());

      if(nTracks < m_minNTrackAssociation)
        continue;

      pandora::OrderedCaloHitList orderedCaloHitList(pOriginalCluster->GetOrderedCaloHitList());
      PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, orderedCaloHitList.Add(pOriginalCluster->GetIsolatedCaloHitList()));

      // Remove arbor connections
      pandora::CaloHitList clusterCaloHitList;
      orderedCaloHitList.FillCaloHitList(clusterCaloHitList);

      PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, CaloHitHelper::RemoveConnections(&clusterCaloHitList));

      // Initialize cluster fragmentation
      pandora::ClusterList clusterList;
      clusterList.push_back(pOriginalCluster);
      std::string originalClustersListName, fragmentClustersListName;

      PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::InitializeFragmentation(*this, clusterList,
          originalClustersListName, fragmentClustersListName));

      TrackToClusterMap trackToClusterMap;
      TrackToHelixMap trackToHelixMap;

      for (pandora::TrackList::const_iterator trackIter = trackList.begin(), trackIterEnd = trackList.end() ;
          trackIter != trackIterEnd; ++trackIter)
      {
        const pandora::Track *const pTrack = *trackIter;
        const pandora::Helix helix(pTrack->GetTrackStateAtCalorimeter().GetPosition(), pTrack->GetTrackStateAtCalorimeter().GetMomentum(), pTrack->GetCharge(), bField);

        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::RemoveTrackClusterAssociation(*this, pTrack, pOriginalCluster));

        const pandora::Cluster *pCluster = NULL;
        PandoraContentApi::Cluster::Parameters parameters;
        parameters.m_pTrack = pTrack;
        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, parameters, pCluster));

        if (!trackToClusterMap.insert(TrackToClusterMap::value_type(pTrack, pCluster)).second ||
            !trackToHelixMap.insert(TrackToHelixMap::value_type(pTrack, helix)).second)
        {
          return pandora::STATUS_CODE_FAILURE;
        }
      }

      // Assign the calo hits in the original cluster to the most appropriate track
      for (pandora::OrderedCaloHitList::const_iterator listIter = orderedCaloHitList.begin(), listIterEnd = orderedCaloHitList.end();
          listIter != listIterEnd; ++listIter)
      {
        for (pandora::CaloHitList::const_iterator hitIter = listIter->second->begin(), hitIterEnd = listIter->second->end();
            hitIter != hitIterEnd; ++hitIter)
        {
          const pandora::CaloHit *const pCaloHit = *hitIter;
          const pandora::CartesianVector &hitPosition(pCaloHit->GetPositionVector());

          // Identify most suitable cluster for calo hit, using distance to helix fit as figure of merit
          const pandora::Cluster *pBestCluster = NULL;
          float minDistanceToTrack(std::numeric_limits<float>::max());

          for (TrackToClusterMap::const_iterator mapIter = trackToClusterMap.begin(), mapIterEnd = trackToClusterMap.end();
              mapIter != mapIterEnd; ++mapIter)
          {
            const pandora::Track *const pTrack(mapIter->first);
            const pandora::Cluster *const pCluster(mapIter->second);

            TrackToHelixMap::const_iterator helixIter = trackToHelixMap.find(pTrack);

            if (trackToHelixMap.end() == helixIter)
              return pandora::STATUS_CODE_FAILURE;

            const pandora::Helix &helix(helixIter->second);

            float distanceToTrack(std::numeric_limits<float>::max());
            PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, GeometryHelper::GetDistanceToHelix(helix, hitPosition, distanceToTrack));

            if (distanceToTrack < minDistanceToTrack)
            {
              minDistanceToTrack = distanceToTrack;
              pBestCluster = pCluster;
            }
          }

          // should never happen
          if (NULL == pBestCluster)
            return pandora::STATUS_CODE_FAILURE;

          PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddToCluster(*this, pBestCluster, pCaloHit));
        }
      }

      // Check for any "empty" clusters and create new track-cluster associations
      for (TrackToClusterMap::iterator mapIter = trackToClusterMap.begin(); mapIter != trackToClusterMap.end();)
      {
        const pandora::Cluster *const pCluster = mapIter->second;

        if (0 == pCluster->GetNCaloHits())
        {
          PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::Delete(*this, pCluster));
          trackToClusterMap.erase(mapIter++);
        }
        else
        {
          PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddTrackClusterAssociation(*this, mapIter->first, pCluster));
          ++mapIter;
        }
      }

      if (trackToClusterMap.empty())
      {
        return pandora::STATUS_CODE_FAILURE;
      }

      // End cluster fragmentation operations, automatically choose the new cluster fragments
      PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::EndFragmentation(*this, fragmentClustersListName,
          originalClustersListName));
    }

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode SplitTrackReclusteringAlgorithm::ReadSettings(const pandora::TiXmlHandle xmlHandle)
  {
    m_minNTrackAssociation = 2;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "MinNTrackAssociation", m_minNTrackAssociation));

    if(m_minNTrackAssociation < 2)
      return pandora::STATUS_CODE_INVALID_PARAMETER;

    return pandora::STATUS_CODE_SUCCESS;
  }

}

