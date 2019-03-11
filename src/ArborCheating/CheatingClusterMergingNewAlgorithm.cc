/**
 *  @file   CheatingClusterMergingNewAlgorithm.cc
 * 
 *  @brief  Implementation of the cheating cluster cleaning algorithm class
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "ArborCheating/CheatingClusterMergingNewAlgorithm.h"
#include "ArborApi/ArborContentApi.h"
#include "ArborUtility/EventPreparationAlgorithm.h"
#include "ArborHelpers/HistogramHelper.h"

//#include "ArborHelpers/ClusterHelper.h"
//#include "ArborUtility/EventPreparationAlgorithm.h"
//#include "ArborHelpers/HistogramHelper.h"

namespace arbor_content
{

pandora::StatusCode CheatingClusterMergingNewAlgorithm::Run()
{
	MergeClusters();
	
    return pandora::STATUS_CODE_SUCCESS;
}

pandora::StatusCode CheatingClusterMergingNewAlgorithm::MergeClusters()
{
	////////////////////////////////////////////////////////////////////////////////
	// check the cluster and its mcp:
	// if one mcp has several clusters, then they should be merged into one cluster
	// it is especially mandatory for charged clusters
	////////////////////////////////////////////////////////////////////////////////
	
    const pandora::ClusterList* pClusterList = nullptr; 
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));
	std::cout << "cluster befor merging: " << pClusterList->size() << std::endl;

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

		///////////////////////////////////////////////////////////////////////////////////////////////////
	    std::vector<float> vars;
	    vars.push_back( float(EventPreparationAlgorithm::GetEventNumber()) );
	    vars.push_back( float(clu->GetNCaloHits()) );
	    vars.push_back( float(clu->GetElectromagneticEnergy()) );
	    vars.push_back( float(clu->GetHadronicEnergy()) );

		HistogramManager::CreateFill("ClusterBeforeMerging", 
		"evtNumber:nCaloHit:clusterEMEnergy:clusterHadEnergy", vars);
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

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));
	std::cout << "cluster after merging: " << pClusterList->size() << std::endl;

    return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode CheatingClusterMergingNewAlgorithm::ReadSettings(const pandora::TiXmlHandle /* xmlHandle */)
{
    //PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ReadValue(xmlHandle,
    //    "ClusterListToTakeNewClusters", m_mergedClusterListName));

    return pandora::STATUS_CODE_SUCCESS;
}

} // namespace arbor_content
