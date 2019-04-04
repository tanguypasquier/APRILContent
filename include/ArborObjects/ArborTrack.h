/**
 * @file LCContent/LCObjects/include/ArborTrack.h
 *
 * @brief Header file for LC Track class
 *
 */

#ifndef ARBOR_TRACK_H
#define ARBOR_TRACK_H 1

#include "Objects/Track.h"
#include "Api/PandoraApi.h"

#include "Pandora/ObjectCreation.h"
#include "Pandora/PandoraObjectFactories.h"

#include "Persistency/BinaryFileReader.h"
#include "Persistency/BinaryFileWriter.h"
#include "Persistency/XmlFileReader.h"
#include "Persistency/XmlFileWriter.h"


namespace arbor_content
{

typedef std::vector<pandora::InputTrackState> LCInputTrackStates;
typedef std::vector<pandora::TrackState> ArborTrackStates;


/**
 *  @brief  ArborTrack Parameters, allow multiple track states at the calorimeter
 */
class ArborTrackParameters : public object_creation::Track::Parameters
{
public:
    LCInputTrackStates m_trackStates;       ///< Vector of TrackStates
};

/**
 *  @brief  ArborTrack extension of the Track class for LC-content
 */
class ArborTrack: public object_creation::Track::Object
{

public:
    ArborTrack(const ArborTrackParameters &parameters);
    virtual ~ArborTrack() {};

    const ArborTrackStates &GetTrackStates() const;

protected:

    ArborTrackStates m_trackStates;

};

/**
 *  @brief  ArborTrackFactory responsible for ArborTrack creation
 */
class ArborTrackFactory : public pandora::ObjectFactory<object_creation::Track::Parameters, object_creation::Track::Object>
{
public:
    /**
     *  @brief  Create new parameters instance on the heap (memory-management to be controlled by user)
     *
     *  @return the address of the new parameters instance
     */
    Parameters *NewParameters() const;

    /**
     *  @brief  Read any additional (derived class only) object parameters from file using the specified file reader
     *
     *  @param  parameters the parameters to pass in constructor
     *  @param  fileReader the file reader, used to extract any additional parameters from file
     */
    pandora::StatusCode Read(Parameters &, pandora::FileReader &) const;

    /**
     *  @brief  Persist any additional (derived class only) object parameters using the specified file writer
     *
     *  @param  pObject the address of the object to persist
     *  @param  fileWriter the file writer
     */
    pandora::StatusCode Write(const Object *const , pandora::FileWriter &) const;

    /**
     *  @brief  Create an object with the given parameters
     *
     *  @param  parameters the parameters to pass in constructor
     *  @param  pObject to receive the address of the object created
     */
    pandora::StatusCode Create(const Parameters &parameters, const Object *&pObject) const;
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline ArborTrack::ArborTrack(const ArborTrackParameters &parameters) :
    object_creation::Track::Object(parameters),
    m_trackStates()
{
    for (auto const& inputTrackState: parameters.m_trackStates ) {
        m_trackStates.push_back( inputTrackState.Get() );
    }

}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const ArborTrackStates &ArborTrack::GetTrackStates() const
{
    return m_trackStates;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline ArborTrackFactory::Parameters *ArborTrackFactory::NewParameters() const
{
    return (new ArborTrackParameters);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::StatusCode ArborTrackFactory::Create(const Parameters &parameters, const Object *&pObject) const
{
    const ArborTrackParameters &lcTrackParameters(dynamic_cast<const ArborTrackParameters&>(parameters));
    pObject = new ArborTrack(lcTrackParameters);

    return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::StatusCode ArborTrackFactory::Read(Parameters &parameters, pandora::FileReader &fileReader) const
{
    // ATTN: To receive this call-back must have already set file reader track factory to this factory
    LCInputTrackStates trackStates;
    int nTrackStates;
    if (pandora::BINARY == fileReader.GetFileType())
    {
        pandora::BinaryFileReader &binaryFileReader(dynamic_cast<pandora::BinaryFileReader&>(fileReader));
        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, binaryFileReader.ReadVariable(nTrackStates));
        for (int i = 0; i < nTrackStates; ++i)
        {
            pandora::TrackState trackState(0.0,0.0,0.0,0.0,0.0,0.0);
            PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, binaryFileReader.ReadVariable(trackState));
            trackStates.push_back( pandora::InputTrackState(trackState) );
        }
    }
    else if (pandora::XML == fileReader.GetFileType())
    {
        pandora::XmlFileReader &xmlFileReader(dynamic_cast<pandora::XmlFileReader&>(fileReader));
        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, xmlFileReader.ReadVariable("NumberOfTrackStates", nTrackStates));
        for (int i = 0; i < nTrackStates; ++i)
        {
            pandora::TrackState trackState(0.0,0.0,0.0,0.0,0.0,0.0);
            std::stringstream trackStateName;
            trackStateName << "TrackState" << i;
            PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, xmlFileReader.ReadVariable(trackStateName.str(), trackState));
            trackStates.push_back( pandora::InputTrackState(trackState) );
        }
    }
    else
    {
        return pandora::STATUS_CODE_INVALID_PARAMETER;
    }

    ArborTrackParameters &lcTrackParameters(dynamic_cast<ArborTrackParameters&>(parameters));
    lcTrackParameters.m_trackStates = trackStates;

    return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::StatusCode ArborTrackFactory::Write(const Object *const pObject, pandora::FileWriter &fileWriter) const
{
    // ATTN: To receive this call-back must have already set file writer track factory to this factory
    const ArborTrack *const pArborTrack(dynamic_cast<const ArborTrack*>(pObject));

    if (!pArborTrack)
        return pandora::STATUS_CODE_INVALID_PARAMETER;

    const ArborTrackStates& trackStates = pArborTrack->GetTrackStates();
    int nTrackStates = trackStates.size();

    if (pandora::BINARY == fileWriter.GetFileType())
    {
        pandora::BinaryFileWriter &binaryFileWriter(dynamic_cast<pandora::BinaryFileWriter&>(fileWriter));
        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, binaryFileWriter.WriteVariable(nTrackStates));
        for (auto const& trackState : trackStates)
        {
            PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, binaryFileWriter.WriteVariable(trackState));
        }
    }
    else if (pandora::XML == fileWriter.GetFileType())
    {
        pandora::XmlFileWriter &xmlFileWriter(dynamic_cast<pandora::XmlFileWriter&>(fileWriter));
        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, xmlFileWriter.WriteVariable("NumberOfTrackStates", nTrackStates));
        int trackStateCounter=0;
        for (auto const& trackState : trackStates)
        {
          std::stringstream trackStateName;
          trackStateName << "TrackState" << trackStateCounter;
          PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, xmlFileWriter.WriteVariable(trackStateName.str(), trackState));
          ++trackStateCounter;
        }
    }
    else
    {
        return pandora::STATUS_CODE_INVALID_PARAMETER;
    }

    return pandora::STATUS_CODE_SUCCESS;
}

}//namespace



#endif
