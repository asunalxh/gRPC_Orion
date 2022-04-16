#ifndef ENCLAVE_UTILS_H
#define ENCLAVE_UTILS_H

#include "stdlib.h"
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include <iterator>
#include <vector>
#include <array>
#include <cstring>
#include "../common/data_type.h"
#include "../common/data_type2.h"

void print_bytes(uint8_t *ptr, uint32_t len);
int cmp(const uint8_t *value1, const uint8_t *value2, uint32_t len);
void clear(uint8_t *dest, uint32_t len);
std::vector<std::string> wordTokenize(char *content, int content_length);

int enc_aes_gcm(const unsigned char *plaintext, int plaintext_len,
                const unsigned char *key,
                unsigned char *ciphertext);
int dec_aes_gcm(unsigned char *ciphertext, int ciphertext_len,
                const unsigned char *key,
                unsigned char *plaintext);
void Hash_SHA256(const void *key, int key_len, const void *msg, int msg_len, void *value);

std::string BucketToString(BUCKET bucket);

BUCKET StringToBucket(std::string str);

void read_rand(unsigned char* result, size_t len);
#endif
