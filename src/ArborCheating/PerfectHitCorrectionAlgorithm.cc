/**
 *  @file   LCContent/src/LCCheating/PerfectHitCorrectionAlgorithm.cc
 * 
 *  @brief  Implementation of the perfect particle flow algorithm class
 * 
 *  @author J. Marshall
 *  @comment Copied from https://github.com/PandoraPFA/LCContent
 *
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "ArborCheating/PerfectHitCorrectionAlgorithm.h" // changed path

using namespace pandora;

namespace arbor_content // changed namespace
{

PerfectHitCorrectionAlgorithm::PerfectHitCorrectionAlgorithm() :
    m_simpleCaloHitCollection(true),
    m_minWeightFraction(0.01f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PerfectHitCorrectionAlgorithm::Run()
{
#if 0
    const MCParticleList *pMCParticleList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pMCParticleList));

    if (pMCParticleList->empty())
        return STATUS_CODE_SUCCESS;

    const ClusterList *pClusterList = NULL; std::string clusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::CreateTemporaryListAndSetCurrent(*this, pClusterList, clusterListName));

    for (MCParticleList::const_iterator iterMC = pMCParticleList->begin(), iterMCEnd = pMCParticleList->end(); iterMC != iterMCEnd; ++iterMC)
    {
        try
        {
            const MCParticle *const pPfoTarget = *iterMC;
            PfoParameters pfoParameters;

            this->CaloHitCollection(pPfoTarget, pfoParameters);
        }
        catch (StatusCodeException &)
        {
        }
    }

    if (!pClusterList->empty())
    {
		//std::string tupleName = "PerfectHitCorrectionAlgorithm-" + string(__func__);
		//std::string varListName = "clusterSize";
		//std::vector<float> vars;

		//vars.push_back(pClusterList->size());

	    //AHM.CreateFill(tupleName, varListName, vars);

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveList<Cluster>(*this, m_outputClusterListName));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<Cluster>(*this, m_outputClusterListName));
    }

    return STATUS_CODE_SUCCESS;
#endif


    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

	std::cout << "Hit Correction -----> list : " << pClusterList->size() << std::endl;

#if 0
    ClusterList localClusterList(pClusterList->begin(), pClusterList->end());
    MCParticleToClusterListMap mcParticleToClusterListMap;
	
	if(!localClusterList.empty())
	{
		for(ClusterList::const_iterator iter = localClusterList.begin(); iter != localClusterList.end(); ++iter)
		{
			const Cluster* const pCluster = *iter;
			SimpleMCParticleClusterListCollection(pCluster, mcParticleToClusterListMap);
		}
	}
#endif
    return STATUS_CODE_SUCCESS;

}

//------------------------------------------------------------------------------------------------------------------------------------------

void PerfectHitCorrectionAlgorithm::CaloHitCollection(const MCParticle *const pPfoTarget, PfoParameters &pfoParameters) const
{
    const CaloHitList *pCaloHitList = NULL;
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pCaloHitList));

    const Cluster *pCluster = NULL;
    const CaloHitList localCaloHitList(pCaloHitList->begin(), pCaloHitList->end());

    for (CaloHitList::const_iterator iter = localCaloHitList.begin(), iterEnd = localCaloHitList.end(); iter != iterEnd; ++iter)
    {
        try
        {
            const CaloHit *const pCaloHit = *iter;

            if (!PandoraContentApi::IsAvailable(*this, pCaloHit))
                continue;

            PandoraContentApi::Cluster::Parameters parameters;
            CaloHitList &caloHitList(parameters.m_caloHitList);

            if (m_simpleCaloHitCollection)
            {
                this->SimpleCaloHitCollection(pPfoTarget, pCaloHit, caloHitList);
            }
            else
            {
                this->FullCaloHitCollection(pPfoTarget, pCaloHit, caloHitList);
            }

            if (caloHitList.empty())
                continue;

            if (NULL == pCluster)
            {
                PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, parameters, pCluster));
                pfoParameters.m_clusterList.push_back(pCluster);
            }
            else
            {
                for (CaloHitList::const_iterator hitIter = caloHitList.begin(), hitIterEnd = caloHitList.end(); hitIter != hitIterEnd; ++hitIter)
                    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddToCluster(*this, pCluster, *hitIter));
            }
        }
        catch (StatusCodeException &e)
        {
			//std::cout << e.ToString() << std::endl;
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void PerfectHitCorrectionAlgorithm::SimpleCaloHitCollection(const MCParticle *const pPfoTarget, const CaloHit *const pCaloHit, CaloHitList &caloHitList) const
{
    float bestWeight(0.f);
    const MCParticle *pBestMCParticle(nullptr);
    const MCParticleWeightMap &hitMCParticleWeightMap(pCaloHit->GetMCParticleWeightMap());

    MCParticleVector mcParticleVector;
    for (const MCParticleWeightMap::value_type &mapEntry : hitMCParticleWeightMap) mcParticleVector.push_back(mapEntry.first);
    std::sort(mcParticleVector.begin(), mcParticleVector.end(), PointerLessThan<MCParticle>());

    for (const MCParticle *const pMCParticle : mcParticleVector)
    {
        const float weight(hitMCParticleWeightMap.at(pMCParticle));

        if (weight > bestWeight)
        {
            bestWeight = weight;
            pBestMCParticle = pMCParticle;
        }
    }

    if (!pBestMCParticle)
	{
		//std::cout << "hitMCParticleWeightMap size: " <<  hitMCParticleWeightMap.size() << std::endl;
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);
	}


    const MCParticle *const pHitMCParticle(pBestMCParticle);
    const MCParticle *const pHitPfoTarget(pHitMCParticle->GetPfoTarget());

    if (pHitPfoTarget != pPfoTarget)
        return;

    caloHitList.push_back(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void PerfectHitCorrectionAlgorithm::FullCaloHitCollection(const MCParticle *const pPfoTarget, const CaloHit *const pCaloHit, CaloHitList &caloHitList) const
{
    const MCParticleWeightMap mcParticleWeightMap(pCaloHit->GetMCParticleWeightMap());

    if (mcParticleWeightMap.size() < 2)
        return this->SimpleCaloHitCollection(pPfoTarget, pCaloHit, caloHitList);

    float mcParticleWeightSum(0.f);

    MCParticleList mcParticleList;
    for (const auto &mapEntry : mcParticleWeightMap) mcParticleList.push_back(mapEntry.first);
    mcParticleList.sort(PointerLessThan<MCParticle>());

    for (const MCParticle *const pMCParticle : mcParticleList)
        mcParticleWeightSum += mcParticleWeightMap.at(pMCParticle);

    if (mcParticleWeightSum < std::numeric_limits<float>::epsilon())
        throw StatusCodeException(STATUS_CODE_FAILURE);

    const CaloHit *pLocalCaloHit = pCaloHit;

    for (const MCParticle *const pHitMCParticle : mcParticleList)
    {
        const MCParticle *const pHitPfoTarget(pHitMCParticle->GetPfoTarget());
        const float weight(mcParticleWeightMap.at(pHitMCParticle));

        if (pHitPfoTarget != pPfoTarget)
            continue;

        const CaloHit *pCaloHitToAdd = pLocalCaloHit;

        if (pCaloHitToAdd->GetWeight() < std::numeric_limits<float>::epsilon())
            throw StatusCodeException(STATUS_CODE_FAILURE);

        const float weightFraction(weight / (mcParticleWeightSum * pCaloHitToAdd->GetWeight()));
        const bool isBelowThreshold((weightFraction - m_minWeightFraction) < std::numeric_limits<float>::epsilon());

        if (isBelowThreshold)
            continue;

        const bool shouldFragment(weightFraction + m_minWeightFraction - 1.f < std::numeric_limits<float>::epsilon());

        if (shouldFragment)
            PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Fragment(*this, pLocalCaloHit, weightFraction, pCaloHitToAdd, pLocalCaloHit));

        caloHitList.push_back(pCaloHitToAdd);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
StatusCode PerfectHitCorrectionAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "OutputClusterListName", m_outputClusterListName));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
