#include "schemas.h"
#include "stdlib.h"
#include "stdio.h"
#include "assert.h"

int schemaByteLength(const Schema* schema) {

    int totalBytes = 1; // first '<'

    for (int i = 0; i < schema->numElements; i++) {
        totalBytes += schema->elements[i].width + 1; // bit width + '|' or '>'
    }

    return totalBytes;

}

char* schemaToBytes(const Schema* schema) {

    int length = schemaByteLength(schema);
    char* dataBytes = (char*)malloc(length * sizeof(char));

    // beginning of schema
    dataBytes[0] = '<';

    // iterate through each field of schema
    int dataPointer = 1; 
    for (int i = 0; i < schema->numElements; i++) {

        SchemaElement element = schema->elements[i];

        // format fixed int width and set appropriate bytes in data
        if (element.type == INT) {
            int value = *(int*)element.value; // cast void pointer to int* and dereference
            snprintf(
                dataBytes[dataPointer], // first element of buffer
                element.width, // number of bytes to write
                "%*d", // variable length integer...
                element.width, // ...with this many bytes...
                value); // ...and this value
        }

        // format fixed float width and set appropriate bytes in data
        else if (element.type == FLOAT) {
            float value = *(float*)element.value; // cast void pointer to float* and dereference
            int numLeadingDigits = element.width - element.decimalPlaces - 1;
            int numDecimalDigits = element.decimalPlaces;
            snprintf(
                dataBytes[dataPointer], // first element of buffer
                element.width, // number of bytes to write
                "%*.*f", // variable percision float...
                numLeadingDigits, numDecimalDigits, // ...with this many leading/trailing digits...
                value); //...and this value
        }

        // advance data pointer by 1 and add a '|' character
        dataPointer += element.width;
        dataBytes[++dataPointer] = '|';

    }

    // last character is '>'
    dataBytes[dataPointer] = '>';

    return dataBytes;

}

void bytesToSchema(Schema* schema, char* dataBytes) {

    int length = schemaByteLength(schema);

    // sanity check: leading and trailing '<' and '>'?
    assert(dataBytes[0] == '<');
    assert(dataBytes[length - 1] == '>');

    // iterate through each field of schema
    int dataPointer = 1; 
    for (int i = 0; i < schema->numElements; i++) {

        SchemaElement element = schema->elements[i];

        // regardless of int or float, need to null-terminate string
        // replace | or > character with null character
        dataBytes[dataPointer + element.width] = 0;

        // format fixed int width and set appropriate bytes in data
        if (element.type == INT) {
            int* valuePtr = (int*)malloc(sizeof(int));
            *valuePtr = atoi((const char *)(dataBytes[dataPointer]));
            element.value = valuePtr;
        }

        // format fixed float width and set appropriate bytes in data
        else if (element.type == FLOAT) {
            float* valuePtr = (float*)malloc(sizeof(float));
            *valuePtr = atof((const char *)(dataBytes[dataPointer]));
            element.value = valuePtr;
        }

        // advance data pointer by element width + 1 ('|' or '>')
        dataPointer += (element.width + 1);

    }

}