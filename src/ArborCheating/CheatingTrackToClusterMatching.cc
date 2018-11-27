/**
 *  @file   ArborContent/src/ArborCheating/CheatingTrackToClusterMatching.cc
 * 
 *  @brief  Implementation of the cheating track to cluster matching algorithm class
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "ArborCheating/CheatingTrackToClusterMatching.h"

using namespace pandora;

namespace arbor_content
{

  StatusCode CheatingTrackToClusterMatching::Run()
  {

	std::cout << "CheatingTrackToCluster......" << std::endl;
    // Read current lists
    const TrackList *pCurrentTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pCurrentTrackList));

    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

    // Clear any existing track - cluster associations
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RemoveCurrentTrackClusterAssociations(*this));

    // Construct a map from mc particle to tracks
    typedef std::map<const MCParticle*, TrackList> TracksPerMCParticle;
    TracksPerMCParticle tracksPerMCParticle;
    TracksPerMCParticle tracksPfoTarget;

    for (TrackList::const_iterator iter = pCurrentTrackList->begin(), iterEnd = pCurrentTrackList->end(); iter != iterEnd; ++iter)
    {
      try
      {
        const Track *const pTrack = *iter;
        //const MCParticle *const pMCParticle(pTrack->GetMainMCParticle());
		const MCParticle *const pMCParticle(MCParticleHelper::GetMainMCParticle(pTrack));
		const MCParticle *const pPfoTarget(pMCParticle->GetPfoTarget());

        TracksPerMCParticle::iterator itTracksPerMCParticle(tracksPerMCParticle.find(pMCParticle));

        if (tracksPerMCParticle.end() == itTracksPerMCParticle)
        {
          TrackList trackList;
          trackList.push_back(pTrack);

		  //FIME:: check existence
		  if(!tracksPerMCParticle.insert(TracksPerMCParticle::value_type(pMCParticle, TrackList(1, pTrack))).second)
            throw StatusCodeException(STATUS_CODE_FAILURE);
          //if (!tracksPerMCParticle.push_back(TracksPerMCParticle::value_type(pMCParticle, trackList)).second)
        }
        else
        {
          itTracksPerMCParticle->second.push_back(pTrack);
        }

		//////////////////
        TracksPerMCParticle::iterator itTrackPfoParget(tracksPfoTarget.find(pPfoTarget));

        if (tracksPfoTarget.end() == itTrackPfoParget)
        {
          TrackList trackList;
          trackList.push_back(pTrack);

		  if(!tracksPfoTarget.insert(TracksPerMCParticle::value_type(pPfoTarget, TrackList(1, pTrack))).second)
            throw StatusCodeException(STATUS_CODE_FAILURE);
          //if (!tracksPerMCParticle.push_back(TracksPerMCParticle::value_type(pMCParticle, trackList)).second)
        }
        else
        {
          itTrackPfoParget->second.push_back(pTrack);
        }

      }
      catch (StatusCodeException &)
      {
      }
    }

    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
      try
      {
        const Cluster *const pCluster = *iter;
        const MCParticle *const pMCParticle(MCParticleHelper::GetMainMCParticle(pCluster));
        const MCParticle *const pClusterPfoTarget(pMCParticle->GetPfoTarget());

		auto foundMCPTracks = tracksPerMCParticle.find(pMCParticle);

		if(foundMCPTracks != tracksPerMCParticle.end() && PandoraContentApi::IsAvailable(*this, pCluster))
		{
			const TrackList& tracks = foundMCPTracks->second;

			for(auto trackIter = tracks.begin(); trackIter != tracks.end(); ++trackIter)
			{
				auto track = *trackIter;
				if( !PandoraContentApi::IsAvailable(*this, track) ) continue;
				PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddTrackClusterAssociation(*this, track, pCluster));
			}
		}
		else
		{
			// if find no matching track-cluster by direct MCP, then use pfo target
			auto foundMCPTracksByPfo = tracksPfoTarget.find( pClusterPfoTarget );

			if( foundMCPTracksByPfo != tracksPfoTarget.end() && PandoraContentApi::IsAvailable(*this, pCluster) )
			{
				const TrackList& tracks = foundMCPTracksByPfo->second;
				for(auto trackIter = tracks.begin(); trackIter != tracks.end(); ++trackIter)
				{
					auto track = *trackIter;
					if( !PandoraContentApi::IsAvailable(*this, track) ) continue;
					PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddTrackClusterAssociation(*this, track, pCluster));
				}
			}
		}
	  }
      catch (StatusCodeException &)
	  {
	  }
	}

#if 0
    // Construct a map from mc particle to clusters
    typedef std::map<const MCParticle*, ClusterList> ClustersPerMCParticle;
    ClustersPerMCParticle clustersPerMCParticle;

    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
      try
      {
        const Cluster *const pCluster = *iter;
        const MCParticle *const pMCParticle(MCParticleHelper::GetMainMCParticle(pCluster));

        ClustersPerMCParticle::iterator itClustersPerMCParticle(clustersPerMCParticle.find(pMCParticle));

        if (clustersPerMCParticle.end() == itClustersPerMCParticle)
        {
          ClusterList clusterList;
          clusterList.push_back(pCluster);

		  //FIXME
		  if(!clustersPerMCParticle.insert(ClustersPerMCParticle::value_type(pMCParticle, ClusterList(1, pCluster))).second)
            throw StatusCodeException(STATUS_CODE_FAILURE);
          //if (!clustersPerMCParticle.push_back(ClustersPerMCParticle::value_type(pMCParticle, clusterList)).second)
        }
        else
        {
          itClustersPerMCParticle->second.push_back(pCluster);
        }
      }
      catch (StatusCodeException &)
      {
      }
    }

    //std::cout << "N mc tracks   = " << tracksPerMCParticle.size() << std::endl;
    //std::cout << "N mc clusters = " << clustersPerMCParticle.size() << std::endl;

	// merge clusters for one MC particle
#if 1
    for (ClustersPerMCParticle::iterator iter = clustersPerMCParticle.begin(), iterEnd = clustersPerMCParticle.end(); iter != iterEnd; ++iter)
	{
      //const MCParticle *const pMCParticle = iter->first;
      ClusterList &clusterList = iter->second;

	  if(clusterList.size()<=1) continue;

	  const pandora::Cluster *const firstCluster = *(clusterList.begin());

	  ClusterList::const_iterator itCluster = clusterList.begin();
	  ++ itCluster;

      for (ClusterList::const_iterator itClusterEnd = clusterList.end(); itCluster != itClusterEnd; ++itCluster)
	  {
		  const pandora::Cluster* cluster = *itCluster;
		  PandoraContentApi::MergeAndDeleteClusters(*this, firstCluster, cluster);
		  //std::cout << "----> merged two clusters: " << firstCluster << " <--- " << cluster << std::endl;
	  }

	  clusterList.clear();
	  clusterList.push_back(firstCluster);
	}
#endif

	std::set<const Cluster*> matchedClusters;

    // Make the track to cluster associations
    for (TracksPerMCParticle::const_iterator iter = tracksPerMCParticle.begin(), iterEnd = tracksPerMCParticle.end(); iter != iterEnd; ++iter)
    {
      const MCParticle *const pMCParticle = iter->first;
      const TrackList &trackList = iter->second;

      ClustersPerMCParticle::const_iterator itClustersPerMCParticle(clustersPerMCParticle.find(pMCParticle));

      if (clustersPerMCParticle.end() == itClustersPerMCParticle)
        continue;

      const ClusterList &clusterList = itClustersPerMCParticle->second;

      if (trackList.empty() || clusterList.empty())
        continue;

	  //std::cout << "---- MCP energy: " << pMCParticle->GetEnergy() << ", track list size: " << trackList.size() << std::endl;

      for (TrackList::const_iterator itTrack = trackList.begin(), itTrackEnd = trackList.end(); itTrack != itTrackEnd; ++itTrack)
      {
        // If the mc particle is associated with multiple clusters, can only associate to highest energy cluster (clusters should be merged)
		//std::cout << " ----- track energy: " << (*itTrack)->GetEnergyAtDca() << std::endl;
        const Cluster *pHighestEnergyCluster = NULL;
        float highestEnergy(-std::numeric_limits<float>::max());

        for (ClusterList::const_iterator itCluster = clusterList.begin(), itClusterEnd = clusterList.end(); itCluster != itClusterEnd; ++itCluster)
        {
          const float clusterEnergy((*itCluster)->GetHadronicEnergy());
		  //std::cout << " -------- cluster energy: " << clusterEnergy << std::endl;

          if (clusterEnergy > highestEnergy)
          {
            highestEnergy = clusterEnergy;
            pHighestEnergyCluster = *itCluster;
          }
        }

        if (NULL == pHighestEnergyCluster)
          throw StatusCodeException(STATUS_CODE_FAILURE);

		if(matchedClusters.end() != matchedClusters.find(pHighestEnergyCluster)) 
		{
			//std::cout << " ------------- cluster : " << pHighestEnergyCluster << ", has used already." << std::endl; 
			continue;
		}

		//std::cout << " --->>>> add association, track: " << *itTrack << " -> cluster: " << pHighestEnergyCluster << std::endl;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddTrackClusterAssociation(*this, *itTrack, pHighestEnergyCluster));

		matchedClusters.insert(pHighestEnergyCluster);

		//std::cout << "----> added relation: " << *itTrack << ", " << pHighestEnergyCluster << std::endl;
      }
    }

	// check the association
    for (TrackList::const_iterator iter = pCurrentTrackList->begin(), iterEnd = pCurrentTrackList->end(); iter != iterEnd; ++iter)
	{
        const Track *const pTrack = *iter;
		bool hasCluster = pTrack->HasAssociatedCluster();

		if(hasCluster) 
		{
		    std::cout << " ------ Track: " << pTrack << ", energy: " << pTrack->GetEnergyAtDca() 
			          << ", has cluster: " << pTrack->GetAssociatedCluster() << std::endl;
		}
		else
		{
			std::cout << " ------ Track: " << pTrack << " has no cluster" << std::endl;
		}
	}

    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
	{
		const Cluster* const pCluster = *iter;

		std::cout << " ------ Cluster's tracks: " << pCluster->GetAssociatedTrackList().size() << std::endl;
	}
#endif
	

    return STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  StatusCode CheatingTrackToClusterMatching::ReadSettings(const TiXmlHandle /*xmlHandle*/)
  {
    return STATUS_CODE_SUCCESS;
  }

} // namespace arbor_content
