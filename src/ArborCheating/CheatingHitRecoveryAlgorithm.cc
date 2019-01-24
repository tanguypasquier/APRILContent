/**
 *  @file   CheatingHitRecoveryAlgorithm.cc
 * 
 *  @brief  Implementation of the cheating cluster cleaning algorithm class
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "ArborCheating/CheatingHitRecoveryAlgorithm.h"
#include "ArborHelpers/ClusterHelper.h"
#include "ArborHelpers/CaloHitRangeSearchHelper.h"
#include "ArborHelpers/CaloHitNeighborSearchHelper.h"
#include "ArborHelpers/HistogramHelper.h"
#include "ArborUtility/EventPreparationAlgorithm.h"
#include "ArborApi/ArborContentApi.h"

namespace arbor_content
{

pandora::StatusCode CheatingHitRecoveryAlgorithm::Run()
{
	if(m_shouldUseMCRecovery || m_shouldUseRecovery)
	{
		if(m_shouldUseMCRecovery)
		{
			MCPCaloHitListMap mcpCaloHitListMap;
			MakeMCPHitsAssociation(mcpCaloHitListMap);
			
			MCPClusterListMap mcpClusterListMap;
			MakeMCPClustersAssociation(mcpClusterListMap);
			
			AddHitToClusterByMCP(mcpClusterListMap, mcpCaloHitListMap);
			AddHitsToNewClusters(mcpCaloHitListMap);
		}

		if(!m_shouldUseMCRecovery && m_shouldUseRecovery)
		{
			ClusterCaloHitListMap clusterCaloHitListMap;
			MakeClusterHitsAssociation(clusterCaloHitListMap);
			AddHitToCluster(clusterCaloHitListMap);
		}
	}

	if(m_shouldUseMCMerge)
	{
		MergeClusters();
	}
	
    return pandora::STATUS_CODE_SUCCESS;
}


pandora::StatusCode CheatingHitRecoveryAlgorithm::MakeMCPHitsAssociation(MCPCaloHitListMap& mcpCaloHitListMap)
{
    const pandora::CaloHitList *pCaloHitList = nullptr; 
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pCaloHitList));
	//std::cout << "------- # CaloHit : " << pCaloHitList->size() << std::endl;

    for(pandora::CaloHitList::const_iterator iter = pCaloHitList->begin(); iter != pCaloHitList->end(); ++iter)
	{
		const pandora::CaloHit* const pCaloHit = *iter;

        if (PandoraContentApi::IsAvailable(*this, pCaloHit))
		{
		   const pandora::MCParticle* pMCHitParticle  = nullptr;

           try
           {
           	 pMCHitParticle = pandora::MCParticleHelper::GetMainMCParticle(pCaloHit);
           	//std::cout << "calo hit: " << caloHit << ", mcp: " << pMCHitParticle << std::endl;
           }
           catch (pandora::StatusCodeException &)
           {
		       continue;
           }

		   if(pMCHitParticle != nullptr && mcpCaloHitListMap.find( pMCHitParticle ) == mcpCaloHitListMap.end())
		   {
		       pandora::CaloHitList hitList;
		       hitList.push_back( pCaloHit );
		       mcpCaloHitListMap[pMCHitParticle] = hitList;
		   }
		   else
		   {
		       mcpCaloHitListMap[pMCHitParticle].push_back( pCaloHit );
		   }
		}
	}

    return pandora::STATUS_CODE_SUCCESS;
}

pandora::StatusCode CheatingHitRecoveryAlgorithm::MakeMCPClustersAssociation(MCPClusterListMap& mcpClusterListMap)
{
	// make mcp and exsiting cluster relationship
    const pandora::ClusterList* pClusterList = nullptr; 
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

	//std::cout << "orginal cluster: " << pClusterList->size() << std::endl;

	for(auto it = pClusterList->begin(); it != pClusterList->end(); ++it)
	{
		const pandora::Cluster* const clu = *it;

		const pandora::MCParticle* pClusterMCParticle  = nullptr;

        try
        {
        	 pClusterMCParticle = pandora::MCParticleHelper::GetMainMCParticle( clu );
        }
        catch (pandora::StatusCodeException &)
        {
		    continue;
		}

		if(pClusterMCParticle != nullptr && mcpClusterListMap.find( pClusterMCParticle ) == mcpClusterListMap.end())
		{
		    pandora::ClusterList clusterList;
		    clusterList.push_back( clu );
		    mcpClusterListMap[pClusterMCParticle] = clusterList;
		}
		else
		{
		    mcpClusterListMap[pClusterMCParticle].push_back( clu );
		}
	}

	//std::cout << "CheatingHitRecovery, mcpClusterListMap size: " << mcpClusterListMap.size() << std::endl;
    return pandora::STATUS_CODE_SUCCESS;
}


pandora::StatusCode CheatingHitRecoveryAlgorithm::AddHitToClusterByMCP(MCPClusterListMap& mcpClusterListMap, MCPCaloHitListMap& mcpCaloHitListMap)
{
	// add the unused hit to exsiting clusters
	for(auto mcpIt = mcpClusterListMap.begin(); mcpIt != mcpClusterListMap.end(); ++mcpIt)
	{
		auto mcp = mcpIt->first;
		auto clusterList = mcpIt->second;

		pandora::CaloHitList& hitList = mcpCaloHitListMap[mcp];

		if(hitList.empty() || clusterList.empty()) continue;

		// just a check
		//if(!mcp->IsPfoTarget()) continue;
        
		// charged particle
    	//int mcpCharge = pandora::PdgTable::GetParticleCharge(mcp->GetParticleId());
	    //if(mcpCharge != 0) continue;

        // simply add the hits to the first cluster of the mcp
		auto pCluster = *(clusterList.begin());

		pandora::CaloHitList hitsAddToCluster;

		for(auto caloHitIter = hitList.begin(); caloHitIter != hitList.end(); ++caloHitIter)
		{
			auto pCaloHit = *caloHitIter;
			const pandora::CartesianVector hitPos = pCaloHit->GetPositionVector();
			float hitClusterDistance;
			ClusterHelper::GetClosestDistanceApproach(pCluster, hitPos, hitClusterDistance);

			//std::cout << " === hit pos: " << hitPos.GetX() << ", " << hitPos.GetY() << ", " << hitPos.GetZ() << "  - cluster: " 
			//	<< pCluster->GetHadronicEnergy() << " - distance: " << hitClusterDistance << std::endl;

			// charged hit with distance less than a specified value is not included
			//if( !(hitClusterDistance < 400 && mcpCharge != 0) )
			if(1)
			//if(hitClusterDistance < 400)
			//if( mcpCharge != 0 )
			{
			   hitsAddToCluster.push_back(pCaloHit);

		       ///////////////////////////////
	           std::vector<float> vars;
	           vars.push_back( float(EventPreparationAlgorithm::GetEventNumber()) );
	           vars.push_back( hitClusterDistance );
	           vars.push_back( float(pCaloHit->GetElectromagneticEnergy()) );
	           vars.push_back( float(pCaloHit->GetHadronicEnergy()) );
	           vars.push_back( float(pCluster->GetElectromagneticEnergy()) );
	           vars.push_back( float(pCluster->GetHadronicEnergy()) );

		       HistogramManager::CreateFill("AddHitToCluster", 
			   "evtNumber:hitClusterDistance:hitEMEnergy:hitHadEnergy:clusterEMEnergy:clusterHadEnergy", vars);
			}
		}

        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ArborContentApi::AddToCluster(*this, pCluster, &hitsAddToCluster));

		hitList.clear();
	}

    return pandora::STATUS_CODE_SUCCESS;
}

pandora::StatusCode CheatingHitRecoveryAlgorithm::AddHitsToNewClusters(MCPCaloHitListMap& mcpCaloHitListMap)
{
	// if no cluster to add hit, create new cluster for neutral one
    const pandora::ClusterList *pNewClusterList = nullptr; 
	std::string clusterListName;

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, 
	PandoraContentApi::CreateTemporaryListAndSetCurrent(*this, pNewClusterList, clusterListName));

	for(auto mcpIt = mcpCaloHitListMap.begin(); mcpIt != mcpCaloHitListMap.end(); ++mcpIt)
	{
		//auto mcp = mcpIt->first;
		auto hitList = mcpIt->second;
        
		if(hitList.empty()) continue;

        PandoraContentApi::Cluster::Parameters parameters;
		parameters.m_caloHitList = hitList;

		const pandora::Cluster *pCluster = nullptr;
        PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ArborContentApi::Create(*this, parameters, pCluster));
	}

	//std::cout << "newly created cluster: " << pNewClusterList->size() << std::endl;
    
	// merge the new clusters into the exsiting cluster list
    // Save the merged list and set it to be the current list for future algorithms
	pandora::ClusterList clustersToSave;

    const pandora::ClusterList *pClusterListToSave = nullptr; 
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterListToSave));

	//std::cout << "temp cluster to save: " << pClusterListToSave->size() << std::endl;

	if (pClusterListToSave->empty()) return pandora::STATUS_CODE_SUCCESS;

	for(auto it = pClusterListToSave->begin(); it != pClusterListToSave->end(); ++it)
	{
		auto clu = *it;

        try
        {
        	auto pClusterMCParticle = pandora::MCParticleHelper::GetMainMCParticle( clu );

			//if( pClusterMCParticle->GetCharge() != 0 ) continue;
		    // just a check
			if( pClusterMCParticle->IsPfoTarget() == false) continue;
        }
        catch (pandora::StatusCodeException &)
        {
		    continue;
        }

		clustersToSave.push_back(*it);
	}
	
	// save clusters to the exsiting cluster list
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_INITIALIZED, !=, 
			PandoraContentApi::SaveList(*this, m_mergedClusterListName, clustersToSave));

    return pandora::STATUS_CODE_SUCCESS;
}

pandora::StatusCode CheatingHitRecoveryAlgorithm::MakeClusterHitsAssociation(ClusterCaloHitListMap& clusterCaloHitListMap)
{
    const pandora::CaloHitList *pCaloHitList = nullptr; 
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pCaloHitList));

    for(pandora::CaloHitList::const_iterator iter = pCaloHitList->begin(); iter != pCaloHitList->end(); ++iter)
	{
		const pandora::CaloHit* const pCaloHit = *iter;

        if (PandoraContentApi::IsAvailable(*this, pCaloHit))
		{
		   const pandora::MCParticle* pMCHitParticle  = nullptr;

		   pandora::CartesianVector testPosition = pCaloHit->GetPositionVector();
		   
		   // FIXME
		   int nNeighbor = 10;
		   pandora::CaloHitList neighborHits;

		   CaloHitNeighborSearchHelper::SearchNeighbourHits(testPosition, nNeighbor, neighborHits);

		   //std::cout << " calo hit is not clustered: " << pCaloHit << ", X = " << testPosition.GetX() << ", " << testPosition.GetY() 
		   //	<< ", " << testPosition.GetZ() << std::endl;

		   const pandora::Cluster* clusterToAdd = nullptr;
		   float hitsDistance = 0.;

		   for(auto& caloHit : neighborHits)
		   {
			   auto& hitPos = caloHit->GetPositionVector();
               const arbor_content::CaloHit *const pArborCaloHit = dynamic_cast<const arbor_content::CaloHit *const>(caloHit);
			   //std::cout << "     the nearby hit distance: " << (hitPos - testPosition).GetMagnitude() 
				 //  << ", pos: " << hitPos.GetX() << ", " << hitPos.GetY() << ", " << hitPos.GetZ() 
				   //<< ", cluster: " << pArborCaloHit->GetMother() << std::endl;

			   if(pArborCaloHit != nullptr && clusterToAdd == nullptr)
			   {
				   clusterToAdd = pArborCaloHit->GetMother();
				   hitsDistance = (hitPos - testPosition).GetMagnitude();
			   }
		   }

           try
           {
           	 pMCHitParticle = pandora::MCParticleHelper::GetMainMCParticle(pCaloHit);
           	//std::cout << "calo hit: " << caloHit << ", mcp: " << pMCHitParticle << std::endl;
           }
           catch (pandora::StatusCodeException &)
           {
		       continue;
           }

		   if(clusterToAdd != nullptr)
		   {
		      const pandora::MCParticle* pClusterMCParticle  = nullptr;

              try
              {
              	 pClusterMCParticle = pandora::MCParticleHelper::GetMainMCParticle(clusterToAdd);
              }
              catch (pandora::StatusCodeException &)
              {
		          continue;
		      }
		      //////
		   	
		      //std::cout << "  ====== hit: " << pCaloHit << ", mcp: " << pMCHitParticle 
		   	  // << ", the cluster which may merge the hit: " << clusterToAdd << ", mcp :" << pClusterMCParticle << endl;

		      if(clusterCaloHitListMap.find( clusterToAdd ) == clusterCaloHitListMap.end())
		      {
		          pandora::CaloHitList hitList;
		          hitList.push_back( pCaloHit );
		          clusterCaloHitListMap[clusterToAdd] = hitList;
		      }
		      else
		      {
		          clusterCaloHitListMap[clusterToAdd].push_back( pCaloHit );
		      }
    	
			  int hitMCPCharge = pandora::PdgTable::GetParticleCharge(pMCHitParticle->GetParticleId());
			  int clusterMCPCharge = pandora::PdgTable::GetParticleCharge(pClusterMCParticle->GetParticleId());

	          std::vector<float> vars;
	          vars.push_back( float(EventPreparationAlgorithm::GetEventNumber()) );
	          vars.push_back( hitsDistance );
	          vars.push_back( float(pMCHitParticle == pClusterMCParticle) );
	          vars.push_back( float(hitMCPCharge) );
	          vars.push_back( float(clusterMCPCharge) );

		      HistogramManager::CreateFill("TestAddHitToCluster", 
			  "evtNumber:hitsDistance:isRight:hitMCPCharge:clusterMCPCharge", vars);
		   }
		}
	}

    return pandora::STATUS_CODE_SUCCESS;
}

pandora::StatusCode CheatingHitRecoveryAlgorithm::AddHitToCluster(ClusterCaloHitListMap& clusterCaloHitListMap)
{
	// add the unused hit to exsiting clusters (not by MCP)
	for(auto clusterIt = clusterCaloHitListMap.begin(); clusterIt != clusterCaloHitListMap.end(); ++clusterIt)
	{
		const auto& cluster = clusterIt->first;
		const auto& caloHitList = clusterIt->second;

		if(caloHitList.empty()) continue;

		const pandora::CaloHitList& hitsAddToCluster = caloHitList;

        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ArborContentApi::AddToCluster(*this, cluster, &hitsAddToCluster));
	}

    return pandora::STATUS_CODE_SUCCESS;
}
	

pandora::StatusCode CheatingHitRecoveryAlgorithm::MergeClusters()
{
	////////////////////////////////////////////////////////////////////////////////
	// check the cluster and its mcp:
	// if one mcp has several clusters, then they should be merged into one cluster
	// it is especially mandatory for charged clusters
	////////////////////////////////////////////////////////////////////////////////
	
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=,
			PandoraContentApi::ReplaceCurrentList<pandora::Cluster>(*this, m_mergedClusterListName));

    const pandora::ClusterList* pClusterList = nullptr; 
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));
	//std::cout << "cluster after saving: " << pClusterList->size() << std::endl;

	std::map<const pandora::MCParticle* const, pandora::ClusterList> mcpClusterListMap;
	
	for(auto it = pClusterList->begin(); it != pClusterList->end(); ++it)
	{
		const pandora::Cluster* const clu = *it;

		const pandora::MCParticle* pClusterMCParticle  = nullptr;

        try
        {
        	 pClusterMCParticle = pandora::MCParticleHelper::GetMainMCParticle( clu );
        }
        catch (pandora::StatusCodeException &)
        {
		    continue;
		}

		if(pClusterMCParticle != nullptr && mcpClusterListMap.find( pClusterMCParticle ) == mcpClusterListMap.end())
		{
		    pandora::ClusterList clusterList;
		    clusterList.push_back( clu );
		    mcpClusterListMap[pClusterMCParticle] = clusterList;
		}
		else
		{
		    mcpClusterListMap[pClusterMCParticle].push_back( clu );
		}
	}

	for(auto it = mcpClusterListMap.begin(); it != mcpClusterListMap.end(); ++it)
	{
		auto clusterList = it->second;

	    if(clusterList.size()>1) 
		{
		    //auto mcp = it->first;
			//std::cout << "------>>> particle cluster # greater than 1: " << clusterList.size() << ", PDG: "
			//	      << mcp->GetParticleId() << std::endl;

			// merge clusters
			auto firstCluster = *( clusterList.begin() );

			for(auto cluIt = clusterList.begin(); cluIt != clusterList.end(); ++cluIt)
			{
				if(cluIt == clusterList.begin()) continue;

		        const pandora::Cluster* cluToMerge = *cluIt;
				ArborContentApi::MergeAndDeleteClusters(*this, firstCluster, cluToMerge);
			}

		}// end if
	} // end for

    return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode CheatingHitRecoveryAlgorithm::ReadSettings(const pandora::TiXmlHandle xmlHandle)
{
    m_shouldUseRecovery = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "ShouldUseRecovery", m_shouldUseRecovery));

    m_shouldUseMCRecovery = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "ShouldUseMCRecovery", m_shouldUseMCRecovery));

    m_shouldUseMCMerge = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "ShouldUseMCMerge", m_shouldUseMCMerge));

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "ClusterListToTakeNewClusters", m_mergedClusterListName));

    return pandora::STATUS_CODE_SUCCESS;
}

} // namespace arbor_content
