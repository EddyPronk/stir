#include <editline/readline.h>
#include <curl/curl.h>
#include <string>
#include <iostream>

std::string start_session(const std::string& base_url)
{ 
	CURL* curl = curl_easy_init();
	if(curl)
	{
		std::string url = base_url + "/start_session";
		std::cout << "fetching " << url << std::endl;

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		CURLcode res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
	return "";
}

struct Buffer
{
	std::string data;
	Buffer()
	{
	}
	size_t callback(void* ptr, size_t size, size_t nmemb)
	{
		data = std::string(reinterpret_cast<char*>(ptr), size*nmemb);
	}
};

size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
	Buffer& buf(*reinterpret_cast<Buffer*>(data));
	buf.callback(ptr, size, nmemb);
 
	return 0;
}

void fetch(const std::string& base_url, const std::string& arg)
{ 
	CURL* curl = curl_easy_init();
	Buffer buf;
	if(curl)
	{
		std::string url = base_url + "/evaluate";
		std::cout << "fetching " << url << std::endl;

		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, arg.c_str());
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
 		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
 		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);

		CURLcode res = curl_easy_perform(curl);
		std::cerr << "[" << buf.data << "]" << std::endl;
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

	curl_global_init(CURL_GLOBAL_ALL);

	start_session(base_url);
	while (true)
	{
		char* raw_line = readline (">>> ");
		if (!raw_line)
			break;
		std::string line(raw_line);
		fetch(base_url, line);
		add_history(raw_line);
	}

	return 0;
}
