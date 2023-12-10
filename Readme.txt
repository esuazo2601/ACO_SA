Instrucciones:

- Para compilar utilizar:
g++ ACO.cpp utils.cpp -O3 -static -o ACO

-Argumentos de entrada:

./ACO -i <instancia> -t <tiempo maximo> -h <cantidad de hormigas> -ro <ratio de evaporacion> -alpha <constante relacionada a feromonas> 
-beta <constante relacionada a heuristica> 

-Ejemplo de ejecución:

./ACO -i inst_200_15_4_0.txt -t 60 -h 5 -ro 0.715 -alpha 4 -beta 5 