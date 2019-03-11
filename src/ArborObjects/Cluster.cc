/*
 *
 * ArborCluster.cc source template automatically generated by a class generator
 * Creation date : lun. mars 30 2015
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


#include "ArborObjects/Cluster.h"

namespace arbor_content
{
  ArborCluster::ArborCluster(const PandoraContentApi::Cluster::Parameters &parameters) :
      pandora::Cluster(parameters),
	  m_axis(0., 0., 0.), m_intercept(0., 0., 0.), m_centroid(0., 0., 0.),
	  m_startingPoint(0., 0., 0.), m_endpoint(0., 0., 0.)
  {
  }

  ArborCluster::~ArborCluster()
  {
  }

  float ArborCluster::GetMergedHadronicEnergy()
  {
	  float hadronicEnergy = 0.;

	  hadronicEnergy += GetHadronicEnergy();

	  for(auto& cluster : m_clustersToMerge)
	  {
		  hadronicEnergy += cluster->GetMergedHadronicEnergy();
	  }

	  return hadronicEnergy;
  }

  const std::vector<ArborCluster*>& ArborCluster::GetMotherCluster() const
  {
	  return m_motherCluster;
  }

  const std::vector<ArborCluster*>& ArborCluster::GetClustersToMerge() const
  {
	  return m_clustersToMerge;
  }

  void ArborCluster::GetAllClustersToMerge(std::vector<ArborCluster*>& allClustersToMerge) const
  {
	  auto& clusters = GetClustersToMerge();

	  for(int iClu = 0; iClu < clusters.size(); ++iClu)
	  {
		  auto& clu = clusters.at(iClu);
		  if(clu->GetMotherCluster().size() > 1) 
		  {
			  std::cout << " ------ merging issue: " << clu << ", E: " << clu->GetHadronicEnergy() 
				  << ", mothers: " << clu->GetMotherCluster().size() << std::endl;

			  auto& mothers = clu->GetMotherCluster();
			  for(auto& mother : mothers)
			  {
				  std::cout << "   --- mother: " << mother << std::endl;
			  }

			  continue;
		  }

		  clu->GetAllClustersToMerge(allClustersToMerge);
		  allClustersToMerge.push_back(clu);
	  }
  }

  const std::vector<ArborCluster*>& ArborCluster::GetNearbyClusters() const
  {
	  return m_nearbyClusters;
  }

  const pandora::CartesianVector& ArborCluster::GetAxis() const
  {
	  return m_axis;
  }

  const pandora::CartesianVector& ArborCluster::GetIntercept() const
  {
	  return m_intercept;
  }

  const pandora::CartesianVector& ArborCluster::GetCentroid() const
  {
	  return m_centroid;
  }

  const pandora::CartesianVector& ArborCluster::GetStartingPoint() const
  {
	  return m_startingPoint;
  }

  const pandora::CartesianVector& ArborCluster::GetEndpoint() const
  {
	  return m_endpoint;
  }

  bool ArborCluster::IsDaughter(ArborCluster* cluster)
  {
	  bool isDaughter = false;
	  
	  auto& mothers = GetMotherCluster();

      if(mothers.end() != std::find(mothers.begin(), mothers.end(), cluster))
      {
          isDaughter = true;
      }
	  else
	  {
		  for(auto motherCluster : mothers)
		  {
			  if(motherCluster->IsDaughter(cluster))
			  {
				  isDaughter = true;
				  break;
			  }
		  }
	  }

	  return isDaughter;
  }

  bool ArborCluster::IsPhoton()
  {
	  return m_isPhoton;
  }

  void ArborCluster::SetMotherCluster(ArborCluster* cluster)
  {
	  m_motherCluster.push_back(cluster);
  }

  void ArborCluster::SetClustersToMerge(const std::vector<ArborCluster*>& clusterVector)
  {
	  std::vector<ArborCluster*> clustersToMerge;

	  for(int i = 0; i < clusterVector.size(); ++i)
	  {
		  auto cluster = clusterVector.at(i);

		  if(!cluster->IsDaughter(this))
		  {
			  cluster->SetMotherCluster(this);
			  clustersToMerge.push_back(cluster);
		  }
	  }

	  m_clustersToMerge = clustersToMerge;
  }

  void ArborCluster::SetNearbyClusters(const std::vector<ArborCluster*>& clusterVector)
  {
	  m_nearbyClusters = clusterVector;
  }

  void ArborCluster::SetAxis(pandora::CartesianVector axis)
  {
	  m_axis = axis;
  }

  void ArborCluster::SetIntercept(pandora::CartesianVector intercept)
  {
	  m_intercept = intercept;
  }

  void ArborCluster::SetCentroid(pandora::CartesianVector centroid)
  {
	  m_centroid = centroid;
  }

  void ArborCluster::SetStartingPoint(pandora::CartesianVector startingPoint)
  {
	  m_startingPoint = startingPoint;
  }

  void ArborCluster::SetEndpoint(pandora::CartesianVector endpoint)
  {
	  m_endpoint = endpoint;
  }
	
  void ArborCluster::SetPhoton(bool isPhoton)
  {
	  m_isPhoton = isPhoton;
  }

} 

