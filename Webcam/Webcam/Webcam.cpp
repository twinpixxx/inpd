#pragma comment(lib, "user32.lib")

#include <windows.h>
#include <map>
#include <functional>
#include <thread>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


#define FRAME_RATE 10
#define FRAME_TIME 1000 / FRAME_RATE
#define HIDDEN_MODE_TIME 10


string GenerateName()
{
    time_t now = time(0);
    tm* ltm = new tm;
    localtime_s(ltm, &now);
    stringstream sstream;
    sstream << ltm->tm_year + 1900 << '_'
        << ltm->tm_mon + 1 << '_'
        << ltm->tm_mday << '_'
        << ltm->tm_hour << '_'
        << ltm->tm_min << '_'
        << ltm->tm_sec;
    return sstream.str();

}

void TakePicture() {
    VideoCapture camera(0);
    if (camera.isOpened()) {
        Mat frame;
        camera >> frame;
        string name = GenerateName() + ".jpg";
        imwrite(name, frame);
        camera.release();
    }
}

void ToggleCapture() {
    static bool tracking = false;
    static VideoCapture camera;
    static VideoWriter video;
 


    if (!tracking) {
        tracking = true;

        thread scanner([]() {
            camera.open(0);
            if (!camera.isOpened()) {
                return;
            }

            double frame_width = camera.get(CAP_PROP_FRAME_WIDTH);
            double frame_height = camera.get(CAP_PROP_FRAME_HEIGHT);

            camera.set(CAP_PROP_SETTINGS, 1);

            string name = GenerateName() + ".avi";


            video.open(name,
                VideoWriter::fourcc('M', 'P', '4', '2'),
                7,
                Size(frame_width, frame_height), true);
            if (!video.isOpened()) {
                return;
            }

            while (true) {
                Mat frame;
                if (!camera.read(frame)) {
                    break;
                }
                if (frame.empty())
                    break;
                video << frame;
            }
            });
        scanner.detach();
    }
    else {
        camera.release();
        video.release();
        tracking = false;
    }
}

bool IsConsoleVisible()
{
    return ::IsWindowVisible(::GetConsoleWindow()) != FALSE;
}

void ToggleConsoleWindow() {
    if (IsConsoleVisible()) {
        //ShowWindow(::GetConsoleWindow(), SW_HIDE);
    }
    else {
        ShowWindow(::GetConsoleWindow(), SW_SHOW);
    }
}

static map<char, function<void()>> library = {
    { 'Y', TakePicture },
    { 'U', ToggleCapture },
    { 'Q', ToggleConsoleWindow }
};

HHOOK hKeyboardHook;

__declspec(dllexport) LRESULT CALLBACK KeyboardEvent(int nCode, WPARAM wParam, LPARAM lParam) {
    if ((nCode == HC_ACTION) &&
        ((wParam == WM_SYSKEYDOWN) ||
            (wParam == WM_KEYDOWN))) {
        KBDLLHOOKSTRUCT key = *reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        if (library.find(key.vkCode) != library.end()) {
            library[key.vkCode]();
        }
    }
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

void MessageLoop() {
    MSG message;
    while (GetMessage(&message, nullptr, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
}

int main() {
    ShowWindow(::GetConsoleWindow(), SW_SHOW);
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    hKeyboardHook = SetWindowsHookEx(
        WH_KEYBOARD_LL,
        (HOOKPROC)KeyboardEvent,
        hInstance,
        NULL);
    MessageLoop(); 
    UnhookWindowsHookEx(hKeyboardHook);
    return 0;
}