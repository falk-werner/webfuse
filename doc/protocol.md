# Webufse 2 Protocol

## Endianness

All numeric data types are transferred in [Big Endian](https://en.wikipedia.org/wiki/Endianness).  
For instance, the uint32 value 1 will be transferred as

    00 00 00 01

## Data Types

### Basic data types

| Data | Width  | Description |
| ---- | ------ | ----------- |
| bool |  8 bit | Represents a boolean value |
| i32  | 32 bit |

## Message



## Methods

### access

| Field | Data Type        | Description |
| ----- | ---------------- | ----------- |
|       | uint32           | message id  |
| type  | uint8            | message type (0x00) |
| path  | string           | |
| mode  | access_mode (i8) |

#### Response

| Field |