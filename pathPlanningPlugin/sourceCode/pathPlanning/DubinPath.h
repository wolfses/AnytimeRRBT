
#pragma once

class CDubinPath
{
public:
	CDubinPath();
	CDubinPath(double x1,double y1,double t1,double x2,double y2,double t2);

	double length() const
	{
		return value[0] + value[1] + value[2];
	}
	enum DubinsPathSegmentType { DUBINS_TYPE_ERR=-1, DUBINS_LEFT=0, DUBINS_STRAIGHT=1, DUBINS_RIGHT=2 };
	void interpolate(double dt, double *result);	
	
	static double rho_;	
	double xyt[3];
	double value[3];
	DubinsPathSegmentType type[3];
	
	static double mod2pi(double x);
private:
	void CDubinPath_(double d, double alpha, double beta);
	void setValue(double t,double p,double q);
	double dubinsLSL(double d,double alpha,double beta);
	double dubinsRSR(double d,double alpha,double beta);
	double dubinsRSL(double d,double alpha,double beta);
	double dubinsLSR(double d,double alpha,double beta);
	double dubinsRLR(double d,double alpha,double beta);
	double dubinsLRL(double d,double alpha,double beta);	

private:
	static const double twopi = 2. * 3.1415926535897932384626433832795;
	static const double DUBINS_EPS = 1e-6;
	static const double DUBINS_ZERO = -1e-9;
	static const double DUBINS_ERR = 1e+100;
};
