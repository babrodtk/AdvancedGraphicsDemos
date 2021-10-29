#include <cstdlib>
#include <iostream>
#include <vector>
#include <windows.h>
#include <time.h>

using std::endl;
using std::cout;
using std::runtime_error;
using std::vector;

long long myFunction1(int n);
long long myFunction2(int n);
long long myFunction3(int n);
long long myFunction4(int n);
long long myFunction5(int n);
long long myFunction6(int n);
double getCurrentTime();

int main(int argc, char**argv) {
	double start, end;
	vector<long long> sums;
	int N = 0;

	if (argc != 2) {
		throw runtime_error("Wrong number of arguments");
	}
	else {
		N = atoi(argv[1]);
	}

	//Compute the sums
	start = getCurrentTime();
	sums.resize(N);
	for (int k=0; k<N; ++k) 
		sums.at(k) = myFunction1(k);
	end = getCurrentTime();

	//Print out the first and last five items in the list
	for (int k=0; k<5; ++k)
		cout << "sum_0^" << k << " (i+i^2+i^4) = " << sums.at(k) << endl;
	cout << "..." << endl;
	for (int k=N-5; k<N; ++k)
		cout << "sum_0^" << k << " (i+i^2+i^4) = " << sums.at(k) << endl;
	cout << "Computed in " << end-start << " seconds." << endl;

	return 0;
}

long long f(long long i) {
	return i + pow(i, 2.0) + pow(i, 4.0);
}

//Compute sum_0^n i+i^2
long long myFunction1(int n) {
	long long retval = 0;
	for (long long i=0; i<=n; ++i)
		retval += f(i);
	return retval;
}


long long g(long long i) {
	return i + i*i + i*i*i*i;
}

//Compute sum_0^n i+i^2
long long myFunction2(int n) {
	long long retval = 0;
	for (long long i=0; i<=n; ++i)
		retval += g(i);
	return retval;
}

inline long long h(long long i) {
	return i + i*i + i*i*i*i;
}

//Compute sum_0^n i+i^2
long long myFunction3(int n) {
	long long retval = 0;
	for (long long i=0; i<=n; ++i)
		retval += h(i);
	return retval;
}

//Compute sum_0^n i+i^2
inline long long myFunction4(int n) {
	long long retval = 0;
	for (long long i=0; i<=n; ++i)
		retval += h(i);
	return retval;
}


inline long long j(long long i) {
	long long ii = i*i;
	return i + ii + ii*ii;
}

//Compute sum_0^n i+i^2
inline long long myFunction5(int n) {
	long long retval = 0;
	for (long long i=0; i<=n; ++i)
		retval += j(i);
	return retval;
}


//Compute sum_0^n i+i^2
inline long long myFunction6(int n) {
	long long retval = 0;
	long long nn = n*n;
	long long nnn = nn*n;
	retval += (nn+n)/2.0;
	retval += nnn/3.0 + nn/2.0 + n/6.0;
	retval += nnn*nn/5.0 + nn*nn/2.0 + nnn/3.0 - n/30.0;
	return retval;
}


double getCurrentTime() {
    LARGE_INTEGER f;
    LARGE_INTEGER t;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&t);
    return t.QuadPart/(double) f.QuadPart;
}