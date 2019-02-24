/// \file ClusterForMerging.cc

#include "ArborTopologicalAssociation/ClusterForMerging.h"

#include "Pandora/AlgorithmHeaders.h"
#include "Helpers/ClusterFitHelper.h"

#include "ArborHelpers/SortingHelper.h"
#include "ArborHelpers/CaloHitHelper.h"
#include "ArborHelpers/ReclusterHelper.h"
#include "ArborHelpers/ClusterHelper.h"

namespace arbor_content
{

  const pandora::PluginManager* ClusterForMerging::m_pPluginManager(nullptr);

  void ClusterForMerging::SetPluginManager(const pandora::PluginManager* pPluginManager)
  {
	  m_pPluginManager = pPluginManager;
  }

  ClusterForMerging::ClusterForMerging(const pandora::Cluster* const pCluster)
	  : m_pCluster(pCluster), 
		m_axis(0., 0., 0.), m_intercept(0., 0., 0.), m_centroid(0., 0., 0.)
  {
      pandora::ClusterFitResult clusterFitResult;
      pandora::ClusterFitHelper::FitFullCluster(m_pCluster, clusterFitResult);

      m_axis = clusterFitResult.GetDirection();
      m_intercept = clusterFitResult.GetIntercept();

	  pandora::CartesianVector centroid(0., 0., 0);
	  ClusterHelper::GetCentroid(m_pCluster, centroid);
	  m_centroid = centroid;
	
	  if(m_pPluginManager == nullptr)
	  {
		  throw pandora::StatusCodeException(pandora::STATUS_CODE_NOT_INITIALIZED);
	  }

	  bool isPhoton = m_pPluginManager->GetParticleId()->IsPhoton(m_pCluster);
	  m_isPhoton = isPhoton;
  }

  float ClusterForMerging::GetMergedHadronicEnergy()
  {
	  float hadronicEnergy = 0.;

	  hadronicEnergy += m_pCluster->GetHadronicEnergy();

	  for(ClusterForMerging& cluster : m_clustersToMerge)
	  {
		  hadronicEnergy += cluster.GetMergedHadronicEnergy();
	  }

	  return hadronicEnergy;
  }

  const pandora::Cluster* ClusterForMerging::GetCluster()
  {
	  return m_pCluster;
  }

  const ClusterForMerging* ClusterForMerging::GetMotherCluster()
  {
	  return m_pMotherCluster;
  }

  const std::vector<ClusterForMerging>& ClusterForMerging::GetClustersToMerge()
  {
	  return m_clustersToMerge;
  }

  const std::vector<ClusterForMerging>& ClusterForMerging::GetNearbyClusters()
  {
	  return m_nearbyClusters;
  }

  const pandora::CartesianVector& ClusterForMerging::GetAxis()
  {
	  return m_axis;
  }

  const pandora::CartesianVector& ClusterForMerging::GetIntercept()
  {
	  return m_intercept;
  }

  const pandora::CartesianVector& ClusterForMerging::GetCentroid()
  {
	  return m_centroid;
  }

  bool ClusterForMerging::IsPhoton()
  {
	  return m_isPhoton;
  }

  void ClusterForMerging::SetMotherCluster(const ClusterForMerging* cluster)
  {
	  m_pMotherCluster = cluster;
  }

  void ClusterForMerging::SetClustersToMerge(const std::vector<ClusterForMerging>& clusterVector)
  {
	  m_clustersToMerge = clusterVector;
  }

  void ClusterForMerging::SetNearbyClusters(const std::vector<ClusterForMerging>& clusterVector)
  {
	  m_nearbyClusters = clusterVector;
  }

} 

