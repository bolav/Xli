#import <CoreFoundation/CoreFoundation.h>
#import <Foundation/Foundation.h>
#include <XliPlatform/PlatformSpecific/iOS.h>
#include <XliHttpClient/HttpClient.h>
#include <XliHttpClient/Uri.h>
#include <Xli/HashMap.h>
#include <Xli/Managed.h>
#include <CFNetwork/CFNetwork.h>
#include <CFNetwork/CFHTTPStream.h>
#include <iostream>
#include <fstream>
#include <Xli/Console.h>

namespace Xli
{
    class CHttpRequest : public HttpRequest
    {
    private:
        HttpClient* client;

        HttpRequestState state;

        String url;
        String method;
        int timeout;
        bool verifyHost;

        CFReadStreamRef cachedReadStream;

        CFDataRef uploadData;
        bool requestOwnsUploadData;
        HashMap<String,String> headers;

        HashMap<String,String> responseHeaders;
        bool dataReady;
        bool reading;
        int readPosition;
        int responseStatus;
        CFWriteStreamRef cachedContentStream;
        Buffer* responseBody;
        bool errored;

    public:
        CHttpRequest(HttpClient* client, String url, String method)
        {
            this->errored = false;
            this->client = client;
            this->state = HttpRequestStateOpened;
            this->url = Uri::AutoEncodeUri(url);
            this->method = method;
            this->timeout = 0;
            this->verifyHost = true;
            this->readPosition = 0;            
            this->reading = false;
            this->cachedContentStream = 0;
            this->cachedReadStream = 0;
            OnStateChanged(this, HttpRequestStateOpened, 0, 0);
        }

        virtual ~CHttpRequest()
        {
            // abort if running

            state = HttpRequestStateDone;

            // spin this off into function
            if (cachedReadStream!=0)
            {
                CFReadStreamClose(cachedReadStream);
                CFRelease(cachedReadStream);
            }
            if (cachedReadStream!=0)
                CFRelease(cachedReadStream);
            if (cachedContentStream!=0)
                CFRelease(cachedContentStream);
            if (uploadData!=0)
                CFRelease(uploadData);
        }

        virtual String GetMethod() const { return method; }
        virtual String GetUrl() const { return url; }        
        virtual HttpRequestState GetState() const { return state; }
        virtual int GetTimeout() const { return timeout; }
        virtual void SetTimeout(int timeout)
        {
            if (state <= HttpRequestStateOpened)
            {
                this->timeout = timeout;
            } else {
                XLI_THROW("HttpRequest->SetTimeout(): Not in a valid state to set the timeout");
            }
        }
        virtual bool GetVerifyHost() const { return verifyHost; }
        virtual void SetVerifyHost(bool verify)
        {
            if (state <= HttpRequestStateOpened)
            {
                verifyHost = verify;
            } else {
                XLI_THROW("HttpRequest->SetTimeout(): Not in a valid state to set the verify host flag");
            }
        }

        virtual void SetHeader(const String& key, const String& value)
        {
            if (state == HttpRequestStateOpened)
            {
                headers.Add(key,value);
            } else {
                XLI_THROW("HttpRequest->SetHeader(): Not in a valid state to set a header");
            }
        }
        virtual void RemoveHeader(const String& key)
        {
            if (state == HttpRequestStateOpened)
            {
                headers.Remove(key);
            } else {
                XLI_THROW("HttpRequest->SetHeader(): Not in a valid state to set a header");
            }
        }
        virtual int HeadersBegin() const { return headers.Begin(); }
        virtual int HeadersEnd() const { return headers.End(); }
        virtual int HeadersNext(int n) const { return headers.Next(n); }
        virtual String GetHeaderKey(int n) const { return headers.GetKey(n); }
        virtual String GetHeaderValue(int n) const { return headers.GetValue(n); }

        virtual void SendAsync(const void* content, int byteLength)
        {
            if (this->state != HttpRequestStateOpened) return;

            CFStringRef nUrlStr = CFStringCreateWithCString(kCFAllocatorDefault, this->url.Ptr(), kCFStringEncodingUTF8);
            CFURLRef nUrl = CFURLCreateWithString(kCFAllocatorDefault, nUrlStr, NULL);
            CFStringRef nMethod = CFStringCreateWithCString(kCFAllocatorDefault, this->method.Ptr(), kCFStringEncodingUTF8);
            CFHTTPMessageRef nHttpReq = CFHTTPMessageCreateRequest(kCFAllocatorDefault, nMethod, nUrl, kCFHTTPVersion1_1);

            if (byteLength>0)
            {
                CFAllocatorRef alRef;
                if (requestOwnsUploadData)
                {
                    alRef = kCFAllocatorDefault;
                } else {
                    alRef = kCFAllocatorNull; // This means CFReleasing uploadData WILL NOT free the data, only the CFDataRef object
                }
                uploadData = CFDataCreateWithBytesNoCopy(kCFAllocatorDefault, (const UInt8*)content, (CFIndex)byteLength, alRef);
                CFHTTPMessageSetBody(nHttpReq, uploadData);
            } else {
                uploadData = 0;
            }

            SetCFHeaders(nHttpReq);

            cachedReadStream = CFReadStreamCreateForHTTPRequest(kCFAllocatorDefault, nHttpReq);
            CFNumberRef nTimeout = CFNumberCreate(kCFAllocatorDefault, kCFNumberDoubleType, &this->timeout);
            CFReadStreamSetProperty(cachedReadStream, CFSTR("_kCFStreamPropertyReadTimeout"), nTimeout); //{TODO} doco is flakey on this, does it work?
            CFReadStreamSetProperty(cachedReadStream, kCFStreamPropertyHTTPShouldAutoredirect, kCFBooleanTrue);
            if (!verifyHost)
            {
                CFMutableDictionaryRef myDict = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, 
                                                                          &kCFTypeDictionaryValueCallBacks);
                CFDictionarySetValue(myDict, kCFStreamSSLValidatesCertificateChain, kCFBooleanFalse);
                CFReadStreamSetProperty(cachedReadStream, kCFStreamPropertySSLSettings, myDict);
            }

            CFOptionFlags optEvents = kCFStreamEventOpenCompleted|kCFStreamEventHasBytesAvailable|kCFStreamEventErrorOccurred|kCFStreamEventEndEncountered;

            CFStreamClientContext context = {0, this, NULL, NULL, NULL };

            CFReadStreamSetClient(cachedReadStream, optEvents, AsyncCallback, &context);
            CFReadStreamScheduleWithRunLoop( cachedReadStream, CFRunLoopGetCurrent(), kCFRunLoopCommonModes );
            CFReadStreamOpen( cachedReadStream );

            this->state = HttpRequestStateSent;
            HttpEventHandler* eh = client->GetEventHandler();
            if (eh!=0) eh->OnRequestStateChanged(this);

            CFRelease(nUrlStr);
            CFRelease(nMethod);
            CFRelease(nUrl);
        }

        virtual void SendAsync(const String& content)
        {
            requestOwnsUploadData = true;
            void* raw = malloc(content.Length());
            memcpy(raw, content.Ptr(), content.Length());
            SendAsync(raw, content.Length());
        }

        virtual void SendAsync()
        {
            SendAsync((void*)0, -1);
        }

        virtual int GetResponseHeaderCount() const
        {
            if (state >= HttpRequestStateHeadersReceived)
            {
                return responseHeaders.Count();
            } else {
                XLI_THROW("HttpRequest->GetResponseHeaderCount(): -Not in a valid state to get the response header count");
            }
        }
        virtual int ResponseHeadersBegin() const
        {
            if (state >= HttpRequestStateHeadersReceived)
            {
                return responseHeaders.Begin();
            } else {
                XLI_THROW("HttpRequest->ResponseHeaderBegin(): Not in a valid state to get the response header iterator");
            }
        }
        virtual int ResponseHeadersNext(int n) const
        {
            if (state >= HttpRequestStateHeadersReceived)
            {
                return responseHeaders.Next(n);
            } else {
                XLI_THROW("HttpRequest->ResponseHeaderNext(): Not in a valid state to get the next response header");
            }
        }
        virtual int ResponseHeadersEnd() const
        {
            if (state >= HttpRequestStateHeadersReceived)
            {
                return responseHeaders.End();
            } else {
                XLI_THROW("HttpRequest->ResponseHeaderEnd(): Not in a valid state to get the response header");
            }
        }
        virtual String GetResponseHeaderKey(int n) const
        {
            if (state >= HttpRequestStateHeadersReceived)
            {
                return responseHeaders.GetKey(n);
            } else {
                XLI_THROW("HttpRequest->GetResponseHeaderN(): Not in a valid state to get the response header");
            }
        }
        virtual String GetResponseHeaderValue(int n) const
        {
            if (state >= HttpRequestStateHeadersReceived)
            {
                return responseHeaders.GetValue(n);
            } else {
                XLI_THROW("HttpRequest->GetResponseHeaderN(): Not in a valid state to get the response header");
            }
        }

        virtual int GetResponseStatus() const
        {
            if (state >= HttpRequestStateHeadersReceived) // {TODO} is this the correct state?
            {
                return responseStatus;
            } else {
                XLI_THROW("HttpRequest->GetResponseStatus(): Not in a valid state to get the response status");
            }
        }

        virtual bool TryGetResponseHeader(const String& key, String& result) const
        {
            if (state >= HttpRequestStateHeadersReceived)
            {
                return responseHeaders.TryGetValue(key, result);
            } else {
                XLI_THROW("HttpRequest->GetResponseHeader(): Not in a valid state to get the response header");
            }
        }


        virtual DataAccessor* GetResponseBody() const
        {
            if (state == HttpRequestStateDone)
            {
                return (DataAccessor*)responseBody;
            } else {
                XLI_THROW("HttpRequest->GetResponseBody(): Not in a valid state to get the response body");
            }
        }

        virtual void SetCFHeaders(CFHTTPMessageRef message)
        {
            if (this->state <= HttpRequestStateOpened)
            {
                int i = this->headers.Begin();
                while (i != this->headers.End())
                {
                    CFStringRef nKey = CFStringCreateWithCString(kCFAllocatorDefault, this->headers.GetKey(i).Ptr(), kCFStringEncodingUTF8);
                    CFStringRef nVal = CFStringCreateWithCString(kCFAllocatorDefault, this->headers.GetValue(i).Ptr(), kCFStringEncodingUTF8);

                    CFHTTPMessageSetHeaderFieldValue(message, nKey, nVal);

                    CFRelease(nKey);
                    CFRelease(nVal);
                    i = this->headers.Next(i);
                }
            }
        }

        virtual void Abort()
        {

            // {TODO} Can we guarentee instant termination?
            //        If so we dont need to worry about post destroy callbacks

            // if (this->state > 1 && this->state < 5 )
            //this->state = HttpRequestStateDone; //{TODO} how does statechanged know if it was successful?

            //{TODO} This doesnt seem to cancel running requests. What if we are in the upload stage?

            if (cachedReadStream!=0)
            {
                CFReadStreamClose(this->cachedReadStream);
                CFRelease(this->cachedReadStream);
            }
            if (cachedReadStream!=0)
                CFRelease(this->cachedReadStream);
            if (cachedContentStream!=0)
                CFRelease(this->cachedContentStream);
            if (uploadData!=0)
                CFRelease(this->uploadData);
            
            HttpEventHandler* eh = client->GetEventHandler();
            if (eh!=0) eh->OnRequestAborted(this);
        }

        //{TODO} we will always download imediately so clean this up and possibly fold into other function
        virtual void StartDownload()
        {
            if ((this->state==HttpRequestStateHeadersReceived) && (this->cachedContentStream == 0))
            {
                this->state = HttpRequestStateLoading;
                this->reading = true;

                HttpEventHandler* eh = client->GetEventHandler();
                if (eh!=0) eh->OnRequestStateChanged(this);

                this->cachedContentStream = CFWriteStreamCreateWithAllocatedBuffers(kCFAllocatorDefault, kCFAllocatorDefault);
                CFWriteStreamOpen(this->cachedContentStream);
                if (dataReady) OnByteDataRecieved(this, cachedReadStream, NULL);
            } else {
                XLI_THROW("Not in correct state to start download");
            }
        }

        virtual void NHeadersToHeaders(CFDictionaryRef dictRef)
        {
            CFDictionaryApplyFunction(dictRef, NHeaderToHeader, this);
        }
        static void NHeaderToHeader(const void* key, const void* value, void* ptr)
        {
            CHttpRequest* request = (CHttpRequest*)ptr;
            if (!request->responseHeaders.ContainsKey((char*)key)) 
                request->responseHeaders.Add((char*)key, (char*)value);
        }

        static void OnStateChanged(CHttpRequest* request, HttpRequestState status, CFReadStreamRef stream, CFStreamEventType event)
        {
            if (request->state>0) {
                request->state = status;
                HttpEventHandler* eh = request->client->GetEventHandler();
                if (eh!=0) eh->OnRequestStateChanged(request);
            }
        }

        static void OnHeadersRecieved(CHttpRequest* request, CFReadStreamRef stream, CFStreamEventType event)
        {
            //get the response
            CFHTTPMessageRef response = (CFHTTPMessageRef) CFReadStreamCopyProperty (stream, kCFStreamPropertyHTTPResponseHeader);
            //headers
            CFDictionaryRef nHeaders = CFHTTPMessageCopyAllHeaderFields(response);
            request->NHeadersToHeaders(nHeaders);
            CFRelease(nHeaders);
            //responseStatus
            CFIndex code = CFHTTPMessageGetResponseStatusCode(response);
            request->responseStatus = (int)code;

            //CFRelease(nHeaders); //{TODO} this crashes...why?
        }

        static void OnByteDataRecieved(CHttpRequest* request, CFReadStreamRef stream, CFStreamEventType event)
        {
            UInt8 buff[1024];
            CFIndex nBytesRead = CFReadStreamRead(stream, buff, 1024);

            if(nBytesRead>0)
            {
                CFWriteStreamWrite(request->cachedContentStream, (UInt8*)(&buff), (CFIndex)nBytesRead);
                request->readPosition += nBytesRead;
            }
            HttpEventHandler* eh = request->client->GetEventHandler();
            if (eh!=0) eh->OnRequestProgress(request, request->readPosition, 0, false);
        }

        static void OnTimeout(CHttpRequest* request, CFReadStreamRef stream, CFStreamEventType event)
        {
            HttpEventHandler* eh = request->client->GetEventHandler();
            if (eh!=0) eh->OnRequestTimeout(request);
        }

        static void OnError(CHttpRequest* request, CFReadStreamRef stream, CFStreamEventType event)
        {
            HttpEventHandler* eh = request->client->GetEventHandler();
            request->state = HttpRequestStateDone;
            request->errored = true;

            CFStreamError err = CFReadStreamGetError(stream);
            if (err.domain == kCFStreamErrorDomainPOSIX)
            {
                String error = String("Posix Error: ") + String((int)err.error);
                if (eh!=0) eh->OnRequestError(request, error);
            } else if (err.domain == kCFStreamErrorDomainMacOSStatus) {
                //OSStatus macError = (OSStatus)err.error; {TODO} use this
                String error = String("OSX Error: ") + String((int)err.error);
                if (eh!=0) eh->OnRequestError(request, error);
            } else if (err.domain == kCFStreamErrorDomainHTTP) {
                CFStreamErrorHTTP httpErr = (CFStreamErrorHTTP)err.error;
                String message;
                switch (httpErr) {
                case kCFStreamErrorHTTPParseFailure:
                    message = "CFStreamError: HTTP Parse Failure";
                    break;
                case kCFStreamErrorHTTPRedirectionLoop:
                    message = "CFStreamError: HTTP Redirection Loop";
                    break;
                case kCFStreamErrorHTTPBadURL:
                    message = "CFStreamError: HTTP Bad Url";

                    break;
                default:
                    message = "Unknown Http Error: ";
                    message += String((int)err.error);
                    break;
                }
                if (eh!=0) eh->OnRequestError(request, message);
            } else {
                String error = String("Unidentified Error in XLI Http: ") + String((int)err.error);
                if (eh!=0) eh->OnRequestError(request,error);
            }
        }

        static void AsyncCallback(CFReadStreamRef stream, CFStreamEventType event, void* ptr)
        {
            CHttpRequest* request = (CHttpRequest*)ptr;
            switch (event)
            {
            case kCFStreamEventOpenCompleted:                
                CHttpRequest::OnHeadersRecieved(request, stream, event);
                CHttpRequest::OnStateChanged(request, HttpRequestStateHeadersReceived, stream, event);
                request->StartDownload();
                break;
            case kCFStreamEventCanAcceptBytes:
                // {TODO} not currently used but will be needed for upload progress
                break;
            case kCFStreamEventHasBytesAvailable:
                Error->WriteLine("heflhshfidfhsdhfsdfhdfshjdhfisdfpisdphfsidfhpsd");
                if (request->reading == true)
                {
                    request->dataReady = true;
                    CHttpRequest::OnByteDataRecieved(request, stream, event);
                } else {
                    request->dataReady = true;
                }
                break;
            case kCFStreamEventErrorOccurred:
                CHttpRequest::OnError(request, stream, event);
                break;
            case kCFStreamEventEndEncountered:
                request->state = HttpRequestStateDone;
                if (!request->errored) 
                {
                    request->getContentArray();
                    CHttpRequest::OnStateChanged(request, HttpRequestStateDone, stream, event);

                    if (request->uploadData!=0) CFRelease(request->uploadData);

                    if (request->cachedReadStream!=0)
                    {
                        CFReadStreamClose(request->cachedReadStream);
                        CFRelease(request->cachedReadStream);
                    }
                }
                break;
            }
        }

        virtual void getContentArray()
        {
            if (state == HttpRequestStateDone && !errored)
            {
                CFStringRef prop = CFSTR("kCFStreamPropertyDataWritten");
                CFDataRef streamDataHandle = (CFDataRef)CFWriteStreamCopyProperty(cachedContentStream, prop);
                long len = CFDataGetLength(streamDataHandle);
                readPosition = len;

                responseBody = Buffer::Create((int)len);
                CFDataGetBytes(streamDataHandle, CFRangeMake(0,len), (UInt8*)responseBody->GetPtr());

                CFWriteStreamClose(cachedContentStream);
                CFRelease(cachedContentStream);
                CFRelease(streamDataHandle);
                cachedContentStream = 0;
            } else {
                XLI_THROW("Not in correct state to get content array"); // {TODO} error needed here
            }
        }
    };

    class CHttpClient : public HttpClient
    {
    private:
        Managed<HttpEventHandler> eventHandler;
    public:
        virtual CHttpRequest* CreateRequest(const String& method, const String& url)
        {
            return new CHttpRequest(this, url, method);
        }
        virtual void Update() { }

        virtual void SetEventHandler(HttpEventHandler* eventHandler)
        {
            this->eventHandler = eventHandler;
        }

        virtual HttpEventHandler* GetEventHandler()
        {
            return eventHandler;
        }
    };

    HttpClient* HttpClient::Create()
    {
        return new CHttpClient();
    }
};
