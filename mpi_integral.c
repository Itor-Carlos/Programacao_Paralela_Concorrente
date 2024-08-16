#include <stdio.h>
#include <mpi.h>

double integral_local(double local_a, double local_b, int local_n, double base_len);    

double f(double x); 

int main(void) {
    int my_rank, comm_sz, n = 1024, numeroTrapezios;
    double a = 0.0, b = 3.0, tamanhoTrapezio, comecoSubIntervalo, finalSubIntervalo;
    double integralParcial, resultadoFinal;
    int source;

    MPI_Init(NULL, NULL);

    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    tamanhoTrapezio = (b-a)/n;          
    numeroTrapezios = n/comm_sz;  

    comecoSubIntervalo = a + my_rank*numeroTrapezios*tamanhoTrapezio;
    finalSubIntervalo = comecoSubIntervalo + numeroTrapezios*tamanhoTrapezio;
    integralParcial = integral_local(comecoSubIntervalo, finalSubIntervalo, numeroTrapezios, tamanhoTrapezio);

    if (my_rank != 0) {
        MPI_Send(&integralParcial, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    } else {
        resultadoFinal = integralParcial;
        for (source = 1; source < comm_sz; source++) {
            MPI_Recv(&integralParcial, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            resultadoFinal += integralParcial;
        }
        printf("Integral de (x*x -4*x+8) no intervalo de %f a %f = %.4f\n", a, b, resultadoFinal);
    }

    MPI_Finalize();

    return 0;
}


double integral_local(
      double local_a , 
      double local_b, 
      int    local_n , 
      double h   ) {
    double integral;
    double x;
    int i;

    integral = (f(local_a) + f(local_b))/2.0;
    x = local_a;
    for (i = 1; i <= local_n-1; i++) {
        x += h;
        integral += f(x);
    }
    integral = integral*h;

    return integral;
} /*  integral_local  */


double f(double x) {
   return x*x - 4*x + 8;
} /* f */