#include "Orion.h"
#include "../common/Utils.h"
#include "stdlib.h"
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include <algorithm> // for std::find
#include <iterator>	 // for std::begin, std::end
#include <vector>
#include <list>
#include <fstream>

#include "../common/data_type.h"
#include "../common/data_type2.h"
#include "../common/Utils.h"

#include "Omap.h"

/*** setup */
Orion::Orion(Client *client, const unsigned char *KW, const unsigned char *KC, int numLeaf, bool initial)
{
	memcpy(this->KW, KW, ENC_KEY_SIZE);
	memcpy(this->KC, KC, ENC_KEY_SIZE);

	this->client = client;
	this->numLeaf = numLeaf;

	// 1: omap search
	// 2: omap update
	omap_search = new OMAP(KC, pow(2, numLeaf), 1, client, initial); // 1024
	omap_update = new OMAP(KW, pow(2, numLeaf), 2, client, initial); // 1024
}

Orion::~Orion()
{
	delete omap_search;
	delete omap_update;
}


void Orion::addDoc(const char *doc_id, size_t id_length, unsigned int docInt, const string &word)
{

	// parse content to keywords splited by comma

	entryKey k_w;
	k_w.content_length = enc_length(word.length());
	k_w.content = (char *)malloc(k_w.content_length);
	enc_aes_128(KW, (const unsigned char *)word.c_str(), word.length(), (unsigned char *)k_w.content);

	unsigned char *k_id = (unsigned char *)malloc(ENTRY_HASH_KEY_LEN_128);
	Hash_SHA256(k_w.content, k_w.content_length, doc_id, id_length, k_id, ENTRY_HASH_KEY_LEN_128);
	Bid key_kid = k_id;

	if (UpdtCnt.count(word) == 0)
	{
		UpdtCnt[word] = 0;
	}

	// insert into the state map for the keyword with (F(w||id),state) where F(w||id) = k_id
	UpdtCnt[word]++;
	omap_update->insert(key_kid, UpdtCnt[word]);
	// insert into the index map for(F(w||state),id) where F(w||state) = k_c
	unsigned char *k_c = (unsigned char *)malloc(ENTRY_HASH_KEY_LEN_128);
	std::string c_str = std::to_string(UpdtCnt[word]);
	char const *c_char = c_str.c_str();
	Hash_SHA256(k_w.content, k_w.content_length, c_char, c_str.length(), k_c, ENTRY_HASH_KEY_LEN_128);
	Bid key_kc = k_c;
	omap_search->insert(key_kc, docInt);
	// update in LastIND
	LastIND[word] = docInt;

	free(k_c);
	free(k_id);
	free(k_w.content);
}

void Orion::delDoc(const char *doc_id, size_t id_length, unsigned int docInt, const string &word)
{

	// parse content to keywords splited by comma

	entryKey k_w;
	k_w.content_length = enc_length(word.length());
	k_w.content = (char *)malloc(k_w.content_length);
	enc_aes_128(KW, (const unsigned char *)word.c_str(), word.length(), (unsigned char *)k_w.content);
	unsigned char *k_id = (unsigned char *)malloc(ENTRY_HASH_KEY_LEN_128);
	Hash_SHA256(k_w.content, k_w.content_length, doc_id, id_length, k_id, ENTRY_HASH_KEY_LEN_128);
	Bid key_kid = k_id;

	unsigned int updt_cnt = omap_update->find(key_kid);
	if (updt_cnt > 0)
	{

		omap_update->insert(key_kid, -1);
		UpdtCnt[word]--;
		if (UpdtCnt[word] > 0)
		{
			if (UpdtCnt[word] + 1 != updt_cnt) // it 's not the same, then recycle this update_cnt for the latest Ind
			{
				// create new kid from the F(w||lastest ind)
				unsigned char *cur_k_id = (unsigned char *)malloc(ENTRY_HASH_KEY_LEN_128);
				// retrieve the lasted ind
				std::string fileName = std::to_string(LastIND[word]);
				// convert fileId to char* and record length
				int doc_id_size = fileName.length() + 1;

				char *latest_doc_id = (char *)malloc(doc_id_size);
				memcpy(latest_doc_id, fileName.c_str(), doc_id_size);

				Hash_SHA256(k_w.content, k_w.content_length, latest_doc_id, doc_id_size, cur_k_id, ENTRY_HASH_KEY_LEN_128);

				// convert to bidKey
				Bid cur_key_kid = cur_k_id;

				// insert into omap update with this key
				omap_update->insert(cur_key_kid, updt_cnt);
				// insert into the index map for(F(w||deleted state),latest id) where F(w||deleted state) = k_c
				unsigned char *k_c = (unsigned char *)malloc(ENTRY_HASH_KEY_LEN_128);
				std::string c_str = std::to_string(updt_cnt);
				char const *c_char = c_str.c_str();
				Hash_SHA256(k_w.content, k_w.content_length, c_char, c_str.length(), k_c, ENTRY_HASH_KEY_LEN_128);
				Bid key_kc = k_c;
				omap_search->insert(key_kc, LastIND[word]);
				free(k_c);
				free(latest_doc_id);
				free(cur_k_id);
			}
			// then retrieve the id of the latested update (w, and the latest state) to assign to LastIND
			unsigned char *k_c_new = (unsigned char *)malloc(ENTRY_HASH_KEY_LEN_128);
			std::string c_str_new = std::to_string(UpdtCnt[word]);
			char const *c_char_new = c_str_new.c_str();
			Hash_SHA256(k_w.content, k_w.content_length, c_char_new, c_str_new.length(), k_c_new, ENTRY_HASH_KEY_LEN_128);
			Bid key_kc_new = k_c_new;
			unsigned int latestDocId = omap_search->find(key_kc_new);
			LastIND[word] = latestDocId;
			free(k_c_new);
		}
		else
		{
			LastIND.erase(word);
		}
	}

	free(k_id);
	free(k_w.content);
}

/*** search for a keyword */
vector<unsigned int> Orion::search(const char *keyword, size_t keyword_len)
{

	// init keys
	std::string keyword_str(keyword, keyword_len);

	entryKey k_w;

	k_w.content_length = enc_length(keyword_len);
	k_w.content = (char *)malloc(k_w.content_length + 1);
	enc_aes_128(KW, (unsigned char *)keyword, keyword_len, (unsigned char *)k_w.content);

	std::vector<Bid> search_key_series;

	if (UpdtCnt[keyword_str] != 0)
	{
		for (int i = 1; i <= UpdtCnt[keyword_str]; i++)
		{

			// search into the index map for(F(w||state),id) where F(w||state) = k_c
			unsigned char *k_c = (unsigned char *)malloc(ENTRY_HASH_KEY_LEN_128);
			std::string c_str = std::to_string(i);
			char const *c_char = c_str.c_str();
			Hash_SHA256(k_w.content, k_w.content_length, c_char, c_str.length(), k_c, ENTRY_HASH_KEY_LEN_128);

			Bid key_kc = k_c;
			search_key_series.push_back(key_kc);

			free(k_c);
		}
	}

	vector<unsigned int> result = omap_search->batchSearch(search_key_series);

	// free memory
	free(k_w.content);

	return result;
}
