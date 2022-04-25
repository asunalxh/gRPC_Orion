#include "Client.h"
#include "Omap.h"
#include "Bid.h"
#include "Client.h"

class Orion
{
public:
	Orion(Client *client, const unsigned char *KW, const unsigned char *KC, bool initial = true);
	~Orion();

	void addDoc(const char *doc_id, size_t id_length, unsigned int docInt, std::vector<std::string> wordList);
	void flush();
	void delDoc(const char *doc_id, size_t id_length, unsigned int docInt, std::vector<std::string> wordList);
	vector<unsigned int> search(const char *keyword, size_t keyword_len);

	void writeToFile(std::string dir = ".");

private:
	// change to malloc for tokens , run ulimit -s 65536 to set stack size to
	// 65536 KB in linux

	// local variables inside Enclave
	unsigned char KW[ENC_KEY_SIZE] = {0};
	unsigned char KC[ENC_KEY_SIZE] = {0};

	int numLeaf = 22; // actual number of (w,id) supported ~ numleaf in worst case - or change to smaller to only 20 // then bucketCount = 8.3 mil

	OMAP *omap_search;
	// OMAP *omap_update;

	std::map<Bid, unsigned int> setupPairs1; // for omap update
	std::map<Bid, unsigned int> setupPairs2; // for omap search

	std::unordered_map<std::string, int> UpdtCnt;		   // this is the ST[w]-> state
	std::unordered_map<std::string, unsigned int> LastIND; // this is the ;astIND[w]-> most recently added id

	Client *client;
};