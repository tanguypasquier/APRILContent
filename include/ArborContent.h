  /// \file ArborContent.h
/*
 *
 * ArborContent.h header template automatically generated by a class generator
 * Creation date : ven. mars 20 2015
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

#ifndef ARBOR_CONTENT_H
#define ARBOR_CONTENT_H 1

#include "Api/PandoraApi.h"
#include "ArborApi/ArborApi.h"

#include "ArborClustering/ClusteringParentAlgorithm.h"
#include "ArborClustering/ConnectorClusteringAlgorithm.h"
#include "ArborClustering/ConnectorSeedingAlgorithm.h"
#include "ArborClustering/GlobalConnectorCleaningAlgorithm.h"
#include "ArborClustering/TreeClusteringAlgorithm.h"

#include "ArborHelpers/CaloHitHelper.h"
#include "ArborHelpers/ClusterHelper.h"
#include "ArborHelpers/GeometryHelper.h"
#include "ArborHelpers/ReclusterHelper.h"
#include "ArborHelpers/SortingHelper.h"

#include "ArborPlugins/ArborBFieldPlugin.h"
#include "ArborPlugins/ArborPseudoLayerPlugin.h"

#include "ArborTopologicalAssociation/TopologicalAssociationParentAlgorithm.h"
#include "ArborTopologicalAssociation/PointingClusterAssociationAlgorithm.h"
#include "ArborTrackClusterAssociation/EnergyDrivenTrackClusterAssociationAlgorithm.h"
#include "ArborTrackClusterAssociation/TopologicalTrackClusterAssociationAlgorithm.h"

#include "ArborPfoConstruction/PfoCreationAlgorithm.h"

#include "ArborMonitoring/VisualMonitoringAlgorithm.h"

#include "ArborUtility/EnergyEstimateTools.h"
#include "ArborUtility/OrderParameterTools.h"
#include "ArborUtility/ReferenceVectorTools.h"
#include "ArborUtility/SplitClusterTool.h"
#include "ArborUtility/EventPreparationAlgorithm.h"
#include "ArborUtility/ClusterPreparationAlgorithm.h"
#include "ArborUtility/TrackPreparationAlgorithm.h"
#include "ArborUtility/ListChangingAlgorithm.h"

/** 
 * @brief  ArborContent class used to register arbor algorithms and plugins
 */ 
class ArborContent 
{

public:

#define ARBOR_ALGORITHM_LIST(d) \
	d("ConnectorSeeding",                    arbor_content::ConnectorSeedingAlgorithm::Factory)       \
	d("ConnectorClustering",                 arbor_content::ConnectorClusteringAlgorithm::Factory)    \
	d("ClusteringParent",                    arbor_content::ClusteringParentAlgorithm::Factory)       \
	d("GlobalConnectorCleaning",             arbor_content::GlobalConnectorCleaningAlgorithm::Factory) \
	d("TreeClustering",                      arbor_content::TreeClusteringAlgorithm::Factory) \
	d("TopologicalAssociationParent",        arbor_content::TopologicalAssociationParentAlgorithm::Factory) \
	d("PointingClusterAssociation",          arbor_content::PointingClusterAssociationAlgorithm::Factory)      \
	d("TopologicalTrackClusterAssociation",  arbor_content::TopologicalTrackClusterAssociationAlgorithm::Factory)       \
	d("EnergyDrivenTrackClusterAssociation", arbor_content::EnergyDrivenTrackClusterAssociationAlgorithm::Factory) \
	d("PfoCreation",                         arbor_content::PfoCreationAlgorithm::Factory) \
	d("VisualMonitoring",                    arbor_content::VisualMonitoringAlgorithm::Factory) \
	d("EventPreparation",                    arbor_content::EventPreparationAlgorithm::Factory) \
	d("ClusterPreparation",                  arbor_content::ClusterPreparationAlgorithm::Factory) \
	d("TrackPreparation",                    arbor_content::TrackPreparationAlgorithm::Factory) \
	d("ListChanging",                        arbor_content::ListChangingAlgorithm::Factory)


#define ARBOR_ALGORITHM_TOOL_LIST(d) \
	d("KappaOrderParameter",                 arbor_content::KappaOrderParameterTool::Factory) \
	d("SimpleReferenceVector",               arbor_content::SimpleReferenceVectorTool::Factory) \
	d("SplitCluster",                        arbor_content::SplitClusterTool::Factory) \
	d("LinearInputEnergyEstimate",           arbor_content::LinearInputEnergyEstimate::Factory) \
	d("CombinedQuadraticEnergyEstimate",     arbor_content::CombinedQuadraticEnergyEstimate::Factory) \
	d("LinearEnergyEstimate",                arbor_content::LinearEnergyEstimate::Factory)


 /**
  *  @brief  Register all the arbor algorithms with pandora
  *
  *  @param  pandora the pandora instance with which to register content
  */
 static pandora::StatusCode RegisterAlgorithms(const pandora::Pandora &pandora);

 /**
  *  @brief  Register pseudo layer plugin with pandora
  *
  *  @param  pandora the pandora instance with which to register content
  */
 static pandora::StatusCode RegisterArborPseudoLayerPlugin(const pandora::Pandora &pandora);

 /**
  *  @brief  Register the b field plugin (note user side configuration) with pandora
  *
  *  @param  pandora the pandora instance with which to register content
  *  @param  innerBField the bfield in the main tracker, ecal and hcal, units Tesla
  *  @param  muonBarrelBField the bfield in the muon barrel, units Tesla
  *  @param  muonEndCapBField the bfield in the muon endcap, units Tesla
  */
 static pandora::StatusCode RegisterBFieldPlugin(const pandora::Pandora &pandora, const float innerBField, const float muonBarrelBField,
     const float muonEndCapBField);
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::StatusCode ArborContent::RegisterAlgorithms(const pandora::Pandora &pandora)
{
	ARBOR_ALGORITHM_LIST(PANDORA_REGISTER_ALGORITHM);
	ARBOR_ALGORITHM_TOOL_LIST(PANDORA_REGISTER_ALGORITHM_TOOL);

	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::StatusCode ArborContent::RegisterArborPseudoLayerPlugin(const pandora::Pandora &pandora)
{
	return PandoraApi::SetPseudoLayerPlugin(pandora, new arbor_content::ArborPseudoLayerPlugin());
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::StatusCode ArborContent::RegisterBFieldPlugin(const pandora::Pandora &pandora, const float innerBField,
    const float muonBarrelBField, const float muonEndCapBField)
{
    return PandoraApi::SetBFieldPlugin(pandora, new arbor_content::ArborBFieldPlugin(innerBField, muonBarrelBField, muonEndCapBField));
}


#endif  //  ARBOR_CONTENT_H
