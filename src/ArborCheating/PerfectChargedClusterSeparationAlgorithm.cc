/**
 *  @file   ArborContent/src/ArborCheating/PerfectChargedClusterSeparationAlgorithm.cc
 * 
 *  @brief  Implementation of the cheating clustering algorithm class
 * 
 *  $Log: $
 *  @author J. Marshall.
 *  Copied from https://github.com/PandoraPFA/LCContent sources
 */

#include "Pandora/AlgorithmHeaders.h"

#include "ArborCheating/PerfectChargedClusterSeparationAlgorithm.h"

using namespace pandora;

namespace arbor_content
{

  PerfectChargedClusterSeparationAlgorithm::PerfectChargedClusterSeparationAlgorithm()
  {
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  StatusCode PerfectChargedClusterSeparationAlgorithm::Run()
  {
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

	//std::cout << "-----> list : " << pClusterList->size() << std::endl;

    ClusterList localClusterList(pClusterList->begin(), pClusterList->end());
	
	// collection of calo hit and cluster to make new relationship 
    MCParticleToCaloHitListMap mcParticleToCaloHitListMap;

	if(!localClusterList.empty())
	{
		ClusterSeparation(localClusterList, mcParticleToCaloHitListMap);
	}

	const ClusterList *pNewClusterList = NULL; std::string newClusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::CreateTemporaryListAndSetCurrent(*this, pNewClusterList, newClusterListName));

	CreateClusters(mcParticleToCaloHitListMap);

	// FIXME
    std::string m_outputClusterListName("SeparatedClusters");
    if (!pNewClusterList->empty())
    {
	   std::cout << "Create new cluster: SeparatedClusters" << std::endl;
       PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveList<Cluster>(*this, m_outputClusterListName));
    }

    return STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------
  
  StatusCode PerfectChargedClusterSeparationAlgorithm::ClusterSeparation(ClusterList& localClusterList, MCParticleToCaloHitListMap& mcParticleToCaloHitListMap)
  {
	// collection to store the calo hit of neutral particle 

    for (ClusterList::const_iterator clusterIter = localClusterList.begin(), clusterIterEnd = localClusterList.end(); 
			clusterIter != clusterIterEnd; ++clusterIter)
    {
      try
      {
        const Cluster *const pCluster = *clusterIter;

#if 0
		const MCParticle *pClusterMCParticle = NULL;

	    try
	    {
           pClusterMCParticle = MCParticleHelper::GetMainMCParticle(pCluster);
	    }
        catch (StatusCodeException &)
        {
        }
#endif

		const OrderedCaloHitList& orderedCaloHitList = pCluster->GetOrderedCaloHitList();

        pandora::CaloHitList hitList;
    	orderedCaloHitList.FillCaloHitList(hitList);

		const pandora::MCParticle* firstMCP = NULL;

    	for(pandora::CaloHitList::const_iterator hitIter = hitList.begin(); hitIter != hitList.end(); ++hitIter)
    	{
    		const pandora::CaloHit* pCaloHit = *hitIter;
    
    		try
    		{
    			const pandora::MCParticle *const pMCParticle(pandora::MCParticleHelper::GetMainMCParticle(pCaloHit));
    			//int mcpCharge = pandora::PdgTable::GetParticleCharge(pMCParticle->GetParticleId());
    
    		    //std::cout << "hit MCP: " << pMCParticle << ", PID: " << pMCParticle->GetParticleId()  << 
    			//	", charge: " << mcpCharge << std::endl;

			   if(firstMCP==NULL)
			   {
			   	 firstMCP = pMCParticle;
			   }

			   if(firstMCP!=NULL && firstMCP != pMCParticle)
			   {
				   std::cout << "firstMCP: " << firstMCP << ", thisMCP: " << pMCParticle << std::endl;

				   AddToClusterMap(pCaloHit, pMCParticle, mcParticleToCaloHitListMap);
		           PandoraContentApi::RemoveFromCluster(*this, pCluster, pCaloHit);
			   }
    		}
    		catch (pandora::StatusCodeException &)
    		{
    		}
    	}
      }
      catch (StatusCodeException &)
      {
		  continue;
      }
    }

    return STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------
  void PerfectChargedClusterSeparationAlgorithm::AddToClusterMap(const pandora::CaloHit* pCaloHit,  const pandora::MCParticle* pMCParticle, MCParticleToCaloHitListMap& mcParticleToCaloHitListMap)
  {
	  MCParticleToCaloHitListMap::iterator caloHitListIter = mcParticleToCaloHitListMap.find(pMCParticle);

	  if(caloHitListIter == mcParticleToCaloHitListMap.end())
	  {
		  pandora::CaloHitList caloHits;
		  caloHits.push_back(pCaloHit);
		  mcParticleToCaloHitListMap.insert(MCParticleToCaloHitListMap::value_type(pMCParticle, caloHits));
	  }
	  else
	  {
		  pandora::CaloHitList& caloHits = caloHitListIter->second;
		  caloHits.push_back(pCaloHit);
	  }
  }
  
  //------------------------------------------------------------------------------------------------------------------------------------------

  void PerfectChargedClusterSeparationAlgorithm::CreateClusters(MCParticleToCaloHitListMap& mcParticleToCaloHitListMap) const
  {
	 for(MCParticleToCaloHitListMap::const_iterator iter = mcParticleToCaloHitListMap.begin(); 
			 iter != mcParticleToCaloHitListMap.end(); ++iter)
	 {
		const Cluster *pCluster = NULL;
		const CaloHitList& pCaloHitList = iter->second;

        PandoraContentApi::Cluster::Parameters parameters;
        parameters.m_caloHitList = pCaloHitList;

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, parameters, pCluster));

		const MCParticle *pClusterMCParticle = NULL;

	    try
	    {
           pClusterMCParticle = MCParticleHelper::GetMainMCParticle(pCluster);
	    }
        catch (StatusCodeException &)
        {
        }

		std::cout << "========PerfectClusterSeparationAlgorithm: create a cluster: " << pCluster 
			      << ", MCP: " << pClusterMCParticle << std::endl;
	 }
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  StatusCode PerfectChargedClusterSeparationAlgorithm::ReadSettings(const TiXmlHandle )
  {
    return STATUS_CODE_SUCCESS;
  }

}
