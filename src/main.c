#include "safetyboard.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

//test body
int main(){
    char* payload = "123456789";
    printf("%d\n", checksum((uint8_t*)payload,9));
    return 0;
}
