#pragma once

class CShootingPath
{
public:
	CShootingPath();
	CShootingPath(double orient,double x1, double y1, double x2, double y2);
	
	double length() const
	{
		return r * d_theta;
	}
	void interpolate(double dt, double *result);	
	
	double r,d_theta;
	double t1,t2;
	double x0,x1,x2;
	double y0,y1,y2;
	int coor;	//1 == 1\BB\E7\BAи\E9 == \BF\C0\B8\A5	//2 == 2\BB\E7\BAи\E9 == \BF\DE
	static double mod2pi(double x);
	
private:
	static const double pi = 3.1415926535897932384626433832795;
	static const double twopi = 2. * 3.1415926535897932384626433832795;
	static const double SHOOTING_ZERO = -1e-9;
	static const double min_r = 0;
	static const double max_r = 1e+5;
};
