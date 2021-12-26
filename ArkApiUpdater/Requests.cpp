#include "Requests.h"
#include "Logger.h"
#include <fstream>
#include <memory>

Requests::Requests()
{
	Poco::Net::initializeSSL();
	Poco::SharedPtr<Poco::Net::InvalidCertificateHandler> ptrCert = new Poco::Net::RejectCertificateHandler(false);
	Poco::Net::Context::Ptr ptrContext = new Poco::Net::Context(Poco::Net::Context::TLS_CLIENT_USE, "", "", "", Poco::Net::Context::VERIFY_NONE, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
	Poco::Net::SSLManager::instance().initializeClient(0, ptrCert, ptrContext);

	Poco::Net::HTTPStreamFactory::registerFactory();
	Poco::Net::HTTPSStreamFactory::registerFactory();
}

Requests::~Requests()
{
	Poco::Net::uninitializeSSL();
}

Requests& Requests::Get()
{
	static Requests instance;
	return instance;
}

std::string Requests::CreateGetRequest(const std::string& url, std::vector<std::string> headers)
{
	std::string Result = "";
	Poco::Net::HTTPResponse response(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
	Poco::Net::HTTPClientSession* session = nullptr;

	try
	{
		Poco::Net::HTTPRequest request = this->ConstructRequest(url, session, headers, Poco::Net::HTTPRequest::HTTP_GET);

		request.set("user-agent", "ArkApi AutoUpdate");

		session->sendRequest(request);
		Result = this->GetResponse(session, response);
	}
	catch (const Poco::Exception& exc)
	{
		Log::GetLog()->error(exc.displayText());
	}

	delete session;
	session = nullptr;

	return Result;
}

bool Requests::DownloadFile(const std::string& DownloadURL, const std::string& DownloadPath)
{
	try
	{
		std::ofstream FileStream;
		FileStream.open(DownloadPath, std::ios::out | std::ios::trunc | std::ios::binary);

		Poco::URI uri(DownloadURL);

		auto DownloadStream = Poco::URIStreamOpener::defaultOpener().open(uri);
		Poco::StreamCopier::copyStream(*DownloadStream, FileStream);
		FileStream.close();

		delete DownloadStream;
		DownloadStream = nullptr;
	}
	catch (const Poco::Exception& exc)
	{
		Log::GetLog()->error(exc.displayText());
		return false;
	}

	return true;
}

std::string Requests::GetResponse(Poco::Net::HTTPClientSession* session, Poco::Net::HTTPResponse& response)
{
	std::string result = "";

	std::istream& rs = session->receiveResponse(response);

	if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
	{
		std::ostringstream oss;
		Poco::StreamCopier::copyStream(rs, oss);
		result = oss.str();
	}
	else
	{
		Poco::NullOutputStream null;
		Poco::StreamCopier::copyStream(rs, null);
		result = std::to_string(response.getStatus()) + " " + response.getReason();
	}

	return result;
}

Poco::Net::HTTPRequest Requests::ConstructRequest(const std::string& url, Poco::Net::HTTPClientSession*& session,
	const std::vector<std::string>& headers, const std::string& request_type)
{
	Poco::URI uri(url);

	const std::string& path(uri.getPathAndQuery());

	if (uri.getScheme() == "https")
		session = new Poco::Net::HTTPSClientSession(uri.getHost(), uri.getPort());
	else
		session = new Poco::Net::HTTPClientSession(uri.getHost(), uri.getPort());

	Poco::Net::HTTPRequest request(request_type, path, Poco::Net::HTTPMessage::HTTP_1_1);

	if (!headers.empty())
	{
		for (const auto& header : headers)
		{
			const std::string& key = header.substr(0, header.find(":"));
			const std::string& data = header.substr(header.find(":") + 1);

			request.add(key, data);
		}
	}

	return request;
}