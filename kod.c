#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

// Идентификаторы элементов управления
#define EDIT_ROWS 100
#define EDIT_COLS 101
#define BUTTON_SOLVE_NW 200
#define BUTTON_SOLVE_LC 201
#define BUTTON_SHOW_DATA 206
#define BUTTON_EXIT 202
#define BUTTON_RESET 204
#define BUTTON_RANDOM_DATA 205
#define STATIC_RESULT 300
#define BUTTON_INPUT_DATA 203
#define EDIT_SUPPLY_BASE 1000
#define EDIT_DEMAND_BASE 2000
#define EDIT_COST_BASE 3000
#define BUTTON_SAVE_DATA 4000
#define BUTTON_CLOSE_DIALOG 4001
#define MENU_SAVE_FILE 5001
#define MENU_LOAD_FILE 5002


// Глобальные переменные для хранения размеров и данных
int g_rows = 0, g_cols = 0;
int* g_supply = NULL;
int* g_demand = NULL;
int** g_cost = NULL;
int** g_allocation = NULL;
HINSTANCE g_hInstance = NULL;

// Прототипы функций
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK DataDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int** allocate_matrix(int rows, int cols);
void free_matrix(int** matrix, int rows);
void northwest_corner(int* supply, int* demand, int** cost, int rows, int cols, int** allocation, long long* total_cost);
void least_cost_method(int* supply, int* demand, int** cost, int rows, int cols, int** allocation, long long* total_cost);
void print_results(HWND hwnd, int** allocation, int rows, int cols, const wchar_t* method, long long total_cost);
void show_data_input_dialog(HWND parent);
int check_balance(int* supply, int* demand, int rows, int cols);

// Основная функция WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    srand((unsigned int)time(NULL));
    g_hInstance = hInstance;
    const wchar_t CLASS_NAME[] = L"TransportSolverSimple";
    const wchar_t DIALOG_CLASS_NAME[] = L"DataInputDialog";

    // Регистрируем класс главного окна
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc);

    // Регистрируем класс диалогового окна один раз
    WNDCLASS dialogWc = { 0 };
    dialogWc.lpfnWndProc = DataDlgProc;
    dialogWc.hInstance = hInstance;
    dialogWc.lpszClassName = DIALOG_CLASS_NAME;
    dialogWc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&dialogWc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Транспортная задача - Упрощенный интерфейс",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

// Функция для сохранения данных в файл
void SaveDataToFile(HWND hwnd) {
    if (!g_supply || !g_demand || !g_cost) {
        MessageBox(hwnd, L"Нет данных для сохранения!", L"Ошибка", MB_ICONERROR);
        return;
    }

    OPENFILENAME ofn = { 0 };
    wchar_t szFile[260] = { 0 };
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
    ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt = L"txt";

    if (GetSaveFileName(&ofn)) {
        FILE* fp;
        if (_wfopen_s(&fp, szFile, L"w") == 0) {
            // Записываем количество строк и столбцов
            fwprintf(fp, L"%d %d\n", g_rows, g_cols);

            // Записываем запасы поставщиков
            for (int i = 0; i < g_rows; i++) {
                fwprintf(fp, L"%d ", g_supply[i]);
            }
            fwprintf(fp, L"\n");

            // Записываем потребности потребителей
            for (int j = 0; j < g_cols; j++) {
                fwprintf(fp, L"%d ", g_demand[j]);
            }
            fwprintf(fp, L"\n");

            // Записываем матрицу стоимостей
            for (int i = 0; i < g_rows; i++) {
                for (int j = 0; j < g_cols; j++) {
                    fwprintf(fp, L"%d ", g_cost[i][j]);
                }
                fwprintf(fp, L"\n");
            }

            fclose(fp);
            MessageBox(hwnd, L"Данные успешно сохранены в файл!", L"Успех", MB_ICONINFORMATION);
        }
        else {
            MessageBox(hwnd, L"Не удалось открыть файл для записи!", L"Ошибка", MB_ICONERROR);
        }
    }
}

// Функция для загрузки данных из файла
void LoadDataFromFile(HWND hwnd) {
    OPENFILENAME ofn = { 0 };
    wchar_t szFile[260] = { 0 };
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
    ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    ofn.lpstrDefExt = L"txt";

    if (GetOpenFileName(&ofn)) {
        FILE* fp;
        if (_wfopen_s(&fp, szFile, L"r") == 0) {
            int rows, cols;
            if (fwscanf_s(fp, L"%d %d", &rows, &cols) != 2 || rows <= 0 || cols <= 0) {
                MessageBox(hwnd, L"Некорректный формат файла: ошибка при чтении размеров.", L"Ошибка", MB_ICONERROR);
                fclose(fp);
                return;
            }

            // Освобождаем старую память, если она была выделена
            if (g_supply) free(g_supply);
            if (g_demand) free(g_demand);
            if (g_cost) free_matrix(g_cost, g_rows);

            // Выделяем новую память
            g_rows = rows;
            g_cols = cols;
            g_supply = (int*)malloc(g_rows * sizeof(int));
            g_demand = (int*)malloc(g_cols * sizeof(int));
            g_cost = allocate_matrix(g_rows, g_cols);

            if (!g_supply || !g_demand || !g_cost) {
                MessageBox(hwnd, L"Не удалось выделить память для данных.", L"Ошибка", MB_ICONERROR);
                if (g_supply) free(g_supply);
                if (g_demand) free(g_demand);
                if (g_cost) free_matrix(g_cost, g_rows);
                g_supply = NULL;
                g_demand = NULL;
                g_cost = NULL;
                fclose(fp);
                return;
            }

            // Читаем запасы поставщиков
            for (int i = 0; i < g_rows; i++) {
                if (fwscanf_s(fp, L"%d", &g_supply[i]) != 1) {
                    MessageBox(hwnd, L"Некорректный формат файла: ошибка при чтении запасов.", L"Ошибка", MB_ICONERROR);
                    free(g_supply);
                    free(g_demand);
                    free_matrix(g_cost, g_rows);
                    g_supply = NULL;
                    g_demand = NULL;
                    g_cost = NULL;
                    fclose(fp);
                    return;
                }
            }

            // Читаем потребности потребителей
            for (int j = 0; j < g_cols; j++) {
                if (fwscanf_s(fp, L"%d", &g_demand[j]) != 1) {
                    MessageBox(hwnd, L"Некорректный формат файла: ошибка при чтении потребностей.", L"Ошибка", MB_ICONERROR);
                    free(g_supply);
                    free(g_demand);
                    free_matrix(g_cost, g_rows);
                    g_supply = NULL;
                    g_demand = NULL;
                    g_cost = NULL;
                    fclose(fp);
                    return;
                }
            }

            // Читаем матрицу стоимостей
            for (int i = 0; i < g_rows; i++) {
                for (int j = 0; j < g_cols; j++) {
                    if (fwscanf_s(fp, L"%d", &g_cost[i][j]) != 1) {
                        MessageBox(hwnd, L"Некорректный формат файла: ошибка при чтении стоимостей.", L"Ошибка", MB_ICONERROR);
                        free(g_supply);
                        free(g_demand);
                        free_matrix(g_cost, g_rows);
                        g_supply = NULL;
                        g_demand = NULL;
                        g_cost = NULL;
                        fclose(fp);
                        return;
                    }
                }
            }

            fclose(fp);
            MessageBox(hwnd, L"Данные успешно загружены из файла!", L"Успех", MB_ICONINFORMATION);
        }
        else {
            MessageBox(hwnd, L"Не удалось открыть файл для чтения!", L"Ошибка", MB_ICONERROR);
        }
    }
}

// Оконная процедура главного окна
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        CreateWindow(L"STATIC", L"Количество поставщиков:", WS_VISIBLE | WS_CHILD,
            20, 20, 150, 25, hwnd, NULL, NULL, NULL);
        CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
            180, 20, 50, 25, hwnd, (HMENU)EDIT_ROWS, NULL, NULL);

        CreateWindow(L"STATIC", L"Количество потребителей:", WS_VISIBLE | WS_CHILD,
            250, 20, 150, 25, hwnd, NULL, NULL, NULL);
        CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
            410, 20, 50, 25, hwnd, (HMENU)EDIT_COLS, NULL, NULL);
        CreateWindow(L"BUTTON", L"Ввести данные", WS_VISIBLE | WS_CHILD,
            20, 60, 200, 30, hwnd, (HMENU)BUTTON_INPUT_DATA, NULL, NULL);
        CreateWindow(L"BUTTON", L"Сгенерировать данные", WS_VISIBLE | WS_CHILD,
            20, 100, 200, 30, hwnd, (HMENU)BUTTON_RANDOM_DATA, NULL, NULL);
        CreateWindow(L"BUTTON", L"Показать данные", WS_VISIBLE | WS_CHILD,
            20, 140, 200, 30, hwnd, (HMENU)BUTTON_SHOW_DATA, NULL, NULL);
        CreateWindow(L"BUTTON", L"Метод Северо-Западного угла", WS_VISIBLE | WS_CHILD,
            20, 180, 200, 30, hwnd, (HMENU)BUTTON_SOLVE_NW, NULL, NULL);
        CreateWindow(L"BUTTON", L"Метод минимальной стоимости", WS_VISIBLE | WS_CHILD,
            20, 220, 200, 30, hwnd, (HMENU)BUTTON_SOLVE_LC, NULL, NULL);
        CreateWindow(L"BUTTON", L"Сброс", WS_VISIBLE | WS_CHILD,
            20, 260, 200, 30, hwnd, (HMENU)BUTTON_RESET, NULL, NULL);
        CreateWindow(L"BUTTON", L"Выход", WS_VISIBLE | WS_CHILD,
            20, 300, 200, 30, hwnd, (HMENU)BUTTON_EXIT, NULL, NULL);
        CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_READONLY,
            250, 60, 520, 480, hwnd, (HMENU)STATIC_RESULT, NULL, NULL);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case BUTTON_INPUT_DATA: {
            wchar_t rowsText[10], colsText[10];
            GetWindowText(GetDlgItem(hwnd, EDIT_ROWS), rowsText, 10);
            GetWindowText(GetDlgItem(hwnd, EDIT_COLS), colsText, 10);
            g_rows = _wtoi(rowsText);
            g_cols = _wtoi(colsText);

            if (g_rows <= 0 || g_cols <= 0) {
                MessageBox(hwnd, L"Введите корректные положительные значения для поставщиков и потребителей.",
                    L"Ошибка", MB_ICONERROR);
                break;
            }
            show_data_input_dialog(hwnd);
            break;
        }
        case BUTTON_SHOW_DATA:
            if (!g_supply || !g_demand || !g_cost) {
                MessageBox(hwnd, L"Сначала введите данные о грузах!", L"Ошибка", MB_ICONERROR);
                break;
            }
            // Создаем новое диалоговое окно для отображения данных
            HWND hDataDlg = CreateWindow(L"DataDisplayClass", L"Данные", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, hwnd, NULL, NULL, NULL);
            if (!hDataDlg) {
                MessageBox(hwnd, L"Не удалось создать окно данных!", L"Ошибка", MB_ICONERROR);
            }
            break;
        case BUTTON_RANDOM_DATA: {
            if (g_rows <= 0 || g_cols <= 0) {
                MessageBox(hwnd, L"Введите корректные значения для количества поставщиков и потребителей.", L"Ошибка", MB_ICONERROR);
                break;
            }

            // Освобождаем старую память, если она была выделена
            if (g_supply) free(g_supply);
            if (g_demand) free(g_demand);
            if (g_cost) free_matrix(g_cost, g_rows);

            // Выделяем новую память
            g_supply = (int*)malloc(g_rows * sizeof(int));
            g_demand = (int*)malloc(g_cols * sizeof(int));
            g_cost = allocate_matrix(g_rows, g_cols);

            if (!g_supply || !g_demand || !g_cost) {
                MessageBox(hwnd, L"Не удалось выделить память для случайных данных.", L"Ошибка", MB_ICONERROR);
                if (g_supply) free(g_supply);
                if (g_demand) free(g_demand);
                if (g_cost) free_matrix(g_cost, g_rows);
                g_supply = NULL;
                g_demand = NULL;
                g_cost = NULL;
                break;
            }

            // Генерируем случайные данные для запасов поставщиков (от 10 до 100)
            for (int i = 0; i < g_rows; i++) {
                g_supply[i] = (rand() % 91) + 10; // Случайное число от 10 до 100
            }

            // Генерируем случайные данные для потребностей потребителей (от 10 до 100)
            for (int j = 0; j < g_cols; j++) {
                g_demand[j] = (rand() % 91) + 10; // Случайное число от 10 до 100
            }

            // Генерируем случайные данные для матрицы стоимостей (от 1 до 50)
            for (int i = 0; i < g_rows; i++) {
                for (int j = 0; j < g_cols; j++) {
                    g_cost[i][j] = (rand() % 50) + 1; // Случайное число от 1 до 50
                }
            }

            MessageBox(hwnd, L"Случайные данные успешно сгенерированы!", L"Успех", MB_ICONINFORMATION);
            break;
        }
        case BUTTON_SOLVE_NW: {
            if (!g_supply || !g_demand || !g_cost) {
                MessageBox(hwnd, L"Сначала введите данные о грузах!", L"Ошибка", MB_ICONERROR);
                break;
            }
            if (!check_balance(g_supply, g_demand, g_rows, g_cols)) {
                MessageBox(hwnd, L"Сумма запасов не равна сумме потребностей. Решение может быть некорректным.",
                    L"Предупреждение", MB_ICONWARNING);
            }
            if (g_allocation) free_matrix(g_allocation, g_rows);
            g_allocation = allocate_matrix(g_rows, g_cols);
            if (!g_allocation) {
                MessageBox(hwnd, L"Не удалось выделить память для результата!", L"Ошибка", MB_ICONERROR);
                break;
            }
            long long total_cost = 0;
            northwest_corner(g_supply, g_demand, g_cost, g_rows, g_cols, g_allocation, &total_cost);
            print_results(hwnd, g_allocation, g_rows, g_cols, L"Северо-Западный угол", total_cost);
            break;
        }
        case BUTTON_SOLVE_LC: {
            if (!g_supply || !g_demand || !g_cost) {
                MessageBox(hwnd, L"Сначала введите данные о грузах!", L"Ошибка", MB_ICONERROR);
                break;
            }
            if (!check_balance(g_supply, g_demand, g_rows, g_cols)) {
                MessageBox(hwnd, L"Сумма запасов не равна сумме потребностей. Решение может быть некорректным.",
                    L"Предупреждение", MB_ICONWARNING);
            }
            if (g_allocation) free_matrix(g_allocation, g_rows);
            g_allocation = allocate_matrix(g_rows, g_cols);
            if (!g_allocation) {
                MessageBox(hwnd, L"Не удалось выделить память для результата!", L"Ошибка", MB_ICONERROR);
                break;
            }
            long long total_cost = 0;
            least_cost_method(g_supply, g_demand, g_cost, g_rows, g_cols, g_allocation, &total_cost);
            print_results(hwnd, g_allocation, g_rows, g_cols, L"Минимальная стоимость", total_cost);
            break;
        }
        case BUTTON_RESET:
            // Освобождаем память для данных
            if (g_supply) {
                free(g_supply);
                g_supply = NULL;
            }
            if (g_demand) {
                free(g_demand);
                g_demand = NULL;
            }
            if (g_cost) {
                free_matrix(g_cost, g_rows);
                g_cost = NULL;
            }
            if (g_allocation) {
                free_matrix(g_allocation, g_rows);
                g_allocation = NULL;
            }
            // Сбрасываем размеры
            g_rows = 0;
            g_cols = 0;
            // Очищаем поля ввода
            SetWindowText(GetDlgItem(hwnd, EDIT_ROWS), L"");
            SetWindowText(GetDlgItem(hwnd, EDIT_COLS), L"");
            // Очищаем поле результатов
            SetWindowText(GetDlgItem(hwnd, STATIC_RESULT), L"");
            MessageBox(hwnd, L"Все данные сброшены.", L"Успех", MB_ICONINFORMATION);
            break;
        case BUTTON_EXIT:
            PostQuitMessage(0);
            break;
        }
        break;

    case WM_DESTROY:
        if (g_allocation) free_matrix(g_allocation, g_rows);
        if (g_cost) free_matrix(g_cost, g_rows);
        if (g_supply) free(g_supply);
        if (g_demand) free(g_demand);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

// Проверка баланса задачи
int check_balance(int* supply, int* demand, int rows, int cols) {
    int total_supply = 0;
    int total_demand = 0;
    for (int i = 0; i < rows; i++) {
        total_supply += supply[i];
    }
    for (int j = 0; j < cols; j++) {
        total_demand += demand[j];
    }
    return total_supply == total_demand ? 1 : 0;
}

// Выделение памяти для матрицы
int** allocate_matrix(int rows, int cols) {
    int** matrix = (int**)malloc(rows * sizeof(int*));
    if (!matrix) return NULL;
    for (int i = 0; i < rows; i++) {
        matrix[i] = (int*)malloc(cols * sizeof(int));
        if (!matrix[i]) {
            for (int k = 0; k < i; k++) free(matrix[k]);
            free(matrix);
            return NULL;
        }
    }
    return matrix;
}

// Освобождение памяти для матрицы
void free_matrix(int** matrix, int rows) {
    if (!matrix) return;
    for (int i = 0; i < rows; i++) {
        if (matrix[i]) free(matrix[i]);
    }
    free(matrix);
}

// Метод северо-западного угла с расчетом стоимости
void northwest_corner(int* supply, int* demand, int** cost, int rows, int cols, int** allocation, long long* total_cost) {
    int* temp_supply = (int*)malloc(rows * sizeof(int));
    int* temp_demand = (int*)malloc(cols * sizeof(int));
    if (!temp_supply || !temp_demand) {
        if (temp_supply) free(temp_supply);
        if (temp_demand) free(temp_demand);
        *total_cost = 0;
        return;
    }
    *total_cost = 0;

    for (int i = 0; i < rows; i++) temp_supply[i] = supply[i];
    for (int j = 0; j < cols; j++) temp_demand[j] = demand[j];

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            allocation[i][j] = 0;

    int i = 0, j = 0;
    while (i < rows && j < cols) {
        int quantity = (temp_supply[i] < temp_demand[j]) ? temp_supply[i] : temp_demand[j];
        allocation[i][j] = quantity;
        *total_cost += (long long)quantity * cost[i][j];
        temp_supply[i] -= quantity;
        temp_demand[j] -= quantity;

        if (temp_supply[i] == 0) i++;
        if (temp_demand[j] == 0) j++;
    }

    free(temp_supply);
    free(temp_demand);
}

// Метод минимальной стоимости с расчетом стоимости
void least_cost_method(int* supply, int* demand, int** cost, int rows, int cols, int** allocation, long long* total_cost) {
    int* temp_supply = (int*)malloc(rows * sizeof(int));
    int* temp_demand = (int*)malloc(cols * sizeof(int));
    if (!temp_supply || !temp_demand) {
        if (temp_supply) free(temp_supply);
        if (temp_demand) free(temp_demand);
        *total_cost = 0;
        return;
    }
    *total_cost = 0;

    for (int i = 0; i < rows; i++) temp_supply[i] = supply[i];
    for (int j = 0; j < cols; j++) temp_demand[j] = demand[j];

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            allocation[i][j] = 0;

    while (1) {
        int min_cost = INT_MAX, min_i = -1, min_j = -1;

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (temp_supply[i] > 0 && temp_demand[j] > 0 && cost[i][j] < min_cost) {
                    min_cost = cost[i][j];
                    min_i = i;
                    min_j = j;
                }
            }
        }

        if (min_i == -1 || min_j == -1) break;

        int quantity = (temp_supply[min_i] < temp_demand[min_j]) ? temp_supply[min_i] : temp_demand[min_j];
        allocation[min_i][min_j] = quantity;
        *total_cost += (long long)quantity * cost[min_i][min_j];
        temp_supply[min_i] -= quantity;
        temp_demand[min_j] -= quantity;
    }

    free(temp_supply);
    free(temp_demand);
}

// Вывод результатов в текстовое поле с общей стоимостью
void print_results(HWND hwnd, int** allocation, int rows, int cols, const wchar_t* method, long long total_cost) {
    wchar_t* result = (wchar_t*)malloc(4096 * sizeof(wchar_t));
    if (!result) return;
    result[0] = L'\0';

    wcscat_s(result, 4096, L"Результаты (");
    wcscat_s(result, 4096, method);
    wcscat_s(result, 4096, L"):\r\n");

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            wchar_t val[12];
            swprintf_s(val, 12, L"%d\t", allocation[i][j]);
            wcscat_s(result, 4096, val);
        }
        wcscat_s(result, 4096, L"\r\n");
    }

    wchar_t cost_text[50];
    swprintf_s(cost_text, 50, L"\r\nОбщая стоимость: %lld\r\n", total_cost);
    wcscat_s(result, 4096, cost_text);

    SetWindowText(GetDlgItem(hwnd, STATIC_RESULT), result);
    free(result);

    HWND resultWnd = GetDlgItem(hwnd, STATIC_RESULT);
    SendMessage(resultWnd, EM_SETLIMITTEXT, (WPARAM)0, 0);
    SendMessage(resultWnd, EM_SETSEL, 0, 0);
}

// Показать диалоговое окно для ввода данных (программно)
void show_data_input_dialog(HWND parent) {
    // Создаем окно диалога
    int dialogWidth = 600;
    int dialogHeight = (g_rows * 30 + g_cols * 30 + 100) > 400 ? 400 : (g_rows * 30 + g_cols * 30 + 100);
    HWND hwndDialog = CreateWindowEx(
        WS_EX_DLGMODALFRAME,
        L"DataInputDialog",
        L"Ввод данных для транспортной задачи",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VSCROLL,
        CW_USEDEFAULT, CW_USEDEFAULT, dialogWidth, dialogHeight,
        parent, NULL, g_hInstance, NULL
    );

    if (!hwndDialog) {
        MessageBox(parent, L"Не удалось создать окно ввода данных.", L"Ошибка", MB_ICONERROR);
        return;
    }

    // Отключаем родительское окно для модального поведения
    EnableWindow(parent, FALSE);
    ShowWindow(hwndDialog, SW_SHOW);
    UpdateWindow(hwndDialog);

    // Запускаем цикл сообщений для модального поведения
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!IsDialogMessage(hwndDialog, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        // Проверяем, закрыто ли окно диалога (через пользовательский флаг или сообщение)
        if (!IsWindow(hwndDialog)) {
            EnableWindow(parent, TRUE);
            break;
        }
    }
}

// Оконная процедура для диалогового окна ввода данных (программно)
LRESULT CALLBACK DataDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND* supplyEdits = NULL;
    static HWND* demandEdits = NULL;
    static HWND** costEdits = NULL;
    static int arraysInitialized = 0;

    switch (uMsg) {
    case WM_CREATE: {
        int yOffset = 20;
        int xOffset = 20;
        int fieldWidth = 50;
        int fieldHeight = 25;
        int spacing = 10;
        int maxVisibleHeight = 500; // Ограничение высоты видимой области окна

        // Выделяем память для хранения указателей на поля ввода
        supplyEdits = (HWND*)malloc(g_rows * sizeof(HWND));
        demandEdits = (HWND*)malloc(g_cols * sizeof(HWND));
        costEdits = (HWND**)malloc(g_rows * sizeof(HWND*));
        if (!supplyEdits || !demandEdits || !costEdits) {
            MessageBox(hwnd, L"Не удалось выделить память для элементов управления.", L"Ошибка", MB_ICONERROR);
            DestroyWindow(hwnd);
            break;
        }
        for (int i = 0; i < g_rows; i++) {
            costEdits[i] = (HWND*)malloc(g_cols * sizeof(HWND));
            if (!costEdits[i]) {
                MessageBox(hwnd, L"Не удалось выделить память для элементов управления.", L"Ошибка", MB_ICONERROR);
                DestroyWindow(hwnd);
                break;
            }
        }
        arraysInitialized = 1;

        // Метки и поля для запасов поставщиков
        CreateWindow(L"STATIC", L"Запасы поставщиков:", WS_VISIBLE | WS_CHILD,
            xOffset, yOffset, 150, fieldHeight, hwnd, NULL, NULL, NULL);
        yOffset += fieldHeight + spacing;

        for (int i = 0; i < g_rows; i++) {
            wchar_t label[50];
            swprintf_s(label, 50, L"Поставщик %d:", i + 1);
            CreateWindow(L"STATIC", label, WS_VISIBLE | WS_CHILD,
                xOffset, yOffset, 100, fieldHeight, hwnd, NULL, NULL, NULL);
            supplyEdits[i] = CreateWindow(L"EDIT", L"0", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
                xOffset + 110, yOffset, fieldWidth, fieldHeight, hwnd, (HMENU)(EDIT_SUPPLY_BASE + i), NULL, NULL);
            yOffset += fieldHeight + spacing;
        }

        // Метки и поля для потребностей потребителей
        yOffset += spacing * 2;
        CreateWindow(L"STATIC", L"Потребности потребителей:", WS_VISIBLE | WS_CHILD,
            xOffset, yOffset, 200, fieldHeight, hwnd, NULL, NULL, NULL);
        yOffset += fieldHeight + spacing;

        for (int j = 0; j < g_cols; j++) {
            wchar_t label[50];
            swprintf_s(label, 50, L"Потребитель %d:", j + 1);
            CreateWindow(L"STATIC", label, WS_VISIBLE | WS_CHILD,
                xOffset, yOffset, 100, fieldHeight, hwnd, NULL, NULL, NULL);
            demandEdits[j] = CreateWindow(L"EDIT", L"0", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
                xOffset + 110, yOffset, fieldWidth, fieldHeight, hwnd, (HMENU)(EDIT_DEMAND_BASE + j), NULL, NULL);
            yOffset += fieldHeight + spacing;
        }

        // Метки и поля для стоимости перевозки
        yOffset += spacing * 2;
        CreateWindow(L"STATIC", L"Стоимости перевозки:", WS_VISIBLE | WS_CHILD,
            xOffset, yOffset, 200, fieldHeight, hwnd, NULL, NULL, NULL);
        yOffset += fieldHeight + spacing;

        for (int i = 0; i < g_rows; i++) {
            for (int j = 0; j < g_cols; j++) {
                wchar_t label[50];
                swprintf_s(label, 50, L"Поставщик %d -> Потребитель %d:", i + 1, j + 1);
                CreateWindow(L"STATIC", label, WS_VISIBLE | WS_CHILD,
                    xOffset, yOffset, 200, fieldHeight, hwnd, NULL, NULL, NULL);
                costEdits[i][j] = CreateWindow(L"EDIT", L"0", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
                    xOffset + 210, yOffset, fieldWidth, fieldHeight, hwnd,
                    (HMENU)(EDIT_COST_BASE + i * g_cols + j), NULL, NULL);
                yOffset += fieldHeight + spacing;
            }
        }

        // Кнопка сохранения данных
        yOffset += spacing;
        CreateWindow(L"BUTTON", L"Сохранить данные", WS_VISIBLE | WS_CHILD,
            xOffset, yOffset, 200, 30, hwnd, (HMENU)BUTTON_SAVE_DATA, NULL, NULL);
        // Кнопка закрытия диалога
        CreateWindow(L"BUTTON", L"Закрыть", WS_VISIBLE | WS_CHILD,
            xOffset + 210, yOffset, 100, 30, hwnd, (HMENU)BUTTON_CLOSE_DIALOG, NULL, NULL);

        // Устанавливаем размер окна и прокрутку
        int totalHeight = yOffset + 50 + 50;
        int windowHeight = totalHeight > maxVisibleHeight ? maxVisibleHeight : totalHeight;
        SetWindowPos(hwnd, NULL, 0, 0, 600, windowHeight, SWP_NOMOVE | SWP_NOZORDER);
        if (totalHeight > maxVisibleHeight) {
            SCROLLINFO si = { sizeof(si), SIF_RANGE | SIF_PAGE, 0, totalHeight, maxVisibleHeight, 0, FALSE };
            SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
        }
        break;
    }
    case WM_VSCROLL: {
        SCROLLINFO si;
        si.cbSize = sizeof(si);
        si.fMask = SIF_ALL;
        GetScrollInfo(hwnd, SB_VERT, &si);

        int yPos = si.nPos;
        switch (LOWORD(wParam)) {
        case SB_LINEUP: yPos -= 10; break;
        case SB_LINEDOWN: yPos += 10; break;
        case SB_PAGEUP: yPos -= si.nPage; break;
        case SB_PAGEDOWN: yPos += si.nPage; break;
        case SB_THUMBTRACK: yPos = HIWORD(wParam); break;
        }

        if (yPos < si.nMin) yPos = si.nMin;
        if (yPos > si.nMax - (int)si.nPage) yPos = si.nMax - (int)si.nPage;

        if (yPos != si.nPos) {
            SetScrollPos(hwnd, SB_VERT, yPos, TRUE);
            ScrollWindow(hwnd, 0, si.nPos - yPos, NULL, NULL);
        }
        break;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case BUTTON_SAVE_DATA: {
            // Сохранение данных из полей ввода
            if (g_supply) free(g_supply);
            if (g_demand) free(g_demand);
            if (g_cost) free_matrix(g_cost, g_rows);

            g_supply = (int*)malloc(g_rows * sizeof(int));
            g_demand = (int*)malloc(g_cols * sizeof(int));
            g_cost = allocate_matrix(g_rows, g_cols);
            if (!g_supply || !g_demand || !g_cost) {
                MessageBox(GetParent(hwnd), L"Не удалось выделить память для данных.", L"Ошибка", MB_ICONERROR);
                if (g_supply) free(g_supply);
                if (g_demand) free(g_demand);
                if (g_cost) free_matrix(g_cost, g_rows);
                g_supply = NULL;
                g_demand = NULL;
                g_cost = NULL;
                break;
            }

            int valid_data = 0;
            // Чтение запасов
            for (int i = 0; i < g_rows; i++) {
                wchar_t text[10];
                GetWindowText(supplyEdits[i], text, 10);
                g_supply[i] = _wtoi(text);
                if (g_supply[i] < 0) g_supply[i] = 0;
                if (g_supply[i] > 0) valid_data = 1;
            }

            // Чтение потребностей
            for (int j = 0; j < g_cols; j++) {
                wchar_t text[10];
                GetWindowText(demandEdits[j], text, 10);
                g_demand[j] = _wtoi(text);
                if (g_demand[j] < 0) g_demand[j] = 0;
                if (g_demand[j] > 0) valid_data = 1;
            }

            // Чтение стоимостей
            for (int i = 0; i < g_rows; i++) {
                for (int j = 0; j < g_cols; j++) {
                    wchar_t text[10];
                    GetWindowText(costEdits[i][j], text, 10);
                    g_cost[i][j] = _wtoi(text);
                    if (g_cost[i][j] < 0) g_cost[i][j] = 0;
                }
            }

            if (!valid_data) {
                MessageBox(GetParent(hwnd), L"Введите хотя бы одно значение запасов или потребностей больше 0!",
                    L"Ошибка", MB_ICONERROR);
                // Не закрываем окно, просто показываем ошибку
                break;  
            }

            MessageBox(GetParent(hwnd), L"Данные успешно сохранены!", L"Успех", MB_ICONINFORMATION);
            // Окно не закрывается, пользователь может продолжить редактирование
            break;
        }
        case BUTTON_CLOSE_DIALOG:
            // Закрываем окно при нажатии на кнопку "Закрыть"
            DestroyWindow(hwnd);
            break;
        }
        break;

    case WM_CLOSE:
        // Освобождение памяти для указателей на элементы управления, если не освобождены
        if (arraysInitialized) {
            if (supplyEdits) free(supplyEdits);
            if (demandEdits) free(demandEdits);
            if (costEdits) {
                for (int i = 0; i < g_rows; i++) if (costEdits[i]) free(costEdits[i]);
                free(costEdits);
            }
            arraysInitialized = 0;
        }
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        // Не вызываем PostQuitMessage(0), чтобы не завершать приложение
        // Включаем родительское окно обратно
        EnableWindow(GetParent(hwnd), TRUE);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
