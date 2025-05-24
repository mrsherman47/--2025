#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

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
    RegisterClass(&wc);

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
        CreateWindow(L"BUTTON", L"Подтвердить данные", WS_VISIBLE | WS_CHILD,
            470, 20, 150, 25, hwnd, (HMENU)BUTTON_CONFIRM_INPUT, NULL, NULL);
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
            else {
                MessageBox(hwnd, L"Данные о количестве поставщиков и потребителей подтверждены.",
                    L"Успех", MB_ICONINFORMATION);
            }
            break;
        }
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
        case BUTTON_RANDOM_DATA: {
            if (g_rows <= 0 || g_cols <= 0) {
                MessageBox(hwnd, L"Введите корректные значения для количества поставщиков и потребителей.", L"Ошибка", MB_ICONERROR);
                break;
            }
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
            for (int i = 0; i < g_rows; i++) {
                g_supply[i] = (rand() % 91) + 10;
            }
            for (int j = 0; j < g_cols; j++) {
                g_demand[j] = (rand() % 91) + 10;
            }
            for (int i = 0; i < g_rows; i++) {
                for (int j = 0; j < g_cols; j++) {
                    g_cost[i][j] = (rand() % 50) + 1;
                }
            }
            MessageBox(hwnd, L"Случайные данные успешно сгенерированы!", L"Успех", MB_ICONINFORMATION);
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
            MessageBox(hwnd, L"Все данные сброшены!", L"Успех", MB_ICONINFORMATION);
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
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

// Оконная процедура для диалогового окна ввода данных
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

        yOffset += spacing * 2;
        CreateWindow(L"STATIC", L"Стоимости перевозки:", WS_VISIBLE | WS_CHILD,
            xOffset, yOffset, 200, fieldHeight, hwnd, NULL, NULL, NULL);
        yOffset += fieldHeight + spacing;

        for (int i = 0; i < g_rows; i++) {
            for (int j = 0; j < g_cols; j++) {
                wchar_t label[50];
                swprintf_s(label, 50, L"П%d -> П%d:", i + 1, j + 1);
                CreateWindow(L"STATIC", label, WS_VISIBLE | WS_CHILD,
                    xOffset + j * (fieldWidth + spacing), yOffset + i * (fieldHeight + spacing), fieldWidth, fieldHeight, hwnd, NULL, NULL, NULL);
                costEdits[i][j] = CreateWindow(L"EDIT", L"0", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
                    xOffset + j * (fieldWidth + spacing) + fieldWidth, yOffset + i * (fieldHeight + spacing), fieldWidth, fieldHeight, hwnd, (HMENU)(EDIT_COST_BASE + i * g_cols + j), NULL, NULL);
            }
        }
        yOffset += g_rows * (fieldHeight + spacing) + spacing * 2;

        CreateWindow(L"BUTTON", L"Сохранить данные", WS_VISIBLE | WS_CHILD,
            xOffset, yOffset, 120, 30, hwnd, (HMENU)BUTTON_SAVE_DATA, NULL, NULL);
        CreateWindow(L"BUTTON", L"Закрыть", WS_VISIBLE | WS_CHILD,
            xOffset + 130, yOffset, 120, 30, hwnd, (HMENU)BUTTON_CLOSE_DIALOG, NULL, NULL);
        break;
    }
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

// Показать диалоговое окно для ввода данных
void show_data_input_dialog(HWND parent) {
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
    // Формируем уникальное имя файла на основе текущего времени
    wchar_t filename[256];
    SYSTEMTIME st;
    GetSystemTime(&st);
    swprintf_s(filename, 256, L"result_%04d%02d%02d_%02d%02d%02d.html",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

    // Открываем файл для записи
    FILE* fp;
    if (_wfopen_s(&fp, filename, L"w, ccs=UTF-8") != 0 || !fp) {
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
    swprintf_s(msg, 512, L"Результаты сохранены в файл: %ls", filename);
    MessageBox(hwnd, msg, L"Успех", MB_ICONINFORMATION);

    // Очищаем текстовое поле в программе (или оставляем пустым)
    SetWindowText(GetDlgItem(hwnd, STATIC_RESULT), L"Результаты сохранены в HTML-файл. Откройте его в браузере для просмотра.");
}

// Проверка баланса между запасами и потребностями
int check_balance(int* supply, int* demand, int rows, int cols) {
    long long total_supply = 0, total_demand = 0;
    for (int i = 0; i < rows; i++) total_supply += supply[i];
    for (int j = 0; j < cols; j++) total_demand += demand[j];
    return total_supply == total_demand;
}
