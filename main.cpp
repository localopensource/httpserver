#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include <unordered_map>
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

using SOCKET = int;
const SOCKET INVALID_SOCKET = -1;
const int SOCKET_ERROR = -1;

class HTTPServer {
public:
    HTTPServer(int port) : port(port), server_fd(INVALID_SOCKET) {
        post_routes["/api/echo"] = [](const std::string& body) {
            std::string json = "{\"echo\": \"" + body + "\"}";
            return "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(json.size()) + "\r\n\r\n" + json;
        };

        post_routes["/api/reverse"] = [](const std::string& body) {
            std::string reversed(body.rbegin(), body.rend());
            std::string json = "{\"reversed\": \"" + reversed + "\"}";
            return "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(json.size()) + "\r\n\r\n" + json;
        };
    }

    ~HTTPServer() {
        if (server_fd != INVALID_SOCKET) {
            close(server_fd);
        }
    }

    void start() {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == INVALID_SOCKET) {
            throw std::runtime_error("Failed to create socket");
        }

        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
            throw std::runtime_error("Failed to bind to port " + std::to_string(port));
        }

        if (listen(server_fd, 10) == SOCKET_ERROR) {
            throw std::runtime_error("Failed to listen on socket");
        }

        std::cout << "Server successfully started on port " << port << " (WSL)" << std::endl;
        std::cout << "Access via http://localhost:" << port << std::endl;

        while (true) {
            sockaddr_in client_addr{};
            socklen_t client_len = sizeof(client_addr);
            SOCKET client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
            
            if (client_socket == INVALID_SOCKET) {
                std::cerr << "Failed to accept client connection" << std::endl;
                continue;
            }

            handle_client(client_socket);
        }
    }

    void handle_client(SOCKET client_socket) {
        char buffer[4096] = {0};
        ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            std::string request(buffer);
            
            std::cout << "[INFO] Received request:\n" << request.substr(0, request.find("\r\n")) << std::endl;

            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\r\nHello from WSL!";
            
            write(client_socket, response.c_str(), response.size());
        }
        close(client_socket);
    }

    std::string handle_get_request(const std::string& file_path) {
        std::string file_content = get_file_content(file_path);
        if (file_content.empty()) {
            return "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: 9\r\n\r\nNot Found";
        }
        std::string mime_type = get_mime_type(file_path);
        std::ostringstream response;
        response << "HTTP/1.1 200 OK\r\n"
                 << "Content-Type: " << mime_type << "\r\n"
                 << "Content-Length: " << file_content.size() << "\r\n"
                 << "\r\n"
                 << file_content;
        return response.str();
    }

private:
    int port;
    SOCKET server_fd;
    std::unordered_map<std::string, std::string> mime_types;
    std::unordered_map<std::string, std::function<std::string(const std::string&)>> get_routes;
    std::unordered_map<std::string, std::function<std::string(const std::string&)>> post_routes;

    std::string get_file_content(const std::string& path) { return ""; }
    std::string get_mime_type(const std::string& path) { return "text/plain"; }
};

int main(int argc, char* argv[]) {
    int port = 6969;

    if (argc > 1) {
        try {
            port = std::stoi(argv[1]);
            if (port < 1 || port > 65535) {
                throw std::out_of_range("Port number out of range");
            }
        }
        catch (const std::exception& e) {
            port = 8080;
            std::cerr << "Invalid port number. Using default port: " << port << std::endl;
        }
    }

    try {
        HTTPServer server(port);
        server.start();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}