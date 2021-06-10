#include <psqlxx/cli.hpp>

#include <cassert>
#include <csignal>

#include <filesystem>
#include <iostream>

#include <histedit.h>


using namespace psqlxx;
namespace fs = std::filesystem;

namespace {

[[nodiscard]]
inline const char *prompt(const std::string &/*user_name = "postgres"*/) {
    // return (user_name + " > ").c_str();
    return "postgres > ";
}

[[nodiscard]]
inline const std::string getDefaultHistoryFile() {
    fs::path home_dir(getenv("HOME"));
    return (home_dir / ".postgres-client.hist").string();
}

[[nodiscard]]
inline auto help(const std::vector<CommandGroup> &command_groups,
                 const char **words,
                 const int word_count) {
    assert(words);
    assert(word_count > 0);

    if (word_count == 2) {
        return HelpGroups(command_groups, words[1]);
    } else {
        return HelpGroups(command_groups);
    }
}

[[nodiscard]]
inline auto doEditlineBuiltinCommands(EditLine *const el,
                                      const char **words,
                                      const int word_count) {
    assert(el);
    assert(words);
    assert(word_count > 0);

    const auto result = el_parse(el, word_count, words);
    switch (result) {
        case -1:
            return CommandResult::unknown;
        case 0:
            return CommandResult::success;
        default:
            return CommandResult::failure;
    }
}

[[nodiscard]]
inline const auto
createBuiltinCommandGroup(const std::vector<CommandGroup> &command_groups,
                          EditLine *const el) {
    CommandGroup group{"builtin", "quit, exit, help and builtin editline commands"};

    group.AddOptions()
    // '@' is a better command prefix when using Tokenizer, as it escapes '\'.
    ({"quit", "exit", "@q"}, {}, &Quit, "To quit")
    ({"help"}, {"[GROUP]"}, [&command_groups](const auto words, const auto word_count) {
        return help(command_groups, words, word_count);
    }, "Print help summary or for an individual group")
    ({}, {VARIADIC_ARGUMENT}, [el](const auto words, const auto word_count) {
        return doEditlineBuiltinCommands(el, words, word_count);
    }, "Execute builtin editline commands, refer to editrc(5) for more information")
    ;

    return group;
}

std::function<void(int)> g_signal_handler;

extern "C" void signalHandler(int sig) {
    if (g_signal_handler) {
        g_signal_handler(sig);
    }
}

}


namespace psqlxx {

CliOptions::CliOptions(FILE *f_in): history_file(getDefaultHistoryFile()),
    input_file(f_in ? f_in : stdin) {
}


void Cli::handleSignal() const {
    el_reset(m_el);
    tok_reset(m_tokenizer);

    std::cout << R"(
Type "quit", "exit", or "@q" to quit.
Press <Enter> to continue: )" << std::flush;

    m_signal_received = true;
}

Cli::Cli(const char *program_path, CliOptions options) : m_options(std::move(options)),
    m_el(el_init(program_path, options.input_file, stdout, stderr)),
    m_history(history_init()),
    m_ev(new HistEvent()),
    m_tokenizer(tok_init(nullptr)) {
    // Keep this statement after other members have been constructed.
    m_command_groups.emplace_back(createBuiltinCommandGroup(m_command_groups, m_el));
}

Cli::~Cli() {
    tok_end(m_tokenizer);

    history(m_history, m_ev.get(), H_SAVE, m_options.history_file.c_str());
    history_end(m_history);

    el_end(m_el);

    g_signal_handler = {};
}

void Cli::Config() const {
    g_signal_handler = [this](int) {
        handleSignal();
    };
    (void) std::signal(SIGINT, signalHandler);  // Handle 'Ctrl+c'
    (void) std::signal(SIGQUIT, signalHandler); // Handle 'Ctrl+\'

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

bool Cli::Run() const {
    const char *a_line = nullptr;
    int line_length = 0;
    bool previous_line_completed = true;
    bool last_result = true;

    while ((a_line = el_gets(m_el, &line_length)) and line_length != 0)  {
        if (m_signal_received.load()) {
            previous_line_completed = true;
            m_signal_received = false;
        }

        // Ignore empty lines
        if (previous_line_completed && line_length == 1) {
            continue;
        }

        int word_count = 0;
        const char **words = nullptr;
        const auto tokenize_result = tok_str(m_tokenizer, a_line, &word_count, &words);
        if (tokenize_result < 0) {
            std::cerr << "Internal error." << std::endl;
            last_result = false;
            continue;
        }
        const auto current_line_completed = not(tokenize_result > 0);

        history(m_history, m_ev.get(), previous_line_completed ? H_ENTER : H_APPEND, a_line);
        previous_line_completed = current_line_completed;
        if (not current_line_completed) {
            continue;
        }

        if (word_count <= 0) {
            continue;
        }

        for (const auto &a_group : m_command_groups) {
            const auto result = a_group(words, word_count);
            if (result == CommandResult::exit) {
                return true;
            } else {
                last_result = (result == CommandResult::success);
                if (result != CommandResult::unknown) {
                    break;
                }
            }
        }

        tok_reset(m_tokenizer);
    }

    return last_result;
}

void Cli::RegisterCommandGroup(CommandGroup group) {
    m_command_groups.push_back(std::move(group));
}

}//namespace psqlxx
