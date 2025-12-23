#include "Database.h"

db::Exception::Exception(const std::string& message)
    : std::runtime_error("Ошибка БД: " + message)
{
}

db::QueryResult::QueryResult(PGresult* result)
    : result_(result, &PQclear)
{
}

// Получение количества строк в результате
int db::QueryResult::GetRowCount() const
{
    return PQntuples(result_.get());
}

// Получение количества колонок
int db::QueryResult::GetColumnCount() const
{
    return PQnfields(result_.get());
}

// Получение значения ячейки как строки
std::optional<std::string> db::QueryResult::GetValue(int row, int col) const
{
    if (PQgetisnull(result_.get(), row, col))
    {
        return std::nullopt;
    }
    return std::string(PQgetvalue(result_.get(), row, col));
}

// Получение значения ячейки по имени колонки
std::optional<std::string> db::QueryResult::GetValue(int row, const std::string& columnName) const
{
    int col = PQfnumber(result_.get(), columnName.c_str());
    if (col == -1)
    {
        return std::nullopt;
    }
    return GetValue(row, col);
}

// Получение значения как integer
std::optional<int> db::QueryResult::GetInt(int row, int col) const
{
    auto val = GetValue(row, col);
    if (!val)
        return std::nullopt;
    try
    {
        return std::stoi(*val);
    }
    catch (...)
    {
        return std::nullopt;
    }
}

// Получение значения как double
std::optional<double> db::QueryResult::GetDouble(int row, int col) const
{
    auto val = GetValue(row, col);
    if (!val)
        return std::nullopt;
    try
    {
        return std::stod(*val);
    }
    catch (...)
    {
        return std::nullopt;
    }
}

// Проверка успешности выполнения
bool db::QueryResult::IsSuccess() const
{
    auto status = PQresultStatus(result_.get());
    return status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK;
}

// Получение сообщения об ошибке
std::string db::QueryResult::GetErrorMessage() const
{
    return PQresultErrorMessage(result_.get());
}

// Деструктор
db::DatabaseManager::~DatabaseManager()
{
    Disconnect();
}

// Подключение к базе данных
bool db::DatabaseManager::Connect(const ConnectionParams& params)
{
    if (IsConnected())
    {
        Disconnect();
    }

    // Формирование строки подключения
    std::string connInfo =
        "host=" + params.host + " port=" + params.port + " dbname=" + params.database + " user=" + params.user;

    if (!params.password.empty())
    {
        connInfo += " password=" + params.password;
    }

    // Установка подключения
    conn_ = PQconnectdb(connInfo.c_str());

    if (PQstatus(conn_) != CONNECTION_OK)
    {
        lastError_ = PQerrorMessage(conn_);
        PQfinish(conn_);
        conn_ = nullptr;
        return false;
    }

    // Установка кодировки UTF-8
    PQexec(conn_, "SET client_encoding = 'UTF8'");

    return true;
}

// Отключение от базы данных
void db::DatabaseManager::Disconnect()
{
    if (conn_)
    {
        PQfinish(conn_);
        conn_ = nullptr;
    }
}

// Проверка подключения
bool db::DatabaseManager::IsConnected() const
{
    return conn_ && PQstatus(conn_) == CONNECTION_OK;
}

// Выполнение SQL запроса
std::unique_ptr<db::QueryResult> db::DatabaseManager::ExecuteQuery(const std::string& query)
{
    if (!IsConnected())
    {
        throw Exception("Нет подключения к БД");
    }

    PGresult* result = PQexec(conn_, query.c_str());
    auto queryResult = std::make_unique<QueryResult>(result);

    if (!queryResult->IsSuccess())
    {
        lastError_ = queryResult->GetErrorMessage();
        throw Exception("Ошибка выполнения запроса: " + lastError_);
    }

    return queryResult;
}

// Выполнение параметризованного запроса
std::unique_ptr<db::QueryResult> db::DatabaseManager::executeQuery(const std::string& query,
                                                                   const std::vector<std::string>& params)
{
    if (!IsConnected())
    {
        throw Exception("Нет подключения к БД");
    }

    // Подготовка параметров - строка "NULL" означает NULL значение
    std::vector<const char*> paramValues;
    for (const auto& param : params)
    {
        if (param == "NULL")
        {
            paramValues.push_back(nullptr);
        }
        else
        {
            paramValues.push_back(param.c_str());
        }
    }

    PGresult* result = PQexecParams(
        conn_, query.c_str(), static_cast<int>(params.size()), nullptr, paramValues.data(), nullptr, nullptr, 0);

    auto queryResult = std::make_unique<QueryResult>(result);

    if (!queryResult->IsSuccess())
    {
        lastError_ = queryResult->GetErrorMessage();
        throw Exception("Ошибка выполнения запроса: " + lastError_);
    }

    return queryResult;
}

// Начало транзакции
void db::DatabaseManager::BeginTransaction()
{
    ExecuteQuery("BEGIN");
}

// Подтверждение транзакции
void db::DatabaseManager::Commit()
{
    ExecuteQuery("COMMIT");
}

// Откат транзакции
void db::DatabaseManager::Rollback()
{
    ExecuteQuery("ROLLBACK");
}

// Выполнение SQL команды без возврата результата
void db::DatabaseManager::Execute(const std::string& query)
{
    ExecuteQuery(query);
}

// Получение последней ошибки
const std::string& db::DatabaseManager::GetLastError() const
{
    return lastError_;
}

// Экранирование строки для SQL
std::string db::DatabaseManager::EscapeString(const std::string& str) const
{
    if (!IsConnected())
    {
        throw Exception("Нет подключения к БД");
    }

    std::vector<char> buffer(str.length() * 2 + 1);
    PQescapeStringConn(conn_, buffer.data(), str.c_str(), str.length(), nullptr);
    return std::string(buffer.data());
}

db::Transaction::Transaction(DatabaseManager& db)
    : db_(db)
    , committed_(false)
{
    db_.BeginTransaction();
}

db::Transaction::~Transaction()
{
    if (!committed_)
    {
        try
        {
            db_.Rollback();
        }
        catch (...)
        {
            // Игнорируем ошибки при откате в деструкторе
        }
    }
}

void db::Transaction::Commit()
{
    db_.Commit();
    committed_ = true;
}

void db::Transaction::Rollback()
{
    db_.Rollback();
    committed_ = true;
}
