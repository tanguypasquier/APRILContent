/**
 *  @file   APRILContent/src/APRILUtility/DataExportAlgorithm.cc
 * 
 *  @brief  Implementation of the data export algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "APRILUtility/DataExportAlgorithm.h"

#include "APRILObjects/Connector.h"
#include "APRILObjects/CaloHit.h"

#include <iostream>
#include <fstream>

//#include <algorithm>

namespace april_content
{

DataExportAlgorithm::DataExportAlgorithm()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode DataExportAlgorithm::Run()
{
    //ExportClustersData();
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ExportClustersData());

    return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode DataExportAlgorithm::ExportClustersData() const
{
    const pandora::ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

    for (pandora::ClusterList::const_iterator clusterIter = pClusterList->begin(), iterEnd = pClusterList->end(); clusterIter != iterEnd; ++clusterIter)
    {
        const pandora::Cluster *const pCluster = *clusterIter;

        std::ofstream outputFile;
        outputFile.open("/scratch/pasquier/DATA_EXPORT.txt",std::ios::app);


        float clusterEnergy=0;

        bool hasEcalHits = false;
        bool isLhcal = false;

        ComputeClusterEnergy(pCluster,clusterEnergy);

        if(clusterEnergy>25)
        {

            std::cout << "Energie du cluster : " << clusterEnergy << std::endl;
            outputFile << "Energie du cluster : " << clusterEnergy << std::endl;

            const pandora::TrackList trackList(pCluster->GetAssociatedTrackList());

        //if(trackList.size()==1)
        
            std::cout << "NB DE TRACKS : " << trackList.size() << std::endl;
            outputFile << "NB DE TRACKS : " << trackList.size() << std::endl;

            pandora::CaloHitList clusterCaloHitList;
            pCluster->GetOrderedCaloHitList().FillCaloHitList(clusterCaloHitList);

            for (const pandora::Track *const pTrack : trackList)
            {
                std::cout << "ENERGIE TRACK : " << pTrack->GetEnergyAtDca() << std::endl;
                outputFile << "ENERGIE TRACK : " << pTrack->GetEnergyAtDca() << std::endl;
                /* std::cout << "MASSE TRACK : " << pTrack->GetMass() << std::endl;
                float Energy = sqrt( pow((pTrack->GetMomentumAtDca()).GetMagnitude() * 3.0e8,2) + pow(pTrack->GetMass()*1e-9,2) * pow(3.0e8,4));
                std::cout << "ENERGIE CALCULEE : " << Energy << std::endl; */

            }


            std::cout<<"NB HITS TOTAL : "<<pCluster->GetNCaloHits()<<std::endl;
            outputFile<<"NB HITS TOTAL : "<<pCluster->GetNCaloHits()<<std::endl;

            double electromagneticEnergy = 0;
            double hadronicEnergy = 0;
            int seed_count = 0;
            int hit_count = 0;

            std::unordered_map<const pandora::CaloHit*, int> hitIndexMap;
            int index = 0;
            for (const auto& hit : clusterCaloHitList) //Initialize the map with the hits and their index
            {
                const pandora::CaloHit *const pPandoraCaloHit(hit);
                if(pPandoraCaloHit->GetHitType() == pandora::ECAL) 
                {
                    hasEcalHits = true;
                    //continue;
                }
                else if(pPandoraCaloHit->GetInputEnergy() != 1 | pPandoraCaloHit->GetInputEnergy() != 2 | pPandoraCaloHit->GetInputEnergy() != 3)
                {
                    isLhcal == true;
                    //continue;
                }
                    
                std::cout<< "Energie " << pPandoraCaloHit->GetInputEnergy() << std::endl; 
                outputFile<< pPandoraCaloHit->GetInputEnergy() << std::endl; 
                hitIndexMap[hit] = index++;
            }


            for(pandora::CaloHitList::const_iterator iter = clusterCaloHitList.begin(), endIter = clusterCaloHitList.end() ;
            endIter != iter ; ++iter)
            {

                if(isLhcal)
                    break;

                /* if(hasEcalHits)
                    break; */
            
                

                //const pandora::CaloHit *const pCaloHit(*iter);
                const april_content::CaloHit *const pCaloHit = dynamic_cast<const april_content::CaloHit *const>(*iter);

                

                //if(pCaloHit->GetHitType() == pandora::HCAL) //Only focus on HCAL hits
                {  
                    
                    hit_count++;

                    /* if(APRILContentApi::IsSeed(pCaloHit))
                        seed_count++; */

                    if(!APRILContentApi::HasAnyConnection(pCaloHit))
                        continue;

                    const ConnectorList &fwdConnectorList(APRILContentApi::GetConnectorList(pCaloHit, FORWARD_DIRECTION)); //Enough to check all conector
                    //const ConnectorList &bakConnectorList(APRILContentApi::GetConnectorList(pCaloHit, BACKWARD_DIRECTION));  
                    //const ConnectorList &AllConnectorList(APRILContentApi::GetConnectorList(pCaloHit, FORWARD_DIRECTION)); 

                    //std::cout << "TAILLE BACKWARD : " << bakConnectorList.size() << std::endl;
                    //std::cout << "TAILLE FORWARD : " << fwdConnectorList.size() << std::endl;

                    auto it_from = hitIndexMap.find(pCaloHit); //Beginning of the connector is the CaloHit as we only look in the forward direction

                    for(auto fwdCon : fwdConnectorList)
                    {
                        float ConnectionLength = fwdCon->GetLength();
                        
                        auto it_to = hitIndexMap.find(fwdCon->GetTo());

                        if (it_from != hitIndexMap.end() && it_to != hitIndexMap.end())
                        {
                            std::cout << "From " << it_from->second << " to " << it_to->second << std::endl;
                            outputFile << it_from->second << " " << it_to->second << " " << ConnectionLength << std::endl;
                        }
                        else
                        {
                            std::cout << "One or both hits not found in hitIndexMap" << std::endl;
                            outputFile << "One or both hits not found in hitIndexMap" << std::endl;
                        }

                        std::cout << "Longueur : " << ConnectionLength << std::endl;                  
                        
                    }
                }
            }

        /*  std::cout << "NB DE SEEDS : " << seed_count << std::endl;
            std::cout << "NB DE HCAL HITS : " << hit_count << std::endl; */
        }
        
    }

    return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode DataExportAlgorithm::ComputeClusterEnergy(const pandora::Cluster *const pCluster, float &correctedEnergy) const
  {
    pandora::FloatVector m_sdhcalThresholds; //Vector to store the thresholds used

    bool useDigi = true;

    if(useDigi)
    {
      m_sdhcalThresholds.push_back(1);
      m_sdhcalThresholds.push_back(2);
      m_sdhcalThresholds.push_back(3);
    }
    else 
    {
      m_sdhcalThresholds.push_back(0.0367023);
      m_sdhcalThresholds.push_back(0.0745279);
      m_sdhcalThresholds.push_back(0.363042);
    }

    unsigned int NHadronicHit = 0;
    unsigned int NHadronicHit1 = 0;
    unsigned int NHadronicHit2 = 0;
    unsigned int NHadronicHit3 = 0;

    unsigned int initialNHit1 = 0;
    unsigned int initialNHit2 = 0;
    unsigned int initialNHit3 = 0;
  
    unsigned int barrelNHadronicHit1 = 0; //For the theta correction in the barrel
    unsigned int barrelNHadronicHit2 = 0;
    unsigned int barrelNHadronicHit3 = 0;

    unsigned int endcapNHadronicHit1 = 0; //For the theta correction in the endcap
    unsigned int endcapNHadronicHit2 = 0; 
    unsigned int endcapNHadronicHit3 = 0; 

    unsigned int firstNHadronicHit1 = 0; //For the phi correction in the first calo block in Videau geometry
    unsigned int firstNHadronicHit2 = 0;
    unsigned int firstNHadronicHit3 = 0; 

    unsigned int secondNHadronicHit1 = 0; //For the phi correction in the second calo block in Videau geometry
    unsigned int secondNHadronicHit2 = 0;
    unsigned int secondNHadronicHit3 = 0; 

    float initialHadronic = 0;

    float lumiHCALEnergy = 0;

    float emEnergy = 0;

    //std::cout << "CORRECTED INIT : " << correctedEnergy << std::endl;

    if(pCluster->GetNCaloHits() == 0)
      return pandora::STATUS_CODE_SUCCESS;

    pandora::CaloHitList clusterCaloHitList;
    pCluster->GetOrderedCaloHitList().FillCaloHitList(clusterCaloHitList);

    bool noShowerHit(true);

    for(pandora::CaloHitList::const_iterator iter = clusterCaloHitList.begin(), endIter = clusterCaloHitList.end() ;
        endIter != iter ; ++iter)
    {
      const pandora::CaloHit *const pCaloHit(*iter);

      if(pCaloHit->GetHitType() == pandora::ECAL) //Only focus on SDHCAL corrections
      {
        emEnergy += pCaloHit->GetElectromagneticEnergy();
        noShowerHit = false;
      }

      else if(pCaloHit->GetHitType() == pandora::HCAL) //Only focus on SDHCAL corrections
      {
        if(pCaloHit->GetHitRegion() == pandora::BARREL)
        {
          //std::cout << pCaloHit->GetCellNormalVector() << std::endl;
          if(fabs(m_sdhcalThresholds.at(0) - pCaloHit->GetInputEnergy()) < std::numeric_limits<float>::epsilon())
          {
            initialHadronic+=pCaloHit->GetHadronicEnergy();
            if(pCaloHit->GetPseudoLayer() == (pCaloHit->GetLayer()+30)) //If pseudolayer!=layer+30, means the hit is in the other block
              firstNHadronicHit1++;
            else
              secondNHadronicHit1++;
          }
          else if(fabs(m_sdhcalThresholds.at(1) - pCaloHit->GetInputEnergy()) < std::numeric_limits<float>::epsilon())
          {
            initialHadronic+=pCaloHit->GetHadronicEnergy();
            if(pCaloHit->GetPseudoLayer() == (pCaloHit->GetLayer()+30)) //If pseudolayer!=layer+30, means the hit is in the other block
              firstNHadronicHit2++;
            else
              secondNHadronicHit2++;
          }
          else if(fabs(m_sdhcalThresholds.at(2) - pCaloHit->GetInputEnergy()) < std::numeric_limits<float>::epsilon())
          {
            initialHadronic+=pCaloHit->GetHadronicEnergy();
            if(pCaloHit->GetPseudoLayer() == (pCaloHit->GetLayer()+30)) //If pseudolayer!=layer+30, means the hit is in the other block
              firstNHadronicHit3++;
            else
              secondNHadronicHit3++;
          }
        }
        else if(pCaloHit->GetHitRegion() == pandora::ENDCAP)
        {
          if(fabs(m_sdhcalThresholds.at(0) - pCaloHit->GetInputEnergy()) < std::numeric_limits<float>::epsilon())
          { 
            endcapNHadronicHit1++;
            initialHadronic+=pCaloHit->GetHadronicEnergy();
          }
          else if(fabs(m_sdhcalThresholds.at(1) - pCaloHit->GetInputEnergy()) < std::numeric_limits<float>::epsilon())
          {
            endcapNHadronicHit2++;
            initialHadronic+=pCaloHit->GetHadronicEnergy();
          }
          else if(fabs(m_sdhcalThresholds.at(2) - pCaloHit->GetInputEnergy()) < std::numeric_limits<float>::epsilon())
          {
            endcapNHadronicHit3++;
            initialHadronic+=pCaloHit->GetHadronicEnergy();
          }
          else
          {
            lumiHCALEnergy+=pCaloHit->GetHadronicEnergy();
          }
        }
        noShowerHit = false;
      }
    }

    if(noShowerHit)
      return pandora::STATUS_CODE_SUCCESS;

    //correctedEnergy-=initialHadronic; // Pull the initial energy of the HCAL hits from the total energy to correct

    //std::cout << "WITHOUT INITIAL : " << correctedEnergy << std::endl;

    barrelNHadronicHit1 = firstNHadronicHit1 + secondNHadronicHit1; //Sum the hits in the barrel
    barrelNHadronicHit2 = firstNHadronicHit2 + secondNHadronicHit2;
    barrelNHadronicHit3 = firstNHadronicHit3 + secondNHadronicHit3;

    initialNHit1 = barrelNHadronicHit1 + endcapNHadronicHit1; //Compute the initial number of hits in sdhcal
    initialNHit2 = barrelNHadronicHit2 + endcapNHadronicHit2;
    initialNHit3 = barrelNHadronicHit3 + endcapNHadronicHit3;

    //Sum the corrected number of hits for each tresholds
    NHadronicHit1 = initialNHit1;
    NHadronicHit2 = initialNHit2;
    NHadronicHit3 = initialNHit3;

    //Calculate the corrected total number of hits
    NHadronicHit = NHadronicHit1 + NHadronicHit2 + NHadronicHit3;

    //std::cout << "NHit corrected : " << NHadronicHit << std::endl;

    const float hadEnergy(NHadronicHit1*0.0367023 + NHadronicHit2*0.0745279 + NHadronicHit3*0.363042); //Initial parameters corresponding to the Energy factors
      
    correctedEnergy = lumiHCALEnergy + emEnergy + hadEnergy;
    

    return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode DataExportAlgorithm::ReadSettings(const pandora::TiXmlHandle xmlHandle)
{
    //PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        //"ShouldExportData", m_shouldExport));
    return pandora::STATUS_CODE_SUCCESS;
}

} // namespace april_content
