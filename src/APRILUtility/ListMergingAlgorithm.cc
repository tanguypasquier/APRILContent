/// \file ListMergingAlgorithm.cc
/*
 *
 * ListMergingAlgorithm.cc source template automatically generated by a class generator
 * Creation date : ven. oct. 9 2015
 *
 * This file is part of ListMergingAlgorithm libraries.
 * 
 * ListMergingAlgorithm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 * 
 * ListMergingAlgorithm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with ListMergingAlgorithm.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */


#include "APRILUtility/ListMergingAlgorithm.h"

#include "Pandora/AlgorithmHeaders.h"

namespace april_content
{

template <typename T>
pandora::Algorithm *InputObjectListMergingAlgorithm<T>::Factory::CreateAlgorithm() const
{
	return new InputObjectListMergingAlgorithm<T>();
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::Algorithm *AlgorithmObjectListMergingAlgorithm<T>::Factory::CreateAlgorithm() const
{
	return new AlgorithmObjectListMergingAlgorithm<T>();
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode InputObjectListMergingAlgorithm<T>::Run()
{
	T list;

	for(pandora::StringVector::iterator iter = ListMergingAlgorithm<T>::m_inputListNames.begin(), endIter = ListMergingAlgorithm<T>::m_inputListNames.end() ;
			endIter != iter ; ++iter)
	{
		const T *pList = NULL;

		const pandora::StatusCode statusCode(PandoraContentApi::GetList(*this, *iter, pList));

		if(statusCode == pandora::STATUS_CODE_NOT_FOUND)
			continue;

		if(statusCode != pandora::STATUS_CODE_SUCCESS)
			return statusCode;

		list.insert(list.begin(), pList->begin(), pList->end());
	}

	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveList(*this, list, ListMergingAlgorithm<T>::m_outputListName));

	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode AlgorithmObjectListMergingAlgorithm<T>::Run()
{
	std::string temporaryListName;
	const MANAGED_CONTAINER<const T *> *pList = NULL;

	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::CreateTemporaryListAndSetCurrent(*this, pList, temporaryListName));

	for(pandora::StringVector::iterator iter = ListMergingAlgorithm<T>::m_inputListNames.begin(), endIter = ListMergingAlgorithm<T>::m_inputListNames.end() ;
			endIter != iter ; ++iter)
	{
		const pandora::StatusCode statusCode(PandoraContentApi::SaveList<T>(*this, *iter, temporaryListName));

		if(statusCode == pandora::STATUS_CODE_NOT_FOUND)
			continue;

		if(statusCode != pandora::STATUS_CODE_SUCCESS)
			return statusCode;
	}

	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveList<T>(*this, ListMergingAlgorithm<T>::m_outputListName));

	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
pandora::StatusCode ListMergingAlgorithm<T>::ReadSettings(const pandora::TiXmlHandle xmlHandle)
{
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ReadVectorOfValues(xmlHandle,
			"InputListNames", m_inputListNames));

	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ReadValue(xmlHandle,
			"OutputListName", m_outputListName));

	return pandora::STATUS_CODE_SUCCESS;
}

}

template pandora::Algorithm *april_content::InputObjectListMergingAlgorithm<pandora::CaloHitList>::Factory::CreateAlgorithm() const;
template pandora::Algorithm *april_content::InputObjectListMergingAlgorithm<pandora::TrackList>::Factory::CreateAlgorithm() const;
template pandora::Algorithm *april_content::InputObjectListMergingAlgorithm<pandora::MCParticleList>::Factory::CreateAlgorithm() const;
template pandora::Algorithm *april_content::AlgorithmObjectListMergingAlgorithm<pandora::Cluster>::Factory::CreateAlgorithm() const;
template pandora::Algorithm *april_content::AlgorithmObjectListMergingAlgorithm<pandora::Pfo>::Factory::CreateAlgorithm() const;
template pandora::Algorithm *april_content::AlgorithmObjectListMergingAlgorithm<pandora::Vertex>::Factory::CreateAlgorithm() const;

template pandora::StatusCode april_content::ListMergingAlgorithm<pandora::CaloHitList>::ReadSettings(const pandora::TiXmlHandle xmlHandle);
template pandora::StatusCode april_content::ListMergingAlgorithm<pandora::TrackList>::ReadSettings(const pandora::TiXmlHandle xmlHandle);
template pandora::StatusCode april_content::ListMergingAlgorithm<pandora::MCParticleList>::ReadSettings(const pandora::TiXmlHandle xmlHandle);
template pandora::StatusCode april_content::ListMergingAlgorithm<pandora::Cluster>::ReadSettings(const pandora::TiXmlHandle xmlHandle);
template pandora::StatusCode april_content::ListMergingAlgorithm<pandora::Pfo>::ReadSettings(const pandora::TiXmlHandle xmlHandle);
template pandora::StatusCode april_content::ListMergingAlgorithm<pandora::Vertex>::ReadSettings(const pandora::TiXmlHandle xmlHandle);

template pandora::StatusCode april_content::InputObjectListMergingAlgorithm<pandora::CaloHitList>::Run();
template pandora::StatusCode april_content::InputObjectListMergingAlgorithm<pandora::TrackList>::Run();
template pandora::StatusCode april_content::InputObjectListMergingAlgorithm<pandora::MCParticleList>::Run();
template pandora::StatusCode april_content::AlgorithmObjectListMergingAlgorithm<pandora::Cluster>::Run();
template pandora::StatusCode april_content::AlgorithmObjectListMergingAlgorithm<pandora::Pfo>::Run();
template pandora::StatusCode april_content::AlgorithmObjectListMergingAlgorithm<pandora::Vertex>::Run();

