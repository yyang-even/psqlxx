#include <psqlxx/db.hpp>
#include <psqlxx/string_utils.hpp>

#include <unistd.h>

#include <iostream>
#include <unordered_map>

#include <cxxopts.hpp>
#include <pqxx/pqxx>


using namespace psqlxx;


namespace {

[[nodiscard]]
inline const std::string_view getTransactionName() {
    return "psqlxx";
}

[[nodiscard]]
inline const auto
overridePasswordFromPrompt(std::string connection_string) {
    const auto password = getpass("Password: ");
    return internal::overridePassword(std::move(connection_string), password);
}

[[nodiscard]]
inline const auto
concatenateKeyValue(std::string key, std::string value) {
    return Joiner{'='}(std::move(key), std::move(value));
}

[[nodiscard]]
inline auto
doTransaction(const DbProxy &proxy,
              const char **words, const int word_count) {
    std::stringstream query;
    for (int i = 0; i < word_count; ++i) {
        query << words[i] << " ";
    }

    return ToCommandResult(proxy.DoTransaction(query.str()));
}

[[nodiscard]]
inline const std::string_view buildListDBsSql() {
    return R"(
SELECT d.datname as "Name",
       pg_catalog.pg_get_userbyid(d.datdba) as "Owner",
       pg_catalog.pg_encoding_to_char(d.encoding) as "Encoding",
       d.datcollate as "Collate",
       d.datctype as "Ctype",
       pg_catalog.array_to_string(d.datacl, E'\n') AS "Access privileges"
FROM pg_catalog.pg_database d
ORDER BY 1;
)";
}

void addConnectionOptions(cxxopts::Options &options) {
    options.add_options("DB Connection")
    ("S,connection-string",
     "PQ connection string. Refer to the libpq connect call for a complete definition of what may go into the connect string. By default the client will try to connect to a server running on the local machine.",
     cxxopts::value<std::string>()->default_value(""))
    ("w,no-password", "never prompt for password",
     cxxopts::value<bool>()->default_value("false"))
    ;
}

[[nodiscard]]
const auto handleConnectionOptions(const cxxopts::ParseResult &parsed_options) {
    ConnectionOptions options{};

    options.base_connection_string = parsed_options["connection-string"].as<std::string>();
    options.prompt_for_password = not parsed_options["no-password"].as<bool>();

    return options;
}

}


namespace psqlxx {

namespace internal {

const std::string overridePassword(std::string connection_string,
                                   std::string password) {
    const auto was_connection_str_empty = connection_string.empty();

    if (StartsWith(connection_string, "postgresql://") or
        StartsWith(connection_string, "postgres://")) {
        if (connection_string.rfind('?') == std::string::npos) {
            connection_string.push_back('?');
        } else {
            connection_string.push_back('&');
        }
    } else {
        if (not was_connection_str_empty) {
            connection_string.push_back(' ');
        }
    }

    return connection_string + ComposeDbParameter(DbParameterKey::password,
                                                  std::move(password));
}

std::unique_ptr<pqxx::connection> makeConnection(const ConnectionOptions &options) {
    for (bool original_tried = false; true; original_tried = true) {
        try {
            if (original_tried and options.prompt_for_password) {
                const auto connection_string =
                    overridePasswordFromPrompt(options.base_connection_string);
                return std::make_unique<pqxx::connection>(connection_string);
            } else {
                return std::make_unique<pqxx::connection>(options.base_connection_string);
            }
        } catch (const pqxx::broken_connection &e) {
            if (original_tried or not strstr(e.what(), "no password supplied")) {
                std::cerr << e.what() << std::endl;
                break;
            }
        }
    }

    return {};
}

}//namespace internal

DbProxy::DbProxy(DbProxyOptions options): m_options(std::move(options)),
    m_out(std::cout.rdbuf()){
    m_connection = internal::makeConnection(m_options.connection_options);

    if (not m_options.format_options.out_file.empty()) {
        m_out_file.open(m_options.format_options.out_file, std::ofstream::out);
        if (m_out_file) {
            m_out.rdbuf(m_out_file.rdbuf());
        } else {
            std::cerr << "Failed to open out file '" <<
                      m_options.format_options.out_file <<
                      "': " << strerror(errno);
        }
    }
}

bool DbProxy::DoTransaction(const std::string_view sql_cmd) const {
    assert(*this);

    return pqxx::perform([this, sql_cmd] {
        try {
            pqxx::work w(*(m_connection), getTransactionName());

            auto r = w.exec(sql_cmd);

            PrintResult(r, m_options.format_options, m_out);
            return true;

        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
    });
}

void AddDbProxyOptions(cxxopts::Options &options) {
    addConnectionOptions(options);

    options.add_options("Command")
    ("l,list-dbs", "list available databases, then exit",
     cxxopts::value<bool>()->default_value("false"))
    ("c,command", "run only single command (SQL or internal) and exit",
     cxxopts::value<std::vector<std::string>>(), "COMMAND")
    ("f,command-file", "execute commands from file, then exit",
     cxxopts::value<std::string>()->default_value(""))
    ;

    AddFormatOptions(options);
}

const DbProxyOptions HandleDbProxyOptions(const cxxopts::ParseResult &parsed_options) {
    DbProxyOptions options{handleConnectionOptions(parsed_options),
        HandleFormatOptions(parsed_options)};

    options.list_DBs_and_exit = parsed_options["list-dbs"].as<bool>();

    if (parsed_options.count("command")) {
        options.commands = parsed_options["command"].as<std::vector<std::string>>();
    }

    options.command_file = parsed_options["command-file"].as<std::string>();

    return options;
}

const std::string
ComposeDbParameter(const DbParameterKey key_enum, std::string value) {
    const static std::unordered_map<DbParameterKey, std::string> DB_PARAMETER_KEY_MAP {
        {DbParameterKey::host, "host"},
        {DbParameterKey::port, "port"},
        {DbParameterKey::dbname, "dbname"},
        {DbParameterKey::user, "user"},
        {DbParameterKey::password, "password"},
    };

    return concatenateKeyValue(DB_PARAMETER_KEY_MAP.at(key_enum), std::move(value));
}

bool ListDbs(const DbProxy &db_proxy) {
    const auto list_dbs_sql = buildListDBsSql();
    return db_proxy.DoTransaction(list_dbs_sql);
}

const CommandGroup
CreatePsqlxxCommandGroup(const DbProxy &proxy) {
    CommandGroup group{"psqlxx", "psqlxx commands"};

    group.AddOptions()
    ({""}, {VARIADIC_ARGUMENT},
     [&proxy](const auto words, const auto word_count) {
        return doTransaction(proxy, words, word_count);
     }, "To execute query")
    ({"@l"}, {}, [&proxy](const auto, const auto) {
        return ToCommandResult(ListDbs(proxy));
    }, "List databases")
    ;

    return group;
}

}//namespace psqlxx
