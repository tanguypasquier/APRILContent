/// \file ChargedFragmentsMergingAlgorithm.cc
/*
 *
 * ChargedFragmentsMergingAlgorithm.cc source template automatically generated by a class generator
 * Creation date : jeu. avr. 9 2015
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

#include "Pandora/AlgorithmHeaders.h"
#include "PandoraMonitoringApi.h"
#include "PandoraMonitoring.h"

#include "ArborTopologicalAssociation/ChargedFragmentsMergingAlgorithm.h"

#include "ArborHelpers/SortingHelper.h"
#include "ArborHelpers/GeometryHelper.h"
#include "ArborHelpers/ReclusterHelper.h"
#include "ArborHelpers/ClusterHelper.h"
#include "ArborHelpers/HistogramHelper.h"
#include "ArborHelpers/ClusterPropertiesHelper.h"

#include "ArborApi/ArborContentApi.h"

#include "ArborUtility/EventPreparationAlgorithm.h"

#include "ArborTools/TrackDrivenSeedingTool.h"
#include "ArborObjects/CaloHit.h"

#include <algorithm>

#define __USEMCP__ 1

namespace arbor_content
{
  pandora::StatusCode ChargedFragmentsMergingAlgorithm::Run()
  {
    // get candidate clusters for association
	std::vector<ArborCluster*> clusterVector;
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->GetEligibleClusters(clusterVector));

	// get cluster properties
	for(int i = 0; i < clusterVector.size(); ++i)
	{
		auto pCluster = clusterVector.at(i);
		
		pCluster->Reset();

		pandora::CartesianVector centroid(0., 0., 0);
		ClusterHelper::GetCentroid(pCluster, centroid);
		pCluster->SetCentroid(centroid);
		
		const pandora::Cluster* const pandoraClu = dynamic_cast<const pandora::Cluster* const>(pCluster);
		bool isPhoton = PandoraContentApi::GetPlugins(*this)->GetParticleId()->IsPhoton(pandoraClu);

		/// help by MC truth
#if __USEMCP__
		try
		{
			isPhoton = pandora::MCParticleHelper::GetMainMCParticle(pandoraClu)->GetParticleId() == 22;
		}
		catch(pandora::StatusCodeException &)
		{
			std::cout << "MCP issue: " << pandoraClu << std::endl;
		}
#endif

		pCluster->SetPhoton(isPhoton);

		if(m_debugOutput)
		{
			std::cout << " --- cluster : " << pCluster << ", energy: " << pCluster->GetHadronicEnergy() 
			      << ", COG: " << centroid.GetX() << ", " << centroid.GetY() << ", " << centroid.GetZ() << ", isPoton: " << isPhoton << std::endl;
		}

		try
		{
		    pandora::ClusterFitResult clusterFitResult;

			// fit with only connected calo hits; if not successful, with all calo hits.
			if(ClusterHelper::FitFullCluster(pCluster, clusterFitResult) != pandora::STATUS_CODE_SUCCESS)
			{
				std::cout << " ---> fit all hits for cluster " << pCluster << ", E: " << pCluster->GetHadronicEnergy() << std::endl;
				pandora::ClusterFitHelper::FitFullCluster(pCluster, clusterFitResult);
			}

		    const pandora::CartesianVector& cluDirection = clusterFitResult.GetDirection();
		    const pandora::CartesianVector& cluIntercept = clusterFitResult.GetIntercept();
			
			pCluster->SetAxis(cluDirection);
			pCluster->SetIntercept(cluIntercept);
		}
		catch(pandora::StatusCodeException &)
		{
		}

		try
		{
		    pandora::ClusterFitResult clusterFitResult;

			if(pandora::ClusterFitHelper::FitStart(pCluster, 3, clusterFitResult) != pandora::STATUS_CODE_SUCCESS)
			{
				pandora::ClusterFitHelper::FitStart(pCluster, 3, clusterFitResult);
			}
			
			const pandora::CartesianVector& startingPoint = clusterFitResult.GetIntercept();

			pCluster->SetStartingPoint(startingPoint);


			// if cluster has connected with track, the intercept is taken as starting point and
			// the direction is computed from the hits in first 6 layers.
			const pandora::TrackList& associatedTrackList = pCluster->GetAssociatedTrackList();
		    int nAssociatedTracks = associatedTrackList.size();

			// re-compute axis and starting point for charged cluster
			if(nAssociatedTracks > 0)
			{
				pCluster->SetIntercept(startingPoint);

				if(nAssociatedTracks == 1)
				{
					const pandora::Track* associatedTrack = *( associatedTrackList.begin() );
					const pandora::CartesianVector trackDirectionAtCalo = 
						                           associatedTrack->GetTrackStateAtCalorimeter().GetMomentum().GetUnitVector();

					pCluster->SetAxis(trackDirectionAtCalo);
				}
				else
				{
				    if(pCluster->GetOrderedCaloHitList().size()>6)
				    {
				    	pandora::ClusterFitResult clusterFitResultChg;

				    	if(pandora::ClusterFitHelper::FitStart(pCluster, 6, clusterFitResultChg) != pandora::STATUS_CODE_SUCCESS)
				    	{
				    		pandora::ClusterFitHelper::FitStart(pCluster, 6, clusterFitResultChg);
				    	}

		                const pandora::CartesianVector& axis = clusterFitResultChg.GetDirection();
			
				    	pCluster->SetAxis(axis);
				    }
				}
			}
		}
		catch(pandora::StatusCodeException &)
		{
		}

		try
		{
		    pandora::ClusterFitResult clusterFitResult;
			pandora::ClusterFitHelper::FitEnd(pCluster, 3, clusterFitResult);
		    const pandora::CartesianVector& endpoint = clusterFitResult.GetIntercept();

			pCluster->SetEndpoint(endpoint);
		}
		catch(pandora::StatusCodeException &)
		{
		}
	}

	m_clustersToMerge.clear();

	for(int i = 0; i < clusterVector.size(); ++i)
	{
		auto cluster = clusterVector.at(i);

		// include a cluster in the vector even if it is identified as photon
		if( (cluster->GetAssociatedTrackList().size() == 0) )
		{
			m_clustersToMerge.push_back(cluster);
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::vector<pandora::CartesianVector> m_clusterCentroids;

	for(auto clu : m_clustersToMerge)
	{
		auto& centroid = clu->GetCentroid();
		m_clusterCentroids.push_back(centroid);
	}

	CaloHitRangeSearchHelper::FillMatixByPoints(m_clusterCentroids, m_clusterCentroidsMatrix);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// search nearby clusters along track
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const pandora::TrackList *pTrackList = nullptr;
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pTrackList));

	// set the cluster with track as root cluster
	for(auto track : *pTrackList)
	{
		if( !(track->HasAssociatedCluster()) ) continue;

		auto clu = track->GetAssociatedCluster();
		auto associatedCluster = ArborContentApi::Modifiable(dynamic_cast<const arbor_content::ArborCluster*>(clu));
		associatedCluster->SetRoot();
	}

	//
    if(m_mergeChargedClusters)
	{
		for(auto track : *pTrackList)
	    {
	    	// Reset the cluster for search
	        for(int i = 0; i < clusterVector.size(); ++i)
	    	{
	    		auto clu = clusterVector.at(i);
	    		clu->SetHasMotherAtSearch(false);
	    	}

	    	if( !(track->HasAssociatedCluster()) ) continue;

	    	auto clu = track->GetAssociatedCluster();
	    	auto associatedCluster = ArborContentApi::Modifiable(dynamic_cast<const arbor_content::ArborCluster*>(clu));

		    if(m_debugOutput)
			{
				std::cout << "     ---> SearchProperClusters from starting cluster: " << clu 
	    		          << ", track E: " << track->GetEnergyAtDca() << std::endl;
			}

	    	std::vector<ArborCluster*> properClusters;
	    	SearchProperClusters(track, associatedCluster, properClusters);
	    }
	}

	// clean clusters
	CleanClusterForMerging(clusterVector);
	
    return pandora::STATUS_CODE_SUCCESS;
  }

  void ChargedFragmentsMergingAlgorithm::SearchProperClusters(const pandora::Track* /* pTrack */, ArborCluster* startingCluster, 
		  std::vector<arbor_content::ArborCluster*>& properClusters)
  {
	  if(m_debugOutput)
	  {
		  const pandora::Cluster* const pandoraTrackStartClu = dynamic_cast<const pandora::Cluster* const>(startingCluster);
	      float startCluEnergy = startingCluster->GetHadronicEnergy();

	      auto pClusterMCParticle = pandora::MCParticleHelper::GetMainMCParticle(pandoraTrackStartClu);
	      std::cout << " SearchProperClusters for charged cluster: " << startingCluster << ", Ehad: " << startCluEnergy << ", MCP: " << pClusterMCParticle << std::endl;
	  }

	  std::vector<arbor_content::ArborCluster*> nearbyClusters;
	  GetNearbyClusters(startingCluster, m_clustersToMerge, nearbyClusters);

	  // map for sorting all nearby clusters by closest distance
	  std::multimap<float, ArborCluster*> clusterDistanceMap;

	  for(int i = 0; i < nearbyClusters.size(); ++i)
	  {
		  auto nearbyCluster = nearbyClusters.at(i);

		  if(nearbyCluster->HasMotherAtSearch() || nearbyCluster == startingCluster || nearbyCluster->IsRoot() || nearbyCluster->IsPhoton()) 
		  {
			  continue;
		  }

		  if(m_debugOutput)
		  {
			  std::cout << "nearbyClusters " << i << " : " << nearbyCluster << ", E: " << nearbyCluster->GetHadronicEnergy() << std::endl;
		  }

		  // GetClustersDistance
		  float closestDistance = 1.e6;

		  try
		  {
			  ClusterHelper::GetClosestDistanceApproach(startingCluster, nearbyCluster, closestDistance, m_onlyUseConnectedHits);
		  }
          catch(pandora::StatusCodeException &)
		  {
			  std::cout << "GetClosestDistanceApproach failed" << std::endl;
		  }

#if __USEMCP__
		  // help by MC truth
		  try
		  {
			  const pandora::Cluster* const pandoraClu = dynamic_cast<const pandora::Cluster* const>(nearbyCluster);
		      auto pandoraCluMCP = pandora::MCParticleHelper::GetMainMCParticle(pandoraClu);

		      if( pandora::PdgTable::GetParticleCharge(pandoraCluMCP->GetParticleId()) == 0. && 
		          nearbyCluster->GetHadronicEnergy() > 0. )
		      {
		        		continue;
		      }
		  }
		  catch(pandora::StatusCodeException &)
		  {
		  }
#endif

		  float angle = 1.e6;
		  float axisDistance = 1.e6;

		  try
		  {
			  angle = GetClusterAxisStartingPointAngle(nearbyCluster);
			  axisDistance = GetClusterAxesDistance(startingCluster, nearbyCluster); 
		  }
		  catch(...)
		  {
			  std::cout << "    === Axis error, cluster " << nearbyCluster << ", E: " << nearbyCluster->GetHadronicEnergy() << std::endl;
		  }

		  // FIXME
		  bool isMergingCandidate = 
				  closestDistance < m_maxClosestClusterDistance ||  // very close cluster
			      ( angle > 0.8 && axisDistance < 100. );           // clusters with compatible axes

		  std::cout << " @_@ Check clusters @_@: " << std::endl
		      << "  startingCluster: " << startingCluster << ", E: " << startingCluster->GetHadronicEnergy() << std::endl
		      << "  nearbyCluster: " << nearbyCluster << ", E: " << nearbyCluster->GetHadronicEnergy() << std::endl
		      << "  closestDistance: " << closestDistance << ", angle: " << angle << ", axisDistance: " << axisDistance << std::endl;

		  if(isMergingCandidate) 
		  {
		      std::vector<float> clusterParameters;

		      clusterParameters.push_back(closestDistance);
		      clusterParameters.push_back(angle); // axis angle
		      clusterParameters.push_back(axisDistance);

		      std::vector<float> parameterPowers;
		      parameterPowers.push_back(5.);
		      parameterPowers.push_back(3.);
		      parameterPowers.push_back(1.);

		      ClustersOrderParameter orderParameter(clusterParameters, parameterPowers);
		      nearbyCluster->SetOrderParameterWithMother(startingCluster, orderParameter);

		      clusterDistanceMap.insert( std::pair<float, ArborCluster*>(closestDistance, nearbyCluster) );
		  }
		  else
		  {
			  std::cout << "    --- Not a merging cadidate" << std::endl;
		  }

		  std::cout << "-----------------------------------------------------------------------------------------" << std::endl;
      }
		  
	  for(auto it = clusterDistanceMap.begin(); it != clusterDistanceMap.end(); ++it)
	  {
		  auto nearbyCluster = it->second;

		  properClusters.push_back(nearbyCluster);
		  nearbyCluster->SetHasMotherAtSearch();
	  }

	  startingCluster->SetClustersToMerge(properClusters);
		
	  if(m_debugOutput)
	  {
		  std::cout << "-----------------------------------------------------------------------------------------------------------" << std::endl;
	  }
  }

  float ChargedFragmentsMergingAlgorithm::GetClusterAxisStartingPointAngle(const ArborCluster* pCluster)
  {
	  auto& clusterAxis = pCluster->GetAxis();
	  auto& clusterStartingPoint = pCluster->GetStartingPoint();
	  float angleAxisStartingPoint = clusterAxis.GetOpeningAngle(clusterStartingPoint);

	  return angleAxisStartingPoint;
  }

  float ChargedFragmentsMergingAlgorithm::GetClusterAxesDistance(const ArborCluster* pCluster1, const ArborCluster* pCluster2)
  {
#if 0
	  const float bField(PandoraContentApi::GetPlugins(*this)->GetBFieldPlugin()->GetBField( pandora::CartesianVector(0.f, 0.f, 0.f)));

	  const pandora::Helix helix(pTrack->GetTrackStateAtCalorimeter().GetPosition(),
	    	  pTrack->GetTrackStateAtCalorimeter().GetMomentum(), pTrack->GetCharge(), bField);
	
	  pandora::CartesianVector trackCluCentroidDistanceVec(0., 0., 0.);
	  float genericTime = 0.;

	  if(pandora::STATUS_CODE_SUCCESS != helix.GetDistanceToPoint(nearbyClusterCOG, trackCluCentroidDistanceVec, genericTime))
	  {
		std::cout << "helix.GetDistanceToPoint failed" << std::endl;
	  	continue;
	  }
#endif

	  auto& cluster1Axis = pCluster1->GetAxis();
	  auto& cluster2Axis = pCluster2->GetAxis();

	  // FIXME: intercept or starting point ?
	  auto& cog1 = pCluster1->GetCentroid();
	  auto& cog2 = pCluster2->GetCentroid();
	  auto directionOfCentroids = cog1 - cog2;

	  // redefition
	  auto d1 = cluster1Axis;
	  auto d2 = cluster2Axis;

	  auto x1 = cog1;
	  auto x2 = cog2;
	
	  // 
	  auto n1 = d1.GetCrossProduct(d2.GetCrossProduct(d1));
	  auto n2 = d2.GetCrossProduct(d1.GetCrossProduct(d2));

	  // the nearest points
	  //auto Np1 = x1 + ( (x2-x1).GetDotProduct(n2)/(d1.GetDotProduct(n2)) )*d1;
	  auto Np1 = d1;
	  Np1 *= (x2-x1).GetDotProduct(n2)/(d1.GetDotProduct(n2));
	  Np1 += x1;

	  //auto Np2 = x2 + (x1-x2).GetDotProduct(n1)/(d2.GetDotProduct(n1))*d2;
	  auto Np2 = d2;
	  Np2 *= (x1-x2).GetDotProduct(n1)/(d2.GetDotProduct(n1));
	  Np2 += x2;

	  float distCOGNp1 = (x1 - Np1).GetMagnitude();
	  float distCOGNp2 = (x2 - Np2).GetMagnitude();

	  std::cout << "       === GetClusterAxesDistance === " << std::endl
		        << " E1: " << pCluster1->GetHadronicEnergy() << ", E2: " << pCluster2->GetHadronicEnergy() << std::endl
		        << " cluster1Axis: " << cluster1Axis.GetX() << ", " << cluster1Axis.GetY() << ", " << cluster1Axis.GetZ() << std::endl
		        << " cluster2Axis: " << cluster2Axis.GetX() << ", " << cluster2Axis.GetY() << ", " << cluster2Axis.GetZ() << std::endl
				<< " COG1: " << cog1.GetX() << ", " << cog1.GetY() << ", " << cog1.GetZ() << std::endl
				<< " COG2: " << cog2.GetX() << ", " << cog2.GetY() << ", " << cog2.GetZ() << std::endl
				<< " d_cog: " << directionOfCentroids.GetMagnitude() << std::endl
				<< " Np1: " << Np1.GetX() << ", " << Np1.GetY() << ", " << Np1.GetZ() << ", distToCOG: " << distCOGNp1 << std::endl
				<< " Np2: " << Np2.GetX() << ", " << Np2.GetY() << ", " << Np2.GetZ() << ", distToCOG: " << distCOGNp2 << std::endl
				<< std::endl;


#if 0
	  if( directionOfCentroids.GetMagnitudeSquared() * pCluster1->GetMagnitudeSquared() > std::numeric_limits<float>::epsilon() )
	  {
		  try
		  {
			  angle = directionOfCentroids.GetOpeningAngle(startingClusterAxis);
		  }
		  catch(pandora::StatusCodeException &)
		  {
			  std::cout << "GetOpeningAngle failed" << std::endl;
		  }
	  }
#endif

	  auto directionsCrossProd = cluster2Axis.GetCrossProduct(cluster1Axis);
	  float axisDistance = fabs(directionsCrossProd.GetDotProduct(directionOfCentroids)) / directionsCrossProd.GetMagnitude();

	  return axisDistance;
  }

  void ChargedFragmentsMergingAlgorithm::GetNearbyClusters(pandora::Cluster* cluster, 
		  const std::vector<arbor_content::ArborCluster*>& clusterVector, std::vector<arbor_content::ArborCluster*>& clustersInRange)
  {
      pandora::CartesianVector centroid(0., 0., 0.);
	  ClusterHelper::GetCentroid(cluster, centroid);

	  const float distance = m_maxStartingClusterDistance;
	  const mlpack::math::Range range(0., distance);

      arma::mat testPoint(3, 1);
	  testPoint.col(0)[0] = centroid.GetX();
	  testPoint.col(0)[1] = centroid.GetY();
	  testPoint.col(0)[2] = centroid.GetZ();
	  
	  mlpack::range::RangeSearch<> rangeSearch(m_clusterCentroidsMatrix);
      std::vector<std::vector<size_t> > resultingNeighbors;
      std::vector<std::vector<double> > resultingDistances;
      rangeSearch.Search(testPoint, range, resultingNeighbors, resultingDistances);

      std::vector<size_t>& neighbors = resultingNeighbors.at(0);
      std::vector<double>& distances = resultingDistances.at(0);
	  
      for(size_t j=0; j < neighbors.size(); ++j)
      {
      	size_t neighbor = neighbors.at(j);
      	//double hitsDist = distances.at(j);

		clustersInRange.push_back( clusterVector.at(neighbor) );
	  }

	  // sort cluster by distance
	  std::multimap<float, ArborCluster*> clusterDistanceMap;

	  for(int i = 0; i < clustersInRange.size(); ++i)
	  {
		  auto clusterInRange = clustersInRange.at(i);

		  clusterDistanceMap.insert( std::pair<float, ArborCluster*>(distances.at(i), clusterInRange) );
	  }

	  clustersInRange.clear();

	  for(auto& mapIter : clusterDistanceMap)
	  {
		  auto clu = mapIter.second;
		  clustersInRange.push_back(clu);
	  }

	  ///////////////////////////////////////////////////////////////////////////////////////////////
		
#if 0
	  auto pClusterMCP = pandora::MCParticleHelper::GetMainMCParticle(cluster);

	  std::cout << "------------ cluster: " << cluster << ", energy: " << cluster->GetHadronicEnergy() 
		  << ", MCP: " << pClusterMCP << ", nearby clusters: " << clustersInRange.size() << std::endl;

	  for(auto it = clusterDistanceMap.begin(); it != clusterDistanceMap.end(); ++it)
	  {
		  //auto pCluster = clustersInRange.at(i);
		  auto distance = it->first;
		  auto pCluster = it->second;


		  const pandora::Cluster* const clu = dynamic_cast<const pandora::Cluster* const>(pCluster);
		  bool isPhoton = PandoraContentApi::GetPlugins(*this)->GetParticleId()->IsPhoton(clu);
		  auto pClusterMCParticle = pandora::MCParticleHelper::GetMainMCParticle(clu);

		  std::cout << " *clu: " << clu << ", dist: " << distance << ", nhits: " << 
			 pCluster->GetNCaloHits() << ", Ehad: " << pCluster->GetHadronicEnergy() 
			 << ", iL: " << pCluster->GetInnerPseudoLayer() << ": isPhoton: " << pCluster->IsPhoton() 
			 << ": MCP: " << pClusterMCParticle << std::endl;
	  }
#endif
  }

  pandora::StatusCode ChargedFragmentsMergingAlgorithm::CleanClusterForMerging(std::vector<ArborCluster*>& clusterVector)
  {
	for(int i = 0; i < clusterVector.size(); ++i)
	{
		auto& cluster = clusterVector.at(i);

		auto& mothers = cluster->GetMotherCluster();

		if(m_debugOutput)
		{
			std::cout << " --- cluster " << cluster << " mothers: " << mothers.size() << ", root?: " << cluster->IsRoot() << std::endl;
		}

		// find the best one
		ClustersOrderParameter bestOrderParameter;
		ArborCluster* bestCluster;

		for(int iMother = 0; iMother < mothers.size(); ++iMother)
		{
			auto mother = mothers.at(iMother);
			ClustersOrderParameter orderParameter = cluster->GetOrderParameterWithMother(mother);

			if(orderParameter < bestOrderParameter)
			{
				bestOrderParameter = orderParameter;
				bestCluster = mother;
			}
		}

		// take the best one
		if(mothers.size() > 1)
		{
			for(int iMother = 0; iMother < mothers.size(); ++iMother)
			{
				auto mother = mothers.at(iMother);

				if(mother != bestCluster)
				{
					if(m_debugOutput)
					{
						std::cout << " !!! cluster: " << mother << " remove cluster to merge: " << cluster << std::endl;
					}

					mother->RemoveFromClustersToMerge(cluster);
				}
			}

			mothers.clear();
			mothers.push_back(bestCluster);
		}
	}

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode ChargedFragmentsMergingAlgorithm::GetEligibleClusters(std::vector<ArborCluster*>& clusterVector) const
  {
	clusterVector.clear();

    const pandora::ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

	//std::cout << ">>>cluster number: " << pClusterList->size() << std::endl;

    if(pClusterList->empty())
      return pandora::STATUS_CODE_SUCCESS;

    for(pandora::ClusterList::const_iterator clusterIter = pClusterList->begin(), clusterEndIter = pClusterList->end() ;
        clusterEndIter != clusterIter ; ++clusterIter)
    {
      const pandora::Cluster *const pCluster = *clusterIter;

      //if(!this->CanMergeCluster(pCluster))
      //  continue;

	  auto arborCluster = ArborContentApi::Modifiable(dynamic_cast<const arbor_content::ArborCluster*>(pCluster));
      clusterVector.push_back(arborCluster);
    }

    // sort them by inner layer
    std::sort(clusterVector.begin(), clusterVector.end(), SortingHelper::SortClustersByInnerLayer);

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode ChargedFragmentsMergingAlgorithm::ReadSettings(const pandora::TiXmlHandle xmlHandle)
  {
    m_maxStartingClusterDistance = 1000.;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "MaxStartingClusterDistance", m_maxStartingClusterDistance));

	m_maxClosestClusterDistance = 500.;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "MaxClosestClusterDistance", m_maxClosestClusterDistance));

	m_mergeChargedClusters = true ;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "MergeChargedClusters", m_mergeChargedClusters));

	m_debugOutput = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "DebugOutput", m_debugOutput));

    return pandora::STATUS_CODE_SUCCESS;
  }

} 

