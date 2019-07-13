/*#include <Rcpp.h>*/
#include <RcppArmadillo.h>
#include <cmath>

using std::sqrt;
using namespace Rcpp;

// [[Rcpp::export]]
double C_midrqLoss(NumericVector b, NumericMatrix G, NumericMatrix x, NumericVector yo, NumericVector offset, int type, double tau, int n, int p, int k){

	NumericVector Rn(n);
	NumericVector xb(n);
	NumericVector midp(n);
	NumericVector Iy(n);
	double midp0 = 0;
	double midp1 = 0;
	double val = 0;
		
	/* linear predictor */
	for(int i = 0; i < n; ++i){
		for(int j = 0; j < p; ++j){
			xb[i] += x(i,j)*b[j];
		}
		xb[i] = xb[i] + offset[i];
	}
	
	/* interpolation */
	for(int m = 0; m < n; ++m){
		double v = xb[m];
		int i = 0;
		int j = k - 1;

		/* handle out-of-domain points */
		if(v < yo[i]) {midp[m] = G(m,i);}
		else if(v > yo[j]) {midp[m] = G(m,j);}
		else {
			/* find the correct interval by bisection */
			while(i < j - 1) {
				/* yo[i] <= v <= yo[j] */
				int ij = (i+j)/2;
				/* i+1 <= ij <= j-1 */
				if(v < yo[ij]) {j = ij;} else {i = ij;}
				/* still i < j */
			}
			midp0 = G(m,i);
			midp1 = G(m,j);

			midp[m] = midp0 + (midp1 - midp0) * ((v - yo[i])/(yo[j] - yo[i]));
		}
		
	Iy[m] = tau - midp[m];
	}
	
	/* objective function */
	
	for(int i = 0; i < n; ++i){
		if(type == 1) {Rn[i] = Iy[i];}
		else {
			IntegerVector Ix(n);
			for(int j = 0; j < n; ++j){
				for(int k = 0; k < p; ++k){
					Ix[j] += x(j,k) <= x(i,k);
				}
			}

			for(int j = 0; j < n; ++j){
				Rn[i] += (Ix[j] == p)*Iy[j]/n;
			}
		}
		val += pow(Rn[i], 2)/n;
	
	}

	return val;
}

// [[Rcpp::export]]
double C_midrqLoss_bc(NumericVector b, NumericMatrix G, NumericMatrix x, NumericVector yo, NumericVector offset, int type, double tau, double lambda, int n, int p, int k){

	NumericVector Rn(n);
	NumericVector xb(n);
	NumericVector midp(n);
	NumericVector Iy(n);
	double midp0 = 0;
	double midp1 = 0;
	double val = 0;
		
	/* linear predictor */
	for(int i = 0; i < n; ++i){
		for(int j = 0; j < p; ++j){
			xb[i] += x(i,j)*b[j];
		}
		xb[i] = xb[i] + offset[i];
		if(lambda == 0){
			xb[i] = exp(xb[i]);
		} else {
			if(lambda*xb[i] + 1 < 0){xb[i] = 0;}
			xb[i] = pow(lambda * xb[i] + 1, 1/lambda);
		}
	}
	
	/* interpolation */
	for(int m = 0; m < n; ++m){
		double v = xb[m];
		int i = 0;
		int j = k - 1;

		/* handle out-of-domain points */
		if(v < yo[i]) {midp[m] = G(m,i);}
		else if(v > yo[j]) {midp[m] = G(m,j);}
		else {
			/* find the correct interval by bisection */
			while(i < j - 1) {
				/* yo[i] <= v <= yo[j] */
				int ij = (i+j)/2;
				/* i+1 <= ij <= j-1 */
				if(v < yo[ij]) {j = ij;} else {i = ij;}
				/* still i < j */
			}
			midp0 = G(m,i);
			midp1 = G(m,j);

			midp[m] = midp0 + (midp1 - midp0) * ((v - yo[i])/(yo[j] - yo[i]));
		}
		
	Iy[m] = tau - midp[m];
	}
	
	/* objective function */
	
	for(int i = 0; i < n; ++i){
		if(type == 1) {Rn[i] = Iy[i];}
		else {
			IntegerVector Ix(n);
			for(int j = 0; j < n; ++j){
				for(int k = 0; k < p; ++k){
					Ix[j] += x(j,k) <= x(i,k);
				}
			}

			for(int j = 0; j < n; ++j){
				Rn[i] += (Ix[j] == p)*Iy[j]/n;
			}
		}
		val += pow(Rn[i], 2)/n;
	
	}

	return val;
}

// [[Rcpp::export]]
double C_midrqLoss_ao(NumericVector b, NumericMatrix G, NumericMatrix x, NumericVector yo, NumericVector offset, int type, double tau, double lambda, int n, int p, int k){

	NumericVector Rn(n);
	NumericVector xb(n);
	NumericVector midp(n);
	NumericVector Iy(n);
	double midp0 = 0;
	double midp1 = 0;
	double val = 0;
	double y = 0;
	double A1 = 0;
	double A2 = 0;
		
	/* linear predictor */
	for(int i = 0; i < n; ++i){
		for(int j = 0; j < p; ++j){
			xb[i] += x(i,j)*b[j];
		}
		xb[i] = xb[i] + offset[i];
		if(lambda == 0){
			xb[i] = 1/(1 + exp(-xb[i]));
		} else {
			y = (lambda * xb[i]/2);
			if(y <= -1){xb[i] = 0;}
				else if(y >= 1){xb[i] = 1;}
					else {
					A1 = pow(1 + y, 1/lambda);
					A2 = pow(1 - y, 1/lambda);
					xb[i] = A1/(A1 + A2);}
			
			
		}
	}
	
	/* interpolation */
	for(int m = 0; m < n; ++m){
		double v = xb[m];
		int i = 0;
		int j = k - 1;

		/* handle out-of-domain points */
		if(v < yo[i]) {midp[m] = G(m,i);}
		else if(v > yo[j]) {midp[m] = G(m,j);}
		else {
			/* find the correct interval by bisection */
			while(i < j - 1) {
				/* yo[i] <= v <= yo[j] */
				int ij = (i+j)/2;
				/* i+1 <= ij <= j-1 */
				if(v < yo[ij]) {j = ij;} else {i = ij;}
				/* still i < j */
			}
			midp0 = G(m,i);
			midp1 = G(m,j);

			midp[m] = midp0 + (midp1 - midp0) * ((v - yo[i])/(yo[j] - yo[i]));
		}
		
	Iy[m] = tau - midp[m];
	}
	
	/* objective function */
	
	for(int i = 0; i < n; ++i){
		if(type == 1) {Rn[i] = Iy[i];}
		else {
			IntegerVector Ix(n);
			for(int j = 0; j < n; ++j){
				for(int k = 0; k < p; ++k){
					Ix[j] += x(j,k) <= x(i,k);
				}
			}

			for(int j = 0; j < n; ++j){
				Rn[i] += (Ix[j] == p)*Iy[j]/n;
			}
		}
		val += pow(Rn[i], 2)/n;
	
	}

	return val;
}

// [[Rcpp::depends("RcppArmadillo")]]
// [[Rcpp::export]]
List C_rcTest(NumericMatrix x, NumericVector psi, NumericMatrix omega, int n, int p, int B){

	NumericMatrix out(p,p);
	arma::cube outstar(p, p, B, arma::fill::zeros);
	NumericMatrix S(p,p);
	
	for(int i = 0; i < n; ++i){
		
		IntegerVector Ix(n);
		NumericVector Rn(p);
	
		for(int j = 0; j < n; ++j){
			for(int k = 0; k < p; ++k){
				Ix[j] += x(j,k) <= x(i,k);
			}
		}

		for(int k = 0; k < p; ++k){
			for(int j = 0; j < n; ++j){
				Rn[k] += x(j,k)*(Ix[j] == p)*psi[j]/sqrt(static_cast<double>(n));
			}
		}	

		for(int k = 0; k < p; ++k){
			for(int j = 0; j < p; ++j){
				out(j,k) += Rn[j]*Rn[k];
			}
		}

		for(int k = 0; k < p; ++k){
			for(int j = 0; j < p; ++j){
				double tmp = 0;
				for(int h = 0; h < n; ++h){
					tmp += x(h,j)*x(h,k)*(Ix[h] == p)/n;
				}
				S(j,k) = tmp;
			}
		}
		
		for(int k = 0; k < B; ++k){
					
			NumericMatrix Sx(p,n);

			for(int h = 0; h < p; ++h){
				for(int j = 0; j < n; ++j){
					for(int l = 0; l < p; ++l){
						Sx(h,j) += S(h,l)*x(j,l)*omega(j,k);
					}
				}
			}
			
			NumericVector Rstar(p);
			
			for(int h = 0; h < p; ++h){
				for(int j = 0; j < n; ++j){
					Rstar(h) += (omega(j,k)*(Ix[j] == p)*x(j,h) - Sx(h,j))/sqrt(static_cast<double>(n));
				}
			}
			
			for(int h = 0; h < p; ++h){
				for(int j = 0; j < p; ++j){
					outstar(j,h,k) += Rstar[j]*Rstar[h];
				}
			}
		
		}


	}

	List ans;
	ans["out"] = out;
	ans["outstar"] = outstar;
	return ans;

}
