#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define BUFFER_SIZE 100000
#define MIN_LENGTH 8  // 2^3.
#define MAX_LENGTH 64 // 2^6.
#define NUMBER_OF_ENGLISH_LETTERS 26
#define NUMBER_OF_DECIMAL_DIGITS 10

// Function that Clears the Input Buffer.
void Clear_Input_Buffer(void)
{
    int Input_Character;
    while ((Input_Character = getchar()) != '\n' && Input_Character != EOF)
    {
        // Discard Characters from the Input Buffer.
    }
    if (Input_Character == EOF)
    {
        printf("\033[0;31mError: Unexpected End-of-File Encountered While Reading Input.\033[0m\n");
        exit(EXIT_FAILURE);
    }
}

// Function for Input Validation.
bool Parse_Integer(char *Input_String, int *Input_Integer)
{
    unsigned short Input_String_Index = 0;
    while (isspace(Input_String[Input_String_Index])) // Moves Through any Leading Whitespace Characters in the Input String.
        Input_String_Index++;
    unsigned short Input_String_Length = strlen(Input_String); // Gets the Length of the Input String.
    if (Input_String_Length == Input_String_Index)             // i.e. If the Input String Contains Only Whitespace Characters.
        return false;
    char *Integer_Buffer = malloc((Input_String_Length + 1) * sizeof(char)); // Dynamic Memory Allocation of Buffer.
    if (Integer_Buffer == NULL)
    {
        printf("\033[0;31mError: Memory Allocation Failed.\033[0m\n");
        exit(EXIT_FAILURE);
    }
    unsigned short Integer_Characters = 0;
    if (Input_String[Input_String_Index] == '-' || Input_String[Input_String_Index] == '+') // In the Case of a Relative Integer.
    {
        Integer_Buffer[Integer_Characters++] = Input_String[Input_String_Index++];
        if (!isdigit(Input_String[Input_String_Index])) // i.e. If the '-' or '+' Character is NOT Followed by a Digit.
        {
            free(Integer_Buffer); // Frees the Allocated Memory Before Exiting to Avoid Memory Leak.
            return false;
        }
    }
    while (Input_String_Index < Input_String_Length && !isspace(Input_String[Input_String_Index]))
    // Loops Through the Characters of the Input String Until the End of the Input String is Reached or Until a Whitespace Character is Encountered.
    {
        if (!isdigit(Input_String[Input_String_Index])) // i.e. If a Non-Digit Character is Encountered in the Input String.
        {
            free(Integer_Buffer); // Frees the Allocated Memory Before Exiting to Avoid Memory Leak.
            return false;
        }
        Integer_Buffer[Integer_Characters++] = Input_String[Input_String_Index++]; // Stores the Next Character of the Input String into the Integer_Buffer String.
    }
    Integer_Buffer[Integer_Characters] = '\0';        // Adds a NULL Terminator at the End of the Integer_Buffer String.
    while (isspace(Input_String[Input_String_Index])) // Moves Through any Trailing Whitespace Characters in the Input String.
        Input_String_Index++;
    if (Input_String[Input_String_Index] != '\0') // i.e. If the There was a Non-Space Character Among the Trailing Whitespace Characters of the Input String.
    {
        free(Integer_Buffer); // Frees the Allocated Memory Before Exiting to Avoid Memory Leak.
        return false;
    }
    *Input_Integer = atoi(Integer_Buffer); // Converts the Integer_Buffer String to an Integer.
    free(Integer_Buffer);                  // Frees the Allocated Memory Before Exiting to Avoid Memory Leak.
    return true;
}

// Function that Takes from the User a Strictly Positive Integer, And Returns the Length of the Password.
const unsigned char Read_Password_Length(void)
{
    int Password_Length;
    bool Parsed_Correctly;
    while (true)
    {
        printf("\033[0;33mEnter the Password Length (%hhu <= Length <= %hhu): \033[0m", MIN_LENGTH, MAX_LENGTH);
        char Buffer[BUFFER_SIZE];
        fgets(Buffer, BUFFER_SIZE, stdin);
        unsigned short Buffer_String_Length = strlen(Buffer);
        if (Buffer[Buffer_String_Length - 1] == '\n')
            Buffer[Buffer_String_Length - 1] = '\0';
        else
            Clear_Input_Buffer();
        Parsed_Correctly = Parse_Integer(Buffer, &Password_Length);
        if (!Parsed_Correctly)
        {
            printf("\n\033[0;31mYour Input was NOT a Valid Integer, Please Try Again.\033[0m\n");
            continue; // Re-Iterate.
        }
        if (Password_Length < MIN_LENGTH || Password_Length > MAX_LENGTH)
        {
            printf("\n\033[0;31mYour Integer was NOT in the Specified Range, Please Try Again.\033[0m\n");
            continue; // Re-Iterate.
        }
        break; // When the User Finally Enters a Valid Integer as Requested.
    }
    return (unsigned char)Password_Length;
}

// Function that Asks the User About the Inclusion of Each Character Type, and Modifies the Boolean Array Accordingly.
void Check_Character_Types_Inclusion(const char Character_Types[4][20], bool is_Character_Type_Included[4])
{
    unsigned char Type;
    int Choice;
    while (true)
    {
        bool Parsed_Correctly;
        for (Type = 0; Type < 4; Type++) // i.e. for Each Character Type.
        {
            while (true)
            {
                printf("\n\033[0;36mIf You Would Like to Include %s in Your Password, Type 1. Otherwise, Type 0: \033[0m", Character_Types[Type]);
                char Buffer[BUFFER_SIZE];
                fgets(Buffer, BUFFER_SIZE, stdin);
                unsigned short Buffer_String_Length = strlen(Buffer);
                if (Buffer[Buffer_String_Length - 1] == '\n')
                    Buffer[Buffer_String_Length - 1] = '\0';
                else
                    Clear_Input_Buffer();
                Parsed_Correctly = Parse_Integer(Buffer, &Choice);
                if (!Parsed_Correctly || (Choice != 0 && Choice != 1))
                {
                    printf("\n\033[0;31mYour Input was Neither 0 Nor 1, Please Try Again.\033[0m");
                    continue; // Re-Iterate.
                }
                break; // When the User Finally Enters Either 0 or 1.
            }
            is_Character_Type_Included[Type] = (Choice == 1);
        }
        if (!is_Character_Type_Included[0] && !is_Character_Type_Included[1] && !is_Character_Type_Included[2] && !is_Character_Type_Included[3])
        {
            printf("\n\033[0;31mYou Must Include At Least ONE Character Type, Please Try Again.\033[0m\n\n");
            continue; // Re-Iterate.
        }
        break; // When the User Finally Includes At Least One Character Type.
    }
}

// Function that Generate a Random Password Based on User-Specified Criteria.
void Generate_Password(const unsigned char Password_Length, char Password[Password_Length + 1], const bool is_Character_Type_Included[4])
{
    const char Special_Characters[] = "%!@#$^&*(){}[]\'\"\\?.,;:`~<>+=-|/";
    srand(time(NULL)); // Random Seed.
    unsigned char Character_Index, Character_Type;
    for (Character_Index = 0; Character_Index < Password_Length; Character_Index++)
    {
        do // As Long As the Randomly Generated Character Type is NOT Included by the User.
        {
            Character_Type = rand() % 4;
        } while (!is_Character_Type_Included[Character_Type]);
        switch (Character_Type)
        {
        case 0: // Uppercase Letters.
        {
            Password[Character_Index] = rand() % NUMBER_OF_ENGLISH_LETTERS + 'A';
            break;
        }
        case 1: // Lowercase Letters.
        {
            Password[Character_Index] = rand() % NUMBER_OF_ENGLISH_LETTERS + 'a';
            break;
        }
        case 2: // Digits.
        {
            Password[Character_Index] = rand() % NUMBER_OF_DECIMAL_DIGITS + '0';
            break;
        }
        case 3: // Special Characters.
        {
            Password[Character_Index] = Special_Characters[rand() % strlen(Special_Characters)];
            break;
        }
        default:
            break;
        }
    }
    Password[Password_Length] = '\0'; // Adds a NULL Terminator to the End of the String.
}

// Main Function.
int main(void)
{
    printf("\n");
    const unsigned char Password_Length = Read_Password_Length(); // Colored in Yellow.
    char Character_Types[4][20] = {"Uppercase Letters", "Lowercase Letters", "Digits", "Special Characters"};
    bool is_Character_Type_Included[4];
    Check_Character_Types_Inclusion(Character_Types, is_Character_Type_Included); // Colored in Cyan.
    char Password[Password_Length + 1];                                           // +1 for the NULL Terminator.
    Generate_Password(Password_Length, Password, is_Character_Type_Included);
    printf("\n\033[0;35mRandomly Generated Password:\033[0;32m %s\n", Password); // Colored in Purple and Green.
    printf("\033[0m\n");
    return 0;
}
