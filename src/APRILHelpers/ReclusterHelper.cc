/// \file ReclusterHelper.cc
/*
 *
 * ReclusterHelper.cc source template automatically generated by a class generator
 * Creation date : jeu. avr. 9 2015
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

#include "Pandora/AlgorithmHeaders.h"

#include "APRILHelpers/ReclusterHelper.h"
#include "APRILHelpers/ClusterHelper.h"
#include "APRILHelpers/CaloHitHelper.h"

//#define __DEBUG__

namespace april_content
{

  float ReclusterHelper::GetTrackClusterCompatibility(const pandora::Pandora &pandora, const pandora::Cluster *const pCluster, const pandora::TrackList &trackList)
  {
    float trackEnergySum(0.);

    for (pandora::TrackList::const_iterator trackIter = trackList.begin(), trackIterEnd = trackList.end(); trackIter != trackIterEnd; ++trackIter)
      trackEnergySum += (*trackIter)->GetEnergyAtDca();

	// FIXME
	// suppose resolutions are not the same for ECAL and HCAL; currently assume 0.3 for ECAL.
    const float hadronicEnergyResolutionECAL = 0.3;
    const float hadronicEnergyResolutionHCAL(pandora.GetSettings()->GetHadronicEnergyResolution());

    if ((trackEnergySum < std::numeric_limits<float>::epsilon()) || (hadronicEnergyResolutionHCAL < std::numeric_limits<float>::epsilon()))
      throw pandora::StatusCodeException(pandora::STATUS_CODE_FAILURE);

	const float comparisonEnergy = pCluster->GetTrackComparisonEnergy(pandora);

	float hadronicEnergyInECAL = ClusterHelper::GetHadronicEnergyInECAL(pCluster);
	float hadronicEnergyInHCAL = trackEnergySum - hadronicEnergyInECAL;

	float hadronicEnergyResolution;

	// FIXME
	if(hadronicEnergyInECAL > 0.01 && hadronicEnergyInHCAL/hadronicEnergyInECAL < 0.4)
	{
		hadronicEnergyResolution = hadronicEnergyResolutionECAL;
	}
	else
	{
		hadronicEnergyResolution = hadronicEnergyResolutionHCAL;
	}

    const float sigmaE( hadronicEnergyResolution * std::sqrt(trackEnergySum));
    const float chi((comparisonEnergy - trackEnergySum)/sigmaE);

#ifdef __DEBUG__
	std::cout << "SigmaE: " << hadronicEnergyResolution << ", trackEnergySum: " << trackEnergySum  
		      << ", comparisonEnergy: " << comparisonEnergy << ", chi: " << chi << std::endl;
#endif

    return chi;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  float ReclusterHelper::GetTrackClusterCompatibility(const pandora::Pandora &pandora, const pandora::Cluster *const pClusterToEnlarge, const pandora::Cluster *const pClusterToMerge, const pandora::TrackList &trackList)
  {
    float trackEnergySum(0.);

    for (pandora::TrackList::const_iterator trackIter = trackList.begin(), trackIterEnd = trackList.end(); trackIter != trackIterEnd; ++trackIter)
      trackEnergySum += (*trackIter)->GetEnergyAtDca();

	// FIXME
	// suppose resolutions are not the same for ECAL and HCAL; currently assume 0.3 for ECAL.
    const float hadronicEnergyResolutionECAL = 0.3;
    const float hadronicEnergyResolutionHCAL(pandora.GetSettings()->GetHadronicEnergyResolution());

    if ((trackEnergySum < std::numeric_limits<float>::epsilon()) || (hadronicEnergyResolutionHCAL < std::numeric_limits<float>::epsilon()))
      throw pandora::StatusCodeException(pandora::STATUS_CODE_FAILURE);

	const float comparisonEnergy = pClusterToEnlarge->GetTrackComparisonEnergy(pandora) + 
		pClusterToMerge->GetTrackComparisonEnergy(pandora);

	float hadronicEnergyInECAL = ClusterHelper::GetHadronicEnergyInECAL(pClusterToEnlarge) 
		                         + ClusterHelper::GetHadronicEnergyInECAL(pClusterToMerge);

	float hadronicEnergyInHCAL = trackEnergySum - hadronicEnergyInECAL;

	float hadronicEnergyResolution;

	// FIXME
	if(hadronicEnergyInECAL > 0.01 && hadronicEnergyInHCAL/hadronicEnergyInECAL < 0.4)
	{
		hadronicEnergyResolution = hadronicEnergyResolutionECAL;
	}
	else
	{
		hadronicEnergyResolution = hadronicEnergyResolutionHCAL;
	}

    const float sigmaE( hadronicEnergyResolution * std::sqrt(trackEnergySum));
    const float chi((comparisonEnergy - trackEnergySum)/sigmaE);

#ifdef __DEBUG__
	std::cout << "SigmaE: " << hadronicEnergyResolution << ", trackEnergySum: " << trackEnergySum  
		      << ", comparisonEnergy: " << comparisonEnergy << ", chi: " << chi << std::endl;
#endif

    return chi;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  float ReclusterHelper::GetTrackClusterCompatibility(const pandora::Pandora &pandora, const float clusterEnergy, const float trackEnergy)
  {
    const float hadronicEnergyResolution(pandora.GetSettings()->GetHadronicEnergyResolution());

    if ((trackEnergy < std::numeric_limits<float>::epsilon()) || (hadronicEnergyResolution < std::numeric_limits<float>::epsilon()))
      throw pandora::StatusCodeException(pandora::STATUS_CODE_FAILURE);

    const float sigmaE(hadronicEnergyResolution * trackEnergy / std::sqrt(trackEnergy));
    const float chi((clusterEnergy - trackEnergy)/sigmaE);

    return chi;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode ReclusterHelper::ExtractReclusterResults(const pandora::Pandora &pandora, const pandora::ClusterList* clusterList, ReclusterResult &reclusterResult)
  {
    if(clusterList == nullptr || clusterList->empty())
      return pandora::STATUS_CODE_INVALID_PARAMETER;

    unsigned int nDof(0);
    float chi(0.f), chi2(0.f), neutralEnergy(0.f), chargedEnergy(0.f), chiWorstAssociation(0.f);

    for(pandora::ClusterList::const_iterator iter = clusterList->begin(), endIter = clusterList->end() ; endIter != iter ; ++iter)
    {
      const pandora::Cluster *const pCluster = *iter;

      const float clusterEnergy(pCluster->GetCorrectedHadronicEnergy(pandora));
      const pandora::TrackList &trackList(pCluster->GetAssociatedTrackList());
      const unsigned int nTracks(trackList.size());

      if(nTracks == 0)
      {
        neutralEnergy += clusterEnergy;
        continue;
      }

      chargedEnergy += clusterEnergy;

      const float newChi(ReclusterHelper::GetTrackClusterCompatibility(pandora, pCluster, trackList));

      chi += newChi;
      chi2 += newChi * newChi;

      ++nDof;

      if( (newChi * newChi) > chiWorstAssociation*chiWorstAssociation )
        chiWorstAssociation = newChi;
    }

    if(0 == nDof)
      return pandora::STATUS_CODE_FAILURE;

    reclusterResult.SetChi(chi);
    reclusterResult.SetChi2(chi2);
    reclusterResult.SetChiPerDof(chi / nDof);
    reclusterResult.SetChi2PerDof(chi2 / nDof);
    reclusterResult.SetNeutralEnergy(neutralEnergy);
    reclusterResult.SetChargedEnergy(chargedEnergy);
    reclusterResult.SetChiWorstAssociation(chiWorstAssociation);

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode ReclusterHelper::SplitTreeFromCluster(const pandora::Algorithm &algorithm, const april_content::CaloHit *const pSeedCaloHit,
      const pandora::Cluster *const pOriginalCluster, const pandora::Cluster *&pSeparatedTreeCluster, const std::string &originalClusterListName)
  {
    if(NULL == pOriginalCluster || NULL == pSeedCaloHit)
      return pandora::STATUS_CODE_INVALID_PARAMETER;

    if(!APRILContentApi::IsSeed(pSeedCaloHit))
      return pandora::STATUS_CODE_INVALID_PARAMETER;

    pandora::CaloHitList clusterCaloHitList;
    pandora::CaloHitList seedCaloHitList;
    pOriginalCluster->GetOrderedCaloHitList().FillCaloHitList(clusterCaloHitList);

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, CaloHitHelper::ExtractSeedCaloHitList(&clusterCaloHitList, seedCaloHitList, false));

    // if only one seed in the original cluster, can't create a separate tree from this cluster
    if(seedCaloHitList.size() <= 1)
      return pandora::STATUS_CODE_NOT_ALLOWED;

    // seed calo hit must be contained in the original cluster
    //if(seedCaloHitList.end() == seedCaloHitList.find(pSeedCaloHit))
    pandora::CaloHitList::iterator listIter = std::find(seedCaloHitList.begin(), seedCaloHitList.end(), pSeedCaloHit);
	if(seedCaloHitList.end() == listIter )
      return pandora::STATUS_CODE_FAILURE;

    pandora::CaloHitList treeCaloHitList;
    treeCaloHitList.push_back(pSeedCaloHit);
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, CaloHitHelper::BuildCaloHitList(pSeedCaloHit, FORWARD_DIRECTION, treeCaloHitList));

    // remove all the hits from the original cluster
    for(pandora::CaloHitList::const_iterator iter = treeCaloHitList.begin(), endIter = treeCaloHitList.end() ;
        endIter != iter ; ++iter)
    {
      PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::RemoveFromCluster(algorithm, pOriginalCluster, *iter));
    }

    // create the separated tree cluster
    object_creation::ClusterParameters clusterParameters;
    clusterParameters.m_caloHitList = treeCaloHitList;
    pSeparatedTreeCluster = NULL;

    std::string clusterListName;
    std::string temporaryClusterListName;
    const pandora::ClusterList *pTemporaryClusterList = NULL;

    // in order to create a separate tree cluster, cluster creation must be enabled by pandora sdk first
    if(originalClusterListName.empty())
    {
      PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentListName<pandora::Cluster>(algorithm, clusterListName));
    }
    else
    {
      clusterListName = originalClusterListName;
    }

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::CreateTemporaryListAndSetCurrent(algorithm, pTemporaryClusterList, temporaryClusterListName));

    // create the tree cluster and save it
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(algorithm, clusterParameters, pSeparatedTreeCluster));
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveList<pandora::Cluster>(algorithm, temporaryClusterListName, clusterListName));

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode ReclusterHelper::SplitClusterIntoTreeClusters(const pandora::Algorithm &algorithm, const pandora::Cluster *const pCluster,
      pandora::ClusterVector &treeClusterVector)
  {
    if(NULL == pCluster)
      return pandora::STATUS_CODE_INVALID_PARAMETER;

    pandora::CaloHitList clusterCaloHitList;
    pandora::CaloHitList seedCaloHitList;
    pCluster->GetOrderedCaloHitList().FillCaloHitList(clusterCaloHitList);

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, CaloHitHelper::ExtractSeedCaloHitList(&clusterCaloHitList, seedCaloHitList, false));

    // if only one seed in the original cluster, can't create a separate tree from this cluster
    if(seedCaloHitList.size() <= 1)
      return pandora::STATUS_CODE_NOT_ALLOWED;

    pandora::ClusterList reclusterClusterList;
    reclusterClusterList.push_back(pCluster);

    std::string originalClusterListName;
    std::string fragmentsClustersListName;

    // fragment the cluster
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::InitializeFragmentation(algorithm, reclusterClusterList, originalClusterListName, fragmentsClustersListName));

    for(pandora::CaloHitList::const_iterator iter = seedCaloHitList.begin(), endIter = seedCaloHitList.end() ;
        endIter != iter ; ++iter)
    {
      const april_content::CaloHit *const pCaloHit = dynamic_cast<const april_content::CaloHit *const>(*iter);

      if(NULL == pCaloHit)
        return pandora::STATUS_CODE_FAILURE;

      pandora::CaloHitList treeCaloHitList;
      treeCaloHitList.push_back(pCaloHit);
      PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, CaloHitHelper::BuildCaloHitList(pCaloHit, FORWARD_DIRECTION, treeCaloHitList));

      // create a separated tree cluster
      object_creation::ClusterParameters clusterParameters;
      clusterParameters.m_caloHitList = treeCaloHitList;
      const pandora::Cluster *pTreeCluster = NULL;

      PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(algorithm, clusterParameters, pTreeCluster));

      treeClusterVector.push_back(pTreeCluster);
    }

    // save fragmented clusters
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::EndFragmentation(algorithm, fragmentsClustersListName, originalClusterListName));

    return pandora::STATUS_CODE_SUCCESS;
  }


} 

