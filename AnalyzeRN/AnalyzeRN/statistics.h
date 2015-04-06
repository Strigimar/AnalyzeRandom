#ifndef STATISTICS_H
#define STATISTICS_H

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <vector>
//Statistic by cephes.c, i.e. by sts-2.1.2

class Statistics{
private:
	const double MACHEP = 1.11022302462515654042E-16;		// 2**-53
	const double MAXLOG = 7.09782712893383996732224E2;	// log(MAXNUM)
	const double MAXNUM = 1.7976931348623158E308;			// 2**1024*(1-MACHEP)
	const double PI = 3.14159265358979323846;			// pi, duh!
	const double MAXLGM = 2.556348e305;

	const double big = 4.503599627370496e15;
	const double biginv = 2.22044604925031308085e-16;

	int sgngam = 0;

	/* A[]: Stirling's formula expansion of log gamma
	* B[], C[]: log gamma function between 2 and 3
	*/
	static const unsigned short D[20];
	static const unsigned short B[24]; 
	static const unsigned short C[24];

	
	double cephes_igamc(double, double);
	double cephes_igam(double, double);
	double cephes_lgam(double);
	double cephes_polevl(double, double*, int);
	double cephes_p1evl(double, double*, int);

	double phi(double);

public:
	double get_chi2Probability(int deg, double chi) { return cephes_igamc(deg / 2.0, chi / 2.0); }
	bool kolmogor_smirn_test(std::vector<double>);
};

#endif