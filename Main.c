#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Constants
#define DISCOUNT_MIN_AGE 18
#define DISCOUNT_MAX_AGE 35
#define DISCOUNT_PERCENTAGE 0.2f
#define MEMBERSHIP_DISCOUNT_PERCENTAGE 0.5f
#define CUSTOMER_NAME_LENGTH 50

// Menu Option constants
#define MENU_OPTION_VIEW_CAR_STOCK 1
#define MENU_OPTION_BUY_CARS 2
#define MENU_OPTION_SELL_CARS 4
#define MENU_OPTION_VIEW_SALES_STATS 3
#define MENU_OPTION_EXIT 5

// Merge option Constants
#define MERGE_OPTION_STOCK 1
#define MERGE_OPTION_PRICE 3
#define MERGE_OPTION_YEAR 2
#define MERGE_OPTION_QUIT 4

// Maximum number of cars in inventory
#define MAX_CARS 50

#define MAX_SALES 100  // Max sales we can store
#define NameLength 50
#define CAR_INVENTORY_FILE_NAME "Cars.csv"
#define SALES_DATA_FILE_NAME "SalesData.csv"

// Struct Definitions
// ADD THE TOTAL SALES CAR PART THEN YOURE DONE WITH ALMOST THE ENTIRE CODE
struct Cars {
    char Name[NameLength];  // Car name
    int Year;                    // Car manufacturing year
    float Price;                 // Price of the car
    int Stock;                    // Number of cars in stock
    float TotalSaleValue;         // total sales of the certain car
};

struct Sales {
    int SaleIndex;              // Sale index (for tracking)
    float SaleAmount;           // Total sale amount
    int CarIndex;               // Index of the car sold
    int CarQuantity;            // Number of cars purchased
    char DiscountGivenText[100]; // Text description of the discount
    char CustomerName[CUSTOMER_NAME_LENGTH]; // Customer name
    int CustomerRating;         // rating from the user
    char CustomerFeedback[100]; // the description from the user
    char DateOfPurchase[50];    // Date where the user made the purchase
};

// Function Prototypes
void MainMenu(struct Cars CarsInventory[], struct Sales Sales[], int CurrentSaleIndex);
void BuyCars(struct Cars CarsInventory[], struct Sales Sales[], int CurrentSaleIndex);
float CalculateDiscount(float TotalPrice, char *DiscountMessage);
void SalesStats(struct Cars CarsInventory[], struct Sales Sales[], int CurrentSaleIndex);
void ClearScreen();
void ClearInputBuffer();
void CarStock(struct Cars CarsInventory[],struct Sales Sales[],int CurrentSaleIndex);
int CheckValidInteger(char* Question);
int CheckValidShort(char* Question);
char* CheckValidString(char* Question);
int ChosenCar(struct Cars CarsInventory[]);
int CarQuantity(int ChosenCarIndex,struct Cars CarsInventory[]);
void LoadCarsDataFromFile(struct Cars CarsInventory[]);
void SaveCarsDataToFile(struct Cars CarsInventory[]);
void SaveSalesDataToFile(struct Cars CarsInventory[],struct Sales Sales[],int CurrentSaleIndex);
void DefaultSalesDataFromFile(struct Cars CarsInventory[], struct Sales Sales[]);
void MergeSort(void* CarsInventory, int Left, int Right, int Field);
void Merge(void* CarsInventory, int Left, int Mid, int Right, int Field);
void SaveNewCarToFile(struct Cars CarsInventory[]);
int GetSizeOfCarInventoryFile(struct Cars CarsInventory[]);

// Helper Functions

void ClearScreen() {
    #ifdef _WIN32
        system("cls");  // Windows
    #else
        system("clear");  // Unix/Linux
    #endif
}

void ClearInputBuffer() {
    while (getchar() != '\n'); // Consume characters until newline
}


// creating Comparators for the user option to sort with
// this compare function needs to work with any type of thing so thats why we want it to work with any case
int CompareByPrice(const void*a ,const void*b) {
    // Cast the void pointers to pointers to struct Cars
    // basically because void* doesnt have a type you  can store anything however if you want to access it then you need to say whats in there and in this case its the struct Cars
    // we do this to access the price field
    struct Cars* CarA = (struct Cars*)a;
    struct Cars* CarB = (struct Cars*)b;

    // For descending order, reverse the comparison:
    if (CarA->Price > CarB->Price) return -1;
    if (CarA->Price < CarB->Price) return 1;
    return 0;
}
// and a b are pointers to the cars in the struct but you need to tell the void* that as it doesnt knwo the structure of the data
//if cars a is greater then -1 if car b is greeater 1 if eual 0

int CompareByStock(const void*a ,const void*b) {

    struct Cars* CarA = (struct Cars*)a;
    struct Cars* CarB = (struct Cars*)b;


    if (CarA->Stock > CarB->Stock) return -1;
    if (CarA->Stock < CarB->Stock) return 1;
    return 0;
}


int CompareByYear(const void*a ,const void*b) {

    struct Cars* CarA = (struct Cars*)a;
    struct Cars* CarB = (struct Cars*)b;


    if (CarA->Year > CarB->Year) return -1;
    if (CarA->Year < CarB->Year) return 1;
    return 0;
}

// divide then conqor
// MergeSort function that takes the 'field' parameter to specify sorting criteria
void MergeSort(void* CarsInventory, int Left, int Right, int Field) {
    if (Left < Right) {
        int Mid = (Left + Right) / 2; // always rounds down

        // Recursively sort the left and right halves
        // not actually splitting the areay into but changing the index and other variables
        MergeSort(CarsInventory, Left, Mid, Field); // for the left
        MergeSort(CarsInventory, Mid + 1, Right, Field); // for the right

        // Merge the sorted halves based on the field
        Merge(CarsInventory, Left, Mid, Right, Field);
    }
}

void Merge(void* CarsInventory, int Left, int Mid, int Right, int Field) {
    // Cast the void pointer to a pointer to struct Cars
    struct Cars* CarsArray = CarsInventory;

    int LeftSize = Mid - Left + 1;
    int RightSize = Right - Mid;

    // Temporary arrays to hold the left and right portions of the data
    struct Cars LeftArray[LeftSize], RightArray[RightSize];

    // Copy data to temporary arrays
    for (int i = 0; i < LeftSize; i++) {
        LeftArray[i] = CarsArray[Left + i];
    }
    for (int i = 0; i < RightSize; i++) {
        RightArray[i] = CarsArray[Mid + 1 + i];
    }

    int i = 0, j = 0, k = Left;

    // Merge the two halves based on the field chosen for sorting
    while (i < LeftSize && j < RightSize) {
        int CompareResult = 0;
        if (Field == 1) { // Sorting by stock
            CompareResult = CompareByStock(&LeftArray[i], &RightArray[j]);
        } else if (Field == 2) { // Sorting by year
            CompareResult = CompareByYear(&LeftArray[i], &RightArray[j]);
        } else if (Field == 3) { // Sorting by price
            CompareResult = CompareByPrice(&LeftArray[i], &RightArray[j]);
        }

        // If the left element is greater, add it to the merged array (for descending order)
        if (CompareResult <= 0) {
            CarsArray[k] = LeftArray[i];
            i++;
        } else {
            CarsArray[k] = RightArray[j];
            j++;
        }
        k++;
    }

    // Copy any remaining elements from left or right
    while (i < LeftSize) {
        CarsArray[k] = LeftArray[i];
        i++;
        k++;
    }
    while (j < RightSize) {
        CarsArray[k] = RightArray[j];
        j++;
        k++;
    }
}

void CarStock(struct Cars CarsInventory[],struct Sales Sales[],int CurrentSaleIndex) {
    int SortingChoice, Field;

    ClearScreen();
    printf("---- Car Inventory ----\n");
    printf("Model\t\tYear\tPrice\tStock\tTotal Sum\n");
    for (int i = 0; i < MAX_CARS; i++) {
        printf("%s\t%d\t%.2f\t%d\t%.2f\n", CarsInventory[i].Name, CarsInventory[i].Year, CarsInventory[i].Price, CarsInventory[i].Stock,CarsInventory[i].TotalSaleValue);
    }

    // Ask user for sorting criteria
    printf("Choose sorting criteria:\n");
    printf("1. Sort by Stock\n");
    printf("2. Sort by Year\n");
    printf("3. Sort by Price\n");
    printf("4. Exit\n");

    SortingChoice=CheckValidInteger("Enter your choice (1-4): ");

    switch(SortingChoice) {
        case MERGE_OPTION_STOCK:
            Field = 1;  // Sorting by stock
            break;
        case MERGE_OPTION_YEAR:
            Field = 2;  // Sorting by year
            break;
        case MERGE_OPTION_PRICE:
            Field = 3;  // Sorting by price
            break;
        case MERGE_OPTION_QUIT:
            MainMenu(CarsInventory, Sales, CurrentSaleIndex);
        default:
            printf("Invalid choice! Defaulting to sorting by stock.\n");
            Field = 1;  // Default to sorting by stock
    }

    // Call MergeSort based on the user's field choice
    MergeSort(CarsInventory, 0, MAX_CARS - 1, Field);

    CarStock(CarsInventory,Sales,CurrentSaleIndex);
}

int CheckValidInteger(char* Question) {
    int ValidInput;
    printf("%s", Question);
    if (scanf("%d", &ValidInput) != 1) {
        printf("Error: Invalid input. Please enter a valid integer.\n");
        ClearInputBuffer();
        return CheckValidInteger(Question);
    }
    return ValidInput;
}

int CheckValidShort(char* Question) {
    int ValidInput;
    printf("%s", Question);
    if (scanf("%hd", &ValidInput) != 1) {
        printf("Error: Invalid input. Please enter a valid number.\n");
        ClearInputBuffer();
        return CheckValidShort(Question);
    }
    return ValidInput;
}

char* CheckValidString(char* Question) {
    static char ValidInput[50]; // to keep it and for it to not erase when the function ends

    ClearInputBuffer();

    printf("%s", Question);
    fgets(ValidInput, 50, stdin);
    // Remove newline character if present
    ValidInput[strcspn(ValidInput, "\n")] = 0;
    for (int i = 0; i < strlen(ValidInput); i++) {
        if (!isalpha(ValidInput[i])) {
            printf("Error: Invalid input. Please enter a valid character.\n");
            ClearInputBuffer();
            return CheckValidString(Question);
        }
    }
    return ValidInput;
}



// Default sales data
void DefaultSalesDataFromFile(struct Cars CarsInventory[],struct Sales Sales[]) {
    int Index = 0;
    FILE *SalesDataFile = fopen(SALES_DATA_FILE_NAME, "r");

    if (SalesDataFile == NULL) {
        printf("Error opening sales data file.\n");
        printf("Creating sales data file with default data.\n");


        // Initialize default sales data
        struct Sales DefaultSales[MAX_SALES] = {
            {1, 36000.00, 0, 1, "20% discount applied", "John Doe", 5, "Excellent purchase!", "2024-11-25 23:26:53"},
            {2, 24000.00, 1, 2, "10% discount applied", "Jane Smith", 4, "Very satisfied", "2024-11-24 15:22:10"}
        };

        // Copy default data to Sales array
        for (Index = 0; Index < 2; Index++) { // Adjust number of default entries
            Sales[Index] = DefaultSales[Index];
        }

        SalesDataFile = fopen(SALES_DATA_FILE_NAME, "w");
        // Write CSV headers
        fprintf(SalesDataFile, "SaleIndex,SaleAmount,CarModel,CarQuantity,Discount,CustomerName,CustomerRating,CustomerFeedback,DateOfPurchase\n");

        // Write each sale data as a CSV row
        for (Index = 0; Index < 2; Index++) {
            fprintf(SalesDataFile, "%d,%.2f,%s,%d,%s,%s,%d,%s,%s\n",
                    Sales[Index].SaleIndex,
                    Sales[Index].SaleAmount,
                    CarsInventory[Sales[Index].CarIndex].Name,
                    Sales[Index].CarQuantity,
                    Sales[Index].DiscountGivenText,
                    Sales[Index].CustomerName,
                    Sales[Index].CustomerRating,
                    Sales[Index].CustomerFeedback,
                    Sales[Index].DateOfPurchase);
        }

        fclose(SalesDataFile);
    } else {
        fclose(SalesDataFile);
    }
}



void LoadCarsDataFromFile(struct Cars CarsInventory[]) {
    int Index = 0, StructArraySize=0;
    FILE *CarsInventoryFile = fopen(CAR_INVENTORY_FILE_NAME, "r");

    if (CarsInventoryFile == NULL) {
        printf("Error opening car inventory file.\n");
        printf("Creating car inventory file with default cars.\n");

        struct Cars DefaultCars[MAX_CARS] = {
            {"BMW Three Series", 2019, 45000.00, 4,450000.00},
            {"Audi RS3", 2017, 30000.00, 5,270000.00},
            {"Audi TT RS", 2019, 36000.00, 8,5000000.00},
            {"Audi RS7", 2021, 70000.00, 10,6000000.00},
            {"VW Golf R", 2024, 37000.00, 14,890000.00}
        };
        for ( Index = 0; Index < MAX_CARS; Index++) {
            CarsInventory[Index] = DefaultCars[Index];
        }

        CarsInventoryFile = fopen(CAR_INVENTORY_FILE_NAME, "w");
        fprintf(CarsInventoryFile,"Car Name,Year,Price,Stock,Total Sales Value\n");

        for (Index = 0; Index < MAX_CARS; Index++) {
            fprintf(CarsInventoryFile, "%s,%d,%.2f,%d,%.2f\n", CarsInventory[Index].Name, CarsInventory[Index].Year, CarsInventory[Index].Price, CarsInventory[Index].Stock,CarsInventory[Index].TotalSaleValue);
        }
        fclose(CarsInventoryFile);
    } else {

        StructArraySize=GetSizeOfCarInventoryFile(CarsInventory);

        // Skip the header line
        char headerLine[256];
        fgets(headerLine, sizeof(headerLine), CarsInventoryFile);  // Read and discard the header line

        // Read the rest of the data into the CarsInventory array
        while (fscanf(CarsInventoryFile, "%49[^,],%d,%f,%d,%f\n",
                      CarsInventory[Index].Name, &CarsInventory[Index].Year,
                      &CarsInventory[Index].Price, &CarsInventory[Index].Stock,
                      &CarsInventory[Index].TotalSaleValue) == StructArraySize) {
            Index++;
        }
    }
}

void SaveCarsDataToFile(struct Cars CarsInventory[]) {
    FILE *CarsInventoryFile = fopen(CAR_INVENTORY_FILE_NAME, "w");
    for (int Index = 0; Index < MAX_CARS; Index++) {
        fprintf(CarsInventoryFile, "%s,%d,%.2f,%d\n", CarsInventory[Index].Name, CarsInventory[Index].Year, CarsInventory[Index].Price, CarsInventory[Index].Stock);
    }
    fclose(CarsInventoryFile);
}

void SaveSalesDataToFile(struct Cars CarsInventory[],struct Sales Sales[],int CurrentSaleIndex) {
    FILE *SalesDataFile = fopen(SALES_DATA_FILE_NAME, "a"); // Open file in append mode

    if (SalesDataFile == NULL) {
        printf("Error creating sales data file.\n");
        return;
    }

    // Write the sales data in CSV format
    fprintf(SalesDataFile, "%d,%.2f,%s,%d,%s,%s,%d,%s,%s\n",
            Sales[CurrentSaleIndex].SaleIndex,
            Sales[CurrentSaleIndex].SaleAmount,
            CarsInventory[Sales[CurrentSaleIndex].CarIndex].Name,
            Sales[CurrentSaleIndex].CarQuantity,
            Sales[CurrentSaleIndex].DiscountGivenText,
            Sales[CurrentSaleIndex].CustomerName,
            Sales[CurrentSaleIndex].CustomerRating,
            Sales[CurrentSaleIndex].CustomerFeedback,
            Sales[CurrentSaleIndex].DateOfPurchase);

    fclose(SalesDataFile);
}


void SalesStats(struct Cars CarsInventory[], struct Sales Sales[], int CurrentSaleIndex) {
    int Index = 0;
    FILE *SalesDataFile = fopen(SALES_DATA_FILE_NAME, "r");

    if (SalesDataFile == NULL) {
        printf("Error opening sales data file.\n");
        return;
    }

    char lineread[240];

    // Read and display the header
    if (fgets(lineread, sizeof(lineread), SalesDataFile) != NULL) {
        // Print the header (just print as-is)
        printf("Header: %s", lineread);
    } else {
        printf("Error reading header.\n");
        fclose(SalesDataFile);
        return;
    }

    // Now, read and display the sales data
    printf("\nDisplaying Sales Data:\n");
    while (fgets(lineread, sizeof(lineread), SalesDataFile) != NULL) {
        // Parse each line using sscanf
        sscanf(lineread,
            "%d,%f,%99[^,],%d,%99[^,],%99[^,],%d,%99[^,],%99[^\n]\n",
            &Sales[Index].SaleIndex,
            &Sales[Index].SaleAmount,
            CarsInventory[Sales[Index].CarIndex].Name,
            &Sales[Index].CarQuantity,
            Sales[Index].DiscountGivenText,
            Sales[Index].CustomerName,
            &Sales[Index].CustomerRating,
            Sales[Index].CustomerFeedback,
            Sales[Index].DateOfPurchase);

        // Print each sale data in the same CSV format
        printf("Sale Index: %d\n", Sales[Index].SaleIndex);
        printf("Sale Amount: %.2f\n", Sales[Index].SaleAmount);
        printf("Car Model: %s\n", CarsInventory[Sales[Index].CarIndex].Name);
        printf("Quantity: %d\n", Sales[Index].CarQuantity);
        printf("Discount: %s\n", Sales[Index].DiscountGivenText);
        printf("Customer Name: %s\n", Sales[Index].CustomerName);
        printf("Customer Rating: %d\n", Sales[Index].CustomerRating);
        printf("Customer Feedback: %s\n", Sales[Index].CustomerFeedback);
        printf("Date Of Purchase: %s\n", Sales[Index].DateOfPurchase);
        printf("\n------------------------------------\n");

        // Increment the index for the next sale
        Index++;
    }

    fclose(SalesDataFile);

    printf("\nPress Enter to return to the main menu...\n");
    getchar();
    getchar();
    MainMenu(CarsInventory, Sales, CurrentSaleIndex);
}

float CalculateDiscount(float TotalPrice, char *DiscountMessage) {
    unsigned short UserAge = 0;
    char HasCompanyCard;
    int DiscountApplied = 0;

    printf("\nTo check if you're eligible for a discount, please answer the following questions:\n");

    do {
        UserAge = CheckValidShort("How old are you? ");
        if (UserAge < 0) {
            printf("Error: Age cannot be negative. Please enter a valid positive number.\n");
        }
        if (UserAge >= DISCOUNT_MIN_AGE && UserAge <= DISCOUNT_MAX_AGE) {
            DiscountApplied = 1;
            TotalPrice *= (1 - DISCOUNT_PERCENTAGE);
            printf("\nCongratulations! You got a 20 percent discount!\n");
            strcpy(DiscountMessage, "20% discount applied.");
        } else {
            getchar();  // Consume leftover newline from previous input
            printf("Do you have a company card? (y/n): ");
            scanf(" %c", &HasCompanyCard);
            if (HasCompanyCard == 'Y' || HasCompanyCard == 'y') {
                DiscountApplied = 2;
                TotalPrice *= (1 - MEMBERSHIP_DISCOUNT_PERCENTAGE);
                printf("You got a 50 percent discount.\n");
                strcpy(DiscountMessage, "50% discount applied.");
            } else {
                printf("You do not have a company card. No discount applied.\n\n");
            }
        }
    } while (UserAge < 0);  // Ensure valid age input

    if (DiscountApplied == 0) {
        printf("\nSorry, you are not eligible for a discount.\n");
        strcpy(DiscountMessage, "No discount applied.");
    }

    return TotalPrice;
}

void BuyCars(struct Cars CarsInventory[], struct Sales Sales[], int CurrentSaleIndex) {
    ClearScreen();

    unsigned short ChosenCarQuantity = 0, ChosenCarIndex = 0, TotalStock = 0;
    float TotalPrice = 0;
    char DiscountMessage[100] ,CustomerDescriptionChoice, CustomerDescription[100];
    int UserRating = 0;
    char * CustomerName;

    printf("Welcome to our car dealership!\n");
    printf("We are thrilled that you've decided to buy a car with us!\n\n");

    for (int i = 0; i < MAX_CARS; i++) {
        TotalStock += CarsInventory[i].Stock;
    }

    if (TotalStock == 0) {
        printf("Error: No cars are available at the moment. Please come back later.\n");
        printf("Redirecting to the main menu...\n");
        getchar();
        getchar();
        MainMenu(CarsInventory, Sales, CurrentSaleIndex);
    }

    printf("There are %hd cars available for purchase.\n", TotalStock);


    CustomerName=CheckValidString("Enter Name: ");

    ChosenCarIndex = ChosenCar(CarsInventory);
    ChosenCarQuantity = CarQuantity(ChosenCarIndex,CarsInventory);

    TotalPrice = CarsInventory[ChosenCarIndex].Price * ChosenCarQuantity;
    TotalPrice = CalculateDiscount(TotalPrice, DiscountMessage);

    CarsInventory[ChosenCarIndex].TotalSaleValue += TotalPrice;

    CarsInventory[ChosenCarIndex].Stock -= ChosenCarQuantity;
    SaveCarsDataToFile(CarsInventory);

    Sales[CurrentSaleIndex].SaleIndex = CurrentSaleIndex + 1;
    Sales[CurrentSaleIndex].SaleAmount = TotalPrice;
    Sales[CurrentSaleIndex].CarIndex = ChosenCarIndex;
    Sales[CurrentSaleIndex].CarQuantity = ChosenCarQuantity;
    strcpy(Sales[CurrentSaleIndex].CustomerName, CustomerName);
    strcpy(Sales[CurrentSaleIndex].DiscountGivenText, DiscountMessage);

    do{
        UserRating = CheckValidInteger("Rate the service between 1-5 ");
        if (UserRating < 1 || UserRating > 5){
            printf("ERROR: Must be in range of 1-5");
        }
        Sales[CurrentSaleIndex].CustomerRating = UserRating;
    } while (UserRating < 1 || UserRating > 5);

        printf("Would you like to leave a description about the car you purchessed (y/n)? ");
        ClearInputBuffer();
        scanf("%c", &CustomerDescriptionChoice);

        if (CustomerDescriptionChoice == 'y' || CustomerDescriptionChoice == 'Y') {
            printf("Enter your description: ");

            // Using scanf to capture an entire line with spaces
            scanf(" %[^\n]", CustomerDescription); // Read until newline is encountered

            strcpy(Sales[CurrentSaleIndex].CustomerFeedback, CustomerDescription);
        }else {
            printf("No Description Given");

            strcpy(Sales[CurrentSaleIndex].CustomerFeedback, "No Description Given");
        }

    // gets the current time in seconds
    time_t Now = time(NULL);
    // formats the struct data (aka the local time) in a string format
    //Destination string: Sales[CurrentSaleIndex].DateOfPurchase
    //Buffer size: sizeof(Sales[CurrentSaleIndex].DateOfPurchase)
    //Format string: "%Y-%m-%d %H:%M:%S"
    //localtime(&Now)
    strftime(Sales[CurrentSaleIndex].DateOfPurchase, sizeof(Sales[CurrentSaleIndex].DateOfPurchase), "%Y-%m-%d %H:%M:%S", localtime(&Now));

    SaveSalesDataToFile(CarsInventory,Sales,CurrentSaleIndex);
    CurrentSaleIndex++;

    printf("\nThank you for your purchase, %s! You bought %d %s(s) for %.2f.\n", CustomerName, ChosenCarQuantity, CarsInventory[ChosenCarIndex].Name, TotalPrice);
    printf("Remaining stock of %s: %d\n", CarsInventory[ChosenCarIndex].Name, CarsInventory[ChosenCarIndex].Stock);

    printf("Press Enter to return to the main menu...\n");
    getchar();
    getchar();
    MainMenu(CarsInventory, Sales, CurrentSaleIndex);
}

int ChosenCar(struct Cars CarsInventory[]) {
    int ChosenCarIndex = 0;
    for (int i = 0; i < MAX_CARS; i++) {
        printf("%d. %s\n", i + 1, CarsInventory[i].Name);
    }

    do {
        ChosenCarIndex = CheckValidInteger("Enter which car you would like to choose: ");
        if (ChosenCarIndex < 1 || ChosenCarIndex > MAX_CARS) {
            printf("Error: Invalid input. Please enter a number between 1 and %d.\n", MAX_CARS);
        }
    } while (ChosenCarIndex < 1 || ChosenCarIndex > MAX_CARS);

    return ChosenCarIndex - 1;
}

int CarQuantity(int ChosenCarIndex,struct Cars CarsInventory[]) {
    int CarQuantity = 0;


    printf("There are %d available currently\n", CarsInventory[ChosenCarIndex].Stock);
    do {
        CarQuantity = CheckValidInteger("Enter how many cars you want to buy: ");
        if (CarQuantity < 1 || CarQuantity > CarsInventory[ChosenCarIndex].Stock) {
            printf("Error: Invalid input. Please enter a number between 1 and %d.\n", CarsInventory[ChosenCarIndex].Stock);
        }
    } while (CarQuantity < 1 || CarQuantity > CarsInventory[ChosenCarIndex].Stock);

    return CarQuantity;
}
void SellCar(struct Cars CarsInventory[],struct Sales Sales[],int CurrentSaleIndex){
    char* CarName;
    int CarYear,CarStock;
    int Index=0;
    float CarPrice,TotalSalesValue = 0.0f;

    Index = GetSizeOfCarInventoryFile(CarsInventory);



    CarName = CheckValidString( "Enter name of the car you want to sell: ");

    CarYear=CheckValidInteger("Enter year of the car you want to sell: ");

    CarStock = CheckValidInteger("Enter how many cars you want to sell: ");

    printf("What is the Car's Price ");
    scanf("%f",&CarPrice);


    strcpy(CarsInventory[Index].Name,CarName);
    CarsInventory[Index].Year=CarYear;
    CarsInventory[Index].Stock=CarStock;
    CarsInventory[Index].Stock=CarStock;
    CarsInventory[Index].TotalSaleValue=TotalSalesValue;

    SaveNewCarToFile(CarsInventory);



    getchar();
    getchar();
    MainMenu(CarsInventory, Sales, CurrentSaleIndex);









}

void SaveNewCarToFile(struct Cars CarsInventory[]) {
    FILE* CarsInventoryFile = fopen(CAR_INVENTORY_FILE_NAME, "w");

    if (CarsInventoryFile == NULL) {
        printf("Unable to open file %s\n", CAR_INVENTORY_FILE_NAME);
        return;
    }

    fprintf(CarsInventoryFile,"Car Name,Year,Price,Stock,Total Sales Value\n");

    for (int i = 0; i < MAX_CARS; i++) {
        fprintf(CarsInventoryFile, "%s,%d,%.2f,%d,%.2f\n", CarsInventory[i].Name, CarsInventory[i].Year, CarsInventory[i].Price, CarsInventory[i].Stock,CarsInventory[i].TotalSaleValue);
    }
    fclose(CarsInventoryFile);

}


int GetSizeOfCarInventoryFile(struct Cars CarsInventory[]){
    int i,j=0;
    // to create an index of how many cars are in the struct array
    for (i = 0; i < MAX_CARS; i++) {
        if (strlen(CarsInventory[i].Name) > 0) {
            printf("Car %d: %s\n", i + 1, CarsInventory[i].Name);  // Print car name
            j++;  // Increment count of valid cars
        }
    }
    return j - 1; //  beacuse of the header line

}

void MainMenu(struct Cars CarsInventory[], struct Sales Sales[], int CurrentSaleIndex) {
    ClearScreen();

    printf("---- Car Dealership System ----\n");
    printf("1. View Available Cars\n");
    printf("2. Buy Cars\n");
    printf("3. View Sales Stats\n");
    printf("4. Sell Car\n");
    printf("5. Exit\n");

    int Choice = CheckValidInteger("Please select an option (1-5): ");

    switch (Choice) {
        case MENU_OPTION_VIEW_CAR_STOCK:
            CarStock(CarsInventory,Sales,CurrentSaleIndex);
            break;
        case MENU_OPTION_BUY_CARS:
            BuyCars(CarsInventory, Sales, CurrentSaleIndex);
            break;
        case MENU_OPTION_VIEW_SALES_STATS:
            SalesStats(CarsInventory, Sales, CurrentSaleIndex);
            break;
        case MENU_OPTION_SELL_CARS:
            SellCar(CarsInventory, Sales,CurrentSaleIndex);
        case MENU_OPTION_EXIT:
            printf("Exiting the system. Thank you for using our service!\n");
            exit(0);
        default:
            printf("Error: Invalid menu option. Please select a number between 1 and 5.\n");
            MainMenu(CarsInventory, Sales, CurrentSaleIndex);
            break;
    }
}

int main() {
    // Declare sales array and current sale index
    struct Sales Sales[MAX_SALES];
    int CurrentSaleIndex = 0;

    // The inventory of cars
    struct Cars CarsInventory[MAX_CARS];

    LoadCarsDataFromFile(CarsInventory);
    DefaultSalesDataFromFile(CarsInventory, Sales);
    MainMenu(CarsInventory, Sales, CurrentSaleIndex);
    return 0;
}



// check if valid string
// check if a set car stock is <1 if so then dont display car
// use the merge sort to rearrange the available cars via stock
// add sell car function
// tidy the code up and seperate the sales data from the buy cars function



