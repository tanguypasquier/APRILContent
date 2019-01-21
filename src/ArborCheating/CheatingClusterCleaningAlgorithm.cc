/**
 *  @file   CheatingClusterCleaningAlgorithm.cc
 * 
 *  @brief  Implementation of the cheating cluster cleaning algorithm class
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "ArborCheating/CheatingClusterCleaningAlgorithm.h"
#include "ArborObjects/CaloHit.h"
#include "ArborHelpers/HistogramHelper.h"

using namespace pandora;

namespace arbor_content
{

StatusCode CheatingClusterCleaningAlgorithm::Run()
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

	//std::cout << " cluster size: " << pClusterList->size() << std::endl;

    // Examine clusters, reducing each to just the hits corresponding to the main mc particle
    for (ClusterList::const_iterator itCluster = pClusterList->begin(), itClusterEnd = pClusterList->end(); itCluster != itClusterEnd; ++itCluster )
    {
        try
        {
            const Cluster *const pCluster = *itCluster;
            const MCParticle *const pMainMCParticle(MCParticleHelper::GetMainMCParticle(pCluster));

            // Remove all calo hits that do not correspond to the cluster main mc particle
			pandora::OrderedCaloHitList orderedCaloHitList(pCluster->GetOrderedCaloHitList());

            for (pandora::OrderedCaloHitList::const_iterator itLyr = orderedCaloHitList.begin(), itLyrEnd = orderedCaloHitList.end(); itLyr != itLyrEnd; ++itLyr)
            {
                for (pandora::CaloHitList::const_iterator hitIter = itLyr->second->begin(), hitIterEnd = itLyr->second->end(); hitIter != hitIterEnd; ++hitIter)
                {
                    try
                    {
                        const pandora::CaloHit *const pCaloHit = *hitIter;
                        const MCParticle *const pMCParticle(MCParticleHelper::GetMainMCParticle(pCaloHit));

                        if (pMainMCParticle != pMCParticle)
                        {
                            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RemoveFromCluster(*this, pCluster, pCaloHit));

							//std::cout << " --- remove hit " << pCaloHit << " from cluster: " << pCluster << std::endl;
        
							const arbor_content::CaloHit *const pArborCaloHit = dynamic_cast<const arbor_content::CaloHit *const>( pCaloHit );

                            if(NULL == pArborCaloHit) continue;

							//pArborCaloHit->GetDensity();
							float fwdConnection = 0.;
							float bakConnection = 0.;

							const ConnectorList &fwdConnectorList(ArborContentApi::GetConnectorList(pArborCaloHit, FORWARD_DIRECTION));
							const ConnectorList &bakConnectorList(ArborContentApi::GetConnectorList(pArborCaloHit, BACKWARD_DIRECTION));

							for(auto fwdCon : fwdConnectorList)
							{
								float fwdConnectionLength = fwdCon->GetLength();

								if( fwdConnectionLength > fwdConnection ) fwdConnection = fwdConnectionLength;
							}

							for(auto bakCon : bakConnectorList)
							{
								float bakConnectionLength = bakCon->GetLength();

								if( bakConnectionLength > bakConnection ) bakConnection = bakConnectionLength;
							}

							float clusterEnergy = pCluster->GetHadronicEnergy();
							float hitEnergy = pArborCaloHit->GetInputEnergy();
			   
							float hitCharge = pandora::PdgTable::GetParticleCharge( pMCParticle->GetParticleId() );
							float cluCharge = pandora::PdgTable::GetParticleCharge( pMainMCParticle->GetParticleId() );

							std::vector<float> vars;
							vars.push_back( clusterEnergy );
							vars.push_back( hitEnergy );
							vars.push_back( fwdConnection );
							vars.push_back( bakConnection );
							vars.push_back( hitCharge );
							vars.push_back( cluCharge );

							HistogramManager::CreateFill("CheatingClusterCleaning", 
									"clusterEnergy:hitEnergy:fwdConnection:bakConnection:hitCharge:cluCharge", vars);
                        }
                    }
                    catch (StatusCodeException &)
                    {
                    }
                }
            }

#if 0
            // Repeat for isolated hits
            CaloHitList isolatedCaloHitList(pCluster->GetIsolatedCaloHitList());

            for (CaloHitList::const_iterator hitIter = isolatedCaloHitList.begin(), hitIterEnd = isolatedCaloHitList.end(); hitIter != hitIterEnd; ++hitIter)
            {
                try
                {
                    const CaloHit *const pCaloHit = *hitIter;
                    const MCParticle *const pMCParticle(MCParticleHelper::GetMainMCParticle(pCaloHit));

                    if (pMainMCParticle != pMCParticle)
                    {
				        std::cout << "------ cleaning isohits: " << std::endl;
                        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RemoveIsolatedFromCluster(*this, pCluster, pCaloHit));
                    }
                }
                catch (StatusCodeException &)
                {
                }
            }
#endif
        }
        catch (StatusCodeException &)
        {
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CheatingClusterCleaningAlgorithm::ReadSettings(const TiXmlHandle /*xmlHandle*/)
{
    return STATUS_CODE_SUCCESS;
}

} // namespace arbor_content
