#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>

#define N 6
#define M 6
#define MAX 256
#define PRIME 101
#define MAX_ITEMS 100
#define MAX_NAME_LENGTH 100

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point points[N * M];
    int length;
    int cost;
} Path;

typedef struct {
    char name[MAX_NAME_LENGTH];
    float price;
} Item;

typedef struct {
    char name[MAX_NAME_LENGTH];
    float price;
    int nutrition;
    int quantity;
    float value; // Nutrition-to-price ratio
} Product;

Product products[MAX_ITEMS];
int productCount;
float budget;
char item[MAX_NAME_LENGTH];

// Utility functions for maze and island problems
bool isValid(int x, int y, int maze[N][M], bool visited[N][M]) {
    return x >= 0 && x < N && y >= 0 && y < M && maze[x][y] == 1 && !visited[x][y];
}

void copyPath(Path* dest, Path* src) {
    dest->length = src->length;
    dest->cost = src->cost;
    for (int i = 0; i < src->length; i++) {
        dest->points[i] = src->points[i];
    }
}

void DFS(int maze[N][M], bool visited[N][M], Point current, Point end, Path* currentPath, Path* shortestPath) {
    if (current.x == end.x && current.y == end.y) {
        currentPath->points[currentPath->length++] = current;
        currentPath->cost++;
        if (currentPath->length < shortestPath->length) {
            copyPath(shortestPath, currentPath);
        }
        currentPath->length--;  // Backtrack
        currentPath->cost--;
        return;
    }

    visited[current.x][current.y] = true;
    currentPath->points[currentPath->length++] = current;
    currentPath->cost++;

    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};

    for (int i = 0; i < 4; i++) {
        int newX = current.x + dx[i];
        int newY = current.y + dy[i];

        if (isValid(newX, newY, maze, visited)) {
            DFS(maze, visited, (Point){newX, newY}, end, currentPath, shortestPath);
        }
    }

    currentPath->length--;  // Backtrack
    currentPath->cost--;
    visited[current.x][current.y] = false;
}

void findShortestPath(int maze[N][M], Point start, Point end) {
    bool visited[N][M] = {false};
    Path currentPath = { .length = 0, .cost = 0 };
    Path shortestPath = { .length = INT_MAX, .cost = 0 };

    DFS(maze, visited, start, end, &currentPath, &shortestPath);

    if (shortestPath.length == INT_MAX) {
        printf("No path found from (%d, %d) to (%d, %d)\n", start.x, start.y, end.x, end.y);
    } else {
        printf("Shortest path from (%d, %d) to (%d, %d):\n", start.x, start.y, end.x, end.y);
        for (int i = 0; i < shortestPath.length; i++) {
            printf("(%d, %d) ", shortestPath.points[i].x, shortestPath.points[i].y);
            if (i < shortestPath.length - 1) {
                printf("-> ");
            }
        }
        printf("\nTotal cost: %d\n", shortestPath.cost - 1);  // Subtract 1 to exclude the start point's cost
    }
}

void DFSIsland(int grid[N][M], int x, int y, bool visited[N][M]) {
    int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};

    visited[x][y] = true;

    for (int i = 0; i < 8; i++) {
        int newX = x + dx[i];
        int newY = y + dy[i];
        if (isValid(newX, newY, grid, visited)) {
            DFSIsland(grid, newX, newY, visited);
        }
    }
}

int countIslands(int grid[N][M]) {
    bool visited[N][M] = {false};
    int count = 0;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (grid[i][j] == 1 && !visited[i][j]) {
                DFSIsland(grid, i, j, visited);
                count++;
            }
        }
    }
    return count;
}

// Utility functions for item management
void readItemsFromFile(const char *filename, Item items[], int *count) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Unable to open file %s\n", filename);
        exit(1);
    }

    *count = 0;
    while (fscanf(file, "%s %f", items[*count].name, &items[*count].price) != EOF) {
        (*count)++;
    }

    fclose(file);
}

int searchItem(const Item items[], int count, const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(items[i].name, name) == 0) {
            return i; // Item found, return index
        }
    }
    return -1; // Item not found
}

void displayItem(const Item *item) {
    printf("Item found: %s, Price: %.2f\n", item->name, item->price);
}

int compareItemsByPrice(const void *a, const void *b) {
    float priceA = ((Item *)a)->price;
    float priceB = ((Item *)b)->price;
    return (priceA > priceB) - (priceA < priceB);
}

void sortItemsByPrice(Item items[], int count) {
    qsort(items, count, sizeof(Item), compareItemsByPrice);
}

void displayItems(const Item items[], int count) {
    printf("Items sorted by price (ascending order):\n");
    for (int i = 0; i < count; i++) {
        printf("%s: %.2f\n", items[i].name, items[i].price);
    }
}

int compareItemsByName(const void *a, const void *b) {
    return strcmp(((Item *)a)->name, ((Item *)b)->name);
}

void sortItemsByName(Item items[], int count) {
    qsort(items, count, sizeof(Item), compareItemsByName);
}

void displayItemsByName(const Item items[], int count) {
    printf("Items sorted by name (alphabetical order):\n");
    for (int i = 0; i < count; i++) {
        printf("%s: %.2f\n", items[i].name, items[i].price);
    }
}

void selectItem(const char *filename) {
    Item items[MAX_ITEMS];
    int itemCount;
    char searchName[MAX_NAME_LENGTH];
    bool process = true;
    while (process) {
        printf("How you want to see your product?\n");
        printf("1. According to price (ascending value)\n");
        printf("2. Alphabetical order by name\n");
        printf("3. Search the product\n");
        printf("4. Back\n");
        int choice;
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                readItemsFromFile(filename, items, &itemCount);
                sortItemsByPrice(items, itemCount);
                displayItems(items, itemCount);
                break;
            case 2:
                readItemsFromFile(filename, items, &itemCount);
                sortItemsByName(items, itemCount);
                displayItemsByName(items, itemCount);
                break;
            case 3:
                readItemsFromFile(filename, items, &itemCount);
                printf("Enter the name of the item to search: ");
                scanf("%s", searchName);
                int index = searchItem(items, itemCount, searchName);
                if (index != -1) {
                    displayItem(&items[index]);
                } else {
                    printf("Item '%s' not found.\n", searchName);
                }
                break;
            case 4:
                process = false;
                break;
            default:
                printf("Invalid choice.\n");
        }
    }
}

// Rabin-Karp algorithm for string matching
unsigned long calculate_hash(const char *str, int end) {
    unsigned long hash = 0;
    for (int i = 0; i <= end; i++) {
        hash = (hash * MAX + str[i]) % PRIME;
    }
    return hash;
}

void RabinKarp(const char *text, const char *pattern) {
    int n = strlen(text);
    int m = strlen(pattern);
    unsigned long pattern_hash = calculate_hash(pattern, m - 1);
    unsigned long text_hash = calculate_hash(text, m - 1);

    for (int i = 0; i <= n - m; i++) {
        if (pattern_hash == text_hash) {
            bool match = true;
            for (int j = 0; j < m; j++) {
                if (text[i + j] != pattern[j]) {
                    match = false;
                    break;
                }
            }
            if (match) {
                printf("Pattern found at index %d\n", i);
            }
        }
        if (i < n - m) {
            text_hash = (MAX * (text_hash - text[i] * MAX) + text[i + m]) % PRIME;
            if (text_hash < 0) {
                text_hash += PRIME;
            }
        }
    }
}

// Utility functions for budget management
int compareProductsByValue(const void *a, const void *b) {
    float valueA = ((Product *)a)->value;
    float valueB = ((Product *)b)->value;
    return (valueA > valueB) - (valueA < valueB);
}

void manageBudget(float budget) {
    qsort(products, productCount, sizeof(Product), compareProductsByValue);

    printf("Products selected within budget:\n");
    for (int i = 0; i < productCount && budget > 0; i++) {
        int quantity = budget / products[i].price;
        if (quantity > products[i].quantity) {
            quantity = products[i].quantity;
        }
        if (quantity > 0) {
            printf("%s - Quantity: %d, Price: %.2f, Total Cost: %.2f\n", products[i].name, quantity, products[i].price, quantity * products[i].price);
            budget -= quantity * products[i].price;
        }
    }

    if (budget > 0) {
        printf("Remaining budget: %.2f\n", budget);
    } else {
        printf("Budget exhausted.\n");
    }
}

void readProducts() {
    printf("Enter the number of products: ");
    scanf("%d", &productCount);

    for (int i = 0; i < productCount; i++) {
        printf("Enter the name of product %d: ", i + 1);
        scanf("%s", products[i].name);
        printf("Enter the price of product %d: ", i + 1);
        scanf("%f", &products[i].price);
        printf("Enter the nutrition of product %d: ", i + 1);
        scanf("%d", &products[i].nutrition);
        printf("Enter the quantity of product %d: ", i + 1);
        scanf("%d", &products[i].quantity);
        products[i].value = (float)products[i].nutrition / products[i].price;
    }

    printf("Enter your budget: ");
    scanf("%f", &budget);

    manageBudget(budget);
}

int main() {
    bool process = true;
    while (process) {
        printf("Welcome to the main menu!\n");
        printf("1. Find the shortest path in the maze\n");
        printf("2. Count islands in the grid\n");
        printf("3. Manage item list\n");
        printf("4. Rabin-Karp string matching\n");
        printf("5. Budget management\n");
        printf("6. Exit\n");
        int choice;
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                int maze[N][M] = {
                    {1, 0, 0, 0, 0, 0},
                    {1, 1, 0, 1, 1, 0},
                    {0, 1, 0, 1, 0, 0},
                    {0, 1, 0, 1, 0, 1},
                    {0, 1, 0, 1, 1, 1},
                    {0, 0, 0, 0, 0, 1}
                };
                Point start = {0, 0};
                Point end = {5, 5};
                findShortestPath(maze, start, end);
                break;
            }
            case 2: {
                int grid[N][M] = {
                    {1, 1, 0, 0, 0, 0},
                    {0, 1, 0, 1, 1, 0},
                    {0, 0, 0, 0, 0, 0},
                    {0, 1, 1, 0, 0, 0},
                    {0, 0, 0, 0, 1, 0},
                    {0, 1, 0, 0, 0, 0}
                };
                int islands = countIslands(grid);
                printf("Number of islands: %d\n", islands);
                break;
            }
            case 3:
                selectItem("items.txt");
                break;
            case 4: {
                char text[1000], pattern[100];
                printf("Enter the text: ");
                scanf("%s", text);
                printf("Enter the pattern to search: ");
                scanf("%s", pattern);
                RabinKarp(text, pattern);
                break;
            }
            case 5:
                readProducts();
                break;
            case 6:
                process = false;
                break;
            default:
                printf("Invalid choice.\n");
        }
    }
    return 0;
}

