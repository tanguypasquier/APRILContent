/// file ClusterForMerging.h

#ifndef CLUSTERFORMERGING_H 
#define CLUSTERFORMERGING_H 


#include "Pandora/PandoraInputTypes.h"
#include "ArborApi/ArborInputTypes.h"
#include "Managers/PluginManager.h"

namespace arbor_content
{


class ClusterForMerging
{
public:
	ClusterForMerging(const pandora::Cluster* const pCluster);

	float GetMergedHadronicEnergy();

	const pandora::Cluster* GetCluster();

	const ClusterForMerging* GetMotherCluster();
	const std::vector<ClusterForMerging>& GetClustersToMerge();
	const std::vector<ClusterForMerging>& GetNearbyClusters();

	const pandora::CartesianVector& GetAxis();
	const pandora::CartesianVector& GetIntercept();
	const pandora::CartesianVector& GetCentroid();

	bool IsPhoton();

	void SetMotherCluster(const ClusterForMerging* cluster);
	void SetClustersToMerge(const std::vector<ClusterForMerging>& clusterVector);
	void SetNearbyClusters(const std::vector<ClusterForMerging>& clusterVector);

	static void SetPluginManager(const pandora::PluginManager* pPluginManager);

private:
	static const pandora::PluginManager* m_pPluginManager;

	const pandora::Cluster* m_pCluster;

	const ClusterForMerging* m_pMotherCluster;

	std::vector<ClusterForMerging> m_clustersToMerge;
	std::vector<ClusterForMerging> m_nearbyClusters;

	pandora::CartesianVector m_axis;
	pandora::CartesianVector m_intercept;
	pandora::CartesianVector m_centroid;

	bool m_isPhoton;
};

} 

#endif
