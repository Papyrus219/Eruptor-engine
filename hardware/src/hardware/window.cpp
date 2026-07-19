#include <Eruptor/hardware/window.hpp>
#include <Eruptor/hardware/core.hpp>
#include <Eruptor/event/event_manager.hpp>

eruptor::hardware::Window::Window(): event_manager{event::event_manager}
{

}

void eruptor::hardware::Window::Init()
{
#ifndef NDEBUG
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

void eruptor::hardware::Window::Create_window(std::string_view title, glm::uvec2 size)
{
    window = glfwCreateWindow(size.x, size.y, title.data(), nullptr, nullptr);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(window, Window::Key_callback);
    glfwSetCursorPosCallback(window, Window::Mouse_callback);
}

void eruptor::hardware::Window::Update()
{
    glfwPollEvents();

    if( glfwWindowShouldClose( window ) )
    {
        event::Event event{};
        event = event::Event::Close_window{};

        event_manager.Announce_event( event );
    }
}

struct
{
    bool esc{}, f1{}, f2{}, f3{}, f4{}, f5{}, f6{}, f7{}, f8{}, f9{}, f10{}, f11{}, f12{}, prtsc{}, scrlk{}, pause{},
    backtic{}, key_1{}, key_2{}, key_3{}, key_4{}, key_5{}, key_6{}, key_7{}, key_8{}, key_9{}, key_0{}, minus{}, plus{}, backspace{}, ins{}, home{}, pgup{}, num{}, backslash{}, asterisk{}, num_minus{},
    tab{}, q{}, w{}, e{}, r{}, t{}, y{}, u{}, i{}, o{}, p{}, left_brasket{}, right_brasket{}, slash{}, del{}, end{}, pgdn{}, num_7{}, num_8{}, num_9{}, num_plus{},
    capslock{}, a{}, s{}, d{}, f{}, g{}, h{}, j{}, k{}, l{}, semicolom{}, apostrofe{}, enter{}, num_4{}, num_5{}, mum_6{},
    left_shift{}, z{}, x{}, c{}, v{}, b{}, n{}, m{}, tri_left_brasket{}, tri_right_brasket{}, question_mark{}, right_shift{}, up{}, num_1{}, num_2{}, num_3{}, num_enter{},
    left_ctrl{}, power{}, left_alt{}, space{}, right_alt{}, fn{}, right_ctr{}, left{}, down{}, right{}, num_0{}, num_comma{};
} key_states;

#define HANDLE_KEY(glfw_key, state_field, event_key) \
case glfw_key:                                   \
    key_states.state_field = pressed;            \
    key_type = eruptor::event::Key::event_key;   \
    break;


void eruptor::hardware::Window::Key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    const bool pressed = action != GLFW_RELEASE;

    eruptor::event::Key key_type{eruptor::event::Key::NONE};

    switch (key)
    {
        HANDLE_KEY(GLFW_KEY_ESCAPE, esc, ESC)

        HANDLE_KEY(GLFW_KEY_F1,  f1,  F1)
        HANDLE_KEY(GLFW_KEY_F2,  f2,  F2)
        HANDLE_KEY(GLFW_KEY_F3,  f3,  F3)
        HANDLE_KEY(GLFW_KEY_F4,  f4,  F4)
        HANDLE_KEY(GLFW_KEY_F5,  f5,  F5)
        HANDLE_KEY(GLFW_KEY_F6,  f6,  F6)
        HANDLE_KEY(GLFW_KEY_F7,  f7,  F7)
        HANDLE_KEY(GLFW_KEY_F8,  f8,  F8)
        HANDLE_KEY(GLFW_KEY_F9,  f9,  F9)
        HANDLE_KEY(GLFW_KEY_F10, f10, F10)
        HANDLE_KEY(GLFW_KEY_F11, f11, F11)
        HANDLE_KEY(GLFW_KEY_F12, f12, F12)

        HANDLE_KEY(GLFW_KEY_GRAVE_ACCENT, backtic, BACKTIC)

        HANDLE_KEY(GLFW_KEY_1, key_1, KEY_1)
        HANDLE_KEY(GLFW_KEY_2, key_2, KEY_2)
        HANDLE_KEY(GLFW_KEY_3, key_3, KEY_3)
        HANDLE_KEY(GLFW_KEY_4, key_4, KEY_4)
        HANDLE_KEY(GLFW_KEY_5, key_5, KEY_5)
        HANDLE_KEY(GLFW_KEY_6, key_6, KEY_6)
        HANDLE_KEY(GLFW_KEY_7, key_7, KEY_7)
        HANDLE_KEY(GLFW_KEY_8, key_8, KEY_8)
        HANDLE_KEY(GLFW_KEY_9, key_9, KEY_9)
        HANDLE_KEY(GLFW_KEY_0, key_0, KEY_0)

        HANDLE_KEY(GLFW_KEY_MINUS, minus, MINUS)
        HANDLE_KEY(GLFW_KEY_EQUAL, plus, PLUS)
        HANDLE_KEY(GLFW_KEY_BACKSPACE, backspace, BACKSPACE)

        HANDLE_KEY(GLFW_KEY_INSERT, ins, INS)
        HANDLE_KEY(GLFW_KEY_HOME, home, HOME)
        HANDLE_KEY(GLFW_KEY_PAGE_UP, pgup, PGUP)

        HANDLE_KEY(GLFW_KEY_NUM_LOCK, num, NUM)

        HANDLE_KEY(GLFW_KEY_BACKSLASH, backslash, BACKSLASH)

        HANDLE_KEY(GLFW_KEY_KP_MULTIPLY, asterisk, ASTERISK)
        HANDLE_KEY(GLFW_KEY_KP_SUBTRACT, num_minus, NUM_MINUS)

        HANDLE_KEY(GLFW_KEY_TAB, tab, TAB)

        HANDLE_KEY(GLFW_KEY_Q, q, Q)
        HANDLE_KEY(GLFW_KEY_W, w, W)
        HANDLE_KEY(GLFW_KEY_E, e, E)
        HANDLE_KEY(GLFW_KEY_R, r, R)
        HANDLE_KEY(GLFW_KEY_T, t, T)
        HANDLE_KEY(GLFW_KEY_Y, y, Y)
        HANDLE_KEY(GLFW_KEY_U, u, U)
        HANDLE_KEY(GLFW_KEY_I, i, I)
        HANDLE_KEY(GLFW_KEY_O, o, O)
        HANDLE_KEY(GLFW_KEY_P, p, P)

        HANDLE_KEY(GLFW_KEY_LEFT_BRACKET, left_brasket, LEFT_BRASKET)
        HANDLE_KEY(GLFW_KEY_RIGHT_BRACKET, right_brasket, RIGHT_BRASKET)

        HANDLE_KEY(GLFW_KEY_KP_DIVIDE, slash, SLASH)

        HANDLE_KEY(GLFW_KEY_DELETE, del, DEL)
        HANDLE_KEY(GLFW_KEY_END, end, END)
        HANDLE_KEY(GLFW_KEY_PAGE_DOWN, pgdn, PGDN)

        HANDLE_KEY(GLFW_KEY_KP_7, num_7, NUM_7)
        HANDLE_KEY(GLFW_KEY_KP_8, num_8, NUM_8)
        HANDLE_KEY(GLFW_KEY_KP_9, num_9, NUM_9)
        HANDLE_KEY(GLFW_KEY_KP_ADD, num_plus, NUM_PLUS)

        HANDLE_KEY(GLFW_KEY_CAPS_LOCK, capslock, CAPSLOCK)

        HANDLE_KEY(GLFW_KEY_A, a, A)
        HANDLE_KEY(GLFW_KEY_S, s, S)
        HANDLE_KEY(GLFW_KEY_D, d, D)
        HANDLE_KEY(GLFW_KEY_F, f, F)
        HANDLE_KEY(GLFW_KEY_G, g, G)
        HANDLE_KEY(GLFW_KEY_H, h, H)
        HANDLE_KEY(GLFW_KEY_J, j, J)
        HANDLE_KEY(GLFW_KEY_K, k, K)
        HANDLE_KEY(GLFW_KEY_L, l, L)

        HANDLE_KEY(GLFW_KEY_SEMICOLON, semicolom, SEMICOLOM)
        HANDLE_KEY(GLFW_KEY_APOSTROPHE, apostrofe, APOSTROFE)

        HANDLE_KEY(GLFW_KEY_ENTER, enter, ENTER)

        HANDLE_KEY(GLFW_KEY_KP_4, num_4, NUM_4)
        HANDLE_KEY(GLFW_KEY_KP_5, num_5, NUM_5)
        HANDLE_KEY(GLFW_KEY_KP_6, mum_6, MUM_6)

        HANDLE_KEY(GLFW_KEY_LEFT_SHIFT, left_shift, LEFT_SHIFT)

        HANDLE_KEY(GLFW_KEY_Z, z, Z)
        HANDLE_KEY(GLFW_KEY_X, x, X)
        HANDLE_KEY(GLFW_KEY_C, c, C)
        HANDLE_KEY(GLFW_KEY_V, v, V)
        HANDLE_KEY(GLFW_KEY_B, b, B)
        HANDLE_KEY(GLFW_KEY_N, n, N)
        HANDLE_KEY(GLFW_KEY_M, m, M)

        HANDLE_KEY(GLFW_KEY_COMMA, tri_left_brasket, TRI_LEFT_BRASKET)
        HANDLE_KEY(GLFW_KEY_PERIOD, tri_right_brasket, TRI_RIGHT_BRASKET)
        HANDLE_KEY(GLFW_KEY_SLASH, question_mark, QUESTION_MARK)

        HANDLE_KEY(GLFW_KEY_RIGHT_SHIFT, right_shift, RIGHT_SHIFT)

        HANDLE_KEY(GLFW_KEY_UP, up, UP)

        HANDLE_KEY(GLFW_KEY_KP_1, num_1, NUM_1)
        HANDLE_KEY(GLFW_KEY_KP_2, num_2, NUM_2)
        HANDLE_KEY(GLFW_KEY_KP_3, num_3, NUM_3)
        HANDLE_KEY(GLFW_KEY_KP_ENTER, num_enter, NUM_ENTER)

        HANDLE_KEY(GLFW_KEY_LEFT_CONTROL, left_ctrl, LEFT_CTRL)
        HANDLE_KEY(GLFW_KEY_LEFT_ALT, left_alt, LEFT_ALT)
        HANDLE_KEY(GLFW_KEY_SPACE, space, SPACE)
        HANDLE_KEY(GLFW_KEY_RIGHT_ALT, right_alt, RIGHT_ALT)
        HANDLE_KEY(GLFW_KEY_RIGHT_CONTROL, right_ctr, RIGHT_CTR)

        HANDLE_KEY(GLFW_KEY_LEFT, left, LEFT)
        HANDLE_KEY(GLFW_KEY_DOWN, down, DOWN)
        HANDLE_KEY(GLFW_KEY_RIGHT, right, RIGHT)

        HANDLE_KEY(GLFW_KEY_KP_0, num_0, NUM_0)
        HANDLE_KEY(GLFW_KEY_KP_DECIMAL, num_comma, NUM_COMMA)

        default:
            return;
    }

    if(pressed)
    {
        event::event_manager.Announce_event( eruptor::event::Event{ eruptor::event::Event::Key_pressed{.key_type = key_type} } );
    }
    else
    {
        event::event_manager.Announce_event( eruptor::event::Event{ eruptor::event::Event::Key_release{.key_type = key_type} } );
    }
}


void eruptor::hardware::Window::Mouse_callback(GLFWwindow* window, double x_pos, double y_pos)
{
    static float last_x{}, last_y{};

    event::Event::Mouse_moved mouse_moved{};
    mouse_moved.x_offset = x_pos - last_x;
    mouse_moved.y_offset = y_pos - last_y;

    event::Event event{};
    event = mouse_moved;

    event::event_manager.Announce_event( event );

    last_x = x_pos;
    last_y = y_pos;
}

bool eruptor::hardware::Window::Is_key_pressed(event::Key key)
{
    switch (key)
    {
        case event::Key::ESC:              return key_states.esc;

        case event::Key::F1:               return key_states.f1;
        case event::Key::F2:               return key_states.f2;
        case event::Key::F3:               return key_states.f3;
        case event::Key::F4:               return key_states.f4;
        case event::Key::F5:               return key_states.f5;
        case event::Key::F6:               return key_states.f6;
        case event::Key::F7:               return key_states.f7;
        case event::Key::F8:               return key_states.f8;
        case event::Key::F9:               return key_states.f9;
        case event::Key::F10:              return key_states.f10;
        case event::Key::F11:              return key_states.f11;
        case event::Key::F12:              return key_states.f12;

        case event::Key::BACKTIC:          return key_states.backtic;

        case event::Key::KEY_1:            return key_states.key_1;
        case event::Key::KEY_2:            return key_states.key_2;
        case event::Key::KEY_3:            return key_states.key_3;
        case event::Key::KEY_4:            return key_states.key_4;
        case event::Key::KEY_5:            return key_states.key_5;
        case event::Key::KEY_6:            return key_states.key_6;
        case event::Key::KEY_7:            return key_states.key_7;
        case event::Key::KEY_8:            return key_states.key_8;
        case event::Key::KEY_9:            return key_states.key_9;
        case event::Key::KEY_0:            return key_states.key_0;

        case event::Key::MINUS:            return key_states.minus;
        case event::Key::PLUS:             return key_states.plus;
        case event::Key::BACKSPACE:        return key_states.backspace;

        case event::Key::INS:              return key_states.ins;
        case event::Key::HOME:             return key_states.home;
        case event::Key::PGUP:             return key_states.pgup;

        case event::Key::NUM:              return key_states.num;

        case event::Key::BACKSLASH:        return key_states.backslash;

        case event::Key::ASTERISK:         return key_states.asterisk;
        case event::Key::NUM_MINUS:        return key_states.num_minus;

        case event::Key::TAB:              return key_states.tab;

        case event::Key::Q:                return key_states.q;
        case event::Key::W:                return key_states.w;
        case event::Key::E:                return key_states.e;
        case event::Key::R:                return key_states.r;
        case event::Key::T:                return key_states.t;
        case event::Key::Y:                return key_states.y;
        case event::Key::U:                return key_states.u;
        case event::Key::I:                return key_states.i;
        case event::Key::O:                return key_states.o;
        case event::Key::P:                return key_states.p;

        case event::Key::LEFT_BRASKET:     return key_states.left_brasket;
        case event::Key::RIGHT_BRASKET:    return key_states.right_brasket;

        case event::Key::SLASH:            return key_states.slash;

        case event::Key::DEL:              return key_states.del;
        case event::Key::END:              return key_states.end;
        case event::Key::PGDN:             return key_states.pgdn;

        case event::Key::NUM_7:            return key_states.num_7;
        case event::Key::NUM_8:            return key_states.num_8;
        case event::Key::NUM_9:            return key_states.num_9;
        case event::Key::NUM_PLUS:         return key_states.num_plus;

        case event::Key::CAPSLOCK:         return key_states.capslock;

        case event::Key::A:                return key_states.a;
        case event::Key::S:                return key_states.s;
        case event::Key::D:                return key_states.d;
        case event::Key::F:                return key_states.f;
        case event::Key::G:                return key_states.g;
        case event::Key::H:                return key_states.h;
        case event::Key::J:                return key_states.j;
        case event::Key::K:                return key_states.k;
        case event::Key::L:                return key_states.l;

        case event::Key::SEMICOLOM:        return key_states.semicolom;
        case event::Key::APOSTROFE:        return key_states.apostrofe;

        case event::Key::ENTER:            return key_states.enter;

        case event::Key::NUM_4:            return key_states.num_4;
        case event::Key::NUM_5:            return key_states.num_5;
        case event::Key::MUM_6:            return key_states.mum_6;

        case event::Key::LEFT_SHIFT:       return key_states.left_shift;

        case event::Key::Z:                return key_states.z;
        case event::Key::X:                return key_states.x;
        case event::Key::C:                return key_states.c;
        case event::Key::V:                return key_states.v;
        case event::Key::B:                return key_states.b;
        case event::Key::N:                return key_states.n;
        case event::Key::M:                return key_states.m;

        case event::Key::TRI_LEFT_BRASKET:  return key_states.tri_left_brasket;
        case event::Key::TRI_RIGHT_BRASKET: return key_states.tri_right_brasket;
        case event::Key::QUESTION_MARK:     return key_states.question_mark;

        case event::Key::RIGHT_SHIFT:      return key_states.right_shift;

        case event::Key::UP:               return key_states.up;

        case event::Key::NUM_1:            return key_states.num_1;
        case event::Key::NUM_2:            return key_states.num_2;
        case event::Key::NUM_3:            return key_states.num_3;
        case event::Key::NUM_ENTER:        return key_states.num_enter;

        case event::Key::LEFT_CTRL:        return key_states.left_ctrl;
        case event::Key::LEFT_ALT:         return key_states.left_alt;
        case event::Key::SPACE:            return key_states.space;
        case event::Key::RIGHT_ALT:        return key_states.right_alt;
        case event::Key::RIGHT_CTR:        return key_states.right_ctr;

        case event::Key::LEFT:             return key_states.left;
        case event::Key::DOWN:             return key_states.down;
        case event::Key::RIGHT:            return key_states.right;

        case event::Key::NUM_0:            return key_states.num_0;
        case event::Key::NUM_COMMA:        return key_states.num_comma;

        default:
            return false;
    }
}


#undef HANDLE_KEY
