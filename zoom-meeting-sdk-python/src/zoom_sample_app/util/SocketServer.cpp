#include "SocketServer.h"

SocketServer::SocketServer() {
    pthread_mutex_init(&m_mutex, NULL);
}

SocketServer::~SocketServer() {
    cleanup();
}

void* SocketServer::threadCreate(void *obj) {
    return reinterpret_cast<SocketServer*>(obj)->run();
}

void SocketServer::threadKill(int sig, siginfo_t *info, void *ctx) {
    auto* self = &SocketServer::getInstance();

    self->stop();
}

void* SocketServer::run() {
    if (pthread_mutex_trylock(&m_mutex) != 0) {
        Log::error("Unable to lock mutex");
        return nullptr;
    }

    cleanup();

    m_listenSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (m_listenSocket == -1) {
        Log::error("unable to create listen socket");
        return nullptr;
    }

    memset(&m_addr, 0, sizeof(struct sockaddr_un));

    m_addr.sun_family = AF_UNIX;
    strncpy(m_addr.sun_path, c_socketPath.c_str(), sizeof(m_addr.sun_path) - 1);

    auto ret = bind(m_listenSocket, (const struct sockaddr *) &m_addr,
                    sizeof(struct sockaddr_un));
    if (ret == -1) {
        Log::error("unable to bind socket");
        return nullptr;
    }

    ret = listen(m_listenSocket, 20);
    if (ret == -1) {
        Log::error("unable to listen on socket");
        return nullptr;
    }

    Log::info("started socket server");
    Log::info("listening on socket " + c_socketPath);

    char buffer[c_bufferSize];

    for (;;) {
        m_dataSocket = accept(m_listenSocket, NULL, NULL);
        if (m_dataSocket == -1) {
            Log::error("failed to accept connection");
            return nullptr;
        }

        for(;;) {
            auto ret = read(m_dataSocket, buffer, 5);
            if (ret == -1) {
                Log::error("failed to read socket");
                return nullptr;
            }

            buffer[c_bufferSize - 1] = 0;
        }
        return nullptr;
    }

    return nullptr;
}

bool SocketServer::isReady() {
    return ready;
}


int SocketServer::writeBuf(const char* buf, int len) {
    auto ret = write(m_dataSocket, buf, len);
    if (ret == -1) {
        Log::error("failed to write data");
        exit(EXIT_FAILURE);
    }

    return 0;
}

int SocketServer::writeBuf(const unsigned char* buf, int len) {
    auto ret = write(m_dataSocket, buf, len);
    if (ret == -1) {
        Log::error("failed to write data");
        exit(EXIT_FAILURE);
    }

    return 0;
}

int SocketServer::writeStr(const string& str) {
    auto buf = str.c_str();
    return writeBuf(buf, strlen(buf));
}

void SocketServer::cleanup () {
    if (access(c_socketPath.c_str(), F_OK) != -1) {
        unlink(c_socketPath.c_str());
    }
}


int SocketServer::start() {
    if (pthread_create(&m_pid, NULL, &(SocketServer::threadCreate), this) != 0) {
        Log::error("unable to start thread");
        return false;
    }
    return true;
}

void SocketServer::stop() {
    pthread_mutex_destroy(&m_mutex);
    if (m_pid) {
        pthread_cancel(m_pid);
        m_pid = 0;
    }
    if (m_listenSocket) {
        close(m_listenSocket);
        m_listenSocket = 0;
    }

    if (m_dataSocket) {
        close(m_dataSocket);
        m_dataSocket = 0;
    }

    Log::info("Stopped Socket Server");
    cleanup();
}