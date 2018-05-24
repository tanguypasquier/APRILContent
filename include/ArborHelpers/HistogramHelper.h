#ifndef HistogramManager_h 
#define HistogramManager_h 

#include <string>

#include "TNtuple.h"

class TDirectory;

using namespace std;

namespace arbor_content
{

class HistogramManager {

private:

	std::vector<TNtuple*> tupleVec;

public:
    HistogramManager(const char * filename="Monitoring.root");
    virtual ~HistogramManager();

    // Create (if doesn't exist) and Fill a fixed bin histogram (TH1F or TH2F)
    void CreateFill(string tupleName, string varNamelist, std::vector<float> varList);

    void Reset();
    void Write();
};

}

#endif 		// HistogramManager_h

