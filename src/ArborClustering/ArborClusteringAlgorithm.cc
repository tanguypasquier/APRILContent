  /// \file ArborClusteringAlgorithm.cc
/*
 *
 * ArborClusteringAlgorithm.cc source template automatically generated by a class generator
 * Creation date : ven. d�c. 4 2015
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


#include "ArborClustering/ArborClusteringAlgorithm.h"

#include "Pandora/AlgorithmHeaders.h"

#include "ArborHelpers/CaloHitHelper.h"
#include "ArborTools/ConnectorAlgorithmTool.h"

namespace arbor_content
{

pandora::StatusCode ArborClusteringAlgorithm::Run()
{
	const pandora::CaloHitList *pCaloHitList = NULL;
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pCaloHitList));

	if(pCaloHitList->empty())
		return pandora::STATUS_CODE_SUCCESS;

	pandora::CaloHitList ecalCaloHitList, hcalCaloHitList, muonCaloHitList;
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->SplitCaloHitList(pCaloHitList, ecalCaloHitList, hcalCaloHitList, muonCaloHitList));
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->ConnectCaloHits(pCaloHitList, ecalCaloHitList, hcalCaloHitList, muonCaloHitList));
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->CreateClusters());

	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborClusteringAlgorithm::SplitCaloHitList(const pandora::CaloHitList *const pCaloHitList, pandora::CaloHitList &ecalCaloHitList,
		pandora::CaloHitList &hcalCaloHitList, pandora::CaloHitList &muonCaloHitList) const
{
	if( m_ecalToolList.empty() && m_hcalToolList.empty() && m_muonToolList.empty() )
		return pandora::STATUS_CODE_SUCCESS;

	for(pandora::CaloHitList::const_iterator iter = pCaloHitList->begin(), endIter = pCaloHitList->end() ;
			endIter !=iter ; ++iter)
	{
		const pandora::CaloHit *const pCaloHit= *iter;

		if(!PandoraContentApi::IsAvailable(*this, pCaloHit))
			continue;

		if(pCaloHit->GetHitType() == pandora::ECAL)
			ecalCaloHitList.insert(pCaloHit);
		else if(pCaloHit->GetHitType() == pandora::HCAL)
			hcalCaloHitList.insert(pCaloHit);
		else if(pCaloHit->GetHitType() == pandora::MUON)
			muonCaloHitList.insert(pCaloHit);
	}

	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborClusteringAlgorithm::ConnectCaloHits(const pandora::CaloHitList *const pCaloHitList, const pandora::CaloHitList &ecalCaloHitList,
		const pandora::CaloHitList &hcalCaloHitList, const pandora::CaloHitList &muonCaloHitList) const
{
	if(m_useMultithread)
	{
#ifdef ARBOR_PARALLEL
		// run in parallel the three detector region in different OpenMP sections
		// returns in case one the thread returned a bad status code
		pandora::StatusCode globalStatusCode = pandora::STATUS_CODE_SUCCESS;

#pragma omp parallel sections
		{
#pragma omp section
			{
				const pandora::StatusCode statusCode = this->ConnectCaloHits(ecalCaloHitList, m_ecalToolList);

#pragma omp critical
				{
					if(globalStatusCode == pandora::STATUS_CODE_SUCCESS)
						globalStatusCode = statusCode;
				}
			}
#pragma omp section
			{
				const pandora::StatusCode statusCode = this->ConnectCaloHits(hcalCaloHitList, m_hcalToolList);

#pragma omp critical
				{
					if(globalStatusCode == pandora::STATUS_CODE_SUCCESS)
						globalStatusCode = statusCode;
				}
			}
#pragma omp section
			{
				const pandora::StatusCode statusCode = this->ConnectCaloHits(muonCaloHitList, m_muonToolList);

#pragma omp critical
				{
					if(globalStatusCode == pandora::STATUS_CODE_SUCCESS)
						globalStatusCode = statusCode;
				}
			}
		}

		if(globalStatusCode != pandora::STATUS_CODE_SUCCESS)
			return globalStatusCode;
#endif
	}
	else
	{
		// single core algorithm
		PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->ConnectCaloHits(ecalCaloHitList, m_ecalToolList));
		PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->ConnectCaloHits(hcalCaloHitList, m_hcalToolList));
		PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->ConnectCaloHits(muonCaloHitList, m_muonToolList));
	}

	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->ConnectCaloHits(*pCaloHitList, m_additionalToolList));

	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborClusteringAlgorithm::ConnectCaloHits(const pandora::CaloHitList &caloHitList, const ConnectorAlgorithmToolVector &toolVector) const
{
	for(ConnectorAlgorithmToolVector::const_iterator iter = toolVector.begin(), endIter = toolVector.end() ;
			endIter != iter ; ++iter)
	{
		ConnectorAlgorithmTool *pTool = *iter;
		PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pTool->Process(*this, &caloHitList));
	}

	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborClusteringAlgorithm::CreateClusters() const
{
	if( m_ecalToolList.empty() && m_hcalToolList.empty() && m_muonToolList.empty() && m_additionalToolList.empty() )
		return pandora::STATUS_CODE_SUCCESS;

	// Find seeds and build clusters from them looking for forward connected hits in the tree structure
	pandora::CaloHitList seedCaloHitList;
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, CaloHitHelper::ExtractCurrentSeedCaloHitList(*this, seedCaloHitList, !m_allowSingleHitClusters));

	for(pandora::CaloHitList::iterator iter = seedCaloHitList.begin(), endIter = seedCaloHitList.end() ;
			endIter != iter ; ++iter)
	{
		const arbor_content::CaloHit *const pCaloHit = dynamic_cast<const arbor_content::CaloHit *const>(*iter);

		// wrong framework baby !
		if(NULL == pCaloHit)
			continue;

		pandora::CaloHitList clusterCaloHitList;
		clusterCaloHitList.insert(pCaloHit);

		// get the whole tree calo hit list
		PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, CaloHitHelper::BuildCaloHitList(pCaloHit, FORWARD_DIRECTION, clusterCaloHitList));

		// create a cluster with this list
		const pandora::Cluster *pCluster = NULL;
		PandoraContentApi::ClusterParameters clusterParameters;
		clusterParameters.m_caloHitList = clusterCaloHitList;

		PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, clusterParameters, pCluster));
	}

	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode ArborClusteringAlgorithm::ReadSettings(const pandora::TiXmlHandle xmlHandle)
{
	pandora::AlgorithmToolList algorithmToolList;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ProcessAlgorithmToolList(*this, xmlHandle,
        "ECalConnectionTools", algorithmToolList));

    for(pandora::AlgorithmToolList::const_iterator iter = algorithmToolList.begin(), endIter = algorithmToolList.end() ;
    		endIter != iter ; ++iter)
    {
    	ConnectorAlgorithmTool *pTool = dynamic_cast<ConnectorAlgorithmTool*>(*iter);

    	if(NULL == pTool)
    		return pandora::STATUS_CODE_INVALID_PARAMETER;

    	m_ecalToolList.push_back(pTool);
    }

    algorithmToolList.clear();

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ProcessAlgorithmToolList(*this, xmlHandle,
        "HCalConnectionTools", algorithmToolList));

    for(pandora::AlgorithmToolList::const_iterator iter = algorithmToolList.begin(), endIter = algorithmToolList.end() ;
    		endIter != iter ; ++iter)
    {
    	ConnectorAlgorithmTool *pTool = dynamic_cast<ConnectorAlgorithmTool*>(*iter);

    	if(NULL == pTool)
    		return pandora::STATUS_CODE_INVALID_PARAMETER;

    	m_hcalToolList.push_back(pTool);
    }

    algorithmToolList.clear();

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ProcessAlgorithmToolList(*this, xmlHandle,
        "MuonConnectionTools", algorithmToolList));

    for(pandora::AlgorithmToolList::const_iterator iter = algorithmToolList.begin(), endIter = algorithmToolList.end() ;
    		endIter != iter ; ++iter)
    {
    	ConnectorAlgorithmTool *pTool = dynamic_cast<ConnectorAlgorithmTool*>(*iter);

    	if(NULL == pTool)
    		return pandora::STATUS_CODE_INVALID_PARAMETER;

    	m_muonToolList.push_back(pTool);
    }

    algorithmToolList.clear();

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ProcessAlgorithmToolList(*this, xmlHandle,
        "AdditionalConnectionTools", algorithmToolList));

    for(pandora::AlgorithmToolList::const_iterator iter = algorithmToolList.begin(), endIter = algorithmToolList.end() ;
    		endIter != iter ; ++iter)
    {
    	ConnectorAlgorithmTool *pTool = dynamic_cast<ConnectorAlgorithmTool*>(*iter);

    	if(NULL == pTool)
    		return pandora::STATUS_CODE_INVALID_PARAMETER;

    	m_additionalToolList.push_back(pTool);
    }

    algorithmToolList.clear();

    m_allowSingleHitClusters = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "AllowSingleHitClusters", m_allowSingleHitClusters));

#ifdef ARBOR_PARALLEL
    m_useMultithread = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "UseMultithread", m_useMultithread));
#else
    m_useMultithread = false;
#endif

    unsigned int nNonEmptyToolList = 0;

    nNonEmptyToolList += m_ecalToolList.empty() ? 0 : 1;
    nNonEmptyToolList += m_hcalToolList.empty() ? 0 : 1;
    nNonEmptyToolList += m_muonToolList.empty() ? 0 : 1;

    if(nNonEmptyToolList <= 1 && m_useMultithread)
    {
    	std::cout << "Multi-threading disabled" << std::endl;
    	m_useMultithread = false;
    }

	return pandora::STATUS_CODE_SUCCESS;
}


} 

