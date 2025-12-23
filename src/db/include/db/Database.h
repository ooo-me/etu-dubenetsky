// ============================================================================
// Database Connection Manager
// Описание: Менеджер подключения к PostgreSQL базе данных
// ============================================================================

#pragma once

#include <memory>
#include <vector>
#include <map>
#include <optional>
#include <stdexcept>
#include <string>

#include <libpq-fe.h>

namespace db
{
// Исключение при работе с базой данных
class Exception : public std::runtime_error
{
public:
    explicit Exception(const std::string& message);
};

// Результат выполнения SQL запроса
class QueryResult
{
public:
    explicit QueryResult(PGresult* result);

    // Получение количества строк в результате
    int GetRowCount() const;

    // Получение количества колонок
    int GetColumnCount() const;

    // Получение значения ячейки как строки
    std::optional<std::string> GetValue(int row, int col) const;

    // Получение значения ячейки по имени колонки
    std::optional<std::string> GetValue(int row, const std::string& columnName) const;

    // Получение значения как integer
    std::optional<int> GetInt(int row, int col) const;

    // Получение значения как double
    std::optional<double> GetDouble(int row, int col) const;

    // Проверка успешности выполнения
    bool IsSuccess() const;

    // Получение сообщения об ошибке
    std::string GetErrorMessage() const;

private:
    std::shared_ptr<PGresult> result_;
};

// Менеджер подключения к базе данных
class DatabaseManager
{
public:
    // Параметры подключения к БД
    struct ConnectionParams
    {
        std::string host = "localhost";
        std::string port = "5433"; // PostgreSQL 15 (5432 занят PostgreSQL 17)
        std::string database = "tariff_system";
        std::string user = "postgres";
        std::string password = "postgres";
    };

    // Конструктор
    DatabaseManager() = default;

    // Деструктор
    ~DatabaseManager();

    // Запрет копирования
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    // Подключение к базе данных
    bool Connect(const ConnectionParams& params);

    // Отключение от базы данных
    void Disconnect();

    // Проверка подключения
    bool IsConnected() const;

    // Выполнение SQL запроса
    std::unique_ptr<QueryResult> ExecuteQuery(const std::string& query);

    // Выполнение параметризованного запроса
    std::unique_ptr<QueryResult> executeQuery(const std::string& query, const std::vector<std::string>& params);

    // Начало транзакции
    void BeginTransaction();

    // Подтверждение транзакции
    void Commit();

    // Откат транзакции
    void Rollback();

    // Выполнение SQL команды без возврата результата
    void Execute(const std::string& query);

    // Получение последней ошибки
    const std::string& GetLastError() const;

    // Экранирование строки для SQL
    std::string EscapeString(const std::string& str) const;

private:
    PGconn* conn_ = nullptr;
    std::string lastError_;
};

// RAII обертка для транзакций
class Transaction
{
public:
    explicit Transaction(DatabaseManager& db);

    ~Transaction();

    void Commit();

    void Rollback();

private:
    DatabaseManager& db_;
    bool committed_;
};

} // namespace db
