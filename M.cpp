#include "splashkit.h"
#include "utilities.h"
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
// Data models
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
// Input helpers
// ==========================

/** Trả về ký tự người dùng vừa gõ (A–Z, 0–9, SPACE); 0 nếu không có.
 *  Non-blocking: chỉ kiểm tra edge "key_typed" của SplashKit. */
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

/** Vẽ chuỗi có chứa '\n' thành nhiều dòng (đơn giản). */
void draw_multiline_text(const string &text, color col, int x, int y, int line_h = 22)
{
    size_t start = 0;
    int line = 0;
    while (start <= text.size())
    {
        size_t nl = text.find('\n', start);
        string line_str = (nl == string::npos) ? text.substr(start)
                                               : text.substr(start, nl - start);
        draw_text(line_str, col, x, y + line * line_h, option_to_screen());
        if (nl == string::npos)
            break;
        start = nl + 1;
        ++line;
    }
}

/** Cắt trắng đầu/cuối (ASCII spaces). */
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

/** Trả về bản chữ thường (lowercase). */
string to_lower(const string &s)
{
    string res = s;
    transform(res.begin(), res.end(), res.begin(), ::tolower);
    return res;
}

// ==========================
// Simple GUI wrappers
// ==========================

/** Hộp thoại thông báo đơn giản (nhấn Enter để đóng). */
void gui_message_box(const string &title, const string &message)
{
    clear_screen(COLOR_WHITE);
    draw_text(title, COLOR_BLACK, 50, 50, option_to_screen());
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

/** Ô nhập trong khung (blocking cho tới khi Enter). */
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
        draw_text(prompt + input + "|", COLOR_BLACK, x + 5, y + h / 4, option_to_screen());
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

/** Nhập chuỗi 1 dòng ở vị trí (x,y); Enter để kết thúc. */
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

/** Đọc chuỗi không rỗng (lặp lại tới khi hợp lệ). */
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

/** Đọc số nguyên, lặp lại nếu không hợp lệ. */
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
// Business logic
// ==========================

/** Lấy mảng valid_units và số lượng theo category. */
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

/** Kiểm tra unit_code có thuộc danh sách của category (không phân biệt hoa/thường). */
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

/** Hiển thị danh sách unit hợp lệ theo cột dọc. */
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

/** Kiểm tra một unit đã được dùng ở bất kỳ nhóm nào hay chưa (case-insensitive). */
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

/** Nhập một môn học (unit + grade) cho category.
 *  - Hiện danh sách unit một lần
 *  - Yêu cầu unit hợp lệ & chưa trùng
 *  - Grade 0–4 (điểm từng môn)
 */
void gui_input_subject(StudentRecord &student, Subject &subj, const string &category, int number)
{
    const string *valid_units;
    int count;
    category_pick(category, valid_units, count);
    if (!valid_units || count == 0)
    {
        gui_message_box("Error", "No valid units for " + category);
        return;
    }

    show_valid_units_dialog(category);

    // Nhập unit code
    bool valid_code = false;
    while (!valid_code)
    {
        string unit_code = trim(gui_read_string_nonempty("Course code for " + category + " #" + to_string(number)));

        // Lưu hiển thị in hoa cho đẹp; kiểm tra không phân biệt hoa/thường
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

    // Nhập grade 0–4
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

/** Tính tổng điểm từng nhóm (0–16 vì mỗi nhóm 4 môn * tối đa 4 điểm). */
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

/** Gợi ý nghề dựa trên tổng điểm (rule-based, hiển thị trong message box). */
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

/** Kiểm tra con trỏ chuột có nằm trong Button hay không. */
bool mouse_in_button(const Button &btn)
{
    return mouse_x() >= btn.x && mouse_x() <= btn.x + btn.w &&
           mouse_y() >= btn.y && mouse_y() <= btn.y + btn.h;
}

/** Tạo thư mục "users" nếu chưa có (dùng cho non-blocking auth). */
void ensure_users_dir()
{
    std::filesystem::path dir = "users";
    if (!std::filesystem::exists(dir))
    {
        std::error_code ec;
        std::filesystem::create_directory(dir, ec);
    }
}

/** Băm password kiểu đơn giản (không dùng cho production). */
string hash_password_simple(const string &pw)
{
    std::hash<string> hasher;
    size_t h = hasher(pw);
    std::ostringstream oss;
    oss << std::hex << h;
    return oss.str();
}

/** Chuyển username sang tên file an toàn. */
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
/** Signup (non-blocking): lưu hash ở thư mục users/. */
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

/** Login (non-blocking): so sánh hash. */
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

// ==========================
// Main
// ==========================
int main()
{
    open_window("Student Portal GUI", 1280, 720);

    string username, password;                    // shared inputs
    Button sign_in{200, 250, 100, 40, "Sign In"}, // UI buttons
        sign_up{350, 250, 100, 40, "Sign Up"},
        back_btn{20, 300, 80, 30, "Back"},
        logout_btn{540, 10, 80, 30, "Logout"},
        create_btn{300, 250, 120, 40, "Create"};

    Screen current = Screen::SIGN_IN;
    Focus focus = Focus::NONE;
    bool done = false;

    while (!done)
    {
        process_events();
        if (quit_requested() || key_typed(ESCAPE_KEY))
            break;

        // ---------- Mouse ----------
        if (mouse_clicked(LEFT_BUTTON))
        {
            // set focus vùng textbox
            if (mouse_x() >= 200 && mouse_x() <= 440 && mouse_y() >= 100 && mouse_y() <= 130)
                focus = Focus::USERNAME;
            else if (mouse_x() >= 200 && mouse_x() <= 440 && mouse_y() >= 150 && mouse_y() <= 180)
                focus = Focus::PASSWORD;
            else
                focus = Focus::NONE;

            // theo màn hình
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
            else if (current == Screen::MAIN_MENU)
            {
                if (mouse_in_button(logout_btn))
                {
                    username.clear();
                    password.clear();
                    current = Screen::SIGN_IN;
                    focus = Focus::NONE;
                }
                // chọn mục "Enter subjects..."
                if (mouse_x() >= 200 && mouse_x() <= 440 && mouse_y() >= 100 && mouse_y() <= 140)
                {
                    current = Screen::ENTER_SUBJECTS;
                    focus = Focus::NONE;
                }
            }
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
                    summary += "Student: " + student.name + "\n";
                    summary += "Bachelor: " + student.bachelor + "\n";
                    gui_message_box("Summary", summary);
                    gui_suggest_job(mt, dt, pt);
                    gui_message_box("Info", "Hope this will help you!");
                }
            }
        }

        // ---------- Keyboard (textbox) ----------
        if (focus != Focus::NONE)
        {
            if (key_typed(BACKSPACE_KEY))
            {
                if (focus == Focus::USERNAME && !username.empty())
                    username.pop_back();
                if (focus == Focus::PASSWORD && !password.empty())
                    password.pop_back();
            }

            // Enter để login khi đang ở Sign In (focus password)
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

            // nhập ký tự thường
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

        // ---------- Render ----------
        clear_screen(COLOR_WHITE);

        if (current == Screen::SIGN_IN)
        {
            draw_text("Welcome - Sign In", COLOR_BLACK, 220, 40);
            fill_rectangle(COLOR_LIGHT_GRAY, 200, 100, 240, 30);
            draw_rectangle(COLOR_BLACK, 200, 100, 240, 30);
            string uname_display = username.empty() && focus != Focus::USERNAME ? "(click to type)" : username;
            draw_text("Username: " + uname_display, COLOR_BLACK, 205, 105);

            fill_rectangle(COLOR_LIGHT_GRAY, 200, 150, 240, 30);
            draw_rectangle(COLOR_BLACK, 200, 150, 240, 30);
            string mask(password.length(), '*');
            if (mask.empty() && focus != Focus::PASSWORD)
                draw_text("Password: (click to type)", COLOR_BLACK, 205, 155);
            else
                draw_text("Password: " + mask, COLOR_BLACK, 205, 155);

            if (focus == Focus::USERNAME)
                draw_rectangle(COLOR_BLUE, 199, 99, 242, 32);
            else if (focus == Focus::PASSWORD)
                draw_rectangle(COLOR_BLUE, 199, 149, 242, 32);

            fill_rectangle(COLOR_LIGHT_BLUE, sign_in.x, sign_in.y, sign_in.w, sign_in.h);
            draw_rectangle(COLOR_BLACK, sign_in.x, sign_in.y, sign_in.w, sign_in.h);
            draw_text(sign_in.label, COLOR_BLACK, sign_in.x + 10, sign_in.y + 12);

            fill_rectangle(COLOR_LIGHT_BLUE, sign_up.x, sign_up.y, sign_up.w, sign_up.h);
            draw_rectangle(COLOR_BLACK, sign_up.x, sign_up.y, sign_up.w, sign_up.h);
            draw_text(sign_up.label, COLOR_BLACK, sign_up.x + 10, sign_up.y + 12);
        }
        else if (current == Screen::SIGN_UP)
        {
            draw_text("Create Account - Sign Up", COLOR_BLACK, 200, 40);

            fill_rectangle(COLOR_LIGHT_GRAY, 200, 100, 240, 30);
            draw_rectangle(COLOR_BLACK, 200, 100, 240, 30);
            string uname_display = username.empty() && focus != Focus::USERNAME ? "(click to type)" : username;
            draw_text("Username: " + uname_display, COLOR_BLACK, 205, 105);

            fill_rectangle(COLOR_LIGHT_GRAY, 200, 150, 240, 30);
            draw_rectangle(COLOR_BLACK, 200, 150, 240, 30);
            string mask_pw(password.length(), '*');
            if (mask_pw.empty() && focus != Focus::PASSWORD)
                draw_text("Password: (click to type)", COLOR_BLACK, 205, 155);
            else
                draw_text("Password: " + mask_pw, COLOR_BLACK, 205, 155);

            fill_rectangle(COLOR_LIGHT_BLUE, back_btn.x, back_btn.y, back_btn.w, back_btn.h);
            draw_rectangle(COLOR_BLACK, back_btn.x, back_btn.y, back_btn.w, back_btn.h);
            draw_text(back_btn.label, COLOR_BLACK, back_btn.x + 8, back_btn.y + 8);

            fill_rectangle(COLOR_LIGHT_BLUE, create_btn.x, create_btn.y, create_btn.w, create_btn.h);
            draw_rectangle(COLOR_BLACK, create_btn.x, create_btn.y, create_btn.w, create_btn.h);
            draw_text(create_btn.label, COLOR_BLACK, create_btn.x + 10, create_btn.y + 12);

            if (focus == Focus::USERNAME)
                draw_rectangle(COLOR_BLUE, 199, 99, 242, 32);
            else if (focus == Focus::PASSWORD)
                draw_rectangle(COLOR_BLUE, 199, 149, 242, 32);
        }
        else if (current == Screen::MAIN_MENU)
        {
            draw_text("Main Menu", COLOR_BLACK, 20, 10);
            fill_rectangle(COLOR_LIGHT_BLUE, 200, 100, 240, 40);
            draw_rectangle(COLOR_BLACK, 200, 100, 240, 40);
            draw_text("1. Enter subjects & get job suggestion", COLOR_BLACK, 210, 110);

            fill_rectangle(COLOR_LIGHT_BLUE, logout_btn.x, logout_btn.y, logout_btn.w, logout_btn.h);
            draw_rectangle(COLOR_BLACK, logout_btn.x, logout_btn.y, logout_btn.w, logout_btn.h);
            draw_text(logout_btn.label, COLOR_BLACK, logout_btn.x + 5, logout_btn.y + 8);
        }
        else if (current == Screen::ENTER_SUBJECTS)
        {
            draw_text("Enter student info and subjects (click to start)", COLOR_BLACK, 140, 20);
            fill_rectangle(COLOR_LIGHT_BLUE, back_btn.x, back_btn.y, back_btn.w, back_btn.h);
            draw_rectangle(COLOR_BLACK, back_btn.x, back_btn.y, back_btn.w, back_btn.h);
            draw_text(back_btn.label, COLOR_BLACK, back_btn.x + 8, back_btn.y + 8);
        }

        refresh_screen();
    }

    close_window("Student Portal GUI");
    return 0;
}
