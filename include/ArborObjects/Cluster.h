/*
 *
 * Cluster.h header template automatically generated by a class generator
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


#ifndef ARBORCLUSTER_H
#define ARBORCLUSTER_H

#include "Objects/Cluster.h"

#include "Api/PandoraApi.h"
#include "Api/PandoraContentApi.h"


#include "Pandora/PandoraInputTypes.h"
#include "ArborApi/ArborInputTypes.h"

#include "ArborApi/ArborContentApi.h"

//#include "ArborObjects/ArborMetaData.h"

namespace arbor_content
{

class ClusterFactory;

class ClustersOrderParameter
{
public:
	ClustersOrderParameter()
	: m_orderParameter(std::numeric_limits<float>::max())
	{
	}

	ClustersOrderParameter(std::vector<float> parameters, std::vector<float> powers) 
	: m_parameters(parameters), m_powers(powers)
	{
		if(m_parameters.size() != m_powers.size())
		{
			std::cout << "parameters and powers length are not consistent!" << std::endl;
            throw pandora::StatusCodeException(pandora::STATUS_CODE_FAILURE);
		}

		m_orderParameter = 1.;

		for(int i = 0; i < m_parameters.size(); ++i)
		{
			m_orderParameter *= std::pow(m_parameters.at(i), m_powers.at(i));
		}
	}

	bool operator<(const ClustersOrderParameter& a) const
	{
		return m_orderParameter < a.m_orderParameter;
	}

	std::vector<float>        m_parameters;        
	std::vector<float>        m_powers;        

	float                     m_orderParameter;
};

/** 
 *  @brief  Cluster class
 */ 
class ArborCluster : public pandora::Cluster
{
public:
	float GetMergedHadronicEnergy();

	std::vector<ArborCluster*>& GetMotherCluster();

    void  Reset();
    void  ClearMotherCluster();
	const std::set<ArborCluster*>& GetClustersToMerge() const;
	void  ClearClustersToMerge();
    void  GetAllClustersToMerge(std::set<ArborCluster*>& allClustersToMerge) const;
	const std::vector<ArborCluster*>& GetNearbyClusters() const;
	const ArborCluster* GetMotherAtSearch() const;

	const pandora::CartesianVector& GetAxis() const;
	const pandora::CartesianVector& GetIntercept() const;
	const pandora::CartesianVector& GetCentroid() const;
	const pandora::CartesianVector& GetStartingPoint() const;
	const pandora::CartesianVector& GetEndpoint() const;

	const pandora::OrderedCaloHitList& GetMainOrderedClusterHits() const;
	const pandora::CaloHitList& GetMainClusterHits() const;

    bool IsRoot();
    bool HasMotherAtSearch();
    bool IsDaughter(ArborCluster* cluster);
	bool IsPhoton();
	bool IsFragment();

    void SetHasMotherAtSearch(bool hasMotherAtSearch = true);
	void SetMotherAtSearch(ArborCluster* cluster);
	void ResetMotherAtSearch();
	void SetMotherCluster(ArborCluster* cluster);
	void SetClustersToMerge(const std::vector<ArborCluster*>& clusterVector);
	void RemoveFromClustersToMerge(ArborCluster* cluster);
	void SetNearbyClusters(const std::vector<ArborCluster*>& clusterVector);

	void SetAxis(pandora::CartesianVector axis);
	void SetIntercept(pandora::CartesianVector intercept);
	void SetCentroid(pandora::CartesianVector centrod);
	void SetStartingPoint(pandora::CartesianVector startingPoint);
	void SetEndpoint(pandora::CartesianVector endpoint);

	void SetMainOrderedClusterHits(pandora::OrderedCaloHitList mainOrderedClusterHits);
	void SetMainClusterHits(pandora::CaloHitList mainClusterHits);

	void SetOrderParameterWithMother(ArborCluster* motherCluster, ClustersOrderParameter& clustersOrderParameter);

    ClustersOrderParameter GetOrderParameterWithMother(ArborCluster* motherCluster);

	void SetPhoton(bool isPhoton);
	void SetRoot();
	void SetFragment();

private:

    //
	ArborCluster(const PandoraContentApi::Cluster::Parameters &parameters);

	~ArborCluster();


protected:
	std::vector<ArborCluster*> m_motherCluster;

	std::set<ArborCluster*> m_clustersToMerge;
	std::vector<ArborCluster*> m_nearbyClusters;

	pandora::CartesianVector m_axis;
	pandora::CartesianVector m_intercept;
	pandora::CartesianVector m_centroid;
	pandora::CartesianVector m_startingPoint;
	pandora::CartesianVector m_endpoint;

	bool m_isPhoton;
	bool m_isRoot;
	bool m_hasMotherAtSearch;
	bool m_isFragment;

	ArborCluster*              m_motherAtSearch;

	std::map<ArborCluster*, ClustersOrderParameter> m_orderWithMotherClusters;

	pandora::OrderedCaloHitList m_mainOrderedClusterHits;
	pandora::CaloHitList m_mainClusterHits;

    friend class ClusterFactory;
};

} 

#endif 
