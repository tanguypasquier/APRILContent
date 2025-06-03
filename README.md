# APRILContent
Algorithm of Particle Reconstruction for ILC - implementation with PandoraSDK

How to compile

1) Make sure that you have mlpack, boost and armadillo installed
2) Clone the repository
3) cd APRILContent; mkdir build; cd build
4) PKG_CONFIG_PATH=/path/to/MLPACK/pkgconfig cmake -C ${ILCSOFT}/ILCSoft.cmake -DPANDORA_MONITORING=ON .. -DBoost_INCLUDE_DIR=/path/to/boost/include -DARMADILLO_INCLUDE_DIR=/path/to/mlpack/build/deps/armadillo-10.3.0/include
5) make install
