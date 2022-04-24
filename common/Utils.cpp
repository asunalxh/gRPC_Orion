#include "Utils.h"
#include "../common/data_type.h"

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/hmac.h>

void print_bytes(uint8_t *ptr, uint32_t len)
{
	for (uint32_t i = 0; i < len; i++)
	{
		printf("%x", *(ptr + i));
	}

	printf("\n");
}

int cmp(const uint8_t *value1, const uint8_t *value2, uint32_t len)
{
	for (uint32_t i = 0; i < len; i++)
	{
		if (*(value1 + i) != *(value2 + i))
		{
			return -1;
		}
	}

	return 0;
}

void clear(uint8_t *dest, uint32_t len)
{
	for (uint32_t i = 0; i < len; i++)
	{
		*(dest + i) = 0;
	}
}

//std::vector<std::string> wordTokenize(char *content, int content_length)
//{
//	char delim[] = ","; //" ,.-";
//	std::vector<std::string> result;

//	char *content_cpy = (char *)malloc(content_length);
//	memcpy(content_cpy, content, content_length);

//	char *token = strtok(content_cpy, delim);
//	while (token != NULL)
//	{
//		result.push_back(token);
//		token = strtok(NULL, delim);
//	}

//	free(token);
//	free(content_cpy);
//	// the last , will be counted
//	// result.erase(result.end()-1);

//	return result;
//}

int enc_aes_gcm(const unsigned char *plaintext, int plaintext_len,
				const unsigned char *key,
				unsigned char *ciphertext)
{

	unsigned char output[AESGCM_MAC_SIZE + AESGCM_IV_SIZE + plaintext_len * 2] = {0};
	memcpy(output + AESGCM_MAC_SIZE, gcm_iv, AESGCM_IV_SIZE);

	int ciphertext_len = 0, final_len = 0;

	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	EVP_EncryptInit(ctx, EVP_aes_128_gcm(), key, gcm_iv);

	EVP_EncryptUpdate(ctx, output + AESGCM_MAC_SIZE + AESGCM_IV_SIZE, &ciphertext_len, plaintext, plaintext_len);
	EVP_EncryptFinal(ctx, output + AESGCM_MAC_SIZE + AESGCM_IV_SIZE + ciphertext_len, &final_len);
	EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, AESGCM_MAC_SIZE, output);
	EVP_CIPHER_CTX_free(ctx);

	ciphertext_len = AESGCM_MAC_SIZE + AESGCM_IV_SIZE + ciphertext_len + final_len;
	memcpy(ciphertext, output, ciphertext_len);

	return ciphertext_len;
}

int dec_aes_gcm(unsigned char *ciphertext, int ciphertext_len,
				const unsigned char *key,
				unsigned char *plaintext)
{
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	int plaintext_len = 0, final_len = 0;

	EVP_DecryptInit(ctx, EVP_aes_128_gcm(), key, gcm_iv);
	EVP_DecryptUpdate(ctx, plaintext, &plaintext_len,
					  ciphertext + AESGCM_MAC_SIZE + AESGCM_IV_SIZE,
					  ciphertext_len - AESGCM_MAC_SIZE - AESGCM_IV_SIZE);

	EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, AESGCM_MAC_SIZE, ciphertext);
	EVP_DecryptFinal(ctx, plaintext + plaintext_len, &final_len);
	EVP_CIPHER_CTX_free(ctx);
	plaintext_len = plaintext_len + final_len;

	return plaintext_len;
}

void Hash_SHA256(const void *key, int key_len, const void *msg, int msg_len, void *value)
{
	unsigned int len;
	HMAC_CTX *ctx = HMAC_CTX_new();
	HMAC_Init_ex(ctx, key, key_len, EVP_sha256(), NULL);
	HMAC_Update(ctx, (unsigned char *)msg, msg_len);
	HMAC_Final(ctx, (unsigned char *)value, &len);
}

std::string BucketToString(BUCKET bucket)
{
	std::string ans;
	for (auto c : bucket)
	{
		ans += c;
	}
	return ans;
}

BUCKET StringToBucket(std::string str)
{
	BUCKET ans;
	for (auto c : str)
	{
		ans.push_back(c);
	}
	return ans;
}

void read_rand(unsigned char *result, size_t len)
{
	RAND_bytes(result, len);
}

char *enc_base64(const unsigned char *inputBuffer, int inputLen, int *outLen)
{
	EVP_ENCODE_CTX *ctx = EVP_ENCODE_CTX_new();
	int base64Len = (((inputLen + 2) / 3) * 4) + 1; // Base64 text length
	int pemLen = base64Len + base64Len / 64;		// PEM adds a newline every 64 bytes
	char *base64 = new char[pemLen];
	int result, tmpLen;
	EVP_EncodeInit(ctx);
	EVP_EncodeUpdate(ctx, (unsigned char *)base64, &result, (unsigned char *)inputBuffer, inputLen);
	EVP_EncodeFinal(ctx, (unsigned char *)&base64[result], &tmpLen);
	result += tmpLen;
	*outLen = result;
	return base64;
}

unsigned char *dec_base64(const char *input, int length, int *outLen)
{
	EVP_ENCODE_CTX *ctx = EVP_ENCODE_CTX_new();
	unsigned char *orgBuf = new unsigned char[length];
	int result, tmpLen;
	EVP_DecodeInit(ctx);
	EVP_DecodeUpdate(ctx, (unsigned char *)orgBuf, &result, (unsigned char *)input, length);
	EVP_DecodeFinal(ctx, (unsigned char *)&orgBuf[result], &tmpLen);
	result += tmpLen;
	*outLen = result;
	return orgBuf;
}