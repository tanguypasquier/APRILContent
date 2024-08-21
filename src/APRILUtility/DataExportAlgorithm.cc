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

    //std::cout << "YOOO" << std::endl;

    for (pandora::ClusterList::const_iterator clusterIter = pClusterList->begin(), iterEnd = pClusterList->end(); clusterIter != iterEnd; ++clusterIter)
    {
        const pandora::Cluster *const pCluster = *clusterIter;

        const pandora::TrackList trackList(pCluster->GetAssociatedTrackList());

        if(trackList.size()==1)
        {
            std::cout << "NB DE TRACKS : " << trackList.size() << std::endl;

            pandora::CaloHitList clusterCaloHitList;
            pCluster->GetOrderedCaloHitList().FillCaloHitList(clusterCaloHitList);

            for (const pandora::Track *const pTrack : trackList)
            {
                std::cout << "ENERGIE TRACK : " << pTrack->GetEnergyAtDca() << std::endl;
                /* std::cout << "MASSE TRACK : " << pTrack->GetMass() << std::endl;
                float Energy = sqrt( pow((pTrack->GetMomentumAtDca()).GetMagnitude() * 3.0e8,2) + pow(pTrack->GetMass()*1e-9,2) * pow(3.0e8,4));
                std::cout << "ENERGIE CALCULEE : " << Energy << std::endl; */

            }


            std::cout<<"NB HITS TOTAL : "<<pCluster->GetNCaloHits()<<std::endl;

            double electromagneticEnergy = 0;
            double hadronicEnergy = 0;
            int seed_count = 0;
            int hit_count = 0;

            std::unordered_map<const pandora::CaloHit*, int> hitIndexMap;
            int index = 0;
            for (const auto& hit : clusterCaloHitList) //Initialize the map with the hits and their index
            {
                const pandora::CaloHit *const pPandoraCaloHit(hit);
                std::cout<< "Energie " << pPandoraCaloHit->GetInputEnergy() << std::endl; 
                hitIndexMap[hit] = index++;
            }

            
            for(pandora::CaloHitList::const_iterator iter = clusterCaloHitList.begin(), endIter = clusterCaloHitList.end() ;
            endIter != iter ; ++iter)
            {
                const pandora::CaloHit *const pCaloHit(*iter);
                const april_content::CaloHit *const pCaloHit = dynamic_cast<const april_content::CaloHit *const>(*iter);

                

                //if(pCaloHit->GetHitType() == pandora::HCAL) //Only focus on HCAL hits
                //{  
                    
                    hit_count++;

                    /* if(APRILContentApi::IsSeed(pCaloHit))
                        seed_count++; */

                    if(!APRILContentApi::HasAnyConnection)
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
                        }
                        else
                        {
                            std::cout << "One or both hits not found in hitIndexMap" << std::endl;
                        }

                        std::cout << "Longueur : " << ConnectionLength << std::endl;
                        
                        
                    }
                //}
            }

        /*  std::cout << "NB DE SEEDS : " << seed_count << std::endl;
            std::cout << "NB DE HCAL HITS : " << hit_count << std::endl; */
        }
        
    }

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
