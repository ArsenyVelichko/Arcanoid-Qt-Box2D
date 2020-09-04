#include <QtWidgets/QApplication>
#include <Windows.h>
#include "ArcanoidApp.h"

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  ArcanoidApp w;
  w.show();

  #ifdef _DEBUG 
    HANDLE hLogFile;
    hLogFile = CreateFile(L"Memory Leaks.txt", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, hLogFile);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, hLogFile);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, hLogFile);
  #endif

  return a.exec();
}
