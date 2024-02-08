# APRILContent
Algorithm of Particle Reconstruction for ILC - implementation with PandoraSDK

How to compile
0) Make sure that you have mlpack, boost and armadillo installed
1) Clone the repository
2) cd APRILContent; mkdir build; cd build
3) cmake -C ${ILCSOFT}/ILCSoft.cmake -DPANDORA_MONITORING=ON .. -Dmlpack_DIR=ML_INSTALL_DIR -DBoost_INCLUDE_DIR=/cvmfs/ilc.desy.de/sw/x86_64_gcc103_centos7/v02-03-01/boost/1.77.0/ -DARMADILLO_INCLUDE_DIR=/path/to/mlpack/build/deps/armadillo-10.3.0/include
4) make install