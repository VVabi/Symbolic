/**
 * @file Symbolic_playground.cpp
 * @brief For experimentation.
 */

#include <iostream>
#include <numeric>
#include <random>
#include <map>
#include <fstream>
#include "parsing/expression_parsing/math_expression_parser.hpp"
#include "types/power_series.hpp"
#include "types/rationals.hpp"
#include "types/bigint.hpp"
#include "examples/graph_isomorphisms.hpp"
#include "number_theory/moebius.hpp"
#include "types/polynomial.hpp"
#include "shell/shell.hpp"
#include "shell/parameters/parameters.hpp"
#include <curl/curl.h>

size_t writefunc(void *ptr, size_t size, size_t nmemb, std::string* s)
{
  std::cout << "writefunc called with size " << size << " and nmemb " << nmemb << std::endl;
  std::string part((char*)ptr, size*nmemb);
  *s += part;
  return size*nmemb;
}

int main(int argc, char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    CURLcode ret;
    CURL *hnd;
    std::string response_string;
    hnd = curl_easy_init();
    curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
    curl_easy_setopt(hnd, CURLOPT_URL, "https://oeis.org/search?q=1,2,3,6,11,23,47,106,235&fmt=json");
    curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/8.5.0");
    curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
    curl_easy_setopt(hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
    curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &response_string);
    /* Here is a list of options the curl code used that cannot get generated
    as source easily. You may choose to either not use them or implement
    them yourself.

    CURLOPT_WRITEDATA was set to an object pointer
    CURLOPT_INTERLEAVEDATA was set to an object pointer
    CURLOPT_WRITEFUNCTION was set to a function pointer
    CURLOPT_READDATA was set to an object pointer
    CURLOPT_READFUNCTION was set to a function pointer
    CURLOPT_SEEKDATA was set to an object pointer
    CURLOPT_SEEKFUNCTION was set to a function pointer
    CURLOPT_ERRORBUFFER was set to an object pointer
    CURLOPT_STDERR was set to an object pointer
    CURLOPT_HEADERFUNCTION was set to a function pointer
    CURLOPT_HEADERDATA was set to an object pointer

    */

    ret = curl_easy_perform(hnd);
    std::cout << response_string << std::endl;

    curl_easy_cleanup(hnd);
    hnd = NULL;
}
