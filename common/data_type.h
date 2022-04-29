#ifndef DATA_TYPE_H
#define DATA_TYPE_H

#include "config.h"
#include <stdint.h>
#include <vector>
#include <algorithm>
#include <array>
#include <list>
#include <string>
#include <tuple>
#include <utility>
#include <unordered_map>

/* for all sources except OCALL/ECALL */

const std::string raw_doc_dir= "/home/asunalxh/Data/enron_datasets/";
const std::string keyword_dir = "/home/asunalxh/Data/enron_keywords/";

#define AESGCM_IV_SIZE 12
static unsigned char gcm_iv[] = {
    0x99, 0xaa, 0x3e, 0x68, 0xed, 0x81, 0x73, 0xa0, 0xee, 0xd0, 0x66, 0x84
};

#define AESGCM_MAC_SIZE 16

#define ENC_KEY_SIZE 16 // for AES128

#define ENTRY_HASH_KEY_LEN_256 32 // for HMAC-SHA128- bit key


/* packet related */
typedef struct docIds {
    char *doc_id; 
    unsigned int doc_int;
    size_t id_length;  // length of the doc_id
} docId; 


typedef struct entryKeys {
    char *content; 
    size_t content_length;  // length of the entry_value
} entryKey;

typedef struct entryValues {
    char *message; 
    size_t message_length;  // length of the entry_value
} entryValue;

typedef struct docContents{
    docId id;
    char* content;
    int content_length;
    //std::vector<std::string> wordList;
} docContent;

typedef std::pair<entryKey, entryValue> entry;

#endif
