/**
 *  @file   CheatingHitRecoveryAlgorithm.cc
 * 
 *  @brief  Implementation of the cheating cluster cleaning algorithm class
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "ArborCheating/CheatingHitRecoveryAlgorithm.h"

using namespace pandora;

namespace arbor_content
{

StatusCode CheatingHitRecoveryAlgorithm::Run()
{
    const pandora::CaloHitList *pCaloHitList = NULL; 
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pCaloHitList));
	std::cout << "------- # CaloHit : " << pCaloHitList->size() << std::endl;

	// make mcp and unused hits relationship
	std::map<const pandora::MCParticle* const, pandora::CaloHitList> mcpCaloHitListMap;

    for(pandora::CaloHitList::const_iterator iter = pCaloHitList->begin(); iter != pCaloHitList->end(); ++iter)
	{
		const pandora::CaloHit* const pCaloHit = *iter;

        if (PandoraContentApi::IsAvailable(*this, pCaloHit))
		{
		   const pandora::MCParticle* pMCHitParticle  = NULL;

           try
           {
           	 pMCHitParticle = pandora::MCParticleHelper::GetMainMCParticle(pCaloHit);
           	//std::cout << "calo hit: " << caloHit << ", mcp: " << pMCHitParticle << std::endl;
           }
           catch (pandora::StatusCodeException &)
           {
		       continue;
           }

		   if(pMCHitParticle != NULL && mcpCaloHitListMap.find( pMCHitParticle ) == mcpCaloHitListMap.end())
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

	// make mcp and cluster relationship
    const pandora::ClusterList* pClusterList = NULL; 
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

	std::cout << "orginal cluster: " << pClusterList->size() << std::endl;

	std::map<const pandora::MCParticle* const, pandora::ClusterList> mcpClusterListMap;
	
	for(auto it = pClusterList->begin(); it != pClusterList->end(); ++it)
	{
		const pandora::Cluster* const clu = *it;

		const pandora::MCParticle* pClusterMCParticle  = NULL;

        try
        {
        	 pClusterMCParticle = pandora::MCParticleHelper::GetMainMCParticle( clu );
        }
        catch (pandora::StatusCodeException &)
        {
		    continue;
		}

		if(pClusterMCParticle != NULL && mcpClusterListMap.find( pClusterMCParticle ) == mcpClusterListMap.end())
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

	std::cout << "CheatingHitRecovery, mcpClusterListMap size: " << mcpClusterListMap.size() << std::endl;

	// add the unused hit to exsiting clusters
	for(auto mcpIt = mcpClusterListMap.begin(); mcpIt != mcpClusterListMap.end(); ++mcpIt)
	{
		auto mcp = mcpIt->first;
		auto clusterList = mcpIt->second;

		pandora::CaloHitList& hitList = mcpCaloHitListMap[mcp];

		if(hitList.empty()) continue;
		if(clusterList.empty()) continue;

		// just a check
		if(!mcp->IsPfoTarget()) continue;

        // simply add the hits to the first cluster of the mcp
		auto pCluster = *(clusterList.begin());

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddToCluster(*this, pCluster, &hitList));

		hitList.clear();
	}
	
	// if no cluster to add hit, create new cluster for neutral one
    const ClusterList *pNewClusterList = NULL; 
	std::string clusterListName;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, 
	PandoraContentApi::CreateTemporaryListAndSetCurrent(*this, pNewClusterList, clusterListName));

	for(auto mcpIt = mcpCaloHitListMap.begin(); mcpIt != mcpCaloHitListMap.end(); ++mcpIt)
	{
		//auto mcp = mcpIt->first;
		auto hitList = mcpIt->second;
        
		if(hitList.empty()) continue;

        PandoraContentApi::Cluster::Parameters parameters;
        CaloHitList& caloHitList(parameters.m_caloHitList);
		caloHitList = hitList;

		const Cluster *pCluster = NULL;
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, parameters, pCluster));
	}

	std::cout << "newly created cluster: " << pNewClusterList->size() << std::endl;
    
	// merge the new clusters into the exsiting cluster list
    // Save the merged list and set it to be the current list for future algorithms
	pandora::ClusterList clustersToSave;

    const pandora::ClusterList *pClusterListToSave = NULL; 
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterListToSave));

	std::string mergedClusterListName("PfoCreation");

	std::cout << "temp cluster to save: " << pClusterListToSave->size() << std::endl;

	if (pClusterListToSave->empty()) return STATUS_CODE_SUCCESS;

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

		clustersToSave.push_back( *it );
	}
	

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_INITIALIZED, !=, 
			PandoraContentApi::SaveList(*this, mergedClusterListName, clustersToSave));

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=,
			PandoraContentApi::ReplaceCurrentList<pandora::Cluster>(*this, mergedClusterListName));

	std::cout << "temp cluster after saving: " << pClusterListToSave->size() << std::endl;

    //const pandora::ClusterList* pClusterList = NULL; 
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));
	std::cout << "cluster after saving: " << pClusterList->size() << std::endl;


	MergeClusters();

    return STATUS_CODE_SUCCESS;
}

StatusCode CheatingHitRecoveryAlgorithm::MergeClusters()
{
	////////////////////////////////////////////////////////////////////////////////
	// check the cluster and its mcp:
	// if one mcp has several clusters, then they should be merged into one cluster
	// it is especially mandatory for charged clusters
	////////////////////////////////////////////////////////////////////////////////
	
    const pandora::ClusterList* pClusterList = NULL; 
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

	//std::cout << "orginal cluster: " << pClusterList->size() << std::endl;

	std::map<const pandora::MCParticle* const, pandora::ClusterList> mcpClusterListMap;
	
	for(auto it = pClusterList->begin(); it != pClusterList->end(); ++it)
	{
		const pandora::Cluster* const clu = *it;

		const pandora::MCParticle* pClusterMCParticle  = NULL;

        try
        {
        	 pClusterMCParticle = pandora::MCParticleHelper::GetMainMCParticle( clu );
        }
        catch (pandora::StatusCodeException &)
        {
		    continue;
		}

		if(pClusterMCParticle != NULL && mcpClusterListMap.find( pClusterMCParticle ) == mcpClusterListMap.end())
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
		auto mcp = it->first;
		auto clusterList = it->second;

	    if(clusterList.size()>1) 
		{
			std::cout << "------>>> particle cluster # greater than 1: " << clusterList.size() << ", PDG: "
				      << mcp->GetParticleId() << std::endl;

			// merge clusters
			auto firstCluster = *( clusterList.begin() );

			for(auto cluIt = clusterList.begin(); cluIt != clusterList.end(); ++cluIt)
			{
				if(cluIt == clusterList.begin()) continue;

		        const pandora::Cluster* cluToMerge = *cluIt;
				PandoraContentApi::MergeAndDeleteClusters(*this, firstCluster, cluToMerge);
			}

		}// end if
	} // end for

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CheatingHitRecoveryAlgorithm::ReadSettings(const TiXmlHandle /*xmlHandle*/)
{
    return STATUS_CODE_SUCCESS;
}

} // namespace arbor_content
