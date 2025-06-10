#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Идентификаторы элементов управления
#define BUTTON_SAVE_TO_FILE 103
#define BUTTON_LOAD_FROM_FILE 104
#define EDIT_ROWS 100
#define EDIT_COLS 101
#define BUTTON_SOLVE_NW 200
#define BUTTON_SOLVE_LC 201
#define BUTTON_SHOW_DATA 206
#define BUTTON_EXIT 202
#define BUTTON_RESET 204
#define BUTTON_RANDOM_DATA 205
#define BUTTON_CONFIRM_INPUT 207
#define BUTTON_TRUCK_DISTRIBUTION 208
#define STATIC_RESULT 300
#define BUTTON_INPUT_DATA 203
#define BUTTON_SAVE_DATA 204
#define EDIT_SUPPLY_BASE 1000
#define EDIT_DEMAND_BASE 2000
#define EDIT_COST_BASE 3000
#define BUTTON_CLOSE_DIALOG 4001

// Глобальные переменные для хранения размеров и данных
int g_rows = 0, g_cols = 0;
int* g_supply = NULL;
int* g_demand = NULL;
int** g_cost = NULL;
int** g_allocation = NULL;
HINSTANCE g_hInstance = NULL;
int g_truck_count = 0;
int* g_truck_capacity = NULL;

// Прототипы функций
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK DataDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
bool load_truck_data_from_file(HWND hwnd, const wchar_t* filePath);
void distribute_trucks(HWND hwnd);
int** allocate_matrix(int rows, int cols);
void free_matrix(int** matrix, int rows);
void load_data_from_file(HWND hwnd, const wchar_t* filePath);
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
    RegisterClass(&wc);

    // Регистрируем класс для диалогового окна ввода данных
    WNDCLASS dlgWc = { 0 };
    dlgWc.lpfnWndProc = DataDlgProc;
    dlgWc.hInstance = hInstance;
    dlgWc.lpszClassName = DIALOG_CLASS_NAME;
    RegisterClass(&dlgWc);

    // Создаем главное окно
    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Transport Solver",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Цикл обработки сообщений
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        // Создаем элементы управления в главном окне
        CreateWindow(L"STATIC", L"Количество поставщиков:", WS_VISIBLE | WS_CHILD,
            20, 20, 150, 25, hwnd, NULL, NULL, NULL);
        CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
            180, 20, 50, 25, hwnd, (HMENU)EDIT_ROWS, NULL, NULL);
        CreateWindow(L"STATIC", L"Количество потребителей:", WS_VISIBLE | WS_CHILD,
            250, 20, 150, 25, hwnd, NULL, NULL, NULL);
        CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
            410, 20, 50, 25, hwnd, (HMENU)EDIT_COLS, NULL, NULL);
        // Добавляем кнопку "Подтвердить данные"
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
        CreateWindow(L"BUTTON", L"Распределение по фурам", WS_VISIBLE | WS_CHILD,
            20, 340, 200, 30, hwnd, (HMENU)BUTTON_TRUCK_DISTRIBUTION, NULL, NULL);
        CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_READONLY,
            250, 60, 520, 480, hwnd, (HMENU)STATIC_RESULT, NULL, NULL);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case BUTTON_CONFIRM_INPUT: {
            wchar_t rowsText[10], colsText[10];
            GetWindowText(GetDlgItem(hwnd, EDIT_ROWS), rowsText, 10);
            GetWindowText(GetDlgItem(hwnd, EDIT_COLS), colsText, 10);
            g_rows = _wtoi(rowsText);
            g_cols = _wtoi(colsText);

            if (g_rows <= 0 || g_cols <= 0) {
                MessageBox(hwnd, L"Введите корректные положительные значения для поставщиков и потребителей.",
                    L"Ошибка", MB_ICONERROR);
            }
            break;
        }
        case BUTTON_TRUCK_DISTRIBUTION: {
            if (!g_allocation) {
                MessageBox(hwnd, L"Сначала выполните распределение грузов с помощью одного из методов!", L"Ошибка", MB_ICONERROR);
                break;
            }

            OPENFILENAME ofn = { 0 };
            wchar_t filePath[MAX_PATH] = L"";
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
            ofn.lpstrFile = filePath;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
            ofn.lpstrTitle = L"Выберите файл с данными о фурах";

            if (GetOpenFileName(&ofn)) {
                // Загружаем данные из файла и проверяем успех
                if (load_truck_data_from_file(hwnd, filePath)) {
                    // Если загрузка успешна, вызываем распределение
                    distribute_trucks(hwnd);
                }
                if (GetOpenFileName(&ofn)) {
                    // Загружаем данные из файла и проверяем успех
                    if (load_truck_data_from_file(hwnd, filePath)) {
                        // Если загрузка успешна, вызываем распределение
                        distribute_trucks(hwnd);
                    }
                }
            }
            break;
        }

        case BUTTON_INPUT_DATA: {
            // Открываем диалог для выбора текстового файла
            OPENFILENAME ofn = { 0 };
            wchar_t filePath[MAX_PATH] = L"";
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
            ofn.lpstrFile = filePath;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

            if (GetOpenFileName(&ofn)) load_data_from_file(hwnd, filePath);
            break;
        }

        case BUTTON_RANDOM_DATA: {
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

            // Показываем сообщение о подтверждении данных
            int result = MessageBox(hwnd, L"Подтвердить введенные размеры?",
                L"Подтверждение", MB_YESNO | MB_ICONQUESTION);
            if (result == IDYES) {
                // Освобождаем память от предыдущих данных, если она была выделена
                if (g_supply) free(g_supply);
                if (g_demand) free(g_demand);
                if (g_cost) free_matrix(g_cost, g_rows);
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

                // Генерируем случайные данные
                for (int i = 0; i < g_rows; i++) {
                    g_supply[i] = (rand() % 91) + 10;
                }
                for (int j = 0; j < g_cols; j++) {
                    g_demand[j] = (rand() % 91) + 10;
                }
                for (int i = 0; i < g_rows; i++) {
                    for (int j = 0; j < g_cols; j++) {
                        g_cost[i][j] = (rand() % 91) + 10;
                    }
                }

            }
            break;
        }
        case BUTTON_SHOW_DATA: {
            if (!g_supply || !g_demand || !g_cost) {
                MessageBox(hwnd, L"Нет данных для отображения. Сначала введите или сгенерируйте данные.", L"Ошибка", MB_ICONERROR);
                break;
            }

            // Формируем путь к папке Results
            wchar_t folderPath[256] = L"Results";
            wchar_t filePath[256];
            SYSTEMTIME st;
            GetSystemTime(&st);

            if (GetFileAttributes(folderPath) == INVALID_FILE_ATTRIBUTES) {
                if (!CreateDirectory(folderPath, NULL)) {
                    MessageBox(hwnd, L"Не удалось создать папку Results. Файл будет сохранен в текущей директории.", L"Предупреждение", MB_ICONWARNING);
                    swprintf_s(filePath, 256, L"data_display_%04d%02d%02d_%02d%02d%02d.html",
                        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
                }
                else {
                    swprintf_s(filePath, 256, L"%ls\\data_display_%04d%02d%02d_%02d%02d%02d.html",
                        folderPath, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
                }
            }
            else {
                swprintf_s(filePath, 256, L"%ls\\data_display_%04d%02d%02d_%02d%02d%02d.html",
                    folderPath, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
            }

            // Открываем файл для записи
            FILE* fp;
            if (_wfopen_s(&fp, filePath, L"w, ccs=UTF-8") != 0 || !fp) {
                MessageBox(hwnd, L"Не удалось создать HTML-файл для записи данных.", L"Ошибка", MB_ICONERROR);
                break;
            }

            // Формируем HTML-код
            fwprintf(fp, L"<!DOCTYPE html>\n");
            fwprintf(fp, L"<html lang=\"ru\">\n");
            fwprintf(fp, L"<head>\n");
            fwprintf(fp, L"    <meta charset=\"UTF-8\">\n");
            fwprintf(fp, L"    <title>Данные транспортной задачи</title>\n");
            fwprintf(fp, L"    <style>\n");
            fwprintf(fp, L"        body { font-family: Arial, sans-serif; margin: 20px; }\n");
            fwprintf(fp, L"        h1, h2 { color: #333; }\n");
            fwprintf(fp, L"        table { border-collapse: collapse; width: auto; margin-top: 20px; }\n");
            fwprintf(fp, L"        th, td { border: 1px solid #999; padding: 8px; text-align: center; }\n");
            fwprintf(fp, L"        th { background-color: #f2f2f2; }\n");
            fwprintf(fp, L"    </style>\n");
            fwprintf(fp, L"</head>\n");
            fwprintf(fp, L"<body>\n");
            fwprintf(fp, L"    <h1>Данные транспортной задачи</h1>\n");

            // Таблица для запасов поставщиков
            fwprintf(fp, L"    <h2>Запасы поставщиков</h2>\n");
            fwprintf(fp, L"    <table>\n");
            fwprintf(fp, L"        <thead>\n");
            fwprintf(fp, L"            <tr>\n");
            fwprintf(fp, L"                <th>Поставщик</th>\n");
            fwprintf(fp, L"                <th>Запас</th>\n");
            fwprintf(fp, L"            </tr>\n");
            fwprintf(fp, L"        </thead>\n");
            fwprintf(fp, L"        <tbody>\n");
            for (int i = 0; i < g_rows; i++) {
                fwprintf(fp, L"            <tr>\n");
                fwprintf(fp, L"                <td>Поставщик %d</td>\n", i + 1);
                fwprintf(fp, L"                <td>%d</td>\n", g_supply[i]);
                fwprintf(fp, L"            </tr>\n");
            }
            fwprintf(fp, L"        </tbody>\n");
            fwprintf(fp, L"    </table>\n");

            // Таблица для потребностей потребителей
            fwprintf(fp, L"    <h2>Потребности потребителей</h2>\n");
            fwprintf(fp, L"    <table>\n");
            fwprintf(fp, L"        <thead>\n");
            fwprintf(fp, L"            <tr>\n");
            fwprintf(fp, L"                <th>Потребитель</th>\n");
            fwprintf(fp, L"                <th>Потребность</th>\n");
            fwprintf(fp, L"            </tr>\n");
            fwprintf(fp, L"        </thead>\n");
            fwprintf(fp, L"        <tbody>\n");
            for (int j = 0; j < g_cols; j++) {
                fwprintf(fp, L"            <tr>\n");
                fwprintf(fp, L"                <td>Потребитель %d</td>\n", j + 1);
                fwprintf(fp, L"                <td>%d</td>\n", g_demand[j]);
                fwprintf(fp, L"            </tr>\n");
            }
            fwprintf(fp, L"        </tbody>\n");
            fwprintf(fp, L"    </table>\n");

            // Таблица для стоимостей перевозки
            fwprintf(fp, L"    <h2>Стоимости перевозки</h2>\n");
            fwprintf(fp, L"    <table>\n");
            fwprintf(fp, L"        <thead>\n");
            fwprintf(fp, L"            <tr>\n");
            fwprintf(fp, L"                <th></th>\n"); // Пустая ячейка в углу
            for (int j = 0; j < g_cols; j++) {
                fwprintf(fp, L"                <th>Потребитель %d</th>\n", j + 1);
            }
            fwprintf(fp, L"            </tr>\n");
            fwprintf(fp, L"        </thead>\n");
            fwprintf(fp, L"        <tbody>\n");
            for (int i = 0; i < g_rows; i++) {
                fwprintf(fp, L"            <tr>\n");
                fwprintf(fp, L"                <th>Поставщик %d</th>\n", i + 1);
                for (int j = 0; j < g_cols; j++) {
                    fwprintf(fp, L"                <td>%d</td>\n", g_cost[i][j]);
                }
                fwprintf(fp, L"            </tr>\n");
            }
            fwprintf(fp, L"        </tbody>\n");
            fwprintf(fp, L"    </table>\n");

            fwprintf(fp, L"</body>\n");
            fwprintf(fp, L"</html>\n");

            // Закрываем файл
            fclose(fp);

            // Сообщаем пользователю, что файл создан
            wchar_t msg[512];
            swprintf_s(msg, 512, L"Данные сохранены в файл: %ls", filePath);

            // Открываем файл в браузере
            ShellExecute(NULL, L"open", filePath, NULL, NULL, SW_SHOWNORMAL);
            break;
        }

        case BUTTON_RESET: {
            // Освобождаем память
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
            // Очищаем область результатов
            SetWindowText(GetDlgItem(hwnd, STATIC_RESULT), L"");
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
            print_results(hwnd, g_allocation, g_rows, g_cols, L"Метод минимальной стоимости", total_cost);
            break;
        }
        case BUTTON_EXIT:
            PostQuitMessage(0);
            break;
        }
        break;

    case WM_DESTROY:
        // Освобождаем память при уничтожении окна
        if (g_supply) free(g_supply);
        if (g_demand) free(g_demand);
        if (g_cost) free_matrix(g_cost, g_rows);
        if (g_allocation) free_matrix(g_allocation, g_rows);
        if (g_truck_capacity) free(g_truck_capacity);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK DataDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND* supplyEdits = NULL;
    static HWND* demandEdits = NULL;
    static HWND** costEdits = NULL;
    static int arraysInitialized = 0;

    switch (uMsg) {
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case BUTTON_SAVE_DATA: {
            if (g_supply) free(g_supply);
            if (g_demand) free(g_demand);
            if (g_cost) free_matrix(g_cost, g_rows);
            g_supply = (int*)malloc(g_rows * sizeof(int));
            g_demand = (int*)malloc(g_cols * sizeof(int));
            g_cost = allocate_matrix(g_rows, g_cols);
            if (!g_supply || !g_demand || !g_cost) {
                MessageBox(hwnd, L"Ошибка выделения памяти", L"Ошибка", MB_ICONERROR);
                break;
            }
            for (int i = 0; i < g_rows; i++) {
                wchar_t text[10];
                GetWindowText(supplyEdits[i], text, 10);
                g_supply[i] = _wtoi(text);
            }
            for (int j = 0; j < g_cols; j++) {
                wchar_t text[10];
                GetWindowText(demandEdits[j], text, 10);
                g_demand[j] = _wtoi(text);
            }
            for (int i = 0; i < g_rows; i++) {
                for (int j = 0; j < g_cols; j++) {
                    wchar_t text[10];
                    GetWindowText(costEdits[i][j], text, 10);
                    g_cost[i][j] = _wtoi(text);
                }
            }
            MessageBox(hwnd, L"Данные сохранены!", L"Успех", MB_ICONINFORMATION);
            break;
        }
        case BUTTON_CLOSE_DIALOG:
            EnableWindow(GetParent(hwnd), TRUE);
            DestroyWindow(hwnd);
            break;
        }
        break;
    case WM_DESTROY:
        if (arraysInitialized) {
            for (int i = 0; i < g_rows; i++) {
                free(costEdits[i]);
            }
            free(costEdits);
            free(supplyEdits);
            free(demandEdits);
            arraysInitialized = 0;
        }
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void load_data_from_file(HWND hwnd, const wchar_t* filePath) {
    FILE* fp = _wfopen(filePath, L"r");
    if (!fp) {
        MessageBox(hwnd, L"Не удалось открыть файл.", L"Ошибка", MB_ICONERROR);
        return;
    }

    // Читаем размеры из файла
    fscanf_s(fp, "%d %d", &g_rows, &g_cols);
    if (g_rows <= 0 || g_cols <= 0) {
        MessageBox(hwnd, L"Некорректные размеры в файле.", L"Ошибка", MB_ICONERROR);
        fclose(fp);
        return;
    }

    // Выделяем память
    if (g_supply) free(g_supply);
    if (g_demand) free(g_demand);
    if (g_cost) free_matrix(g_cost, g_rows);
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

    // Читаем данные для g_supply, g_demand, g_cost из файла
    for (int i = 0; i < g_rows; i++) {
        fscanf_s(fp, "%d", &g_supply[i]);
    }
    for (int j = 0; j < g_cols; j++) {
        fscanf_s(fp, "%d", &g_demand[j]);
    }
    for (int i = 0; i < g_rows; i++) {
        for (int j = 0; j < g_cols; j++) {
            fscanf_s(fp, "%d", &g_cost[i][j]);
        }
    }

    fclose(fp);
}

// Показать диалоговое окно для ввода данных
void show_data_input_dialog(HWND parent) {
    int dialogWidth = 600;
    int dialogHeight = (g_rows * 30 + g_cols * 30 + 100) > 400 ? (g_rows * 30 + g_cols * 30 + 100) : 400;
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

    EnableWindow(parent, FALSE);
    ShowWindow(hwndDialog, SW_SHOW);
    UpdateWindow(hwndDialog);

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!IsDialogMessage(hwndDialog, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (!IsWindow(hwndDialog)) {
            EnableWindow(parent, TRUE);
            break;
        }
    }
}

// Выделение памяти под матрицу
int** allocate_matrix(int rows, int cols) {
    int** matrix = (int**)malloc(rows * sizeof(int*));
    if (!matrix) return NULL;
    for (int i = 0; i < rows; i++) {
        matrix[i] = (int*)malloc(cols * sizeof(int));
        if (!matrix[i]) {
            for (int j = 0; j < i; j++) free(matrix[j]);
            free(matrix);
            return NULL;
        }
    }
    return matrix;
}

// Освобождение памяти матрицы
void free_matrix(int** matrix, int rows) {
    if (!matrix) return;
    for (int i = 0; i < rows; i++) {
        if (matrix[i]) free(matrix[i]);
    }
    free(matrix);
}

// Метод Северо-Западного угла
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

// Метод минимальной стоимости
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

void print_results(HWND hwnd, int** allocation, int rows, int cols, const wchar_t* method, long long total_cost) {
    // Формируем путь к папке Results
    wchar_t folderPath[256] = L"Results";
    wchar_t filePath[256];
    SYSTEMTIME st;
    GetSystemTime(&st);

    if (GetFileAttributes(folderPath) == INVALID_FILE_ATTRIBUTES) {
        if (!CreateDirectory(folderPath, NULL)) {
            MessageBox(hwnd, L"Не удалось создать папку Results. Файл будет сохранен в текущей директории.", L"Предупреждение", MB_ICONWARNING);
            swprintf_s(filePath, 256, L"result_%04d%02d%02d_%02d%02d%02d.html",
                st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
        }
        else {
            swprintf_s(filePath, 256, L"%ls\\result_%04d%02d%02d_%02d%02d%02d.html",
                folderPath, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
        }
    }
    else {
        swprintf_s(filePath, 256, L"%ls\\result_%04d%02d%02d_%02d%02d%02d.html",
            folderPath, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    }

    // Открываем файл для записи
    FILE* fp;
    if (_wfopen_s(&fp, filePath, L"w, ccs=UTF-8") != 0 || !fp) {
        MessageBox(hwnd, L"Не удалось создать HTML-файл для записи результатов.", L"Ошибка", MB_ICONERROR);
        return;
    }

    // Формируем HTML-код
    fwprintf(fp, L"<!DOCTYPE html>\n");
    fwprintf(fp, L"<html lang=\"ru\">\n");
    fwprintf(fp, L"<head>\n");
    fwprintf(fp, L"    <meta charset=\"UTF-8\">\n");
    fwprintf(fp, L"    <title>Результаты транспортной задачи</title>\n");
    fwprintf(fp, L"    <style>\n");
    fwprintf(fp, L"        body { font-family: Arial, sans-serif; margin: 20px; }\n");
    fwprintf(fp, L"        h1 { color: #333; }\n");
    fwprintf(fp, L"        table { border-collapse: collapse; width: auto; margin-top: 20px; }\n");
    fwprintf(fp, L"        th, td { border: 1px solid #999; padding: 8px; text-align: center; }\n");
    fwprintf(fp, L"        th { background-color: #f2f2f2; }\n");
    fwprintf(fp, L"        .total-cost { margin-top: 20px; font-weight: bold; }\n");
    fwprintf(fp, L"    </style>\n");
    fwprintf(fp, L"</head>\n");
    fwprintf(fp, L"<body>\n");
    fwprintf(fp, L"    <h1>Результаты транспортной задачи (%ls)</h1>\n", method);

    // Создаем таблицу для матрицы распределения
    fwprintf(fp, L"    <table>\n");
    fwprintf(fp, L"        <thead>\n");
    fwprintf(fp, L"            <tr>\n");
    fwprintf(fp, L"                <th></th>\n"); // Пустая ячейка в углу
    for (int j = 0; j < cols; j++) {
        fwprintf(fp, L"                <th>Потребитель %d</th>\n", j + 1);
    }
    fwprintf(fp, L"            </tr>\n");
    fwprintf(fp, L"        </thead>\n");
    fwprintf(fp, L"        <tbody>\n");
    for (int i = 0; i < rows; i++) {
        fwprintf(fp, L"            <tr>\n");
        fwprintf(fp, L"                <th>Поставщик %d</th>\n", i + 1);
        for (int j = 0; j < cols; j++) {
            fwprintf(fp, L"                <td>%d</td>\n", allocation[i][j]);
        }
        fwprintf(fp, L"            </tr>\n");
    }
    fwprintf(fp, L"        </tbody>\n");
    fwprintf(fp, L"    </table>\n");

    // Выводим общую стоимость
    fwprintf(fp, L"    <div class=\"total-cost\">Общая стоимость: %lld</div>\n", total_cost);
    fwprintf(fp, L"</body>\n");
    fwprintf(fp, L"</html>\n");

    // Закрываем файл
    fclose(fp);

    // Сообщаем пользователю, что файл создан
    wchar_t msg[512];
    swprintf_s(msg, 512, L"Результаты сохранены в файл: %ls", filePath);

    // Открываем файл в браузере
    ShellExecute(NULL, L"open", filePath, NULL, NULL, SW_SHOWNORMAL);

    // Очищаем текстовое поле в программе
    SetWindowText(GetDlgItem(hwnd, STATIC_RESULT), L"Результаты сохранены в HTML-файл. Откройте его в браузере для просмотра.");
}

// Проверка баланса между запасами и потребностями
int check_balance(int* supply, int* demand, int rows, int cols) {
    long long total_supply = 0, total_demand = 0;
    for (int i = 0; i < rows; i++) total_supply += supply[i];
    for (int j = 0; j < cols; j++) total_demand += demand[j];
    return total_supply == total_demand;
}

void show_truck_input_dialog(HWND parent) {
    int dialogWidth = 400;
    int dialogHeight = 300;
    // Динамически вычисляем высоту окна в зависимости от количества полей
    int initialCount = (g_truck_count > 0) ? g_truck_count : 10; // Учитываем текущее количество фур
    dialogHeight = 100 + initialCount * 35; // Учитываем поля ввода и отступы
    HWND hwndDialog = CreateWindowEx(
        WS_EX_DLGMODALFRAME,
        L"TruckInputDialog",
        L"Ввод данных о фурах",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VSCROLL, // Добавлен WS_VSCROLL для прокрутки
        CW_USEDEFAULT, CW_USEDEFAULT, dialogWidth, dialogHeight,
        parent, NULL, g_hInstance, NULL
    );

    if (!hwndDialog) {
        MessageBox(parent, L"Не удалось создать окно ввода данных о фурах.", L"Ошибка", MB_ICONERROR);
        return;
    }

    EnableWindow(parent, FALSE);
    ShowWindow(hwndDialog, SW_SHOW);
    UpdateWindow(hwndDialog);

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!IsDialogMessage(hwndDialog, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (!IsWindow(hwndDialog)) {
            EnableWindow(parent, TRUE);
            break;
        }
    }
}

bool load_truck_data_from_file(HWND hwnd, const wchar_t* filePath) {
    FILE* fp;
    if (_wfopen_s(&fp, filePath, L"r") != 0) {
        MessageBox(hwnd, L"Не удалось открыть файл.", L"Ошибка", MB_ICONERROR);
        return false;
    }

    wchar_t line[100]; // Буфер для чтения строк

    // Читаем первую строку для количества фур
    if (!fgetws(line, 100, fp)) {
        fclose(fp);
        MessageBox(hwnd, L"Ошибка чтения количества фур из файла.", L"Ошибка", MB_ICONERROR);
        return false;
    }
    // Удаляем перевод строки, если есть
    line[wcscspn(line, L"\n")] = 0;
    line[wcscspn(line, L"\r")] = 0; // Для совместимости с разными системами

    g_truck_count = _wtoi(line); // Преобразуем в целое число
    if (g_truck_count <= 0) {
        fclose(fp);
        MessageBox(hwnd, L"Количество фур должно быть положительным числом.", L"Ошибка", MB_ICONERROR);
        return false;
    }

    // Освобождаем существующую память, если она есть
    if (g_truck_capacity) {
        free(g_truck_capacity);
        g_truck_capacity = NULL;
    }

    // Выделяем память для массива вместимостей
    g_truck_capacity = (int*)malloc(g_truck_count * sizeof(int));
    if (!g_truck_capacity) {
        fclose(fp);
        MessageBox(hwnd, L"Ошибка выделения памяти для данных о фурах.", L"Ошибка", MB_ICONERROR);
        g_truck_count = 0;
        return false;
    }

    // Читаем каждую строку для вместимости фур
    for (int i = 0; i < g_truck_count; i++) {
        if (!fgetws(line, 100, fp)) {
            fclose(fp);
            free(g_truck_capacity);
            g_truck_capacity = NULL;
            g_truck_count = 0;
            MessageBox(hwnd, L"Ошибка чтения данных о вместимости фур.", L"Ошибка", MB_ICONERROR);
            return false;
        }
        // Удаляем перевод строки
        line[wcscspn(line, L"\n")] = 0;
        line[wcscspn(line, L"\r")] = 0;

        int capacity = _wtoi(line); // Преобразуем в целое число
        if (capacity <= 0) {
            fclose(fp);
            free(g_truck_capacity);
            g_truck_capacity = NULL;
            g_truck_count = 0;
            MessageBox(hwnd, L"Вместимость фур должна быть положительным числом.", L"Ошибка", MB_ICONERROR);
            return false;
        }
        g_truck_capacity[i] = capacity;
    }

    fclose(fp);
    MessageBox(hwnd, L"Данные о фурах успешно загружены!", L"Успех", MB_ICONINFORMATION);
    return true; // Возвращаем true при успешной загрузке
}

void distribute_trucks(HWND hwnd) {
    if (g_truck_count == 0 || !g_truck_capacity || !g_allocation) {
        MessageBox(hwnd, L"Нет данных о фурах или распределении грузов.", L"Ошибка", MB_ICONERROR);
        return;
    }

    // Формируем путь к папке Results
    wchar_t folderPath[256] = L"Results";
    wchar_t filePath[256];
    SYSTEMTIME st;
    GetSystemTime(&st);

    if (GetFileAttributes(folderPath) == INVALID_FILE_ATTRIBUTES) {
        if (!CreateDirectory(folderPath, NULL)) {
            MessageBox(hwnd, L"Не удалось создать папку Results. Файл будет сохранен в текущей директории.", L"Предупреждение", MB_ICONWARNING);
            swprintf_s(filePath, 256, L"truck_distribution_%04d%02d%02d_%02d%02d%02d.html",
                st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
        }
        else {
            swprintf_s(filePath, 256, L"%ls\\truck_distribution_%04d%02d%02d_%02d%02d%02d.html",
                folderPath, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
        }
    }
    else {
        swprintf_s(filePath, 256, L"%ls\\truck_distribution_%04d%02d%02d_%02d%02d%02d.html",
            folderPath, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    }

    // Открываем файл для записи
    FILE* fp;
    if (_wfopen_s(&fp, filePath, L"w, ccs=UTF-8") != 0 || !fp) {
        MessageBox(hwnd, L"Не удалось создать HTML-файл для записи распределения по фурам.", L"Ошибка", MB_ICONERROR);
        return;
    }

    // Формируем HTML-код
    fwprintf(fp, L"<!DOCTYPE html>\n");
    fwprintf(fp, L"<html lang=\"ru\">\n");
    fwprintf(fp, L"<head>\n");
    fwprintf(fp, L"    <meta charset=\"UTF-8\">\n");
    fwprintf(fp, L"    <title>Распределение груза по фурам</title>\n");
    fwprintf(fp, L"    <style>\n");
    fwprintf(fp, L"        body { font-family: Arial, sans-serif; margin: 20px; }\n");
    fwprintf(fp, L"        h1 { color: #333; }\n");
    fwprintf(fp, L"        table { border-collapse: collapse; width: auto; margin-top: 20px; }\n");
    fwprintf(fp, L"        th, td { border: 1px solid #999; padding: 8px; text-align: center; }\n");
    fwprintf(fp, L"        th { background-color: #f2f2f2; }\n");
    fwprintf(fp, L"    </style>\n");
    fwprintf(fp, L"</head>\n");
    fwprintf(fp, L"<body>\n");
    fwprintf(fp, L"    <h1>Распределение груза по фурам</h1>\n");

    // Таблица с информацией о фурах
    fwprintf(fp, L"    <h2>Информация о фурах</h2>\n");
    fwprintf(fp, L"    <table>\n");
    fwprintf(fp, L"        <thead>\n");
    fwprintf(fp, L"            <tr>\n");
    fwprintf(fp, L"                <th>Фура</th>\n");
    fwprintf(fp, L"                <th>Грузоподъемность</th>\n");
    fwprintf(fp, L"            </tr>\n");
    fwprintf(fp, L"        </thead>\n");
    fwprintf(fp, L"        <tbody>\n");
    for (int i = 0; i < g_truck_count; i++) {
        fwprintf(fp, L"            <tr>\n");
        fwprintf(fp, L"                <td>%d</td>\n", i + 1);
        fwprintf(fp, L"                <td>%d</td>\n", g_truck_capacity[i]);
        fwprintf(fp, L"            </tr>\n");
    }
    fwprintf(fp, L"        </tbody>\n");
    fwprintf(fp, L"    </table>\n");

    // Распределение по фурам
    fwprintf(fp, L"    <h2>Распределение груза по фурам</h2>\n");
    int truck_index = 0; // Индекс текущей доступной фуры
    for (int i = 0; i < g_rows; i++) {
        for (int j = 0; j < g_cols; j++) {
            if (g_allocation[i][j] > 0) {
                int remaining_load = g_allocation[i][j];
                fwprintf(fp, L"    <h3>Поставщик %d -> Потребитель %d (всего груза: %d)</h3>\n", i + 1, j + 1, remaining_load);
                fwprintf(fp, L"    <table>\n");
                fwprintf(fp, L"        <thead>\n");
                fwprintf(fp, L"            <tr>\n");
                fwprintf(fp, L"                <th>Фура</th>\n");
                fwprintf(fp, L"                <th>Перевозимый груз</th>\n");
                fwprintf(fp, L"            </tr>\n");
                fwprintf(fp, L"        </thead>\n");
                fwprintf(fp, L"        <tbody>\n");

                while (remaining_load > 0 && truck_index < g_truck_count) {
                    int load = (remaining_load < g_truck_capacity[truck_index]) ? remaining_load : g_truck_capacity[truck_index];
                    fwprintf(fp, L"            <tr>\n");
                    fwprintf(fp, L"                <td>%d</td>\n", truck_index + 1);
                    fwprintf(fp, L"                <td>%d</td>\n", load);
                    fwprintf(fp, L"            </tr>\n");
                    remaining_load -= load;
                    truck_index++;
                }

                if (remaining_load > 0) {
                    fwprintf(fp, L"            <tr>\n");
                    fwprintf(fp, L"                <td colspan=\"2\">Недостаточно фур для перевозки оставшегося груза (%d)</td>\n", remaining_load);
                    fwprintf(fp, L"            </tr>\n");
                }
                fwprintf(fp, L"        </tbody>\n");
                fwprintf(fp, L"    </table>\n");
            }
        }
    }

    if (truck_index < g_truck_count) {
        fwprintf(fp, L"    <p>Осталось неиспользованных фур: %d</p>\n", g_truck_count - truck_index);
    }

    fwprintf(fp, L"</body>\n");
    fwprintf(fp, L"</html>\n");

    // Закрываем файл
    fclose(fp);

    // Сообщаем пользователю, что файл создан
    wchar_t msg[512];
    swprintf_s(msg, 512, L"Результаты распределения по фурам сохранены в файл: %ls", filePath);
    MessageBox(hwnd, msg, L"Успех", MB_ICONINFORMATION);

    // Открываем файл в браузере
    ShellExecute(NULL, L"open", filePath, NULL, NULL, SW_SHOWNORMAL);

    // Очищаем текстовое поле в программе
    SetWindowText(GetDlgItem(hwnd, STATIC_RESULT), L"Результаты распределения по фурам сохранены в HTML-файл. Откройте его в браузере для просмотра.");
}
