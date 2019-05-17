#ifndef ARBOR_DBSCAN_H
#define ARBOR_DBSCAN_H

// this an example to use mlpack and dlib
// For dlib, we have to define __COMPILE_DLIB__ from cmake configuration.

namespace arbor_content
{

class ArborDBSCAN
{
public:

	ArborDBSCAN();

#if __COMPILE_DLIB__
	void DoClustering();
	void DoGraph();
#endif

private:

};

}

#endif
