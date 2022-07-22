#pragma once
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/StreamCopier.h>
#include <Poco/URIStreamOpener.h>
#include <Poco/StreamCopier.h>
#include <Poco/NullStream.h>
#include <Poco/Path.h>
#include <Poco/URI.h>
#include <Poco/Exception.h>
#include <Poco/Net/HTTPStreamFactory.h>
#include <Poco/Net/HTTPSStreamFactory.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/Net/InvalidCertificateHandler.h>
#include <Poco/Net/RejectCertificateHandler.h>

class Requests
{
public:
	Requests();
	~Requests();

	static Requests& Get();

	std::string CreateGetRequest(const std::string& url, std::vector<std::string> headers = {});
	std::unique_ptr<std::istream> DownloadFile(const std::string& DownloadURL);

private:
	struct RequestData
	{
		Poco::Net::HTTPRequest Request;
		std::unique_ptr<Poco::Net::HTTPClientSession> Session;
	};

	std::string GetResponse(Poco::Net::HTTPClientSession* session, Poco::Net::HTTPResponse& response);

	RequestData ConstructRequest(const std::string& url, const std::vector<std::string>& headers, const std::string& request_type);
};