#ifndef DSWF_H
#define DSWF_H
#include <eigen/Eigen>
#include <Eigen/Dense>
#include <math.h>
#include <iostream>
#include <QDebug>

const double PI = 3.1415926;
using namespace Eigen;
using namespace std;

class DSWF
{
public:
    DSWF(double elevationAngle, double *azAngle, int losNum, int heightNum, double *losVelocityMat);
    double *getHVelocity();
    double *getHAngle();
    double *getVVelocity();
private:
    MatrixXd vectorVelocity;
    int heightNum;
};
#endif // DSWF_H
