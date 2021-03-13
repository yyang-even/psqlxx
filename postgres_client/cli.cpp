#include <postgres_client/cli.hpp>

#include <filesystem>
#include <iostream>

#include <histedit.h>


namespace fs = std::filesystem;

namespace {

inline const char *prompt(const std::string &/*user_name = "postgres"*/) {
    // return (user_name + " > ").c_str();
    return "postgres > ";
}

inline const std::string getDefaultHistoryFile() {
    fs::path home_dir(getenv("HOME"));
    return (home_dir / ".postgres-client.hist").string();
}

}

namespace postgres_client {

CliOptions::CliOptions(): history_file(getDefaultHistoryFile()) {
}

Cli::Cli(const char *program_path, CliOptions options) : m_ev(new HistEvent()),
    m_options(std::move(options)) {
    m_el = el_init(program_path, stdin, stdout, stderr);

    m_history = history_init();
}

Cli::~Cli() {
    history(m_history, m_ev.get(), H_SAVE, m_options.history_file.c_str());
    history_end(m_history);

    el_end(m_el);
}

void Cli::Config() const {
    history(m_history, m_ev.get(), H_SETSIZE, m_options.history_size);
    history(m_history, m_ev.get(), H_SETUNIQUE, 1);

    el_set(m_el, EL_HIST, history, m_history);

    el_set(m_el, EL_EDITOR, m_options.editor.c_str());

    el_set(m_el, EL_SIGNAL, 1);

    el_set(m_el, EL_PROMPT, prompt);

    // el_set(m_el, EL_ADDFN, "ed-complete", "Complete argument", complete);

    // el_set(m_el, EL_BIND, "^I", "ed-complete", NULL);
    el_set(m_el, EL_BIND, "\b", "ed-delete-next-char", NULL);
    el_set(m_el, EL_BIND, "\033[A", "ed-prev-history", NULL);
    el_set(m_el, EL_BIND, "^r", "em-inc-search-prev", NULL);
    el_set(m_el, EL_BIND, "-a", "k", "ed-prev-line", NULL);
    el_set(m_el, EL_BIND, "-a", "j", "ed-next-line", NULL);

    el_source(m_el, m_options.editrc_file);
    history(m_history, m_ev.get(), H_LOAD, m_options.history_file.c_str());
}

void Cli::Run() const {
    const char *a_line = nullptr;
    int line_length = 0;
    while ((a_line = el_gets(m_el, &line_length)) and line_length != 0)  {
        history(m_history, m_ev.get(), H_ENTER, a_line);
        std::cout << a_line << std::endl;
    }
}

}//namespace postgres_client
