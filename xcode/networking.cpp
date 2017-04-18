//
//  networking.cpp
//  ami_proto_2
//
//  Created by Joe on 11/02/2017.
//
//

#include "networking.hpp"
#include <curl/curl.h>


size_t networking::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

json networking::test()
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.joemcalister.com/ami/testscript2.txt"); //https://api.joemcalister.com/ami/testscript2.txt
        // allow redirection
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        // write function allows parse to string
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        
        // make request
        res = curl_easy_perform(curl);
        
        // error check
        if(res != CURLE_OK)
        {
            cout << "ERROR: curl failed with: " << curl_easy_strerror(res) << endl;
        }
        
        // clean up
        curl_easy_cleanup(curl);
        
        // parse as json
        return json::parse(readBuffer);
        
    }
    
    return nil;
}

json networking::fetch()
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.joemcalister.com/ami/queue/check/getNextUser.php");
        // allow redirection
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        // write function allows parse to string
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        
        // make request
        res = curl_easy_perform(curl);
        
        // error check
        if(res != CURLE_OK)
        {
            cout << "ERROR: curl failed with: " << curl_easy_strerror(res) << endl;
        }
        
        // clean up
        curl_easy_cleanup(curl);
        
        // parse as json
        return json::parse(readBuffer);
        
    }
    
    return nil;
}
