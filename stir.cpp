#include <stdio.h>
#include <curl/curl.h>
#include <string>
#include <iostream>

void fetch(const std::string& base_url, const std::string& arg)
{ 
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();
	if(curl)
	{
		std::string url = base_url + "?" + arg;
		std::cout << "fetching " << url << std::endl;
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
}

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		std::cerr << "Usage: progname [URL]" << std::endl;
		return 0;
	}

	std::string base_url = argv[1];
	std::string line;
	do
	{
		std::cerr << ">>> ";
		std::cin >> line;
		std::cout << line << std::endl;
		fetch(base_url, line);
		std::cerr << std::endl;
	}
	while(line != "");

	return 0;
}
