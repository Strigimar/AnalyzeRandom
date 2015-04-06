#include "statistics.h"
#include <algorithm>
#include <map>
#include <float.h>

const unsigned short Statistics::D[] = { 
	0x6661, 0x2733, 0x9850, 0x3f4a, 
	0xe943, 0xb580, 0x7fbd, 0xbf43, 
	0x5ebb, 0x20dc, 0x019f, 0x3f4a, 
	0xa5a1, 0x16b0, 0xc16c, 0xbf66, 
	0x554b, 0x5555, 0x5555, 0x3fb5 
};
const unsigned short Statistics::B[] = {
	0x6761, 0x8ff3, 0x8901, 0xc095,
	0xb93e, 0x355b, 0xf234, 0xc0e2,
	0x89e5, 0xf890, 0x3d73, 0xc114,
	0xdb51, 0xf994, 0xbc82, 0xc131,
	0xf20b, 0x0219, 0x4589, 0xc13a,
	0x055e, 0x5418, 0x0c67, 0xc12a
};
const unsigned short Statistics::C[] = {
	/*0x0000,0x0000,0x0000,0x3ff0,*/
	0x12b2, 0x1cf3, 0xfd0d, 0xc075,
	0xd757, 0x7b89, 0xaa0d, 0xc0d0,
	0x4c9b, 0xb974, 0xeb84, 0xc10a,
	0x0043, 0x7195, 0x6286, 0xc131,
	0xf34c, 0x892f, 0x5255, 0xc143,
	0xe14a, 0x6a11, 0xce4b, 0xc13e
};

double Statistics::cephes_igamc(double a, double x)
{
	double ans, ax, c, yc, r, t, y, z;
	double pk, pkm1, pkm2, qk, qkm1, qkm2;

	if ((x <= 0) || (a <= 0))
		return(1.0);

	if ((x < 1.0) || (x < a))
		return(1.e0 - cephes_igam(a, x));

	ax = a * log(x) - x - cephes_lgam(a);

	/*if (ax < -MAXLOG) {
		std::cout<< "igamc: UNDERFLOW\n";
		return 0.0;
	}*/
	ax = exp(ax);

	/* continued fraction */
	y = 1.0 - a;
	z = x + y + 1.0;
	c = 0.0;
	pkm2 = 1.0;
	qkm2 = x;
	pkm1 = x + 1.0;
	qkm1 = z * x;
	ans = pkm1 / qkm1;

	do {
		c += 1.0;
		y += 1.0;
		z += 2.0;
		yc = y * c;
		pk = pkm1 * z - pkm2 * yc;
		qk = qkm1 * z - qkm2 * yc;
		if (qk != 0) {
			r = pk / qk;
			t = fabs((ans - r) / r);
			ans = r;
		}
		else
			t = 1.0;
		pkm2 = pkm1;
		pkm1 = pk;
		qkm2 = qkm1;
		qkm1 = qk;
		if (fabs(pk) > big) {
			pkm2 *= biginv;
			pkm1 *= biginv;
			qkm2 *= biginv;
			qkm1 *= biginv;
		}
	} while (t > MACHEP);

	return ans*ax;
}

double Statistics::cephes_igam(double a, double x)
{
	double ans, ax, c, r;

	if ((x <= 0) || (a <= 0))
		return 0.0;

	if ((x > 1.0) && (x > a))
		return 1.e0 - cephes_igamc(a, x);

	/* Compute  x**a * exp(-x) / gamma(a)  */
	ax = a * log(x) - x - cephes_lgam(a);
	/*if (ax < -MAXLOG) {
		std::cout << "igam underflow\n";
		//printf("igam: UNDERFLOW\n");
		return 0.0;
	}*/
	ax = exp(ax);

	/* power series */
	r = a;
	c = 1.0;
	ans = 1.0;

	do {
		r += 1.0;
		c *= x / r;
		ans += c;
	} while (c / ans > MACHEP);

	return ans * ax / a;
}

/* Logarithm of gamma function */
double Statistics::cephes_lgam(double x)
{
	double	p, q, u, w, z;
	int		i;

	sgngam = 1;

	if (x < -34.0) {
		q = -x;
		w = cephes_lgam(q); /* note this modifies sgngam! */
		p = floor(q);
		if (p == q) {
		lgsing:
			goto loverf;
		}
		i = (int)p;
		if ((i & 1) == 0)
			sgngam = -1;
		else
			sgngam = 1;
		z = q - p;
		if (z > 0.5) {
			p += 1.0;
			z = p - q;
		}
		z = q * sin(PI * z);
		if (z == 0.0)
			goto lgsing;
		/*      z = log(PI) - log( z ) - w;*/
		z = log(PI) - log(z) - w;
		return z;
	}

	if (x < 13.0) {
		z = 1.0;
		p = 0.0;
		u = x;
		while (u >= 3.0) {
			p -= 1.0;
			u = x + p;
			z *= u;
		}
		while (u < 2.0) {
			if (u == 0.0)
				goto lgsing;
			z /= u;
			p += 1.0;
			u = x + p;
		}
		if (z < 0.0) {
			sgngam = -1;
			z = -z;
		}
		else
			sgngam = 1;
		if (u == 2.0)
			return(log(z));
		p -= 2.0;
		x = x + p;
		p = x * cephes_polevl(x, (double *)B, 5) / cephes_p1evl(x, (double *)C, 6);

		return log(z) + p;
	}

	if (x > MAXLGM) {
	loverf:
		printf("lgam: OVERFLOW\n");

		return sgngam * MAXNUM;
	}

	q = (x - 0.5) * log(x) - x + log(sqrt(2 * PI));
	if (x > 1.0e8)
		return q;

	p = 1.0 / (x*x);
	if (x >= 1000.0)
		q += ((7.9365079365079365079365e-4 * p
		- 2.7777777777777777777778e-3) *p
		+ 0.0833333333333333333333) / x;
	else
		q += cephes_polevl(p, (double *)D, 4) / x;

	return q;
}

double Statistics::cephes_polevl(double x, double *coef, int N)
{
	double	ans;
	int		i;
	double	*p;

	p = coef;
	ans = *p++;
	i = N;

	do
		ans = ans * x + *p++;
	while (--i);

	return ans;
}

double Statistics::cephes_p1evl(double x, double *coef, int N)
{
	double	ans;
	double	*p;
	int		i;

	p = coef;
	ans = x + *p++;
	i = N - 1;

	do
		ans = ans * x + *p++;
	while (--i);

	return ans;
}
bool Statistics::kolmogor_smirn_test(std::vector<double> values)
{
	int count = values.size();
	double mean = 0.0, variance = 0.0;

	std::sort(values.begin(), values.end());

	std::map<double, int> freq_p_values;

	std::for_each(values.begin(), values.end(), [&](double it){
		mean += (it / count);
		variance += (pow(it, 2) / (double)(count - 1));

		std::map<double, int>::iterator itMap = freq_p_values.find(it);
		if (itMap != freq_p_values.end())
			itMap->second++;
		else
			freq_p_values.insert(std::make_pair(it, 1));
		
	});
	variance -= (((double)count / (count - 1))*pow(mean, 2));
	double Dn = DBL_MIN;

	int sum = 0;
	std::for_each(freq_p_values.begin(), freq_p_values.end(), [&](std::pair<double, int> pair){
		sum += pair.second;
		double Fn = ((double)sum / count);
		double Sn = phi(((pair.first - mean) / sqrt(variance)));
		double d = abs(Fn - Sn);
		if (d > Dn)
			Dn = d;
	});
	
	// thanks to high count (n) = 1000, we can use 1.36/sqrt(n)
	double approximately = (1.36 / sqrt(count));
	return Dn > approximately;


}
double Statistics::phi(double x)
{
	// constants
	double a1 = 0.254829592;
	double a2 = -0.284496736;
	double a3 = 1.421413741;
	double a4 = -1.453152027;
	double a5 = 1.061405429;
	double p = 0.3275911;

	// Save the sign of x
	int sign = 1;
	if (x < 0)
		sign = -1;
	x = fabs(x) / sqrt(2.0);

	// A&S formula 7.1.26
	double t = 1.0 / (1.0 + p*x);
	double y = 1.0 - (((((a5*t + a4)*t) + a3)*t + a2)*t + a1)*t*exp(-x*x);

	return 0.5*(1.0 + sign*y);
}