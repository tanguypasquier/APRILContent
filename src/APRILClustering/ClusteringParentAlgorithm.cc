/// \file ClusteringParentAlgorithm.cc
/*
 *
 * ParentClusteringAlgorithm.cc source template automatically generated by a class generator
 * Creation date : mer. avr. 8 2015
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
 * 	@author J. Marshall
 * 	@comment Copied from LCContent at https://github.com/PandoraPFA/LCContent
 */
#include "Pandora/AlgorithmHeaders.h"

#include "APRILClustering/ClusteringParentAlgorithm.h"

using namespace pandora;

namespace april_content // modified namespace
{

  ClusteringParentAlgorithm::ClusteringParentAlgorithm() :
        m_restoreOriginalCaloHitList(false),
        m_replaceCurrentClusterList(true)
  {
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  StatusCode ClusteringParentAlgorithm::Run()
  {
    // If specified, change the current calo hit list, i.e. the input to the clustering algorithm
    std::string originalCaloHitListName;

    if (!m_inputCaloHitListName.empty())
    {
      PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentListName<CaloHit>(*this, originalCaloHitListName));
      PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<CaloHit>(*this, m_inputCaloHitListName));
    }

    // Run the initial cluster formation algorithm
    const ClusterList *pClusterList = NULL;
    std::string newClusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, m_clusteringAlgorithmName,
        pClusterList, newClusterListName));

    // Run the topological association algorithms to modify clusters
    if (!pClusterList->empty() && !m_associationAlgorithmName.empty())
      PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_associationAlgorithmName));

    // Save the new cluster list
    if (!pClusterList->empty())
    {
      PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveList<Cluster>(*this, m_clusterListName));

      if (m_replaceCurrentClusterList)
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<Cluster>(*this, m_clusterListName));
    }

    // Unless specified, return current calo hit list to that when algorithm started
    if (m_restoreOriginalCaloHitList && !m_inputCaloHitListName.empty())
    {
      PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<CaloHit>(*this, originalCaloHitListName));
    }

    return STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  StatusCode ClusteringParentAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
  {
    // Daughter algorithm parameters
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle,
        "ClusterFormation", m_clusteringAlgorithmName));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle,
        "ClusterAssociation", m_associationAlgorithmName));

    // Input parameters: name of input calo hit list and whether it should persist as the current list after algorithm has finished
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "InputCaloHitListName", m_inputCaloHitListName));

    m_restoreOriginalCaloHitList = !m_inputCaloHitListName.empty();
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "RestoreOriginalCaloHitList", m_restoreOriginalCaloHitList));

    // Output parameters: name of output cluster list and whether it should subsequently be used as the current list
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "ClusterListName", m_clusterListName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "ReplaceCurrentClusterList", m_replaceCurrentClusterList));

    return STATUS_CODE_SUCCESS;
  }

}
