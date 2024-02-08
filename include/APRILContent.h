/// \file APRILContent.h
/*
 *
 * APRILContent.h header template automatically generated by a class generator
 * Creation date : ven. mars 20 2015
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

#ifndef APRIL_CONTENT_H
#define APRIL_CONTENT_H 1

// multi-threading option
#ifdef APRIL_PARALLEL
#include <omp.h>
#endif

#include "Api/PandoraApi.h"
#include "Pandora/PandoraInternal.h"
#include "APRILApi/APRILContentApi.h"
#include "APRILApi/ObjectFactories.h"

#include "APRILCheating/CheatingTrackToClusterMatching.h"
#include "APRILCheating/PerfectParticleFlowAlgorithm.h"
#include "APRILCheating/PerfectClusteringAlgorithm.h"
#include "APRILCheating/PerfectClusteringAlgorithmNew.h"
#include "APRILCheating/PerfectHitCorrectionAlgorithm.h"
#include "APRILCheating/PerfectFragmentRemovalAlgorithm.h"
#include "APRILCheating/PerfectIsoHitRemovalAlgorithm.h"
#include "APRILCheating/PerfectNeutralHitRecoveryAlgorithm.h"
#include "APRILCheating/PerfectChargedHitRecoveryAlgorithm.h"
#include "APRILCheating/PerfectChargedClusterSeparationAlgorithm.h"
#include "APRILCheating/PerfectPfoCreationAlgorithm.h"
#include "APRILCheating/SimplePfoTestAlgorithm.h"
#include "APRILCheating/SemiPerfectPfoCreationAlgorithm.h"
#include "APRILCheating/CheatingClusterCleaningAlgorithm.h"
#include "APRILCheating/CheatingClusterMergingAlgorithm.h"
#include "APRILCheating/CheatingClusterMergingNewAlgorithm.h"
#include "APRILCheating/CheatingHitRecoveryAlgorithm.h"
#include "APRILCheating/CheatingParticleIDAlgorithm.h"

#include "APRILClustering/APRILClusteringAlgorithm.h"
#include "APRILClustering/ClusteringParentAlgorithm.h"
#include "APRILClustering/SimpleRegionClusteringAlgorithm.h"
#include "APRILClustering/IsoHitRemovalAlgorithm.h"
#include "APRILClustering/IsoHitClusteringAlgorithm.h"
#include "APRILClustering/FragmentsFromIsoHitsMergingAlgorithm.h"

#include "APRILHelpers/CaloHitHelper.h"
#include "APRILHelpers/CaloHitRangeSearchHelper.h"
#include "APRILHelpers/CaloHitNeighborSearchHelper.h"
#include "APRILHelpers/ClusterHelper.h"
#include "APRILHelpers/ClusterPropertiesHelper.h"
#include "APRILHelpers/GeometryHelper.h"
#include "APRILHelpers/ReclusterHelper.h"
#include "APRILHelpers/SortingHelper.h"
#include "APRILHelpers/BDTBasedClusterIdHelper.h"
#include "APRILHelpers/ClusterPropertiesFillingHelper.h"
#include "APRILHelpers/ClusterTrackEnergyFillingHelper.h"

#include "APRILMonitoring/VisualMonitoringAlgorithm.h"
#include "APRILMonitoring/PerformanceMonitoringAlgorithm.h"
#include "APRILMonitoring/SingleParticleMonitoringAlgorithm.h"

#include "APRILPfoConstruction/PfoCreationAlgorithm.h"

#include "APRILParticleId/FinalParticleIdAlgorithm.h"
#include "APRILParticleId/PhotonReconstructionAlgorithm.h"
#include "APRILParticleId/SingleClusterIdAlgorithm.h"

#include "APRILPlugins/APRILBFieldPlugin.h"
#include "APRILPlugins/APRILPseudoLayerPlugin.h"
#include "APRILPlugins/EnergyCorrectionPlugins.h"
#include "APRILPlugins/ParticleIdPlugins.h"
#include "APRILPlugins/ShowerProfilePlugin.h"

#include "APRILReclustering/EnergyExcessReclusteringAlgorithm.h"
#include "APRILReclustering/MissingEnergyReclusteringAlgorithm.h"
#include "APRILReclustering/SplitTrackReclusteringAlgorithm.h"
#include "APRILReclustering/ResolveTrackAssociationsAlg.h"
#include "APRILReclustering/ForcedClusteringAlgorithm.h"
#include "APRILReclustering/ForceSplitTrackAssociationsAlg.h"

#include "APRILTools/ConnectorCleaningTool.h"
#include "APRILTools/ConnectorSeedingTool.h"
#include "APRILTools/ConnectorPlusTool.h"
#include "APRILTools/NearbyHitsConnectingTool.h"
#include "APRILTools/ConnectorAlignmentTool.h"
#include "APRILTools/TrackDrivenSeedingTool.h"
#include "APRILTools/CaloHitMergingTool.h"

#include "APRILTopologicalAssociation/TopologicalAssociationParentAlgorithm.h"
#include "APRILTopologicalAssociation/PointingClusterAssociationAlgorithm.h"
#include "APRILTopologicalAssociation/PointingClusterAssociationNewAlgorithm.h"
#include "APRILTopologicalAssociation/ClustersMergingAlgorithm.h"
#include "APRILTopologicalAssociation/MatchTrackWithStartingClusterAlgorithm.h"
#include "APRILTopologicalAssociation/ClusterFromTrackMergingAlgorithm.h"
#include "APRILTopologicalAssociation/ClusterFromTrackMergingAlgorithm2.h"
#include "APRILTopologicalAssociation/ClusterFromTrackMergingAlgorithm3.h"
#include "APRILTopologicalAssociation/ChargedFragmentsMergingAlgorithm.h"
#include "APRILTopologicalAssociation/ChargedFragmentsMergingAlgorithm2.h"
#include "APRILTopologicalAssociation/ChargedFragmentsMergingAlgorithm3.h"
#include "APRILTopologicalAssociation/ClusterFromTrackMergingAlgorithm4.h"
#include "APRILTopologicalAssociation/ClusterFromTrackMergingAlgorithm5.h"
#include "APRILTopologicalAssociation/NeutralFragmentMergingAlgorithm.h"
#include "APRILTopologicalAssociation/NearbyFragmentsMergingAlgorithm.h"
#include "APRILTopologicalAssociation/ClosebySeedMergingAlgorithm.h"
#include "APRILTopologicalAssociation/ClusterFragmentMergingAlgorithm.h"
#include "APRILTopologicalAssociation/SurroundingHitsMergingAlgorithm.h"
#include "APRILTopologicalAssociation/NearbyTrackPhotonRemovalAlg.h"
#include "APRILTopologicalAssociation/MipFragmentMergingAlg.h"
#include "APRILTopologicalAssociation/FragmentRemovalAlgorithm.h"
#include "APRILTopologicalAssociation/FragmentsFindingAlgorithm.h"
#include "APRILTopologicalAssociation/PhotonCleanAlgorithm.h"
#include "APRILTopologicalAssociation/ConeBasedMergingAlgorithm.h"
#include "APRILTopologicalAssociation/MainFragmentRemovalAlgorithm.h"

#include "APRILTrackClusterAssociation/TrackClusterAssociationMVAAlgorithm.h"
#include "APRILTrackClusterAssociation/UnassociatedTrackRecoveryAlg.h"
#include "APRILTrackClusterAssociation/TrackClusterAssociationAlgorithm.h"
#include "APRILTrackClusterAssociation/LoopingTrackAssociationAlgorithm.h"
#include "APRILTrackClusterAssociation/TrackRecoveryAlgorithm.h"
#include "APRILTrackClusterAssociation/TrackRecoveryHelixAlgorithm.h"
#include "APRILTrackClusterAssociation/TrackRecoveryInteractionsAlgorithm.h"

#include "APRILUtility/AlgorithmConfiguration.h"
#include "APRILUtility/EventPreparationAlgorithm.h"
#include "APRILUtility/ClusterPreparationAlgorithm.h"
#include "APRILUtility/TrackPreparationAlgorithm.h"
#include "APRILUtility/ListChangingAlgorithm.h"
#include "APRILUtility/ListMergingAlgorithm.h"
#include "APRILUtility/CaloHitPreparationAlgorithm.h"
#include "APRILUtility/CaloHitTimingAlgorithm.h"
#include "APRILUtility/ClusterCheckAlgorithm.h"
#include "APRILUtility/ClusterPurityAlgorithm.h"
#include "APRILUtility/ClusterEfficiencyAlgorithm.h"
#include "APRILUtility/IsolatedHitClusteringAlgorithm.h"
#include "APRILUtility/LoadEMClusterAlgorithm.h"
#include "APRILUtility/NearbyHitRecoveryAlgorithm.h"

/** 
 * @brief  APRILContent class used to register april algorithms and plugins
 */ 
class APRILContent 
{
public:

#define APRIL_ALGORITHM_LIST(d) \
    d("PerfectParticleFlow",                 april_content::PerfectParticleFlowAlgorithm::Factory) \
    d("PerfectClustering",                   april_content::PerfectClusteringAlgorithm::Factory) \
    d("PerfectClusteringNew",                april_content::PerfectClusteringAlgorithmNew::Factory) \
    d("PerfectHitCorrection",                april_content::PerfectHitCorrectionAlgorithm::Factory) \
    d("PerfectFragmentRemoval",              april_content::PerfectFragmentRemovalAlgorithm::Factory) \
    d("PerfectIsoHitRemoval",                april_content::PerfectIsoHitRemovalAlgorithm::Factory) \
    d("IsoHitRemoval",                       april_content::IsoHitRemovalAlgorithm::Factory) \
    d("IsoHitClustering",                    april_content::IsoHitClusteringAlgorithm::Factory) \
    d("FragmentsFromIsoHitsMerging",         april_content::FragmentsFromIsoHitsMergingAlgorithm::Factory) \
    d("PerfectNeutralHitRecovery",           april_content::PerfectNeutralHitRecoveryAlgorithm::Factory) \
    d("PerfectChargedHitRecovery",           april_content::PerfectChargedHitRecoveryAlgorithm::Factory) \
    d("PerfectChargedClusterSeparation",     april_content::PerfectChargedClusterSeparationAlgorithm::Factory) \
    d("PerfectPfoCreation",                  april_content::PerfectPfoCreationAlgorithm::Factory) \
    d("SimplePfoTest",                       april_content::SimplePfoTestAlgorithm::Factory) \
    d("SemiPerfectPfoCreation",              april_content::SemiPerfectPfoCreationAlgorithm::Factory) \
    d("CheatingTrackToClusterMatching",      april_content::CheatingTrackToClusterMatching::Factory) \
    d("APRILClustering",                     april_content::APRILClusteringAlgorithm::Factory) \
    d("CheatingClusterCleaning",             april_content::CheatingClusterCleaningAlgorithm::Factory) \
    d("CheatingClusterMerging",              april_content::CheatingClusterMergingAlgorithm::Factory) \
    d("CheatingClusterMergingNew",           april_content::CheatingClusterMergingNewAlgorithm::Factory) \
    d("CheatingHitRecovery",                 april_content::CheatingHitRecoveryAlgorithm::Factory) \
    d("CheatingParticleID",                  april_content::CheatingParticleIDAlgorithm::Factory) \
    d("ClusteringParent",                    april_content::ClusteringParentAlgorithm::Factory) \
    d("SimpleRegionClustering",              april_content::SimpleRegionClusteringAlgorithm::Factory) \
    d("VisualMonitoring",                    april_content::VisualMonitoringAlgorithm::Factory) \
    d("PerformanceMonitoring",               april_content::PerformanceMonitoringAlgorithm::Factory) \
    d("SingleParticleMonitoring",            april_content::SingleParticleMonitoringAlgorithm::Factory) \
    d("FinalParticleId",                     april_content::FinalParticleIdAlgorithm::Factory) \
    d("PhotonReconstruction",                april_content::PhotonReconstructionAlgorithm::Factory) \
    d("SingleClusterId",                     april_content::SingleClusterIdAlgorithm::Factory) \
    d("PfoCreation",                         april_content::PfoCreationAlgorithm::Factory) \
    d("EnergyExcessReclustering",            april_content::EnergyExcessReclusteringAlgorithm::Factory) \
    d("MissingEnergyReclustering",           april_content::MissingEnergyReclusteringAlgorithm::Factory) \
    d("SplitTrackReclustering",              april_content::SplitTrackReclusteringAlgorithm::Factory) \
    d("ForceSplitTrackAssociations",         april_content::ForceSplitTrackAssociationsAlg::Factory) \
    d("ResolveTrackAssociations",            april_content::ResolveTrackAssociationsAlg::Factory) \
    d("ForcedClustering",                    april_content::ForcedClusteringAlgorithm::Factory) \
    d("TopologicalAssociationParent",        april_content::TopologicalAssociationParentAlgorithm::Factory) \
    d("PointingClusterAssociation",          april_content::PointingClusterAssociationAlgorithm::Factory) \
    d("PointingClusterAssociationNew",       april_content::PointingClusterAssociationNewAlgorithm::Factory) \
    d("ClustersMerging",                     april_content::ClustersMergingAlgorithm::Factory) \
    d("MatchTrackWithStartingCluster",       april_content::MatchTrackWithStartingClusterAlgorithm::Factory) \
    d("ClusterFromTrackMerging",             april_content::ClusterFromTrackMergingAlgorithm::Factory) \
    d("ClusterFromTrackMerging2",            april_content::ClusterFromTrackMergingAlgorithm2::Factory) \
    d("ClusterFromTrackMerging3",            april_content::ClusterFromTrackMergingAlgorithm3::Factory) \
    d("ChargedFragmentsMerging",             april_content::ChargedFragmentsMergingAlgorithm::Factory) \
    d("ChargedFragmentsMerging2",            april_content::ChargedFragmentsMergingAlgorithm2::Factory) \
    d("ChargedFragmentsMerging3",            april_content::ChargedFragmentsMergingAlgorithm3::Factory) \
    d("ClusterFromTrackMerging4",            april_content::ClusterFromTrackMergingAlgorithm4::Factory) \
    d("ClusterFromTrackMerging5",            april_content::ClusterFromTrackMergingAlgorithm5::Factory) \
    d("NeutralFragmentMerging",              april_content::NeutralFragmentMergingAlgorithm::Factory) \
    d("NearbyFragmentsMerging",              april_content::NearbyFragmentsMergingAlgorithm::Factory) \
    d("ClosebySeedMerging",                  april_content::ClosebySeedMergingAlgorithm::Factory) \
    d("ClusterFragmentMerging",              april_content::ClusterFragmentMergingAlgorithm::Factory) \
    d("SurroundingHitsMerging",              april_content::SurroundingHitsMergingAlgorithm::Factory) \
    d("NearbyTrackPhotonRemoval",            april_content::NearbyTrackPhotonRemovalAlg::Factory) \
    d("MipFragmentMerging",                  april_content::MipFragmentMergingAlg::Factory) \
    d("FragmentRemoval",                     april_content::FragmentRemovalAlgorithm::Factory) \
    d("FragmentsFinding",                    april_content::FragmentsFindingAlgorithm::Factory) \
    d("PhotonClean",                         april_content::PhotonCleanAlgorithm::Factory) \
    d("ConeBasedMerging",                    april_content::ConeBasedMergingAlgorithm::Factory) \
    d("MainFragmentRemoval",                 april_content::MainFragmentRemovalAlgorithm::Factory) \
    d("TrackClusterAssociationMVA",          april_content::TrackClusterAssociationMVAAlgorithm::Factory) \
    d("UnassociatedTrackRecovery",           april_content::UnassociatedTrackRecoveryAlg::Factory) \
    d("TrackClusterAssociation",             april_content::TrackClusterAssociationAlgorithm::Factory) \
    d("LoopingTrackAssociation",             april_content::LoopingTrackAssociationAlgorithm::Factory) \
    d("TrackRecovery",                       april_content::TrackRecoveryAlgorithm::Factory) \
    d("TrackRecoveryHelix",                  april_content::TrackRecoveryHelixAlgorithm::Factory) \
    d("TrackRecoveryInteractions",           april_content::TrackRecoveryInteractionsAlgorithm::Factory) \
    d("EventPreparation",                    april_content::EventPreparationAlgorithm::Factory) \
    d("ClusterPreparation",                  april_content::ClusterPreparationAlgorithm::Factory) \
    d("TrackPreparation",                    april_content::TrackPreparationAlgorithm::Factory) \
    d("CaloHitListMerging",                  april_content::InputObjectListMergingAlgorithm<pandora::CaloHitList>::Factory) \
    d("TrackListMerging",                    april_content::InputObjectListMergingAlgorithm<pandora::TrackList>::Factory) \
    d("MCParticleListMerging",               april_content::InputObjectListMergingAlgorithm<pandora::MCParticleList>::Factory) \
    d("PfoListMerging",                      april_content::AlgorithmObjectListMergingAlgorithm<pandora::ParticleFlowObject>::Factory) \
    d("ClusterListMerging",                  april_content::AlgorithmObjectListMergingAlgorithm<pandora::Cluster>::Factory) \
    d("VertexListMerging",                   april_content::AlgorithmObjectListMergingAlgorithm<pandora::Vertex>::Factory) \
    d("ListChanging",                        april_content::ListChangingAlgorithm::Factory) \
    d("AlgorithmConfiguration",              april_content::AlgorithmConfiguration::Factory) \
    d("CaloHitPreparation",                  april_content::CaloHitPreparationAlgorithm::Factory) \
    d("CaloHitTiming",                       april_content::CaloHitTimingAlgorithm::Factory) \
    d("ClusterCheck",                        april_content::ClusterCheckAlgorithm::Factory) \
    d("ClusterPurity",                       april_content::ClusterPurityAlgorithm::Factory) \
    d("ClusterEfficiency",                   april_content::ClusterEfficiencyAlgorithm::Factory) \
    d("IsolatedHitClustering",               april_content::IsolatedHitClusteringAlgorithm::Factory) \
    d("NearbyHitRecovery",                   april_content::NearbyHitRecoveryAlgorithm::Factory) \
    d("LoadEMCluster",                       april_content::LoadEMClusterAlgorithm::Factory)


#define APRIL_ALGORITHM_TOOL_LIST(d) \
    d("ConnectorCleaning",                   april_content::ConnectorCleaningTool::Factory) \
    d("ConnectorSeeding",                    april_content::ConnectorSeedingTool::Factory) \
    d("ConnectorPlus",                       april_content::ConnectorPlusTool::Factory) \
    d("NearbyHitsConnecting",                april_content::NearbyHitsConnectingTool::Factory) \
    d("TrackDrivenSeeding",                  april_content::TrackDrivenSeedingTool::Factory) \
    d("ConnectorAlignment",                  april_content::ConnectorAlignmentTool::Factory) \
    d("CaloHitMerging",                      april_content::CaloHitMergingTool::Factory)

#define APRIL_PARTICLE_ID_LIST(d) \
    d("APRILEmShowerId",                     april_content::APRILParticleIdPlugins::APRILEmShowerId) \
    d("APRILPhotonId",                       april_content::APRILParticleIdPlugins::APRILPhotonId) \
    d("APRILElectronId",                     april_content::APRILParticleIdPlugins::APRILElectronId) \
    d("APRILMuonId",                         april_content::APRILParticleIdPlugins::APRILMuonId)


  /**
   *  @brief  Register all the april algorithms with pandora
   *
   *  @param  pandora the pandora instance with which to register content
   */
  static pandora::StatusCode RegisterAlgorithms(const pandora::Pandora &pandora);

  /**
   *  @brief  Register pseudo layer plugin with pandora
   *
   *  @param  pandora the pandora instance with which to register content
   */
  static pandora::StatusCode RegisterAPRILPseudoLayerPlugin(const pandora::Pandora &pandora);
  static pandora::StatusCode RegisterAPRILShowerProfilePlugin(const pandora::Pandora &pandora);

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

  /**
   *  @brief  Register the energy corrections with pandora
   *
   *  @param  pandora the pandora instance with which to register content
   */
  static pandora::StatusCode RegisterEnergyCorrections(const pandora::Pandora &pandora);

  /**
   *  @brief  Register april particle id functions
   *
   *  @param  pandora the pandora instance with which to register content
   */
  static pandora::StatusCode RegisterParticleIds(const pandora::Pandora &pandora);
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::StatusCode APRILContent::RegisterAlgorithms(const pandora::Pandora &pandora)
{
  APRIL_ALGORITHM_LIST(PANDORA_REGISTER_ALGORITHM);
  APRIL_ALGORITHM_TOOL_LIST(PANDORA_REGISTER_ALGORITHM_TOOL);

  return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::StatusCode APRILContent::RegisterAPRILPseudoLayerPlugin(const pandora::Pandora &pandora)
{
  return PandoraApi::SetPseudoLayerPlugin(pandora, new april_content::APRILPseudoLayerPlugin());
}

inline pandora::StatusCode APRILContent::RegisterAPRILShowerProfilePlugin(const pandora::Pandora &pandora)
{
  return PandoraApi::SetShowerProfilePlugin(pandora, new april_content::APRILShowerProfilePlugin());
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::StatusCode APRILContent::RegisterBFieldPlugin(const pandora::Pandora &pandora, const float innerBField,
    const float muonBarrelBField, const float muonEndCapBField)
{
  return PandoraApi::SetBFieldPlugin(pandora, new april_content::APRILBFieldPlugin(innerBField, muonBarrelBField, muonEndCapBField));
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::StatusCode APRILContent::RegisterEnergyCorrections(const pandora::Pandora &pandora)
{
  /* PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraApi::RegisterEnergyCorrectionPlugin(pandora,
      "SdhcalQuadraticEnergyFunction", pandora::HADRONIC, new april_content::SdhcalQuadraticEnergyFunction));

  PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraApi::RegisterEnergyCorrectionPlugin(pandora,
      "SdhcalQuadraticEnergyFunction", pandora::ELECTROMAGNETIC, new april_content::SdhcalQuadraticEnergyFunction));

  PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraApi::RegisterEnergyCorrectionPlugin(pandora,
      "AnalogicEnergyFunction", pandora::HADRONIC, new april_content::AnalogicEnergyFunction));

  PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraApi::RegisterEnergyCorrectionPlugin(pandora,
      "AnalogicEnergyFunction", pandora::ELECTROMAGNETIC, new april_content::AnalogicEnergyFunction));

  PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraApi::RegisterEnergyCorrectionPlugin(pandora,
      "BarrelGapEnergyFunction", pandora::HADRONIC, new april_content::BarrelGapEnergyFunction));

  PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraApi::RegisterEnergyCorrectionPlugin(pandora,
      "ThetaEnergyFunction", pandora::HADRONIC, new april_content::ThetaEnergyFunction));
 */
  /* PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraApi::RegisterEnergyCorrectionPlugin(pandora,
      "ThetaNHitFunction", pandora::HADRONIC, new april_content::ThetaNHitFunction()));

  PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraApi::RegisterEnergyCorrectionPlugin(pandora,
      "PhiNHitFunction", pandora::HADRONIC, new april_content::PhiNHitFunction()));
 */
  /* PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraApi::RegisterEnergyCorrectionPlugin(pandora,
      "CleanClusters", pandora::HADRONIC, new april_content::CleanCluster()));
 */
  return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode APRILContent::RegisterParticleIds(const pandora::Pandora &pandora)
{
  APRIL_PARTICLE_ID_LIST(PANDORA_REGISTER_PARTICLE_ID);

  return pandora::STATUS_CODE_SUCCESS;
}


#endif  //  APRIL_CONTENT_H
