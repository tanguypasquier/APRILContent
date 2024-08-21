/// \file ConnectorCleaningTool.cc
/*
 *
 * ConnectorCleaningTool.cc source template automatically generated by a class generator
 * Creation date : mar. d�c. 8 2015
 *
 * This file is part of APRILContent libraries.
 * 
 * APRILContent is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 * 
 * APRILContent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with APRILContent.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */


#include "APRILTools/ConnectorCleaningTool.h"

#include "Pandora/AlgorithmHeaders.h"

#include "APRILObjects/CaloHit.h"
#include "APRILObjects/Connector.h"
#include "APRILHelpers/CaloHitHelper.h"
#include "APRILHelpers/HistogramHelper.h"
#include "APRILHelpers/SortingHelper.h"
#include "APRILClustering/APRILClusteringAlgorithm.h"

#include "APRILUtility/EventPreparationAlgorithm.h"

//Added by TP
//#include <fstream>

namespace april_content
{
  float ConnectorOrderParameter::m_smallAngleRange(0.01);
  float ConnectorOrderParameter::m_orderParameterAnglePower(1.);
  float ConnectorOrderParameter::m_orderParameterDistancePower(1.);

  pandora::StatusCode ConnectorCleaningTool::Process(const pandora::Algorithm& /*algorithm*/, const pandora::CaloHitList *const /* caloHitList */)
  {
	//std::cout << "  --- ConnectorCleaningTool::Process" << std::endl;
    ConnectorOrderParameter::m_orderParameterAnglePower = m_orderParameterAnglePower;
    ConnectorOrderParameter::m_orderParameterDistancePower = m_orderParameterDistancePower;

    const pandora::CaloHitList* pCaloHitList = APRILClusteringAlgorithm::GetCaloHitList();

    if(0 == m_strategy)
    {
      PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->CleanCaloHits(pCaloHitList));
    }
    else
    {
      pandora::OrderedCaloHitList orderedCaloHitList;
      PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, orderedCaloHitList.Add(*pCaloHitList));

      PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->CleanCaloHits(orderedCaloHitList));
    }

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::CartesianVector ConnectorCleaningTool::GetReferenceVector(const april_content::CaloHit *const pCaloHit) const
  {
    pandora::CartesianVector referenceVector = pandora::CartesianVector(0, 0, 0);

    try
    {
      pandora::CartesianVector meanForwardDirection(0, 0, 0);
      pandora::CartesianVector meanBackwardDirection(0, 0, 0);

      PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, CaloHitHelper::GetMeanDirection(pCaloHit, FORWARD_DIRECTION,
          meanForwardDirection, m_forwardReferenceDirectionDepth));
      meanForwardDirection *= m_forwardConnectorWeight;

      PANDORA_THROW_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, CaloHitHelper::GetMeanDirection(pCaloHit, BACKWARD_DIRECTION,
          meanBackwardDirection, m_backwardReferenceDirectionDepth));

	  //std::cout << "m_backwardConnectorWeight: " << m_backwardConnectorWeight << ", m_forwardConnectorWeight: "
		//  << m_forwardConnectorWeight << ", m_backwardReferenceDirectionDepth: " << m_backwardReferenceDirectionDepth 
		//  << ", m_forwardReferenceDirectionDepth: " << m_forwardReferenceDirectionDepth << std::endl;

      meanBackwardDirection *= m_backwardConnectorWeight;

      if(meanForwardDirection == meanBackwardDirection)
        throw pandora::StatusCodeException(pandora::STATUS_CODE_FAILURE);

      // both directions are defined in the forward direction 
      referenceVector = meanBackwardDirection + meanForwardDirection;


#if 0
	  std::cout << "  GetReferenceVector ==== " << std::endl;
	  std::cout << "  --- meanBackwardDirection: " << meanBackwardDirection.GetX() << ", " << meanBackwardDirection.GetY() << ", " 
		        << meanBackwardDirection.GetZ() << std::endl;

	  std::cout << "  --- meanForwardDirection: " << meanForwardDirection.GetX() << ", " << meanForwardDirection.GetY() << ", " 
		        << meanForwardDirection.GetZ() << std::endl;

	  std::cout << "  --- referenceVector: " << referenceVector.GetX() << ", " << referenceVector.GetY() << ", " 
		        << referenceVector.GetZ() << std::endl;
#endif

    }
    catch(pandora::StatusCodeException &exception)
    {
    }

    return referenceVector;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode ConnectorCleaningTool::CleanCaloHits(const pandora::CaloHitList *const pCaloHitList) const
  {
    CaloHitCleaningMap caloHitCleaningMap;

	// sort the hits by their positions,
	// the connector list will also be sorted.
	pandora::CaloHitList aHitList(*pCaloHitList);
	aHitList.sort(SortingHelper::SortHitsByPosition);
    pandora::CaloHitVector caloHitVector(aHitList.begin(), aHitList.end());

    for(auto caloHit : caloHitVector)
    {
      const april_content::CaloHit *pCaloHit = dynamic_cast<const april_content::CaloHit *>(caloHit);

      if(NULL == pCaloHit) return pandora::STATUS_CODE_FAILURE;

      ConnectorList backwardConnectorList(APRILContentApi::GetConnectorList(pCaloHit, BACKWARD_DIRECTION));
	    std::vector<const Connector*> backwardConnectorVector(backwardConnectorList.begin(), backwardConnectorList.end());
	    std::sort(backwardConnectorVector.begin(), backwardConnectorVector.end(), SortingHelper::SortConnectorsByFromPosition);

      if(backwardConnectorList.size() < 2) continue;

      pandora::CartesianVector referenceVector(this->GetReferenceVector(pCaloHit));

      if(referenceVector == pandora::CartesianVector(0.f, 0.f, 0.f)) return pandora::STATUS_CODE_FAILURE;

      const CaloHit *pBestCaloHit = NULL;
      ConnectorOrderParameter bestOrderParameter; 
      pandora::CaloHitList deleteConnectionCaloHitList;

      //Added by TP
      bool hasNonCausalConnector=false;

      // find the best connector with the smallest order parameter
	  for(int iCon = 0; iCon < backwardConnectorVector.size(); ++iCon)
      {
        const Connector *pConnector = backwardConnectorVector.at(iCon);
        const CaloHit *pFromCaloHit = pConnector->GetFrom();
        const pandora::CartesianVector connectorVector = pConnector->GetVector(FORWARD_DIRECTION);
        const float distance = pConnector->GetLength(); //In mm
#if 0
        //Added by TP
        if(pandora::HCAL == pFromCaloHit->GetHitType())
        {
          const float timing = pConnector->GetTiming() * 1e-6; //time in nanoseconds that we convert to have milliseconds
          const float c = 2.99792458e8; //Lightspeed
          const float beta = (distance/timing) / c;
          //std::cout << "Beta : " << beta << std::endl;
          //fichier << beta << std::endl;
          //fichier.close();
          if( beta > 1)
          {
            deleteConnectionCaloHitList.push_back(pFromCaloHit);
            hasNonCausalConnector=true;
            continue; //Add the connection to the list to delete and go to the next one
          }
        }

        //End added by TP
#endif

        const float angle = referenceVector.GetOpeningAngle(connectorVector);

        ///////////////////////////////
        std::vector<float> vars;
        vars.push_back( float(EventPreparationAlgorithm::GetEventNumber()) );
        vars.push_back( angle );
        vars.push_back( distance );
	
        HistogramManager::CreateFill("ConnectorProperties", "evtNumber:angle:distance", vars);

		const unsigned int creationStage = pConnector->GetCreationStage();
		//std::cout << "connector creation at stage: " << creationStage << std::endl;

		// different weights for fwd and bwd
	    const unsigned int nConnectors = APRILContentApi::GetConnectorList(pFromCaloHit, FORWARD_DIRECTION).size() + 
			100 * APRILContentApi::GetConnectorList(pFromCaloHit, BACKWARD_DIRECTION).size();

		auto& hitPos = pFromCaloHit->GetPositionVector();
		ConnectorOrderParameter orderParameter(distance, angle, nConnectors, creationStage, hitPos);

        if(orderParameter < bestOrderParameter)
        {
          if(NULL != pBestCaloHit)
            deleteConnectionCaloHitList.push_back(pBestCaloHit);

          bestOrderParameter = orderParameter;
          pBestCaloHit = pFromCaloHit;
        }
        else
        {
          deleteConnectionCaloHitList.push_back(pFromCaloHit);
        }
      }

      if(NULL != pBestCaloHit || hasNonCausalConnector) //Added by TP the condition to delete the connectors if hasNonCausalConnector is true even if no bestCaloHit found
        caloHitCleaningMap[pCaloHit] = deleteConnectionCaloHitList;
    }

    // remove all un-needed connections
    for(CaloHitCleaningMap::const_iterator iter = caloHitCleaningMap.begin(), endIter = caloHitCleaningMap.end() ;
        endIter != iter ; ++iter)
    {
      for(pandora::CaloHitList::const_iterator hitIter = iter->second.begin(), hitEndIter = iter->second.end() ;
          hitEndIter != hitIter ; ++hitIter)
      {
        const april_content::CaloHit *const pCaloHit = dynamic_cast<const april_content::CaloHit *const>(*hitIter);
        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, APRILContentApi::RemoveConnectionBetween(iter->first, pCaloHit));
      }
    }

    caloHitCleaningMap.clear();

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode ConnectorCleaningTool::CleanCaloHits(const pandora::OrderedCaloHitList &orderedCaloHitList) const
  {
    if(1 == m_strategy)
    {
      for(pandora::OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), endIter = orderedCaloHitList.end() ;
          endIter != iter ; ++iter)
      {
        if(iter->second->empty())
          continue;

        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->CleanCaloHits(iter->second));
      }
    }
    else
    {
      for(pandora::OrderedCaloHitList::const_reverse_iterator iter = orderedCaloHitList.rbegin(), endIter = orderedCaloHitList.rend() ;
          endIter != iter ; ++iter)
      {
        if(iter->second->empty())
          continue;

        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->CleanCaloHits(iter->second));
      }
    }

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode ConnectorCleaningTool::ReadSettings(const pandora::TiXmlHandle xmlHandle)
  {
    m_backwardConnectorWeight = 1.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "BackwardConnectorWeight", m_backwardConnectorWeight));

    m_forwardConnectorWeight = 3.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "ForwardConnectorWeight", m_forwardConnectorWeight));

    m_backwardReferenceDirectionDepth = 1;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "BackwardReferenceDirectionDepth", m_backwardReferenceDirectionDepth));

    m_forwardReferenceDirectionDepth = 2;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "ForwardReferenceDirectionDepth", m_forwardReferenceDirectionDepth));

    if(m_backwardReferenceDirectionDepth == 0 || m_forwardReferenceDirectionDepth == 0)
      return pandora::STATUS_CODE_INVALID_PARAMETER;

    m_orderParameterAnglePower = 1.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "OrderParameterAnglePower", m_orderParameterAnglePower));

    m_orderParameterDistancePower = 5.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "OrderParameterDistancePower", m_orderParameterDistancePower));

    m_strategy = 2;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "Strategy", m_strategy));

    if(2 < m_strategy)
      return pandora::STATUS_CODE_INVALID_PARAMETER;

    return pandora::STATUS_CODE_SUCCESS;
  }

} 

