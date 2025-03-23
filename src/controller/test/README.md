# Automated tests

The folders and files for this folder are as follows:

<-- Describe Dem Goofy Ahh Tests -->

## Unit Tests

test_crc - CR8/SMBUS tests based on the spec based on the spec based on [this](!https://crccalc.com/?crc=123456789&method=CRC-8/SMBUS&datatype=ascii&outtype=hex)

test_rc - Tests for checking rolling count (RC) functions, and how crc changes with rolling counts

test_pid - Tests for PID parameters and system

test_pack_unpack - Tests for serialization/deserialization of data

## System Tests

### Note
I tested my algorithms in the folder safety\test using the main.c file found there, and ported them to gtest so th