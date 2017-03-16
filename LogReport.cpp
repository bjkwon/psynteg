#include "stdafx.h"
#include <string>
#include <process.h>

#include <curl/curl.h>
#include "LogReport.h"

using namespace std;

#define POST_URL	"http://www.auditorypro.com/explog/post.php"
#define POST_KEY	"explog123"


LogReport::LogReport(char *namePrefix)
: NamePrefix(namePrefix), fUseEmail(false), fUseFtp(false)
{
}


size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
	size_t realsize = size * nmemb;
	string *mem = (string *)data;
	*mem += string((char *)ptr, realsize);
	return realsize;
}


void uploadLogFile(void *pLogReport)
{
	LogReport *p = (LogReport *)pLogReport;
	CURL *curl;
	CURLcode res;

	CString rfname;
	char buf[200] = "";
	DWORD cbComputerName = sizeof(buf);
	GetComputerName(buf, &cbComputerName);
	rfname = p->NamePrefix + "_" + buf;

	time_t time0 = time(NULL);
	strftime(buf, sizeof(buf), "_%Y_%m_%d_%H%M.log", localtime(&time0));
	rfname += buf;

	curl_global_init(CURL_GLOBAL_ALL);
	if (p->fUseFtp) {
		if (p->ftp_address.Left(6).MakeLower() != "ftp://")
			p->ftp_address.Insert(0, "ftp://");
		if (p->ftp_address.Right(1) != "/")
			p->ftp_address += "/";
		if (FILE *file = fopen(p->filename, "rb")) {
			CString FtpURL = p->ftp_address + rfname;
			curl = curl_easy_init();
			curl_easy_setopt(curl, CURLOPT_UPLOAD, 1) ;
			curl_easy_setopt(curl, CURLOPT_USERPWD, p->ftp_id + ":" + p->ftp_password) ;
				curl_easy_setopt(curl, CURLOPT_URL, FtpURL);
			curl_easy_setopt(curl, CURLOPT_READDATA, file);
			curl_easy_setopt(curl, CURLOPT_READFUNCTION, fread);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
			fclose(file);
		} else
			MessageBox(NULL, "File open failed - "+p->filename, "Log reporting", MB_OK);
	}

	if (p->fUseEmail) {
		string outstr;
		curl = curl_easy_init();
		struct curl_httppost* post = NULL;
		struct curl_httppost* last = NULL;
		curl_formadd(&post, &last, CURLFORM_PTRNAME, "key",
							CURLFORM_PTRCONTENTS, POST_KEY, CURLFORM_END); 
		curl_formadd(&post, &last, CURLFORM_PTRNAME, "to",
							CURLFORM_PTRCONTENTS, p->email_address, CURLFORM_END); 
		curl_formadd(&post, &last, CURLFORM_PTRNAME, "subject",
							CURLFORM_PTRCONTENTS, rfname, CURLFORM_END); 
		curl_formadd(&post, &last, CURLFORM_COPYNAME, "body",
							CURLFORM_FILECONTENT, p->filename, CURLFORM_END);
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
		curl_easy_setopt(curl, CURLOPT_URL, POST_URL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&outstr);
		curl_easy_perform(curl);
		curl_formfree(post);
		curl_easy_cleanup(curl);
		if (!outstr.empty())
			MessageBox(NULL, outstr.c_str(), "POST result", MB_OK);
	}
	curl_global_cleanup();
	_endthread();
}
