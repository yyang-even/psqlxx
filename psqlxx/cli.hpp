#pragma once

#include <functional>
#include <memory>
#include <string>

#include <psqlxx/command.hpp>


struct editline;
typedef editline EditLine;

struct history;
typedef history History;

struct HistEvent;

struct tokenizer;
typedef tokenizer Tokenizer;


namespace psqlxx {

struct CliOptions {
    std::string editor = "vi";
    std::string history_file;

    const char *editrc_file = nullptr;

    int history_size = 10000;

    CliOptions();
};


class Cli {
    using LineHandlerType = std::function<void(const char *)>;

    const CliOptions m_options;

    std::vector<CommandGroup> m_command_groups;

    EditLine *m_el = nullptr;

    History *m_history = nullptr;
    std::unique_ptr<HistEvent> m_ev;

    Tokenizer *m_tokenizer = nullptr;

    LineHandlerType m_line_handler;

public:
    Cli(const char *program_path, CliOptions options);
    Cli(const Cli &) = delete;
    Cli &operator=(const Cli &) = delete;
    ~Cli();

    void Config() const;
    void Run() const;
    void RegisterLineHandler(const LineHandlerType handler);
};

}//namespace psqlxx
