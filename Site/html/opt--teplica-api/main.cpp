#include "crow_all.h"
#include <fstream>
#include <filesystem>
#include <regex>

namespace fs = std::filesystem;

int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/api/upload/<string>").methods("POST"_method)
    ([](const crow::request& req, std::string code) {
        // Проверка: 6 символов, только буквы/цифры, чувствительно к регистру
        std::regex code_format("^[A-Za-z0-9]{6}$");
        if (!std::regex_match(code, code_format)) {
            return crow::response(400, "Invalid code format (must be 6 uppercase alphanumerics)");
        }

        // Директория и путь к файлу
        std::string dir = "/var/www/teplica";
        std::string filename = "data_" + code + ".json";
        std::string filepath = dir + "/" + filename;

        try {
            fs::create_directories(dir); // создаёт, если нет
            std::ofstream out(filepath, std::ios::app);
            if (!out.is_open()) {
                return crow::response(500, "Cannot open file for writing");
            }

            // Добавляем тело POST-запроса как строку JSON
            out << req.body << "\n";
            out.close();
            return crow::response(200, "Data appended");
        } catch (const std::exception& e) {
            return crow::response(500, std::string("Exception: ") + e.what());
        }
    });

    app.port(18080).multithreaded().run();
}
