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
#define MERGE_OPTION_NAME 4
#define MERGE_OPTION_QUIT 5

// Maximum number of cars in inventory
#define MAX_CARS 50
#define MAX_SALES 100  // Max sales we can store
#define NameLength 50 // NEED TO PUT IT IN CAPS

// File name constants
#define CAR_INVENTORY_FILE_NAME "Cars.csv"
#define SALES_DATA_FILE_NAME "SalesData.csv"

// Struct Definitions
struct Cars {
    char Name[NameLength];
    int Year;
    float Price;
    int Stock;
    float TotalSaleValue;
};

struct Sales {
    int SaleIndex;
    float SaleAmount;
    int CarIndex;
    int CarQuantity;
    char DiscountGivenText[100];
    char CustomerName[CUSTOMER_NAME_LENGTH];
    int CustomerRating;
    char CustomerFeedback[100];
    char DateOfPurchase[50];
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
void SaveSalesDataToFile(struct Cars CarsInventory[],struct Sales Sales[],int CurrentSaleIndex);
void DefaultSalesDataFromFile(struct Cars CarsInventory[], struct Sales Sales[]);
void MergeSort(void* CarsInventory, int Left, int Right, int Field);
void Merge(void* CarsInventory, int Left, int Mid, int Right, int Field);
void SaveCarDataToFile(struct Cars CarsInventory[]);
int GetSizeOfCarInventoryFile(struct Cars CarsInventory[]);
void SellCar(struct Cars CarsInventory[],struct Sales Sales[],int CurrentSaleIndex);
int CompareByPrice(const void*a ,const void*b);
int CompareByStock(const void*a ,const void*b);
int CompareByYear(const void*a ,const void*b);
int BinarySearch(struct Cars* CarsInventory,char* SearchElemnt,int Left ,int Right);
// Helper Functions

void ClearScreen() {
    // checks when the code is being compiled if the _WIN32 exist which basically means if the code is running on a windows machine
    #ifdef _WIN32
        // the command to clear the terminal in windows
        system("cls");
    #else
        // the command to clear the screan for unix/linux
        system("clear");
    #endif // ends the ifdef
}

// can use this throughout the code to deal with when a user input something incorrectly and it gets put in the buffer
void ClearInputBuffer() {
    // Consume characters until newline that basically clears the input buffer
    while (getchar() != '\n');
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
// and a b are pointers to the cars in the struct but you need to tell the void* that as it doesn't know the structure of the data
//if cars a is greater than -1 if car b is greater than 1 if the same then return 0
// this works for the other comparison functions other than and which is slightly different
// source of idea from github

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

int CompareByName(const void*a ,const void*b) {

    struct Cars* CarA = (struct Cars*)a;
    struct Cars* CarB = (struct Cars*)b;

    // returns the comparison via strcmp  if CarA name is greater than Car B
    return strcmp(CarA->Name, CarB->Name);

}

void MergeSort(void* CarsInventory, int Left, int Right, int Field) {
    // base case to prevent an infinite loop
    if (Left < Right) {
        // always rounds down if there's a remainder and will find the middle
        int Mid = (Left + Right) / 2;

        // Recursively sort the left and right halves
        // not actually splitting the array into but changing the index and other variables
        MergeSort(CarsInventory, Left, Mid, Field); // for the left
        MergeSort(CarsInventory, Mid + 1, Right, Field); // for the right

        // Merge the sorted halves based on the field
        Merge(CarsInventory, Left, Mid, Right, Field);
    }
}

void Merge(void* CarsInventory, int Left, int Mid, int Right, int Field) {
    // Cast the void pointer to a pointer to struct Cars
    struct Cars* CarsArray = CarsInventory;

    // gets the size of the left and right array
    int LeftSize = Mid - Left + 1;
    int RightSize = Right - Mid;

    // Temporary arrays to hold the left and right portions of the data
    struct Cars LeftArray[LeftSize], RightArray[RightSize];

    // Copies the  data to the temporary arrays
    for (int i = 0; i < LeftSize; i++) {
        LeftArray[i] = CarsArray[Left + i];
    }
    for (int i = 0; i < RightSize; i++) {
        RightArray[i] = CarsArray[Mid + 1 + i];
    }

    // variables for incrementation
    int i = 0, j = 0, k = Left;

    // the loop will merge both arrays depending on the uses choice via the field variable
    while (i < LeftSize && j < RightSize) {
        // store the answer in CompareResult variable
        int CompareResult = 0;
        // if the user chose to sort by stock
        if (Field == 1) {
            CompareResult = CompareByStock(&LeftArray[i], &RightArray[j]);
        }
        // if the user chose to sort by year
        else if (Field == 2) {
            CompareResult = CompareByYear(&LeftArray[i], &RightArray[j]);
        }
        // if the user chose to sort by price
        else if (Field == 3) {
            CompareResult = CompareByPrice(&LeftArray[i], &RightArray[j]);
        }
        // if the user chose to sort via names
        else if (Field == 4) {
            CompareResult = CompareByName(&LeftArray[i], &RightArray[j]);
        }

        // If the left element is greater than the right element , add it to the merged array
        if (CompareResult <= 0) {
            CarsArray[k] = LeftArray[i];
            i++;
        }
        // else to do the opposite
        else {
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
    // clears the screen to enhance the users experience
    ClearScreen();
    // displays the cars inventory and the sub headings
    printf("---- Car Inventory ----\n");
    printf("Model\t\tYear\tPrice\tStock\tTotal Sum\n");

    // loops through the CarsInventory and if the CarsInventory.Name is populated then it will display the data
    for (int i = 0; i < MAX_CARS; i++) {
        if (strlen(CarsInventory[i].Name) >0){
            // prints out the data
            printf("%s\t%d\t%.2f\t%d\t%.2f\n", CarsInventory[i].Name, CarsInventory[i].Year, CarsInventory[i].Price, CarsInventory[i].Stock,CarsInventory[i].TotalSaleValue);
        }
    }

    // Ask user for sorting criteria
    printf("\n\nChoose sorting criteria:\n");
    printf("1. Sort by Stock\n");
    printf("2. Sort by Year\n");
    printf("3. Sort by Price\n");
    printf("4. Sort by Name\n");
    printf("5. Exit\n");


    do{
        // checks if what the user inputted is a valid integer
        SortingChoice=CheckValidInteger("Enter your choice (1-5): ");

        switch(SortingChoice) {
            // if the user chose the stock option
            case MERGE_OPTION_STOCK:
                // sorting by stock
                Field = 1;
                // sues breaks to prevent fall through
                break;
            // if the user chose the year option
            case MERGE_OPTION_YEAR:
                // sorting by year
                Field = 2;
                break;
            // if the user chose the price option
            case MERGE_OPTION_PRICE:
                // sorting by price
                Field = 3;
                break;
            // fi the user chose the name option
            case MERGE_OPTION_NAME:
                // sorting by name
                Field = 4;
                break;
            // if the user chose to quit option
            case MERGE_OPTION_QUIT:
                MainMenu(CarsInventory, Sales, CurrentSaleIndex);
            // if the user inputs something that is invalid
            default:
                // displaus and error message
                printf("ERROR: Enter a valid input in the range of (1-4)");
                // clears the input buffer
                ClearInputBuffer();
        }
    }while (SortingChoice < 1 || SortingChoice >5);


    // calls MergeSort and depending on what the user chose the field would be different as well
    MergeSort(CarsInventory, 0, MAX_CARS - 1, Field);

    // recalls the function to update the the cars order on the user screen
    CarStock(CarsInventory,Sales,CurrentSaleIndex);
}

// checks if valid integer
int CheckValidInteger(char* Question) {
    int ValidInput;
    // prints out the question through whatever parameter was when being called
    printf("%s", Question);
    // does a do loop to prevent stack overflow
    do{
        // checks if what the user input is valid via scanf and its placeholder
        if (scanf("%d", &ValidInput) != 1) {
            printf("Error: Invalid input. Please enter a valid integer.\n");
            // clears the input as whatever is not correctly stored from teh scanf then it stored in the buffer therefore its used to prevent errors in the code later on
            ClearInputBuffer();
        }
        // keeps looping till the valid input is valid
    }while(scanf("%d", &ValidInput) == 1);

    // returns the valid input
    return ValidInput;
}

int CheckValidShort(char* Question) {
    int ValidInput;
    // prints the question to the user
    printf("%s", Question);
    // does a do whole loop to prevent stack overflow
    do {
        // if the placeholder is not stored properly then it will nto return 1
        if (scanf("%hd", &ValidInput) != 1) {
            // sends and error message
            printf("Error: Invalid input. Please enter a valid number.\n");
            // clears the input buffer
            ClearInputBuffer();
        }
        // keeps looping till the code it is properly stored via the scanf
    }while(scanf("%hd", &ValidInput) == 1);
    // return the valid input
    return ValidInput;
}

char* CheckValidString(char* Question) {
    // the function is used recursively or when its result needs to be used after the function ends.
    // to keep it and for it to not erase when the function ends
    static char ValidInput[50];

    // clears the input buffer
    ClearInputBuffer();

    // prints out the function
    printf("%s", Question);
    // stores the user input via fgets
    fgets(ValidInput, 50, stdin);

    // Remove newline character if present
    //fgets puts a /n
    ValidInput[strcspn(ValidInput, "\n")] = 0;
    // loops to check every letter
    for (int i = 0; i < strlen(ValidInput); i++) {
        // to ignore if its a space or if the character is a letter
        if(!(isalpha(ValidInput[i]) || ValidInput[i] == ' ')) {
            // then it will give an error message telling the user they did not enter a valid string
            printf("Error: Invalid input. Please enter only characters.\n");
            // then reclears the input buffer
            ClearInputBuffer();
            // loops back to the question
            return CheckValidString(Question);
        }
    }
    // returns the valid response
    return ValidInput;
}



// Default sales data
void DefaultSalesDataFromFile(struct Cars CarsInventory[],struct Sales Sales[]) {
    // holds the index
    int Index = 0;
    // opens the sales data file in read mode
    FILE *SalesDataFile = fopen(SALES_DATA_FILE_NAME, "r");

    // if the file does not exist then it will tell the user an error occured and it is now creating default data
    if (SalesDataFile == NULL) {
        printf("Error opening sales data file.\n");
        printf("Creating sales data file with default data.\n");


        // initialize default sales data
        struct Sales DefaultSales[MAX_SALES] = {
            {1, 36000.00, 0, 1, "20% discount applied", "John Doe", 5, "Excellent purchase!", "2024-11-25 23:26:53"},
            {2, 24000.00, 1, 2, "10% discount applied", "Jane Smith", 4, "Very satisfied", "2024-11-24 15:22:10"}
        };

        //  puts the defualt data in the sales struct array
        for (Index = 0; Index < 2; Index++) {
            Sales[Index] = DefaultSales[Index];
        }

        // then opens the file in write mode
        SalesDataFile = fopen(SALES_DATA_FILE_NAME, "w");

        // write CSV headers
        fprintf(SalesDataFile, "SaleIndex,SaleAmount,CarModel,CarQuantity,Discount,CustomerName,CustomerRating,CustomerFeedback,DateOfPurchase\n");

        // writes the sales data in the file
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
        // clsoes the file
        fclose(SalesDataFile);

    }else {
        // if the file exists then it will close the file
        fclose(SalesDataFile);
    }
}



void LoadCarsDataFromFile(struct Cars CarsInventory[]) {
    // holds the current index
    int Index = 0;

    // opens the desired file in read mode
    FILE *CarsInventoryFile = fopen(CAR_INVENTORY_FILE_NAME, "r");

    // cehcks if the file exists and if the fuile doesnt then creates a defualt file data and saves the data in the CarsInventory struct array
    if (CarsInventoryFile == NULL) {
        printf("Error opening car inventory file.\n");
        printf("Creating car inventory file with default cars.\n");

        // struct array for default cars data
        struct Cars DefaultCars[MAX_CARS] = {
            {"Volkswagen Passat", 2019, 45000.00, 4,450000.00},
            {"Ford Mustang", 2017, 30000.00, 5,270000.00},
            {"Chevrolet Impala", 2019, 36000.00, 8,5000000.00},
            {"Honda Civic", 2021, 70000.00, 10,6000000.00},
            {"VW Golf R", 2024, 37000.00, 14,890000.00}
        };

        // loops to store the default cars data in the CarsInventory struct array
        for ( Index = 0; Index < MAX_CARS; Index++) {
            CarsInventory[Index] = DefaultCars[Index];
        }

        // pens the file in write mode
        CarsInventoryFile = fopen(CAR_INVENTORY_FILE_NAME, "w");

        // the header of the file for it to be easier to read when looking at the file
        fprintf(CarsInventoryFile,"Car Name,Year,Price,Stock,Total Sales Value\n");

        // writes the data in the selceted format via the placeholders
        for (Index = 0; Index < MAX_CARS; Index++) {
            fprintf(CarsInventoryFile, "%s,%d,%.2f,%d,%.2f\n", CarsInventory[Index].Name, CarsInventory[Index].Year, CarsInventory[Index].Price, CarsInventory[Index].Stock,CarsInventory[Index].TotalSaleValue);
        }

        // closes the file
        fclose(CarsInventoryFile);


    }else {
        // Skip the header line
        char headerLine[256];
        // reads and discards the header line
        fgets(headerLine, sizeof(headerLine), CarsInventoryFile);

        // Read the rest of the lines
        while (fscanf(CarsInventoryFile, "%49[^,],%d,%f,%d,%f\n",
                      CarsInventory[Index].Name,
                      &CarsInventory[Index].Year,
                      &CarsInventory[Index].Price,
                      &CarsInventory[Index].Stock,
                      &CarsInventory[Index].TotalSaleValue) == 5) {
            // moves on to the next index
            Index++;
                      }
        // clsoes the file
        fclose(CarsInventoryFile);
        // wirtes  to the user that cars have been loaded to the file
        printf("Loaded cars from the file.\n");
    }



}

void SaveSalesDataToFile(struct Cars CarsInventory[],struct Sales Sales[],int CurrentSaleIndex) {
    // opens the Sales data file in append mode
    FILE *SalesDataFile = fopen(SALES_DATA_FILE_NAME, "a");

    // if the file does not exist then it displays a message saying where the erorr is
    if (SalesDataFile == NULL) {
        printf("Error creating sales data file.\n");
        return;
    }

    // Write the sales data into the file
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

    // closes the file
    fclose(SalesDataFile);
}

void SaveCarDataToFile(struct Cars CarsInventory[]) {
    // opens the file in write mode
    FILE* CarsInventoryFile = fopen(CAR_INVENTORY_FILE_NAME, "w");

    // if the file does not exist then displays the error message that the file could not be opened
    if (CarsInventoryFile == NULL) {
        printf("Unable to open file. \n");
        return;
    }

    // prints the header in the file
    fprintf(CarsInventoryFile,"Car Name,Year,Price,Stock,Total Sales Value\n");

    // loops through the CarsInventory data for every car and updates the file in the process
    for (int i = 0; i < MAX_CARS; i++) {
        fprintf(CarsInventoryFile, "%s,%d,%.2f,%d,%.2f\n", CarsInventory[i].Name, CarsInventory[i].Year, CarsInventory[i].Price, CarsInventory[i].Stock,CarsInventory[i].TotalSaleValue);
    }
    // closes the file
    fclose(CarsInventoryFile);

}

void SalesStats(struct Cars CarsInventory[], struct Sales Sales[], int CurrentSaleIndex) {
    int Index = 0;
    char Lineread[240];
    // opens the sales data file
    FILE *SalesDataFile = fopen(SALES_DATA_FILE_NAME, "r");

    // checks if the file doesnt exist
    if (SalesDataFile == NULL) {
        // displays a message if the file does not exist
        printf("Error opening sales data file.\n");
        return;
    }

    // Read and display the header
    // got this from my tutorial lecturer nigel
    if (fgets(Lineread, sizeof(Lineread), SalesDataFile) != NULL) {
        // Print the header (just print as-is)
        printf("Header: %s", Lineread);
    } else {
        // displays message if they cant display the file header
        printf("Error reading header.\n");
        // closes the file
        fclose(SalesDataFile);
        return;
    }

    // reads and display the sales data
    printf("\nDisplaying Sales Data:\n");
    while (fgets(Lineread, sizeof(Lineread), SalesDataFile) != NULL) {
        // Parse each line using sscanf
        sscanf(Lineread,
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

        // if the car name is populated
        if (CarsInventory[Index].Name >0) {
            // Print each sale data
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
    }

    // closes the file
    fclose(SalesDataFile);

    // tells the user to press enter to head to the main menu
    printf("\nPress Enter to return to the main menu...\n");
    // makes the user press enter to move to the next section of the code
    getchar();
    getchar();
    // calls the function mainmenu
    MainMenu(CarsInventory, Sales, CurrentSaleIndex);
}

// I have wrote all the code togehter but realisticallyi feel like if im having a variable called Applied discount i should create a switch statmenet and split the maths form the outputs then i can use the default as a debug message
float CalculateDiscount(float TotalPrice, char *DiscountMessage) {
    unsigned short UserAge = 0;
    char HasCompanyCard;
    int DiscountApplied = 0;

    // displays the message to tell the user to enter the details to see fi they can get the discount
    printf("\nTo check if you're eligible for a discount, please answer the following questions:\n");

    do {
        // checks if the UserAge is a valid short integer via CheckValidShort
        UserAge = CheckValidShort("How old are you? ");

        // if the user inputs something that is less than zero then displays an error message
        if (UserAge < 0) {
            printf("Error: Age cannot be negative. Please enter a valid positive number.\n");
        }

        // checks if the user is in range of getting the discount via the max and min age
        if (UserAge >= DISCOUNT_MIN_AGE && UserAge <= DISCOUNT_MAX_AGE) {
            // if the user then they get the discount applied message
            DiscountApplied = 1;
            // updates the totalprice via the 1- of the discount percentage
            TotalPrice *= (1 - DISCOUNT_PERCENTAGE);
        } else {
            // Consume leftover newline from previous input
            getchar();

            do {
                // asks the user if they have a company card
                printf("Do you have a company card? (y/n): ");
                // stores the data via scanf
                scanf(" %c", &HasCompanyCard);
                // if the user enters either a valid upper or lowercase y
                if (HasCompanyCard == 'Y' || HasCompanyCard == 'y') {
                    // discount applied is now 2
                    DiscountApplied = 2;
                    // gives a different discount percentage than if the user fits in the age range
                    TotalPrice *= (1 - MEMBERSHIP_DISCOUNT_PERCENTAGE);
                } // if the use enters n lowercase or uppercase then it will display the message
                else if (HasCompanyCard == 'N' || HasCompanyCard == 'n') {
                    // displays a message to state the user does not have a have a company card
                    printf("You do not have a company card.\n\n");
                }
                else {
                    // tells the user that they did not enter a valid option
                    printf("You did not enter a valid option. must either be y/n");
                    // clears the input buffer
                    ClearInputBuffer();
                }
                // loops until the user enters a valid input
            } while (HasCompanyCard == 'N' || HasCompanyCard == 'n'|| HasCompanyCard == 'Y' || HasCompanyCard == 'y');
        }
        // loops until the user enters a UserAge that is greater than 0
    } while (UserAge < 0);

    // if DiscountApplied is zero
    if (DiscountApplied == 0) {
        // displays message saying they are eligible for a discount
        printf("\nSorry, you are not eligible for a discount.\n");
        // stores the data in DiscountMessage via strcpy
        strcpy(DiscountMessage, "No discount applied.");
    }
    // if DiscountApplied is 1
    else if (DiscountApplied == 1) {
        // displays the messages that they received a 20% discount
        printf("\nCongratulations! You got a 20 percent discount!\n");
        // store it in DiscountMessage via strcpy
        strcpy(DiscountMessage, "20% discount applied.");
    }
    // if DiscountApplied is 2
    else if (DiscountApplied == 2) {
        // tells the user that they got a 50% discount
        printf("\nYou got a 50 percent discount.\n");
        // stores the the data in DiscountMessages via strcpy
        strcpy(DiscountMessage, "50% discount applied.");
    }

    // returns the updated total price
    return TotalPrice;
}

void BuyCars(struct Cars CarsInventory[], struct Sales Sales[], int CurrentSaleIndex) {
    // clears the screen for enhance user experience
    ClearScreen();

    // variables
    unsigned short ChosenCarQuantity = 0, ChosenCarIndex = 0, TotalStock = 0;
    float TotalPrice = 0;
    char DiscountMessage[100] ,CustomerDescriptionChoice, CustomerDescription[100];
    int UserRating = 0;
    char * CustomerName;

    // greets the user
    printf("Welcome to our car dealership!\n");
    printf("We are thrilled that you've decided to buy a car with us!\n\n");

    // loops through to store the total stock available currently
    for (int i = 0; i < MAX_CARS; i++) {
            TotalStock += CarsInventory[i].Stock;
    }

    // if theres no cars have stock
    if (TotalStock == 0) {
        // will display an error message to the user and tell them to come back later and press enter twice till the main menu appears
        printf("Error: No cars are available at the moment. Please come back later.\n");
        printf("Redirecting to the main menu.\n");
        printf("Please press enter till you see the main menu.....");

        // makes the user have to press enter to move forward
        getchar();
        getchar();
        // calls back the main menu
        MainMenu(CarsInventory, Sales, CurrentSaleIndex);
    }

    // dipslays how much total  stock is available
    printf("There are %hd cars available for purchase.\n", TotalStock);

    // checks if the custoemr name is a valid string via the function CheckValidString
    CustomerName=CheckValidString("Enter Name: ");

    // gets the chosen car index of the car the user wants
    ChosenCarIndex = ChosenCar(CarsInventory);
    // gets the quantity of cars that want to be bought by the user
    ChosenCarQuantity = CarQuantity(ChosenCarIndex,CarsInventory);

    // calculates the current total price
    TotalPrice = CarsInventory[ChosenCarIndex].Price * ChosenCarQuantity;
    // then calls a function to wrok out if the user gets a discount ro nto
    TotalPrice = CalculateDiscount(TotalPrice, DiscountMessage);

    //updates the TotalSaleValue of the car with adding the TotalPrice to the existing
    CarsInventory[ChosenCarIndex].TotalSaleValue += TotalPrice;

    //  updates the chosen cars stock by taking away the quantity the user bough
    CarsInventory[ChosenCarIndex].Stock -= ChosenCarQuantity;
    // calls the SaveCarDataFile to save the updated data
    SaveCarDataToFile(CarsInventory);


    // updates all the sales data with the CurrentSalesIndex
    Sales[CurrentSaleIndex].SaleIndex = CurrentSaleIndex + 1;
    Sales[CurrentSaleIndex].SaleAmount = TotalPrice;
    Sales[CurrentSaleIndex].CarIndex = ChosenCarIndex;
    Sales[CurrentSaleIndex].CarQuantity = ChosenCarQuantity;
    strcpy(Sales[CurrentSaleIndex].CustomerName, CustomerName);
    strcpy(Sales[CurrentSaleIndex].DiscountGivenText, DiscountMessage);


    do{
        // checks if the user rating is a valid integer via CheckValidInteger
        UserRating = CheckValidInteger("Rate the service between 1-5 (Whole numbers only) ");

        // checks if the user inputted a integer out of the range
        if (UserRating < 1 || UserRating > 5){
            // displays an error message
            printf("ERROR: Must be in range of 1-5");
        }
        // Stores the UserRating in the sales data
        Sales[CurrentSaleIndex].CustomerRating = UserRating;
        // loops untill the user enters a vaid response
    } while (UserRating < 1 || UserRating > 5);

    // asks the user if they want to add a description
    printf("Would you like to leave a description about the car you purchessed (y/n)? ");

    do {
        // clears the input buffer
        ClearInputBuffer();
        // stores the response via scanf
        scanf("%c", &CustomerDescriptionChoice);

        // if the user enters y or Y
        if (CustomerDescriptionChoice == 'y' || CustomerDescriptionChoice == 'Y') {
            printf("Enter your description: ");

            // Using scanf to capture an entire line with spaces
            // Read until newline is encountered
            scanf(" %[^\n]", CustomerDescription);

            // stores the feedback in the sales array via strcpy
            strcpy(Sales[CurrentSaleIndex].CustomerFeedback, CustomerDescription);
        }
        // if the user enters upper or lowercase n
        else if (CustomerDescriptionChoice == 'n' || CustomerDescriptionChoice == 'N') {
            printf("No Description Given");

            strcpy(Sales[CurrentSaleIndex].CustomerFeedback, "No Description Given");
        }
        else {
            printf("ERROR: Please enter a valid option(y/n).\n");
            // clears the input buffer
            ClearInputBuffer();
        }
        // loops until valid resposne
    }while(CustomerDescriptionChoice == 'n' || CustomerDescriptionChoice == 'N'|| CustomerDescriptionChoice == 'y' || CustomerDescriptionChoice == 'Y');

    // gets the current time in seconds
    time_t Now = time(NULL);

    // formats the struct data (aka the local time) in a string format
    //Destination string: Sales[CurrentSaleIndex].DateOfPurchase
    //Buffer size: sizeof(Sales[CurrentSaleIndex].DateOfPurchase)
    //Format string: "%Y-%m-%d %H:%M:%S"
    //localtime(&Now)
    strftime(Sales[CurrentSaleIndex].DateOfPurchase, sizeof(Sales[CurrentSaleIndex].DateOfPurchase), "%Y-%m-%d %H:%M:%S", localtime(&Now));

    // saves sales data via SaveSalesDataToFile
    SaveSalesDataToFile(CarsInventory,Sales,CurrentSaleIndex);
    // increments CurrentSaleIndex by 1
    CurrentSaleIndex++;

    // thanks the user and shows what they bought and for how much
    printf("\nThank you for your purchase, %s! You bought %d %s(s) for %.2f.\n", CustomerName, ChosenCarQuantity, CarsInventory[ChosenCarIndex].Name, TotalPrice);
    // displays the remaining stock for the car the user choses
    printf("Remaining stock of %s: %d\n", CarsInventory[ChosenCarIndex].Name, CarsInventory[ChosenCarIndex].Stock);

    // tells the user to press enter to go to the main menu
    printf("Press Enter to return to the main menu...\n");
    getchar();
    getchar();
    // calls the main menu function
    MainMenu(CarsInventory, Sales, CurrentSaleIndex);
}

int ChosenCar(struct Cars CarsInventory[]) {
    // initialise the variable to hold the chosen car index
    int ChosenCarIndex = 0;

    // loops through to displays the car name and the list how many cars are there if the carsInventory[i] is populated
    for (int i = 0; i < MAX_CARS; i++) {
        if(strlen(CarsInventory[i].Name) > 0) {
            printf("%d. %s\n", i + 1, CarsInventory[i].Name);
        }
    }

    do {
        // gets the user ChosenCarIndex and checks if its a valid integer via CheckValidInteger function
        ChosenCarIndex = CheckValidInteger("Enter which car you would like to choose: ");
        // if the user enters something that is either less than 1 or is greater than the max cars then the program will display an error message
        if (ChosenCarIndex < 1 || ChosenCarIndex > MAX_CARS) {
            // displays an error message if its out of the range
            printf("Error: Invalid input. Please enter a number between 1 and %d.\n", MAX_CARS);
        }
        // keeps looping over the code untill the criteria is met
    } while (ChosenCarIndex < 1 || ChosenCarIndex > MAX_CARS);

    // returns the correct index
    return ChosenCarIndex - 1;
}

int CarQuantity(int ChosenCarIndex,struct Cars CarsInventory[]) {
    int CarQuantity = 0;

    // displays the current stock available for the car the user chose
    printf("There are %d available currently\n", CarsInventory[ChosenCarIndex].Stock);

    do {
        // asks how many cars the user wants to buy and checks it via CheckValidInteger
        CarQuantity = CheckValidInteger("Enter how many cars you want to buy: ");

        // if the user enters something that is out of range then the code will display and error message
        if (CarQuantity < 1 || CarQuantity > CarsInventory[ChosenCarIndex].Stock) {
            printf("Error: Invalid input. Please enter a number between 1 and %d.\n", CarsInventory[ChosenCarIndex].Stock);
        }

        // keep looping till the requirments are met
    } while (CarQuantity < 1 || CarQuantity > CarsInventory[ChosenCarIndex].Stock);

    // returns the car quantity
    return CarQuantity;
}

void SellCar(struct Cars CarsInventory[],struct Sales Sales[],int CurrentSaleIndex){
    char* CarName;
    int CarYear,CarStock,CarMatch,AddingStock;
    int Index=0;
    float CarPrice,TotalSalesValue = 0.0f;
    char UserChoice;

    Index = GetSizeOfCarInventoryFile(CarsInventory);
    MergeSort(CarsInventory, 0, Index-1 - 1, 4);

    for (int i = 0; i < MAX_CARS; i++) {
        if (strlen(CarsInventory[i].Name) >0){
            printf("%s\t%d\t%.2f\t%d\t%.2f\n", CarsInventory[i].Name, CarsInventory[i].Year, CarsInventory[i].Price, CarsInventory[i].Stock,CarsInventory[i].TotalSaleValue);
        }
    }


    // gets the car name and checks if its a valid string
    CarName = CheckValidString( "Enter name of the car you want to sell:( please type every word to start with a capitial letter at the start to be able to search it through pur database) ");
    // uses CarMatch to store the value that the function BinarySearch
    CarMatch=BinarySearch(CarsInventory,CarName,0,Index-1);

        // if Carmatch equal 0
        if (CarMatch == 0) {
            // tells the user we found the car name and asks if they want to add their stock to the existing stock
            printf("we match the car name you entered to one of our cars already in stock.\n");
            printf("Do you want to add your stock to the existing one? (y/n)\n");
            // stores the repsone via userchoice
            scanf("%c", &UserChoice);


            do {
                // if the user enters y upper or lowercase
                if (UserChoice == 'y' || UserChoice == 'Y') {
                    // checks if its a valid integer and how muhc stock the user wants to add
                    AddingStock = CheckValidInteger("Enter how much stock you want to add: ");
                    // adds the stock to the existing stock
                    CarsInventory[CarMatch].Stock += AddingStock;

                    // tell the user to press enter to go to main menu
                    printf("Press enter to go to main menu");
                    getchar(); // Consume the newline from the previous input
                    getchar(); // Wait for user to press Enter
                    MainMenu(CarsInventory, Sales, CurrentSaleIndex);
                // if the user enter n upper or lower case
                } else if (UserChoice == 'n' || UserChoice == 'N') {
                    // infroms the user that no stock will be added
                    printf("Not adding stock to matched car\n");
                } else {
                    // if the user enters an invalid response then message will display
                    printf("Please enter a valid input y/n\n");
                    // Clear the buffer in case of invalid input
                    ClearInputBuffer();
                }
                // loops untill the user enters a valid response
            } while (UserChoice != 'y' && UserChoice != 'Y' && UserChoice != 'n' && UserChoice != 'N');
            // if CarMatch is -1
        } else if (CarMatch == -1) {
            // diplsay message that the car was not found
            printf("car was not found");
        }



    // gets the CarYear from the user and check if its a valid integer via CheckValidInteger
    CarYear=CheckValidInteger("Enter the year of the car you want to sell: ");

    // gets the CarStock and checks if its a valid integer via CheckValidInteger
    CarStock = CheckValidInteger("Enter how many cars you want to sell: ");

    // the price that the user wants to enter
    printf("What is the Car's Price ");
    scanf("%f",&CarPrice);


    // stores the data in the CarsInventory Struct array
    strcpy(CarsInventory[Index].Name,CarName);
    CarsInventory[Index].Year=CarYear;
    CarsInventory[Index].Stock=CarStock;
    CarsInventory[Index].TotalSaleValue=TotalSalesValue;

    // calls the SaveCarDataToFile to save the cars data
    SaveCarDataToFile(CarsInventory);

    // tells the user to press enter to go to the main menu
    printf("Press enter to go to main menu");
    getchar();
    getchar();
    // calls the function MainMenu
    MainMenu(CarsInventory, Sales, CurrentSaleIndex);


}



int GetSizeOfCarInventoryFile(struct Cars CarsInventory[]){
    int i,j=0;
    // to create an index of how many cars are in the struct array
    for (i = 0; i < MAX_CARS; i++) {
        if (strlen(CarsInventory[i].Name) > 0) {// Print car name
            j++;  // Increment count of valid cars
        }
    }
    return j-1; //  beacuse of the header line

}
// return one if found then go to car Sales and go if car found ask question if not then dont and allow to write a new entry
int BinarySearch(struct Cars* CarsInventory, char* SearchElement, int Left, int Right) {
    // base case to prevent continously looping
    if (Left > Right) {
        // if not found  displays the message
        printf("Not found!\n");
        // returns -1
        return -1;
    }

    // finds the middle
    int Middle = (Left + Right) / 2;
    // tells the user what they are comparing
    printf("Comparing '%s' with '%s'\n", CarsInventory[Middle].Name, SearchElement);
    // storing the value if the user has str are the same via strcmp
    int Target = strcmp(CarsInventory[Middle].Name, SearchElement);

    // if it equals 0 then
    if (Target == 0) {
        // outputs that the program found it and where they found it
        printf("Found '%s' at index %d!\n", SearchElement, Middle);
        // returns the middle
        return Middle;
    }
    // if the the target is greater than 0 then it would check the left side of the array
    if (Target > 0) {
        BinarySearch(CarsInventory, SearchElement, Left, Middle - 1);
    } else {
        // checks via the right side of the array
        BinarySearch(CarsInventory, SearchElement, Middle + 1, Right);
    }
}



void MainMenu(struct Cars CarsInventory[], struct Sales Sales[], int CurrentSaleIndex) {
    // clears the screen for the user to make it cleaner fo rhte user
    ClearScreen();

    // displays the options the user can pick
    printf("---- Car Dealership System ----\n");
    printf("1. View Available Cars\n");
    printf("2. Buy Cars\n");
    printf("3. View Sales Stats\n");
    printf("4. Sell Car\n");
    printf("5. Exit\n");

    // uses a valid int checker to go through and ask the question to see if its valid
    int Choice = CheckValidInteger("Please select an option (1-5): ");
    // created a do while loop to prevent the code from stack overflowing from the users constant failed attempts
    do {
        switch (Choice) {
            // if the user wanted to go view the available cars
            case MENU_OPTION_VIEW_CAR_STOCK:
                // calls the function to view car availability
                CarStock(CarsInventory,Sales,CurrentSaleIndex);
            // if the user wants to buy a car
            case MENU_OPTION_BUY_CARS:
                BuyCars(CarsInventory, Sales, CurrentSaleIndex);
            // if the user wants to view the sales stats
            case MENU_OPTION_VIEW_SALES_STATS:
                SalesStats(CarsInventory, Sales, CurrentSaleIndex);
            // if the user wants to sell a car
            case MENU_OPTION_SELL_CARS:
                SellCar(CarsInventory, Sales,CurrentSaleIndex);
            // if the user wants to exit the program
            case MENU_OPTION_EXIT:
                printf("Exiting the system. Thank you for using our service!\n");
                exit(0);
            // if the user enters an invalid option
            default:
                printf("Error: Invalid menu option. Please select a number between 1 and 5.\n");
        }
        // loops untill the option is valid
    }while (Choice <1 || Choice > 5);
}

int main() {
    // Declare sales struct array which will store the sales data
    struct Sales Sales[MAX_SALES];

    // to keep track of the index of the current sale
    int CurrentSaleIndex = 0;

    // declares the CarsInventory struct array inventory of cars
    struct Cars CarsInventory[MAX_CARS];

    // calls the function LoadCarsDataFromFile and DefaultSalesDataFromFile to create our csv file ready for the program to use
    LoadCarsDataFromFile(CarsInventory);
    DefaultSalesDataFromFile(CarsInventory, Sales);

    // calls the main menu after all the inital data we need is loaded
    MainMenu(CarsInventory, Sales, CurrentSaleIndex);
    return 0;
}

