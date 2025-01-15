#pragma once

/*
This file defines a simple custom schema for serializing compound data while
maintaining readability to facilitate debugging. Compatible with C/C++.

A Schema object defines something with a number of int and float fields with
byte widths that can be serialzied/deserialized to/from the following format:

<field1|field2|field3|field4>

with leading and trailing '<' and '>' characters, and '|' characters separate fields.
*/

enum FieldType {
    INT,
    FLOAT
};

struct SchemaElement {
    FieldType type;
    int width;
    int decimalPlaces;
    void* value;
};

struct Schema {
    SchemaElement* elements;
    int numElements;
};

int schemaByteLength(const Schema* schema);
char* schemaToBytes(const Schema* schema);
void bytesToSchema(Schema* schema, char* dataBytes);