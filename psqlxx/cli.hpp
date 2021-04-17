#pragma once

#include <atomic>
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
    const CliOptions m_options;

    std::vector<CommandGroup> m_command_groups;

    mutable std::atomic<bool> m_signal_received{false};

    EditLine *m_el = nullptr;

    History *m_history = nullptr;
    const std::unique_ptr<HistEvent> m_ev;

    Tokenizer *m_tokenizer = nullptr;

    void handleSignal() const;
public:
    Cli(const char *program_path, CliOptions options);
    Cli(const Cli &) = delete;
    Cli &operator=(const Cli &) = delete;
    Cli(Cli &&) = delete;
    ~Cli();

    void Config() const;
    [[nodiscard]]
    bool Run() const;
    void RegisterCommandGroup(CommandGroup group);
};

}//namespace psqlxx
