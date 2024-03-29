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
// std::vector<std::string> wordTokenize(char *content, int content_length);

int enc_length(int message_len);

int enc_aes_128(const unsigned char *key,
				const unsigned char *plaintext, int plaintext_len,
				unsigned char *ciphertext);

int dec_aes_128(
	const unsigned char *key,
	unsigned char *ciphertext, int ciphertext_len,
	unsigned char *plaintext);
int enc_aes_gcm(const unsigned char *key,
				const unsigned char *plaintext, int plaintext_len,
				unsigned char *ciphertext);

int dec_aes_gcm(
	const unsigned char *key,
	unsigned char *ciphertext, int ciphertext_len,
	unsigned char *plaintext);

void Hash_SHA256(const void *key, int key_len, const void *msg, int msg_len, void *value,int value_len);

std::string BucketToString(BUCKET bucket);

BUCKET StringToBucket(std::string str);

void read_rand(unsigned char *result, size_t len);

char *enc_base64(const unsigned char *inputBuffer, int inputLen, int *outLen);
unsigned char *dec_base64(const char *input, int length, int *outLen);
#endif
