// hasil evolusi dari si c
// bahasa sejuta umat untuk pemula (katanya)
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct ResponseData {
    std::string data;
    size_t size;
};

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    ResponseData* responseData = static_cast<ResponseData*>(userp);
    responseData->data.append(static_cast<char*>(contents), totalSize);
    responseData->size += totalSize;
    return totalSize;
}

std::string sendRequest(const std::string& text) {
    CURL* curl;
    CURLcode res;
    ResponseData response = { "", 0 };

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        std::string postFields = "text=" + text + "&lc=id";

        curl_easy_setopt(curl, CURLOPT_URL, "https://api.simsimi.vn/v1/simtalk");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    return response.data;
}

std::string parseResponse(const std::string& responseData) {
    auto responseJson = json::parse(responseData);
    if (responseJson.contains("message")) {
        return responseJson["message"].get<std::string>();
    } else {
        return "Tidak ada pesan yang diterima";
    }
}

int main() {
    std::string inputText;

    while (true) {
        std::cout << "you>: ";
        std::getline(std::cin, inputText);

        if (inputText == "exit" || inputText == "quit" || inputText == "keluar" || inputText == "murtad") {
            std::cout << "logout bang!!" << std::endl;
            break;
        }

        std::string response = sendRequest(inputText);
        std::string message = parseResponse(response);
        std::cout << "bot>: " << message << std::endl;
    }

    return 0;
}
