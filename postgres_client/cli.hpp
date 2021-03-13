#pragma once

#include <string>


struct EditLine;
struct History;
struct HistEvent;

namespace postgres_client {

struct CliOptions {
    int history_size = 10000;
    std::string editor = "vi";
    std::string history_file;
    const char *editrc_file = nullptr;

    CliOptions();
};

class Cli {
    EditLine *m_el = nullptr;

    History *m_history = nullptr;
    HistEvent m_ev;

    const CliOptions m_options;

public:
    Cli(const char *program_path, CliOptions options);
    ~Cli();
};

}//namespace postgres_client
