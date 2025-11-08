

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <algorithm>

#ifdef _WIN32
  #define NOMINMAX
  #include <windows.h>
  #include <conio.h>
#else
  #include <termios.h>
  #include <unistd.h>
  #include <sys/ioctl.h>
  #include <sys/select.h>
#endif

// ===== ANSI helpers =====
namespace ansi {
    const char* CLEAR      = "\x1b[2J";
    const char* HOME       = "\x1b[H";
    const char* HIDE       = "\x1b[?25l";
    const char* SHOW       = "\x1b[?25h";
    const char* RESET      = "\x1b[0m";
    const char* BOLD       = "\x1b[1m";
    const char* INVERSE    = "\x1b[7m";
    const char* DIM        = "\x1b[2m";
    const char* FG_CYAN    = "\x1b[36m";
    const char* FG_GREEN   = "\x1b[32m";
    const char* FG_YELLOW  = "\x1b[33m";
    const char* FG_RED     = "\x1b[31m";
}

static void flush() { std::cout.flush(); }
static void clear_screen() { std::cout << ansi::CLEAR << ansi::HOME; }
static void hide_cursor(bool hide=true) { std::cout << (hide ? ansi::HIDE : ansi::SHOW); }

// ===== Raw/VT setup =====
#ifdef _WIN32
static bool enable_vt_mode() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return false;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return false;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
    if (!SetConsoleMode(hOut, dwMode)) return false;
    return true;
}
#else
struct RawTerm {
    termios orig{};
    bool active = false;
    RawTerm() {
        if (!isatty(STDIN_FILENO)) return;
        if (tcgetattr(STDIN_FILENO, &orig) == -1) return;
        termios raw = orig;
        raw.c_lflag &= ~(ICANON | ECHO);
        raw.c_cc[VMIN]  = 1;
        raw.c_cc[VTIME] = 0;
        if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == -1) return;
        active = true;
    }
    ~RawTerm() {
        if (active) tcsetattr(STDIN_FILENO, TCSANOW, &orig);
    }
};
#endif

// ===== Keys =====
enum class Key { None, Up, Down, Left, Right, Enter, Quit };

// ===== Input (robust, non-blocking) =====
#ifndef _WIN32
static Key read_key() {
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(STDIN_FILENO, &rfds);
    timeval tv{}; // zero-timeout poll
    int ready = select(STDIN_FILENO + 1, &rfds, nullptr, nullptr, &tv);
    if (ready <= 0 || !FD_ISSET(STDIN_FILENO, &rfds)) return Key::None;

    unsigned char c;
    if (read(STDIN_FILENO, &c, 1) != 1) return Key::None;

    if (c == '\n' || c == '\r') return Key::Enter;
    if (c == 'q' || c == 'Q')   return Key::Quit;
    if (c != 27)                return Key::None;

    auto read_byte_nb = [](unsigned char& out)->bool {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);
        timeval tv{};
        int r = select(STDIN_FILENO + 1, &rfds, nullptr, nullptr, &tv);
        if (r <= 0 || !FD_ISSET(STDIN_FILENO, &rfds)) return false;
        return read(STDIN_FILENO, &out, 1) == 1;
    };

    unsigned char first{};
    if (!read_byte_nb(first)) return Key::Quit; // bare ESC

    if (first == '[' || first == 'O') {
        unsigned char ch{};
        for (int i = 0; i < 8; ++i) {
            if (!read_byte_nb(ch)) break;
            if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || ch == '~') {
                switch (ch) {
                    case 'A': return Key::Up;
                    case 'B': return Key::Down;
                    case 'C': return Key::Right;
                    case 'D': return Key::Left;
                    default:  return Key::None;
                }
            }
        }
        return Key::None;
    }
    return Key::None;
}
#else
static Key read_key() {
    if (!_kbhit()) return Key::None;
    int ch = _getch();
    if (ch == 27)               return Key::Quit;   // ESC
    if (ch == '\r')             return Key::Enter;  // Enter
    if (ch == 'q' || ch == 'Q') return Key::Quit;

    if (ch == 0 || ch == 224) { // extended
        int k = _getch();
        switch (k) {
            case 72: return Key::Up;
            case 80: return Key::Down;
            case 75: return Key::Left;
            case 77: return Key::Right;
            default: return Key::None;
        }
    }
    return Key::None;
}
#endif

// ===== Simple raw-mode line input =====
static std::string read_line_raw(const std::string& prompt) {
    clear_screen();
    hide_cursor(false);
    std::cout << ansi::BOLD << prompt << ansi::RESET << "\n";
    std::cout << "> ";
    flush();

    std::string buf;
    for (;;) {
#ifdef _WIN32
        int ch = _getch();
        if (ch == '\r') { std::cout << "\n"; break; }   // Enter
        if (ch == 3)   { break; } // Ctrl-C
        if (ch == 8) { // Backspace
            if (!buf.empty()) { buf.pop_back(); std::cout << "\b \b"; flush(); }
            continue;
        }
        if (ch == 0 || ch == 224) { (void)_getch(); continue; } // ignore arrows
        if (ch >= 32 && ch <= 126) { buf.push_back((char)ch); std::cout << (char)ch; flush(); }
#else
        unsigned char c;
        if (read(STDIN_FILENO, &c, 1) != 1) continue;
        if (c == '\n' || c == '\r') { std::cout << "\n"; break; }
        if (c == 3) { break; }
        if (c == 127 || c == 8) { if (!buf.empty()) { buf.pop_back(); std::cout << "\b \b"; flush(); } continue; }
        if (c >= 32 && c <= 126) { buf.push_back((char)c); std::cout << (char)c; flush(); }
#endif
    }
    hide_cursor(true);
    return buf;
}

static void wait_any_key() {
#ifdef _WIN32
    _getch();
#else
    unsigned char c; (void)read(STDIN_FILENO, &c, 1);
#endif
}

// ===== Helpers =====
static std::string lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });
    return s;
}

template <typename T>
static void print_list(const std::string& title, const std::vector<T>& v) {
    std::cout << ansi::BOLD << title << ansi::RESET << "\n";
    for (const auto& item : v) std::cout << "  - " << item << "\n";
    std::cout << "\n";
}

// ===== Catalog (replace with your real data / dynamic lists) =====
static std::vector<std::string> kNames  = { "Bulbasaur", "Charmander", "Squirtle", "Pikachu" };
static std::vector<std::string> kTypes  = { "Grass/Poison", "Fire", "Water", "Electric" };
static std::vector<int>         kIDs    = { 1, 4, 7, 25 };

// ===== Validation against catalog =====
static bool validate_name(const std::string& in) {
    std::string q = lower(in);
    for (auto& n : kNames) if (lower(n) == q) return true;
    return false;
}
static bool validate_type(const std::string& in) {
    std::string q = lower(in);
    for (auto& t : kTypes) if (lower(t) == q) return true;
    return false;
}
static bool validate_id(const std::string& in) {
    if (in.empty() || !std::all_of(in.begin(), in.end(), ::isdigit)) return false;
    int id = std::stoi(in);
    return std::find(kIDs.begin(), kIDs.end(), id) != kIDs.end();
}

static void show_error(const std::string& msg) {
    clear_screen();
    std::cout << ansi::FG_RED << msg << ansi::RESET << "\n\n";
    std::cout << ansi::DIM << "Please enter valid input. Press Enter to continue."
              << ansi::RESET << std::endl;
    flush();
    wait_any_key();
}

// ===== Menu =====
class Menu {
    std::vector<std::string> items_;
    int selected_ = 0;
public:
    explicit Menu(std::vector<std::string> items) : items_(std::move(items)) {}
    void render(const std::string& title) const {
        clear_screen();
        std::cout << ansi::BOLD << ansi::FG_CYAN << title << ansi::RESET << "\n";
        std::cout << ansi::DIM
                  << "Use Up/Down arrows to move, Enter to select, q to quit"
                  << ansi::RESET << "\n\n";

        for (int i = 0; i < (int)items_.size(); ++i) {
            bool sel = (i == selected_);
            std::cout << (sel ? "-> " : "  ");
            if (sel) std::cout << ansi::INVERSE << items_[i] << ansi::RESET << "\n";
            else     std::cout << items_[i] << "\n";
        }
        std::cout << "\n";
        flush();
    }
    bool move_up()   { if (selected_ > 0) { --selected_; return true; } return false; }
    bool move_down() { if (selected_ + 1 < (int)items_.size()) { ++selected_; return true; } return false; }
    const std::string& selected_text() const { return items_[selected_]; }
};

// ===================== BACKEND INTERFACE =====================
// Common interface both implementations must satisfy.
class Backend {
public:
    virtual ~Backend() = default;

    // Return lines to display for each query; empty => no results.
    virtual std::vector<std::string> find_by_name(const std::string& name) = 0;
    virtual std::vector<std::string> find_by_typing(const std::string& typing) = 0;
    virtual std::vector<std::string> find_by_id(int id) = 0;
};

// -------------------- Hash Map backend -----------------------
class HashMapBackend : public Backend {
    // Add your members here, e.g. references to your hash tables.
    // Example:
    // MyHashMap<std::string, Pokemon> byName;
    // MyHashMap<int, Pokemon> byId;
    // MyHashMultiMap<std::string, Pokemon> byTyping;

public:
    HashMapBackend(/* pass your structures here if needed */) {
        // optional setup
    }

    std::vector<std::string> find_by_name(const std::string& name) override {
        // implementation goes here (use your hash map keyed by name)
        // Return formatted lines to display.
        return {};
    }
    std::vector<std::string> find_by_typing(const std::string& typing) override {
        // implementation goes here (iter. bucket or secondary index on typing)
        return {};
    }
    std::vector<std::string> find_by_id(int id) override {
        // implementation goes here (hash map keyed by id)
        return {};
    }
};

// -------------------- B+ Tree backend ------------------------
class BPlusTreeBackend : public Backend {
    // Add your members here, e.g. references to your B+ tree indices.
    // Example:
    // BPlusTree<std::string, Pokemon*> byName;
    // BPlusTree<int, Pokemon*> byId;
    // BPlusTree<std::string, std::vector<Pokemon*>> byTyping;

public:
    BPlusTreeBackend(/* pass your trees here if needed */) {
        // optional setup
    }

    std::vector<std::string> find_by_name(const std::string& name) override {
        // implementation goes here (exact match or case-insensitive as you choose)
        return {};
    }
    std::vector<std::string> find_by_typing(const std::string& typing) override {
        // implementation goes here (exact match, or prefix/range if you support it)
        return {};
    }
    std::vector<std::string> find_by_id(int id) override {
        // implementation goes here (exact match by primary key)
        return {};
    }
};
// =============================================================

// ===== Query prompt with validation =====
struct Query { std::string mode; std::string value; };

static bool prompt_for_query(const std::string& banner, Query& q) {
    Menu sub({ "Name", "Typing", "ID", "Back" });
    bool dirty = true;

    while (true) {
        if (dirty) {
            sub.render(banner + " — Choose input type");
            dirty = false;
        }
        Key k = read_key();
        switch (k) {
            case Key::Up:   if (sub.move_up())   dirty = true; break;
            case Key::Down: if (sub.move_down()) dirty = true; break;
            case Key::Enter: {
                std::string choice = sub.selected_text();
                if (choice == "Back") return false;

                // Show available options (from catalog) to guide the user
                clear_screen();
                if (choice == "Name") {
                    print_list("Available Names:", kNames);
                } else if (choice == "Typing") {
                    print_list("Available Typings:", kTypes);
                } else if (choice == "ID") {
                    std::cout << ansi::BOLD << "Available IDs:" << ansi::RESET << "\n";
                    for (auto id : kIDs) std::cout << "  - " << id << "\n";
                    std::cout << "\n";
                }
                std::cout << ansi::DIM << "(Enter one of the above exactly)" << ansi::RESET << "\n";
                flush();

                std::string val = read_line_raw("Enter " + choice + ":");

                bool ok = false;
                if (choice == "Name")   ok = validate_name(val);
                if (choice == "Typing") ok = validate_type(val);
                if (choice == "ID")     ok = validate_id(val);

                if (!ok) {
                    show_error("Please enter a valid " + choice + ".");
                    dirty = true; // back to picker
                    break;
                }

                q.mode  = choice;
                q.value = val;
                return true;
            }
            case Key::Quit: return false;
            default: break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(8));
    }
}

// ===== Generic presenter for results =====
static void show_results(const std::string& banner, const std::vector<std::string>& lines) {
    clear_screen();
    std::cout << ansi::BOLD << banner << ansi::RESET << "\n\n";
    if (lines.empty()) {
        std::cout << ansi::DIM << "(No results returned — backend not implemented)"
                  << ansi::RESET << "\n\n";
    } else {
        for (const auto& s : lines) std::cout << "• " << s << "\n";
        std::cout << "\n";
    }
    std::cout << ansi::DIM << "(Press any key to return)" << ansi::RESET << std::endl;
    flush();
    wait_any_key();
}

// ===== Mode UIs: select backend and query =====
static void run_mode_ui(const std::string& banner, Backend& backend) {
    Query q;
    if (!prompt_for_query(banner, q)) return;

    std::vector<std::string> out;
    if      (q.mode == "Name")   out = backend.find_by_name(q.value);
    else if (q.mode == "Typing") out = backend.find_by_typing(q.value);
    else if (q.mode == "ID")     out = backend.find_by_id(std::stoi(q.value));

    show_results(banner + " Results", out);
}

static void run_hash_map_ui() {
    HashMapBackend hm;                // TODO: pass your real tables into this ctor
    run_mode_ui("[Hash Map Mode]", hm);
}

static void run_bplus_tree_ui() {
    BPlusTreeBackend bpt;             // TODO: pass your real trees into this ctor
    run_mode_ui("[B+ Tree Mode]", bpt);
}

// ===== Main =====
int main() {
#ifdef _WIN32
    enable_vt_mode();
#else
    RawTerm raw_guard;
#endif
    hide_cursor(true);

    Menu menu({ "Hash Map", "B+ Tree", "Exit" });
    bool running = true, dirty = true;

    while (running) {
        if (dirty) {
            menu.render("== Data Structure Selector ==");
            dirty = false;
        }

        Key k = read_key();
        switch (k) {
            case Key::Up:    if (menu.move_up())   dirty = true; break;
            case Key::Down:  if (menu.move_down()) dirty = true; break;
            case Key::Enter: {
                const auto& choice = menu.selected_text();
                if      (choice == "Hash Map") run_hash_map_ui();
                else if (choice == "B+ Tree")  run_bplus_tree_ui();
                else if (choice == "Exit")     running = false;
                dirty = true;
                break;
            }
            case Key::Quit: running = false; break;
            default: break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(8));
    }

    clear_screen();
    hide_cursor(false);
    std::cout << ansi::FG_YELLOW << "Goodbye!" << ansi::RESET << "\n";
    flush();
    return 0;
}