
#include<kernel.h>
#define RAND_MAX 32767


double pow(double x, int y) {
    if (y == 0) {
        return 1.0; // x^0 is always 1
    }

    double result = 1.0;
    int exponent = (y > 0) ? y : -y; // Use the absolute value of y
    int i = 1;

    for ( i = 1; i <= exponent; i++) {
        result *= x;
    }

    return (y > 0) ? result : 1.0 / result; // Adjust the result based on the sign of y
}

double log(double x) {
  if (x<=0){
    return -1;
  }
  double sum = 0;
  int i = 1;
  for ( i = 1; i <= 20; i++) {
    sum += ((pow(-1,(i + 1)) * pow(x - 1, i)) / i);
  }
  return sum;
}


double expdev(double lambda) {
    double dummy;
    do
        dummy= (double) rand() / RAND_MAX;
    while (dummy == 0.0);
    return -log(dummy) / lambda;
}