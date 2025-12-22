// ============================================================================
// Database Connection Manager
// Язык: C++20
// Описание: Менеджер подключения к PostgreSQL базе данных
// ============================================================================

#pragma once

#include "../utils/Types.hpp"
#include <libpq-fe.h>
#include <memory>
#include <vector>
#include <map>
#include <optional>
#include <stdexcept>

namespace TariffSystem::Database {

/**
 * Результат выполнения SQL запроса
 */
class QueryResult {
public:
    explicit QueryResult(PGresult* result) : result_(result, &PQclear) {}
    
    /**
     * Получение количества строк в результате
     */
    int getRowCount() const {
        return PQntuples(result_.get());
    }
    
    /**
     * Получение количества колонок
     */
    int getColumnCount() const {
        return PQnfields(result_.get());
    }
    
    /**
     * Получение значения ячейки как строки
     */
    std::optional<String> getValue(int row, int col) const {
        if (PQgetisnull(result_.get(), row, col)) {
            return std::nullopt;
        }
        return String(PQgetvalue(result_.get(), row, col));
    }
    
    /**
     * Получение значения ячейки по имени колонки
     */
    std::optional<String> getValue(int row, const String& columnName) const {
        int col = PQfnumber(result_.get(), columnName.c_str());
        if (col == -1) {
            return std::nullopt;
        }
        return getValue(row, col);
    }
    
    /**
     * Получение значения как integer
     */
    std::optional<Integer> getInt(int row, int col) const {
        auto val = getValue(row, col);
        if (!val) return std::nullopt;
        try {
            return std::stoi(*val);
        } catch (...) {
            return std::nullopt;
        }
    }
    
    /**
     * Получение значения как double
     */
    std::optional<Double> getDouble(int row, int col) const {
        auto val = getValue(row, col);
        if (!val) return std::nullopt;
        try {
            return std::stod(*val);
        } catch (...) {
            return std::nullopt;
        }
    }
    
    /**
     * Проверка успешности выполнения
     */
    bool isSuccess() const {
        auto status = PQresultStatus(result_.get());
        return status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK;
    }
    
    /**
     * Получение сообщения об ошибке
     */
    String getErrorMessage() const {
        return PQresultErrorMessage(result_.get());
    }

private:
    std::shared_ptr<PGresult> result_;
};

/**
 * Менеджер подключения к базе данных
 */
class DatabaseManager {
public:
    /**
     * Параметры подключения к БД
     */
    struct ConnectionParams {
        String host = "localhost";
        String port = "5433";  // PostgreSQL 15 (5432 занят PostgreSQL 17)
        String database = "tariff_system";
        String user = "postgres";
        String password = "postgres";
    };
    
    /**
     * Конструктор
     */
    DatabaseManager() = default;
    
    /**
     * Деструктор
     */
    ~DatabaseManager() {
        disconnect();
    }
    
    // Запрет копирования
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    
    /**
     * Подключение к базе данных
     */
    bool connect(const ConnectionParams& params) {
        if (isConnected()) {
            disconnect();
        }
        
        // Формирование строки подключения
        String connInfo = 
            "host=" + params.host + 
            " port=" + params.port + 
            " dbname=" + params.database + 
            " user=" + params.user;
        
        if (!params.password.empty()) {
            connInfo += " password=" + params.password;
        }
        
        // Установка подключения
        conn_ = PQconnectdb(connInfo.c_str());
        
        if (PQstatus(conn_) != CONNECTION_OK) {
            lastError_ = PQerrorMessage(conn_);
            PQfinish(conn_);
            conn_ = nullptr;
            return false;
        }
        
        // Установка кодировки UTF-8
        PQexec(conn_, "SET client_encoding = 'UTF8'");
        
        return true;
    }
    
    /**
     * Отключение от базы данных
     */
    void disconnect() {
        if (conn_) {
            PQfinish(conn_);
            conn_ = nullptr;
        }
    }
    
    /**
     * Проверка подключения
     */
    bool isConnected() const {
        return conn_ && PQstatus(conn_) == CONNECTION_OK;
    }
    
    /**
     * Выполнение SQL запроса
     */
    std::unique_ptr<QueryResult> executeQuery(const String& query) {
        if (!isConnected()) {
            throw DatabaseException("Нет подключения к БД");
        }
        
        PGresult* result = PQexec(conn_, query.c_str());
        auto queryResult = std::make_unique<QueryResult>(result);
        
        if (!queryResult->isSuccess()) {
            lastError_ = queryResult->getErrorMessage();
            throw DatabaseException("Ошибка выполнения запроса: " + lastError_);
        }
        
        return queryResult;
    }
    
    /**
     * Выполнение параметризованного запроса
     */
    std::unique_ptr<QueryResult> executeQuery(
        const String& query,
        const std::vector<String>& params
    ) {
        if (!isConnected()) {
            throw DatabaseException("Нет подключения к БД");
        }
        
        // Подготовка параметров - строка "NULL" означает NULL значение
        std::vector<const char*> paramValues;
        for (const auto& param : params) {
            if (param == "NULL") {
                paramValues.push_back(nullptr);
            } else {
                paramValues.push_back(param.c_str());
            }
        }
        
        PGresult* result = PQexecParams(
            conn_,
            query.c_str(),
            static_cast<int>(params.size()),
            nullptr,
            paramValues.data(),
            nullptr,
            nullptr,
            0
        );
        
        auto queryResult = std::make_unique<QueryResult>(result);
        
        if (!queryResult->isSuccess()) {
            lastError_ = queryResult->getErrorMessage();
            throw DatabaseException("Ошибка выполнения запроса: " + lastError_);
        }
        
        return queryResult;
    }
    
    /**
     * Начало транзакции
     */
    void beginTransaction() {
        executeQuery("BEGIN");
    }
    
    /**
     * Подтверждение транзакции
     */
    void commit() {
        executeQuery("COMMIT");
    }
    
    /**
     * Откат транзакции
     */
    void rollback() {
        executeQuery("ROLLBACK");
    }
    
    /**
     * Получение последней ошибки
     */
    const String& getLastError() const {
        return lastError_;
    }
    
    /**
     * Экранирование строки для SQL
     */
    String escapeString(const String& str) const {
        if (!isConnected()) {
            throw DatabaseException("Нет подключения к БД");
        }
        
        std::vector<char> buffer(str.length() * 2 + 1);
        PQescapeStringConn(conn_, buffer.data(), str.c_str(), str.length(), nullptr);
        return String(buffer.data());
    }

private:
    PGconn* conn_ = nullptr;
    String lastError_;
};

/**
 * RAII обертка для транзакций
 */
class Transaction {
public:
    explicit Transaction(DatabaseManager& db) : db_(db), committed_(false) {
        db_.beginTransaction();
    }
    
    ~Transaction() {
        if (!committed_) {
            try {
                db_.rollback();
            } catch (...) {
                // Игнорируем ошибки при откате в деструкторе
            }
        }
    }
    
    void commit() {
        db_.commit();
        committed_ = true;
    }
    
    void rollback() {
        db_.rollback();
        committed_ = true;
    }

private:
    DatabaseManager& db_;
    bool committed_;
};

} // namespace TariffSystem::Database
