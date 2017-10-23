#include "dswf.h"
#include <QtMath>

DSWF::DSWF(double elevationAngle, VectorXd azimuthAngle, int losNum,int heightNum, MatrixXd losVelocity)
{
    this->heightNum = heightNum;
    qDebug() << losVelocity.rows();
    qDebug() << losVelocity.cols();
    vectorVelocity = MatrixXd::Zero(heightNum,3);

    Eigen::Matrix3d SiSum;
    SiSum << 0,0,0,0,0,0,0,0,0;
    Eigen::MatrixXd Si = MatrixXd::Zero(3,losNum);
    for(int i=0;i<losNum;i++){
        Si(0,i) = qSin(qDegreesToRadians(elevationAngle));
        Si(1,i) = qCos(qDegreesToRadians(elevationAngle))*qCos(qDegreesToRadians(azimuthAngle(i)));
        Si(2,i) = qCos(qDegreesToRadians(elevationAngle))*qSin(qDegreesToRadians(azimuthAngle(i)));
        SiSum = SiSum + Si*Si.transpose();
    }

    for(int m=0;m<heightNum;m++) {
        Eigen::Vector3d SiVri(0,0,0);
        Eigen::Vector3d temp(0,0,0);
        for(int n=0;n<losNum;n++) {
            SiVri = SiVri + losVelocity(m,n)*Si.col(n);
        }
        temp = SiSum.inverse()*SiVri;
        vectorVelocity.row(m) = temp;
    }
}
double *DSWF::getHVelocity()
{
    double *HVelocity = new double[heightNum];
    for (int i = 0; i < heightNum; i++) {
        HVelocity[i] = qSqrt(vectorVelocity(i,1)*vectorVelocity(i,1) +
                             vectorVelocity(i,2)*vectorVelocity(i,2));
    }
    return HVelocity;
}

double *DSWF::getHAngle()
{
    double *HAngle = new double[heightNum];
    for (int i = 0; i < heightNum; i++) {
        HAngle[i] = 0.0-qRadiansToDegrees(qAtan2(vectorVelocity(i,2), vectorVelocity(i,1)));
    }
    return HAngle;
}

double *DSWF::getVVelocity()
{
    double *VVelocity = new double[heightNum];
    for (int i = 0; i < heightNum; i++) {
        VVelocity[i] = vectorVelocity(i,0);
    }
    return VVelocity;
}
