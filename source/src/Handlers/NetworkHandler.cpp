#include "Global.h"
#include "Presentation.h"
#include "Storage.h"
#include "Log.h"

#define BUFFER_LEN 128

void PresentationMgr::InitNetwork()
{
    uint32 port = sStorage->GetNetworkPort();
    if (port == 0)
        return;

#ifdef _WIN32
    WORD version = MAKEWORD(1,1);
    WSADATA data;
    if (WSAStartup(version, &data) != 0)
    {
        sLog->ErrorLog("NetworkHandler: Failed to start network service");
        return;
    }
#endif

    if ((m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        sLog->ErrorLog("NetworkHandler: Failed to create socket");
        return;
    }

    sockaddr_in sockName;

    sockName.sin_family = AF_INET;
    sockName.sin_port = htons(port);
    sockName.sin_addr.s_addr = INADDR_ANY;
    if (bind(m_socket, (sockaddr*)&sockName, sizeof(sockName)) == -1)
    {
        sLog->ErrorLog("NetworkHandler: Failed to bind socket");
        return;
    }

    if (listen(m_socket, 10) == -1)
    {
        sLog->ErrorLog("NetworkHandler: Couldn't create connection queue");
        return;
    }

#ifdef _WIN32
    u_long arg = 1;
    if (ioctlsocket(m_socket, FIONBIO, &arg) == SOCKET_ERROR)
    {
        sLog->ErrorLog("NetworkHandler: Failed to switch socket to non-blocking mode");
    }
#else
    int oldFlag = fcntl(m_socket, F_GETFL, 0);
    if (fcntl(m_socket, F_SETFL, oldFlag | O_NONBLOCK) == -1)
    {
        sLog->ErrorLog("NetworkHandler: Failed to switch socket to non-blocking mode");
    }
#endif
}

void PresentationMgr::UpdateNetwork()
{
    if (!sStorage->IsNetworkEnabled())
        return;

    char* buf = new char[BUFFER_LEN];
    int32 result, error, len = sizeof(sockaddr);
    SOCK res;

    res = accept(m_socket, (sockaddr*)&m_clientAddr, &len);
    error = LASTERROR();

    if (res == INVALID_SOCKET && error == SOCKETWOULDBLOCK)
    {
        // no incoming connection
    }
    else if (res == INVALID_SOCKET && error != SOCKETWOULDBLOCK)
    {
        sLog->ErrorLog("NetworkHandler: Unhandled socket error %i", error);
    }
    else
    {
        m_client = res;
        len = sizeof(sockaddr_in);
    }

    if (m_client != 0 && m_client != INVALID_SOCKET)
    {
        memset(buf,0,BUFFER_LEN);
        result = recv(m_client, buf, BUFFER_LEN, 0);
        error = LASTERROR();

        if (result > 0)
        {
            HandleExternalMessage(buf, result);
        }
        else if (result == 0 || error == SOCKETCONNRESET)
        {
            m_client = 0;
            return;
        }
        else
        {
            if (error != SOCKETWOULDBLOCK && error != 0)
                sLog->ErrorLog("NetworkHandler: Unhandled socket error %u", error);
        }
    }
}
