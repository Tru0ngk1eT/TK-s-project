#include "splashkit.h"
#include <fstream>
#include <string>
#include <filesystem>
#include <algorithm>
#include <cctype>

using std::ifstream;
using std::ofstream;
using std::string;
using std::to_string;

const int NUM_SUBJECTS = 4;
const int MAX_VALID_UNITS = 8;

// ==========================
//       Data structures
// ==========================
struct Subject
{
    string code;
    int grade;
};
struct StudentRecord
{
    string name;
    string bachelor;
    Subject math[NUM_SUBJECTS];
    Subject data[NUM_SUBJECTS];
    Subject programming[NUM_SUBJECTS];
};
struct UserAccount
{
    string username;
    string password;
    StudentRecord student;
};
struct Button
{
    int x, y, w, h;
    string label;
};

enum class Screen
{
    SIGN_IN,
    SIGN_UP,
    MAIN_MENU,
    ENTER_SUBJECTS /* add more if needed */
};
enum class Focus
{
    NONE,
    USERNAME,
    PASSWORD
};

// Valid units (lowercase for case-insensitive check)
string valid_math_units[MAX_VALID_UNITS] = {"sit192", "sit194", "sit176", "sit291", "sit292", "sit221", "sit396", "sit399"};
string valid_data_units[MAX_VALID_UNITS] = {"sit112", "sit103", "sit221", "sit220", "sit215", "sit307", "sit319", "sit330"};
string valid_programming_units[MAX_VALID_UNITS] = {"sit102", "sit111", "sit182", "sit232", "sit202", "sit218", "sit332", "sit324"};

// ==========================
//        Input helpers
// ==========================

// Interact with keyboard
char get_typed_char()
{
    if (key_typed(A_KEY))
        return 'a';
    if (key_typed(B_KEY))
        return 'b';
    if (key_typed(C_KEY))
        return 'c';
    if (key_typed(D_KEY))
        return 'd';
    if (key_typed(E_KEY))
        return 'e';
    if (key_typed(F_KEY))
        return 'f';
    if (key_typed(G_KEY))
        return 'g';
    if (key_typed(H_KEY))
        return 'h';
    if (key_typed(I_KEY))
        return 'i';
    if (key_typed(J_KEY))
        return 'j';
    if (key_typed(K_KEY))
        return 'k';
    if (key_typed(L_KEY))
        return 'l';
    if (key_typed(M_KEY))
        return 'm';
    if (key_typed(N_KEY))
        return 'n';
    if (key_typed(O_KEY))
        return 'o';
    if (key_typed(P_KEY))
        return 'p';
    if (key_typed(Q_KEY))
        return 'q';
    if (key_typed(R_KEY))
        return 'r';
    if (key_typed(S_KEY))
        return 's';
    if (key_typed(T_KEY))
        return 't';
    if (key_typed(U_KEY))
        return 'u';
    if (key_typed(V_KEY))
        return 'v';
    if (key_typed(W_KEY))
        return 'w';
    if (key_typed(X_KEY))
        return 'x';
    if (key_typed(Y_KEY))
        return 'y';
    if (key_typed(Z_KEY))
        return 'z';

    if (key_typed(NUM_0_KEY))
        return '0';
    if (key_typed(NUM_1_KEY))
        return '1';
    if (key_typed(NUM_2_KEY))
        return '2';
    if (key_typed(NUM_3_KEY))
        return '3';
    if (key_typed(NUM_4_KEY))
        return '4';
    if (key_typed(NUM_5_KEY))
        return '5';
    if (key_typed(NUM_6_KEY))
        return '6';
    if (key_typed(NUM_7_KEY))
        return '7';
    if (key_typed(NUM_8_KEY))
        return '8';
    if (key_typed(NUM_9_KEY))
        return '9';

    if (key_typed(SPACE_KEY))
        return ' ';
    return 0;
}

// Down line
void draw_multiline_text(const string &text, color col, int x, int y, int line_h = 22)
{
    size_t start = 0;
    int line = 0;
    while (start <= text.size())
    {
        size_t nl = text.find('\n', start);
        string line_str = (nl == string::npos) ? text.substr(start)
                                               : text.substr(start, nl - start);
        draw_text(line_str, col, x, y + line * line_h);
        if (nl == string::npos)
            break;
        start = nl + 1;
        ++line;
    }
}

// Remove extra spaces at the beginning and end of the string
string trim(const string &s)
{
    auto st = s.begin();
    while (st != s.end() && isspace(*st))
        ++st;
    auto ed = s.end();
    do
    {
        --ed;
    } while (std::distance(st, ed) > 0 && isspace(*ed));
    return string(st, ed + 1);
}

// Lowercase
string to_lower(const string &s)
{
    string res = s;
    transform(res.begin(), res.end(), res.begin(), ::tolower);
    return res;
}

// ===========================
//     Simple GUI wrappers
// ===========================

// Simple text
void gui_message_box(const string &title, const string &message)
{
    clear_screen(COLOR_WHITE);
    draw_text(title, COLOR_BLACK, 50, 50);
    draw_multiline_text(message, COLOR_BLACK, 50, 100, 22);
    refresh_screen();
    while (true)
    {
        process_events();
        if (key_typed(RETURN_KEY))
            break;
        delay(10);
    }
}

// Create box
string gui_input_textbox(const string &prompt, int x, int y, int w, int h)
{
    string input;
    bool finished = false;
    while (!finished)
    {
        process_events();
        clear_screen(COLOR_WHITE);

        fill_rectangle(COLOR_LIGHT_BLUE, x, y, w, h);
        draw_rectangle(COLOR_BLACK, x, y, w, h);
        draw_text(prompt + input + "|", COLOR_BLACK, x + 5, y + h / 4);
        refresh_screen();

        for (int k = 0; k < 256; ++k)
        {
            if (key_typed((key_code)k))
            {
                if (k == RETURN_KEY)
                {
                    finished = true;
                    break;
                }
                if (k == BACKSPACE_KEY)
                {
                    if (!input.empty())
                        input.pop_back();
                }
                else
                {
                    if (char c = get_typed_char())
                        input += c;
                }
            }
        }
    }
    return input;
}

// Text inside box
string gui_input_text(const string &prompt, int x, int y)
{
    string input;
    bool finished = false;
    while (!finished)
    {
        process_events();
        clear_screen(COLOR_WHITE);
        draw_text(prompt + input + "|", COLOR_BLACK, x, y);
        refresh_screen();

        for (int k = 0; k < 256; ++k)
        {
            if (key_typed((key_code)k))
            {
                if (k == RETURN_KEY)
                {
                    finished = true;
                    break;
                }
                if (k == BACKSPACE_KEY)
                {
                    if (!input.empty())
                        input.pop_back();
                }
                else
                {
                    if (char c = get_typed_char())
                        input += c;
                }
            }
        }
    }
    return input;
}

// Read string
string gui_read_string_nonempty(const string &prompt)
{
    while (true)
    {
        string v = gui_input_text(prompt, 220, 220);
        if (!v.empty())
            return v;
        gui_message_box("Input required", "Please enter a value.");
    }
}

// Read integer
int gui_read_integer(const string &prompt)
{
    while (true)
    {
        string s = gui_input_text(prompt + " (enter a number)", 220, 220);
        try
        {
            return std::stoi(s);
        }
        catch (...)
        {
            gui_message_box("Invalid", "Please enter a valid integer.");
        }
    }
}

// ==========================
//        Program Logic
// ==========================

// Pick category
void category_pick(const string &category, const string *&valid_units, int &count)
{
    if (category == "Math")
    {
        valid_units = valid_math_units;
        count = MAX_VALID_UNITS;
    }
    else if (category == "Data")
    {
        valid_units = valid_data_units;
        count = MAX_VALID_UNITS;
    }
    else if (category == "Programming")
    {
        valid_units = valid_programming_units;
        count = MAX_VALID_UNITS;
    }
    else
    {
        valid_units = nullptr;
        count = 0;
    }
}

// Check if unit valid with each category
bool is_valid_unit(const string &unit_code, const string &category)
{
    const string *valid_units;
    int count;
    category_pick(category, valid_units, count);
    string code = to_lower(trim(unit_code));
    for (int i = 0; i < count; ++i)
        if (valid_units[i] == code)
            return true;
    return false;
}

// List valid units vertically
void show_valid_units_dialog(const string &category)
{
    const string *valid_units;
    int count;
    category_pick(category, valid_units, count);
    string list = "Unit list for " + category + ":\n";
    for (int i = 0; i < count; ++i)
        list += to_string(i + 1) + ". " + valid_units[i] + "\n";
    gui_message_box("Valid units", list);
}

// Check if a unit is already picked
bool is_unit_already_used(const StudentRecord &student, const string &unit_code)
{
    string code = to_lower(unit_code);
    for (int i = 0; i < NUM_SUBJECTS; ++i)
        if (to_lower(student.math[i].code) == code)
            return true;
    for (int i = 0; i < NUM_SUBJECTS; ++i)
        if (to_lower(student.data[i].code) == code)
            return true;
    for (int i = 0; i < NUM_SUBJECTS; ++i)
        if (to_lower(student.programming[i].code) == code)
            return true;
    return false;
}

// Take input of unit code and grade
void gui_input_subject(StudentRecord &student, Subject &subj, const string &category, int number)
{
    const string *valid_units;
    int count;
    category_pick(category, valid_units, count);
    show_valid_units_dialog(category);

    // Unit code
    bool valid_code = false;
    while (!valid_code)
    {
        string unit_code = trim(gui_read_string_nonempty("Course code for " + category + " #" + to_string(number)));
        string unit_code_upper = unit_code;
        for (auto &c : unit_code_upper)
            c = toupper((unsigned char)c);

        if (is_valid_unit(unit_code, category))
        {
            if (!is_unit_already_used(student, unit_code))
            {
                subj.code = unit_code_upper;
                valid_code = true;
            }
            else
                gui_message_box("Duplicate unit", "Unit already used. Choose another one.");
        }
        else
            gui_message_box("Invalid unit", "Please choose a unit from the list.");
    }

    // Unit grade
    bool valid_grade = false;
    while (!valid_grade)
    {
        string grade_str = gui_read_string_nonempty("Grade for " + subj.code + " (0-4)");
        try
        {
            subj.grade = stoi(grade_str);
            if (subj.grade >= 0 && subj.grade <= 4)
                valid_grade = true;
            else
                gui_message_box("Invalid grade", "Grade must be 0-4.");
        }
        catch (...)
        {
            gui_message_box("Invalid grade", "Enter a valid number.");
        }
    }
}

// Calculate grade
void total_grade(const StudentRecord &student, int &math_total, int &data_total, int &prog_total)
{
    math_total = data_total = prog_total = 0;
    for (int i = 0; i < NUM_SUBJECTS; ++i)
        math_total += student.math[i].grade;
    for (int i = 0; i < NUM_SUBJECTS; ++i)
        data_total += student.data[i].grade;
    for (int i = 0; i < NUM_SUBJECTS; ++i)
        prog_total += student.programming[i].grade;
}

// Suggest job
void gui_suggest_job(int math_total, int data_total, int prog_total)
{
    string out;

    // Data
    if (data_total <= 4)
        out += "Your data skills are just acceptable, try to improve them.\n";
    if (data_total >= 5)
        out += "- Junior Data Entry Clerk\n";
    if (data_total >= 7)
        out += "- Business Intelligence Assistant\n";
    if (data_total >= 11)
        out += "- Statistician / Research Analyst\n";
    if (data_total >= 13)
        out += "- Machine Learning Engineer\n";
    if (data_total >= 15)
        out += "- Data Engineer\n";
    if (data_total >= 16)
        out += "- Data Scientist\n";

    // Math
    if (math_total <= 4)
        out += "Your math skills are just acceptable, try to improve them.\n";
    if (math_total >= 5)
        out += "- Math Tutor / Teaching Assistant\n";
    if (math_total >= 7)
        out += "- Actuarial Assistant\n";
    if (math_total >= 9)
        out += "- Operations Research Analyst\n";
    if (math_total >= 11)
        out += "- Statistician\n";
    if (math_total >= 13)
        out += "- Quantitative Analyst\n";
    if (math_total >= 15)
        out += "- Theoretical Mathematician / Researcher\n";
    if (math_total >= 16)
        out += "- Data Scientist / AI Researcher\n";

    // Programming
    if (prog_total <= 4)
        out += "Your programming skills are just acceptable, try to improve them.\n";
    if (prog_total >= 5)
        out += "- IT Support Assistant\n";
    if (prog_total >= 7)
        out += "- Web Developer\n";
    if (prog_total >= 9)
        out += "- Mobile App Developer\n";
    if (prog_total >= 11)
        out += "- Software Developer\n";
    if (prog_total >= 13)
        out += "- Game Developer\n";
    if (prog_total >= 15)
        out += "- Software Engineer\n";
    if (prog_total >= 16)
        out += "- AI / Machine Learning Engineer\n";

    if (data_total <= 4 && math_total <= 4 && prog_total <= 4)
        out += "Focus on improving your grades.\n";
    if (data_total == 16 && math_total == 16 && prog_total == 16)
        out += "Your performance is outstanding!\n";

    gui_message_box("Job Suggestions", out);
}

// ==========================
// File / Auth helpers
// ==========================

// Check if mouse click the button
bool mouse_in_button(const Button &btn)
{
    return mouse_x() >= btn.x && mouse_x() <= btn.x + btn.w &&
           mouse_y() >= btn.y && mouse_y() <= btn.y + btn.h;
}

// Create user file
void ensure_users_dir()
{
    std::filesystem::path dir = "users";
    if (!std::filesystem::exists(dir))
    {
        std::error_code ec;
        std::filesystem::create_directory(dir, ec);
    }
}

// Turn password to hash (not save the original password)
string hash_password_simple(const string &pw)
{
    std::hash<string> hasher;
    size_t h = hasher(pw);
    std::ostringstream oss;
    oss << std::hex << h;
    return oss.str();
}

// Move Username to a save file
string sanitize_filename(const string &name)
{
    string out;
    for (unsigned char c : name)
    {
        if (std::isalnum(c) || c == '-' || c == '_')
            out.push_back(static_cast<char>(c));
        else
            out.push_back('_');
    }
    if (out.empty())
        out = "user";
    return out;
}

// Save hash in Users
bool signup_nonblocking(const UserAccount &user)
{
    ensure_users_dir();
    if (user.username.empty() || user.password.empty())
        return false;

    string filename = "users/" + sanitize_filename(user.username) + ".txt";
    std::ifstream infile(filename, std::ios::binary);
    if (infile.good())
    {
        infile.close();
        return false;
    } // existed

    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile.good())
        return false;
    outfile << hash_password_simple(user.password);
    return true;
}

// Login - Compare hash
bool login_nonblocking(const UserAccount &user)
{
    if (user.username.empty() || user.password.empty())
        return false;

    string filename = "users/" + sanitize_filename(user.username) + ".txt";
    std::ifstream infile(filename, std::ios::binary);
    if (!infile.good())
        return false;

    string stored_hash;
    infile >> stored_hash;
    return stored_hash == hash_password_simple(user.password);
}

// ================
//       Main
// ================
int main()
{
    open_window("Student Portal GUI", 1280, 720);

    string username, password;
    Button sign_in{440, 300, 190, 50, "Sign In"},
        sign_up{650, 300, 190, 50, "Sign Up"},
        back_btn{50, 50, 100, 40, "Back"},
        logout_btn{1100, 50, 100, 40, "Logout"},
        create_btn{540, 300, 200, 50, "Create Account"};

    Screen current = Screen::SIGN_IN;
    Focus focus = Focus::NONE;
    bool done = false;

    while (!done)
    {
        process_events();
        if (quit_requested() || key_typed(ESCAPE_KEY))
            break;

        // Mouse
        if (mouse_clicked(LEFT_BUTTON))
        {
            // Set focus to textbox - SIGN IN/UP screens
            if (current == Screen::SIGN_IN || current == Screen::SIGN_UP)
            {
                if (mouse_x() >= 440 && mouse_x() <= 840 && mouse_y() >= 170 && mouse_y() <= 210)
                    focus = Focus::USERNAME;
                else if (mouse_x() >= 440 && mouse_x() <= 840 && mouse_y() >= 230 && mouse_y() <= 270)
                    focus = Focus::PASSWORD;
                else
                    focus = Focus::NONE;
            }

            // Sign in screen
            if (current == Screen::SIGN_IN)
            {
                if (mouse_in_button(sign_up))
                {
                    current = Screen::SIGN_UP;
                    focus = Focus::NONE;
                }
                if (mouse_in_button(sign_in))
                {
                    UserAccount u{username, password};
                    if (login_nonblocking(u))
                    {
                        gui_message_box("Login", "Login successful!");
                        current = Screen::MAIN_MENU;
                        focus = Focus::NONE;
                    }
                    else
                        gui_message_box("Login", "Login failed!");
                }
            }

            // Sign up screen
            else if (current == Screen::SIGN_UP)
            {
                if (mouse_in_button(back_btn))
                {
                    current = Screen::SIGN_IN;
                    focus = Focus::NONE;
                }
                if (mouse_in_button(create_btn))
                {
                    UserAccount u{username, password};
                    if (signup_nonblocking(u))
                    {
                        gui_message_box("Signup", "Signup successful! Please login.");
                        current = Screen::SIGN_IN;
                        password.clear();
                        focus = Focus::NONE;
                    }
                    else
                    {
                        gui_message_box("Signup", "Signup failed (exists or invalid).");
                        current = Screen::SIGN_IN;
                        focus = Focus::NONE;
                    }
                }
            }

            // Main menu screen
            else if (current == Screen::MAIN_MENU)
            {
                if (mouse_in_button(logout_btn))
                {
                    username.clear();
                    password.clear();
                    current = Screen::SIGN_IN;
                    focus = Focus::NONE;
                }
                // Enter subjects
                if (mouse_x() >= 440 && mouse_x() <= 840 && mouse_y() >= 330 && mouse_y() <= 380)
                {
                    current = Screen::ENTER_SUBJECTS;
                    focus = Focus::NONE;
                }
            }

            // Enter subjects screen
            else if (current == Screen::ENTER_SUBJECTS)
            {
                if (mouse_in_button(back_btn))
                {
                    current = Screen::MAIN_MENU;
                    focus = Focus::NONE;
                }
                else
                {
                    StudentRecord student;

                    // Math
                    gui_message_box("Section", "=== MATH ===");
                    for (int i = 0; i < NUM_SUBJECTS; ++i)
                        gui_input_subject(student, student.math[i], "Math", i + 1);

                    // Data
                    gui_message_box("Section", "=== DATA ===");
                    for (int i = 0; i < NUM_SUBJECTS; ++i)
                        gui_input_subject(student, student.data[i], "Data", i + 1);

                    // Programming
                    gui_message_box("Section", "=== PROGRAMMING ===");
                    for (int i = 0; i < NUM_SUBJECTS; ++i)
                        gui_input_subject(student, student.programming[i], "Programming", i + 1);

                    // Summary + Suggestion
                    int mt, dt, pt;
                    total_grade(student, mt, dt, pt);
                    string summary = "=== Job Suggestion ===\n";
                    summary += "Student: " + username + "\n";
                    gui_message_box("Summary", summary);
                    gui_suggest_job(mt, dt, pt);
                    gui_message_box("Info", "Hope this will help you!");
                }
            }
        }

        // Keyboard
        if (focus != Focus::NONE)
        {
            if (key_typed(BACKSPACE_KEY))
            {
                if (focus == Focus::USERNAME && !username.empty())
                    username.pop_back();
                if (focus == Focus::PASSWORD && !password.empty())
                    password.pop_back();
            }

            // Enter to login in Sign in screen
            if (key_typed(RETURN_KEY) && focus == Focus::PASSWORD && current == Screen::SIGN_IN)
            {
                UserAccount u{username, password};
                if (login_nonblocking(u))
                {
                    gui_message_box("Login", "Login successful!");
                    current = Screen::MAIN_MENU;
                    focus = Focus::NONE;
                }
                else
                {
                    gui_message_box("Login", "Login failed!");
                    current = Screen::SIGN_IN;
                    focus = Focus::NONE;
                }
            }

            // type lowercase key
            for (int k = 32; k < 127; ++k)
            {
                if (key_typed(static_cast<key_code>(k)))
                {
                    char c = static_cast<char>(k);
                    if (focus == Focus::USERNAME)
                        username.push_back(c);
                    else if (focus == Focus::PASSWORD)
                        password.push_back(c);
                }
            }
        }

        // Render
        clear_screen(COLOR_WHITE);

        // Sign in screen
        if (current == Screen::SIGN_IN)
        {
            draw_text("Welcome - Sign In", COLOR_BLACK, 540, 100, option_to_screen());

            fill_rectangle(COLOR_LIGHT_GRAY, 440, 170, 400, 40); // Tăng chiều cao ô
            draw_rectangle(COLOR_BLACK, 440, 170, 400, 40);
            string uname_display = username.empty() && focus != Focus::USERNAME ? "(click to type)" : username;
            draw_text("Username: " + uname_display, COLOR_BLACK, 450, 180, option_to_screen());

            fill_rectangle(COLOR_LIGHT_GRAY, 440, 230, 400, 40); // Tăng chiều cao ô
            draw_rectangle(COLOR_BLACK, 440, 230, 400, 40);
            string mask(password.length(), '*');
            if (mask.empty() && focus != Focus::PASSWORD)
                draw_text("Password: (click to type)", COLOR_BLACK, 450, 240, option_to_screen());
            else
                draw_text("Password: " + mask, COLOR_BLACK, 450, 240, option_to_screen());

            if (focus == Focus::USERNAME)
                draw_rectangle(COLOR_BLUE, 439, 169, 402, 42);
            else if (focus == Focus::PASSWORD)
                draw_rectangle(COLOR_BLUE, 439, 229, 402, 42);

            fill_rectangle(COLOR_LIGHT_BLUE, 440, 300, 190, 50); // Tăng chiều cao nút
            draw_rectangle(COLOR_BLACK, 440, 300, 190, 50);
            draw_text(sign_in.label, COLOR_BLACK, 515, 315, option_to_screen());

            fill_rectangle(COLOR_LIGHT_BLUE, 650, 300, 190, 50); // Tăng chiều cao nút
            draw_rectangle(COLOR_BLACK, 650, 300, 190, 50);
            draw_text(sign_up.label, COLOR_BLACK, 725, 315, option_to_screen());
        }

        // Sign up screen
        else if (current == Screen::SIGN_UP)
        {
            draw_text("Create Account - Sign Up", COLOR_BLACK, 540, 100, option_to_screen());

            fill_rectangle(COLOR_LIGHT_GRAY, 440, 170, 400, 40); // Tăng chiều cao ô
            draw_rectangle(COLOR_BLACK, 440, 170, 400, 40);
            string uname_display = username.empty() && focus != Focus::USERNAME ? "(click to type)" : username;
            draw_text("Username: " + uname_display, COLOR_BLACK, 450, 180, option_to_screen());

            fill_rectangle(COLOR_LIGHT_GRAY, 440, 230, 400, 40); // Tăng chiều cao ô
            draw_rectangle(COLOR_BLACK, 440, 230, 400, 40);
            string mask_pw(password.length(), '*');
            if (mask_pw.empty() && focus != Focus::PASSWORD)
                draw_text("Password: (click to type)", COLOR_BLACK, 450, 240, option_to_screen());
            else
                draw_text("Password: " + mask_pw, COLOR_BLACK, 450, 240, option_to_screen());

            if (focus == Focus::USERNAME)
                draw_rectangle(COLOR_BLUE, 439, 169, 402, 42);
            else if (focus == Focus::PASSWORD)
                draw_rectangle(COLOR_BLUE, 439, 229, 402, 42);

            fill_rectangle(COLOR_LIGHT_BLUE, back_btn.x, back_btn.y, back_btn.w, back_btn.h);
            draw_rectangle(COLOR_BLACK, back_btn.x, back_btn.y, back_btn.w, back_btn.h);
            draw_text(back_btn.label, COLOR_BLACK, back_btn.x + 15, back_btn.y + 12, option_to_screen());

            fill_rectangle(COLOR_LIGHT_BLUE, create_btn.x, create_btn.y, create_btn.w, create_btn.h);
            draw_rectangle(COLOR_BLACK, create_btn.x, create_btn.y, create_btn.w, create_btn.h);
            draw_text(create_btn.label, COLOR_BLACK, create_btn.x + 20, create_btn.y + 15, option_to_screen());
        }

        // Main menu screen
        else if (current == Screen::MAIN_MENU)
        {
            draw_text("Main Menu", COLOR_BLACK, 540, 100, option_to_screen());
            fill_rectangle(COLOR_LIGHT_BLUE, 440, 330, 400, 50);
            draw_rectangle(COLOR_BLACK, 440, 330, 400, 50);
            draw_text("1. Enter subjects & get job suggestion", COLOR_BLACK, 460, 345, option_to_screen());

            fill_rectangle(COLOR_LIGHT_BLUE, logout_btn.x, logout_btn.y, logout_btn.w, logout_btn.h);
            draw_rectangle(COLOR_BLACK, logout_btn.x, logout_btn.y, logout_btn.w, logout_btn.h);
            draw_text(logout_btn.label, COLOR_BLACK, logout_btn.x + 15, logout_btn.y + 12, option_to_screen());
        }

        // Enter subjects screen
        else if (current == Screen::ENTER_SUBJECTS)
        {
            draw_text("Enter student info and subjects (click to start)", COLOR_BLACK, 440, 100, option_to_screen());
            fill_rectangle(COLOR_LIGHT_BLUE, back_btn.x, back_btn.y, back_btn.w, back_btn.h);
            draw_rectangle(COLOR_BLACK, back_btn.x, back_btn.y, back_btn.w, back_btn.h);
            draw_text(back_btn.label, COLOR_BLACK, back_btn.x + 15, back_btn.y + 12, option_to_screen());
        }

        refresh_screen();
    }

    close_window("Student Portal GUI");
    return 0;
}