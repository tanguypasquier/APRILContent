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

  bool CheatingTrackToClusterMatching::IsParent(const pandora::MCParticle* parent, const pandora::MCParticle* daughter)
  {
	  if(daughter == parent) return true;

	  if(daughter->IsPfoTarget()) return false;

	  auto parentList = daughter->GetParentList();

	  bool foundParentMCP = false;

	  if( std::find(parentList.begin(), parentList.end(), parent) != parentList.end() )
	  {
		  foundParentMCP = true;
	  }
	  else
	  {
		  for(auto mcp : parentList)
		  {
			  if (IsParent(parent, mcp) == true)
			  {
				  foundParentMCP = true;
				  break;
			  }
		  }

	  }
	
	  return foundParentMCP;
  }

  StatusCode CheatingTrackToClusterMatching::Run()
  {
	//std::cout << "CheatingTrackToCluster......" << std::endl;
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

	////////////////////////////////////////// matching track-cluster by direct MCP
    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
      try
      {
        const Cluster *const pCluster = *iter;
        const MCParticle *const pMCParticle(MCParticleHelper::GetMainMCParticle(pCluster));
        //const MCParticle *const pClusterPfoTarget(pMCParticle->GetPfoTarget());

		auto foundMCPTracks = tracksPerMCParticle.find(pMCParticle);

		if(foundMCPTracks != tracksPerMCParticle.end() && PandoraContentApi::IsAvailable(*this, pCluster))
		{
			const TrackList& tracks = foundMCPTracks->second;

			for(auto trackIter = tracks.begin(); trackIter != tracks.end(); ++trackIter)
			{
				auto track = *trackIter;

				if(track->HasAssociatedCluster())
				{
					//std::cout << __LINE__ << ": the track has AssociatedCluster !!! Track energy: " << track->GetEnergyAtDca() 
					//	      << ", cluster energy to match: " << pCluster->GetHadronicEnergy() <<  ", AssociatedCluster energy: " <<
					//track->GetAssociatedCluster()->GetHadronicEnergy() << std::endl;

					continue;
				}

				if( !PandoraContentApi::IsAvailable(*this, track) || !PandoraContentApi::IsAvailable(*this, pCluster) ) continue;
				PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddTrackClusterAssociation(*this, track, pCluster));
			}
		}
	  }
      catch (StatusCodeException &)
	  {
	  }
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// If find no matching track-cluster by direct MCP, use pfo target: if the track is parent of
	// the cluster, it means they can be matched
	// 1) if track has no associated cluster, just macth the track and cluster
	// 2) otherwise record the cluster to merge
	// 3) finally merge all clusters in the record
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
      try
      {
		//std::cout << "haha" << std::endl;
        const Cluster *const pCluster = *iter;
	    //std::cout << pCluster << std::endl;
        const MCParticle *const pMCParticle(MCParticleHelper::GetMainMCParticle(pCluster));
        const MCParticle *const pClusterPfoTarget(pMCParticle->GetPfoTarget());

		auto foundMCPTracksByPfo = tracksPfoTarget.find( pClusterPfoTarget );

		if( foundMCPTracksByPfo != tracksPfoTarget.end() && PandoraContentApi::IsAvailable(*this, pCluster) )
		{
			const TrackList& tracks = foundMCPTracksByPfo->second;
			for(auto trackIter = tracks.begin(); trackIter != tracks.end(); ++trackIter)
			{
				auto track = *trackIter;
				if( !PandoraContentApi::IsAvailable(*this, track) || !PandoraContentApi::IsAvailable(*this, pCluster) ) continue;

				// if track is parent of cluster, add the cluster
				bool isParent = false;

				try
				{
                    const MCParticle *const trackMCP(MCParticleHelper::GetMainMCParticle(track));
                    const MCParticle *const cluMCP(MCParticleHelper::GetMainMCParticle(pCluster));
				    isParent = IsParent(trackMCP, cluMCP); // recursive function
				}
                catch (StatusCodeException &)
				{
					//std::cout << "============= mcp problem ...." << std::endl;
					continue;
				}

				if(isParent)
				{
				    // if track has associated cluster, merge the clusters
				    if(! track->HasAssociatedCluster() ) 
					{
				        // add association for the track and cluster
						//std::cout << "match t-c : " << track << " --- " << pCluster << std::endl;
						//std::cout << "track energy: " << track->GetMomentumAtDca().GetMagnitude() << ", cluster energy: "
						//	      << pCluster->GetHadronicEnergy() << std::endl;
						// make a map
				        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddTrackClusterAssociation(*this, track, pCluster));
					}

				    // if the cluster is associated to track, stop the loop
				    break;
				}
			} // for each track
		}
	  }
      catch (StatusCodeException &)
	  {
	  }
	}


	//std::cout << "------------------" << std::endl;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::map<const pandora::Cluster*, pandora::ClusterList> clustersToMerge;
	
    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
      try
      {
        const Cluster *const pCluster = *iter;
        const MCParticle *const pMCParticle(MCParticleHelper::GetMainMCParticle(pCluster));
        const MCParticle *const pClusterPfoTarget(pMCParticle->GetPfoTarget());

		auto foundMCPTracksByPfo = tracksPfoTarget.find( pClusterPfoTarget );

		if( foundMCPTracksByPfo != tracksPfoTarget.end() && PandoraContentApi::IsAvailable(*this, pCluster) )
		{
			const TrackList& tracks = foundMCPTracksByPfo->second;
			for(auto trackIter = tracks.begin(); trackIter != tracks.end(); ++trackIter)
			{
				auto track = *trackIter;
				if( !PandoraContentApi::IsAvailable(*this, track) || !PandoraContentApi::IsAvailable(*this, pCluster) ) continue;

				// if track is parent of cluster, add the cluster
				bool isParent = false;

				try
				{
                    const MCParticle *const trackMCP(MCParticleHelper::GetMainMCParticle(track));
                    const MCParticle *const cluMCP(MCParticleHelper::GetMainMCParticle(pCluster));
				    isParent = IsParent(trackMCP, cluMCP); // recursive function
				}
                catch (StatusCodeException &)
				{
					//std::cout << "============= mcp problem ...." << std::endl;
					continue;
				}

				if(isParent)
				{
				    // if track has associated cluster, merge the clusters
				    if(track->HasAssociatedCluster() ) 
				    {
				        //std::cout << __LINE__ << ": the track has AssociatedCluster !!! Track energy: " << track->GetEnergyAtDca() 
				        //	      << ", cluster energy to match: " << pCluster->GetHadronicEnergy() <<  ", AssociatedCluster energy: " << 
				        //track->GetAssociatedCluster()->GetHadronicEnergy() << std::endl;
						if(track->GetAssociatedCluster() != pCluster)
						{
							//std::cout << " merge clusters: " << track->GetAssociatedCluster() << " --- " << pCluster << std::endl;
							//std::cout << " clusters energy: " << track->GetAssociatedCluster()->GetHadronicEnergy() << " --- " 
							//	      << pCluster->GetHadronicEnergy() << std::endl;

							const pandora::Cluster* mainCluster = track->GetAssociatedCluster();
							auto clusterToMerge = pCluster;

							if(clustersToMerge.find(mainCluster) == clustersToMerge.end())
							{
								pandora::ClusterList clusterList;
								clusterList.push_back( clusterToMerge );
								clustersToMerge[mainCluster] = clusterList;
							}
							else
							{
								clustersToMerge[mainCluster].push_back(clusterToMerge);
							}

						    // make a map
		                    //PandoraContentApi::MergeAndDeleteClusters(*this, track->GetAssociatedCluster(), pCluster);
						}
				    }

				    // if the cluster is associated to track, stop the loop
				    break;
				}
			} // for each track
		}
	  }
      catch (StatusCodeException &)
	  {
	  }
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	for(auto cluIter : clustersToMerge)
	{
		auto mainCluster = cluIter.first;
		auto clusters = cluIter.second;

		if(clusters.empty()) continue;

		for( auto iter : clusters)
		{
			auto clu = iter;
			//std::cout << "merge clusters: " << mainCluster << " --- " << clu << std::endl;
			//std::cout << "cluster energy: " << mainCluster->GetHadronicEnergy() << " --- " << clu->GetHadronicEnergy() << std::endl;
		    PandoraContentApi::MergeAndDeleteClusters(*this, mainCluster, clu);
		}
	}

    return STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  StatusCode CheatingTrackToClusterMatching::ReadSettings(const TiXmlHandle /*xmlHandle*/)
  {
    return STATUS_CODE_SUCCESS;
  }

} // namespace arbor_content
