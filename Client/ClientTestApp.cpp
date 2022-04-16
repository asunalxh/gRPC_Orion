#include <string>
#include "stdio.h"
#include "stdlib.h"

#include "../common/data_type.h"
#include "../common/Utils.h"
#include "Client.h"
#include "Orion.h"

// for measurement
#include <cstdint>
#include <chrono>
#include <iostream>

unsigned long timeSinceEpochMillisec()
{
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}
// end for measurement

#define ENCLAVE_FILE "CryptoEnclave.signed.so"

int total_file_no = (int)5; // 85000 // 150000
int del_no = (int)0;        // delete 50%

Client *myClient;
Orion* orion;

int main()
{
    printf("======== Create Client ========\n");
    myClient = new Client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));

    printf("======== Create Orion ========\n");
    orion = new Orion(myClient);
    

    // test ORAM

    /***
    printf("batch Insertion time\n");
    std::cout << timeSinceEpochMillisec() << std::endl;
    ecall_testInsertFinal(eid);
    std::cout << timeSinceEpochMillisec() << std::endl;

    printf("batch Search time\n");
    std::cout << timeSinceEpochMillisec() << std::endl;
    ecall_oramtest(eid);
    std::cout << timeSinceEpochMillisec() << std::endl;


    ***/
    // end Test ORam

    unsigned long long start, end;
    printf("======== Adding doc ========\n");

    // Update Protocol with op = add
    for (int i = 1; i <= total_file_no; i++)
    {

        docContent *fetch_data;
        fetch_data = (docContent *)malloc(sizeof(docContent));
        myClient->ReadNextDoc(fetch_data);

        // encrypt and send to Server
        entry *encrypted_entry;
        encrypted_entry = (entry *)malloc(sizeof(entry));

        encrypted_entry->first.content_length = fetch_data->id.id_length;
        encrypted_entry->first.content = (char *)malloc(fetch_data->id.id_length);
        encrypted_entry->second.message_length = fetch_data->content_length + AESGCM_MAC_SIZE + AESGCM_IV_SIZE;
        encrypted_entry->second.message = (char *)malloc(encrypted_entry->second.message_length);

        myClient->EncryptDoc(fetch_data, encrypted_entry);

        // myClient->Call_ReceiveEncDoc(encrypted_entry);

        // upload (op,in) to Enclave
        orion->addDoc(fetch_data->id.doc_id, fetch_data->id.id_length, fetch_data->id.doc_int,
                     fetch_data->content, fetch_data->content_length);

        // free memory

        free(fetch_data->content);
        free(fetch_data->id.doc_id);
        free(fetch_data);

        free(encrypted_entry->first.content);
        free(encrypted_entry->second.message);
        free(encrypted_entry);

        // do this one to flush doc by doc enclave to flush all documents in OMAP to server
        if (i % 10000 == 0)
        {
            printf("Processing insertion %d\n", i);
        }
    }

    // std::cout << timeSinceEpochMillisec() << std::endl;

    // Update Protocol with op = del (id)
    // printf("\nDeleting doc\n");

    // std::cout << timeSinceEpochMillisec() << std::endl;

    // for(int del_index=1; del_index <=del_no; del_index++){

    // 	docContent *fetch_data;
    // 	fetch_data = (docContent *)malloc(sizeof( docContent));
    // 	myClient->Del_GivenDocIndex(del_index,fetch_data);

    // 	if(del_index%1000==0){
    // 		printf("Processing deleting docs %d\n",del_index);
    // 	}
    // 	ecall_delDoc(eid,fetch_data->id.doc_id,fetch_data->id.id_length,fetch_data->id.doc_int,
    // 					fetch_data->content,fetch_data->content_length);

    // 	//later need to free fetch_data
    // 	free(fetch_data->content);
    // 	free(fetch_data->id.doc_id);
    // 	free(fetch_data);

    // // }

    // printf("Finish deleting all docs\n");
    // std::cout << timeSinceEpochMillisec() << std::endl;

    // Simulate setup start flushing
    printf("======== flush ========\n");
    orion->flush();

    ////search

    // std::string s_keyword[10]= {"the","of","and","to","a","in","for","is","on","that"};
    std::string s_keyword[2] = {"word1", "word2"};

    for (int s_i = 0; s_i < 2; s_i++)
    {
        printf("\nSearching ==> %s\n", s_keyword[s_i].c_str());

        orion->search(s_keyword[s_i].c_str(), s_keyword[s_i].size());
    }

    // free omap and client and server
    delete orion;
    delete myClient;

    return 0;
}