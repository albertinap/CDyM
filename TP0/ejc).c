//ejc)
#include <stdio.h>
#include <stdint.h>

void convertirAscii(int x, char ascii[]){
    short unsigned int digito,i=0;
    digito = x % 10;
    while(x!=0){
        ascii[i] = digito + '0';
        i++;
        x=x/10;
        digito = x % 10;
    }
    ascii[i] = '\0'; //fin de string
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
    
    char ascii[4];  //4 dígitos y terminador
    convertirAscii(numero,ascii);
    unsigned short int i=0;
    
    printf("ASCII de ese número: \n");
    while (ascii[i]!='\0'){
        printf("%c ", ascii[i]);
        i++;
    }
    return 0;
}