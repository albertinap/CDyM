//ejd)
#include <stdio.h>
#include <unistd.h>

void imprimir(int x){
    short unsigned int digito,i;
    digito = x;
    for (i=1;i<=8;i++){
        digito &= 0x1;
        printf("%u \n", digito);
        digito = x >> i;
        sleep(1);
    }
}

int main() {
    char aux[9];      // 8 bits + '\0'
    int numero = 0;

    printf("Ingresa un numero binario de 8 bits: ");
    scanf("%8s", aux);

    for(int i = 0; i < 8; i++){
        numero = numero << 1;      // desplaza a la izquierda
        numero |= (aux[i] - '0');  // agrega el bit
    }

    printf("El numero binario ingresado: %s\n", aux);
    printf("Convertido a entero decimal: %d\n", numero);
 
    imprimir(numero);
    
    return 0;
}